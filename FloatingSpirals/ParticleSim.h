#pragma once

#include "lib/FastNoise.h"

class ParticleSim {
private:

public:
    FastNoise noise;
    Vector2u size;
    struct Particle {
        Vector2f pos, vel, acc;
        float val;
        void update(const float dT, const float cT, const Vector2u& size, const FastNoise& noise) {
            val = noise.GetNoise(pos.x, pos.y, cT) * 180.f;
            acc += Vector2f(cosf(val), sinf(val)) * dT * 1000.f;

            pos += vel * dT;
            vel += acc * dT;

            acc = Vector2f();
            vel *= .995f;

            /*
            if (pos.x < 0.f) {
                pos.x = (float)size.x;
            } else if (pos.x > size.x) {
                pos.x = 0.f;
            }
            if (pos.y < 0.f) {
                pos.y = (float)size.y;
            } else if (pos.y > size.y) {
                pos.y = 0.f;
            }
            
            */
            if (pos.x < 0.f) {
                pos.x = 0.f;
            } else if (pos.x > size.x) {
                pos.x = size.x;
            }
            if (pos.y < 0.f) {
                pos.y = 0.f;
            } else if (pos.y > size.y) {
                    pos.y = size.y;
            }
        }
    };
    std::vector<Particle> particles;
    float resolution;
    ParticleSim(int width, int height, int resolution = 144, int particleCount = 100) {
        noise.SetSeed(time(NULL));
        noise.SetNoiseType(FastNoise::Value);
        noise.SetFrequency(.005);

        this->size = Vector2u(width, height);

        this->resolution = 1.f / resolution;

        Particle p{};
        p.pos = Vector2f(-100.f, -100.f);
        particles.push_back(p);
        p.pos = Vector2f(-100.f, height + 100.f);
        particles.push_back(p);
        p.pos = Vector2f(width + 100.f, -100.f);
        particles.push_back(p);
        p.pos = Vector2f(width + 100.f, height + 100.f);
        particles.push_back(p);
        for (int i = 0; i < particleCount; i++) {
            p.pos = Vector2f((float)(rand() % width), (float)(rand() % height));
            particles.push_back(p);
        }
    }
    void update(float currentTime) {
        for (size_t i = 4; i < particles.size(); i++) {
            particles[i].update(resolution, currentTime, size, noise);
        }
    }
};