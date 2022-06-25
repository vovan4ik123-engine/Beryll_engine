#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"
#include "Beryll/Core/Timer.h"

namespace Beryll
{
    // Concave meshes should only be used for static objects (btBvhTriangleMeshShape).
    // Convex hull for dynamic (btConvexHullShape). or all?

    // About character controller
    // https://www.reddit.com/r/gamedev/comments/72kgm2/how_could_i_implement_a_character_controller_with/

    //Dynamic (moving) rigidbodies
    //    positive mass
    //    every simulation frame the dynamics will update its world transform
    //Static rigidbodies
    //    zero mass
    //    static objects should never be moved by the user, just collide
    //Kinematic rigidbodies
    //    zero mass
    //    can be animated by the user, but there will be only one-way interaction: dynamic
    //    objects will be pushed away but there is no influence from dynamics objects
    //    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT); // make object kinematic
    //    do body->setActivationState(DISABLE_DEACTIVATION); or body->activate(true); // when you move object. moving object should be active !

    // Each object in physics world can be a part of zero, one, or more collision groups.
    // Groups could represent concepts such as players, power-ups, enemies, and so on.
    // Collision mask indicates which groups this object should collide with.
    // In this way, we can use collision filtering to preventing the player from being hit by their own weapon projectiles,
    // or preventing enemies from being able to pick up power-ups.
    enum class CollisionGroups
    {
        NONE = 0,
        CUBE = 1,
        GROUND = 2,
        CAMERA = 4,
        PLAYER = 8,
        WALL = 16,
        BULLET = 32,
        YOU_CANN_ADD_YOUR_GROUP1 = 64,
        YOU_CANN_ADD_YOUR_GROUP2 = 128,

        YOU_CANN_COMBINE_GROUPS_WHEN_USE = GROUND | CAMERA | WALL,

        ALL_GROUPS = -1 // all bits = 1
    };
    inline CollisionGroups operator | (const CollisionGroups& gr1, const CollisionGroups& gr2)
    {
        return static_cast<CollisionGroups>(static_cast<int>(gr1) | static_cast<int>(gr2));
    }

    enum class CollisionFlags
    {
        DYNAMIC = 0, // mass > 0
        STATIC = 1, // mass = 0
        KINEMATIC = 2 // use if you want move object with mass = 0
    };

    struct RigidBodyData
    {
        RigidBodyData(const std::string& id, const std::shared_ptr<btRigidBody>& b, const bool& exist)
        : bodyID(id), rb(b), existInDynamicWorld(exist) {}

        const std::string bodyID;
        const std::shared_ptr<btRigidBody> rb;
        bool existInDynamicWorld = false;
    };

    struct PhysicsTransforms
    {
        glm::vec3 position{0.0f, 0.0f, 0.0f};
        glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f}; // identity quaternion = no rotation
    };

    struct RayClosestHit
    {
        bool hit = false;
        std::string_view objectID; // if comething was hitted
        glm::vec3 hitPoint = glm::vec3(0.0f);
        glm::vec3 hitNormal = glm::vec3(0.0f);
        float hitFraction = 0.0f; // hit distance in range 0...1 between start and end points

        operator bool() const { return hit; }
    };
    struct RayAllHits
    {
        bool hit = false;
        std::vector<std::string_view> objectsID; // all hitted
        std::vector<glm::vec3> hitPoints;
        std::vector<glm::vec3> hitNormals;
        std::vector<float> hitFractions; // hit distances in range 0...1 between start and end points

        operator bool() const { return hit; }
    };

    class Physics
    {
    public:
        Physics() = delete;
        ~Physics() = delete;

        static void create();
        static void simulate();

        // Set count of internal steps during one simulation. From 1 to 20
        // That increase simulation time and CPU usage and increase simulation accuracy(resolution)
        // If your balls penetrates your walls instead of colliding with them increace it
        static void setResolution(int res)
        {
            if(res >= 20)
                m_resolutionFactor = 20;
            else if(res > 0 && res < 20)
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

        static void addObject(const std::vector<glm::vec3>& vertices,
                              const std::vector<uint32_t>& indices,
                              const glm::mat4& transforms,
                              const std::string& meshName,
                              const std::string& objectID,
                              float mass,
                              bool wantCallBack,
                              CollisionFlags collFlag,
                              CollisionGroups collGroup,
                              CollisionGroups collMask);

        static void setPosition(const std::string& ID, const glm::vec3& pos, bool resetVelocities = true);
        static PhysicsTransforms getTransforms(const std::string& ID);
        // bullet physics dont store scale in transform. keep scale in model matrix/vector in model

        static void softRemoveObject(const std::string& ID); // remove from simulation but keep in m_rigidBodiesMap
        static void restoreObject(const std::string& ID, bool resetVelocities = true); // restore from m_rigidBodiesMap to simulation

        static void setAngularFactor(const std::string& ID, const glm::vec3& angFactor); // affect objects rotation speed during collisions
        static void setLinearFactor(const std::string& ID, const glm::vec3& linFactor); // affect objects translation speed during collisions
        static void setAngularVelocity(const std::string& ID, const glm::vec3& angVelocity); // set rotation velocity
        static void setLinearVelocity(const std::string& ID, const glm::vec3& linVelocity); // set translation velocity

        static void setDefaultGravity(const glm::vec3& gravity); // change gravity for whole physics world
        static void setGravityForObject(const std::string& ID, const glm::vec3& gravity); // change gravity for object
        static void disableGravityForObject(const std::string& ID);
        static void setDefaultGravityForObject(const std::string& ID);

        static bool getIsCollision(std::string_view id1, std::string_view id2);
        static const std::set<std::pair<std::string_view, std::string_view>>& getAllCollisions() { return m_collisionPairs; }

        // Cast ray. Only objects in physics world can be hitted
        static RayClosestHit castRayClosestHit(const glm::vec3& from, const glm::vec3 to, CollisionGroups collGroup, CollisionGroups collMask);
        static RayAllHits castRayAllHits(const glm::vec3& from, const glm::vec3 to, CollisionGroups collGroup, CollisionGroups collMask);

    private:
        static bool collisionsCallBack(btManifoldPoint& cp, const btCollisionObjectWrapper* ob1, int id1, int index1,
                                                            const btCollisionObjectWrapper* ob2, int id2, int index2);
        static std::set<std::pair<std::string_view, std::string_view>> m_collisionPairs;

        static btVector3 m_gravity;
        static Timer m_timer;

        static std::unique_ptr<btDefaultCollisionConfiguration> m_collisionConfiguration;
        // use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
        static std::unique_ptr<btCollisionDispatcher> m_dispatcher;
        // btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
        static std::unique_ptr<btBroadphaseInterface> m_overlappingPairCache;
        // the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
        static std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
        static std::unique_ptr<btDiscreteDynamicsWorld> m_dynamicsWorld;

        // keep pointers from destroying
        static std::vector<std::shared_ptr<btCollisionShape>> m_collisionShapes;
        static std::vector<std::shared_ptr<btTriangleMesh>> m_triangleMeshes;
        static std::vector<std::shared_ptr<btDefaultMotionState>> m_motionStates;
        static std::map<const std::string, std::shared_ptr<RigidBodyData>> m_rigidBodiesMap;

        // increase resolution if your ball penetrate wall but you want collision
        // physics engine will do more small iteration during one simulation
        static int m_resolutionFactor;

        static float m_timeStep;
        static bool m_simulationEnabled;

        static void addConcaveMesh(const std::vector<glm::vec3>& vertices,
                                   const std::vector<uint32_t>& indices,
                                   const glm::mat4& transforms,
                                   const std::string& objectID,
                                   float mass,
                                   bool wantCallBack,
                                   CollisionFlags collFlag,
                                   CollisionGroups collGroup,
                                   CollisionGroups collMask); // vognutaja

        static void addConvexMesh(const std::vector<glm::vec3>& vertices,
                                  const std::vector<uint32_t>& indices,
                                  const glm::mat4& transforms,
                                  const std::string& objectID,
                                  float mass,
                                  bool wantCallBack,
                                  CollisionFlags collFlag,
                                  CollisionGroups collGroup,
                                  CollisionGroups collMask); // vypuklaja

    };
}