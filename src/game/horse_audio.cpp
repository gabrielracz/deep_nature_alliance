#include "horse_audio.h"

#include <cmath>
#include <string>
#include <glm/gtc/constants.hpp>

using namespace irrklang;

namespace {

const int RATE = 22050;
const float TAU = glm::two_pi<float>();

struct Rand {
    unsigned int s;
    float Next() { // [-1, 1]
        s = s * 1664525u + 1013904223u;
        return ((s >> 8) & 0xFFFF) / 32767.5f - 1.0f;
    }
};

struct OnePole {
    float a;
    float y = 0.0f;
    OnePole(float cutoff) : a(1.0f - std::exp(-TAU * cutoff / RATE)) {}
    float operator()(float x) { y += a * (x - y); return y; }
};

// RBJ band pass, constant 0 dB peak gain
struct BandPass {
    float b0, b2, a1, a2;
    float x1 = 0, x2 = 0, y1 = 0, y2 = 0;
    BandPass(float freq, float q) {
        float w = TAU * freq / RATE;
        float alpha = std::sin(w) / (2.0f * q);
        float a0 = 1.0f + alpha;
        b0 = alpha / a0;
        b2 = -alpha / a0;
        a1 = -2.0f * std::cos(w) / a0;
        a2 = (1.0f - alpha) / a0;
    }
    float operator()(float x) {
        float y = b0 * x + b2 * x2 - a1 * y1 - a2 * y2;
        x2 = x1; x1 = x;
        y2 = y1; y1 = y;
        return y;
    }
};

std::vector<float> Buffer(float seconds) {
    return std::vector<float>((size_t)(seconds * RATE), 0.0f);
}

void Normalize(std::vector<float>& b, float peak) {
    float m = 1e-6f;
    for (float v : b) m = std::fmax(m, std::fabs(v));
    for (float& v : b) v *= peak / m;
}

// a hoof landing in loose sand: a dull thump, a short click and a crunchy spray
std::vector<float> SynthHoof(unsigned int seed) {
    Rand r{seed};
    std::vector<float> b = Buffer(0.2f);
    float f0 = 80.0f + 35.0f * (r.Next() * 0.5f + 0.5f);
    OnePole crunch_lp(1000.0f + 350.0f * r.Next());
    OnePole click_lp(5000.0f);
    float phase = 0.0f;
    float grain = 1.0f;
    for (size_t n = 0; n < b.size(); n++) {
        float t = (float)n / RATE;
        float f = f0 * (1.0f - 0.45f * std::fmin(t / 0.08f, 1.0f));
        phase += TAU * f / RATE;
        float thump = std::sin(phase) * std::exp(-t * 32.0f);
        if (n % 40 == 0) grain = 0.4f + 0.6f * std::fabs(r.Next());
        float noise = r.Next();
        float crunch = crunch_lp(noise) * std::exp(-t * 26.0f) * grain;
        float click = (noise - click_lp(noise)) * std::exp(-t * 160.0f);
        b[n] = 1.1f * thump + 1.3f * crunch + 0.08f * click;
    }
    Normalize(b, 0.8f);
    return b;
}

std::vector<float> SynthLanding() {
    std::vector<float> b = Buffer(0.6f);
    std::vector<float> h1 = SynthHoof(77u);
    std::vector<float> h2 = SynthHoof(91u);
    size_t offset = (size_t)(0.08f * RATE);
    for (size_t n = 0; n < h1.size(); n++) {
        b[n] += h1[n];
        b[n + offset] += h2[n] * 0.9f;
    }
    Rand r{5u};
    OnePole lp(900.0f);
    OnePole sub(140.0f);
    for (size_t n = 0; n < b.size(); n++) {
        float t = (float)n / RATE;
        b[n] += lp(r.Next()) * std::exp(-t * 7.0f) * 1.4f;
        b[n] += sub(r.Next()) * std::exp(-t * 9.0f) * 4.0f;
    }
    Normalize(b, 0.95f);
    return b;
}

// the classic rising then falling, trembling neigh
std::vector<float> SynthWhinny(unsigned int seed, float length, float base) {
    Rand r{seed};
    std::vector<float> b = Buffer(length);
    BandPass formant1(1000.0f, 2.2f);
    BandPass formant2(2500.0f, 3.5f);
    OnePole breath_lp(3200.0f);
    float phase = 0.0f;
    for (size_t n = 0; n < b.size(); n++) {
        float t = (float)n / RATE;
        float u = t / length;
        float f;
        if (t < 0.12f) {
            f = glm::mix(base * 0.55f, base, t / 0.12f);
        } else {
            f = base * std::pow(0.45f, (t - 0.12f) / (length - 0.12f));
        }
        float vib_depth = 0.02f + 0.12f * u;
        f *= 1.0f + vib_depth * std::sin(TAU * (12.5f + 2.0f * u) * t);
        phase += TAU * f / RATE;

        float s = 0.0f;
        for (int h = 1; h <= 10; h++) {
            s += std::sin(phase * h) / std::pow((float)h, 1.7f);
        }
        float voiced = 0.45f * s + 0.9f * formant1(s) + 0.2f * formant2(s);

        float env = std::fmin(t / 0.03f, 1.0f) * std::fmin((length - t) / 0.25f, 1.0f);
        // the stuttering "ha-ha-ha" at the end of the call
        float stutter = u > 0.55f ? 0.35f + 0.65f * std::pow(std::fabs(std::sin(TAU * 4.2f * (t - 0.55f * length))), 0.6f) : 1.0f;
        float breath = breath_lp(r.Next()) * 0.25f;
        b[n] = (voiced + breath) * env * stutter;
    }
    Normalize(b, 0.85f);
    return b;
}

// lips fluttering as air is blown out the nose
std::vector<float> SynthSnort(unsigned int seed) {
    Rand r{seed};
    std::vector<float> b = Buffer(0.75f);
    OnePole lp(650.0f);
    BandPass nasal(320.0f, 1.5f);
    for (size_t n = 0; n < b.size(); n++) {
        float t = (float)n / RATE;
        float env = std::fmin(t / 0.02f, 1.0f) * std::exp(-t * 3.6f);
        float flutter = 0.45f + 0.55f * std::sin(TAU * (27.0f + 4.0f * std::sin(t * 9.0f)) * t);
        float noise = lp(r.Next());
        b[n] = (noise * 2.0f + nasal(noise) * 3.0f) * env * flutter;
    }
    Normalize(b, 0.8f);
    return b;
}

std::vector<float> SynthBreath(unsigned int seed) {
    Rand r{seed};
    std::vector<float> b = Buffer(0.65f);
    OnePole lp(950.0f);
    BandPass body(500.0f, 1.0f);
    for (size_t n = 0; n < b.size(); n++) {
        float t = (float)n / RATE;
        float env = std::pow(std::sin(glm::pi<float>() * t / 0.65f), 2.0f);
        float noise = lp(r.Next());
        b[n] = (noise + body(noise) * 1.5f) * env;
    }
    Normalize(b, 0.6f);
    return b;
}

// two note whistle, short then a rising call
std::vector<float> SynthWhistle() {
    Rand r{3u};
    std::vector<float> b = Buffer(1.0f);
    BandPass air(1400.0f, 1.5f);
    float phase = 0.0f;
    for (size_t n = 0; n < b.size(); n++) {
        float t = (float)n / RATE;
        float f = 0.0f;
        float env = 0.0f;
        if (t < 0.24f) {
            f = 1250.0f + 180.0f * (t / 0.24f);
            env = std::fmin(t / 0.02f, 1.0f) * std::fmin((0.24f - t) / 0.03f, 1.0f);
        } else if (t > 0.32f && t < 0.92f) {
            float k = (t - 0.32f) / 0.6f;
            f = 1100.0f + 650.0f * std::sqrt(k);
            env = std::fmin((t - 0.32f) / 0.03f, 1.0f) * std::fmin((0.92f - t) / 0.06f, 1.0f);
        }
        f *= 1.0f + 0.008f * std::sin(TAU * 6.0f * t);
        phase += TAU * f / RATE;
        float tone = std::sin(phase) + 0.12f * std::sin(2.0f * phase);
        b[n] = (tone + air(r.Next()) * 0.6f) * env;
    }
    Normalize(b, 0.7f);
    return b;
}

// seamless gusty wind loop for galloping
std::vector<float> SynthWind() {
    Rand r{99u};
    const float loop = 4.0f;
    const float fade = 0.5f;
    std::vector<float> raw = Buffer(loop + fade);
    OnePole lp(380.0f);
    BandPass whistle(900.0f, 4.0f);
    float brown = 0.0f;
    for (size_t n = 0; n < raw.size(); n++) {
        float t = (float)n / RATE;
        brown = brown * 0.995f + r.Next() * 0.08f;
        float gust = 0.65f + 0.2f * std::sin(TAU * 0.25f * t) + 0.15f * std::sin(TAU * 0.75f * t + 1.0f);
        raw[n] = (lp(brown) * 3.0f + whistle(r.Next()) * 0.25f) * gust;
    }
    size_t L = (size_t)(loop * RATE);
    size_t X = (size_t)(fade * RATE);
    std::vector<float> b(raw.begin(), raw.begin() + L);
    for (size_t i = 0; i < X; i++) {
        float k = (float)i / X;
        b[i] = raw[i] * k + raw[L + i] * (1.0f - k);
    }
    Normalize(b, 0.8f);
    return b;
}

vec3df ToIrr(const glm::vec3& v) {
    // irrKlang is left handed
    return vec3df(v.x, v.y, -v.z);
}

} // namespace

HorseAudio::HorseAudio(ISoundEngine* engine) : engine_(engine) {
    if (!engine_) {
        return;
    }
    for (unsigned int i = 0; i < 5; i++) {
        Register(HOOF_SAND, ("horse_hoof_" + std::to_string(i)).c_str(), SynthHoof(1000u + i * 7919u));
    }
    Register(LANDING, "horse_landing", SynthLanding());
    Register(WHINNY, "horse_whinny_0", SynthWhinny(11u, 1.6f, 950.0f));
    Register(WHINNY, "horse_whinny_1", SynthWhinny(23u, 1.3f, 1050.0f));
    Register(SNORT, "horse_snort_0", SynthSnort(31u));
    Register(SNORT, "horse_snort_1", SynthSnort(47u));
    Register(BREATH, "horse_breath_0", SynthBreath(53u));
    Register(BREATH, "horse_breath_1", SynthBreath(59u));
    Register(WHISTLE, "player_whistle", SynthWhistle());
    Register(WIND, "riding_wind", SynthWind());
}

void HorseAudio::Register(Sound s, const char* name, const std::vector<float>& samples) {
    // roll off the top end so nothing is harsh on the ears (two pole low pass, ~12 dB/oct)
    const float cutoff = s == WHISTLE ? 3500.0f : s == WIND ? 1600.0f : 2800.0f;
    OnePole lp1(cutoff), lp2(cutoff);
    std::vector<short> pcm(samples.size());
    for (size_t i = 0; i < samples.size(); i++) {
        float v = lp2(lp1(samples[i]));
        pcm[i] = (short)(std::fmax(-1.0f, std::fmin(1.0f, v)) * 32000.0f);
    }
    SAudioStreamFormat format;
    format.ChannelCount = 1;
    format.FrameCount = (ik_s32)pcm.size();
    format.SampleRate = RATE;
    format.SampleFormat = ESF_S16;
    ISoundSource* src = engine_->getSoundSource(name, false);
    if (!src) {
        src = engine_->addSoundSourceFromPCMData(pcm.data(), (ik_s32)(pcm.size() * sizeof(short)), name, format, true);
    }
    if (src) {
        src->setDefaultMinDistance(40.0f);
        sources_[s].push_back(src);
    }
}

ISoundSource* HorseAudio::Pick(Sound s) {
    if (!engine_ || sources_[s].empty()) {
        return nullptr;
    }
    rng_ = rng_ * 1664525u + 1013904223u;
    return sources_[s][(rng_ >> 16) % sources_[s].size()];
}

void HorseAudio::Play(Sound s, const glm::vec3& pos, float volume, float pitch) {
    ISoundSource* src = Pick(s);
    if (!src) {
        return;
    }
    ISound* snd = engine_->play3D(src, ToIrr(pos), false, true, true);
    if (snd) {
        snd->setVolume(volume);
        snd->setPlaybackSpeed(pitch);
        snd->setIsPaused(false);
        snd->drop();
    }
}

void HorseAudio::Play2D(Sound s, float volume, float pitch) {
    ISoundSource* src = Pick(s);
    if (!src) {
        return;
    }
    ISound* snd = engine_->play2D(src, false, true, true);
    if (snd) {
        snd->setVolume(volume);
        snd->setPlaybackSpeed(pitch);
        snd->setIsPaused(false);
        snd->drop();
    }
}

ISound* HorseAudio::StartLoop(Sound s, float volume) {
    ISoundSource* src = Pick(s);
    if (!src) {
        return nullptr;
    }
    ISound* snd = engine_->play2D(src, true, true, true);
    if (snd) {
        snd->setVolume(volume);
        snd->setIsPaused(false);
    }
    return snd;
}

void HorseAudio::StopLoop(ISound*& sound) {
    if (sound) {
        sound->stop();
        sound->drop();
        sound = nullptr;
    }
}

void HorseAudio::SetListener(const glm::vec3& pos, const glm::vec3& forward, const glm::vec3& up) {
    if (engine_) {
        engine_->setListenerPosition(ToIrr(pos), ToIrr(forward), vec3df(0, 0, 0), ToIrr(up));
    }
}
