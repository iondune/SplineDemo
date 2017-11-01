
#version 330

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoords;

uniform mat4 uModelMatrix;
uniform mat4 uNormalMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out vec3 fObjectPosition;
out vec3 fWorldPosition;
out vec3 fNormal;
out vec2 fTexCoords;


void main()
{
	vec4 Position = uModelMatrix * vec4(vPosition, 1.0);

	fObjectPosition = vPosition;
	fWorldPosition = Position.xyz;
	fTexCoords = vTexCoords;

	fNormal = vec3(uNormalMatrix * vec4(vNormal, 0.0));

	gl_Position = uProjectionMatrix * uViewMatrix * Position;
}
