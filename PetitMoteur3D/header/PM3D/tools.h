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

	struct Light_Manager{

		XMVECTOR vLumiere1;    // la position de la source d��clairage (Point)
		XMVECTOR vLumiere2;    // la position de la source d��clairage (Point)
		XMVECTOR vCamera;    // la position de la cam�ra
		XMVECTOR vAEcl;		// la valeur ambiante de l��clairage
		XMVECTOR vAMat;     // la valeur ambiante du mat�riau
		XMVECTOR vDEcl;     // la valeur diffuse de l��clairage
		XMVECTOR vDMat;     // la valeur diffuse du mat�riau
	};

}