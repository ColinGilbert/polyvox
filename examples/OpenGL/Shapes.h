#ifndef __OpenGLExample_Shapes_H__
#define __OpenGLExample_Shapes_H__

#include "PolyVoxCore/Volume.h"

void createSphereInVolume(PolyVox::Volume<PolyVox::uint8>& volData, float fRadius, PolyVox::uint8 uValue);
void createCubeInVolume(PolyVox::Volume<PolyVox::uint8>& volData, PolyVox::Vector3DUint16 lowerCorner, PolyVox::Vector3DUint16 upperCorner, PolyVox::uint8 uValue);

#endif //__OpenGLExample_Shapes_H__