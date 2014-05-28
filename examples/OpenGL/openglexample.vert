#version 140

in vec4 position; // This will be the position of the vertex in model-space
in vec4 normal; // The normal data may not have been set
in ivec2 material;

uniform mat4 cameraToClipMatrix;
uniform mat4 worldToCameraMatrix;
uniform mat4 modelToWorldMatrix;

out vec4 worldPosition; //This is being passed to the fragment shader to calculate the normals
out vec3 normalFromVS;
flat out ivec2 materialFromVS;

void main()
{
	// Compute the usual OpenGL transformation to clip space.
	gl_Position = cameraToClipMatrix * worldToCameraMatrix * modelToWorldMatrix * position;
	
	// This example is demonstrating the marching cubes mesh, which does have per-vertex normals. We can 
	// just pass them through, though real code might want to deal with transforming normals appropriatly.
	normalFromVS = normal.xyz;
	
	// Nothing special here, we just pass the material through to the fragment shader.
	materialFromVS = material;
}
