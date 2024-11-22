//
// Created by Rami Elwan on 28.10.24.
//
#include <jni.h>
#include <string>

#include <android/asset_manager_jni.h>

#include "AudioEngine.h"
#include "utils/logging.h"

auto audioEngine = std::make_unique<AudioEngine>();

std::vector<std::pair<std::string, bool>> zipStringBooleanArrays(JNIEnv  *env, jobjectArray stringArray, jbooleanArray boolArray ){
    jsize size = env->GetArrayLength(stringArray);

    std::vector<std::pair<std::string, bool>> zipped{};
    jboolean *jbooleanArray = env->GetBooleanArrayElements(boolArray, nullptr);

    for(jsize i = 0; i < size; i++){
        auto idObject = (jstring)env->GetObjectArrayElement(stringArray, i);
        const char *cString = env->GetStringUTFChars(idObject, nullptr);
        std::string cppString(cString);
        env->ReleaseStringUTFChars(idObject, cString);
        env->DeleteLocalRef(idObject);

        zipped.emplace_back(cppString, jbooleanArray[i]);
    }

    env->ReleaseBooleanArrayElements(boolArray, jbooleanArray, 0);

    return  zipped;
}

std::vector<std::pair<std::string, double>> zipStringDoubleArrays(JNIEnv  *env, jobjectArray stringArray, jdoubleArray doubleArray ){
    jsize size = env->GetArrayLength(stringArray);

    std::vector<std::pair<std::string, double>> zipped{};
    jdouble *jDoubleArray = env->GetDoubleArrayElements(doubleArray, nullptr);

    for(jsize i = 0; i < size; i++){
        auto idObject = (jstring)env->GetObjectArrayElement(stringArray, i);
        const char *cString = env->GetStringUTFChars(idObject, nullptr);
        std::string cppString(cString);
        env->ReleaseStringUTFChars(idObject, cString);
        env->DeleteLocalRef(idObject);

        zipped.emplace_back(cppString, jDoubleArray[i]);
    }

    env->ReleaseDoubleArrayElements(doubleArray, jDoubleArray, 0);

    return  zipped;
}

std::string jstringToStdString(JNIEnv* env, jstring jStr) {
    if (!jStr) {
        return ""; // Return an empty std::string if jStr is null
    }

    // Convert jstring to a C-style string
    const char* chars = env->GetStringUTFChars(jStr, nullptr);
    std::string str(chars); // Create std::string from C-style string
    env->ReleaseStringUTFChars(jStr, chars); // Release the JNI string memory

    return str;
}

extern "C" {
JNIEXPORT void JNICALL
Java_com_audioplayback_AudioPlaybackModule_setupAudioStreamNative(JNIEnv *env, jobject thiz, jdouble sample_rate, jdouble channel_count) {
    audioEngine->setupAudioStream(sample_rate, channel_count);
}

JNIEXPORT void JNICALL
Java_com_audioplayback_AudioPlaybackModule_openAudioStreamNative(JNIEnv *env, jobject thiz) {
    audioEngine->openAudioStream();
}

JNIEXPORT void JNICALL
Java_com_audioplayback_AudioPlaybackModule_closeAudioStreamNative(JNIEnv *env, jobject thiz) {
    audioEngine->closeAudioStream();
}

JNIEXPORT void JNICALL
Java_com_audioplayback_AudioPlaybackModule_unloadSoundNative(JNIEnv *env, jobject instance, jstring playerId) {
    audioEngine->unloadSound(jstringToStdString(env, playerId));
}


JNIEXPORT jstring JNICALL
Java_com_audioplayback_AudioPlaybackModule_loadSoundNative(JNIEnv *env, jobject instance, jint fd, jint fileLength, jint fileOffset) {
   auto id = audioEngine->loadSound(fd, fileOffset, fileLength);
   // Once done, close the file descriptor
   if (close(fd) == -1) {
       LOGE("Error closing file descriptor: %s", strerror(errno));
   } else {
       LOGD("File descriptor closed");
   }

   if(id) {
       return env->NewStringUTF(id->c_str());
   } else {
       return nullptr;
   }
}


JNIEXPORT void JNICALL
Java_com_audioplayback_AudioPlaybackModule_playSoundsNative(JNIEnv *env, jobject thiz, jobjectArray ids,
                                          jbooleanArray values) {
    audioEngine->playSounds(zipStringBooleanArrays(env, ids, values));
}

JNIEXPORT void JNICALL
Java_com_audioplayback_AudioPlaybackModule_loopSoundsNative(JNIEnv *env, jobject thiz, jobjectArray ids,
                                          jbooleanArray values) {
    audioEngine->loopSounds(zipStringBooleanArrays(env, ids, values));
}

JNIEXPORT void JNICALL
Java_com_audioplayback_AudioPlaybackModule_seekSoundsToNative(JNIEnv *env, jobject thiz, jobjectArray ids,
                                            jdoubleArray values) {
    audioEngine->seekSoundsTo(zipStringDoubleArrays(env, ids, values));
}
}
