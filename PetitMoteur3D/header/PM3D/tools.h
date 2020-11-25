#pragma once

#include <PxPhysicsAPI.h>
#include <vector>
#include <memory>

using namespace physx;

namespace PM3D {

	struct Scene {
		PxDefaultAllocator allocator_ = PxDefaultAllocator();
		PxDefaultErrorCallback errorCallback_ = PxDefaultErrorCallback();
		PxFoundation* foundation_ = NULL;
		PxDefaultCpuDispatcher* dispatcher_ = NULL;
		PxPhysics* physic_ = NULL;
		PxMaterial* material_ = NULL;
		PxPvd* pvd_ = NULL;
		PxScene* scene_ = NULL;
		std::vector<std::unique_ptr<CObjet3D>> ListeScene_{};
	};	
}