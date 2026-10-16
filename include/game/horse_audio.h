#ifndef HORSE_AUDIO_H_
#define HORSE_AUDIO_H_

#include <vector>
#include <glm/glm.hpp>
#include <irrKlang.h>

// Synthesizes the horse's sound bank at startup (no audio assets needed) and plays it positionally.
class HorseAudio {
public:
    enum Sound {
        HOOF_SAND,      // several variations, picked at random
        LANDING,
        WHINNY,
        SNORT,
        BREATH,
        WHISTLE,
        WIND,           // looping
        NUM_SOUNDS
    };

    explicit HorseAudio(irrklang::ISoundEngine* engine);

    // fire and forget positional sound
    void Play(Sound s, const glm::vec3& pos, float volume = 1.0f, float pitch = 1.0f);
    // non positional sound (the player's own whistle)
    void Play2D(Sound s, float volume = 1.0f, float pitch = 1.0f);
    // looping sound the caller controls; must be released with StopLoop
    irrklang::ISound* StartLoop(Sound s, float volume);
    static void StopLoop(irrklang::ISound*& sound);

    void SetListener(const glm::vec3& pos, const glm::vec3& forward, const glm::vec3& up);

private:
    irrklang::ISoundEngine* engine_;
    std::vector<irrklang::ISoundSource*> sources_[NUM_SOUNDS];
    unsigned int rng_ = 12345u;

    irrklang::ISoundSource* Pick(Sound s);
    void Register(Sound s, const char* name, const std::vector<float>& samples);
};

#endif // HORSE_AUDIO_H_
