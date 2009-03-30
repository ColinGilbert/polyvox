#ifndef __OpenGLExample_Shapes_H__
#define __OpenGLExample_Shapes_H__

#include "PolyVoxCore/Volume.h"

void createSphereInVolume(PolyVox::Volume<PolyVox::uint8_t>& volData, float fRadius, PolyVox::uint8_t uValue);
void createCubeInVolume(PolyVox::Volume<PolyVox::uint8_t>& volData, PolyVox::Vector3DUint16 lowerCorner, PolyVox::Vector3DUint16 upperCorner, PolyVox::uint8_t uValue);

#endif //__OpenGLExample_Shapes_H__