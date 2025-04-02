#include "AudioHandler.h"

//=============================
// DEFAULT CONSTRUCTOR 
//=============================

AudioHandler::AudioHandler() {
    spec.channels = 2;
    spec.format = SDL_AUDIO_F32LE;
    spec.freq = 44100;
    Mix_OpenAudio(0, &spec);
}

//=============================
// DELOADING
//=============================

void AudioHandler::ClearAll() {
    music_bank.clear();
    sound_bank.clear();
}

//=============================
// LOADING
//=============================

void AudioHandler::LoadSound(std::string name, std::string path) {
    sound_bank[name] = Mix_LoadWAV(path.c_str());
}

void AudioHandler::LoadMusic(std::string name, std::string path) {
    music_bank[name] = Mix_LoadMUS(path.c_str());
}

//=============================
// PLAYING
//=============================

void AudioHandler::PlaySound(std::string name, int n, Uint8 volume) {
    if (!sound_bank.count(name) == 0) {
        Util::ThrowError("SOUND: " + name + " DOES NOT EXIST", "AudioHandler::PlaySound");
        return;
    }
    if (muted) return;

    Mix_PlayChannel(-1, sound_bank[name], n);
    Mix_Volume(-1, volume);
}

void AudioHandler::PlayMusic(std::string name, int n, Uint8 volume) {
    if (!music_bank.count(name) == 0) {
        Util::ThrowError("MUSIC: " + name + " DOES NOT EXIST", "AudioHandler::PlayMusic");
        return;
    }
    if (muted) return;

    Mix_PlayMusic(music_bank[name], n);
    Mix_VolumeMusic(volume);
}

//=============================
// CHANNEL CONTROL
//=============================

void AudioHandler::ToggleMute() {
    muted = !muted;
    if (Mix_PlayingMusic() && muted) Mix_PauseMusic();
    if (Mix_PausedMusic() && !muted) Mix_ResumeMusic();
}

void AudioHandler::SetSoundVolume(Uint8 volume) {
    Mix_Volume(-1, volume);
}

void AudioHandler::SetMusicVolume(Uint8 volume) {
    Mix_VolumeMusic(volume);
}

//=============================
// GETTERS
//=============================

std::string AudioHandler::GetMusicState() {
    if (Mix_PlayingMusic()) return "PLAYING";
    if (Mix_PausedMusic()) return "PAUSED";
    if (muted) return "MUTED";
    return "NONE";
}

//=============================
// MEMORY MANAGEMENT
//=============================

void AudioHandler::FreeAll() {
    for (auto const& [key, val] : sound_bank) {
        Mix_FreeChunk(val);
    }
    for (auto const& [key, val] : music_bank) {
        Mix_FreeMusic(val);
    }
    Mix_CloseAudio();
}