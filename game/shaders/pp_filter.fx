Texture TargetMap;
sampler samplerBloomColor = sampler_state
{
	Texture = <TargetMap>; 
	AddressU = Clamp;
	AddressV = Clamp; 
	MipFilter = Linear;
};

float4 vecSkill1;

float4 bloomFilter(float2 Tex : TEXCOORD0) : COLOR0
{
        float4 bloom = tex2D(samplerBloomColor, Tex.xy);
        if(max(max(bloom.x, bloom.y), bloom.z) < vecSkill1.x)
        {
            bloom = float4(0.0f, 0.0f, 0.0f, 1.0f);
        }
        return bloom;
}

technique PostProcess 
{
	pass p1 
	{
		AlphaBlendEnable = false;
		VertexShader = null;
                PixelShader = compile ps_2_0 bloomFilter();
	}
}
