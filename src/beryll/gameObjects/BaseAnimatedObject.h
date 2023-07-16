#pragma once

#include "SceneObject.h"

namespace Beryll
{
    class BaseAnimatedObject : public SceneObject
    {
    protected:
        struct BoneMatrix // store loaded transforms for bone and final transform after frame interpolation
        {
            aiMatrix4x4 offsetMatrix{};
            aiMatrix4x4 finalWorldTransform{};
        };

    public:
        ~BaseAnimatedObject() override {}

        virtual uint32_t getBoneCount() = 0;
        virtual const std::vector<BoneMatrix>& getBoneMatrices() = 0;
        virtual void setCurrentAnimationByName(const char* name, bool playOneTime, bool startEvenIfSameAnimPlaying) = 0;
        virtual void setCurrentAnimationByIndex(int index, bool playOneTime, bool startEvenIfSameAnimPlaying) = 0;
        virtual void setDefaultAnimationByName(const char* name) = 0;
        virtual void setDefaultAnimationByIndex(int index) = 0;
        virtual bool getIsOneTimeAnimationFinished() = 0;
    };
}