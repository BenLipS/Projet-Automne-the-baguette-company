#pragma once
#include "Objet3DDynamic.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include "tools.h"

#include "PxPhysicsAPI.h"
using namespace DirectX;

namespace PM3D
{
	class CDispositifD3D11;

	//  Classe : CBloc
	//
	//  BUT : 	Classe de bloc
	//
	class BlocDynamic : public Objet3DDynamic
	{
	public:
		BlocDynamic(Scene* _scene, PxTransform _position, const float dx, const float dy, const float dz,
			CDispositifD3D11* _pDispositif);

		// Destructeur
		virtual ~BlocDynamic();

		virtual void Anime(float tempsEcoule);
		

	private:
		void InitShaders();

		ID3D11VertexShader* pVertexShader;
		ID3D11PixelShader* pPixelShader;

		float dx_, dy_, dz_;

		static PxRigidDynamic* createRigidBody(Scene* _scene, PxTransform _position,
			const float _dx, const float _dy, const float _dz);
	};

	/*static void cr�ationTubulaire(const int nbFacette, float longueur, float rayon) {

		const int nbPoints = nbFacette * 2;
		const int nbSommets = nbFacette * 4;
		const int nbIndexs = nbFacette * 6;

		XMFLOAT3 origine(0.0f, 0.0f, 0.0f);

		//Points et normales
		XMFLOAT3* point = new XMFLOAT3[nbPoints];
		XMFLOAT3* normale = new XMFLOAT3[nbFacette];
		CSommetBloc* sommets = new CSommetBloc[nbSommets];
		int* index = new int[nbIndexs];

		for (int i = 0; i < nbFacette; i++) {
			//points
			point[i] = {rayon * cos((XM_2PI/ nbFacette) * i), rayon * cos((XM_2PI / nbFacette) * i), longueur};
			point[i + nbFacette] = { rayon * cos((XM_2PI / nbFacette) * i),rayon * cos((XM_2PI / nbFacette) * i), longueur};
		}

		for (int i = 0; i < nbFacette; i++) {
			//normales
			XMVECTOR normale_temp = XMLoadFloat3(point + i) + XMLoadFloat3(point + ((i + 1) % nbFacette)); //normale = addition des 2 sommets normalis�s
			XMStoreFloat3(normale + i, XMVector3Normalize(normale_temp));

			//sommets
			sommets[0 + (i * 4)] = CSommetBloc(point[i], normale[i]);
			sommets[1 + (i * 4)] = CSommetBloc(point[i + 1], normale[i]);
			sommets[2 + (i * 4)] = CSommetBloc(point[nbFacette + i], normale[i]);
			sommets[3 + (i * 4)] = CSommetBloc(point[nbFacette + i + 1], normale[i]);

			//indexs
			index[(i * 6) + 0] = i;
			index[(i * 6) + 1] = i + nbFacette;
			index[(i * 6) + 2] = i + 1;
			index[(i * 6) + 3] = i + 1;
			index[(i * 6) + 4] = i + nbFacette;
			index[(i * 6) + 5] = i + nbFacette + 1;
		}
	}*/
} // namespace PM3D
