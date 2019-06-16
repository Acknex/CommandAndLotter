Texture entSkin1;

sampler sMaskTex = sampler_state {
    Texture = <entSkin1>;
    MipFilter = Point;
    MinFilter = Point;
    MagFilter = Point;
};

Texture maploader_terrain_digital_bmap;
Texture maploader_terrain_analogue_bmap;
Texture maploader_terrain_splatter_bmap;
Texture maploader_terrain_street_bmap;
Texture shader_noise_bmap;

sampler sDigital = sampler_state { Texture = <maploader_terrain_digital_bmap>; MipFilter = Linear; };
sampler sAnalog = sampler_state { Texture = <maploader_terrain_analogue_bmap>; MipFilter = Linear; };
sampler sSplatter = sampler_state { Texture = <maploader_terrain_splatter_bmap>; MipFilter = Linear; };
sampler sNoise = sampler_state { Texture = <shader_noise_bmap>; MipFilter = Linear; };
sampler sStreet = sampler_state { Texture = <maploader_terrain_street_bmap>; MipFilter = Linear; };


float4x4 matWorld;
float4x4 matWorldViewProj;

//////////////////////////////////////////////////////////////////////
struct out_terraintex3 // Output to the pixelshader fragment
{
    float4 pos        : POSITION;
    // float  Fog        : FOG;
    float2 uv         : TEXCOORD0;
    float3 normal     : TEXCOORD1;
    float3 world      : TEXCOORD2;
};



out_terraintex3 vs_terraintex3(
	float4 inPos       : POSITION,
	float3 inNormal    : NORMAL,
	float2 inTexCoord0 : TEXCOORD0
)
{
	out_terraintex3 Out;

	Out.pos = mul(inPos, matWorldViewProj);
	Out.normal = mul(inNormal, (float3x3)matWorld);
	Out.world = mul(inPos, matWorld);
	Out.uv = inTexCoord0.xy;

	return Out;
}

float4 ps_terraintex3(out_terraintex3 In) : COLOR
{
  if((abs(In.world.x) - 4000) < 0 && (abs(In.world.z) - 6000) < 0)
    clip(-1);

  return float4(0.5,0.5,0.5,1.0);
}

technique terraintex3_13
{
	pass one
	{
		AlphaBlendEnable = false;
		ZWriteEnable = true;
		ZEnable = true;
		VertexShader = compile vs_3_0 vs_terraintex3();
		PixelShader = compile ps_3_0 ps_terraintex3();
	}
}
