//
//  Mixer.swift
//  PlayAudio
//
//  Created by Rami Elwan on 15/11/2024.
//

class Mixer: RenderableAudio {
  let channelCount: Int
  var players = [Player]()

  init(channelCount: Int) {
    self.channelCount = channelCount
  }

  func renderAudio(audioData: UnsafeMutablePointer<Float32>, numFrames: UInt32) {
    for player in players {
      player.renderAudio(audioData: audioData, numFrames: numFrames)
    }
  }

  func addPlayer(_ player: Player) {
    players.append(player)
  }

  public func removePlayer(_ player: Player) {
    players.removeAll(where: { $0 === player })
  }
}

