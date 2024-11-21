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

#include "Player.h"
#include "utils/logging.h"

void Player::renderAudio(float *targetData, int32_t numFrames){

    const AudioProperties properties = mSource->getProperties();

    if (mIsPlaying){

        int64_t framesToRenderFromData = numFrames;
        int64_t totalSourceFrames = mSource->getSize() / properties.channelCount;
        const float *data = mSource->getData();

        // Check whether we're about to reach the end of the recording
        if (!mIsLooping && mReadFrameIndex + numFrames >= totalSourceFrames){
            framesToRenderFromData = totalSourceFrames - mReadFrameIndex;
            mIsPlaying = false;
        }

        for (int i = 0; i < framesToRenderFromData; ++i) {
            for (int j = 0; j < properties.channelCount; ++j) {
                targetData[(i*properties.channelCount)+j] += data[(mReadFrameIndex*properties.channelCount)+j];
            }

            // Increment and handle wraparound
            if (++mReadFrameIndex >= totalSourceFrames) mReadFrameIndex = 0;
        }
    }
}

void Player::seekTo(int64_t timeInMs) {
    if(timeInMs == 0) {
        mReadFrameIndex = 0;
        return;
    }
    const AudioProperties audioProperties = mSource->getProperties();

    auto targetFrame = static_cast<int32_t>((static_cast<int32_t>(timeInMs) / 1000.0) * audioProperties.sampleRate);
    auto totalFrames = static_cast<int32_t>(mSource->getSize() / audioProperties.channelCount);

    if(targetFrame >= 0 && targetFrame < totalFrames) {
        mReadFrameIndex = targetFrame;
    } else {
        mReadFrameIndex = std::min(std::max(targetFrame, 0), totalFrames);
    }
}
