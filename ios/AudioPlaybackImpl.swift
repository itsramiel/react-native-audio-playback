import AVFoundation
import AudioToolbox


@objc public class AudioPlaybackImpl: NSObject {
  private static let unknownError: String = "An unknown error occurred while loading the audio file. Please create an issue with a reproducible"
  let audioEngine = AudioEngine()

  @objc public func getAudioStreamState() -> Double {
    return Double(audioEngine.getStreamState().rawValue)
  }

  @objc public func setupAudioStream(
    sampleRate: Double,
    channelCount: Double,
    audioSessionCategory: Double
  ) -> String? {
    guard let audioSessionCategoryEnum = getAudoSessionCategoryEnumMemeberFromRawValue(audioSessionCategory) else {
      return "Invalid audio session category"
    }

    do {
      try audioEngine.setupAudioStream(
        sampleRate: sampleRate,
        channelCount: Int(channelCount),
        audioSessionCategory: audioSessionCategoryEnum
      )
      return nil
    } catch let error as AudioEngineError {
      return error.localizedDescription
    } catch {
      return Self.unknownError
    }
  }

  @objc public func openAudioStream() -> String? {
    do {
      try audioEngine.openAudioStream()
      return nil
    } catch let error as AudioEngineError {
      return error.localizedDescription
    } catch {
      return Self.unknownError
    }
  }

  @objc public func loopSounds(arg: NSArray) {
    audioEngine.loopSounds(convertNSArrayToArrayOfStringBoolTuples(arg))
  }

  @objc public func playSounds(arg: NSArray) {
    audioEngine.playSounds(convertNSArrayToArrayOfStringBoolTuples(arg))
  }

  @objc public func seekSoundsTo(arg: NSArray) {
    audioEngine.seekSoundsTo(convertNSArrayToArrayOfStringDoubleTuples(arg))
  }

  @objc public func setSoundsVolume(arg: NSArray) {
    audioEngine.setSoundsVolume(convertNSArrayToArrayOfStringDoubleTuples(arg))
  }

  @objc public func loadSound(uri: String, completion: @escaping (_ id:String?, _ error: String?) -> Void) {
    let isLocalFile = uri.hasPrefix("file://")
    let url = URL(string: uri)

    guard let url else {
      completion(nil, "Invalid Uri: \(uri)")
      return
    }

    let cb = {(_ localFileUrl: URL) in
      do {
        let id = try self.audioEngine.loadAudioWith(localFileUrl: localFileUrl)
        completion(id, nil)
      } catch let error as AudioEngineError {
        completion(nil, error.localizedDescription)
      } catch {
        completion(nil, Self.unknownError)
      }
    }

    if isLocalFile {
      cb(url)
    } else {
      loadRemoteSound(url: url) {  localUrl, error in
        guard  let localUrl else {
          completion(nil, error)
          return
        }

        cb(localUrl)
      }
    }
  }

  @objc public func unloadSound(id: String) -> String? {
    do {
      try audioEngine.unloadSound(id: id)
      return nil
    } catch let error as AudioEngineError {
      return error.localizedDescription
    } catch {
      return Self.unknownError
    }
  }

  @objc public func pauseAudioStream() -> String? {
    do {
      try audioEngine.pauseAudioStream()
      return nil
    } catch let error as AudioEngineError {
      return error.localizedDescription
    } catch {
      return Self.unknownError
    }
  }

  @objc public func closeAudioStream() -> String? {
    do {
      try audioEngine.closeAudioStream()
      return nil
    } catch let error as AudioEngineError {
      return error.localizedDescription
    } catch {
      return Self.unknownError
    }
  }

  private func loadRemoteSound(url: URL, _ completion: @escaping (_ url: URL?,_ error: String?) -> Void) {
    URLSession.shared.downloadTask(with: url) { localUrl, response, error in
      if let error {
        completion(nil, "Downloading local file failed: \(error.localizedDescription)")
        return
      }

      if let localUrl {
        completion(localUrl, nil)
      } else {
        completion(nil, Self.unknownError)
      }
    }.resume()
  }

  private func convertNSArrayToArrayOfStringBoolTuples(_ array: NSArray) -> [(String, Bool)] {
    var result: [(String, Bool)] = []
    for item in array {
      if let  subArray = item as? NSArray,
         subArray.count == 2,
         let key = subArray[0] as? String,
         let value = subArray[1] as? Bool {
        result.append((key, value))
      }
    }
    return result
  }

  private func convertNSArrayToArrayOfStringDoubleTuples(_ array: NSArray) -> [(String, Double)] {
    var result: [(String, Double)] = []
    for item in array {
      if let  subArray = item as? NSArray,
         subArray.count == 2,
         let key = subArray[0] as? String,
         let value = subArray[1] as? Double {
        result.append((key, value))
      }
    }
    return result
  }

  private func getAudoSessionCategoryEnumMemeberFromRawValue(_ rawValue: Double) -> AVAudioSession.Category? {
    switch Int(rawValue) {
    case 0: return .ambient
    case 1: return .multiRoute
    case 2: return .playAndRecord
    case 3: return .playback
    case 4: return .record
    case 5: return .soloAmbient
    default: return nil
    }
  }
}

