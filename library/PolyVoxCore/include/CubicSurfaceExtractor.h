/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*******************************************************************************/

#ifndef __PolyVox_CubicSurfaceExtractor_H__
#define __PolyVox_CubicSurfaceExtractor_H__

#include "Array.h"
#include "PolyVoxForwardDeclarations.h"
#include "VolumeSampler.h"

#include "PolyVoxImpl/TypeDef.h"

namespace PolyVox
{
	struct IndexAndMaterial
	{
		int32_t iIndex    : 24;
		int32_t uMaterial : 8;
	};

	template <typename VoxelType>
	class CubicSurfaceExtractor
	{
	public:
		CubicSurfaceExtractor(Volume<VoxelType>* volData, Region region, SurfaceMesh<PositionMaterial>* result);

		void execute();

		int32_t addVertex(float fX, float fY, float fZ, uint8_t uMaterial, Array<3, IndexAndMaterial>& existingVertices);

	private:
		//The volume data and a sampler to access it.
		Volume<VoxelType>* m_volData;
		VolumeSampler<VoxelType> m_sampVolume;

		//Information about the region we are currently processing
		Region m_regSizeInVoxels;

		//The surface patch we are currently filling.
		SurfaceMesh<PositionMaterial>* m_meshCurrent;

		//Array<4, IndexAndMaterial> m_vertices;
		Array<3, IndexAndMaterial> m_previousSliceVertices;
		Array<3, IndexAndMaterial> m_currentSliceVertices;

		//Although we try to avoid creating multiple vertices at the same location, sometimes this is unavoidable
		//if they have different materials. For example, four different materials next to each other would mean
		//four quads (though more triangles) sharing the vertex. As far as I can tell, four is the worst case scenario.
		static const uint32_t MaxQuadsSharingVertex;
	};
}

#include "CubicSurfaceExtractor.inl"

#endif
