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
sampler sColorTex = sampler_state { Texture = <entSkin1>; MipFilter = Linear; };

//////////////////////////////////////////////////////////////////////
struct out_vertex_main // Output to the pixelshader fragment
{
    float4 Pos		: POSITION;
    float2 TexCoord : TEXCOORD0;
};

out_vertex_main vs_building_main(
	float4 inPos : POSITION,
	float3 inNormal : NORMAL,
	float2 inTexCoord0 : TEXCOORD0)
{
    out_vertex_main Out;

	Out.Pos = DoTransform(inPos); // transform to screen coordinates
    Out.TexCoord = inTexCoord0.xy;

	return Out;
}

float4 ps_building_main(out_vertex_main In) : COLOR
{
    float4 color = tex2D(sColorTex, In.TexCoord);
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
}
