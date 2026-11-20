// This file is part of the Geiss macOS port.
//                           ___ ___ ___ ___ ___
//                          / __| __|_ _/ __/ __|
//                         | (_ | _| | |\__ \__ \
//                          \___|___|___|___/___/
// 3-Clause BSD License
//
// Copyright (c) 1998-2022 Ryan Geiss (@geissomatik)
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import AppKit
import CoreAudio
import os

enum CaptureStatus: Equatable {
    case starting
    case running
    case failed(OSStatus)

    var isFailed: Bool {
        if case .failed = self {
            return true
        }
        return false
    }
}

/// Stereo mix of the default output. The IO callback only copies samples into a preallocated ring.
final class SystemAudioTap {
    var onStatus: ((CaptureStatus) -> Void)?

    /// A running tap that delivers no callbacks for this long is restarted. A restart that
    /// cannot create the tap again, for example after permission is revoked, reports a failure.
    private static let stallSeconds: UInt64 = 3

    private let queue = DispatchQueue(label: "com.janrothen.geiss.tap")
    private let queueKey = DispatchSpecificKey<UInt8>()
    private let log = Logger(subsystem: "com.janrothen.geiss", category: "tap")
    private let ring = SampleRing(capacityFrames: 2048)

    private var tapID: AudioObjectID = 0
    private var aggregateID: AudioObjectID = 0
    private var procID: AudioDeviceIOProcID?
    private var status: CaptureStatus = .starting
    private var listener: AudioObjectPropertyListenerBlock?
    private var listenerAddress = AudioObjectPropertyAddress()
    private var observerTokens: [NSObjectProtocol] = []
    private var watchdog: DispatchSourceTimer?
    private var watchedSequence: UInt64 = 0
    private var stalledSince: UInt64?
    private var activated = false

    init() {
        queue.setSpecific(key: queueKey, value: 1)
    }

    func activate() {
        queue.async {
            guard !self.activated else { return }
            self.activated = true
            self.installListeners()
            self.startWatchdog()
            self.startLocked()
        }
    }

    func stop() {
        if DispatchQueue.getSpecific(key: queueKey) != nil {
            stopLocked()
        } else {
            queue.sync { stopLocked() }
        }
    }

    func restart() {
        queue.async {
            self.stopLocked()
            self.startLocked()
        }
    }

    /// Copies the latest stereo frames into `buffer`. Call from one consumer thread.
    /// `buffer` is sized once and then reused, so a steady caller does not allocate.
    func latest(into buffer: inout [Float]) -> AudioSnapshot? {
        ring.snapshot(into: &buffer)
    }

    private func installListeners() {
        listenerAddress = AudioObjectPropertyAddress(
            mSelector: kAudioHardwarePropertyDefaultOutputDevice,
            mScope: kAudioObjectPropertyScopeGlobal,
            mElement: kAudioObjectPropertyElementMain
        )
        let block: AudioObjectPropertyListenerBlock = { [weak self] _, _ in
            self?.restart()
        }
        let status = AudioObjectAddPropertyListenerBlock(
            AudioObjectID(kAudioObjectSystemObject),
            &listenerAddress,
            queue,
            block
        )
        if status == noErr {
            listener = block
        } else {
            log.error("default output listener failed \(status) \(Self.fourCC(status), privacy: .public)")
        }

        let workspace = NSWorkspace.shared.notificationCenter
        observerTokens.append(workspace.addObserver(forName: NSWorkspace.willSleepNotification, object: nil, queue: nil) { [weak self] _ in
            self?.stop()
        })
        observerTokens.append(workspace.addObserver(forName: NSWorkspace.didWakeNotification, object: nil, queue: nil) { [weak self] _ in
            self?.restart()
        })
        observerTokens.append(NotificationCenter.default.addObserver(
            forName: NSApplication.willTerminateNotification,
            object: nil,
            queue: nil
        ) { [weak self] _ in
            self?.stop()
        })
        observerTokens.append(NotificationCenter.default.addObserver(
            forName: NSApplication.didBecomeActiveNotification,
            object: nil,
            queue: nil
        ) { [weak self] _ in
            self?.retryIfNeeded()
        })
    }

    private func startWatchdog() {
        let timer = DispatchSource.makeTimerSource(queue: queue)
        timer.schedule(deadline: .now() + 1, repeating: 1)
        timer.setEventHandler { [weak self] in
            self?.checkStall()
        }
        timer.resume()
        watchdog = timer
    }

    private func checkStall() {
        guard procID != nil, status == .running else {
            stalledSince = nil
            return
        }
        let sequence = ring.sequence()
        if sequence != watchedSequence {
            watchedSequence = sequence
            stalledSince = nil
            return
        }
        let now = DispatchTime.now().uptimeNanoseconds
        guard let since = stalledSince else {
            stalledSince = now
            return
        }
        if now - since >= Self.stallSeconds * NSEC_PER_SEC {
            log.error("no samples for \(Self.stallSeconds) s, restarting the tap")
            stopLocked()
            startLocked()
        }
    }

    private func retryIfNeeded() {
        queue.async {
            if self.procID == nil {
                self.startLocked()
            }
        }
    }

    private func startLocked() {
        if procID != nil {
            return
        }
        stalledSince = nil
        // A retry after a failure keeps the overlay up until the tap is running again.
        if !status.isFailed {
            publish(.starting)
        }
        guard let uid = Self.defaultOutputUID() else {
            fail(kAudioHardwareBadDeviceError, "no default output device")
            return
        }

        let description = CATapDescription(excludingProcesses: [], deviceUID: uid, stream: 0)
        description.name = "Geiss"
        description.muteBehavior = .unmuted
        description.isPrivate = true

        var newTap: AudioObjectID = 0
        var accepted = description
        var status = AudioHardwareCreateProcessTap(description, &newTap)
        if status != noErr {
            log.error("device tap failed \(status) \(Self.fourCC(status), privacy: .public)")
            let global = CATapDescription(stereoGlobalTapButExcludeProcesses: [])
            global.name = "Geiss"
            global.muteBehavior = .unmuted
            global.isPrivate = true
            status = AudioHardwareCreateProcessTap(global, &newTap)
            if status != noErr {
                fail(status, "process tap")
                return
            }
            accepted = global
        }
        tapID = newTap

        status = Self.createAggregate(tapUID: accepted.uuid.uuidString, into: &aggregateID)
        if status != noErr {
            fail(status, "aggregate device")
            return
        }

        var stream = AudioStreamBasicDescription()
        status = Self.inputFormat(of: aggregateID, into: &stream)
        if status != noErr {
            fail(status, "tap format")
            return
        }
        guard let layout = SampleRing.Layout(stream) else {
            log.error("unsupported tap format id \(stream.mFormatID) flags \(stream.mFormatFlags) bits \(stream.mBitsPerChannel)")
            fail(kAudioDeviceUnsupportedFormatError, "tap format")
            return
        }
        // The IO proc does not exist yet, so the ring can take the layout without racing it.
        ring.configure(layout)

        var newProc: AudioDeviceIOProcID?
        status = AudioDeviceCreateIOProcID(aggregateID, geissTapIOProc, Unmanaged.passUnretained(ring).toOpaque(), &newProc)
        if status != noErr || newProc == nil {
            fail(status, "io proc")
            return
        }
        procID = newProc
        status = AudioDeviceStart(aggregateID, newProc)
        if status != noErr {
            fail(status, "device start")
            return
        }
        log.info("capturing \(uid, privacy: .public) at \(stream.mSampleRate, format: .fixed(precision: 0)) Hz")
        publish(.running)
    }

    private func stopLocked() {
        if aggregateID != 0, let procID {
            // Called off the IO thread, so the IO proc has returned when this does.
            AudioDeviceStop(aggregateID, procID)
            AudioDeviceDestroyIOProcID(aggregateID, procID)
        }
        procID = nil
        if aggregateID != 0 {
            AudioHardwareDestroyAggregateDevice(aggregateID)
            aggregateID = 0
        }
        if tapID != 0 {
            AudioHardwareDestroyProcessTap(tapID)
            tapID = 0
        }
        stalledSince = nil
        ring.reset()
    }

    private func fail(_ status: OSStatus, _ where_: String) {
        log.error("\(where_, privacy: .public) failed \(status) \(Self.fourCC(status), privacy: .public)")
        stopLocked()
        publish(.failed(status))
    }

    /// Runs on the tap queue. The main queue delivers statuses in the order they were sent.
    private func publish(_ new: CaptureStatus) {
        status = new
        guard let callback = onStatus else { return }
        DispatchQueue.main.async {
            callback(new)
        }
    }

    private static func createAggregate(tapUID: String, into aggregateID: inout AudioObjectID) -> OSStatus {
        let aggregate: [String: Any] = [
            kAudioAggregateDeviceNameKey: "Geiss Capture",
            kAudioAggregateDeviceUIDKey: UUID().uuidString,
            kAudioAggregateDeviceIsPrivateKey: NSNumber(value: 1),
            kAudioAggregateDeviceIsStackedKey: NSNumber(value: 0),
            kAudioAggregateDeviceTapAutoStartKey: NSNumber(value: 0),
            kAudioAggregateDeviceTapListKey: [
                [kAudioSubTapUIDKey: tapUID]
            ]
        ]
        return AudioHardwareCreateAggregateDevice(aggregate as CFDictionary, &aggregateID)
    }

    private static func inputFormat(of device: AudioObjectID, into stream: inout AudioStreamBasicDescription) -> OSStatus {
        var size = UInt32(MemoryLayout<AudioStreamBasicDescription>.size)
        var address = AudioObjectPropertyAddress(
            mSelector: kAudioDevicePropertyStreamFormat,
            mScope: kAudioDevicePropertyScopeInput,
            mElement: kAudioObjectPropertyElementMain
        )
        return AudioObjectGetPropertyData(device, &address, 0, nil, &size, &stream)
    }

    private static func defaultOutputUID() -> String? {
        var device = AudioDeviceID()
        var size = UInt32(MemoryLayout<AudioDeviceID>.size)
        var address = AudioObjectPropertyAddress(
            mSelector: kAudioHardwarePropertyDefaultOutputDevice,
            mScope: kAudioObjectPropertyScopeGlobal,
            mElement: kAudioObjectPropertyElementMain
        )
        var status = AudioObjectGetPropertyData(
            AudioObjectID(kAudioObjectSystemObject),
            &address,
            0,
            nil,
            &size,
            &device
        )
        guard status == noErr, device != 0 else { return nil }

        var uid: Unmanaged<CFString>?
        size = UInt32(MemoryLayout<Unmanaged<CFString>?>.size)
        address.mSelector = kAudioDevicePropertyDeviceUID
        status = withUnsafeMutablePointer(to: &uid) { pointer in
            AudioObjectGetPropertyData(device, &address, 0, nil, &size, pointer)
        }
        guard status == noErr, let uid else { return nil }
        return uid.takeRetainedValue() as String
    }

    static func statusText(_ status: OSStatus) -> String {
        let code = fourCC(status)
        if code == String(status) {
            return code
        }
        return "\(code) (\(status))"
    }

    fileprivate static func fourCC(_ status: OSStatus) -> String {
        let value = UInt32(bitPattern: status)
        let bytes = [
            UInt8((value >> 24) & 0xff),
            UInt8((value >> 16) & 0xff),
            UInt8((value >> 8) & 0xff),
            UInt8(value & 0xff)
        ]
        if bytes.allSatisfy({ $0 >= 32 && $0 < 127 }) {
            return String(bytes: bytes, encoding: .ascii) ?? "\(status)"
        }
        return String(status)
    }

    deinit {
        // Stop first: the IO proc points at `ring`, which is released with this object.
        stop()
        watchdog?.cancel()
        if let listener {
            AudioObjectRemovePropertyListenerBlock(
                AudioObjectID(kAudioObjectSystemObject),
                &listenerAddress,
                queue,
                listener
            )
        }
        let workspace = NSWorkspace.shared.notificationCenter
        for token in observerTokens {
            workspace.removeObserver(token)
            NotificationCenter.default.removeObserver(token)
        }
    }
}

private func geissTapIOProc(
    _ device: AudioObjectID,
    _ now: UnsafePointer<AudioTimeStamp>,
    _ inputData: UnsafePointer<AudioBufferList>,
    _ inputTime: UnsafePointer<AudioTimeStamp>,
    _ outputData: UnsafeMutablePointer<AudioBufferList>,
    _ outputTime: UnsafePointer<AudioTimeStamp>,
    _ clientData: UnsafeMutableRawPointer?
) -> OSStatus {
    guard let clientData else { return noErr }
    Unmanaged<SampleRing>.fromOpaque(clientData).takeUnretainedValue().write(inputData)
    return noErr
}
