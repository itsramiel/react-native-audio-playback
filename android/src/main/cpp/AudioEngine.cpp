//
// Created by Rami Elwan on 28.10.24.
//

#include "AudioEngine.h"
#include "utils/logging.h"
#include "utils/uuid.h"

#include "audio/AAssetDataSource.h"

SetupAudioStreamResult AudioEngine::setupAudioStream(
        double sampleRate,
        double channelCount,
        int usage) {
    if(mAudioStream) {
        return { .error =  "Setting up an audio stream while one is already available"};
    }

    mDesiredSampleRate = static_cast<int32_t>(sampleRate);
    mDesiredChannelCount = static_cast<int>(channelCount);

    oboe::AudioStreamBuilder builder {};

    builder.setUsage(getUsageFromInt(usage));
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

PauseAudioStreamResult AudioEngine::pauseAudioStream() {
    if(!mAudioStream) {
        return {.error = "There is no audio stream to pause" };
    }
    auto streamState = mAudioStream->getState();

    if(streamState == oboe::StreamState::Pausing || streamState == oboe::StreamState::Paused) {
        return {.error = "Audio stream was requested to pause but it is already paused"};
    }

    if(streamState != oboe::StreamState::Starting && streamState != oboe::StreamState::Started) {
        return {.error = "Cannot pause a not started stream"};
    }

    oboe::Result result = mAudioStream->requestPause();
    if(result != oboe::Result::OK) {
        auto error = "Failed to pause stream, Error: %s" + std::string(oboe::convertToText(result));
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

void AudioEngine::setSoundsVolume(const std::vector<std::pair<std::string, double>> & pairs) {
    for (const auto& pair: pairs) {
        auto it = mPlayers.find(pair.first);
        if(it != mPlayers.end()) {
            it->second->setVolume(static_cast<float >(pair.second));
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

void AudioEngine::unloadSounds(const std::optional<std::vector<std::string>> &ids)  {
    if(ids.has_value()) {
        for (const auto & id: ids.value()) {
            auto player = mPlayers.find(id);
            if(player != mPlayers.end()) {
                mPlayers.erase(player);
            }
        }
    } else {
        mPlayers.clear();
    }
}

oboe::Usage AudioEngine::getUsageFromInt(int usage) {
    switch(usage) {
        case 0: return oboe::Usage::Media;
        case 1: return oboe::Usage::VoiceCommunication;
        case 2: return oboe::Usage::VoiceCommunicationSignalling;
        case 3: return oboe::Usage::Alarm;
        case 4: return oboe::Usage::Notification;
        case 5: return oboe::Usage::NotificationRingtone;
        case 6: return oboe::Usage::NotificationEvent;
        case 7: return oboe::Usage::AssistanceAccessibility;
        case 8: return oboe::Usage::AssistanceNavigationGuidance;
        case 9: return oboe::Usage::AssistanceSonification;
        case 10: return oboe::Usage::Game;
        case 11: return oboe::Usage::Assistant;
        default: return oboe::Usage::Media;
    }
}
