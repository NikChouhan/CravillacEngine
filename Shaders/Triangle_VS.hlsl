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
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Position = float4(input.Position, 1.0f);
    output.Color = input.Color;
    return output;
}
