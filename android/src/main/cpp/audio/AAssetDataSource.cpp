/*
 * Copyright (C) 2018 The Android Open Source Project
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


#include <utils/logging.h>
#include <oboe/Oboe.h>

#include "AAssetDataSource.h"

#include "NDKExtractor.h"

NewFromCompressedAssetResult
AAssetDataSource::newFromCompressedAsset(int fd, int offset,
                                         int length, AudioProperties targetProperties) {

    auto decodeResult = NDKExtractor::decodeFileDescriptor(fd, offset, length, targetProperties);
    if(decodeResult.error) {
        return {.dataSource = nullptr, .error = decodeResult.error };
    }

    auto numSamples = decodeResult.data->size() / sizeof(int16_t);

    // Now we know the exact number of samples we can create a float array to hold the audio data
    auto outputBuffer = std::make_unique<float[]>(numSamples);

    // The NDK decoder can only decode to int16, we need to convert to floats
    oboe::convertPcm16ToFloat(
            reinterpret_cast<int16_t*>(decodeResult.data->data()),
            outputBuffer.get(),
            static_cast<int32_t>(numSamples));

    return {
            .dataSource = new AAssetDataSource(std::move(outputBuffer),
                                               numSamples,
                                               targetProperties),
            .error = std::nullopt
    };
}
