#include "Physics.h"
#include "Beryll/Core/Log.h"
#include "Beryll/Utils/Matrix.h"

namespace Beryll
{
    btVector3 Physics::m_gravity = btVector3(0.0f, -10.0f, 0.0f);
    Timer Physics::m_timer;
    float Physics::m_timeStep = 0.0f;
    bool Physics::m_simulationEnabled = true;
    int Physics::m_resolutionFactor = 1;
    std::set<std::pair<std::string_view, std::string_view>> Physics::m_collisionPairs;
    std::vector<std::shared_ptr<btCollisionShape>> Physics::m_collisionShapes;
    std::vector<std::shared_ptr<btTriangleMesh>> Physics::m_triangleMeshes;
    std::vector<std::shared_ptr<btDefaultMotionState>> Physics::m_motionStates;
    std::map<const std::string, std::shared_ptr<RigidBodyData>> Physics::m_rigidBodiesMap;

    std::unique_ptr<btDefaultCollisionConfiguration> Physics::m_collisionConfiguration = nullptr;
    std::unique_ptr<btCollisionDispatcher> Physics::m_dispatcher = nullptr;
    std::unique_ptr<btBroadphaseInterface> Physics::m_overlappingPairCache = nullptr;
    std::unique_ptr<btSequentialImpulseConstraintSolver> Physics::m_solver = nullptr;
    std::unique_ptr<btDiscreteDynamicsWorld> Physics::m_dynamicsWorld = nullptr;

    void Physics::create()
    {
        m_collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
        m_dispatcher = std::make_unique<btCollisionDispatcher>(m_collisionConfiguration.get());
        m_overlappingPairCache = std::make_unique<btDbvtBroadphase>(); // make from subclass
        m_solver = std::make_unique<btSequentialImpulseConstraintSolver>();
        m_dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(m_dispatcher.get(), m_overlappingPairCache.get(), m_solver.get(), m_collisionConfiguration.get());

        m_dynamicsWorld->setGravity(m_gravity);

        // collisions call backs
        gContactAddedCallback = collisionsCallBack;
    }

    void Physics::simulate()
    {
        BR_ASSERT((m_dynamicsWorld != nullptr), "Create physics before simulate");

        // dont simulate if disabled or no objects
        // or time after m_timer.reset() is very short (for example we return from state (pause, ...) where simulation was disabled)
        if(!m_simulationEnabled || m_timer.elapsedSec() < 0.003f || m_dynamicsWorld->getNumCollisionObjects() == 0)
        {
            return;
        }

        m_collisionPairs.clear();

        // https://stackoverflow.com/questions/22825391/stepsimulation-parameters-in-bullet-physics
        // https://stackoverflow.com/questions/12778229/what-does-step-mean-in-stepsimulation-and-what-do-its-parameters-mean-in-bulle
        // timeStep: time in seconds passed since last m_world->stepSimulation() call
        // maxSubSteps: timeStep < maxSubSteps * fixedTimeStep
        // fixedTimeStep: simulation resolution increases as this value decreases.
        //                If your balls penetrates your walls instead of colliding with them decrease it
        m_timeStep = m_timer.elapsedSec();
        m_timer.reset();
        m_dynamicsWorld->stepSimulation(m_timeStep,
                                     m_resolutionFactor + 1,
                                     m_timeStep / m_resolutionFactor);
        //BR_INFO("Simulation objects count:{0}", m_dynamicsWorld->getNumCollisionObjects());
        //BR_INFO("Simulation time millisec:{0}", timer.elapsedMilliSec());
    }

    void Physics::addObject(const std::vector<glm::vec3>& vertices,
                            const std::vector<uint32_t>& indices,
                            const glm::mat4& transforms,
                            const std::string& meshName,
                            const std::string& objectID,
                            float mass,
                            bool wantCallBack,
                            CollisionFlags collFlag,
                            CollisionGroups collGroup,
                            CollisionGroups collMask)
    {
        BR_INFO("Physics::addObject name:{0}, ID:{1}", meshName, objectID);

        if(meshName.find("CollisionConcaveMesh") != std::string::npos)
        {
            addConcaveMesh(vertices, indices, transforms, objectID, mass, wantCallBack, collFlag, collGroup, collMask);
        }
        else if(meshName.find("CollisionConvexMesh") != std::string::npos)
        {
            addConvexMesh(vertices, indices, transforms, objectID, mass, wantCallBack, collFlag, collGroup, collMask);
        }
        else
        {
            BR_ASSERT(false, "Collision shape not supported:{0}", meshName);
        }
    }

    void Physics::addConcaveMesh(const std::vector<glm::vec3>& vertices,
                                 const std::vector<uint32_t>& indices,
                                 const glm::mat4& transforms,
                                 const std::string& objectID,
                                 float mass,
                                 bool wantCallBack,
                                 CollisionFlags collFlag,
                                 CollisionGroups collGroup,
                                 CollisionGroups collMask)
    {
        BR_ASSERT((mass == 0.0f), "ConcaveMesh can be only static or kinematic. means mass = 0.");
        BR_ASSERT((collFlag != CollisionFlags::DYNAMIC), "ConcaveMesh can be only static or kinematic.");

        glm::vec3 transl;
        glm::vec3 scale;
        glm::quat rot;
        Utils::Matrix::decompose4x4Glm(transforms, scale, rot, transl);

        // apply scale
        glm::vec3 vertex1;
        glm::vec3 vertex2;
        glm::vec3 vertex3;

        std::shared_ptr<btTriangleMesh> triangleMesh = std::make_shared<btTriangleMesh>();
        m_triangleMeshes.push_back(triangleMesh);
        triangleMesh->preallocateVertices(indices.size());

        for(int i = 0; i < indices.size(); )
        {
            vertex1 = scale * vertices[indices[i]];
            ++i;
            vertex2 = scale * vertices[indices[i]];
            ++i;
            vertex3 = scale * vertices[indices[i]];
            ++i;

            triangleMesh->addTriangle(btVector3(vertex1.x, vertex1.y, vertex1.z),
                                      btVector3(vertex2.x, vertex2.y, vertex2.z),
                                      btVector3(vertex3.x, vertex3.y, vertex3.z));
        }

        std::shared_ptr<btBvhTriangleMeshShape> shape = std::make_shared<btBvhTriangleMeshShape>(triangleMesh.get(), true);
        m_collisionShapes.push_back(shape);

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(transl.x, transl.y, transl.z));
        startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

        btVector3 localInertia(0, 0, 0);
        shape->calculateLocalInertia(mass, localInertia);

        std::shared_ptr<btDefaultMotionState> motionState = std::make_shared<btDefaultMotionState>(startTransform);
        m_motionStates.push_back(motionState);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState.get(), shape.get(), localInertia);
        std::shared_ptr<btRigidBody> body = std::make_shared<btRigidBody>(rbInfo);

        std::shared_ptr<RigidBodyData> rigidBodyData = std::make_shared<RigidBodyData>(objectID, body, true);
        body->setUserPointer(rigidBodyData.get()); // then we can fetch this rigidBodyData in collision call back

        if(collFlag == CollisionFlags::KINEMATIC)
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

        if(wantCallBack)
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

        m_rigidBodiesMap.insert(std::make_pair(objectID, rigidBodyData));

        m_dynamicsWorld->addRigidBody(body.get(), static_cast<int>(collGroup), static_cast<int>(collMask));
    }

    void Physics::addConvexMesh(const std::vector<glm::vec3>& vertices,
                                const std::vector<uint32_t>& indices,
                                const glm::mat4& transforms,
                                const std::string& objectID,
                                float mass,
                                bool wantCallBack,
                                CollisionFlags collFlag,
                                CollisionGroups collGroup,
                                CollisionGroups collMask)
    {
        BR_ASSERT(((mass == 0.0f && collFlag != CollisionFlags::DYNAMIC) ||
                   (mass > 0.0f && collFlag == CollisionFlags::DYNAMIC)), "Wrong parameters for convex mesh.");

        glm::vec3 transl;
        glm::vec3 scale;
        glm::quat rot;
        Utils::Matrix::decompose4x4Glm(transforms, scale, rot, transl);

        // btConvexHullShape should have less that 100 vertices for better performance
        std::shared_ptr<btConvexHullShape> shape = std::make_shared<btConvexHullShape>();
        m_collisionShapes.push_back(shape);

        for(int i = 0; i < indices.size(); ++i)
        {
            // apply scale
            shape->addPoint(btVector3(vertices[indices[i]].x * scale.x, vertices[indices[i]].y * scale.y, vertices[indices[i]].z * scale.z), false);
        }
        shape->recalcLocalAabb();

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(transl.x, transl.y, transl.z));
        startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

        btVector3 localInertia(0, 0, 0);
        shape->calculateLocalInertia(mass, localInertia);

        std::shared_ptr<btDefaultMotionState> motionState = std::make_shared<btDefaultMotionState>(startTransform);
        m_motionStates.push_back(motionState);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState.get(), shape.get(), localInertia);
        std::shared_ptr<btRigidBody> body = std::make_shared<btRigidBody>(rbInfo);

        std::shared_ptr<RigidBodyData> rigidBodyData = std::make_shared<RigidBodyData>(objectID, body, true);
        body->setUserPointer(rigidBodyData.get()); // then we can fetch this rigidBodyData in collision call back

        if(collFlag == CollisionFlags::KINEMATIC)
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

        if(wantCallBack)
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
        
        m_rigidBodiesMap.insert(std::make_pair(objectID, rigidBodyData));

        m_dynamicsWorld->addRigidBody(body.get(), static_cast<int>(collGroup), static_cast<int>(collMask));
    }

    bool Physics::collisionsCallBack(btManifoldPoint& cp, const btCollisionObjectWrapper* ob1, int id1, int index1,
                                                          const btCollisionObjectWrapper* ob2, int id2, int index2)
    {
        m_collisionPairs.emplace(static_cast<RigidBodyData*>(ob1->getCollisionObject()->getUserPointer())->bodyID,
                                 static_cast<RigidBodyData*>(ob2->getCollisionObject()->getUserPointer())->bodyID);
        return false;
    }

    bool Physics::getIsCollision(std::string_view id1, std::string_view id2)
    {
        if(m_collisionPairs.find(std::make_pair(id1, id2)) != m_collisionPairs.end()) { return true; }

        if(m_collisionPairs.find(std::make_pair(id2, id1)) != m_collisionPairs.end()) { return true; }

        return false;
    }

    void Physics::setTransforms(const std::string& ID, const glm::vec3& pos, const glm::quat& rot, bool resetVelocities)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            btTransform t;
            t.setOrigin(btVector3(pos.x, pos.y, pos.z));
            t.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

            iter->second->rb->setWorldTransform(t);
            iter->second->rb->getMotionState()->setWorldTransform(t);

            if(resetVelocities)
            {
                iter->second->rb->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
                iter->second->rb->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
                iter->second->rb->clearForces();
            }

            iter->second->rb->activate(true);

            //for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; --i)
            //{
            //    btRigidBody* body = btRigidBody::upcast(m_dynamicsWorld->getCollisionObjectArray()[i]);
            //    if (body && body->getActivationState() != ACTIVE_TAG)
            //        body->activate(true);
            //}
        }
        else
        {
            BR_ASSERT(false, "Can not set transforms for {0}", ID);
        }
    }

    PhysicsTransforms Physics::getTransforms(const std::string& ID)
    {
        PhysicsTransforms physicsTransforms;

        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            btTransform trans;

            if(iter->second->rb->getMotionState()) iter->second->rb->getMotionState()->getWorldTransform(trans);
            else trans = iter->second->rb->getWorldTransform();

            physicsTransforms.position = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
            physicsTransforms.rotation = glm::quat(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ());
        }
        else
        {
            BR_ASSERT(false, "Can not find transforms for {0}", ID);
        }

        return physicsTransforms;
    }

    void Physics::softRemoveObject(const std::string& ID)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end() && iter->second->existInDynamicWorld) // found object by id and it exist in world
        {
            m_dynamicsWorld->removeRigidBody(iter->second->rb.get());
            iter->second->existInDynamicWorld = false;
        }
    }

    void Physics::restoreObject(const std::string& ID, bool resetVelocities)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end() && !iter->second->existInDynamicWorld)
        {
            if(resetVelocities)
            {
                iter->second->rb->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
                iter->second->rb->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
                iter->second->rb->clearForces();
            }

            iter->second->rb->activate(true);
            m_dynamicsWorld->addRigidBody(iter->second->rb.get());
            iter->second->existInDynamicWorld = true;
        }
    }

    void Physics::setAngularFactor(const std::string& ID, const glm::vec3& angFactor)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            iter->second->rb->setAngularFactor(btVector3(angFactor.x, angFactor.y, angFactor.z));
        }
    }

    void Physics::setLinearFactor(const std::string& ID, const glm::vec3& linFactor)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            iter->second->rb->setLinearFactor(btVector3(linFactor.x, linFactor.y, linFactor.z));
        }
    }

    void Physics::setAngularVelocity(const std::string& ID, const glm::vec3& angVelocity)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            iter->second->rb->setAngularVelocity(btVector3(angVelocity.x, angVelocity.y, angVelocity.z));
        }
    }

    void Physics::setLinearVelocity(const std::string& ID, const glm::vec3& linVelocity)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            iter->second->rb->setLinearVelocity(btVector3(linVelocity.x, linVelocity.y, linVelocity.z));
        }
    }

    void Physics::setDefaultGravity(const glm::vec3& gravity)
    {
        m_gravity = btVector3(gravity.x, gravity.y, gravity.z);
        if(m_dynamicsWorld)
            m_dynamicsWorld->setGravity(m_gravity);
    }

    void Physics::setGravityForObject(const std::string& ID, const glm::vec3& gravity)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            iter->second->rb->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
        }
    }

    void Physics::disableGravityForObject(const std::string& ID)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            iter->second->rb->setGravity(btVector3(0.0f, 0.0f, 0.0f));
        }
    }

    void Physics::setDefaultGravityForObject(const std::string& ID)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            iter->second->rb->setGravity(m_gravity);
        }
    }

    RayClosestHit Physics::castRayClosestHit(const glm::vec3& from, const glm::vec3 to, CollisionGroups collGroup, CollisionGroups collMask)
    {
        btVector3 fr(from.x, from.y, from.z);
        btVector3 t(to.x, to.y, to.z);
        btCollisionWorld::ClosestRayResultCallback closestResults(fr, t);
        closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;
        closestResults.m_collisionFilterGroup = static_cast<int>(collGroup);
        closestResults.m_collisionFilterMask = static_cast<int>(collMask);

        m_dynamicsWorld->rayTest(fr, t, closestResults);

        if(closestResults.hasHit())
        {
            return RayClosestHit{true,
                                 static_cast<RigidBodyData*>(closestResults.m_collisionObject->getUserPointer())->bodyID,
                                 glm::vec3(closestResults.m_hitPointWorld.x(), closestResults.m_hitPointWorld.y(), closestResults.m_hitPointWorld.z()),
                                 glm::vec3(closestResults.m_hitNormalWorld.x(), closestResults.m_hitNormalWorld.y(), closestResults.m_hitNormalWorld.z()),
                                 closestResults.m_closestHitFraction
            };
        }

        return RayClosestHit{};
    }

    RayAllHits Physics::castRayAllHits(const glm::vec3& from, const glm::vec3 to, CollisionGroups collGroup, CollisionGroups collMask)
    {
        btVector3 fr(from.x, from.y, from.z);
        btVector3 t(to.x, to.y, to.z);
        btCollisionWorld::AllHitsRayResultCallback allResults(fr, t);
        allResults.m_flags |= btTriangleRaycastCallback::kF_KeepUnflippedNormal;
        allResults.m_flags |= btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;
        allResults.m_collisionFilterGroup = static_cast<int>(collGroup);
        allResults.m_collisionFilterMask = static_cast<int>(collMask);

        m_dynamicsWorld->rayTest(fr, t, allResults);

        if(allResults.hasHit())
        {
            RayAllHits res;
            res.hit = true;
            res.hitPoints.reserve(allResults.m_hitPointWorld.size());
            res.hitNormals.reserve(allResults.m_hitPointWorld.size());
            res.hitFractions.reserve(allResults.m_hitPointWorld.size());
            res.objectsID.reserve(allResults.m_hitPointWorld.size());

            for (int i = 0; i < allResults.m_hitPointWorld.size(); i++)
            {
                res.hitPoints.emplace_back(allResults.m_hitPointWorld[i].x(), allResults.m_hitPointWorld[i].y(), allResults.m_hitPointWorld[i].z());
                res.hitNormals.emplace_back(allResults.m_hitNormalWorld[i].x(), allResults.m_hitNormalWorld[i].y(), allResults.m_hitNormalWorld[i].z());
                res.hitFractions.emplace_back(allResults.m_hitFractions[i]);
                res.objectsID.emplace_back(static_cast<RigidBodyData*>(allResults.m_collisionObjects[i]->getUserPointer())->bodyID);
            }

            return res;
        }

        return RayAllHits{};
    }
}