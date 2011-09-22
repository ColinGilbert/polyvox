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

#ifndef __AmbientOcclusionCalculator_H__
#define __AmbientOcclusionCalculator_H__

#include "PolyVoxImpl/RandomUnitVectors.h"
#include "PolyVoxImpl/RandomVectors.h"

#include "PolyVoxCore/Array.h"
#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/Raycast.h"

#include <algorithm>

namespace PolyVox
{
	template< template<typename> class VolumeType, typename VoxelType>
	class AmbientOcclusionCalculator
	{
	public:
		AmbientOcclusionCalculator(VolumeType<VoxelType>* volInput, Array<3, uint8_t>* arrayResult, Region region, float fRayLength, uint8_t uNoOfSamplesPerOutputElement);
		~AmbientOcclusionCalculator();

		void execute(void);

	private:
		Region m_region;
		typename VolumeType<VoxelType>::Sampler m_sampVolume;
		VolumeType<VoxelType>* m_volInput;
		Array<3, uint8_t>* m_arrayResult;
		float m_fRayLength;

		uint8_t m_uNoOfSamplesPerOutputElement;

		uint16_t mRandomUnitVectorIndex;
		uint16_t mRandomVectorIndex;
		uint16_t mIndexIncreament;
	};
}

#include "PolyVoxCore/AmbientOcclusionCalculator.inl"

#endif //__AmbientOcclusionCalculator_H__
