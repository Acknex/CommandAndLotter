Texture entSkin1; // Red/green for blending, blue for shadow
Texture entSkin2; // Basic tiled terrain texture
Texture entSkin3; // Red masked tiled texture
Texture entSkin4; // Green masked tiled texture

float4 vecSkill41;
float fAmbient;

sampler sMaskTex = sampler_state { Texture = <entSkin1>; MipFilter = Linear; };
sampler sBaseTex = sampler_state { Texture = <entSkin2>; MipFilter = Linear; };
sampler sRedTex = sampler_state { Texture = <entSkin3>; MipFilter = Linear; };
sampler sGreenTex = sampler_state {	Texture = <entSkin4>; MipFilter = Linear;	};

float4x4 matWorld;
float4x4 matWorldViewProj;

//////////////////////////////////////////////////////////////////////
struct out_terraintex3 // Output to the pixelshader fragment
{
	float4 Pos		: POSITION;
	// float4 Color	: COLOR0;
	// float  Fog		: FOG;
	float2 MaskCoord: TEXCOORD0;
	// float2 BaseCoord: TEXCOORD1;
	// float2 RedCoord : TEXCOORD2;
	// float2 GreenCoord: TEXCOORD3;
};

out_terraintex3 vs_terraintex3(
	float4 inPos : POSITION,
	float3 inNormal : NORMAL,
	float2 inTexCoord0 : TEXCOORD0)
{
	out_terraintex3 Out;
	Out.Pos = mul(matWorldViewProj, inPos); // transform to screen coordinates
	Out.MaskCoord = inTexCoord0.xy;
	return Out;
}

float4 ps_terraintex3(out_terraintex3 In): COLOR
{
	float4 MaskColor = tex2D(sMaskTex, In.MaskCoord);
	return float4(1,0,0,1);
}

technique terraintex
{
	pass one
	{
		sampler[0] = (sMaskTex);
		sampler[1] = (sBaseTex);
		sampler[2] = (sRedTex);
		sampler[3] = (sGreenTex);

		VertexShader = compile vs_2_0 vs_terraintex3();
		PixelShader = compile ps_2_0 ps_terraintex3();
	}
}
