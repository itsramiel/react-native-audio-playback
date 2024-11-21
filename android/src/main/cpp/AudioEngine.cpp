//
// Created by Rami Elwan on 28.10.24.
//

#include "AudioEngine.h"
#include "utils/logging.h"
#include "utils/uuid.h"

#include "audio/AAssetDataSource.h"

AudioEngine::AudioEngine(double sampleRate, double channelCount):
mDesiredSampleRate{static_cast<int32_t>(sampleRate)},
mDesiredChannelCount{static_cast<int>(channelCount)}
{
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
        LOGE("Failed to open stream. Error: %s", convertToText(result));
    } else {
        LOGE("Opened stream successfully");
    }

}

void AudioEngine::openAudioStream() {
    oboe::Result result = mAudioStream->requestStart();
    if(result != oboe::Result::OK) {
        LOGE("Failed to start stream, Error: %s", oboe::convertToText(result));
    }
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

std::optional<std::string> AudioEngine::loadSound(int fd, int offset, int length) {
    AudioProperties targetProperties {
            .channelCount = mDesiredChannelCount,
            .sampleRate = mDesiredSampleRate
    };

    std::shared_ptr<AAssetDataSource> mClapSource {
            AAssetDataSource::newFromCompressedAsset(fd, offset, length, targetProperties)
    };

    if(mClapSource == nullptr) {
        LOGE("Could not load source data for clap sound");
        return std::nullopt;
    }

    std::string id = uuid::generate_uuid_v4();
    mPlayers[id] = std::make_unique<Player>(std::move(mClapSource));
    return id;
}

void AudioEngine::unloadSound(const std::string &playerId) {
    auto it = mPlayers.find(playerId);
    if(it != mPlayers.end()) {
        mPlayers.erase(it);
    } else {
        LOGE("Player with identifier: %s not found", playerId.c_str());
    }
}
