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

float4 vecSkill41;
float4 vecSkill45;

struct out_vertex_wireframe // Output to the pixelshader fragment
{
    float4 Pos : POSITION;
    float3 WorldPos : TEXCOORD0;
};

out_vertex_wireframe vs_building_wireframe(
    float4 inPos : POSITION,
    float3 inNormal : NORMAL,
    float2 inTexCoord0 : TEXCOORD0)
{
    out_vertex_wireframe Out;

    Out.Pos = DoTransform(inPos); // transform to screen coordinates
    Out.WorldPos = mul(inPos, matWorld);

    return Out;
}

float4 ps_building_wireframe(out_vertex_wireframe In) : COLOR
{
    float fadeFactor = smoothstep(vecSkill41.x-200.0, vecSkill41.x, In.WorldPos.y - vecSkill45.y);
    return float4(0.0f, 0.8f, 1.0f, 0.3f * fadeFactor);
}

technique building_technique
{
    pass one
    {
        AlphaBlendEnable = True;
        ZWriteEnable = False;
        VertexShader = compile vs_3_0 vs_building_wireframe();
        PixelShader = compile ps_3_0 ps_building_wireframe();
    }
}
