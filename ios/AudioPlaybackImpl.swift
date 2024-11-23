import AVFoundation
import AudioToolbox

@objc public class AudioPlaybackImpl: NSObject {
  let audioEngine = AudioEngine()

  @objc public func setupAudioStream(sampleRate: Double, channelCount: Double) {
    audioEngine.setupAudioStream(sampleRate: sampleRate, channelCount: Int(channelCount))
  }

  @objc public func openAudioStream() {
    audioEngine.openAudioStream()
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

  @objc public func loadSound(uri: String, completion: @escaping (String?) -> Void) {
    let isLocalFile = uri.hasPrefix("file://")
    let url = URL(string: uri)

    guard let url else {
      completion(nil)
      return
    }

    if isLocalFile {
      completion(self.audioEngine.loadAudioWith(localFileUrl: url))
    } else {
      loadRemoteSound(url: url) { [weak self] localUrl in
        guard let self, let localUrl else {
          completion(nil)
          return
        }

        completion(self.audioEngine.loadAudioWith(localFileUrl: localUrl))
      }
    }
  }

  @objc public func unloadSound(id: String) {
    audioEngine.unloadSound(id: id)
  }

  @objc public func closeAudioStream() {
    audioEngine.closeAudioStream()
  }

  private func loadRemoteSound(url: URL, _ completion: @escaping (URL?) -> Void) {
    URLSession.shared.downloadTask(with: url) { localUrl, response, error in
      if let error {
        print("Downloading local file failed: \(error)")
        completion(nil)
        return
      }

      if let localUrl {
        completion(localUrl)
      } else {
        print("Downloaded file is nil with no error")
        completion(nil)
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
}

