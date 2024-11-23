package com.audioplayback

import android.net.Uri
import android.util.Log
import com.facebook.react.bridge.Promise
import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.bridge.ReactMethod
import com.facebook.react.bridge.ReadableArray
import com.facebook.react.bridge.ReadableType
import com.audioplayback.models.FileDescriptorProps
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.net.URL


class AudioPlaybackModule internal constructor(context: ReactApplicationContext) :
  AudioPlaybackSpec(context) {

  override fun getName(): String {
    return NAME
  }

  override fun setupAudioStream(sampleRate: Double, channelCount: Double) {
    setupAudioStreamNative(sampleRate, channelCount)
  }

  override fun closeAudioStream() {
    closeAudioStreamNative()
  }

  override fun openAudioStream() {
    openAudioStreamNative()
  }

  override fun loopSounds(arg: ReadableArray) {
    val (ids, values) = readableArrayToStringBooleanArray(arg)
    loopSoundsNative(ids, values)
  }

  override fun playSounds(arg: ReadableArray) {
    val (ids, values) = readableArrayToStringBooleanArray(arg)
    playSoundsNative(ids, values)
  }

  override fun seekSoundsTo(arg: ReadableArray) {
    val (ids, doubles) = readableArrayToStringDoubleArray(arg)
    seekSoundsToNative(ids, doubles)
  }


  override fun unloadSound(id: String) {
    unloadSoundNative(id)
  }

  override fun loadSound(uri: String, promise: Promise) {
    val scheme = Uri.parse(uri).scheme
    if( scheme == null) {
      val fileDescriptorProps = FileDescriptorProps.fromLocalResource(reactApplicationContext, uri)
      return promise.resolve(loadSoundNative(fileDescriptorProps.id, fileDescriptorProps.length, fileDescriptorProps.offset))
    } else {
      CoroutineScope(Dispatchers.Main).launch {
        withContext(Dispatchers.IO) {
          val url = URL(uri)
          val fileDescriptorProps = FileDescriptorProps.getFileDescriptorPropsFromUrl(reactApplicationContext, url)
          if(fileDescriptorProps == null) {
            promise.resolve(null)
            Log.d("Could not get file descriptor info from uri", LOG)
          } else {
            promise.resolve(loadSoundNative(fileDescriptorProps.id, fileDescriptorProps.length, fileDescriptorProps.offset))
          }
        }
      }
    }
  }

  private fun readableArrayToStringBooleanArray(arg: ReadableArray): Pair<Array<String>, BooleanArray> {
    val size = arg.size()
    // Arrays to hold the results
    val strings = Array(size) { "" }
    val bools = BooleanArray(size)


    // Iterate over the outer array
    for (i in 0 until arg.size()) {
      if (arg.getType(i) === ReadableType.Array) {
        // Get the nested array
        val nestedArray = arg.getArray(i)

        // Extract id (String) and value (Boolean)
        if (nestedArray.size() == 2) {
          val string = nestedArray.getString(0) // First element as String
          val bool = nestedArray.getBoolean(1) // Second element as Boolean

          // Add to the respective arrays
          strings[i] = string
          bools[i] = bool
        }
      }
    }

    return Pair(strings, bools)
  }

  private fun readableArrayToStringDoubleArray(arg: ReadableArray): Pair<Array<String>, DoubleArray> {
    val size = arg.size()
    // Arrays to hold the results
    val strings = Array(size) { "" }
    val bools = DoubleArray(size)


    // Iterate over the outer array
    for (i in 0 until arg.size()) {
      if (arg.getType(i) === ReadableType.Array) {
        // Get the nested array
        val nestedArray = arg.getArray(i)

        // Extract id (String) and value (Boolean)
        if (nestedArray.size() == 2) {
          val string = nestedArray.getString(0) // First element as String
          val bool = nestedArray.getDouble(1) // Second element as Boolean

          // Add to the respective arrays
          strings[i] = string
          bools[i] = bool
        }
      }
    }

    return Pair(strings, bools)
  }

  override fun invalidate() {
    super.invalidate()
    closeAudioStreamNative()
  }

  private external fun setupAudioStreamNative(sampleRate: Double, channelCount: Double)
  private external fun openAudioStreamNative()
  private external fun closeAudioStreamNative()
  private external fun playSoundsNative(ids: Array<String>, values: BooleanArray)
  private external fun loopSoundsNative(ids: Array<String>, values: BooleanArray)
  private external fun seekSoundsToNative(ids: Array<String>, values: DoubleArray)
  private external fun loadSoundNative(fd: Int, fileLength: Int, fileOffset: Int): String?
  private external fun unloadSoundNative(playerId: String): Unit

  // Example method
  // See https://reactnative.dev/docs/native-modules-android

  companion object {
    init {
      System.loadLibrary("native-lib")
    }
    const val NAME = "AudioPlayback"
    const val LOG = "AudioPlaybackModule"
  }
}
