#include "SimpleCollidingCharacter.h"

namespace Beryll
{
    SimpleCollidingCharacter::SimpleCollidingCharacter(const char* filePath,  // Common params.
                                                       float collisionMassKg,    // Physics params.
                                                       bool wantCollisionCallBack,
                                                       CollisionFlags collFlag,
                                                       CollisionGroups collGroup,
                                                       CollisionGroups collMask,
                                                       SceneObjectGroups sceneGroup)
                                                       // Call base class constructor.
                                                       : SimpleCollidingObject(filePath,
                                                                               collisionMassKg,
                                                                               wantCollisionCallBack,
                                                                               collFlag,
                                                                               collGroup,
                                                                               collMask,
                                                                               sceneGroup)
    {
        BR_ASSERT(((collFlag == CollisionFlags::DYNAMIC && wantCollisionCallBack == true) ||
                   (collFlag != CollisionFlags::DYNAMIC)), "%s", "collFlag DYNAMIC must have wantCollisionCallBack = true to use character controller.");
    }

    SimpleCollidingCharacter::~SimpleCollidingCharacter()
    {

    }

    void SimpleCollidingCharacter::updateBeforePhysics()
    {
        // Call base class method first.
        SimpleCollidingObject::updateBeforePhysics();
    }

    void SimpleCollidingCharacter::updateAfterPhysics()
    {
        // Call base class method first.
        SimpleCollidingObject::updateAfterPhysics();

        m_controller.update();
    }

    void SimpleCollidingCharacter::draw()
    {
        // Call base class method first.
        SimpleCollidingObject::draw();
    }
}
