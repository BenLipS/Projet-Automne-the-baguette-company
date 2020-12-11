#pragma once

#include <PxPhysicsAPI.h>
#include "objet3d.h"
#include <vector>
#include <memory>

using namespace physx;

namespace PM3D {

	struct Scene {
		PxDefaultAllocator allocator_ = PxDefaultAllocator();
		PxDefaultErrorCallback errorCallback_ = PxDefaultErrorCallback();

		PxSimulationFilterShader filterShader;
		PxSimulationEventCallback* eventCallback;
		PxContactModifyCallback* modifyCallback;

		PxFoundation* foundation_ = NULL;
		PxDefaultCpuDispatcher* dispatcher_ = NULL;
		PxPhysics* physic_ = NULL;
		PxMaterial* material_ = NULL;
		PxPvd* pvd_ = NULL;
		PxScene* scene_ = NULL;
		std::vector<std::unique_ptr<CObjet3D>> ListeScene_{};
		
	};	

	struct Light_Manager{

		DirectX::XMVECTOR vLumiere1;    // la position de la source d��clairage (Point)
		DirectX::XMVECTOR vLumiere2;    // la position de la source d��clairage (Point)
		DirectX::XMVECTOR vCamera;    // la position de la cam�ra
		DirectX::XMVECTOR vAEcl;		// la valeur ambiante de l��clairage
		DirectX::XMVECTOR vAMat;     // la valeur ambiante du mat�riau
		DirectX::XMVECTOR vDEcl;     // la valeur diffuse de l��clairage
		DirectX::XMVECTOR vDMat;     // la valeur diffuse du mat�riau
	};

}