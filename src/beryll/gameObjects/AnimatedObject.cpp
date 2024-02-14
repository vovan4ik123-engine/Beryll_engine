#include "AnimatedObject.h"

namespace Beryll
{
    AnimatedObject::AnimatedObject(const char* filePath,
                                   SceneObjectGroups sceneGroup)
                                   : BaseAnimatedObject(filePath, sceneGroup)
    {
        BR_ASSERT((m_scene->mNumMeshes == 1), "Animated object: %s MUST contain only 1 mesh.", filePath);
        BR_ASSERT((m_scene->HasAnimations() && m_scene->mMeshes[0]->mNumBones > 0), "%s", "Animated object must have animation + bone.");
    }

    AnimatedObject::~AnimatedObject()
    {

    }
}
