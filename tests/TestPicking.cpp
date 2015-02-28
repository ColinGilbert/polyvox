/*******************************************************************************
Copyright (c) 2013 Matt Williams

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

#include "TestPicking.h"

#include "PolyVox/Picking.h"
#include "PolyVox/RawVolume.h"

#include <QtTest>

using namespace PolyVox;

void TestPicking::testExecute()
{
	const int32_t uVolumeSideLength = 32;

	RawVolume<int8_t> volData(Region(Vector3DInt32(0, 0, 0), Vector3DInt32(uVolumeSideLength - 1, uVolumeSideLength - 1, uVolumeSideLength - 1)));
	for (int32_t z = 0; z < uVolumeSideLength; z++)
	{
		for (int32_t y = 0; y < uVolumeSideLength; y++)
		{
			for (int32_t x = 0; x < uVolumeSideLength; x++)
			{
				if((x > uVolumeSideLength/2)) //x > 16 is filled
				{
					volData.setVoxel(x, y, z, 100);
				}
				else
				{
					volData.setVoxel(x, y, z, 0);
				}
			}
		}
	}

	const int8_t emptyVoxelExample = 0; //A voxel value of zero will represent empty space.	
	PickResult resultHit = pickVoxel(&volData, Vector3DFloat(0, uVolumeSideLength / 2, uVolumeSideLength / 2), Vector3DFloat(uVolumeSideLength, 0, 0), emptyVoxelExample);
	
	QCOMPARE(resultHit.didHit, true);
	QCOMPARE(resultHit.hitVoxel, Vector3DInt32((uVolumeSideLength / 2) + 1, uVolumeSideLength / 2, uVolumeSideLength / 2));
	QCOMPARE(resultHit.previousVoxel, Vector3DInt32((uVolumeSideLength / 2), uVolumeSideLength / 2, uVolumeSideLength / 2));
	
	PickResult resultMiss = pickVoxel(&volData, Vector3DFloat(0, uVolumeSideLength / 2, uVolumeSideLength / 2), Vector3DFloat(uVolumeSideLength / 2, uVolumeSideLength, uVolumeSideLength), emptyVoxelExample);
	
	QCOMPARE(resultMiss.didHit, false);
}

QTEST_MAIN(TestPicking)
