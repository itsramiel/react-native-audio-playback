//
// Created by Rami Elwan on 28.10.24.
//

#ifndef AUDIOPLAYBACK_LOGGING_H
#define AUDIOPLAYBACK_LOGGING_H

#include <android/log.h>
#include <vector>

#define LIB_NAME "react-native-audio-playback"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LIB_NAME, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LIB_NAME, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LIB_NAME, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LIB_NAME, __VA_ARGS__))


#endif //AUDIOPLAYBACK_LOGGING_H
