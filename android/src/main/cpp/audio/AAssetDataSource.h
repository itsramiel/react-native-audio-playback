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

#ifndef AUDIOPLAYBACK_AASSETDATASOURCE_H
#define AUDIOPLAYBACK_AASSETDATASOURCE_H

#include <optional>
#include <android/asset_manager.h>
#include <AudioConstants.h>
#include "DataSource.h"

class AAssetDataSource;

struct NewFromCompressedAssetResult {
    AAssetDataSource *dataSource;
    std::optional<std::string> error;
};

class AAssetDataSource : public DataSource {

public:
    [[nodiscard]] int64_t getSize() const override { return mBufferSize; }
    [[nodiscard]] AudioProperties getProperties() const override { return mProperties; }
    [[nodiscard]] const float* getData() const override { return mBuffer.get(); }

    static NewFromCompressedAssetResult newFromCompressedAsset(
            int fd, int offset, int length,
            AudioProperties targetProperties);


private:

    AAssetDataSource(std::unique_ptr<float[]> data, size_t size,
                     const AudioProperties properties)
            : mBuffer(std::move(data))
            , mBufferSize(size)
            , mProperties(properties) {
    }

    const std::unique_ptr<float[]> mBuffer;
    const int64_t mBufferSize;
    const AudioProperties mProperties;

};
#endif //AUDIOPLAYBACK_AASSETDATASOURCE_H
