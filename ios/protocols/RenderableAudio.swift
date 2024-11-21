//
//  RenderableAudio.swift
//  PlayAudio
//
//  Created by Rami Elwan on 15/11/2024.
//

protocol RenderableAudio {
  func renderAudio(audioData: UnsafeMutablePointer<Float32>, numFrames: UInt32)
}

