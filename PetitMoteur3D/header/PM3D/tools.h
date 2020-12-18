#pragma once

#include <PxPhysicsAPI.h>
#include "objet3d.h"
#include <vector>
#include <memory>

namespace PM3D {

	struct Scene {
		physx::PxDefaultAllocator allocator_ = physx::PxDefaultAllocator();
		physx::PxDefaultErrorCallback errorCallback_ = physx::PxDefaultErrorCallback();

		physx::PxSimulationFilterShader filterShader = physx::PxDefaultSimulationFilterShader;
		physx::PxSimulationEventCallback* eventCallback = NULL;
		physx::PxContactModifyCallback* modifyCallback = NULL;

		physx::PxFoundation* foundation_ = NULL;
		physx::PxDefaultCpuDispatcher* dispatcher_ = NULL;
		physx::PxPhysics* physic_ = NULL;
		physx::PxMaterial* material_ = NULL;
		physx::PxPvd* pvd_ = NULL;
		physx::PxScene* scene_ = NULL;
		std::vector<std::unique_ptr<CObjet3D>> ListeScene_{};
		
	};	

}