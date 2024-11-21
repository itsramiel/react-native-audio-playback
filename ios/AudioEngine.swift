//
//  AudioEngine.swift
//  react-native-audio-playback
//
//  Created by Rami Elwan on 18/11/2024.
//

import Foundation

import AVFoundation
import AudioToolbox

class AudioEngine {
  private let desiredSampleRate: Double
  private let desiredChannelCount: Int
  private let desiredSampleSize = MemoryLayout<Float32>.size
  private let desiredCommonFormat: AVAudioCommonFormat = .pcmFormatFloat32
  private let desiredInterleaved: Bool = true
  private var audioDescription: AudioStreamBasicDescription
  private var audioUnit: AudioUnit?
  private var players = [String: Player]()
  private let mixer: Mixer

  init(sampleRate: Double, channelCount: Int) {
    self.desiredSampleRate = sampleRate
    self.desiredChannelCount = channelCount

    self.audioDescription = AudioStreamBasicDescription(
      mSampleRate: Float64(desiredSampleRate),
      mFormatID: kAudioFormatLinearPCM,
      mFormatFlags: kAudioFormatFlagIsFloat,
      mBytesPerPacket: UInt32(desiredSampleSize * desiredChannelCount),
      mFramesPerPacket: 1,
      mBytesPerFrame: UInt32(desiredSampleSize * desiredChannelCount),
      mChannelsPerFrame: UInt32(desiredChannelCount),
      mBitsPerChannel: UInt32(8 * desiredSampleSize),
      mReserved: 0
    )

    self.mixer = Mixer(channelCount: self.desiredChannelCount)

    // configure Audio Session
    let audioSession = AVAudioSession.sharedInstance()
    try? audioSession.setCategory(.playback)
    try? audioSession.setActive(true)


    setupAudioUnit()
  }

  private func setupAudioUnit() {
    var audioComponentDescription = AudioComponentDescription(
      componentType: kAudioUnitType_Output,
      componentSubType: kAudioUnitSubType_RemoteIO,
      componentManufacturer: kAudioUnitManufacturer_Apple,
      componentFlags: 0,
      componentFlagsMask: 0
    )

    guard let audioComponent = AudioComponentFindNext(nil, &audioComponentDescription) else {
      return
    }

    AudioComponentInstanceNew(audioComponent, &audioUnit)

    AudioUnitSetProperty(
      audioUnit!,
      kAudioUnitProperty_StreamFormat,
      kAudioUnitScope_Input,
      0,
      &self.audioDescription,
      UInt32(MemoryLayout<AudioStreamBasicDescription>.size)
    )

    var renderCallbackStruct = AURenderCallbackStruct(
      inputProc: renderCallback,
      inputProcRefCon: UnsafeMutableRawPointer(Unmanaged.passUnretained(self).toOpaque())
    )

    AudioUnitSetProperty(
      audioUnit!,
      kAudioUnitProperty_SetRenderCallback,
      kAudioUnitScope_Global,
      0,
      &renderCallbackStruct,
      UInt32(MemoryLayout<AURenderCallbackStruct>.size)
    )
    print("Callback set")


    AudioUnitInitialize(audioUnit!)
  }

  // Render callback function
  private let renderCallback: AURenderCallback = { (
    inRefCon,
    ioActionFlags,
    inTimeStamp,
    inBusNumber,
    inNumberFrames,
    ioData) -> OSStatus in

    let audioPlayer = Unmanaged<AudioEngine>.fromOpaque(inRefCon).takeUnretainedValue()
    guard let outputBuffer = ioData?.pointee.mBuffers.mData?.assumingMemoryBound(to: Float32.self) else {
      print("Audio buffer is nil")
      return noErr
    }

    // zero out the output buffer before feeding to mixer
    outputBuffer.update(repeating: 0, count: Int(inNumberFrames) * audioPlayer.desiredChannelCount)

    audioPlayer.mixer.renderAudio(audioData: outputBuffer, numFrames: inNumberFrames)

    return noErr
  }

  @objc public func openAudioStream() {
    guard let audioUnit else { return }
    AudioOutputUnitStart(audioUnit)
  }

  @objc public func loadAudioWith(localFileUrl: URL) -> String?  {
    do {
      // Initialize AVAudioFile
      let audioFile = try AVAudioFile(forReading: localFileUrl)

      // Get the file's processing format
      let fileFormat = audioFile.processingFormat
      let fileSampleRate = fileFormat.sampleRate
      let fileChannels = fileFormat.channelCount
      let fileCommonFormat = fileFormat.commonFormat
      let fileInterleaved = fileFormat.isInterleaved

      // Check if sample rate, channel count, and format match the desired format
      let needsSampleRateConversion = fileSampleRate != desiredSampleRate
      let needsChannelConversion = fileChannels != desiredChannelCount
      let needsFormatConversion = fileCommonFormat != desiredCommonFormat

      // Handle only interleaving changes; other conversions are not supported in this simplified approach
      guard !needsSampleRateConversion,
            !needsChannelConversion,
            !needsFormatConversion else {
        print("Unsupported conversion requirements. Only interleaving change is handled.")
        return nil
      }

      // Prepare to read the audio data
      let frameCount = AVAudioFrameCount(audioFile.length)
      guard let pcmBuffer = AVAudioPCMBuffer(pcmFormat: fileFormat, frameCapacity: frameCount) else {
        print("Failed to create PCM Buffer with file's format")
        return nil
      }

      // Read the entire audio file into the buffer
      try audioFile.read(into: pcmBuffer)
      print("Read \(pcmBuffer.frameLength) frames in file's native format")

      // Access the float32 audio data
      guard let channelData = pcmBuffer.floatChannelData else {
        print("Failed to access channel data")
        return nil
      }

      let channels = Int(fileChannels)
      let frames = Int(pcmBuffer.frameLength)
      let totalSamples = frames * channels

      // Allocate the Float32 buffer
      var audioBuffer: [Float32]

      if fileInterleaved {
        // If the file is already interleaved, copy the data directly
        let data = channelData[0]
        audioBuffer = Array(UnsafeBufferPointer(start: data, count: totalSamples))
      } else {
        audioBuffer = Array(repeating: 0, count: totalSamples)
        // Manual interleaving: Iterate through each frame and channel to interleave samples
        for frame in 0..<frames {
          for channel in 0..<channels {
            audioBuffer[frame * channels + channel] = channelData[channel][frame]
          }
        }
      }

      let uuid = UUID()


      let player = Player(source: DataSource(sampleCount: Int(audioFile.length * 2), data: audioBuffer, sampleRate: desiredSampleRate, channelCount: desiredChannelCount))
      self.players[uuid.uuidString] = player
      self.mixer.addPlayer(player)
      return uuid.uuidString
    } catch {
      print("Error loading audio file: \(error.localizedDescription)")
      return nil
    }
  }

  @objc public func unloadSound(id: String) {
    if let player = players.removeValue(forKey: id) {
      mixer.removePlayer(player)
    }
  }

  public func playSounds(_ args: [(String, Bool)]) {
    for (id, isPlaying) in args {
      guard let player = players[id] else { continue }
      player.setIsPlaying(isPlaying)
    }
  }

  public func loopSounds(_ args: [(String, Bool)]) {
    for (id, isLooping) in args {
      guard let player = players[id] else { continue }
      player.setIsLooping(isLooping)
    }
  }

  public func seekSoundsTo(_ args: [(String, Double)]) {
    for (id, timeInMs) in args {
      guard let player = players[id] else { continue }
      player.seekTo(timeInMs)
    }
  }

  deinit {
    print("Deinit called")
    if let audioUnit {
      AudioOutputUnitStop(audioUnit)
    }
  }
}


