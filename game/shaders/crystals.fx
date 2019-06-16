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
float fAmbient;
sampler sColorTex = sampler_state { Texture = <entSkin1>; MipFilter = Linear; };

//////////////////////////////////////////////////////////////////////
struct out_vertex_main // Output to the pixelshader fragment
{
    float4 Pos		: POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 WorldPos : TEXCOORD2;
};

out_vertex_main vs_building_main(
	float4 inPos : POSITION,
	float3 inNormal : NORMAL,
	float2 inTexCoord0 : TEXCOORD0)
{
    out_vertex_main Out;

	Out.Pos = DoTransform(inPos); // transform to screen coordinates
    Out.TexCoord = inTexCoord0.xy;
    Out.Normal = mul(float4(inNormal, 0.0), matWorld);
    Out.WorldPos = mul(inPos, matWorld);

	return Out;
}

float4 ps_building_main(out_vertex_main In) : COLOR
{
    float4 color = tex2D(sColorTex, In.TexCoord) * (fAmbient + 1.0f) * vecColor * 2.0f;
    color.rgb *= saturate(In.Normal.y * 0.5 + 0.5);
    color.rgb += (1.0 - saturate(dot(normalize(vecViewPos.xyz-In.WorldPos), In.Normal))) * 0.3;
    return color;
}

technique building_technique
{
	pass one
    {
        CullMode = CCW;
        ZWriteEnable = True;
        ZEnable = True;
        ShadeMode = Flat;

        VertexShader = compile vs_3_0 vs_building_main();
        PixelShader = compile ps_3_0 ps_building_main();
    }
}
