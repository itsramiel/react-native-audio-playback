//
//  AudioEngine.swift
//  react-native-audio-playback
//
//  Created by Rami Elwan on 18/11/2024.
//

import Foundation

import AVFoundation
import AudioToolbox

enum AudioStreamState: Int {
  case closed, initialized, opened, paused
}

struct InterruptionState {
  let wasOpen: Bool
}

enum AudioEngineError: LocalizedError {
  case audioStreamAlreadyInitialized
  case failedToSetupAudioStream(reason: String)
  case failedToOpenAudioStream(reason: String)
  case failedToCloseAudioStream(reason: String)
  case failedToPauseAudioStream(reason: String)
  case failedToUnloadSound
  case failedToLoadAudioFile(reason: String)

  var errorDescription: String? {
    switch self {
    case .audioStreamAlreadyInitialized:
      return "Audio stream is already initialized"
    case .failedToSetupAudioStream(reason: let reason):
      return "Failed to setup audio stream: \(reason)"
    case .failedToOpenAudioStream(reason: let reason):
      return "Failed to open audio stream: \(reason)"
    case .failedToPauseAudioStream(reason: let reason):
      return "Failed to pause audio stream: \(reason)"
    case .failedToCloseAudioStream(reason: let reason):
      return "Failed to close audio stream: \(reason)"
    case .failedToUnloadSound:
      return "Failed to unload sound. No Audio file is loaded"
    case .failedToLoadAudioFile(reason: let reason):
      return "Failed to load audio file: \(reason)"
    }
  }
}

class AudioEngine {
  private var desiredSampleRate: Double = 44100
  private var desiredChannelCount: Int = 2
  private let desiredSampleSize = MemoryLayout<Float32>.size
  private let desiredCommonFormat: AVAudioCommonFormat = .pcmFormatFloat32
  private let desiredInterleaved: Bool = true
  private var audioUnit: AudioUnit?
  private var players = [String: Player]()
  private var audioStreamState: AudioStreamState = .closed
  private var interruptionState: InterruptionState?

  init() {

    NotificationCenter.default.addObserver(
      self,
      selector: #selector(handleAudioSessionInterruption),
      name: AVAudioSession.interruptionNotification,
      object: nil
    )
  }

  @objc private func handleAudioSessionInterruption(notification: Notification) {
    guard let userInfo = notification.userInfo,
          let typeValue = userInfo[AVAudioSessionInterruptionTypeKey] as? UInt,
          let type = AVAudioSession.InterruptionType(rawValue: typeValue) else {
      return
    }

    switch type {
    case .began:
      let isOpen = audioStreamState == .opened
      if isOpen {
        try? pauseAudioStream()
      }
      self.interruptionState = .init(wasOpen: isOpen)
    case .ended:
      guard let optionsValue = userInfo[AVAudioSessionInterruptionOptionKey] as? UInt, let interruptionState else { return }
      let options = AVAudioSession.InterruptionOptions(rawValue: optionsValue)
      if options.contains(.shouldResume)
          && audioStreamState != .closed
          && interruptionState.wasOpen {
        try? openAudioStream()
      }

      self.interruptionState = nil
    @unknown default:
      break
    }
  }

  public func getStreamState() -> AudioStreamState {
    return audioStreamState
  }

  public func setupAudioStream(
    sampleRate: Double,
    channelCount: Int,
    audioSessionCategory: AVAudioSession.Category
  ) throws {
    guard audioStreamState != .initialized else {
      throw AudioEngineError.audioStreamAlreadyInitialized
    }
    
    // configure Audio Session
    let audioSession = AVAudioSession.sharedInstance()
    try? audioSession.setCategory(audioSessionCategory)
    try? audioSession.setActive(true)

    self.desiredSampleRate = sampleRate
    self.desiredChannelCount = channelCount

    try setupAudioUnit()
  }

  private func setupAudioUnit() throws {
    var audioDescription = AudioStreamBasicDescription(
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

    var audioComponentDescription = AudioComponentDescription(
      componentType: kAudioUnitType_Output,
      componentSubType: kAudioUnitSubType_RemoteIO,
      componentManufacturer: kAudioUnitManufacturer_Apple,
      componentFlags: 0,
      componentFlagsMask: 0
    )

    guard let audioComponent = AudioComponentFindNext(nil, &audioComponentDescription) else {
      throw AudioEngineError.failedToSetupAudioStream(reason: "audio component not found")
    }

    AudioComponentInstanceNew(audioComponent, &audioUnit)

    guard let audioUnit else {
      throw AudioEngineError.failedToSetupAudioStream(reason: "could not create audio unit")
    }

    AudioUnitSetProperty(
      audioUnit,
      kAudioUnitProperty_StreamFormat,
      kAudioUnitScope_Input,
      0,
      &audioDescription,
      UInt32(MemoryLayout<AudioStreamBasicDescription>.size)
    )

    var renderCallbackStruct = AURenderCallbackStruct(
      inputProc: renderCallback,
      inputProcRefCon: UnsafeMutableRawPointer(Unmanaged.passUnretained(self).toOpaque())
    )

    AudioUnitSetProperty(
      audioUnit,
      kAudioUnitProperty_SetRenderCallback,
      kAudioUnitScope_Global,
      0,
      &renderCallbackStruct,
      UInt32(MemoryLayout<AURenderCallbackStruct>.size)
    )

    let status = AudioUnitInitialize(audioUnit)

    if(status == noErr) {
      audioStreamState = .initialized
    } else {
      throw AudioEngineError.failedToSetupAudioStream(reason: "initializing the stream failed")
    }
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
      return noErr
    }

    // zero out the output buffer before feeding to mixer
    outputBuffer.update(repeating: 0, count: Int(inNumberFrames) * audioPlayer.desiredChannelCount)

    for (playerId, player) in audioPlayer.players {
      player.renderAudio(audioData: outputBuffer, numFrames: inNumberFrames)
    }

    return noErr
  }

  @objc public func openAudioStream() throws {
    print(audioStreamState)
    guard audioStreamState == .initialized || audioStreamState == .paused else {
      throw AudioEngineError.failedToOpenAudioStream(reason: "Stream is not initialized")
    }

    guard let audioUnit else { return }
    let status = AudioOutputUnitStart(audioUnit)

    if(status == noErr) {
      audioStreamState = .opened
    } else {
      throw AudioEngineError.failedToOpenAudioStream(reason: "Stream could not be open")
    }
  }

  @objc public func loadAudioWith(localFileUrl: URL) throws -> String  {
    let audioFile: AVAudioFile
    do {
      audioFile = try AVAudioFile(forReading: localFileUrl)
    } catch {
      throw AudioEngineError.failedToLoadAudioFile(reason: "Could not create AVAudioFile")
    }

    let fileFormat = audioFile.processingFormat
    let fileSampleRate = fileFormat.sampleRate
    let fileChannels = fileFormat.channelCount
    let fileCommonFormat = fileFormat.commonFormat
    let fileInterleaved = fileFormat.isInterleaved

    guard fileSampleRate == desiredSampleRate else {
      throw AudioEngineError.failedToLoadAudioFile(reason: "Sample rate mismatch. Resampling is not supported. File sample rate: \(fileSampleRate), stream sample rate: \(desiredSampleRate).")
    }

    guard fileChannels == desiredChannelCount else {
      throw AudioEngineError.failedToLoadAudioFile(reason: "Channel count mismatch. File channel count: \(fileChannels), stream channel count: \(desiredChannelCount).")
    }

    guard fileCommonFormat == desiredCommonFormat else {
      throw AudioEngineError.failedToLoadAudioFile(reason: "Format mismatch. File format: \(fileCommonFormat), stream format: \(desiredCommonFormat).")
    }

    // Prepare to read the audio data
    let frameCount = AVAudioFrameCount(audioFile.length)
    guard let pcmBuffer = AVAudioPCMBuffer(pcmFormat: fileFormat, frameCapacity: frameCount) else {
      throw AudioEngineError.failedToLoadAudioFile(reason: "Could not create AVAudioPCMBuffer.")
    }

    do {
      // Read the entire audio file into the buffer
      try audioFile.read(into: pcmBuffer)
    } catch {
      throw AudioEngineError.failedToLoadAudioFile(reason: "Could not read audio file into buffer: \(error.localizedDescription).")
    }

    // Access the float32 audio data
    guard let channelData = pcmBuffer.floatChannelData else {
      throw AudioEngineError.failedToLoadAudioFile(reason: "Could not access audio data.")
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
    return uuid.uuidString
  }

  @objc public func unloadSound(id: String) throws {
    guard players.removeValue(forKey: id) != nil else {
      throw AudioEngineError.failedToUnloadSound
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

  public func setSoundsVolume(_ args: [(String, Double)]) {
    for (id, volume) in args {
      guard let player = players[id] else { continue }
      player.setVolume(Float(volume))
    }
  }

  public func pauseAudioStream() throws {
    guard let audioUnit else {
      throw AudioEngineError.failedToPauseAudioStream(reason: "No stream to pause found.")
    }

    guard audioStreamState != .paused else {
      throw AudioEngineError.failedToPauseAudioStream(reason: "Stream is already paused.")
    }

    guard audioStreamState == .opened else {
      throw AudioEngineError.failedToPauseAudioStream(reason: "Cannot pause a non started stream.")
    }

    let status = AudioOutputUnitStop(audioUnit)
    if(status != noErr) {
      throw AudioEngineError.failedToCloseAudioStream(reason: "Failed to pause audio stream.")
    }

    self.audioStreamState = .paused
  }


  public func closeAudioStream() throws {
    guard let audioUnit else {
      throw AudioEngineError.failedToCloseAudioStream(reason: "No stream to close found.")
    }

    guard audioStreamState != .closed else {
      throw AudioEngineError.failedToCloseAudioStream(reason: "Stream is already closed.")
    }

    var status = AudioOutputUnitStop(audioUnit)
    if(status != noErr) {
      throw AudioEngineError.failedToCloseAudioStream(reason: "Failed to stop audio stream.")
    }

    status = AudioUnitUninitialize(audioUnit)
    if(status != noErr) {
      throw AudioEngineError.failedToCloseAudioStream(reason: "Failed to uninitialize audio stream.")
    }

    self.audioStreamState = .closed
    self.audioUnit = nil
  }

  deinit {
    try? closeAudioStream()
  }
}


