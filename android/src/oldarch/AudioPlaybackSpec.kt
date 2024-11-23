package com.audioplayback

import com.facebook.proguard.annotations.DoNotStrip
import com.facebook.react.bridge.Promise
import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.bridge.ReactContextBaseJavaModule
import com.facebook.react.bridge.ReactMethod
import com.facebook.react.bridge.ReadableArray


abstract class AudioPlaybackSpec internal constructor(context: ReactApplicationContext) :
  ReactContextBaseJavaModule(context) {

  @ReactMethod
  @DoNotStrip
  abstract fun setupAudioStream(sampleRate: Double, channelCount: Double)

  @ReactMethod
  @DoNotStrip
  abstract fun openAudioStream()

  @ReactMethod
  @DoNotStrip
  abstract fun closeAudioStream()

  @ReactMethod
  @DoNotStrip
  abstract fun loopSounds(arg: ReadableArray)

  @ReactMethod
  @DoNotStrip
  abstract fun playSounds(arg: ReadableArray)

  @ReactMethod
  @DoNotStrip
  abstract fun seekSoundsTo(arg: ReadableArray)

  @ReactMethod
  @DoNotStrip
  abstract fun unloadSound(id: String)

  @ReactMethod
  @DoNotStrip
  abstract fun loadSound(uri: String, promise: Promise)
}
