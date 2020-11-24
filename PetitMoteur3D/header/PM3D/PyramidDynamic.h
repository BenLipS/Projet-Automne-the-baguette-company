#pragma once
class CDispositifD3D11;

//  Classe : CBloc
//
//  BUT : 	Classe de bloc
//
class PyramidDynamic : public Objet3DDynamic
{
public:
	PyramidDynamic(Scene* _scene, PxTransform _position, const float dx, const float dy, const float dz,
		CDispositifD3D11* _pDispositif);

	// Destructeur
	virtual ~PyramidDynamic();

	virtual void Anime(float tempsEcoule) override;
	virtual void Draw() override;

private:
	CDispositifD3D11* pDispositif;
	void InitShaders();

	ID3D11Buffer* pVertexBuffer;
	ID3D11Buffer* pIndexBuffer;

	ID3D11VertexShader* pVertexShader;
	ID3D11PixelShader* pPixelShader;
	ID3D11InputLayout* pVertexLayout;

	// Définitions des valeurs d'animation
	ID3D11Buffer* pConstantBuffer;
	XMMATRIX matWorld;
	//float rotation;
	/*static PxRigidDynamic* createRigidBody(Scene* _scene, PxTransform _position,
		const float _dx, const float _dy, const float _dz);*/
