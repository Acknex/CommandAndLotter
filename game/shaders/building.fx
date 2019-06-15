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
#include <color>

Texture entSkin1;

float4 vecTime;
float4 vecSkill41;
float4 vecSkill45;

sampler sColorTex = sampler_state { Texture = <entSkin1>; MipFilter = Linear; };

//////////////////////////////////////////////////////////////////////
struct out_vertex_main // Output to the pixelshader fragment
{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
	float  Fog		: FOG;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 WorldPos : TEXCOORD2;
};

struct out_vertex_inside // Output to the pixelshader fragment
{
    float4 Pos		: POSITION;
    float3 ViewPos : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
};

out_vertex_main vs_building_main(
	float4 inPos : POSITION,
	float3 inNormal : NORMAL,
	float2 inTexCoord0 : TEXCOORD0)
{
    out_vertex_main Out;

	Out.Pos = DoTransform(inPos); // transform to screen coordinates

// rotate and normalize the normal
    Out.Normal = DoNormal(inNormal);
    Out.WorldPos = mul(inPos, matWorld);

    Out.Color = DoAmbient(); // Add ambient and sun light
	for (int i=0; i<8; i++)  // Add 8 dynamic lights
        Out.Color += DoLight(Out.WorldPos, Out.Normal, i);
	Out.Fog = DoFog(inPos); // Add fog

// scale the texture coordinates for the masked textures
    Out.TexCoord = inTexCoord0.xy;
	return Out;
}

float4 ps_building_main(out_vertex_main In) : COLOR
{
    clip(vecSkill41.x - In.WorldPos.y + vecSkill45.y);

    float4 color = tex2D(sColorTex, In.TexCoord);
    //float4 light = In.Color * DoSunLight(normalize(In.Normal));

    color = lerp(color, float4(0.0f, 0.8f, 1.0f, 1.0f), smoothstep(vecSkill41.x - 10.0f, vecSkill41.x, In.WorldPos.y - vecSkill45.y));

    return color;// * light;
}

out_vertex_inside vs_building_inside(
    float4 inPos : POSITION)
{
    out_vertex_inside Out;

    Out.Pos = DoTransform(inPos);
    Out.WorldPos = mul(inPos, matWorld);
    Out.ViewPos = -mul(inPos, matWorldView);

    return Out;
}

float4 ps_building_inside(out_vertex_inside In) : COLOR
{
    clip(vecSkill41.x - In.WorldPos.y + vecSkill45.y);

    float3 diffToView = normalize(vecViewPos.xyz - In.WorldPos.xyz);
    float heightDiff = vecSkill41.x - In.WorldPos.y + vecSkill45.y;
    float3 surfacePosition;
    surfacePosition.x = In.WorldPos.x + diffToView.x * heightDiff - vecSkill45.x;
    surfacePosition.y = 0.0f;
    surfacePosition.z = In.WorldPos.z + diffToView.z * heightDiff - vecSkill45.z;

    float distance = saturate(length(surfacePosition.xz) / vecSkill41.y);
    float4 color = lerp(float4(0.0f, 0.0f, 0.0f, 1.0f), float4(0.0f, 0.8f, 1.0f, 1.0f), 1.0f-distance);

    color = lerp(color, float4(0.0f, 0.8f, 1.0f, 1.0f), smoothstep(vecSkill41.x - 10.0f, vecSkill41.x, In.WorldPos.y - vecSkill45.y));
    color.a = vecSkill41.z;

    return color;
}

technique building_technique
{
	pass one
    {
        CullMode = CCW;
        ZWriteEnable = True;
        ZEnable = True;

        VertexShader = compile vs_3_0 vs_building_main();
        PixelShader = compile ps_3_0 ps_building_main();
    }

    pass two
    {
        CullMode = CW;
        AlphaBlendEnable = True;
        ZWriteEnable = False;
        ZEnable = True;

        VertexShader = compile vs_3_0 vs_building_inside();
        PixelShader = compile ps_3_0 ps_building_inside();
    }
}
