/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AUDIOPLAYBACK_PLAYER_H
#define AUDIOPLAYBACK_PLAYER_H

#include <cstdint>
#include <array>

#include <chrono>
#include <memory>
#include <atomic>
#include <utility>

#include <android/asset_manager.h>

#include "shared/IRenderableAudio.h"
#include "DataSource.h"
#include "utils/logging.h"

class Player : public IRenderableAudio{

public:
    /**
     * Construct a new Player from the given DataSource.
     *
     * @param source
     */
    explicit Player(DataSource *source)
        : mSource(source)
    {};

    void renderAudio(float *targetData, int32_t numFrames) override;
    void setPlaying(bool isPlaying) { LOGE("Setting isPlaying to: %d", isPlaying); mIsPlaying = isPlaying; };
    void setLooping(bool isLooping) { mIsLooping = isLooping; };
    void setVolume(float volume) { mVolume = volume; };
    void seekTo(int64_t timeInMs);

private:
    int32_t mReadFrameIndex = 0;
    float mVolume = 1;
    std::atomic<bool> mIsPlaying { false };
    std::atomic<bool> mIsLooping { false };
    std::unique_ptr<DataSource> mSource;
};

#endif //AUDIOPLAYBACK_PLAYER_H
