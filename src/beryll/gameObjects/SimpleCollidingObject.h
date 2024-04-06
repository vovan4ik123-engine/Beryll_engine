#pragma once

#include "BaseSimpleObject.h"

namespace Beryll
{
    // Not animated object, participates in physics simulation.
    class SimpleCollidingObject : public BaseSimpleObject
    {
    public:
        SimpleCollidingObject() = delete;
        /*
         * filePath - path to model file (.DAE or .FBX). start path from first folder inside assets/
         * collisionMassKg - mass of this object for physics simulation. 0 for static objects
         * wantCollisionCallBack - drop performance too much because call back use std::scoped_lock<std::mutex>
         *                         if true Physics module will store actual collisions for this object,
         *                         you can check it with Physics::getIsCollision(id1, id2) or Physics::getAllCollisions()
         * collFlag - type of collision object in physics world
         * collGroup - group or groups of current object in physics world
         * collMask - should contain collGroup or groups with which you want collisions
         * sceneGroup - game specific group to which this scene object belong
         */
        SimpleCollidingObject(const char* filePath,  // Common params.
                              float collisionMassKg,    // Physics params.
                              bool wantCollisionCallBack,
                              CollisionFlags collFlag,
                              CollisionGroups collGroup,
                              CollisionGroups collMask,
                              SceneObjectGroups sceneGroup);
        SimpleCollidingObject(const std::string& filePath,
                              const aiScene* scene,
                              const aiMesh* graphicsMesh,
                              const aiMesh* collisionMesh,
                              const std::string& collisionMeshName,
                              float collisionMassKg,
                              bool wantCollisionCallBack,
                              CollisionFlags collFlag,
                              CollisionGroups collGroup,
                              CollisionGroups collMask,
                              SceneObjectGroups sceneGroup);
        ~SimpleCollidingObject() override;

        // All loaded objects will have same parameters(mass, flags, groups, ...).
        static std::vector<std::shared_ptr<SimpleCollidingObject>> loadManyModelsFromOneFile(const char* filePath,
                                                                                             float collisionMassKg,
                                                                                             bool wantCollisionCallBack,
                                                                                             CollisionFlags collFlag,
                                                                                             CollisionGroups collGroup,
                                                                                             CollisionGroups collMask,
                                                                                             SceneObjectGroups sceneGroup);


    private:
        void loadCollisionMesh(const aiScene* scene,
                               const aiMesh* collisionMesh,
                               const std::string& meshName,
                               float mass,
                               bool wantCallBack,
                               CollisionFlags collFlag,
                               CollisionGroups collGroup,
                               CollisionGroups collMask);
    };
}
