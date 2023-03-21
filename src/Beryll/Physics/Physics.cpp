#include "Physics.h"
#include "Beryll/Core/Log.h"
#include "Beryll/Utils/Matrix.h"

namespace Beryll
{
    btVector3 Physics::m_gravity = btVector3(0.0f, -10.0f, 0.0f);
    Timer Physics::m_timer;
    float Physics::m_timeStep = 0.0f;
    bool Physics::m_simulationEnabled = true;
    float Physics::m_simulationTime = 0.0f;
    int Physics::m_resolutionFactor = 1;
    std::mutex Physics::m_mutex;
    std::vector<std::pair<const int, const int>> Physics::m_collisionPairs;

    std::vector<std::shared_ptr<btCollisionShape>> Physics::m_collisionShapes;
    std::vector<std::shared_ptr<btTriangleMesh>> Physics::m_triangleMeshes;
    std::vector<std::shared_ptr<btDefaultMotionState>> Physics::m_motionStates;
    std::map<const int, std::shared_ptr<RigidBodyData>> Physics::m_rigidBodiesMap;

    std::unique_ptr<btDefaultCollisionConfiguration> Physics::m_collisionConfiguration = nullptr;
    std::unique_ptr<btCollisionDispatcherMt> Physics::m_dispatcherMT = nullptr;
    std::unique_ptr<btDbvtBroadphase> Physics::m_broadPhase = nullptr;
    std::unique_ptr<btConstraintSolverPoolMt> Physics::m_solverPoolMT = nullptr;
    std::unique_ptr<btSequentialImpulseConstraintSolverMt> Physics::m_constraintSolverMT = nullptr;
    std::unique_ptr<btDiscreteDynamicsWorldMt> Physics::m_dynamicsWorldMT = nullptr;

    void Physics::create()
    {
        if(m_dynamicsWorldMT) { return; }

        btSetTaskScheduler(btCreateDefaultTaskScheduler());

        BR_INFO("Number of threads on device: %d", btGetTaskScheduler()->getNumThreads());

        btDefaultCollisionConstructionInfo cci;
        cci.m_defaultMaxPersistentManifoldPoolSize = 50000;
        cci.m_defaultMaxCollisionAlgorithmPoolSize = 50000;
        m_collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>(cci);
        m_dispatcherMT = std::make_unique<btCollisionDispatcherMt>(m_collisionConfiguration.get());
        m_broadPhase = std::make_unique<btDbvtBroadphase>();
        // let pool of solvers be 2 times more than available threads on device
        m_solverPoolMT = std::make_unique<btConstraintSolverPoolMt>(btGetTaskScheduler()->getNumThreads() * 2);
        m_constraintSolverMT = std::make_unique<btSequentialImpulseConstraintSolverMt>();
        m_dynamicsWorldMT = std::make_unique<btDiscreteDynamicsWorldMt>(m_dispatcherMT.get(),
                                                                        m_broadPhase.get(),
                                                                        m_solverPoolMT.get(),
                                                                        m_constraintSolverMT.get(),
                                                                        m_collisionConfiguration.get());

        m_dynamicsWorldMT->setGravity(m_gravity);
        m_dynamicsWorldMT->getSolverInfo().m_numIterations = 5;

        // set collisions call backs to bullet
        gContactAddedCallback = collisionsCallBack;

        m_collisionPairs.reserve(10000);
    }

    void Physics::simulate()
    {
        BR_ASSERT((m_dynamicsWorldMT != nullptr), "%s", "Create physics before simulate");

        // dont simulate if disabled or no objects
        // or time after m_timer.reset() is very short (for example we return from state (pause, ...) where simulation was disabled)
        if(!m_simulationEnabled || m_timer.elapsedSec() < 0.003f || m_dynamicsWorldMT->getNumCollisionObjects() == 0)
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
        m_timeStep = std::min(m_timer.elapsedSec(), 0.07f); // protection from lag (FPS dropped down and is < 14 FPS)
        m_timer.reset();
        m_dynamicsWorldMT->stepSimulation(m_timeStep,
                                     m_resolutionFactor + 1,
                                     m_timeStep / static_cast<float>(m_resolutionFactor));

        m_simulationTime = m_timer.elapsedMilliSec();
        //BR_INFO("Simulation objects count: %d", m_dynamicsWorldMT->getNumCollisionObjects());
        //BR_INFO("Simulation time millisec: %d", timer.elapsedMilliSec());
    }

    void Physics::addObject(const std::vector<glm::vec3>& vertices,
                            const std::vector<uint32_t>& indices,
                            const glm::mat4& transforms,
                            const std::string& meshName,
                            const int objectID,
                            float mass,
                            bool wantCallBack,
                            CollisionFlags collFlag,
                            CollisionGroups collGroup,
                            CollisionGroups collMask)
    {
        BR_INFO("Physics::addObject name: %s, mass: %f, ID: %d", meshName.c_str(), mass, objectID);

        if(meshName.find("CollisionConcaveMesh") != std::string::npos)
        {
            addConcaveMesh(vertices, indices, transforms, objectID, mass, wantCallBack, collFlag, collGroup, collMask);
        }
        else if(meshName.find("CollisionConvexMesh") != std::string::npos)
        {
            addConvexMesh(vertices, indices, transforms, objectID, mass, wantCallBack, collFlag, collGroup, collMask);
        }
        else if(meshName.find("CollisionBox") != std::string::npos)
        {
            addBoxShape(vertices, transforms, objectID, mass, wantCallBack, collFlag, collGroup, collMask);
        }
        else if(meshName.find("CollisionSphere") != std::string::npos)
        {
            addSphereShape(vertices, transforms, objectID, mass, wantCallBack, collFlag, collGroup, collMask);
        }
        else if(meshName.find("CollisionCapsule") != std::string::npos)
        {
            addCapsuleShape(vertices, transforms, objectID, mass, wantCallBack, collFlag, collGroup, collMask);
        }
        else
        {
            BR_ASSERT(false, "Collision shape not supported: %s", meshName.c_str());
        }
    }

    void Physics::addConcaveMesh(const std::vector<glm::vec3>& vertices,
                                 const std::vector<uint32_t>& indices,
                                 const glm::mat4& transforms,
                                 const int objectID,
                                 float mass,
                                 bool wantCallBack,
                                 CollisionFlags collFlag,
                                 CollisionGroups collGroup,
                                 CollisionGroups collMask)
    {
        BR_ASSERT((mass == 0.0f), "%s", "ConcaveMesh can be only static or kinematic. means mass = 0.");
        BR_ASSERT((collFlag != CollisionFlags::DYNAMIC), "%s", "ConcaveMesh can be only static or kinematic.");
        BR_ASSERT((vertices.empty() == false), "%s", "Vertices empty.")

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

        std::shared_ptr<btBvhTriangleMeshShape> shape = std::make_shared<btBvhTriangleMeshShape>(triangleMesh.get(), true, true);
        m_collisionShapes.push_back(shape);

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(transl.x, transl.y, transl.z));
        startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

        btVector3 localInertia(0, 0, 0);
        if(mass != 0.0f) shape->calculateLocalInertia(mass, localInertia);

        std::shared_ptr<btDefaultMotionState> motionState = std::make_shared<btDefaultMotionState>(startTransform);
        m_motionStates.push_back(motionState);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState.get(), shape.get(), localInertia);
        std::shared_ptr<btRigidBody> body = std::make_shared<btRigidBody>(rbInfo, objectID);

        std::shared_ptr<RigidBodyData> rigidBodyData = std::make_shared<RigidBodyData>(objectID, body, true, collGroup, collMask, collFlag, mass);
        body->setUserPointer(rigidBodyData.get()); // then we can fetch this rigidBodyData from CollisionObject ->getUserPointer()

        if(collFlag == CollisionFlags::KINEMATIC)
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

        if(wantCallBack)
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

        std::scoped_lock<std::mutex> lock (m_mutex);

        m_rigidBodiesMap.insert(std::make_pair(objectID, rigidBodyData));
        m_dynamicsWorldMT->addRigidBody(body.get(), static_cast<int>(collGroup), static_cast<int>(collMask));
    }

    void Physics::addConvexMesh(const std::vector<glm::vec3>& vertices,
                                const std::vector<uint32_t>& indices,
                                const glm::mat4& transforms,
                                const int objectID,
                                float mass,
                                bool wantCallBack,
                                CollisionFlags collFlag,
                                CollisionGroups collGroup,
                                CollisionGroups collMask)
    {
        BR_ASSERT(((mass == 0.0f && collFlag != CollisionFlags::DYNAMIC) ||
                   (mass > 0.0f && collFlag == CollisionFlags::DYNAMIC)), "%s", "Wrong parameters for convex mesh.");
        BR_ASSERT((vertices.empty() == false), "%s", "Vertices empty.")

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
        if(mass != 0.0f) shape->calculateLocalInertia(mass, localInertia);

        std::shared_ptr<btDefaultMotionState> motionState = std::make_shared<btDefaultMotionState>(startTransform);
        m_motionStates.push_back(motionState);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState.get(), shape.get(), localInertia);
        std::shared_ptr<btRigidBody> body = std::make_shared<btRigidBody>(rbInfo, objectID);

        std::shared_ptr<RigidBodyData> rigidBodyData = std::make_shared<RigidBodyData>(objectID, body, true, collGroup, collMask, collFlag, mass);
        body->setUserPointer(rigidBodyData.get()); // then we can fetch this rigidBodyData from CollisionObject ->getUserPointer()

        if(collFlag == CollisionFlags::KINEMATIC)
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

        if(wantCallBack)
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

        std::scoped_lock<std::mutex> lock (m_mutex);

        m_rigidBodiesMap.insert(std::make_pair(objectID, rigidBodyData));
        m_dynamicsWorldMT->addRigidBody(body.get(), static_cast<int>(collGroup), static_cast<int>(collMask));
    }

    void Physics::addBoxShape(const std::vector<glm::vec3>& vertices,
                              const glm::mat4& transforms,
                              const int objectID,
                              float mass,
                              bool wantCallBack,
                              CollisionFlags collFlag,
                              CollisionGroups collGroup,
                              CollisionGroups collMask)
    {
        BR_ASSERT(((mass == 0.0f && collFlag != CollisionFlags::DYNAMIC) ||
                   (mass > 0.0f && collFlag == CollisionFlags::DYNAMIC)), "%s", "Wrong parameters for box shape.");
        BR_ASSERT((vertices.empty() == false), "%s", "Vertices empty.")

        glm::vec3 transl;
        glm::vec3 scale;
        glm::quat rot;
        Utils::Matrix::decompose4x4Glm(transforms, scale, rot, transl);

        float bottomX = 0.0f;
        float topX = 0.0f;
        float bottomY = 0.0f;
        float topY = 0.0f;
        float bottomZ = 0.0f;
        float topZ = 0.0f;
        for(const glm::vec3& vert : vertices)
        {
            glm::vec3 v = glm::vec3(vert.x * scale.x, vert.y * scale.y, vert.z * scale.z);

            if(v.x < bottomX) bottomX = v.x;
            if(v.x > topX) topX = v.x;

            if(v.y < bottomY) bottomY = v.y;
            if(v.y > topY) topY = v.y;

            if(v.z < bottomZ) bottomZ = v.z;
            if(v.z > topZ) topZ = v.z;
        }

        float Xsize = topX - bottomX;
        float Ysize = topY - bottomY;
        float Zsize = topZ - bottomZ;

        std::shared_ptr<btBoxShape> boxShape = std::make_shared<btBoxShape>(btVector3(Xsize / 2, Ysize / 2, Zsize / 2));
        m_collisionShapes.push_back(boxShape);

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(transl.x, transl.y, transl.z));
        startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

        btVector3 localInertia(0, 0, 0);
        if(mass != 0.0f) boxShape->calculateLocalInertia(mass, localInertia);

        std::shared_ptr<btDefaultMotionState> motionState = std::make_shared<btDefaultMotionState>(startTransform);
        m_motionStates.push_back(motionState);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState.get(), boxShape.get(), localInertia);
        std::shared_ptr<btRigidBody> body = std::make_shared<btRigidBody>(rbInfo, objectID);

        std::shared_ptr<RigidBodyData> rigidBodyData = std::make_shared<RigidBodyData>(objectID, body, true, collGroup, collMask, collFlag, mass);
        body->setUserPointer(rigidBodyData.get()); // then we can fetch this rigidBodyData from CollisionObject ->getUserPointer()

        if(collFlag == CollisionFlags::KINEMATIC)
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

        if(wantCallBack)
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

        std::scoped_lock<std::mutex> lock (m_mutex);

        m_rigidBodiesMap.insert(std::make_pair(objectID, rigidBodyData));
        m_dynamicsWorldMT->addRigidBody(body.get(), static_cast<int>(collGroup), static_cast<int>(collMask));
    }

    void Physics::addSphereShape(const std::vector<glm::vec3>& vertices,
                        const glm::mat4& transforms,
                        const int objectID,
                        float mass,
                        bool wantCallBack,
                        CollisionFlags collFlag,
                        CollisionGroups collGroup,
                        CollisionGroups collMask)
    {
        BR_ASSERT(((mass == 0.0f && collFlag != CollisionFlags::DYNAMIC) ||
                   (mass > 0.0f && collFlag == CollisionFlags::DYNAMIC)), "%s", "Wrong parameters for sphere shape.");
        BR_ASSERT((vertices.empty() == false), "%s", "Vertices empty.")

        glm::vec3 transl;
        glm::vec3 scale;
        glm::quat rot;
        Utils::Matrix::decompose4x4Glm(transforms, scale, rot, transl);

        glm::vec3 point = glm::vec3(vertices[0].x * scale.x, vertices[0].y * scale.y, vertices[0].z * scale.z);
        float radius = glm::distance(glm::vec3(0.0f, 0.0f, 0.0f), point);

        std::shared_ptr<btSphereShape> sphereShape = std::make_shared<btSphereShape>(radius);
        m_collisionShapes.push_back(sphereShape);

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(transl.x, transl.y, transl.z));
        startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

        btVector3 localInertia(0, 0, 0);
        if(mass != 0.0f) sphereShape->calculateLocalInertia(mass, localInertia);

        std::shared_ptr<btDefaultMotionState> motionState = std::make_shared<btDefaultMotionState>(startTransform);
        m_motionStates.push_back(motionState);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState.get(), sphereShape.get(), localInertia);
        std::shared_ptr<btRigidBody> body = std::make_shared<btRigidBody>(rbInfo, objectID);

        std::shared_ptr<RigidBodyData> rigidBodyData = std::make_shared<RigidBodyData>(objectID, body, true, collGroup, collMask, collFlag, mass);
        body->setUserPointer(rigidBodyData.get()); // then we can fetch this rigidBodyData from CollisionObject ->getUserPointer()

        if(collFlag == CollisionFlags::KINEMATIC)
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

        if(wantCallBack)
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

        std::scoped_lock<std::mutex> lock (m_mutex);

        m_rigidBodiesMap.insert(std::make_pair(objectID, rigidBodyData));
        m_dynamicsWorldMT->addRigidBody(body.get(), static_cast<int>(collGroup), static_cast<int>(collMask));
    }

    void Physics::addCapsuleShape(const std::vector<glm::vec3>& vertices,
                                const glm::mat4& transforms,
                                const int objectID,
                                float mass,
                                bool wantCallBack,
                                CollisionFlags collFlag,
                                CollisionGroups collGroup,
                                CollisionGroups collMask)
    {
        BR_ASSERT(((mass == 0.0f && collFlag != CollisionFlags::DYNAMIC) ||
                   (mass > 0.0f && collFlag == CollisionFlags::DYNAMIC)), "%s", "Wrong parameters for capsule shape.");
        BR_ASSERT((vertices.empty() == false), "%s", "Vertices empty.")

        // original capsule position should be around the Y axis

        glm::vec3 transl;
        glm::vec3 scale;
        glm::quat rot;
        Utils::Matrix::decompose4x4Glm(transforms, scale, rot, transl);

        float bottomX = 0.0f;
        float topX = 0.0f;
        float bottomY = 0.0f;
        float topY = 0.0f;
        for(const glm::vec3& vert : vertices)
        {
            glm::vec3 v = glm::vec3(vert.x * scale.x, vert.y * scale.y, vert.z * scale.z);

            if(v.x < bottomX) bottomX = v.x;
            if(v.x > topX) topX = v.x;

            if(v.y < bottomY) bottomY = v.y;
            if(v.y > topY) topY = v.y;
        }

        float radius = (topX - bottomX) / 2.0f;
        float totalHeight = topY - bottomY;
        float heightNoRadiuses = totalHeight - radius * 2.0f;

        std::shared_ptr<btCapsuleShape> capsuleShape = std::make_shared<btCapsuleShape>(radius, heightNoRadiuses);
        m_collisionShapes.push_back(capsuleShape);

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(transl.x, transl.y, transl.z));
        startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

        btVector3 localInertia(0, 0, 0);
        if(mass != 0.0f) capsuleShape->calculateLocalInertia(mass, localInertia);

        std::shared_ptr<btDefaultMotionState> motionState = std::make_shared<btDefaultMotionState>(startTransform);
        m_motionStates.push_back(motionState);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState.get(), capsuleShape.get(), localInertia);
        std::shared_ptr<btRigidBody> body = std::make_shared<btRigidBody>(rbInfo, objectID);

        std::shared_ptr<RigidBodyData> rigidBodyData = std::make_shared<RigidBodyData>(objectID, body, true, collGroup, collMask, collFlag, mass);
        body->setUserPointer(rigidBodyData.get()); // then we can fetch this rigidBodyData from CollisionObject ->getUserPointer()

        if(collFlag == CollisionFlags::KINEMATIC)
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

        if(wantCallBack)
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

        std::scoped_lock<std::mutex> lock (m_mutex);

        m_rigidBodiesMap.insert(std::make_pair(objectID, rigidBodyData));
        m_dynamicsWorldMT->addRigidBody(body.get(), static_cast<int>(collGroup), static_cast<int>(collMask));
    }

    // called from MANY threads !!!!!
    bool Physics::collisionsCallBack(btManifoldPoint& cp, const btCollisionObjectWrapper* ob1, int ID1, int index1,
                                                          const btCollisionObjectWrapper* ob2, int ID2, int index2)
    {
        {
            std::scoped_lock<std::mutex> lock (m_mutex);

            m_collisionPairs.emplace_back(ob1->getCollisionObject()->beryllEngineObjectID,
                                          ob2->getCollisionObject()->beryllEngineObjectID);
        }

        return false;
    }

    bool Physics::getIsCollision(const int ID1, const int ID2)
    {
        if(ID1 == ID2) { return false; }

        for(const std::pair<const int, const int>& pair : m_collisionPairs)
        {
            if((pair.first == ID1 && pair.second == ID2) ||
               (pair.first == ID2 && pair.second == ID1))
            {
                return true;
            }
        }

        return false;
    }

    bool Physics::getIsCollision(const int ID, const CollisionGroups group)
    {
        for(int i = 0; i < m_dynamicsWorldMT->getNumCollisionObjects(); ++i)
        {
            if(m_dynamicsWorldMT->getCollisionObjectArray()[i]->getBroadphaseHandle()->m_collisionFilterGroup & static_cast<int>(group))
            {
                if(getIsCollision(ID, m_dynamicsWorldMT->getCollisionObjectArray()[i]->beryllEngineObjectID))
                {
                    return true;
                }
            }
        }

        return false;
    }

    std::vector<int> Physics::getCollisionsWithGroup(const int ID, const CollisionGroups group)
    {
        std::vector<int> ids;
        ids.reserve(3);

        for(int i = 0; i < m_dynamicsWorldMT->getNumCollisionObjects(); ++i)
        {
            if(m_dynamicsWorldMT->getCollisionObjectArray()[i]->getBroadphaseHandle()->m_collisionFilterGroup & static_cast<int>(group))
            {
                if(getIsCollision(ID, m_dynamicsWorldMT->getCollisionObjectArray()[i]->beryllEngineObjectID))
                {
                    ids.push_back(m_dynamicsWorldMT->getCollisionObjectArray()[i]->beryllEngineObjectID);
                }
            }
        }

        return std::move(ids);
    }

    std::vector<std::pair<glm::vec3, glm::vec3>> Physics::getAllCollisionPoints(const int ID1, const int ID2)
    {
        std::vector<std::pair<glm::vec3, glm::vec3>> pointsAndNormals;

        if(ID1 == ID2) { return std::move(pointsAndNormals); }

        pointsAndNormals.reserve(5);

        int numManifolds = m_dynamicsWorldMT->getDispatcher()->getNumManifolds();
        for(int i = 0; i < numManifolds; i++)
        {
            btPersistentManifold* contactManifold = m_dynamicsWorldMT->getDispatcher()->getManifoldByIndexInternal(i);
            const btCollisionObject* obA = contactManifold->getBody0();
            const btCollisionObject* obB = contactManifold->getBody1();

            if((obA->beryllEngineObjectID == ID1 && obB->beryllEngineObjectID == ID2) ||
               (obA->beryllEngineObjectID == ID2 && obB->beryllEngineObjectID == ID1))
            {
                // we found contact point between 2 objects
                for(int j = 0; j < contactManifold->getNumContacts(); j++)
                {
                    const btManifoldPoint& pt = contactManifold->getContactPoint(j);

                    const btVector3& ptB = pt.getPositionWorldOnB();
                    btVector3 normalOnB = pt.m_normalWorldOnB;
                    normalOnB.normalize();

                    pointsAndNormals.emplace_back(glm::vec3(ptB.getX(), ptB.getY(), ptB.getZ()), // point
                                                  glm::normalize(glm::vec3(normalOnB.getX(), normalOnB.getY(), normalOnB.getZ()))); // normal
                }
            }
        }

        return std::move(pointsAndNormals);
    }

    std::vector<std::pair<glm::vec3, glm::vec3>> Physics::getAllCollisionPoints(const int ID1, const std::vector<int>& IDs)
    {
        std::vector<std::pair<glm::vec3, glm::vec3>> pointsAndNormals;
        pointsAndNormals.reserve(5);

        int numManifolds = m_dynamicsWorldMT->getDispatcher()->getNumManifolds();
        for(int i = 0; i < numManifolds; i++)
        {
            btPersistentManifold* contactManifold = m_dynamicsWorldMT->getDispatcher()->getManifoldByIndexInternal(i);
            const btCollisionObject* obA = contactManifold->getBody0();
            const btCollisionObject* obB = contactManifold->getBody1();

            bool obA_ID_existInIDs = false;
            bool obB_ID_existInIDs = false;
            for(const int id : IDs)
            {
                if(id == obA->beryllEngineObjectID)
                {
                    obA_ID_existInIDs = true;
                    break;
                }

                if(id == obB->beryllEngineObjectID)
                {
                    obB_ID_existInIDs = true;
                    break;
                }
            }

            if((obA->beryllEngineObjectID == ID1 && obB_ID_existInIDs) ||
               (obB->beryllEngineObjectID == ID1 && obA_ID_existInIDs))
            {
                // we found collision between 2 objects
                for(int j = 0; j < contactManifold->getNumContacts(); j++)
                {
                    const btManifoldPoint& pt = contactManifold->getContactPoint(j);

                    const btVector3& ptB = pt.getPositionWorldOnB();
                    btVector3 normalOnB = pt.m_normalWorldOnB;
                    normalOnB.normalize();

                    pointsAndNormals.emplace_back(glm::vec3(ptB.getX(), ptB.getY(), ptB.getZ()), // point
                                                  glm::normalize(glm::vec3(normalOnB.getX(), normalOnB.getY(), normalOnB.getZ()))); // normal
                }
            }
        }

        return std::move(pointsAndNormals);
    }

    void Physics::setOrigin(const int ID, const glm::vec3& orig, bool resetVelocities)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            std::scoped_lock<std::mutex> lock (m_mutex);

            btTransform t;
            if(iter->second->rb->getMotionState())
                iter->second->rb->getMotionState()->getWorldTransform(t);
            else
                t = iter->second->rb->getWorldTransform();

            t.setOrigin(btVector3(orig.x, orig.y, orig.z));

            iter->second->rb->setWorldTransform(t);
            if(iter->second->rb->getMotionState())
                iter->second->rb->getMotionState()->setWorldTransform(t);

            resetVelocitiesForObject(iter->second->rb, resetVelocities);

            iter->second->rb->activate(true);

            //for(int i = m_dynamicsWorldMT->getNumCollisionObjects() - 1; i >= 0; --i)
            //{
            //    btRigidBody* body = btRigidBody::upcast(m_dynamicsWorldMT->getCollisionObjectArray()[i]);
            //    if (body && body->getActivationState() != ACTIVE_TAG)
            //        body->activate(true);
            //}
        }
        else
        {
            BR_ASSERT(false, "Can not set origin for ID: %d", ID);
        }
    }

    void Physics::addToOrigin(const int ID, const glm::vec3& dist, bool resetVelocities)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            std::scoped_lock<std::mutex> lock (m_mutex);

            btTransform t;
            if(iter->second->rb->getMotionState())
                iter->second->rb->getMotionState()->getWorldTransform(t);
            else
                t = iter->second->rb->getWorldTransform();

            t.getOrigin() += btVector3(dist.x, dist.y, dist.z); // add to reference

            iter->second->rb->setWorldTransform(t);
            if(iter->second->rb->getMotionState())
                iter->second->rb->getMotionState()->setWorldTransform(t);

            resetVelocitiesForObject(iter->second->rb, resetVelocities);

            iter->second->rb->activate(true);
        }
        else
        {
            BR_ASSERT(false, "Can not add to origin for ID: %d", ID);
        }
    }

    void Physics::addToRotation(const int ID, const glm::quat& rot, bool resetVelocities)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            std::scoped_lock<std::mutex> lock (m_mutex);

            btTransform t;
            if(iter->second->rb->getMotionState())
                iter->second->rb->getMotionState()->getWorldTransform(t);
            else
                t = iter->second->rb->getWorldTransform();

            btQuaternion originalRotation = t.getRotation();
            // rotations will be combined from right to left(originalRotation first, then btQuaternion(.....)
            t.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w) * originalRotation);

            iter->second->rb->setWorldTransform(t);
            if(iter->second->rb->getMotionState())
                iter->second->rb->getMotionState()->setWorldTransform(t);

            resetVelocitiesForObject(iter->second->rb, resetVelocities);

            iter->second->rb->activate(true);
        }
        else
        {
            BR_ASSERT(false, "Can not set rotation for ID: %d", ID);
        }
    }

    PhysicsTransforms Physics::getTransforms(const int ID)
    {
        PhysicsTransforms physicsTransforms;

        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            btTransform t;

            {
                std::scoped_lock<std::mutex> lock (m_mutex);

                if(iter->second->rb->getMotionState())
                    iter->second->rb->getMotionState()->getWorldTransform(t);
                else
                    t = iter->second->rb->getWorldTransform();
            }

            physicsTransforms.origin = glm::vec3(t.getOrigin().getX(), t.getOrigin().getY(), t.getOrigin().getZ());
            physicsTransforms.rotation = glm::quat(t.getRotation().getW(), t.getRotation().getX(), t.getRotation().getY(), t.getRotation().getZ());
        }
        else
        {
            BR_ASSERT(false, "Can not find transforms for ID: %d", ID);
        }

        return physicsTransforms;
    }

    void Physics::softRemoveObject(const int ID)
    {
        auto iter = m_rigidBodiesMap.find(ID);

        std::scoped_lock<std::mutex> lock (m_mutex);

        if(iter != m_rigidBodiesMap.end() && iter->second->existInDynamicWorld) // found object by ID and it exist in world
        {
            m_dynamicsWorldMT->removeRigidBody(iter->second->rb.get());
            iter->second->existInDynamicWorld = false;
        }
    }

    void Physics::restoreObject(const int ID, bool resetVelocities)
    {
        auto iter = m_rigidBodiesMap.find(ID);

        std::scoped_lock<std::mutex> lock (m_mutex);

        if(iter != m_rigidBodiesMap.end() && !iter->second->existInDynamicWorld)
        {
            resetVelocitiesForObject(iter->second->rb, resetVelocities);

            iter->second->rb->activate(true);

            m_dynamicsWorldMT->addRigidBody(iter->second->rb.get(),
                                            static_cast<int>(iter->second->collGroup),
                                            static_cast<int>(iter->second->collMask));
            iter->second->existInDynamicWorld = true;
        }
    }

    void Physics::hardRemoveAllObjects()
    {
        m_collisionPairs.clear();

        BR_INFO("Count before hard delete: %d", m_dynamicsWorldMT->getCollisionObjectArray().size());
        // in reverse order
        while(!m_motionStates.empty())
        {
            m_motionStates.pop_back();
        }

        // in reverse order
        for(auto revIter = m_rigidBodiesMap.rbegin(); revIter != m_rigidBodiesMap.rend(); ++revIter)
        {
            if(revIter->second->existInDynamicWorld)
            {
                m_dynamicsWorldMT->removeRigidBody(revIter->second->rb.get());
                revIter->second->existInDynamicWorld = false;
            }
        }

        m_rigidBodiesMap.clear();
        m_collisionShapes.clear();
        m_triangleMeshes.clear();

        BR_INFO("Count after hard delete: %d", m_dynamicsWorldMT->getCollisionObjectArray().size());
    }

    void Physics::activateObject(const int ID)
    {
        auto iter = m_rigidBodiesMap.find(ID);

        std::scoped_lock<std::mutex> lock (m_mutex);

        if(iter != m_rigidBodiesMap.end() && iter->second->existInDynamicWorld) // found object by ID and it exist in world
        {
            iter->second->rb->activate(true);
        }
    }

    bool Physics::getIsObjectActive(const int ID)
    {
        auto iter = m_rigidBodiesMap.find(ID);

        std::scoped_lock<std::mutex> lock (m_mutex);

        if(iter != m_rigidBodiesMap.end() && iter->second->existInDynamicWorld) // found object by ID and it exist in world
        {
            int activationState = iter->second->rb->getActivationState();

            if(activationState == ACTIVE_TAG ||
               activationState == WANTS_DEACTIVATION ||
               activationState == DISABLE_DEACTIVATION)
            {
                return true;
            }

            return false;
        }
        else
        {
            BR_ASSERT(false, "Object: %d not in simulation", ID);
        }

        return false;
    }

    void Physics::setAngularFactor(const int ID, const glm::vec3& angFactor, bool resetVelocities)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            std::scoped_lock<std::mutex> lock (m_mutex);

            iter->second->rb->setAngularFactor(btVector3(angFactor.x, angFactor.y, angFactor.z));

            resetVelocitiesForObject(iter->second->rb, resetVelocities);
        }
    }

    void Physics::setLinearFactor(const int ID, const glm::vec3& linFactor, bool resetVelocities)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            std::scoped_lock<std::mutex> lock (m_mutex);

            iter->second->rb->setLinearFactor(btVector3(linFactor.x, linFactor.y, linFactor.z));

            resetVelocitiesForObject(iter->second->rb, resetVelocities);
        }
    }

    void Physics::setAngularVelocity(const int ID, const glm::vec3& angVelocity, bool resetVelocities)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            std::scoped_lock<std::mutex> lock (m_mutex);

            iter->second->rb->setAngularVelocity(btVector3(angVelocity.x, angVelocity.y, angVelocity.z));

            resetVelocitiesForObject(iter->second->rb, resetVelocities);
        }
    }

    void Physics::setLinearVelocity(const int ID, const glm::vec3& linVelocity, bool resetVelocities)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            std::scoped_lock<std::mutex> lock (m_mutex);

            iter->second->rb->setLinearVelocity(btVector3(linVelocity.x, linVelocity.y, linVelocity.z));

            resetVelocitiesForObject(iter->second->rb, resetVelocities);
        }
    }

    void Physics::setGravityForAllWorld(const glm::vec3& gravity)
    {
        std::scoped_lock<std::mutex> lock (m_mutex);

        m_gravity = btVector3(gravity.x, gravity.y, gravity.z);
        if(m_dynamicsWorldMT)
            m_dynamicsWorldMT->setGravity(m_gravity);
    }

    void Physics::setGravityForObject(const int ID, const glm::vec3& gravity, bool resetVelocities)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            std::scoped_lock<std::mutex> lock (m_mutex);

            iter->second->rb->setGravity(btVector3(gravity.x, gravity.y, gravity.z));

            resetVelocitiesForObject(iter->second->rb, resetVelocities);
        }
    }

    void Physics::disableGravityForObject(const int ID, bool resetVelocities)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            std::scoped_lock<std::mutex> lock (m_mutex);

            iter->second->rb->setGravity(btVector3(0.0f, 0.0f, 0.0f));

            resetVelocitiesForObject(iter->second->rb, resetVelocities);
        }
    }

    void Physics::enableDefaultGravityForObject(const int ID, bool resetVelocities)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            std::scoped_lock<std::mutex> lock (m_mutex);

            iter->second->rb->setGravity(m_gravity);

            resetVelocitiesForObject(iter->second->rb, resetVelocities);
        }
    }

    // should be thread safe without mutex
    RayClosestHit Physics::castRayClosestHit(const glm::vec3& from, const glm::vec3 to, CollisionGroups collGroup, CollisionGroups collMask)
    {
        btVector3 fr(from.x, from.y, from.z);
        btVector3 t(to.x, to.y, to.z);
        btCollisionWorld::ClosestRayResultCallback closestResults(fr, t);
        closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;
        closestResults.m_flags |= btTriangleRaycastCallback::kF_UseGjkConvexCastRaytest;
        closestResults.m_collisionFilterGroup = static_cast<int>(collGroup);
        closestResults.m_collisionFilterMask = static_cast<int>(collMask);

        m_dynamicsWorldMT->rayTest(fr, t, closestResults);

        if(closestResults.hasHit())
        {
            return RayClosestHit{true,
                                 static_cast<RigidBodyData*>(closestResults.m_collisionObject->getUserPointer())->bodyID,
                                 static_cast<RigidBodyData*>(closestResults.m_collisionObject->getUserPointer())->collFlag,
                                 static_cast<RigidBodyData*>(closestResults.m_collisionObject->getUserPointer())->mass,
                                 glm::vec3(closestResults.m_hitPointWorld.x(), closestResults.m_hitPointWorld.y(), closestResults.m_hitPointWorld.z()),
                                 glm::vec3(closestResults.m_hitNormalWorld.x(), closestResults.m_hitNormalWorld.y(), closestResults.m_hitNormalWorld.z()),
                                 closestResults.m_closestHitFraction
            };
        }

        return RayClosestHit{};
    }

    // should be thread safe without mutex
    RayAllHits Physics::castRayAllHits(const glm::vec3& from, const glm::vec3 to, CollisionGroups collGroup, CollisionGroups collMask)
    {
        btVector3 fr(from.x, from.y, from.z);
        btVector3 t(to.x, to.y, to.z);
        btCollisionWorld::AllHitsRayResultCallback allResults(fr, t);
        allResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;
        allResults.m_flags |= btTriangleRaycastCallback::kF_UseGjkConvexCastRaytest;
        allResults.m_collisionFilterGroup = static_cast<int>(collGroup);
        allResults.m_collisionFilterMask = static_cast<int>(collMask);

        m_dynamicsWorldMT->rayTest(fr, t, allResults);

        if(allResults.hasHit())
        {
            RayAllHits res;
            res.hit = true;
            res.hitPoints.reserve(allResults.m_hitPointWorld.size());
            res.hitNormals.reserve(allResults.m_hitPointWorld.size());
            res.hitFractions.reserve(allResults.m_hitPointWorld.size());
            res.objectsID.reserve(allResults.m_hitPointWorld.size());

            for(int i = 0; i < allResults.m_hitPointWorld.size(); i++)
            {
                res.hitPoints.emplace_back(allResults.m_hitPointWorld[i].x(), allResults.m_hitPointWorld[i].y(), allResults.m_hitPointWorld[i].z());
                res.hitNormals.emplace_back(allResults.m_hitNormalWorld[i].x(), allResults.m_hitNormalWorld[i].y(), allResults.m_hitNormalWorld[i].z());
                res.hitFractions.emplace_back(allResults.m_hitFractions[i]);
                res.objectsID.emplace_back(static_cast<RigidBodyData*>(allResults.m_collisionObjects[i]->getUserPointer())->bodyID);
                res.objectsCollFlags.emplace_back(static_cast<RigidBodyData*>(allResults.m_collisionObjects[i]->getUserPointer())->collFlag);
                res.objectsMass.emplace_back(static_cast<RigidBodyData*>(allResults.m_collisionObjects[i]->getUserPointer())->mass);
            }

            return res;
        }

        return RayAllHits{};
    }

    void Physics::resetVelocitiesForObject(const std::shared_ptr<btRigidBody>& b, bool reset)
    {
        if(!reset) { return; }

        b->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
        b->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
        b->clearForces();
    }

    void Physics::resetVelocitiesForObject(const int ID)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            std::scoped_lock<std::mutex> lock (m_mutex);

            iter->second->rb->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
            iter->second->rb->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
            iter->second->rb->clearForces();
        }
    }

    void Physics::applyCentralImpulseForObject(const int ID, const glm::vec3& impulse)
    {
        auto iter = m_rigidBodiesMap.find(ID);
        if(iter != m_rigidBodiesMap.end())
        {
            std::scoped_lock<std::mutex> lock (m_mutex);

            iter->second->rb->activate(true);
            iter->second->rb->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
        }
    }
}