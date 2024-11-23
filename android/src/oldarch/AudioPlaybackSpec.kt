package com.audioplayback

import com.facebook.proguard.annotations.DoNotStrip
import com.facebook.react.bridge.Promise
import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.bridge.ReactContextBaseJavaModule
import com.facebook.react.bridge.ReactMethod
import com.facebook.react.bridge.ReadableArray


abstract class AudioPlaybackSpec internal constructor(context: ReactApplicationContext) :
  ReactContextBaseJavaModule(context) {

  abstract fun setupAudioStream(sampleRate: Double, channelCount: Double)

  abstract fun openAudioStream()

  abstract fun closeAudioStream()

  abstract fun loopSounds(arg: ReadableArray)

  abstract fun playSounds(arg: ReadableArray)

  abstract fun seekSoundsTo(arg: ReadableArray)

  abstract fun unloadSound(id: String)

  abstract fun loadSound(uri: String, promise: Promise)
}
