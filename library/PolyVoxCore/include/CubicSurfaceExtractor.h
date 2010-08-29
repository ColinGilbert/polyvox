#pragma region License
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
#pragma endregion

#ifndef __PolyVox_CubicSurfaceExtractor_H__
#define __PolyVox_CubicSurfaceExtractor_H__

#pragma region Headers
#include "PolyVoxForwardDeclarations.h"
#include "VolumeSampler.h"

#include "PolyVoxImpl/TypeDef.h"
#pragma endregion

namespace PolyVox
{
	template <typename VoxelType>
	class CubicSurfaceExtractor
	{
	public:
		CubicSurfaceExtractor(Volume<VoxelType>* volData, Region region, SurfaceMesh* result);

		void execute();

	private:
		//The volume data and a sampler to access it.
		Volume<VoxelType>* m_volData;
		VolumeSampler<VoxelType> m_sampVolume;

		//The surface patch we are currently filling.
		SurfaceMesh* m_meshCurrent;

		//Information about the region we are currently processing
		Region m_regSizeInVoxels;
		Region m_regSizeInCells;
	};
}

#include "CubicSurfaceExtractor.inl"

#endif
