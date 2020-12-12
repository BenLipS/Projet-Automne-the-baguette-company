#pragma once
#include "Objet3DStatic.h"
#include "PxPhysicsAPI.h"
#include "tools.h"
#include "dispositifD3D11.h"

using namespace physx;

namespace PM3D {

    class PlanStatic :
        public Objet3DStatic
    {
    public:
        PlanStatic(Scene* _scene, PxVec3 _point, PxVec3 _normale, PxVec3 _direction);
        void Draw() noexcept override {};
        PxTransform getTerrainNormale();
        PxVec3 getDirection();
        PxVec3 getNormale();
    protected:
        static PxRigidStatic* createRigidBody(Scene* _scene, PxVec3 _point, PxVec3 _normale);
        PxVec3 normale_;
        PxVec3 direction_;
    };
}