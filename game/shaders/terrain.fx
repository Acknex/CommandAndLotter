
Texture entSkin1;

sampler sMaskTex = sampler_state {
    Texture = <entSkin1>;
    MipFilter = Anisotropic;
    MinFilter = Anisotropic;
    MagFilter = Linear;
};

Texture maploader_terrain_digital_bmap;
Texture maploader_terrain_analogue_bmap;
Texture maploader_terrain_splatter_bmap;
Texture maploader_terrain_street_digital_bmap;
Texture maploader_terrain_street_analogue_bmap;
Texture maploader_terrain_digital_fancy_bmap;
Texture maploader_terrain_digital_wall_bmap;
Texture shader_noise_bmap;

sampler sDigital = sampler_state { Texture = <maploader_terrain_digital_bmap>; MipFilter = Anisotropic; MinFilter = Anisotropic; MagFilter = Linear; };
sampler sAnalog = sampler_state { Texture = <maploader_terrain_analogue_bmap>; MipFilter = Anisotropic; MinFilter = Anisotropic; MagFilter = Linear; };
sampler sSplatter = sampler_state { Texture = <maploader_terrain_splatter_bmap>; MipFilter = Anisotropic; MinFilter = Anisotropic; MagFilter = Linear; };
sampler sNoise = sampler_state { Texture = <shader_noise_bmap>; MipFilter = Anisotropic; MinFilter = Anisotropic; MagFilter = Linear; };
sampler sStreetDigital = sampler_state { Texture = <maploader_terrain_street_digital_bmap>; MipFilter = Anisotropic; MinFilter = Anisotropic; MagFilter = Linear; };
sampler sStreetAnalog = sampler_state { Texture = <maploader_terrain_street_analogue_bmap>; MipFilter = Anisotropic; MinFilter = Anisotropic; MagFilter = Linear; };
sampler sDigitalFancy = sampler_state { Texture = <maploader_terrain_digital_fancy_bmap>; MipFilter = Anisotropic; MinFilter = Anisotropic; MagFilter = Linear; };
sampler sDigitalWall = sampler_state { Texture = <maploader_terrain_digital_wall_bmap>; MipFilter = Anisotropic; MinFilter = Anisotropic; MagFilter = Linear; };

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
    float3 fakenormal = normalize(cross(normalize(ddx(In.world)), normalize(ddy(In.world))));

    float use_walls = step(dot(float3(0,1,0), fakenormal), 0.4);

    float lighting = lerp(1.0, 0.6, use_walls);

    float4 attribs = tex2D(sMaskTex, In.uv);

    float road = attribs.r;
    float vegetation = attribs.g;
    float elevation = attribs.b;

    float2 tilepos = floor(In.world.xz / 256.0);

    float4 noise = tex2D(sNoise, 0.05 * tilepos);

    float4 ground_digital = lerp(
      tex2D(sDigital, In.world.xz / 256.0),
      tex2D(sDigitalFancy, In.world.xz / 256.0),
      step(noise.x, 0.02)
    );
    float4 ground_analog  = textureNoTile(In.world.xz / 256.0);

    float4 road_digital = tex2D(sStreetDigital, In.world.xz / 512.0);
    float4 road_analog  = tex2D(sStreetAnalog, In.world.xz / 512.0);

    float4 digital = lerp(ground_digital, road_digital, road);
    float4 analog = lerp(ground_analog, road_analog, road);

    float analfactor = smoothstep(0.0, 1.0, 0.5 + 100.0 * (vegetation - 0.5 * analog.a));

    float4 surface = lerp(
        digital,
        analog,
        analfactor
    );

    float4 final = lerp(
      surface * (0.8 + elevation),
      float4(0,0,0,0),
      clamp(-0.001 * In.world.y, 0.0, 1.0)
    );

    float4 digital_wall = tex2D(sDigitalWall, float2(In.world.x + In.world.z, In.world.y) / 256.0);
    float4 analog_wall = tex2D(sAnalog, float2(In.world.x + In.world.z, In.world.y) / 256.0);

    final = lerp(
      final,
      lerp(digital_wall, analog_wall, analfactor),
      use_walls
    );

    final.rgb *= lighting;
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
