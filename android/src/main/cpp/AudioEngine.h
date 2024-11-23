//
// Created by Rami Elwan on 28.10.24.
//

#ifndef AUDIOPLAYBACK_AUDIOENGINE_H
#define AUDIOPLAYBACK_AUDIOENGINE_H

#include <map>
#include <string>
#include <optional>

#include <oboe/Oboe.h>
#include "audio/Player.h"
#include <android/asset_manager.h>

class AudioEngine : public oboe::AudioStreamDataCallback{
public:
    void setupAudioStream(double sampleRate, double channelCount);
    void openAudioStream();
    void closeAudioStream();
    std::optional<std::string> loadSound(int fd, int offset, int length);
    void unloadSound(const std::string &playerId);
    void playSounds(const std::vector<std::pair<std::string, bool>>&);
    void loopSounds(const std::vector<std::pair<std::string, bool>>&);
    void seekSoundsTo(const std::vector<std::pair<std::string, double>>&);

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;

private:
    std::shared_ptr<oboe::AudioStream> mAudioStream;
    std::map<std::string, std::unique_ptr<Player>> mPlayers;
    int32_t mDesiredSampleRate{};
    int mDesiredChannelCount{};
};

#endif //AUDIOPLAYBACK_AUDIOENGINE_H
