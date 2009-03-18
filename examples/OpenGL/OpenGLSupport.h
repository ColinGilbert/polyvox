#ifndef __OpenGLExample_OpenGLSupport_H__
#define __OpenGLExample_OpenGLSupport_H__

#include "PolyVoxCore/PolyVoxForwardDeclarations.h"

#include "glew/glew.h"

struct OpenGLColour
{
	GLfloat red;
	GLfloat green;
	GLfloat blue;
};

OpenGLColour convertMaterialIDToColour(PolyVox::uint8 materialID);

#endif //__OpenGLExample_OpenGLSupport_H__