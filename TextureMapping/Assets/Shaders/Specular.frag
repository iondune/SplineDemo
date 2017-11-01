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
uniform int uTexCoordMode;
uniform int uShowTexCoords;

out vec4 outColor;


float sq(float v)
{
	return v * v;
}

const float Pi = 3.14159;



vec2 CubeMap(vec3 p)
{
	if (abs(p.x) > abs(p.y) && abs(p.x) > abs(p.z))
	{
		return vec2(abs(p.y * 1.414 + 0.5), abs(p.z * 1.414 + 0.5));
	}
	else if (abs(p.y) > abs(p.z))
	{
		return vec2(abs(p.x * 1.414 + 0.5), abs(p.z * 1.414 + 0.5));
	}
	else
	{
		return vec2(abs(p.x * 1.414 + 0.5), abs(p.y * 1.414 + 0.5));
	}
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

	switch (uTexCoordMode)
	{
	default:
	case 0: // Mesh tex coords
		TexCoord = vec2(1.0 - fTexCoords.x, fTexCoords.y);
		break;

	case 1: // Project XY
		TexCoord = fObjectPosition.xy + vec2(0.5);
		break;

	case 2: // Spherical
		TexCoord = vec2(asin(nNormal.x) / Pi + 0.5, asin(nNormal.y) / Pi + 0.5);
		break;

	case 3: // Cube map
		TexCoord = CubeMap(fObjectPosition);
		break;
	}

	if (uShowTexCoords != 0)
	{
		Color = vec3(TexCoord, 0.0);
	}
	else
	{
		Color *= texture(uTexture, TexCoord).rgb;
	}

	outColor.a = 1.0;
	outColor.rgb = Color;
}
