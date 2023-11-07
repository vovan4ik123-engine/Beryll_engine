#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "beryll/core/Timer.h"

namespace Beryll
{
    // Concave meshes should only be used for static objects (btBvhTriangleMeshShape).
    // Convex hull for dynamic (btConvexHullShape). or all?

    // About character controller
    // https://www.reddit.com/r/gamedev/comments/72kgm2/how_could_i_implement_a_character_controller_with/

    //Dynamic (moving) rigidbodies:
    //    Positive mass.
    //    Every simulation frame the dynamics will update its world transform.
    //Static rigidbodies:
    //    Zero mass.
    //    Static objects should never be moved by the user, just collide.
    //Kinematic rigidbodies:
    //    Zero mass.
    //    Can be animated by the user, but there will be only one-way interaction: dynamic.
    //    Objects will be pushed away but there is no influence from dynamics objects.
    //    body->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT); // Make object kinematic. Replace coll flags. Use | operator to add.

    // Each object in physics world can be a part of zero, one, or more collision groups.
    // Groups could represent concepts such as players, power-ups, enemies, and so on.
    // Collision mask indicates which groups this object should collide with.
    // In this way, we can use collision filtering to preventing the player from being hit by their own weapon projectiles,
    // or preventing enemies from being able to pick up power-ups.
    enum class CollisionGroups
    {
        // Max number of collision groups = 32(including NONE).
        // Last group is 31(1 << 30).
        NONE = 0,
        CAMERA = 1 << 0,
        STATIC_ENVIRONMENT = 1 << 1,
        DYNAMIC_ENVIRONMENT = 1 << 2,
        GROUND = 1 << 3,
        ARMY_RAY = 1 << 4,

        PLAYER_GROUP_1 = 1 << 5,
        PLAYER_GROUP_2 = 1 << 6,
        PLAYER_GROUP_3 = 1 << 7,
        PLAYER_GROUP_4 = 1 << 8,
        PLAYER_GROUP_5 = 1 << 9,
        PLAYER_GROUP_6 = 1 << 10,
        PLAYER_GROUP_7 = 1 << 11,
        PLAYER_GROUP_8 = 1 << 12,

        ENEMY_GROUP_1 = 1 << 13,
        ENEMY_GROUP_2 = 1 << 14,
        ENEMY_GROUP_3 = 1 << 15,
        ENEMY_GROUP_4 = 1 << 16,
        ENEMY_GROUP_5 = 1 << 17,
        ENEMY_GROUP_6 = 1 << 18,
        ENEMY_GROUP_7 = 1 << 19,
        ENEMY_GROUP_8 = 1 << 20,

        YOU_CAN_COMBINE_GROUPS_WHEN_USE = STATIC_ENVIRONMENT | CAMERA,

        ALL_GROUPS = -1 // all bits = 1
    };
    inline CollisionGroups operator | (CollisionGroups gr1, CollisionGroups gr2)
    {
        return static_cast<CollisionGroups>(static_cast<int>(gr1) | static_cast<int>(gr2));
    }

    enum class CollisionFlags
    {
        NONE = 0,
        DYNAMIC = 1, // Mass > 0.
        STATIC = 2, // Mass = 0.
        KINEMATIC = 3 // Use if you want move object with mass = 0, BUT maybe move DYNAMIC object.
    };

    struct RigidBodyData
    {
        RigidBodyData(int id,
                      const std::shared_ptr<btRigidBody>& b,
                      const bool exist,
                      CollisionGroups collGr,
                      CollisionGroups collMs,
                      CollisionFlags collFl,
                      float ms)
        : bodyID(id), rb(b), existInDynamicWorld(exist), collGroup(collGr), collMask(collMs), collFlag(collFl), mass(ms)  {}

        const int bodyID;
        const std::shared_ptr<btRigidBody> rb;
        bool existInDynamicWorld = false;

        CollisionGroups collGroup = CollisionGroups::NONE;
        CollisionGroups collMask = CollisionGroups::NONE;

        CollisionFlags collFlag = CollisionFlags::NONE;
        float mass = -1.0f;
    };

    struct PhysicsTransforms
    {
        glm::vec3 origin{0.0f, 0.0f, 0.0f};
        glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f}; // Identity quaternion = no rotation.
    };

    struct RayClosestHit
    {
        const bool isHit = false;
        const int objectID = -1; // If something was hitted.
        const CollisionFlags collFlag = CollisionFlags::NONE;
        const float hittedObjectMass = -1.0f;
        const glm::vec3 hitPoint{0.0f};
        const glm::vec3 hitNormal{0.0f};
        const float hitFraction = 0.0f; // Hit distance in range 0...1 between start and end points.
        const glm::vec3 hittedObjectOrigin{0.0f, 0.0f, 0.0f};

        operator bool() const { return isHit; }
    };
    struct RayAllHits
    {
        bool isHit = false;
        std::vector<int> objectsID; // All hitted.
        std::vector<CollisionFlags> objectsCollFlags;
        std::vector<float> hittedObjectsMass;
        std::vector<glm::vec3> hitPoints;
        std::vector<glm::vec3> hitNormals;
        std::vector<float> hitFractions; // Hit distances in range 0...1 between start and end points.
        std::vector<glm::vec3> hittedObjectsOrigins;

        operator bool() const { return isHit; }
    };

    class Physics final
    {
    public:
        Physics() = delete;
        ~Physics() = delete;

        // Set count of internal steps during one simulation. From 1 to 20.
        // That increase simulation time and CPU usage and increase simulation accuracy(resolution).
        // If your balls penetrates your walls instead of colliding with them increase it.
        static void setResolution(int res)
        {
            if(res > 0 && res <= 20)
                m_resolutionFactor = res;
        }

        static void disableSimulation()
        {
            m_simulationEnabled = false;
        }

        static void enableSimulation()
        {
            m_timer.reset();
            m_simulationEnabled = true;
        }

        static void resetTimer()
        {
            m_timer.reset();
        }

        // Simulation time in milli sec.
        static float getSimulationTime()
        {
            return m_simulationTime;
        }

        static void hardRemoveAllObjects(); // Remove from everywhere.

        static bool getIsCollisionGroupContainsOther(CollisionGroups gr1, CollisionGroups gr2)
        {
            // Return true if gr1 contains gr2.
            return (static_cast<int>(gr1) & static_cast<int>(gr2)) > 0;
        }

        static void setAngularVelocity(const int ID, const glm::vec3& angVelocity, bool resetVelocities = false); // Set rotation velocity.
        static void setLinearVelocity(const int ID, const glm::vec3& linVelocity, bool resetVelocities = false); // Set translation velocity.
        static void setGravityForAllWorld(const glm::vec3& gravity); // Change gravity for whole physics world.

        static bool getIsCollision(const int ID1, const int ID2);
        static bool getIsCollision(const int ID, const CollisionGroups group);
        static std::vector<int> getCollisionsWithGroup(const int id, const CollisionGroups group); // Return IDs of all colliding objects in specific group.
        static const std::vector<std::pair<const int, const int>>& getAllCollisions() { return m_collisionPairs; }
        static std::vector<std::pair<glm::vec3, glm::vec3>> getAllCollisionPoints(const int ID1, const int ID2); // Return point + his normal.
        static std::vector<std::pair<glm::vec3, glm::vec3>> getAllCollisionPoints(const int ID1, const std::vector<int>& IDs); // Return point + his normal.
        static glm::vec3 getDefaultGravity() { return glm::vec3(m_gravity.x(), m_gravity.y(), m_gravity.z()); }

        // Cast ray. Only objects in physics world can be hit.
        static RayClosestHit castRayClosestHit(const glm::vec3& from, const glm::vec3& to, CollisionGroups collGroup, CollisionGroups collMask);
        static RayAllHits castRayAllHits(const glm::vec3& from, const glm::vec3& to, CollisionGroups collGroup, CollisionGroups collMask);

    private:
        friend class GameLoop;
        static void create();
        static void simulate();

        static bool collisionsCallBack(btManifoldPoint& cp, const btCollisionObjectWrapper* ob1, int ID1, int index1,
                                                            const btCollisionObjectWrapper* ob2, int ID2, int index2);
        static std::vector<std::pair<const int, const int>> m_collisionPairs;

        static btVector3 m_gravity;
        static Timer m_timer;

        static std::mutex m_mutex;

        static std::unique_ptr<btDefaultCollisionConfiguration> m_collisionConfiguration;
        static std::unique_ptr<btCollisionDispatcherMt> m_dispatcherMT;
        static std::unique_ptr<btDbvtBroadphase> m_broadPhase;
        // Pool solvers shouldn't be parallel solvers.
        static std::unique_ptr<btConstraintSolverPoolMt> m_solverPoolMT;
        // Single solver should be parallel solver
        static std::unique_ptr<btSequentialImpulseConstraintSolverMt> m_constraintSolverMT;
        static std::unique_ptr<btDiscreteDynamicsWorldMt> m_dynamicsWorldMT;

        // Keep pointers from destroying.
        static std::vector<std::shared_ptr<btCollisionShape>> m_collisionShapes;
        static std::vector<std::shared_ptr<btTriangleMesh>> m_triangleMeshes;
        static std::vector<std::shared_ptr<btDefaultMotionState>> m_motionStates;
        static std::map<const int, std::shared_ptr<RigidBodyData>> m_rigidBodiesMap;

        // Increase resolution if your ball penetrate wall but you want collision.
        // Physics engine will do more small iteration during one simulation.
        static int m_resolutionFactor;

        static float m_timeStep;
        static bool m_simulationEnabled;
        static float m_simulationTime; // Simulation time in milli sec.

        static void resetVelocitiesForObject(const std::shared_ptr<btRigidBody>& b, bool reset);

        friend class SceneObject;
        static void setOrigin(const int ID, const glm::vec3& orig, bool resetVelocities = false);
        static void addToOrigin(const int ID, const glm::vec3& dist, bool resetVelocities = false);
        static void addToRotation(const int ID, const glm::quat& qua, bool resetVelocities = false);
        static void setAngularFactor(const int ID, const glm::vec3& angFactor, bool resetVelocities = false); // Affect objects rotation speed during collisions.
        static void setLinearFactor(const int ID, const glm::vec3& linFactor, bool resetVelocities = false); // Affect objects translation speed during collisions.
        static void disableGravityForObject(const int ID, bool resetVelocities = false);
        static void enableDefaultGravityForObject(const int ID, bool resetVelocities = false);
        static void setGravityForObject(const int ID, const glm::vec3& gravity, bool resetVelocities = false); // Change gravity for object.
        static void activateObject(const int ID); // Awake object in physics world.
        static bool getIsObjectActive(const int ID); // Check if object is active.
        static void resetVelocitiesForObject(const int ID);
        static void applyCentralImpulseForObject(const int ID, const glm::vec3& impulse);
        static void softRemoveObject(const int ID); // Remove from simulation but keep in m_rigidBodiesMap.
        static void restoreObject(const int ID, bool resetVelocities = false); // Restore from m_rigidBodiesMap to simulation.

        // addObject() should be called only from SimpleCollidingObject/AnimatedCollidingObject and only from one thread.
        // getTransforms() should be called only from SimpleCollidingObject/AnimatedCollidingObject and can be called from many threads.
        // Bullet physics does not store scale in transforms.
        friend class SimpleCollidingObject;
        friend class AnimatedCollidingObject;
        static PhysicsTransforms getTransforms(const int ID);

        static void addObject(const std::vector<glm::vec3>& vertices,
                              const std::vector<uint32_t>& indices,
                              const glm::mat4& transforms,
                              const std::string& meshName,
                              const int objectID,
                              float mass,
                              bool wantCallBack,
                              CollisionFlags collFlag,
                              CollisionGroups collGroup,
                              CollisionGroups collMask);

        static void addConcaveMesh(const std::vector<glm::vec3>& vertices,
                                   const std::vector<uint32_t>& indices,
                                   const glm::mat4& transforms,
                                   const int objectID,
                                   float mass,
                                   bool wantCallBack,
                                   CollisionFlags collFlag,
                                   CollisionGroups collGroup,
                                   CollisionGroups collMask); // vognutaja

        static void addConvexMesh(const std::vector<glm::vec3>& vertices,
                                  const std::vector<uint32_t>& indices,
                                  const glm::mat4& transforms,
                                  const int objectID,
                                  float mass,
                                  bool wantCallBack,
                                  CollisionFlags collFlag,
                                  CollisionGroups collGroup,
                                  CollisionGroups collMask); // vypuklaja

        static void addBoxShape(const std::vector<glm::vec3>& vertices,
                                  const glm::mat4& transforms,
                                  const int objectID,
                                  float mass,
                                  bool wantCallBack,
                                  CollisionFlags collFlag,
                                  CollisionGroups collGroup,
                                  CollisionGroups collMask);

        static void addSphereShape(const std::vector<glm::vec3>& vertices,
                                    const glm::mat4& transforms,
                                    const int objectID,
                                    float mass,
                                    bool wantCallBack,
                                    CollisionFlags collFlag,
                                    CollisionGroups collGroup,
                                    CollisionGroups collMask);

        static void addCapsuleShape(const std::vector<glm::vec3>& vertices,
                                    const glm::mat4& transforms,
                                    const int objectID,
                                    float mass,
                                    bool wantCallBack,
                                    CollisionFlags collFlag,
                                    CollisionGroups collGroup,
                                    CollisionGroups collMask);

        static void addCylinderShape(const std::vector<glm::vec3>& vertices,
                                    const glm::mat4& transforms,
                                    const int objectID,
                                    float mass,
                                    bool wantCallBack,
                                    CollisionFlags collFlag,
                                    CollisionGroups collGroup,
                                    CollisionGroups collMask);
    };
}
