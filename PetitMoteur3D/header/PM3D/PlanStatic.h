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
        PlanStatic(Scene* _scene, physx::PxVec3 _point, physx::PxVec3 _normale);
        void Draw() noexcept override {};
        physx::PxTransform getTerrainNormale();
        physx::PxVec3 getDirection();
        physx::PxVec3 getNormale();
        physx::PxVec3 getPointPlan(physx::PxVec3 _point);
    protected:
        static physx::PxRigidStatic* createRigidBody(Scene* _scene, physx::PxVec3 _point, physx::PxVec3 _normale);
    };
}