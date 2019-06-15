<<<<<<< HEAD
//enable: Shadow Map on Blue
//help: Terrain contains a shadow map on the Skin1 blue channel.
//help: With PS 1.1 (GeForce3), sun & dynamic lights are then disabled.
//id: 14
#define SHADOWMAP

#include <define>
#include <transform>
#include <sun>
#include <lights>
#include <fog>
#include <normal>

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

//////////////////////////////////////////////////////////////////////
struct out_terraintex3 // Output to the pixelshader fragment
{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
	float  Fog		: FOG;
	float2 MaskCoord: TEXCOORD0;
	float2 BaseCoord: TEXCOORD1;
	float2 RedCoord : TEXCOORD2;
	float2 GreenCoord: TEXCOORD3;
};

out_terraintex3 vs_terraintex3(
	float4 inPos : POSITION,
	float3 inNormal : NORMAL,
	float2 inTexCoord0 : TEXCOORD0)
{
	out_terraintex3 Out;

	Out.Pos = DoTransform(inPos); // transform to screen coordinates

// rotate and normalize the normal
	float3 N = DoNormal(inNormal);
	float3 P = mul(inPos,matWorld);

	Out.Color = fAmbient; // Add ambient and sun light
	for (int i=0; i<8; i++)  // Add 8 dynamic lights
		Out.Color += DoLight(P,N,i);
	Out.Fog = DoFog(inPos); // Add fog

// scale the texture coordinates for the masked textures
	Out.MaskCoord = inTexCoord0.xy;
	Out.BaseCoord = inTexCoord0.xy * vecSkill41.y;
	Out.RedCoord = inTexCoord0.xy * vecSkill41.z;
	Out.GreenCoord = inTexCoord0.xy * vecSkill41.w;
	return Out;
}

float4 ps_terraintex3(out_terraintex3 In): COLOR
{
	float4 MaskColor = tex2D(sMaskTex,In.MaskCoord);
	return MaskColor.ggga;
}

technique terraintex3_13
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
=======
//Variables
float4x4 matWorldViewProj;
float4x4 matWorld;

float4 vecFog;
float4 vecViewPos;

float3 vecSunDir;
float4 vecSunColor;
float4 vecLightPos[8];
float4 vecLightColor[8];
float4 vecLight;

float4 vecSkill1;


//Textures
Texture entSkin1;
Texture entSkin2;
Texture entSkin3;
Texture entSkin4;
Texture mtlSkin1;
Texture mtlSkin2;


//Samplers
sampler mapBlend = sampler_state
{
	Texture = <entSkin1>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	AddressU = Wrap;
	Addressv = Wrap;
};

sampler mapShadow = sampler_state
{
	Texture = <entSkin2>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	AddressU = Wrap;
	Addressv = Wrap;
};

sampler mapS = sampler_state
{
	Texture = <entSkin3>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	AddressU = Wrap;
	Addressv = Wrap;
};

sampler mapR = sampler_state
{
	Texture = <entSkin4>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	AddressU = Wrap;
	Addressv = Wrap;
};

sampler mapG = sampler_state
{
	Texture = <mtlSkin1>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	AddressU = Wrap;
	Addressv = Wrap;
};

sampler mapB = sampler_state
{
	Texture = <mtlSkin2>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	AddressU = Wrap;
	Addressv = Wrap;
};


//Structs
struct VS_TO_PS
{
	float4 Pos			: POSITION;
	float Fog			: FOG;
	
	float3 Normal		: TEXCOORD1;
	
	float2 TexCoords	: TEXCOORD2;
	float2 TexCoords2	: TEXCOORD3;
	
	float3 WPos			: TEXCOORD4;
};


//Vertexshader
VS_TO_PS TMulti_VS(	float4 inPos		: POSITION,
							float4 inNormal	: NORMAL,
							float2 TexCoords	: TEXCOORD0	)
{
	VS_TO_PS Out;
	
	//do the transformations
	Out.Pos = mul(inPos, matWorldViewProj);
	
	//pass the normal
	Out.Normal = inNormal;	// It somehow does not work if it is multiplicated with matWorld!!!
	
	
	//Fog
	Out.WPos = mul(inPos, matWorld);
	Out.Fog = 1 - (distance(Out.WPos, vecViewPos) - vecFog.x) * (vecFog.z);
	
	//Pass the texture coordinates
	Out.TexCoords.xy = TexCoords;
	Out.TexCoords2.xy = TexCoords*vecSkill1.x;
	
	return Out;
}


//Pixelshader
float4 TMulti_PS(VS_TO_PS In): COLOR
{
	//get blend and shadow texture
	float4 BlendColor = tex2D(mapBlend, In.TexCoords.xy);
	float4 ShadowColor = tex2D(mapShadow,In.TexCoords.xy);
	
	//get the tiled textures
	float4 SColor = tex2D(mapS, In.TexCoords2.xy);
	float4 RColor = tex2D(mapR, In.TexCoords2.xy);
	float4 GColor = tex2D(mapG, In.TexCoords2.xy);
	float4 BColor = tex2D(mapB, In.TexCoords2.xy);
	
	//blend the textures together
	float4 Color = SColor;
	Color = lerp(Color, RColor, BlendColor.r);
	Color = lerp(Color, GColor, BlendColor.g);
	Color = lerp(Color, BColor, BlendColor.b);
	
	//compute the diffuse shading
	float4 Diffuse = (vecLight/*+saturate(dot(-vecSunDir, In.Normal))*vecSunColor*/)*ShadowColor;
	float3 LightDir;
	float attenuation;
	
	for(int i = 0; i < 4; i++)
	{
		LightDir = vecLightPos[i]-In.WPos;
		attenuation = saturate(1.0f - length(LightDir/vecLightPos[i].w));
		Diffuse += saturate(dot(normalize(LightDir),In.Normal))*vecLightColor[i]*attenuation;
	}
	
	//compute the final color
	Color = Color*Diffuse;
	Color.a = 1.0f;
	
	return Color;
}


//Technique
technique tmulti
{
	pass one
	{
		VertexShader = compile vs_3_0 TMulti_VS();
		PixelShader = compile ps_3_0 TMulti_PS();
	}
}
>>>>>>> Terrain shader
