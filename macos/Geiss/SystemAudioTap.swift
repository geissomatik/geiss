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
}

/// Stereo mix of the default output. The IO callback only copies samples.
final class SystemAudioTap {
    var onStatus: ((CaptureStatus) -> Void)?

    private let queue = DispatchQueue(label: "com.janrothen.geiss.tap")
    private let queueKey = DispatchSpecificKey<UInt8>()
    private let log = Logger(subsystem: "com.janrothen.geiss", category: "tap")
    private let audio = LatestAudio()

    private var tapID: AudioObjectID = 0
    private var aggregateID: AudioObjectID = 0
    private var procID: AudioDeviceIOProcID?
    private var format = AudioStreamBasicDescription()
    private var deviceUID = ""
    private var listener: AudioObjectPropertyListenerBlock?
    private var listenerAddress = AudioObjectPropertyAddress()
    private var sleepTokens: [NSObjectProtocol] = []
    private var activated = false

    init() {
        queue.setSpecific(key: queueKey, value: 1)
    }

    func activate() {
        queue.async {
            guard !self.activated else { return }
            self.activated = true
            self.installListeners()
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

    func latest() -> (samples: [Float], frames: Int, sampleRate: Double, sequence: UInt64)? {
        audio.snapshot()
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
        listener = block
        AudioObjectAddPropertyListenerBlock(
            AudioObjectID(kAudioObjectSystemObject),
            &listenerAddress,
            queue,
            block
        )

        let center = NSWorkspace.shared.notificationCenter
        sleepTokens.append(center.addObserver(forName: NSWorkspace.willSleepNotification, object: nil, queue: nil) { [weak self] _ in
            self?.stop()
        })
        sleepTokens.append(center.addObserver(forName: NSWorkspace.didWakeNotification, object: nil, queue: nil) { [weak self] _ in
            self?.restart()
        })
        sleepTokens.append(NotificationCenter.default.addObserver(
            forName: NSApplication.willTerminateNotification,
            object: nil,
            queue: nil
        ) { [weak self] _ in
            self?.stop()
        })
        sleepTokens.append(NotificationCenter.default.addObserver(
            forName: NSApplication.didBecomeActiveNotification,
            object: nil,
            queue: nil
        ) { [weak self] _ in
            self?.retryIfNeeded()
        })
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
        publish(.starting)
        guard let uid = Self.defaultOutputUID() else {
            fail(kAudioHardwareBadDeviceError, "no default output device")
            return
        }
        deviceUID = uid

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

        let tapUID = accepted.uuid.uuidString
        let aggregateUID = UUID().uuidString
        let aggregate: [String: Any] = [
            kAudioAggregateDeviceNameKey: "Geiss Capture",
            kAudioAggregateDeviceUIDKey: aggregateUID,
            kAudioAggregateDeviceIsPrivateKey: NSNumber(value: 1),
            kAudioAggregateDeviceIsStackedKey: NSNumber(value: 0),
            kAudioAggregateDeviceTapAutoStartKey: NSNumber(value: 0),
            kAudioAggregateDeviceTapListKey: [
                [kAudioSubTapUIDKey: tapUID]
            ]
        ]
        var newAggregate: AudioObjectID = 0
        status = AudioHardwareCreateAggregateDevice(aggregate as CFDictionary, &newAggregate)
        if status != noErr {
            fail(status, "aggregate device")
            return
        }
        aggregateID = newAggregate

        var stream = AudioStreamBasicDescription()
        var size = UInt32(MemoryLayout<AudioStreamBasicDescription>.size)
        var address = AudioObjectPropertyAddress(
            mSelector: kAudioDevicePropertyStreamFormat,
            mScope: kAudioDevicePropertyScopeInput,
            mElement: kAudioObjectPropertyElementMain
        )
        status = AudioObjectGetPropertyData(aggregateID, &address, 0, nil, &size, &stream)
        if status != noErr || stream.mSampleRate <= 0 {
            fail(status == noErr ? kAudioHardwareBadStreamError : status, "tap format")
            return
        }
        format = stream

        var newProc: AudioDeviceIOProcID?
        status = AudioDeviceCreateIOProcID(aggregateID, geissTapIOProc, Unmanaged.passUnretained(self).toOpaque(), &newProc)
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
        format = AudioStreamBasicDescription()
        audio.reset()
    }

    private func fail(_ status: OSStatus, _ where_: String) {
        log.error("\(where_, privacy: .public) failed \(status) \(Self.fourCC(status), privacy: .public)")
        stopLocked()
        publish(.failed(status))
    }

    private func publish(_ status: CaptureStatus) {
        let callback = onStatus
        Task { @MainActor in
            callback?(status)
        }
    }

    fileprivate func consume(_ input: UnsafePointer<AudioBufferList>) {
        let list = UnsafeMutableAudioBufferListPointer(UnsafeMutablePointer(mutating: input))
        guard !list.isEmpty, format.mSampleRate > 0 else { return }
        let channels = Int(format.mChannelsPerFrame)
        guard channels > 0 else { return }
        let interleaved = Self.interleavedStereo(list: list, format: format)
        guard interleaved.frames > 0 else { return }
        interleaved.samples.withUnsafeBufferPointer { buffer in
            guard let base = buffer.baseAddress else { return }
            audio.append(base, frames: interleaved.frames, rate: format.mSampleRate)
        }
    }

    private static func interleavedStereo(
        list: UnsafeMutableAudioBufferListPointer,
        format: AudioStreamBasicDescription
    ) -> (samples: [Float], frames: Int) {
        let channels = Int(format.mChannelsPerFrame)
        let nonInterleaved = format.mFormatFlags & kAudioFormatFlagIsNonInterleaved != 0
        let isFloat = format.mFormatFlags & kAudioFormatFlagIsFloat != 0
        guard format.mFormatID == kAudioFormatLinearPCM, channels > 0 else {
            return ([], 0)
        }

        if nonInterleaved {
            guard let data = list[0].mData else { return ([], 0) }
            let bytesPerFrame = max(Int(format.mBytesPerFrame), 1)
            let frames = Int(list[0].mDataByteSize) / bytesPerFrame
            guard frames > 0 else { return ([], 0) }
            var output = [Float](repeating: 0, count: frames * 2)
            let left = data.assumingMemoryBound(to: Float.self)
            let right = channels > 1 && list.count > 1 ? list[1].mData?.assumingMemoryBound(to: Float.self) : nil
            if !isFloat {
                return integerNonInterleaved(list: list, format: format, frames: frames)
            }
            for frame in 0..<frames {
                output[frame * 2] = left[frame]
                output[frame * 2 + 1] = right?[frame] ?? left[frame]
            }
            return (output, frames)
        }

        guard let data = list[0].mData, format.mBytesPerFrame > 0 else { return ([], 0) }
        let frames = Int(list[0].mDataByteSize) / Int(format.mBytesPerFrame)
        guard frames > 0 else { return ([], 0) }
        if isFloat && format.mBitsPerChannel == 32 && channels >= 2 {
            let source = data.assumingMemoryBound(to: Float.self)
            if channels == 2 {
                return (Array(UnsafeBufferPointer(start: source, count: frames * 2)), frames)
            }
            var output = [Float](repeating: 0, count: frames * 2)
            for frame in 0..<frames {
                output[frame * 2] = source[frame * channels]
                output[frame * 2 + 1] = source[frame * channels + 1]
            }
            return (output, frames)
        }
        if isFloat && channels == 1 {
            let source = data.assumingMemoryBound(to: Float.self)
            var output = [Float](repeating: 0, count: frames * 2)
            for frame in 0..<frames {
                output[frame * 2] = source[frame]
                output[frame * 2 + 1] = source[frame]
            }
            return (output, frames)
        }
        return integerInterleaved(data: data, format: format, frames: frames, channels: channels)
    }

    private static func integerNonInterleaved(
        list: UnsafeMutableAudioBufferListPointer,
        format: AudioStreamBasicDescription,
        frames: Int
    ) -> (samples: [Float], frames: Int) {
        guard format.mBitsPerChannel == 16, let leftData = list[0].mData else { return ([], 0) }
        let left = leftData.assumingMemoryBound(to: Int16.self)
        let right = list.count > 1 ? list[1].mData?.assumingMemoryBound(to: Int16.self) : nil
        var output = [Float](repeating: 0, count: frames * 2)
        for frame in 0..<frames {
            output[frame * 2] = Float(left[frame]) / 32768
            output[frame * 2 + 1] = Float(right?[frame] ?? left[frame]) / 32768
        }
        return (output, frames)
    }

    private static func integerInterleaved(
        data: UnsafeMutableRawPointer,
        format: AudioStreamBasicDescription,
        frames: Int,
        channels: Int
    ) -> (samples: [Float], frames: Int) {
        guard format.mBitsPerChannel == 16 else { return ([], 0) }
        let source = data.assumingMemoryBound(to: Int16.self)
        var output = [Float](repeating: 0, count: frames * 2)
        for frame in 0..<frames {
            let left = source[frame * channels]
            let right = channels > 1 ? source[frame * channels + 1] : left
            output[frame * 2] = Float(left) / 32768
            output[frame * 2 + 1] = Float(right) / 32768
        }
        return (output, frames)
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
        if let listener {
            AudioObjectRemovePropertyListenerBlock(
                AudioObjectID(kAudioObjectSystemObject),
                &listenerAddress,
                queue,
                listener
            )
        }
        let center = NSWorkspace.shared.notificationCenter
        let tokens = sleepTokens
        for token in tokens {
            center.removeObserver(token)
            NotificationCenter.default.removeObserver(token)
        }
        stop()
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
    let tap = Unmanaged<SystemAudioTap>.fromOpaque(clientData).takeUnretainedValue()
    tap.consume(inputData)
    return noErr
}

private final class LatestAudio: @unchecked Sendable {
    private let lock = NSLock()
    private let capacityFrames = 2048
    private var ring: [Float] = []
    private var writeFrame = 0
    private var storedFrames = 0
    private var sampleRate = 48_000.0
    private var sequence: UInt64 = 0

    func append(_ samples: UnsafePointer<Float>, frames: Int, rate: Double) {
        guard frames > 0 else { return }
        lock.lock()
        defer { lock.unlock() }
        if ring.count != capacityFrames * 2 {
            ring = [Float](repeating: 0, count: capacityFrames * 2)
            writeFrame = 0
            storedFrames = 0
        }
        sampleRate = rate
        var remaining = frames
        var source = samples
        while remaining > 0 {
            let space = capacityFrames - writeFrame
            let count = min(space, remaining)
            ring.withUnsafeMutableBufferPointer { destination in
                destination.baseAddress!.advanced(by: writeFrame * 2).update(from: source, count: count * 2)
            }
            source = source.advanced(by: count * 2)
            remaining -= count
            writeFrame = (writeFrame + count) % capacityFrames
            storedFrames = min(capacityFrames, storedFrames + count)
        }
        sequence &+= 1
    }

    func reset() {
        lock.lock()
        defer { lock.unlock() }
        writeFrame = 0
        storedFrames = 0
    }

    func snapshot() -> (samples: [Float], frames: Int, sampleRate: Double, sequence: UInt64)? {
        lock.lock()
        defer { lock.unlock() }
        guard storedFrames > 0 else { return nil }
        var output = [Float](repeating: 0, count: storedFrames * 2)
        let start = (writeFrame - storedFrames + capacityFrames) % capacityFrames
        let first = min(storedFrames, capacityFrames - start)
        output.withUnsafeMutableBufferPointer { destination in
            ring.withUnsafeBufferPointer { source in
                destination.baseAddress!.update(from: source.baseAddress!.advanced(by: start * 2), count: first * 2)
                let rest = storedFrames - first
                if rest > 0 {
                    destination.baseAddress!.advanced(by: first * 2).update(from: source.baseAddress!, count: rest * 2)
                }
            }
        }
        return (output, storedFrames, sampleRate, sequence)
    }
}
