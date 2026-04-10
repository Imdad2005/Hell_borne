#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#ifdef AUDIO_ENABLED

#include <SDL_mixer.h>
#include <map>
#include <string>

enum SoundID {
    SOUND_JUMP,
    SOUND_ATTACK,
    SOUND_HIT,
    SOUND_DEATH,
    SOUND_PICKUP,
    SOUND_DASH
};

enum MusicID {
    MUSIC_MENU,
    MUSIC_LEVEL,
    MUSIC_BOSS
};

class AudioManager {
private:
    std::map<int, Mix_Chunk*> sounds;
    std::map<int, Mix_Music*> music;
    int soundVolume;
    int musicVolume;

public:
    AudioManager() : soundVolume(128), musicVolume(64) {}

    ~AudioManager() {
        shutdown();
    }

    bool init() {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            SDL_Log("SDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError());
            return false;
        }
        return true;
    }

    void shutdown() {
        for (auto& pair : sounds) {
            if (pair.second != nullptr) {
                Mix_FreeChunk(pair.second);
            }
        }
        sounds.clear();

        for (auto& pair : music) {
            if (pair.second != nullptr) {
                Mix_FreeMusic(pair.second);
            }
        }
        music.clear();

        Mix_CloseAudio();
    }

    bool loadSound(int soundId, const std::string& path) {
        Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
        if (chunk == nullptr) {
            SDL_Log("Failed to load sound %s! SDL_mixer Error: %s", path.c_str(), Mix_GetError());
            return false;
        }
        sounds[soundId] = chunk;
        return true;
    }

    bool loadMusic(int musicId, const std::string& path) {
        Mix_Music* mus = Mix_LoadMUS(path.c_str());
        if (mus == nullptr) {
            SDL_Log("Failed to load music %s! SDL_mixer Error: %s", path.c_str(), Mix_GetError());
            return false;
        }
        music[musicId] = mus;
        return true;
    }

    void playSound(int soundId) {
        if (sounds.find(soundId) != sounds.end() && sounds[soundId] != nullptr) {
            Mix_PlayChannel(-1, sounds[soundId], 0);
        }
    }

    void playMusic(int musicId) {
        if (music.find(musicId) != music.end() && music[musicId] != nullptr) {
            Mix_PlayMusic(music[musicId], -1); // Loop indefinitely
        }
    }

    void stopMusic() {
        Mix_HaltMusic();
    }

    void pauseMusic() {
        Mix_PauseMusic();
    }

    void resumeMusic() {
        Mix_ResumeMusic();
    }

    void setSoundVolume(int volume) {
        soundVolume = volume;
        Mix_Volume(-1, soundVolume);
    }

    void setMusicVolume(int volume) {
        musicVolume = volume;
        Mix_VolumeMusic(musicVolume);
    }

    bool isMusicPlaying() {
        return Mix_PlayingMusic() != 0;
    }
};

#endif // AUDIO_ENABLED

#endif // AUDIO_MANAGER_H
