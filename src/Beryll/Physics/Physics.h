#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"
#include "Beryll/Core/Timer.h"
#include "Beryll/Core/GameLoop.h"

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
        RigidBodyData(int id, const std::shared_ptr<btRigidBody>& b, const bool& exist)
        : bodyID(id), rb(b), existInDynamicWorld(exist) {}

        const int bodyID;
        const std::shared_ptr<btRigidBody> rb;
        bool existInDynamicWorld = false;
    };

    struct PhysicsTransforms
    {
        glm::vec3 origin{0.0f, 0.0f, 0.0f};
        glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f}; // identity quaternion = no rotation
    };

    struct RayClosestHit
    {
        bool hit = false;
        const int objectID; // if something was hitted
        glm::vec3 hitPoint = glm::vec3(0.0f);
        glm::vec3 hitNormal = glm::vec3(0.0f);
        float hitFraction = 0.0f; // hit distance in range 0...1 between start and end points

        operator bool() const { return hit; }
    };
    struct RayAllHits
    {
        bool hit = false;
        std::vector<int> objectsID; // all hitted
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
                              const int objectID,
                              float mass,
                              bool wantCallBack,
                              CollisionFlags collFlag,
                              CollisionGroups collGroup,
                              CollisionGroups collMask);

        static void setPosition(const int ID, const glm::vec3& pos, bool resetVelocities = true);
        static void setRotation(const int ID, const glm::quat& rot, bool resetVelocities = true);
        static PhysicsTransforms getTransforms(const int ID);
        // bullet physics dont store scale in transform. keep scale in model matrix/vector in model

        static void softRemoveObject(const int ID); // remove from simulation but keep in m_rigidBodiesMap
        static void restoreObject(const int ID, bool resetVelocities = true); // restore from m_rigidBodiesMap to simulation

        static void activateObject(const int ID); // awake object in physics world

        static void setAngularFactor(const int ID, const glm::vec3& angFactor); // affect objects rotation speed during collisions
        static void setLinearFactor(const int ID, const glm::vec3& linFactor); // affect objects translation speed during collisions
        static void setAngularVelocity(const int ID, const glm::vec3& angVelocity); // set rotation velocity
        static void setLinearVelocity(const int ID, const glm::vec3& linVelocity); // set translation velocity

        static void setDefaultGravity(const glm::vec3& gravity); // change gravity for whole physics world
        static void setGravityForObject(const int ID, const glm::vec3& gravity); // change gravity for object
        static void disableGravityForObject(const int ID);
        static void enableGravityForObject(const int ID);

        static bool getIsCollision(const int ID1, const int ID2);
        static std::vector<int> getCollisionsWithGroup(const int id, const CollisionGroups group); // return IDs of all colliding objects in specific group
        static const std::set<std::pair<const int, const int>>& getAllCollisions() { return m_collisionPairs; }
        static std::vector<std::pair<glm::vec3, glm::vec3>> getAllCollisionPoints(const int ID1, const int ID2); // return point + his normal
        static std::vector<std::pair<glm::vec3, glm::vec3>> getAllCollisionPoints(const int ID1, const std::vector<int>& IDs); // return point + his normal

        // Cast ray. Only objects in physics world can be hitted
        static RayClosestHit castRayClosestHit(const glm::vec3& from, const glm::vec3 to, CollisionGroups collGroup, CollisionGroups collMask);
        static RayAllHits castRayAllHits(const glm::vec3& from, const glm::vec3 to, CollisionGroups collGroup, CollisionGroups collMask);

    private:
        friend class GameLoop;
        static void create();
        static void simulate();

        static bool collisionsCallBack(btManifoldPoint& cp, const btCollisionObjectWrapper* ob1, int ID1, int index1,
                                                            const btCollisionObjectWrapper* ob2, int ID2, int index2);
        static std::set<std::pair<const int, const int>> m_collisionPairs;

        static btVector3 m_gravity;
        static Timer m_timer;

        static std::mutex m_mutex;

        static std::unique_ptr<btDefaultCollisionConfiguration> m_collisionConfiguration;
        static std::unique_ptr<btCollisionDispatcherMt> m_dispatcherMT;
        static std::unique_ptr<btDbvtBroadphase> m_broadPhase;
        // pool solvers shouldn't be parallel solvers
        static std::unique_ptr<btConstraintSolverPoolMt> m_solverPoolMT;
        // single solver should be parallel solver
        static std::unique_ptr<btSequentialImpulseConstraintSolverMt> m_constraintSolverMT;
        static std::unique_ptr<btDiscreteDynamicsWorldMt> m_dynamicsWorldMT;

        // keep pointers from destroying
        static std::vector<std::shared_ptr<btCollisionShape>> m_collisionShapes;
        static std::vector<std::shared_ptr<btTriangleMesh>> m_triangleMeshes;
        static std::vector<std::shared_ptr<btDefaultMotionState>> m_motionStates;
        static std::map<const int, std::shared_ptr<RigidBodyData>> m_rigidBodiesMap;

        // increase resolution if your ball penetrate wall but you want collision
        // physics engine will do more small iteration during one simulation
        static int m_resolutionFactor;

        static float m_timeStep;
        static bool m_simulationEnabled;

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

    };
}