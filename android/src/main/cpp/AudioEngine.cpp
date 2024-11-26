//
// Created by Rami Elwan on 28.10.24.
//

#include "AudioEngine.h"
#include "utils/logging.h"
#include "utils/uuid.h"

#include "audio/AAssetDataSource.h"

SetupAudioStreamResult AudioEngine::setupAudioStream(double sampleRate, double channelCount) {
    if(mAudioStream) {
        return { .error =  "Setting up an audio stream while one is already available"};
    }

    mDesiredSampleRate = static_cast<int32_t>(sampleRate);
    mDesiredChannelCount = static_cast<int>(channelCount);

    oboe::AudioStreamBuilder builder {};

    builder.setFormat(oboe::AudioFormat::Float);
    builder.setFormatConversionAllowed(true);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setSharingMode(oboe::SharingMode::Exclusive);
    builder.setSampleRate(mDesiredSampleRate);
    builder.setSampleRateConversionQuality(
            oboe::SampleRateConversionQuality::Medium
    );
    builder.setChannelCount(mDesiredChannelCount);
    builder.setDataCallback(this);
    oboe::Result result = builder.openStream(mAudioStream);

    if( result != oboe::Result::OK) {
        auto error = "Failed to open stream:" + std::string (convertToText(result));
        return { .error = error};
    } else {
        return {.error = std::nullopt};
    }
}


OpenAudioStreamResult AudioEngine::openAudioStream() {
    if(!mAudioStream) {
        return {.error = "There is no audio stream to start" };
    }
    auto streamState = mAudioStream->getState();

    if(streamState == oboe::StreamState::Starting || streamState == oboe::StreamState::Started) {
        return {.error = "Audio stream was requested to start but it is already started"};
    }

    oboe::Result result = mAudioStream->requestStart();
    if(result != oboe::Result::OK) {
       auto error = "Failed to start stream, Error: %s" + std::string(oboe::convertToText(result));
       return {.error = error};
    } else {
        return {.error = std::nullopt};
    }
}

CloseAudioStreamResult AudioEngine::closeAudioStream() {
    if(!mAudioStream) {
        return { .error = "There is no audio stream to close" };
    }
    auto streamState = mAudioStream->getState();

    if(streamState == oboe::StreamState::Closing || streamState == oboe::StreamState::Closed) {
        return { .error = "Audio stream was requested to close but it is already closed" };
    }

    oboe::Result result = mAudioStream->close();
    if(result != oboe::Result::OK) {
        auto error ="Failed to close stream: %s" + std::string(oboe::convertToText(result));
        return {.error = error};
    }
    mAudioStream = nullptr;
    return {.error = std::nullopt};
}

oboe::DataCallbackResult
AudioEngine::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) {
    memset(audioData, 0, sizeof(float) * numFrames * mDesiredChannelCount);

    for (const auto& player: mPlayers) {
        player.second->renderAudio(static_cast<float *>(audioData), numFrames);
    }

    return oboe::DataCallbackResult::Continue;
}

void AudioEngine::playSounds(const std::vector<std::pair<std::string, bool>>& pairs) {
    for (const auto& pair: pairs) {
        auto it = mPlayers.find(pair.first);
        if(it != mPlayers.end()) {
            it->second->setPlaying(pair.second);
        }
    }
}

void AudioEngine::loopSounds(const std::vector<std::pair<std::string, bool>>& pairs) {
    for (const auto& pair: pairs) {
        auto it = mPlayers.find(pair.first);
        if(it != mPlayers.end()) {
            it->second->setLooping(pair.second);
        }
    }
}

void AudioEngine::seekSoundsTo(const std::vector<std::pair<std::string, double>> & pairs) {
    for (const auto& pair: pairs) {
        auto it = mPlayers.find(pair.first);
        if(it != mPlayers.end()) {
            it->second->seekTo(static_cast<int64_t>(pair.second));
        }
    }
}

LoadSoundResult AudioEngine::loadSound(int fd, int offset, int length) {
    auto playersSize = mPlayers.size();

    LOGD("Loading audio with already %d sounds loaded", playersSize);

    AudioProperties targetProperties {
            .channelCount = mDesiredChannelCount,
            .sampleRate = mDesiredSampleRate
    };


    auto compressedAssetResult = AAssetDataSource::newFromCompressedAsset(fd, offset, length, targetProperties);

    if(compressedAssetResult.error) {
        return {.id = std::nullopt, .error = compressedAssetResult.error};
    } else if(compressedAssetResult.dataSource == nullptr) {
        return {.id = std::nullopt, .error = "An unknown error occurred while loading the audio file. Please create an issue with a reproducible"};
    }

    std::string id = uuid::generate_uuid_v4();
    mPlayers[id] = std::make_unique<Player>(compressedAssetResult.dataSource);
    return {.id = id, .error = std::nullopt};
}

UnloadSoundResult AudioEngine::unloadSound(const std::string &playerId) {
    auto it = mPlayers.find(playerId);
    if(it != mPlayers.end()) {
        mPlayers.erase(it);
    } else {
        return {.error = "Audio file could not be unloaded because it is not found"};
    }

    return {.error = std::nullopt};
}

