// Tweakables: 
static const float AmbientIntensity  = 1.0f; // The intensity of the ambient light. 
static const float DiffuseIntensity = 1.0f;  // The intensity of the diffuse light. 
static const float4 SunColor = {0.9f, 0.9f, 0.5f, 1.0f}; // Color vector of the sunlight. 
    
// Application fed data: 
const float4x4 matWorldViewProj; // World*view*projection matrix. 
const float4x4 matWorld; // World matrix. 
const float4 vecAmbient; // Ambient color, passed by the engine. 
const float4 vecSunDir;  // The sun direction vector. 
texture entSkin1; // Model texture

// Color map sampler
sampler ColorMapSampler = sampler_state 
{ 
   Texture = <entSkin1>; 
   mipfilter = none;
   minfilter = none;
   magfilter = none;
   AddressU  = Clamp; 
   AddressV  = Clamp; 
}; 
    
// Vertex Shader: 
void DiffuseVS( 
   in float4 InPos: POSITION, 
   in float3 InNormal: NORMAL, 
   in float2 InTex: TEXCOORD0, 
   out float4 OutPos: POSITION, 
   out float2 OutTex: TEXCOORD0, 
   out float3 OutNormal: TEXCOORD1) 
{ 
// Transform the vertex from object space to clip space: 
   OutPos = mul(InPos, matWorldViewProj); 
// Transform the normal from object space to world space: 
   OutNormal = normalize(mul(InNormal, matWorld));
// Pass the texture coordinate to the pixel shader:
InTex.x *= 52.0/64.0;
   OutTex = InTex; 
} 
    
// Pixel Shader: 
float4 DiffusePS( 
   in float2 InTex: TEXCOORD0, 
   in float3 InNormal: TEXCOORD1): COLOR 
{ 
// Calculate the ambient term: 
   float4 Color = tex2D(ColorMapSampler, InTex); 
// Calculate final color: 
Color.a = 0.25;
   return Color; 
} 
 
// Technique: 
technique DiffuseTechnique 
{ 
   pass P0 
   { 
      VertexShader = compile vs_2_0 DiffuseVS(); 
      PixelShader  = compile ps_2_0 DiffusePS(); 
   } 
} 

