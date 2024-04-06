#include "AnimatedCollidingCharacter.h"

namespace Beryll
{
    AnimatedCollidingCharacter::AnimatedCollidingCharacter(const char* filePath,  // Common params.
                                                           float collisionMassKg,    // Physics params.
                                                           bool wantCollisionCallBack,
                                                           CollisionFlags collFlag,
                                                           CollisionGroups collGroup,
                                                           CollisionGroups collMask,
                                                           SceneObjectGroups sceneGroup)
                                                           // Call base class constructor.
                                                           : AnimatedCollidingObject(filePath,
                                                                                     collisionMassKg,
                                                                                     wantCollisionCallBack,
                                                                                     collFlag,
                                                                                     collGroup,
                                                                                     collMask,
                                                                                     sceneGroup)
    {
        BR_ASSERT(((collFlag == CollisionFlags::DYNAMIC && wantCollisionCallBack == true) ||
                   (collFlag != CollisionFlags::DYNAMIC)), "%s", "collFlag DYNAMIC must have wantCollisionCallBack = true to use character controller.");

        m_controller.m_previousYPos = m_origin.y;
    }

    AnimatedCollidingCharacter::~AnimatedCollidingCharacter()
    {

    }

    void AnimatedCollidingCharacter::updateBeforePhysics()
    {
        // Call base class method first.
        AnimatedCollidingObject::updateBeforePhysics();
    }

    void AnimatedCollidingCharacter::updateAfterPhysics()
    {
        // Call base class method first.
        AnimatedCollidingObject::updateAfterPhysics();

        m_controller.update();
    }

    void AnimatedCollidingCharacter::draw()
    {
        // Call base class method first.
        AnimatedCollidingObject::draw();
    }
}
