#include "OpenGLSupport.h"

using namespace PolyVox;

OpenGLColour convertMaterialIDToColour(PolyVox::uint8 materialID)
{
	OpenGLColour colour;

	switch(materialID)
	{
	case 1:
		colour.red = 1.0;
		colour.green = 0.0;
		colour.blue = 0.0;
		break;
	case 2:
		colour.red = 0.0;
		colour.green = 1.0;
		colour.blue = 0.0;
		break;
	case 3:
		colour.red = 0.0;
		colour.green = 0.0;
		colour.blue = 1.0;
		break;
	case 4:
		colour.red = 1.0;
		colour.green = 1.0;
		colour.blue = 0.0;
		break;
	case 5:
		colour.red = 1.0;
		colour.green = 0.0;
		colour.blue = 1.0;
		break;
	}

	return colour;
}