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

        BR_ASSERT((collMask != CollisionGroups::NONE), "%s", "Character has collMask = NONE.");

        // Colliding character described by collision mesh.
        m_controller.m_fromOriginToTop = std::abs(m_mostTopVertex);
        m_controller.m_fromOriginToBottom = std::abs(m_mostBottomVertex);
        m_controller.m_characterHeight = m_controller.m_fromOriginToTop + m_controller.m_fromOriginToBottom;
        m_controller.m_XZRadius = (std::abs(m_biggestX) + std::abs(m_smallestX)) * 0.5f;
        BR_INFO("m_fromOriginToTop: %f, m_fromOriginToBottom: %f, m_XZRadius: %f, m_characterHeight: %f",
                m_controller.m_fromOriginToTop, m_controller.m_fromOriginToBottom, m_controller.m_XZRadius, m_controller.m_characterHeight);

        BR_ASSERT((m_controller.m_fromOriginToBottom > 0.0f && m_controller.m_fromOriginToTop > 0.0f && m_controller.m_XZRadius > 0.0f && m_controller.m_characterHeight > 0.0f),
                  "%s", "Characters XYZ dimensions are 0.");

        m_controller.m_previousYPos = m_origin.y;
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
