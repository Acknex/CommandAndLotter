Texture TargetMap;

sampler2D source_map = sampler_state { texture = <TargetMap>; MipFilter = Linear;	};

float4 vecSkill1; // xyz = rgba
float4 vecSkill5; // x = progress

float4 postprocessing_colorshift(float4 screenSpace : VPOS ) : COLOR0
{
	float2 uv = screenSpace.xy / 128.0;

	float2 p = 2.0 * uv - 1.0;

	float a = 0.5 + 0.5 * atan2(p.x, p.y) / 3.14159265359;

	return lerp(
		float4(0, 0, 0, 0),
		vecSkill1.rgba,
		step(length(p), vecSkill5.y) * step(a, vecSkill5.x)
	);
}

technique PostProcess
{
	pass p1
	{
		AlphaBlendEnable = false;
		VertexShader = null;
		PixelShader = compile ps_3_0 postprocessing_colorshift();
	}
}
