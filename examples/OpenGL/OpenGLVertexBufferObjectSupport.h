#ifndef __OpenGLExample_OpenGLVertexBufferObjectSupport_H__
#define __OpenGLExample_OpenGLVertexBufferObjectSupport_H__

#include "PolyVoxForwardDeclarations.h"

#include "glew/glew.h"

struct OpenGLSurfacePatch
{
	GLulong noOfIndices;
	GLuint indexBuffer;
	GLuint vertexBuffer;
	const PolyVox::IndexedSurfacePatch* sourceISP;
};

OpenGLSurfacePatch BuildOpenGLSurfacePatch(const PolyVox::IndexedSurfacePatch& isp);
void renderRegionVertexBufferObject(const OpenGLSurfacePatch& openGLSurfacePatch, unsigned int uLodLevel);

#endif //__OpenGLExample_OpenGLVertexBufferObjectSupport_H__
