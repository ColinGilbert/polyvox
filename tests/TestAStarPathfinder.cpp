/*******************************************************************************
Copyright (c) 2011 David Williams

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

#include "TestAStarPathfinder.h"

#include "PolyVoxCore/AStarPathfinder.h"
#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/RawVolume.h"

#include <QtTest>

using namespace PolyVox;

void TestAStarPathfinder::testExecute()
{
	//The expected path
	//The following results work on Linux/GCC
	const Vector3DInt32 expectedResult[] = 
	{
		Vector3DInt32(0,0,0),
		Vector3DInt32(1,1,1),
		Vector3DInt32(2,2,1),
		Vector3DInt32(3,3,1),
		Vector3DInt32(4,4,1),
		Vector3DInt32(4,5,1),
		Vector3DInt32(5,6,1),
		Vector3DInt32(6,7,2),
		Vector3DInt32(7,8,3),
		Vector3DInt32(8,9,3),
		Vector3DInt32(9,10,3),
		Vector3DInt32(10,11,3),
		Vector3DInt32(11,12,4),
		Vector3DInt32(12,13,5),
		Vector3DInt32(13,13,6),
		Vector3DInt32(13,13,7),
		Vector3DInt32(13,13,8),
		Vector3DInt32(13,13,9),
		Vector3DInt32(14,14,10),
		Vector3DInt32(14,14,11),
		Vector3DInt32(14,14,12),
		Vector3DInt32(14,14,13),
		Vector3DInt32(15,15,14),
		Vector3DInt32(15,15,15),
	};
	
	//The following results work on Windows/VS2010
	/*const Vector3DInt32 expectedResult[] = 
	{
		Vector3DInt32(0,0,0),
		Vector3DInt32(1,1,1),
		Vector3DInt32(1,2,1),
		Vector3DInt32(1,3,1),
		Vector3DInt32(1,4,1),
		Vector3DInt32(1,5,1),
		Vector3DInt32(1,6,1),
		Vector3DInt32(1,7,1),
		Vector3DInt32(1,8,1),
		Vector3DInt32(2,9,2),
		Vector3DInt32(3,10,3),
		Vector3DInt32(3,11,4),
		Vector3DInt32(4,12,5),
		Vector3DInt32(5,12,5),
		Vector3DInt32(6,13,6),
		Vector3DInt32(7,13,7),
		Vector3DInt32(8,13,8),
		Vector3DInt32(9,13,9),
		Vector3DInt32(10,13,10),
		Vector3DInt32(11,13,11),
		Vector3DInt32(12,13,12),
		Vector3DInt32(13,13,13),
		Vector3DInt32(14,14,14),
		Vector3DInt32(15,15,15)
	};*/

	//Create an empty volume
	RawVolume<Material8> volData(Region(Vector3DInt32(0,0,0), Vector3DInt32(15, 15, 15)));

	//Place a solid cube in the middle of it
	for(int z = 4; z < 12; z++)
	{
		for(int y = 4; y < 12; y++)
		{
			for(int x = 4; x < 12; x++)
			{
				Material8 solidVoxel(1);
				volData.setVoxelAt(x,y,z,solidVoxel);
			}
		}
	}

	//List to store the resulting path
	std::list<Vector3DInt32> result;

	//Create an AStarPathfinder
	AStarPathfinderParams<RawVolume, Material8> params(&volData, Vector3DInt32(0,0,0), Vector3DInt32(15,15,15), &result);
	AStarPathfinder<RawVolume, Material8> pathfinder(params);

	//Execute the pathfinder.
	pathfinder.execute();

	//Make sure the right number of steps were created.
	QCOMPARE(result.size(), static_cast<size_t>(24));

	//Make sure that each step is correct.
	uint32_t uExpectedIndex = 0;
	for(std::list<Vector3DInt32>::iterator iterResult = result.begin(); iterResult != result.end(); iterResult++)
	{
		Vector3DInt32 res = *iterResult;
		Vector3DInt32 exp = expectedResult[uExpectedIndex];
		QCOMPARE(res, exp);
		uExpectedIndex++;
	}
}

QTEST_MAIN(TestAStarPathfinder)
