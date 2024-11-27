//
// Created by Rami Elwan on 31.10.24.
//

#ifndef AUDIOPLAYBACK_AUDIOCONSTANTS_H
#define AUDIOPLAYBACK_AUDIOCONSTANTS_H

#include <cstdint>
#include <optional>
#include <string>

struct AudioProperties {
    int32_t channelCount;
    int32_t sampleRate;
};

struct SetupAudioStreamResult {
    std::optional<std::string> error;
};

struct OpenAudioStreamResult {
    std::optional<std::string> error;
};

struct CloseAudioStreamResult {
    std::optional<std::string> error;
};

struct LoadSoundResult {
    std::optional<std::string> id;
    std::optional<std::string> error;
};

struct UnloadSoundResult {
    std::optional<std::string> error;
};

#endif //AUDIOPLAYBACK_AUDIOCONSTANTS_H
