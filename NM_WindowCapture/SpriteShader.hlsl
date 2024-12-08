
HLSL_EXTERNAL_INCLUDE(

Texture2D captureTexture : register(t0);
SamplerState captureTextureSampler : register(s0);

struct VSOut
{
    float4 Pos : SV_Position;
    float2 Uv : TEXCOORD0; 
};

VSOut VS(float3 Pos : POSITION, float2 Uv : TEXUV)
{
    VSOut Out = (VSOut)0;
    Out.Pos = float4(Pos, 1);
    Out.Uv = Uv;
    return Out;
}

float4 PS(VSOut In) : SV_Target0
{
    return captureTexture.Sample(captureTextureSampler, In.Uv);
}

)