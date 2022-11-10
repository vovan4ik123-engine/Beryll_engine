#pragma once

#include "CppHeaders.h"
#include "LibsHeaders.h"

#include "Beryll/Core/Log.h"
#include "Beryll/Core/GameLoop.h"

namespace Beryll
{

    class ParticleSystem
    {
    public:
        virtual ~ParticleSystem() {};

        static std::unique_ptr<ParticleSystem>& getInstance()
        {
            BR_ASSERT((m_system != nullptr), "%s", "ParticleSystem is not created. Use ParticleSystem::create() first");
            return m_system;
        }

        virtual void draw() = 0;

        virtual void EmitQuadsFromCenter(int particlesCount,
                                    float lifeTime,
                                    float sizeBegin,
                                    float sizeEnd,
                                    glm::vec4 colorBegin,
                                    glm::vec4 colorEnd,
                                    glm::vec3 pos,
                                    glm::vec3 gravity,
                                    float speed) = 0;

        virtual void EmitQuadsExplosion(int particlesCount,
                                   float lifeTime,
                                   float sizeBegin,
                                   float sizeEnd,
                                   glm::vec3 cloudStartSize,
                                   glm::vec4 colorBegin,
                                   glm::vec4 colorEnd,
                                   glm::vec3 pos,
                                   glm::vec3 gravity,
                                   float speed) = 0;

        virtual void EmitCubesFromCenter(int particlesCount,
                                    float lifeTime,
                                    float sizeBegin,
                                    float sizeEnd,
                                    glm::vec4 colorBegin,
                                    glm::vec4 colorEnd,
                                    glm::vec3 pos,
                                    glm::vec3 gravity,
                                    float speed) = 0;

        virtual void EmitCubesExplosion(int particlesCount,
                                   float lifeTime,
                                   float sizeBegin,
                                   float sizeEnd,
                                   glm::vec3 cloudStartSize,
                                   glm::vec4 colorBegin,
                                   glm::vec4 colorEnd,
                                   glm::vec3 pos,
                                   glm::vec3 gravity,
                                   float speed) = 0;

        virtual int getActiveCount() = 0;

    protected:
        ParticleSystem() {};

        struct Particle
        {
            bool isActive = false;

            float lifeTimeFull = 1.0f;
            float lifeTimePassed = 0.0f;

            float sizeBegin = 1.0f;
            float sizeEnd = 0.0f;
            float finalSize = 0.0f;

            glm::vec4 colorBegin{1.0f};
            glm::vec4 colorEnd{0.0f};
            glm::vec4 finalColor{0.0f};

            glm::vec3 pos{0.0f};
            glm::vec3 moveDir{1.0f}; // must be normalized !!! and impacted by gravity (as parameter: glm::vec3 gravity)
            float moveSpeed = 1.0f; // meters in sec

            float rotation = 0.0f; // only for quad particle
            float rotationSpeed = 0.0f; // only for quad particle

            glm::mat4 rotationMatrix{1.0f}; // only for cube particle
            glm::mat4 MVPMatrix{1.0f};
            const glm::vec3 faceDir{0.0f, 0.0f, 1.0f};
        };

    private:
        friend class GameLoop;
        static void create();

        static std::unique_ptr<ParticleSystem> m_system;
    };
}
