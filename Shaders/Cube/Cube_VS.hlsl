cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

struct VS_INPUT
{
    float4 Position : POSITION;
    float4 Color : COLOR0;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float4 Color : COLOR0;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Position = mul(input.Position, World);
    output.Position = mul(input.Position, View);
    output.Position = mul(input.Position, Projection);
    output.Color = input.Color;
    return output;
}
