#ifndef __OpenGLExample_OpenGLVertexBufferObjectSupport_H__
#define __OpenGLExample_OpenGLVertexBufferObjectSupport_H__

#include "PolyVoxCore/PolyVoxForwardDeclarations.h"

#include "glew/glew.h"

struct OpenGLSurfacePatch
{
	GLulong noOfIndices;
	GLuint indexBuffer;
	GLuint vertexBuffer;
};

OpenGLSurfacePatch BuildOpenGLSurfacePatch(PolyVox::IndexedSurfacePatch& isp);

#endif //__OpenGLExample_OpenGLVertexBufferObjectSupport_H__
