/*******************************************************************************
Copyright (c) 2010 Matt Williams

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

#include "TestRaycast.h"

#include "PolyVox/Density.h"
#include "PolyVox/Raycast.h"
#include "PolyVox/RawVolume.h"

#include "PolyVox/Impl/RandomUnitVectors.h"

#include <QtTest>

using namespace PolyVox;

// This is the callback functor which is called by the raycast() function for every voxel it touches.
// It's primary purpose is to tell the raycast whether or not to continue (i.e. it tests whether the
// ray has hit a solid voxel). Because the instance of this class is passed to the raycast() function
// by reference we can also use it to encapsulate some state. We're testing this by counting the total
// number of voxels touched.
class RaycastTestFunctor
{
public:
	RaycastTestFunctor()
		:m_uVoxelsTouched(0)
		,m_bRayLeftVolume(false)
	{
	}

	bool operator()(const RawVolume<int8_t>::Sampler& sampler)
	{
		m_uVoxelsTouched++;

		// For this particular test we know that we are always starting a ray inside the volume,
		// so if it ever leaves the volume we know it can't go back in and so we can terminate early.
		// This optimisation is worthwhile because samplers get slow once outside the volume.
		if(!sampler.isCurrentPositionValid())
		{
			m_bRayLeftVolume = true;
			return false;
		}

		// We are in the volume, so decide whether to continue based on the voxel value.
		return sampler.getVoxel() <= 0;
	}

	uint32_t m_uVoxelsTouched;
	bool m_bRayLeftVolume;
};

void TestRaycast::testExecute()
{
	const int32_t uVolumeSideLength = 32;

	//Create a hollow volume, with solid sides on x and y but with open ends in z.
	RawVolume<int8_t> volData(Region(Vector3DInt32(0, 0, 0), Vector3DInt32(uVolumeSideLength - 1, uVolumeSideLength - 1, uVolumeSideLength - 1)));
	for (int32_t z = 0; z < uVolumeSideLength; z++)
	{
		for (int32_t y = 0; y < uVolumeSideLength; y++)
		{
			for (int32_t x = 0; x < uVolumeSideLength; x++)
			{
				if((x == 0) || (x == uVolumeSideLength-1) || (y == 0) || (y == uVolumeSideLength-1))
				{
					volData.setVoxel(x, y, z, 100);
				}
				else
				{
					volData.setVoxel(x, y, z, -100);
				}				
			}
		}
	}

	//Cast rays from the centre. Roughly 2/3 should escape.
	Vector3DFloat start (uVolumeSideLength / 2, uVolumeSideLength / 2, uVolumeSideLength / 2);	

	// We could have counted the total number of hits in the same way as the total number of voxels
	// touched, but for demonstration and testing purposes we are making use of the raycast return value
	// and counting them seperatly in this variable.
	int hits = 0;
	uint32_t uTotalVoxelsTouched = 0;

	// Cast a large number of random rays
	for(int ct = 0; ct < 1000000; ct++)
	{
		RaycastTestFunctor raycastTestFunctor;
		RaycastResult result = raycastWithDirection(&volData, start, randomUnitVectors[ct % 1024] * 1000.0f, raycastTestFunctor);

		uTotalVoxelsTouched += raycastTestFunctor.m_uVoxelsTouched;

		// If the raycast completed then we know it did not hit anything.If it was interupted then it
		// probably hit something, unless we noted that the reason it was interupted was that it left the volume.
		if((result == RaycastResults::Interupted) && (raycastTestFunctor.m_bRayLeftVolume == false))
		{
			hits++;
		}
	}	

	// Check the number of hits.
	QCOMPARE(hits, 687494);

	// Check the total number of voxels touched
	QCOMPARE(uTotalVoxelsTouched, static_cast<uint32_t>(29783248));
}

QTEST_MAIN(TestRaycast)
