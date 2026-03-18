#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <glm/vec3.hpp>
#include <SDL2/SDL.h>

constexpr int SAMPLE_RATE = 44100;

class Sound
{
public:
    Sound() : device(0), phaseL(0.0f), phaseR(0.0f) {}
    ~Sound()
    {
        if (device != 0)
        {
            SDL_CloseAudioDevice(device);
            SDL_QuitSubSystem(SDL_INIT_AUDIO);
        }
    }

    bool init()
    {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
            return false;

        SDL_AudioSpec want{};
        want.freq = SAMPLE_RATE;
        want.format = AUDIO_F32SYS;
        want.channels = 2; // stereo
        want.samples = 512;

        device = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);
        return device != 0;
    }

    // Main API
    void playFromPoints(const std::vector<glm::vec3> &points, float duration)
    {
        if (points.size() < 2)
            return;
        auto buffer = generate(points, duration);
        SDL_QueueAudio(device, buffer.data(), buffer.size() * sizeof(float));
        SDL_PauseAudioDevice(device, 0); // non-blocking start
        SDL_Delay(static_cast<Uint32>(duration * 1000));
    }

    // Check if audio is still playing
    bool isPlaying() { return SDL_GetQueuedAudioSize(device) > 0; }

private:
    SDL_AudioDeviceID device;
    float phaseL, phaseR;

    std::vector<float> generate(const std::vector<glm::vec3> &points, float duration)
    {
        int totalSamples = static_cast<int>(duration * SAMPLE_RATE);
        std::vector<float> buffer(totalSamples * 2, 0.0f); // stereo: L+R

        float lastAmp = 0.0f;
        float lastFreq = 0.0f;

        for (int i = 0; i < totalSamples; i++)
        {
            float t = (float)i / SAMPLE_RATE;
            float norm_t = t / duration;

            glm::vec3 p = sampleGraph(points, norm_t);

            // Normalize y for amplitude & frequency
            float yNorm = std::clamp(p.y / 50.0f, -1.0f, 1.0f);
            float amp = 0.99f * lastAmp + 0.01f * yNorm;
            lastAmp = amp;

            float freq = 220.0f + amp * 220.0f;
            freq = 0.99f * lastFreq + 0.01f * freq;
            lastFreq = freq;

            // x -> panning [-1,1]
            float pan = std::clamp(p.x / 50.0f, -1.0f, 1.0f);
            float left = amp * (1.0f - pan) * 0.5f;
            float right = amp * (1.0f + pan) * 0.5f;

            // z -> secondary modulation (vibrato)
            float vibrato = sin(2.0f * M_PI * p.z * 5.0f / SAMPLE_RATE);
            left *= 1.0f + 0.05f * vibrato;
            right *= 1.0f + 0.05f * vibrato;

            // advance phases
            phaseL += 2.0f * M_PI * freq / SAMPLE_RATE;
            phaseR += 2.0f * M_PI * freq / SAMPLE_RATE;
            if (phaseL > 2.0f * M_PI)
                phaseL -= 2.0f * M_PI;
            if (phaseR > 2.0f * M_PI)
                phaseR -= 2.0f * M_PI;

            buffer[2 * i] = std::clamp(left * sin(phaseL), -1.0f, 1.0f);
            buffer[2 * i + 1] = std::clamp(right * sin(phaseR), -1.0f, 1.0f);
        }

        // Fade in/out 10ms
        int fadeSamples = SAMPLE_RATE / 100;
        for (int i = 0; i < fadeSamples; i++)
        {
            float f = (float)i / fadeSamples;
            buffer[2 * i] *= f;
            buffer[2 * i + 1] *= f;
            buffer[2 * (totalSamples - 1 - i)] *= f;
            buffer[2 * (totalSamples - 1 - i) + 1] *= f;
        }

        return buffer;
    }

    glm::vec3 sampleGraph(const std::vector<glm::vec3> &points, float t)
    {
        int n = points.size();
        float index = t * (n - 1);
        int i = static_cast<int>(index);
        if (i >= n - 1)
            return points[n - 1];
        float frac = index - i;
        glm::vec3 a = points[i];
        glm::vec3 b = points[i + 1];
        return a + frac * (b - a);
    }
};