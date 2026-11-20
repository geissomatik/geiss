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

import CoreAudio
import os

struct AudioSnapshot {
    let frames: Int
    let sampleRate: Double
    let sequence: UInt64
}

/// Latest stereo frames from the tap. Storage is allocated once, and all state lives behind
/// raw pointers, so the IO thread never allocates, never takes a Swift exclusivity check on
/// the ring, and never waits: when the reader holds the lock, that block is dropped.
final class SampleRing: @unchecked Sendable {
    /// The tap format the IO proc converts from. Only 32-bit float PCM is accepted.
    struct Layout {
        var channels = 0
        var nonInterleaved = false
        var bytesPerFrame = 0
        var sampleRate = 0.0

        init() {}

        init?(_ format: AudioStreamBasicDescription) {
            let isFloat = format.mFormatFlags & kAudioFormatFlagIsFloat != 0
            guard format.mFormatID == kAudioFormatLinearPCM,
                  isFloat,
                  format.mBitsPerChannel == 32,
                  format.mChannelsPerFrame > 0,
                  format.mBytesPerFrame > 0,
                  format.mSampleRate > 0
            else {
                return nil
            }
            channels = Int(format.mChannelsPerFrame)
            nonInterleaved = format.mFormatFlags & kAudioFormatFlagIsNonInterleaved != 0
            bytesPerFrame = Int(format.mBytesPerFrame)
            sampleRate = format.mSampleRate
        }
    }

    private struct State {
        var writeFrame = 0
        var storedFrames = 0
        var sequence: UInt64 = 0
        var layout = Layout()
    }

    private let capacityFrames: Int
    private let samples: UnsafeMutablePointer<Float>
    private let state: UnsafeMutablePointer<State>
    private let lock: UnsafeMutablePointer<os_unfair_lock>

    init(capacityFrames: Int) {
        self.capacityFrames = capacityFrames
        samples = .allocate(capacity: capacityFrames * 2)
        samples.initialize(repeating: 0, count: capacityFrames * 2)
        state = .allocate(capacity: 1)
        state.initialize(to: State())
        lock = .allocate(capacity: 1)
        lock.initialize(to: os_unfair_lock())
    }

    deinit {
        samples.deallocate()
        state.deinitialize(count: 1)
        state.deallocate()
        lock.deinitialize(count: 1)
        lock.deallocate()
    }

    /// Tap queue, before the IO proc is created.
    func configure(_ layout: Layout) {
        os_unfair_lock_lock(lock)
        state.pointee.layout = layout
        state.pointee.writeFrame = 0
        state.pointee.storedFrames = 0
        os_unfair_lock_unlock(lock)
    }

    /// Tap queue, after the IO proc is destroyed.
    func reset() {
        os_unfair_lock_lock(lock)
        state.pointee.layout = Layout()
        state.pointee.writeFrame = 0
        state.pointee.storedFrames = 0
        os_unfair_lock_unlock(lock)
    }

    func sequence() -> UInt64 {
        os_unfair_lock_lock(lock)
        defer { os_unfair_lock_unlock(lock) }
        return state.pointee.sequence
    }

    /// IO thread. Converts to interleaved stereo straight into the ring.
    func write(_ input: UnsafePointer<AudioBufferList>) {
        guard os_unfair_lock_trylock(lock) else { return }
        defer { os_unfair_lock_unlock(lock) }

        let layout = state.pointee.layout
        guard layout.channels > 0, layout.bytesPerFrame > 0 else { return }
        let buffers = UnsafeMutableAudioBufferListPointer(UnsafeMutablePointer(mutating: input))
        guard !buffers.isEmpty, let first = buffers[0].mData else { return }
        let frames = Int(buffers[0].mDataByteSize) / layout.bytesPerFrame
        guard frames > 0 else { return }

        let left = first.assumingMemoryBound(to: Float.self)
        let right: UnsafePointer<Float>
        let stride: Int
        if layout.nonInterleaved {
            stride = 1
            if layout.channels > 1, buffers.count > 1, let second = buffers[1].mData {
                right = UnsafePointer(second.assumingMemoryBound(to: Float.self))
            } else {
                right = UnsafePointer(left)
            }
        } else {
            stride = layout.channels
            right = UnsafePointer(layout.channels > 1 ? left + 1 : left)
        }

        // Keep only the newest frames when one block is larger than the ring.
        let start = max(0, frames - capacityFrames)
        var writeFrame = state.pointee.writeFrame
        for frame in start..<frames {
            samples[writeFrame * 2] = left[frame * stride]
            samples[writeFrame * 2 + 1] = right[frame * stride]
            writeFrame += 1
            if writeFrame == capacityFrames {
                writeFrame = 0
            }
        }
        state.pointee.writeFrame = writeFrame
        state.pointee.storedFrames = min(capacityFrames, state.pointee.storedFrames + frames - start)
        state.pointee.sequence &+= 1
    }

    /// Consumer thread. Holds the lock only for the copy.
    func snapshot(into buffer: inout [Float]) -> AudioSnapshot? {
        if buffer.count != capacityFrames * 2 {
            buffer = [Float](repeating: 0, count: capacityFrames * 2)
        }
        return buffer.withUnsafeMutableBufferPointer { destination -> AudioSnapshot? in
            guard let output = destination.baseAddress else { return nil }
            os_unfair_lock_lock(lock)
            defer { os_unfair_lock_unlock(lock) }
            let stored = state.pointee.storedFrames
            let sampleRate = state.pointee.layout.sampleRate
            guard stored > 0, sampleRate > 0 else { return nil }
            let start = (state.pointee.writeFrame - stored + capacityFrames) % capacityFrames
            let first = min(stored, capacityFrames - start)
            output.update(from: samples + start * 2, count: first * 2)
            let rest = stored - first
            if rest > 0 {
                (output + first * 2).update(from: samples, count: rest * 2)
            }
            return AudioSnapshot(frames: stored, sampleRate: sampleRate, sequence: state.pointee.sequence)
        }
    }
}
