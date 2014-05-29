#version 140

in uvec4 position; // This will be the position of the vertex in model-space
in uint normal;

// The usual matrices are provided
uniform mat4 cameraToClipMatrix;
uniform mat4 worldToCameraMatrix;
uniform mat4 modelToWorldMatrix;

// This will be used by the fragment shader to calculate flat-shaded normals. This is an unconventional approach
// but we use it in this example framework because not all surface extractor generate surface normals.
out vec4 worldPosition;
out vec4 worldNormal;

void main()
{
	vec4 decodedPosition = position;
	decodedPosition.xyz = decodedPosition.xyz * (1.0 / 256.0);
	
	uint encodedX = (normal >> 10u) & 0x1Fu;
	uint encodedY = (normal >> 5u) & 0x1Fu;
	uint encodedZ = (normal) & 0x1Fu;
	worldNormal.xyz = vec3(encodedX, encodedY, encodedZ);
	worldNormal.xyz = worldNormal.xyz / 15.5;
	worldNormal.xyz = worldNormal.xyz - vec3(1.0, 1.0, 1.0);
	worldNormal.w = 1.0;
	
	// Standard sequence of OpenGL transformations.
	worldPosition = modelToWorldMatrix * decodedPosition;
	vec4 cameraPosition = worldToCameraMatrix * worldPosition;
	gl_Position = cameraToClipMatrix * cameraPosition;
}
