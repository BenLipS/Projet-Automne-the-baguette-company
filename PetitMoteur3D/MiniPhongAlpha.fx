cbuffer param
{
	float4x4 matWorldViewProj;   // la matrice totale 
	float4x4 matWorld;		// matrice de transformation dans le monde 
	float4 vLumiere1; 		// la position de la source d'éclairage 1 (Point)
	float4 vLumiere2; 		// la position de la source d'éclairage 2 (Globe)
	float4 vLumiere3; 		// la position de la source d'éclairage 3 (Globe)
	float4 vLumiere4; 		// la position de la source d'éclairage 4 (Globe)
	float4 vLumiere5; 		// la position de la source d'éclairage 5 (Globe)
	float4 vLumiere6; 		// la position de la source d'éclairage 6 (Globe)
	float4 vLumiere7; 		// la position de la source d'éclairage 7 (Globe)
	float4 vLumiere8; 		// la position de la source d'éclairage 8 (Globe)
	float4 vLumiere9; 		// la position de la source d'éclairage 9 (Globe)
	float4 vCamera; 			// la position de la cam�ra
	float4 vAEcl; 			// la valeur ambiante de l'�clairage
	float4 vAMat; 			// la valeur ambiante du mat�riau
	float4 vDEcl; 			// la valeur diffuse de l'�clairage 
	float4 vDMat; 			// la valeur diffuse du mat�riau 
	float4 vSEcl; 			// la valeur sp�culaire de l'�clairage 
	float4 vSMat; 			// la valeur sp�culaire du mat�riau 
	float puissance;
	int bTex;		    // Bool�en pour la pr�sence de texture
	float2 remplissage;
	float4 vTEcl; 			// la valeur de l'éclairage Tunnel
}

struct VS_Sortie
{
	float4 Pos : SV_Position;
	float3 Norm :    TEXCOORD0;
	float3 vDirLum : TEXCOORD1;
	float3 vDirCam : TEXCOORD2;
	float2 coordTex : TEXCOORD3;
	float bTun : TEXCOORD4;
};

VS_Sortie MiniPhongAlphaVS(float4 Pos : POSITION, float3 Normale : NORMAL, float2 coordTex : TEXCOORD)
{
	VS_Sortie sortie = (VS_Sortie)0;

	sortie.Pos = mul(Pos, matWorldViewProj);
	sortie.Norm = mul(float4(Normale, 0.0f), matWorld).xyz;

	float3 PosWorld = mul(Pos, matWorld).xyz;

	sortie.vDirLum = vLumiere1.xyz - PosWorld;
	sortie.vDirCam = vCamera.xyz - PosWorld;

	// Coordonn�es d'application de texture
	sortie.coordTex = coordTex;

	float3 diffPosET = vLumiere2.xyz - PosWorld;
	float dist = pow(diffPosET.x, 2) + pow(diffPosET.y, 2) + pow(diffPosET.z, 2);

	diffPosET = vLumiere3.xyz - PosWorld;
	dist = min(dist, pow(diffPosET.x, 2) + pow(diffPosET.y, 2) + pow(diffPosET.z, 2));

	diffPosET = vLumiere4.xyz - PosWorld;
	dist = min(dist, pow(diffPosET.x, 2) + pow(diffPosET.y, 2) + pow(diffPosET.z, 2));

	diffPosET = vLumiere5.xyz - PosWorld;
	dist = min(dist, pow(diffPosET.x, 2) + pow(diffPosET.y, 2) + pow(diffPosET.z, 2));

	diffPosET = vLumiere6.xyz - PosWorld;
	dist = min(dist, pow(diffPosET.x, 2) + pow(diffPosET.y, 2) + pow(diffPosET.z, 2));

	diffPosET = vLumiere7.xyz - PosWorld;
	dist = min(dist, pow(diffPosET.x, 2) + pow(diffPosET.y, 2) + pow(diffPosET.z, 2));

	diffPosET = vLumiere8.xyz - PosWorld;
	dist = min(dist, pow(diffPosET.x, 2) + pow(diffPosET.y, 2) + pow(diffPosET.z, 2));

	diffPosET = vLumiere9.xyz - PosWorld;
	dist = min(dist, pow(diffPosET.x, 2) + pow(diffPosET.y, 2) + pow(diffPosET.z, 2));

	if (dist < 200000)
		sortie.bTun = 1.0f;
	else if (dist < 500000)
		sortie.bTun = 1.0f - ((dist - 200000) / 300000);
	else
		sortie.bTun = 0.0f;

	return sortie;
}

Texture2D texture1;  // la texture
Texture2D texture2;  // la texture
Texture2D textureMasque;  // la texture
SamplerState SampleState;  // l'�tat de sampling

float4 MiniPhongAlphaPS(VS_Sortie vs) : SV_Target
{
	float3 couleur;

// Normaliser les param�tres
float3 N = normalize(vs.Norm);
float3 L = normalize(vs.vDirLum);
float3 V = normalize(vs.vDirCam);

// Valeur de la composante diffuse
float3 diff = saturate(dot(N, L));

// R = 2 * (N.L) * N � L
float3 R = normalize(2 * diff * N - L);

// Calcul de la sp�cularit� 
float3 S = pow(saturate(dot(R, V)), puissance);

float3 couleurTexture;
float3 couleurTexture1;
float3 couleurTexture2;
float3 alpha;

if (bTex > 0)
{
	// �chantillonner la couleur du pixel � partir de la texture
	couleurTexture1 = texture1.Sample(SampleState, vs.coordTex).rgb;
	couleurTexture2 = texture2.Sample(SampleState, vs.coordTex).rgb;
	alpha = textureMasque.Sample(SampleState, vs.coordTex).rgb;

	couleurTexture = alpha.r * couleurTexture1 + (1 - alpha.r) * couleurTexture2;

	// I = A + D * N.L + (R.V)n
	couleur = couleurTexture * vAEcl.rgb +
		couleurTexture * vDEcl.rgb * diff +
		vSEcl.rgb * vSMat.rgb * S;
	if (vs.bTun > 0)
	{
		couleur = couleurTexture * vTEcl.rgb * vs.bTun * diff + couleurTexture * vAEcl.rgb + (1.0f - vs.bTun) * vDEcl.rgb * diff;
	}
	/*couleur = couleurTexture * vAEcl.rgb*/
}
else
{
	couleur = vAEcl.rgb * vAMat.rgb + vDEcl.rgb * vDMat.rgb * diff +
		vSEcl.rgb * vSMat.rgb * S;
	if (vs.bTun > 0)
	{
		couleur = vAMat.rgb * vTEcl.rgb * vs.bTun * diff + vAEcl.rgb * vAMat.rgb + (1.0f - vs.bTun) * vDEcl.rgb * diff * vAMat.rgb;
	}
}
return float4(couleur, 1.0f);
}

technique11 MiniPhongAlpha
{
	pass pass0
	{
		SetVertexShader(CompileShader(vs_4_0, MiniPhongAlphaVS()));
		SetPixelShader(CompileShader(ps_4_0, MiniPhongAlphaPS()));
		SetGeometryShader(NULL);
	}
}
