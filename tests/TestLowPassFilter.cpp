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

#include "TestLowPassFilter.h"

#include "PolyVoxCore/Density.h"
#include "PolyVoxCore/LowPassFilter.h"
#include "PolyVoxCore/RawVolume.h"

#include <QtTest>

using namespace PolyVox;

void TestLowPassFilter::testExecute()
{
	const int32_t g_uVolumeSideLength = 16;

	Region reg(Vector3DInt32(0,0,0), Vector3DInt32(g_uVolumeSideLength-1, g_uVolumeSideLength-1, g_uVolumeSideLength-1));

	//Create empty volume
	RawVolume<Density8> volData(reg);

	//Create two solid walls at opposite sides of the volume
	for (int32_t z = 0; z < g_uVolumeSideLength; z++)
	{
		for (int32_t y = 0; y < g_uVolumeSideLength; y++)
		{
			for (int32_t x = 0; x < g_uVolumeSideLength; x++)
			{
				if(x % 2 == 0)
				{
					Density8 voxel(32);
					volData.setVoxelAt(x, y, z, voxel);
				}
			}
		}
	}

	RawVolume<Density8> resultVolume(reg);

	LowPassFilter<RawVolume, RawVolume, Density8> pass1(&volData, reg, &resultVolume, reg, 5);

	pass1.executeSAT();

	std::cout << "Voxel = " << static_cast<int>(resultVolume.getVoxelAt(1,2,3).getDensity()) << std::endl; // 7
	std::cout << "Voxel = " << static_cast<int>(resultVolume.getVoxelAt(10,6,7).getDensity()) << std::endl; // 17
	std::cout << "Voxel = " << static_cast<int>(resultVolume.getVoxelAt(15,2,12).getDensity()) << std::endl; // 4
}

QTEST_MAIN(TestLowPassFilter)
