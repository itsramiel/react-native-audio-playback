//
//  Player.swift
//  PlayAudio
//
//  Created by Rami Elwan on 13/11/2024.
//

class Player: RenderableAudio {
  init(source: DataSource) {
    self.mSource = source
  }

  private var mIsPlaying: Bool = false
  private var mIsLooping: Bool = false
  private var mSource: DataSource
  private var mReadFrameIndex: Int = 0

  public func setIsPlaying(_ isPlaying: Bool) {
    mIsPlaying = isPlaying
  }

  public func setIsLooping(_ isLooping: Bool) {
    mIsLooping = isLooping
  }

  public func seekTo(_ timeInMs: Double) {
    let seekedToFrame = Int(timeInMs / 1000 * mSource.sampleRate)


    if seekedToFrame >= 0 || seekedToFrame < mSource.frameCount {
      mReadFrameIndex = seekedToFrame
    } else {
      mReadFrameIndex = 0
    }
  }

  public func renderAudio(audioData: UnsafeMutablePointer<Float>, numFrames: UInt32) {
    let inFrames = Int(numFrames)
    if mIsPlaying {
      var framesToRenderFromData = inFrames
      let totalSourceFrames = mSource.sampleCount / mSource.channelCount
      let data = mSource.data

      if(!mIsLooping && mReadFrameIndex + inFrames >= totalSourceFrames) {
        framesToRenderFromData = totalSourceFrames - mReadFrameIndex
        mIsPlaying = false
      }

      for i in 0..<framesToRenderFromData {
        for j in 0..<mSource.channelCount {
          audioData[i * mSource.channelCount + j] += data[Int(mReadFrameIndex) * mSource.channelCount + j]
        }

        mReadFrameIndex += 1
        if mReadFrameIndex >= totalSourceFrames {
          mReadFrameIndex = 0
        }
      }


    } else {
    }
  }
}
