//enable: Shadow Map on Blue
//help: Terrain contains a shadow map on the Skin1 blue channel.
//help: With PS 1.1 (GeForce3), sun & dynamic lights are then disabled.
//id: 14
#define SHADOWMAP

Texture entSkin1;

sampler sMaskTex = sampler_state {
    Texture = <entSkin1>;
    MipFilter = Linear;
};

Texture maploader_terrain_digital_bmap;
Texture maploader_terrain_analogue_bmap;
Texture maploader_terrain_splatter_bmap;
Texture shader_noise_bmap;

sampler sDigital = sampler_state { Texture = <maploader_terrain_digital_bmap>; MipFilter = Linear; };
sampler sAnalog = sampler_state { Texture = <maploader_terrain_analogue_bmap>; MipFilter = Linear; };
sampler sSplatter = sampler_state { Texture = <maploader_terrain_splatter_bmap>; MipFilter = Linear; };
sampler sNoise = sampler_state { Texture = <shader_noise_bmap>; MipFilter = Linear; };


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

    float4 terrain_attribs = tex2D(sMaskTex, inTexCoord0);

    Out.pos = mul(inPos, matWorldViewProj);
    Out.normal = mul(inNormal, (float3x3)matWorld);
    Out.world = mul(inPos, matWorld);
	Out.uv = inTexCoord0.xy;

	return Out;
}

float sum( float3 v ) { return v.x+v.y+v.z; }

float4 textureNoTile( float2 x )
{
    float k = tex2D( sNoise, 0.005 * x ).x; // cheap (cache friendly) lookup

    float l = k*8.0;
    float i = floor( l );
    float f = frac( l );

    float2 offa = sin(float2(3.0,7.0)*(i+0.0)); // can replace with any other hash
    float2 offb = sin(float2(3.0,7.0)*(i+1.0)); // can replace with any other hash

    float4 cola = tex2D( sAnalog, x + offa);
    float4 colb = tex2D( sAnalog, x + offb);

    return lerp( cola, colb, smoothstep(0.2,0.8,f-0.1*sum(cola-colb)) );
}

float4 ps_terraintex3(out_terraintex3 In) : COLOR
{
    float4 attribs = tex2D(sMaskTex, In.uv);

    float4 pattern = tex2D(sSplatter, In.world.xz / 512.0);

    float4 surface = lerp(
        tex2D(sDigital, In.world.xz / 256.0),
        textureNoTile(In.world.xz / 512.0),
        step(pattern.r, smoothstep(0.0, 1.0, attribs.g)));


    return surface * (0.5 + attribs.b);
}

technique terraintex3_13
{
	pass one
	{
        shademode = flat;

		VertexShader = compile vs_2_0 vs_terraintex3();
		PixelShader = compile ps_2_0 ps_terraintex3();
	}
}
