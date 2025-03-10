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

std::vector<std::string> jniStringArrayToStringVector(JNIEnv* env, jobjectArray stringArray) {
    std::vector<std::string> cppVector{};
    jsize length = env->GetArrayLength(stringArray);

    for(jsize i = 0; i < length; ++i) {
        auto javaString = (jstring)env->GetObjectArrayElement(stringArray, i);
        const char* chars = env->GetStringUTFChars(javaString, nullptr);
        cppVector.emplace_back(chars);
        env->ReleaseStringUTFChars(javaString, chars);
        env->DeleteLocalRef(javaString);
    }

    return cppVector;
}

extern "C" {
JNIEXPORT jobject JNICALL
Java_com_audioplayback_AudioPlaybackModule_setupAudioStreamNative(
        JNIEnv *env,
        jobject,
        jdouble sample_rate,
        jdouble channel_count,
        jint usage) {
    auto result = audioEngine->setupAudioStream(sample_rate, channel_count, usage);

    jclass structClass = env->FindClass("com/audioplayback/models/SetupAudioStreamResult");
    jmethodID constructor = env->GetMethodID(structClass, "<init>", "(Ljava/lang/String;)V");

    jstring jError = result.error.has_value() ? env->NewStringUTF(result.error->c_str()): nullptr;
    jobject returnValue = env->NewObject(structClass, constructor, jError);

    if(jError) {
        env->DeleteLocalRef(jError);
    }

    return returnValue;
}

JNIEXPORT jobject JNICALL
Java_com_audioplayback_AudioPlaybackModule_openAudioStreamNative(JNIEnv *env, jobject) {
    auto result = audioEngine->openAudioStream();

    jclass structClass = env->FindClass("com/audioplayback/models/OpenAudioStreamResult");
    jmethodID constructor = env->GetMethodID(structClass, "<init>", "(Ljava/lang/String;)V");

    jstring jError = result.error.has_value() ? env->NewStringUTF(result.error->c_str()): nullptr;
    jobject returnValue = env->NewObject(structClass, constructor, jError);

    if(jError) {
        env->DeleteLocalRef(jError);
    }

    return returnValue;
}

JNIEXPORT jobject JNICALL
Java_com_audioplayback_AudioPlaybackModule_pauseAudioStreamNative(JNIEnv *env, jobject ) {
    auto result = audioEngine->pauseAudioStream();

    jclass structClass = env->FindClass("com/audioplayback/models/PauseAudioStreamResult");
    jmethodID constructor = env->GetMethodID(structClass, "<init>", "(Ljava/lang/String;)V");

    jstring jError = result.error.has_value() ? env->NewStringUTF(result.error->c_str()): nullptr;
    jobject returnValue = env->NewObject(structClass, constructor, jError);

    if(jError) {
        env->DeleteLocalRef(jError);
    }

    return returnValue;
}


JNIEXPORT jobject JNICALL
Java_com_audioplayback_AudioPlaybackModule_closeAudioStreamNative(JNIEnv *env, jobject ) {
    auto result = audioEngine->closeAudioStream();

    jclass structClass = env->FindClass("com/audioplayback/models/CloseAudioStreamResult");
    jmethodID constructor = env->GetMethodID(structClass, "<init>", "(Ljava/lang/String;)V");

    jstring jError = result.error.has_value() ? env->NewStringUTF(result.error->c_str()): nullptr;
    jobject returnValue = env->NewObject(structClass, constructor, jError);

    if(jError) {
        env->DeleteLocalRef(jError);
    }

    return returnValue;
}

JNIEXPORT void JNICALL
Java_com_audioplayback_AudioPlaybackModule_unloadSoundsNative(JNIEnv *env, jobject ,
                                                              jobjectArray ids) {
    if(ids == nullptr) {
        audioEngine->unloadSounds(std::nullopt);
    } else {
        audioEngine->unloadSounds(jniStringArrayToStringVector(env, ids));
    }
}


JNIEXPORT jobject JNICALL
Java_com_audioplayback_AudioPlaybackModule_loadSoundNative(JNIEnv *env, jobject , jint fd, jint fileLength, jint fileOffset) {
   auto result = audioEngine->loadSound(fd, fileOffset, fileLength);

   // Once done, close the file descriptor
   if (close(fd) == -1) {
       LOGE("Error closing file descriptor: %s", strerror(errno));
   } else {
       LOGD("File descriptor closed");
   }

    jclass structClass = env->FindClass("com/audioplayback/models/LoadSoundResult");
    jmethodID constructor = env->GetMethodID(structClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");

    jstring jError = result.error.has_value() ? env->NewStringUTF(result.error->c_str()): nullptr;
    jstring jId = result.id.has_value() ? env->NewStringUTF(result.id->c_str()): nullptr;
    jobject returnValue = env->NewObject(structClass, constructor, jError, jId);

    if(jError) {
        env->DeleteLocalRef(jError);
        env->DeleteLocalRef(jId);
    }

    return returnValue;
}


JNIEXPORT void JNICALL
Java_com_audioplayback_AudioPlaybackModule_playSoundsNative(JNIEnv *env, jobject , jobjectArray ids,
                                          jbooleanArray values) {
    audioEngine->playSounds(zipStringBooleanArrays(env, ids, values));
}

JNIEXPORT void JNICALL
Java_com_audioplayback_AudioPlaybackModule_loopSoundsNative(JNIEnv *env, jobject , jobjectArray ids,
                                          jbooleanArray values) {
    audioEngine->loopSounds(zipStringBooleanArrays(env, ids, values));
}

JNIEXPORT void JNICALL
Java_com_audioplayback_AudioPlaybackModule_seekSoundsToNative(JNIEnv *env, jobject , jobjectArray ids,
                                            jdoubleArray values) {
    audioEngine->seekSoundsTo(zipStringDoubleArrays(env, ids, values));
}

JNIEXPORT void JNICALL
Java_com_audioplayback_AudioPlaybackModule_setSoundsVolumeNative(JNIEnv *env, jobject ,
                                                                 jobjectArray ids,
                                                                 jdoubleArray values) {
    audioEngine->setSoundsVolume(zipStringDoubleArrays(env, ids, values));
}
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_audioplayback_AudioPlaybackModule_getStreamStateNative(JNIEnv *, jobject ) {
    return static_cast<int>(audioEngine->getStreamState());
}
