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

#include <sys/types.h>

#include <cstring>
#include <sstream>

#include <media/NdkMediaExtractor.h>
#include <utils/logging.h>
#include <cinttypes>

#include "NDKExtractor.h"

DecodeFileDescriptorResult NDKExtractor::decodeFileDescriptor(int fd, int offset, int length, AudioProperties targetProperties) {
    auto extractor = AMediaExtractor_new();
    auto amResult = AMediaExtractor_setDataSourceFd(
            extractor,
            fd,
            static_cast<off64_t>(offset),
            static_cast<off64_t>(length));

    if (amResult != AMEDIA_OK){
        return {.error = "Decoding sound file failed"};
    }

    // Specify our desired output format by creating it from our source
    auto format = AMediaExtractor_getTrackFormat(extractor, 0);

    int32_t sampleRate;
    if (AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_SAMPLE_RATE, &sampleRate)){
        if (sampleRate != targetProperties.sampleRate){
            std::stringstream error;
            error
                << "Resampling audio files is not supported."
                << "The sample rate of the audio file, "
                << sampleRate
                << ", doesn't match the sample rate of the stream, "
                << targetProperties.sampleRate << ".";

            return {.error = error.str()};
        }
    } else {
        return {.error = "Failed to load sound file: could not determine sample rate"};
    };

    int32_t channelCount;
    if (AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_CHANNEL_COUNT, &channelCount)){
        if (channelCount != targetProperties.channelCount){
            std::stringstream error;
            error
                    << "The channel count of the audio file, "
                    << channelCount
                    << ", doesn't match the channel count of the stream, "
                    << targetProperties.channelCount << ".";

            return {.error = error.str()};
        }
    } else {
        return {.error = "Failed to load sound file: could not determine channel count"};
    }

    const char *mimeType;
    if (!AMediaFormat_getString(format, AMEDIAFORMAT_KEY_MIME, &mimeType)) {
        return {.error = "Failed to load sound file: could not determine mimeType"};
    }

    // Obtain the correct decoder
    AMediaExtractor_selectTrack(extractor, 0);
    auto codec = AMediaCodec_createDecoderByType(mimeType);
    AMediaCodec_configure(codec, format, nullptr, nullptr, 0);
    AMediaCodec_start(codec);

    std::vector<uint8_t> data{};
    // DECODE

    bool isExtracting = true;
    bool isDecoding = true;

    while(isExtracting || isDecoding){

        if (isExtracting){

            auto inputIndex = AMediaCodec_dequeueInputBuffer(codec, 2000);

            if(inputIndex >= 0) {

                // Obtain the actual buffer and read the encoded data into it
                size_t inputSize {};
                auto inputBuffer = AMediaCodec_getInputBuffer(codec, inputIndex, &inputSize);

                if(inputBuffer) {
                    auto sampleSize = AMediaExtractor_readSampleData(extractor, inputBuffer, inputSize);
                    auto presentationTimeUs = AMediaExtractor_getSampleTime(extractor);

                    if (sampleSize > 0){
                        AMediaCodec_queueInputBuffer(codec, inputIndex, 0, sampleSize, presentationTimeUs, 0);
                        AMediaExtractor_advance(extractor);
                    } else {
                        isExtracting = false;
                        AMediaCodec_queueInputBuffer(codec, inputIndex, 0, 0, presentationTimeUs, AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM);
                    }
                }
            }
        }

        if(isDecoding) {
            AMediaCodecBufferInfo bufferInfo{};
            auto outputIndex = AMediaCodec_dequeueOutputBuffer(codec, &bufferInfo, 2000);
            while(outputIndex >= 0) {
                auto outputBuffer = AMediaCodec_getOutputBuffer(codec, outputIndex, nullptr);
                if(outputBuffer) {
                    data.insert(data.end(), outputBuffer, outputBuffer + bufferInfo.size);
                }
                AMediaCodec_releaseOutputBuffer(codec, outputIndex, false);
                outputIndex = AMediaCodec_dequeueOutputBuffer(codec, &bufferInfo, 0);
            }

            if(bufferInfo.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM) {
                isDecoding = false;
            }
        }
    }

    // Clean up
    AMediaFormat_delete(format);
    AMediaCodec_delete(codec);
    AMediaExtractor_delete(extractor);

    return {.data = data};
}

