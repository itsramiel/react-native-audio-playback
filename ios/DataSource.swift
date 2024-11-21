//
//  DataSource.swift
//  PlayAudio
//
//  Created by Rami Elwan on 13/11/2024.
//

import Foundation

class DataSource {
  init(sampleCount: Int, data: [Float32], sampleRate: Double, channelCount: Int) {
    self.sampleCount = sampleCount
    self.data = data
    self.sampleRate = sampleRate
    self.channelCount = channelCount
  }

  let sampleCount: Int
  var frameCount: Int { sampleCount / channelCount }
  let data: [Float32]
  let sampleRate: Double
  let channelCount: Int
}
