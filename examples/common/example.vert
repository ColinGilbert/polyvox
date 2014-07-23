#version 140

in vec4 position; // This will be the position of the vertex in model-space
in vec3 normal;

// The usual matrices are provided
uniform mat4 cameraToClipMatrix;
uniform mat4 worldToCameraMatrix;
uniform mat4 modelToWorldMatrix;

// This will be used by the fragment shader to calculate flat-shaded normals. This is an unconventional approach
// but we use it in this example framework because not all surface extractor generate surface normals.
out vec4 worldPosition;
out vec3 worldNormal;

void main()
{
	// Standard sequence of OpenGL transformations.
	worldPosition = modelToWorldMatrix * position;
	vec4 cameraPosition = worldToCameraMatrix * worldPosition;
	
	worldNormal = normal;
	
	gl_Position = cameraToClipMatrix * cameraPosition;
}
