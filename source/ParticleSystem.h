#pragma once

#include "Types.h"
#include <array>
#include <atomic>
#include <vector>

constexpr float ParticleLifeTimeMax = 5.0f;
constexpr size_t ParticlesByExplosion = 64;
constexpr size_t EffectCountMax = 2048;
constexpr size_t SimultaneousEmissionsMax = 256;
constexpr size_t RemoveParticlesOnUpdateMax = 1024;

constexpr float ParticleExplosionProbability = 0.85f;
constexpr Vector2f GravityVector{ 0.0f, 100.0f };

class ParticleSystem
{
public:
    ParticleSystem(const Bounds& bounds);

    void emit(const Vector2f& position);

    void update(float dt);
    void render() const;

private:
    void updateExplosion(float dt, size_t idx);
    void checkExplosions();
    void swapBuffers();

private:
    struct Particle
    {
        Vector2f position;
        Vector2f velocity;
        Color color;
        float lifetime;
        float age;
    };

    struct Explosion
    {
        std::array<Particle, ParticlesByExplosion> particles;
        size_t activeParticles;
    };

private:
    Bounds m_bounds;

    using ParticleContainer = std::vector<Explosion>;
    ParticleContainer m_buffers[2];
    ParticleContainer* m_updateBuffer;
    ParticleContainer* m_renderBuffer;

    std::vector<Vector2f> m_toEmit;

    mutable std::atomic_bool m_isRenderActive = false;
    std::atomic_bool m_blockRender = false;
};
