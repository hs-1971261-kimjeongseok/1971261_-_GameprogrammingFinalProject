////////////////////////////////////////////////////////////////////////////////
// Filename: normalmap.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture1 : register(t0);
Texture2D shaderTexture2 : register(t1);
SamplerState SampleType : register(s0);

cbuffer LightBuffer
{
    float4 specularColor;
    float4 diffuseColor;
    float3 lightDirection;
    float4 diffuseColor2;
    float3 lightDirection2;
    float2 padding;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 NormalMapPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float4 bumpMap;
    float3 bumpNormal;
    float3 lightDir;
    float lightIntensity;
    float specularlightIntensity;
    
    float3 lightDir2;
    float lightIntensity2;
    
    float4 color;
    
    float4 ambientColor;

	
	// Sample the pixel color from the color texture at this location.
    textureColor = shaderTexture1.Sample(SampleType, input.tex);

	// Sample the pixel from the normal map.
    bumpMap = shaderTexture2.Sample(SampleType, input.tex);
    
	// Expand the range of the normal value from (0, +1) to (-1, +1).
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    
	// Calculate the normal from the data in the normal map.
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
    
	// Normalize the resulting bump normal.
    bumpNormal = normalize(bumpNormal);
    
	// Invert the light direction for calculations.
    lightDir = -lightDirection;
    lightDir = normalize(lightDir);

    
    // Invert the light direction for calculations.
    lightDir2 = -lightDirection;
    lightDir2 = normalize(lightDir2);
    
    // Calculate the amount of light on this pixel.
    float3 viewingV = normalize(-input.position.xyz); // ps_4_0_level_9_3 버전으로 하니 오류가 나서 버전을 올림
    float3 HalfV = normalize(lightDir + viewingV);
    float3 reflectV = reflect(lightDir, input.normal);
    specularlightIntensity = pow(saturate(dot(input.normal, reflectV)), 4);
    //specularlightIntensity = pow(saturate(dot(input.normal, HalfV)), 4);
    
    lightIntensity = saturate(dot(input.normal, lightDir));
    lightIntensity2 = saturate(dot(input.normal, lightDir2));

    // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    color = saturate(diffuseColor * lightIntensity + diffuseColor2 * lightIntensity2)
    + saturate(specularlightIntensity * specularColor);

	// Combine the final light color with the texture color.
    color = color * textureColor;

    //return textureColor;
    return color;
}
