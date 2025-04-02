#ifndef SRC_AUDIOHANDLER_H_
#define SRC_AUDIOHANDLER_H_

#pragma once

#include <iostream>
#include <vector>
#include <map>

#include <SDL3/SDL.h>
#include <SDL3/SDL_mixer.h>

#include "Util.h"

/*
ABOUT:
Controls the music and sound effects.
*/

class AudioHandler {
    public:
        //////// CONSTRUCTOR
        AudioHandler();

        //////// DELOADING
        void ClearAll();

        //////// LOADING
        void LoadSound(std::string name, std::string path);
        void LoadMusic(std::string name, std::string path);

        //////// PLAYING
        void PlaySound(std::string name, int n, Uint8 volume = 80);
        void PlayMusic(std::string name, int n, Uint8 volume = 80);

        //////// CHANNEL CONTROL
        void ToggleMute();

        void SetSoundVolume(Uint8 volume);
        void SetMusicVolume(Uint8 volume);

        //////// GETTERS
        std::string GetMusicState();

        //////// MEMORY MANAGEMENT
        void FreeAll();

    private:
        //////// BASICS
        bool muted = false;
        SDL_AudioSpec spec;

        //////// MAPS
        std::map<std::string, Mix_Chunk*> sound_bank;
        std::map<std::string, Mix_Music*> music_bank;

};

#endif /* SRC_AUDIOHANDLER_H_ */