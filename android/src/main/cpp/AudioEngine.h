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
#include "AudioConstants.h"
#include <android/asset_manager.h>

class AudioEngine : public oboe::AudioStreamDataCallback{
public:
    SetupAudioStreamResult setupAudioStream(double sampleRate, double channelCount);
    OpenAudioStreamResult openAudioStream();
    PauseAudioStreamResult pauseAudioStream();
    CloseAudioStreamResult closeAudioStream();
    void playSounds(const std::vector<std::pair<std::string, bool>>&);
    void loopSounds(const std::vector<std::pair<std::string, bool>>&);
    void seekSoundsTo(const std::vector<std::pair<std::string, double>>&);
    void setSoundsVolume(const std::vector<std::pair<std::string, double>>&);
    LoadSoundResult loadSound(int fd, int offset, int length);
    void unloadSounds(const std::optional<std::vector<std::string>>&);

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;

private:
    std::shared_ptr<oboe::AudioStream> mAudioStream;
    std::map<std::string, std::unique_ptr<Player>> mPlayers;
    int32_t mDesiredSampleRate{};
    int mDesiredChannelCount{};
};

#endif //AUDIOPLAYBACK_AUDIOENGINE_H
