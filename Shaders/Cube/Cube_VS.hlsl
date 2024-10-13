cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

struct VS_INPUT
{
    float3 Position : POSITION;
    float4 Color : COLOR0;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float4 Color : COLOR0;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    float4 worldPosition = mul(float4(input.Position, 1.0f), World);
    float4 viewPosition = mul(worldPosition, View);
    output.Position = mul(viewPosition, Projection);
    
    output.Color = input.Color;
    return output;
}