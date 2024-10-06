struct PS_INPUT
{
    float4 Position : SV_Position;
    float4 Color : COLOR0;
};

struct PS_OUTPUT
{
    float4 Color : SV_Target0;
};

PS_OUTPUT PSMain(PS_INPUT input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0;
    output.Color = input.Color;
    return output;
}
