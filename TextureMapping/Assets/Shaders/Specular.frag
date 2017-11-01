#version 150

#define LIGHT_MAX 7

struct SMaterial
{
	vec3 AmbientColor;
	vec3 DiffuseColor;
	vec3 SpecularColor;
	float Shininess;
};

struct SLight
{
	vec3 Position;
	vec3 Color;
	float Radius;
};

in vec3 fObjectPosition;
in vec3 fWorldPosition;
in vec3 fNormal;
in vec2 fTexCoords;

uniform sampler2D uTexture;

uniform int uPointLightsCount;
uniform SLight uPointLights[LIGHT_MAX];
uniform SMaterial uMaterial;
uniform vec3 uCameraPosition;
uniform int uMode;

out vec4 outColor;


float sq(float v)
{
	return v * v;
}

void main()
{
	vec3 nEye = normalize(uCameraPosition - fWorldPosition);
	vec3 nNormal = normalize(fNormal);

	vec3 Diffuse = vec3(0);
	vec3 Specular = vec3(0);
	vec3 Ambient = uMaterial.AmbientColor;

	for (int i = 0; i < LIGHT_MAX && i < uPointLightsCount; ++ i)
	{
		vec3 Light = uPointLights[i].Position - fWorldPosition;
		vec3 nLight = normalize(Light);
		vec3 Half = normalize(nLight + nEye);

		float Distance = length(Light);
		float Attenuation = 1.0 / sq(Distance / uPointLights[i].Radius + 1.0);

		float Shading = clamp(dot(nNormal, nLight), 0.0, 1.0);
		Diffuse += uMaterial.DiffuseColor * Shading * Attenuation * uPointLights[i].Color;

		float Highlight = pow(clamp(dot(Half, nNormal), 0.0, 1.0), uMaterial.Shininess);
		Specular += uMaterial.SpecularColor * Highlight * Attenuation * uPointLights[i].Color;
	}

	vec3 Color = (Specular + Diffuse + Ambient);
	vec2 TexCoord = vec2(0.0);

	switch (uMode)
	{
	case 0: // Show tex coords
		Color = vec3(fTexCoords, 0.0);
		break;

	case 1: // Mesh tex coords
		TexCoord = fTexCoords;
		break;

	case 2: // Project XZ
		TexCoord = fObjectPosition.xz / 2.0 + vec2(0.5);
		break;
	}

	if (uMode > 0)
	{
		Color *= texture(uTexture, TexCoord).rgb;
	}

	outColor.a = 1.0;
	outColor.rgb = Color;
}
