cbuffer param {
	float4x4 matWorldViewProj;   // la matrice totale
	float4x4 matWorld;   // matrice de transformation dans le monde
	float4 vLumiere1;    // la position de la source d��clairage (Point)
	float4 vLumiere2;    // la position de la source d��clairage (Point)
	float4 vCamera;    // la position de la cam�ra
	float4 vAEcl;     // la valeur ambiante de l��clairage
	float4 vAMat;     // la valeur ambiante du mat�riau
	float4 vDEcl;     // la valeur diffuse de l��clairage
	float4 vDMat;     // la valeur diffuse du mat�riau
} 

struct VS_Sortie {
	float4 Pos : SV_Position;
	float3 Norm : TEXCOORD0;
	float3 vDirLum : TEXCOORD1;
	float3 vDirCam : TEXCOORD2;
};

float4 MiniPhongPS(VS_Sortie vs) : SV_Target0{
	float3 couleur;    
	
	// Normaliser les param�tres
	float3 N = normalize(vs.Norm);
	float3 L = normalize(vs.vDirLum);
	float3 V = normalize(vs.vDirCam); 

	// Valeur de la composante diffuse
	float3 diff = saturate(dot(N, L)); 

	// R = 2 * (N.L) * N � L
	float3 R = normalize(2 * diff.xyz * N - L); 

	// Puissance de 4 - pour l�exemple
	float S = pow(saturate(dot(R, V)), 4);  

	// I = A + D * N.L + (R.V)n
	couleur =  vAEcl.rgb * vAMat.rgb + vDEcl.rgb * vDMat.rgb * diff;
	couleur += S; 
	
	return float4(couleur, 1.0f);
}