package com.audioplayback.models

data class SetupAudioStreamResult(val error: String?)
data class OpenAudioStreamResult(val error: String?)
data class PauseAudioStreamResult(val error: String?)
data class CloseAudioStreamResult(val error: String?)
data class LoadSoundResult(val error: String?, val id: String?)
