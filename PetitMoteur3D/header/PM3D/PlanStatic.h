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
        PlanStatic(Scene* _scene, PxVec3 _point, PxVec3 _normale);
        void Draw() {};
    protected:
        static PxRigidStatic* createRigidBody(Scene* _scene, PxVec3 _point, PxVec3 _normale);
    };
}