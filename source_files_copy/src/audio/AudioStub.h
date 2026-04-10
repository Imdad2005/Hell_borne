#ifndef AUDIO_STUB_H
#define AUDIO_STUB_H

// No-op audio implementation for builds without SDL2_mixer
// Used when AUDIO_ENABLED is not defined

#ifndef AUDIO_ENABLED

class AudioManager {
public:
    AudioManager() {}
    ~AudioManager() {}

    bool init() { return true; }
    void shutdown() {}

    void playSound(int soundId) {}
    void playMusic(int musicId) {}
    void stopMusic() {}
    void pauseMusic() {}
    void resumeMusic() {}

    void setSoundVolume(int volume) {}
    void setMusicVolume(int volume) {}

    bool isMusicPlaying() { return false; }
};

#endif // !AUDIO_ENABLED

#endif // AUDIO_STUB_H
