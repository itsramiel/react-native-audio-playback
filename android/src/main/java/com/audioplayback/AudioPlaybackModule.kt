package com.audioplayback

import android.net.Uri
import com.audioplayback.models.CloseAudioStreamResult
import com.facebook.react.bridge.Promise
import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.bridge.ReactMethod
import com.facebook.react.bridge.ReadableArray
import com.facebook.react.bridge.ReadableType
import com.audioplayback.models.FileDescriptorProps
import com.audioplayback.models.LoadSoundResult
import com.audioplayback.models.OpenAudioStreamResult
import com.audioplayback.models.PauseAudioStreamResult
import com.audioplayback.models.SetupAudioStreamResult
import com.facebook.react.bridge.Arguments
import com.facebook.react.bridge.ReadableMap
import com.facebook.react.bridge.WritableMap
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

  @ReactMethod(isBlockingSynchronousMethod = true)
  override fun setupAudioStream(options: ReadableMap): WritableMap {
    val sampleRate = options.getDouble("sampleRate")
    val channelCount = options.getDouble("channelCount")
    val usage = options.getMap("android")!!.getInt("usage")

    val result = setupAudioStreamNative(sampleRate, channelCount, usage)
    val map = Arguments.createMap()
    result.error?.let { map.putString("error", it) } ?: map.putNull("error")
    return map
  }

  @ReactMethod(isBlockingSynchronousMethod = true)
  override fun openAudioStream(): WritableMap {
    val result = openAudioStreamNative()
    val map = Arguments.createMap()
    result.error?.let { map.putString("error", it) } ?: map.putNull("error")
    return map
  }

  @ReactMethod(isBlockingSynchronousMethod = true)
  override fun pauseAudioStream(): WritableMap {
    val result = pauseAudioStreamNative()
    val map = Arguments.createMap()
    result.error?.let { map.putString("error", it) } ?: map.putNull("error")
    return map
  }


  @ReactMethod(isBlockingSynchronousMethod = true)
  override fun closeAudioStream(): WritableMap {
    val result = closeAudioStreamNative()
    val map = Arguments.createMap()
    result.error?.let { map.putString("error", it) } ?: map.putNull("error")
    return map
  }


  @ReactMethod
  override fun loopSounds(arg: ReadableArray) {
    val (ids, values) = readableArrayToStringBooleanArray(arg)
    loopSoundsNative(ids, values)
  }

  @ReactMethod
  override fun playSounds(arg: ReadableArray) {
    val (ids, values) = readableArrayToStringBooleanArray(arg)
    playSoundsNative(ids, values)
  }

  @ReactMethod
  override fun seekSoundsTo(arg: ReadableArray) {
    val (ids, doubles) = readableArrayToStringDoubleArray(arg)
    seekSoundsToNative(ids, doubles)
  }

  @ReactMethod
  override fun setSoundsVolume(arg: ReadableArray) {
    val (ids, doubles) = readableArrayToStringDoubleArray(arg)
    setSoundsVolumeNative(ids, doubles)
  }


  @ReactMethod
  override fun unloadSound(id: String) {
    unloadSoundsNative(arrayOf(id))
  }

  @ReactMethod
  override fun loadSound(uri: String, promise: Promise) {
    val map = Arguments.createMap()

    val scheme = Uri.parse(uri).scheme
    if( scheme == null) {
      val fileDescriptorProps = FileDescriptorProps.fromLocalResource(reactApplicationContext, uri)
      val result = loadSoundNative(fileDescriptorProps.id, fileDescriptorProps.length, fileDescriptorProps.offset)
      result.error?.let { map.putString("error", it) } ?: map.putNull("error")
      result.id?.let { map.putString("id", it) } ?: map.putNull("id")
      promise.resolve(map)
    } else {
      CoroutineScope(Dispatchers.Main).launch {
        withContext(Dispatchers.IO) {
          val url = URL(uri)
          val fileDescriptorProps = FileDescriptorProps.getFileDescriptorPropsFromUrl(reactApplicationContext, url)
          if(fileDescriptorProps == null) {
            promise.resolve(null)
            map.putString("error", "Failed to load sound file")
            map.putNull("id")
          } else {
            val result = loadSoundNative(fileDescriptorProps.id, fileDescriptorProps.length, fileDescriptorProps.offset)
            result.error?.let { map.putString("error", it) } ?: map.putNull("error")
            result.id?.let { map.putString("id", it) } ?: map.putNull("id")
            promise.resolve(map)
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
        val nestedArray = arg.getArray(i)!! // Type was checked to be array so it is safe to use !!

        // Extract id (String) and value (Boolean)
        if (nestedArray.size() == 2) {
          val string = nestedArray.getString(0)!! // First element as String
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
        val nestedArray = arg.getArray(i)!! // Type was checked to be array so it is safe to use !!

        // Extract id (String) and value (Boolean)
        if (nestedArray.size() == 2) {
          val string = nestedArray.getString(0)!! // First element as String
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
    unloadSoundsNative(null)
  }

  private external fun setupAudioStreamNative(sampleRate: Double, channelCount: Double, usage: Int): SetupAudioStreamResult
  private external fun openAudioStreamNative(): OpenAudioStreamResult
  private external fun pauseAudioStreamNative(): PauseAudioStreamResult
  private external fun closeAudioStreamNative(): CloseAudioStreamResult
  private external fun playSoundsNative(ids: Array<String>, values: BooleanArray)
  private external fun loopSoundsNative(ids: Array<String>, values: BooleanArray)
  private external fun seekSoundsToNative(ids: Array<String>, values: DoubleArray)
  private external fun setSoundsVolumeNative(ids: Array<String>, values: DoubleArray)
  private external fun loadSoundNative(fd: Int, fileLength: Int, fileOffset: Int): LoadSoundResult
  private external fun unloadSoundsNative(ids: Array<String>?)

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
