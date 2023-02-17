#include "ParticleSystem.h"

#include "Tools.h"
#include <mutex>

namespace platform
{
    void drawPoint(float x, float y, float r, float g, float b, float a);

} // namespace platform

ParticleSystem::ParticleSystem(const Bounds& bounds)
    : m_bounds(bounds)
{
    for (auto& buffer : m_buffers)
    {
        buffer.resize(EffectCountMax);
    }

    m_updateBuffer = &m_buffers[0];
    m_renderBuffer = &m_buffers[1];

    m_toEmit.reserve(SimultaneousEmissionsMax);
}

void ParticleSystem::updateExplosion(float dt, size_t idx)
{
    auto& explosion = m_updateBuffer->at(idx);
    explosion = m_renderBuffer->at(idx);

    auto& particles = explosion.particles;
    auto& activeParticles = explosion.activeParticles;

    for (size_t i = 0, size = activeParticles; i < size; i++)
    {
        const auto idx = size - i - 1;
        auto& particle = particles[idx];

        particle.position += particle.velocity * dt;
        if (!m_bounds.test(particle.position)) // position + particleRadius
        {
            particle = particles[activeParticles - 1];
            activeParticles--;
            continue;
        }

        particle.velocity += GravityVector * (-dt);
        particle.age += dt;
        if (particle.age >= particle.lifetime)
        {
            const auto isEmissionRequired = tools::RandomFloat(0.0f, 1.0f) <= ParticleExplosionProbability;
            if (isEmissionRequired)
            {
                emit(particle.position);
            }

            particle = particles[activeParticles - 1];
            activeParticles--;
        }
    }
}

void ParticleSystem::checkExplosions()
{
    auto itEffect = m_updateBuffer->begin();
    for (const auto& pos : m_toEmit)
    {
        itEffect = std::find_if(itEffect, m_updateBuffer->end(), [](const auto& effect) { return effect.activeParticles == 0; });
        if (itEffect == m_updateBuffer->cend())
            break;

        for (size_t i = 0; i < ParticlesByExplosion; i++)
        {
            auto& particle = itEffect->particles[i];

            const auto v = tools::RandomFloat(0.0f, 6.28f);
            const auto ra = tools::RandomFloat(50.0f, 100.0f);

            particle.position = pos;
            particle.velocity = { ra * static_cast<float>(cos(v)), ra * static_cast<float>(sin(v)) };
            particle.lifetime = tools::RandomFloat(2.0f, ParticleLifeTimeMax);
            particle.color = { tools::RandomFloat(0.0f, 1.0f),
                               tools::RandomFloat(0.0f, 1.0f),
                               tools::RandomFloat(0.0f, 1.0f) };
            particle.age = 0.0f;
        }

        itEffect->activeParticles = ParticlesByExplosion;
    }

    m_toEmit.clear();
}

void ParticleSystem::emit(const Vector2f& position)
{
    if (m_toEmit.capacity() > m_toEmit.size())
        m_toEmit.push_back(position);
}

void ParticleSystem::update(float dt)
{
    for (size_t i = 0; i < EffectCountMax; i++)
        updateExplosion(dt, i);

    checkExplosions();
    swapBuffers();
}

void ParticleSystem::swapBuffers()
{
    m_blockRender.store(true);
    while (m_isRenderActive.load())
    {
    }

    std::swap(m_updateBuffer, m_renderBuffer);

    m_blockRender.store(false);
}

void ParticleSystem::render() const
{
    while (m_blockRender.load())
    {
    }

    m_isRenderActive.store(true);
    for (const auto& explosion : *m_renderBuffer)
    {
        for (size_t i = 0; i < explosion.activeParticles; i++)
        {
            const auto& particle = explosion.particles[i];
            const auto& position = particle.position;
            const auto& color = particle.color;

            platform::drawPoint(position.x, position.y, color.r, color.g, color.b, 1.0f);
        }
    }
    m_isRenderActive.store(false);
}
