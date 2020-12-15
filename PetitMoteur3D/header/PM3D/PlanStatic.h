#pragma once
#include "Objet3DStatic.h"
#include "PxPhysicsAPI.h"
#include "tools.h"
#include "dispositifD3D11.h"

namespace PM3D {

    class PlanStatic :
        public Objet3DStatic
    {
    public:
        PlanStatic(Scene* _scene, physx::PxVec3 _point, physx::PxVec3 _normale, physx::PxVec3 _direction);
        void Draw() noexcept override {};
        physx::PxTransform getTerrainNormale();
        physx::PxVec3 getDirection();
        physx::PxVec3 getNormale();
    protected:
        static physx::PxRigidStatic* createRigidBody(Scene* _scene, physx::PxVec3 _point, physx::PxVec3 _normale);
        physx::PxVec3 normale_;
        physx::PxVec3 direction_;
    };
}