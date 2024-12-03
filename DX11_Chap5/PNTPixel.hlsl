SamplerState SampleType;
Texture2D shaderTexture;

cbuffer LightBuffer
{
    float4 diffuseColor;
    float3 lightDirection;
    float4 diffuseColor2;
    float3 lightDirection2;
    //float padding;
};

struct PixelInputType
{
	// SV: system value
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
};

// SV_Target0 = SV_Target, SV_Target1, SV_Target2 등은 MRT
// SV_Depth depth를 출력하면 최적화 불가로 약간 느려짐
float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    
    float3 lightDir2;
    float lightIntensity2;
    
    float4 color;


	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);

	// Invert the light direction for calculations.
    lightDir = -lightDirection;
    lightDir = normalize(lightDir);
    
    // Invert the light direction for calculations.
    lightDir2 = -lightDirection;
    lightDir2 = normalize(lightDir2);
    
    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, lightDir));
    lightIntensity2 = saturate(dot(input.normal, lightDir2));

    // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    color = saturate(diffuseColor * lightIntensity + diffuseColor2 * lightIntensity2);

    // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
    //color = color * textureColor;

    return color;
}