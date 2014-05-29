#version 140

in uvec4 position; // This will be the position of the vertex in model-space
in vec4 normal;

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
	
	worldNormal = normal;
	
	// Standard sequence of OpenGL transformations.
	worldPosition = modelToWorldMatrix * decodedPosition;
	vec4 cameraPosition = worldToCameraMatrix * worldPosition;
	gl_Position = cameraToClipMatrix * cameraPosition;
}
