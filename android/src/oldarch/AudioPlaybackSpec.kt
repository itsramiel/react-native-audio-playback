package com.audioplayback

import com.facebook.react.bridge.Promise
import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.bridge.ReactContextBaseJavaModule
import com.facebook.react.bridge.ReadableArray
import com.facebook.react.bridge.WritableMap


abstract class AudioPlaybackSpec internal constructor(context: ReactApplicationContext) :
  ReactContextBaseJavaModule(context) {

  abstract fun setupAudioStream(sampleRate: Double, channelCount: Double): WritableMap

  abstract fun openAudioStream(): WritableMap

  abstract fun closeAudioStream(): WritableMap

  abstract fun loopSounds(arg: ReadableArray)

  abstract fun playSounds(arg: ReadableArray)

  abstract fun seekSoundsTo(arg: ReadableArray)

  abstract fun unloadSound(id: String): WritableMap

  abstract fun loadSound(uri: String, promise: Promise)
}
