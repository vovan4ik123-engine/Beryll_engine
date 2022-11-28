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

        virtual void EmitCubesFromCenter(int particlesCount,
                                    float lifeTime,
                                    float sizeBegin,
                                    float sizeEnd,
                                    glm::vec4 colorBegin,
                                    glm::vec4 colorEnd,
                                    glm::vec3 pos,
                                    glm::vec3 gravity,
                                    float speed) = 0;

        virtual int getActiveCount() = 0;

        const static std::vector<glm::vec4> quadVertices;

        const static std::vector<glm::vec4> cubeVertices;

    protected:
        ParticleSystem() {};

        struct QuadParticle
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

            float rotation = 0.0f;
            float rotationSpeed = 0.0f;
            float rotationSide = 0.0f; // rotate to left or to right. set to -1.0f or +1.0f

            const glm::vec3 faceDir{0.0f, 0.0f, 1.0f};

            glm::mat4 MVPMatrix{1.0f};
            glm::vec4 finalVertex0{0.0f}; // quad original vertices multiplied by MVPMatrix
            glm::vec4 finalVertex1{0.0f};
            glm::vec4 finalVertex2{0.0f};
            glm::vec4 finalVertex3{0.0f};
        };

        struct CubeParticle
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

            glm::mat4 rotationMatrix{1.0f};
            glm::mat4 MVPMatrix{1.0f};
            glm::vec4 finalVertex0{0.0f}; // cube original vertices multiplied by MVPMatrix
            glm::vec4 finalVertex1{0.0f};
            glm::vec4 finalVertex2{0.0f};
            glm::vec4 finalVertex3{0.0f};

            glm::vec4 finalVertex4{0.0f};
            glm::vec4 finalVertex5{0.0f};
            glm::vec4 finalVertex6{0.0f};
            glm::vec4 finalVertex7{0.0f};
        };

    private:
        friend class GameLoop;
        static void create();

        static std::unique_ptr<ParticleSystem> m_system;
    };
}
