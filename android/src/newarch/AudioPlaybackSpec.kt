package com.audioplayback

import com.facebook.react.bridge.ReactApplicationContext

abstract class AudioPlaybackSpec internal constructor(context: ReactApplicationContext) :
  NativeAudioPlaybackSpec(context) {
}
