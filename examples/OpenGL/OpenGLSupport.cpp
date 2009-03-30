#include "OpenGLSupport.h"

using namespace PolyVox;

OpenGLColour convertMaterialIDToColour(PolyVox::uint8_t materialID)
{
	OpenGLColour colour;

	switch(materialID)
	{
	case 1:
		colour.red = 1.0f;
		colour.green = 0.0f;
		colour.blue = 0.0f;
		break;
	case 2:
		colour.red = 0.0f;
		colour.green = 1.0f;
		colour.blue = 0.0f;
		break;
	case 3:
		colour.red = 0.0f;
		colour.green = 0.0f;
		colour.blue = 1.0f;
		break;
	case 4:
		colour.red = 1.0f;
		colour.green = 1.0f;
		colour.blue = 0.0f;
		break;
	case 5:
		colour.red = 1.0f;
		colour.green = 0.0f;
		colour.blue = 1.0f;
		break;
	default:
		colour.red = 1.0f;
		colour.green = 1.0f;
		colour.blue = 1.0f;
	}

	return colour;
}