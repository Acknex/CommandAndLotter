//enable: Shadow Map on Blue
//help: Terrain contains a shadow map on the Skin1 blue channel.
//help: With PS 1.1 (GeForce3), sun & dynamic lights are then disabled.
//id: 14
#define SHADOWMAP

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

const float MTL_BARRICADE = 0.0;
const float MTL_STREET    = 32.0 / 255.0;
const float MTL_WATER     = 64.0 / 255.0;
const float MTL_LAVA      = 128.0 / 255.0;
const float MTL_HOLE      = 192.0 / 255.0;
const float MTL_DEFAULT   = 1.0;

bool is_material(float t, float mtl)
{
	return abs(t - mtl) < (8.0 / 255.0);
}

out_terraintex3 vs_terraintex3(
	float4 inPos       : POSITION,
	float3 inNormal    : NORMAL,
	float2 inTexCoord0 : TEXCOORD0
)
{
	out_terraintex3 Out;

	float4 terrain_attribs = tex2Dlod(sMaskTex, float4(inTexCoord0, 0, 0));

	if(is_material(terrain_attribs.r, MTL_HOLE)) {
		inPos.y = -10000.0;
	}
	else if(is_material(terrain_attribs.r, MTL_WATER)) {
		inPos.y = -50.0;
	}
	else if(is_material(terrain_attribs.r, MTL_BARRICADE)) {
		inPos.y += 500.0;
	}

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
    float4 attribs = tex2Dlod(sMaskTex, float4(In.uv, 0, 0));

    float4 surface;

    if(is_material(attribs.r, MTL_WATER)) {
      surface = float4(0,0,1,1);
    }
    // if street, do streetstuff
    else if(is_material(attribs.r, MTL_STREET))
    {
      surface = tex2D(sStreet, In.world.xz / 512.0);
    }
    else if(is_material(attribs.r, MTL_STREET))
    {
      surface = tex2D(sStreet, In.world.xz / 512.0);
    }
    else if(is_material(attribs.r, MTL_LAVA))
    {
      surface = float4(1,0,0,1);
    }
    else if(is_material(attribs.r, MTL_DEFAULT) || is_material(attribs.r, MTL_HOLE))
    {
      float4 pattern = tex2D(sSplatter, In.world.xz / 512.0);

      float4 analog = textureNoTile(In.world.xz / 512.0);

      surface = lerp(
          tex2D(sDigital, In.world.xz / 512.0),
          analog,
          pow(length(analog), 0.2) * smoothstep(0.0, 1.0, attribs.g)
      );
    }
    else if(is_material(attribs.r, MTL_BARRICADE))
    {
      surface = float4(0.1,0.1,0.1,1);
    }
    else
    {
      surface = float4(1,0,1,1);
    }

    float4 final = lerp(
      surface * (0.8 + attribs.b),
      float4(0,0,0,0),
      clamp(-0.001 * In.world.y, 0.0, 1.0)
    );
    final.a = 1.0;
    return final;
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
