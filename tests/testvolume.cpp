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

#include "testvolume.h"

#include "PolyVoxCore/LargeVolume.h"
#include "PolyVoxCore/RawVolume.h"
#include "PolyVoxCore/SimpleVolume.h"

#include <QtTest>

using namespace PolyVox;

template <typename VolumeType>
int32_t complexVolumeTest(void)
{
	//Create the volume
	VolumeType testVolume(Region(-57, -31, 12, 64, 96, 131)); // Deliberatly awkward size

	//Fill the volume with some data
	for(int z = testVolume.getEnclosingRegion().getLowerZ(); z <= testVolume.getEnclosingRegion().getUpperZ(); z++)
	{
		for(int y = testVolume.getEnclosingRegion().getLowerY(); y <= testVolume.getEnclosingRegion().getUpperY(); y++)
		{
			for(int x = testVolume.getEnclosingRegion().getLowerX(); x <= testVolume.getEnclosingRegion().getUpperX(); x++)
			{
				testVolume.setVoxelAt(x, y, z, x + y + z);
			}
		}
	}

	int32_t result = 0;

	//Test the getVoxel function
	for(int z = testVolume.getEnclosingRegion().getLowerZ(); z <= testVolume.getEnclosingRegion().getUpperZ(); z++)
	{
		for(int y = testVolume.getEnclosingRegion().getLowerY(); y <= testVolume.getEnclosingRegion().getUpperY(); y++)
		{
			for(int x = testVolume.getEnclosingRegion().getLowerX(); x <= testVolume.getEnclosingRegion().getUpperX(); x++)
			{
				result += testVolume.getVoxel(x, y, z);
			}
		}
	}

	//Test border wrap mode
	for(int z = testVolume.getEnclosingRegion().getLowerZ(); z <= testVolume.getEnclosingRegion().getUpperZ(); z++)
	{
		//Extending outside in y
		for(int y = testVolume.getEnclosingRegion().getLowerY() - 3; y <= testVolume.getEnclosingRegion().getUpperY() + 5; y++)
		{
			for(int x = testVolume.getEnclosingRegion().getLowerX(); x <= testVolume.getEnclosingRegion().getUpperX(); x++)
			{
				result += testVolume.getVoxelWithWrapping(x, y, z, WrapModes::Border, 3);
			}
		}
	}

	//Test clamp wrap mode
	for(int z = testVolume.getEnclosingRegion().getLowerZ(); z <= testVolume.getEnclosingRegion().getUpperZ(); z++)
	{
		for(int y = testVolume.getEnclosingRegion().getLowerY(); y <= testVolume.getEnclosingRegion().getUpperY(); y++)
		{
			//Extending outside in x
			for(int x = testVolume.getEnclosingRegion().getLowerX() - 2; x <= testVolume.getEnclosingRegion().getUpperX() + 4; x++)
			{
				result += testVolume.getVoxelWithWrapping(x, y, z, WrapModes::Clamp);
			}
		}
	}

	//Test the sampler setPosition
	VolumeType::Sampler sampler(&testVolume);
	sampler.setWrapMode(WrapModes::Border, 1);

	for(int z = testVolume.getEnclosingRegion().getLowerZ() - 2; z <= testVolume.getEnclosingRegion().getUpperZ() + 1; z++)
	{
		for(int y = testVolume.getEnclosingRegion().getLowerY() - 1; y <= testVolume.getEnclosingRegion().getUpperY() + 3; y++)
		{
			for(int x = testVolume.getEnclosingRegion().getLowerX() - 4; x <= testVolume.getEnclosingRegion().getUpperX() + 2; x++)
			{
				sampler.setPosition(x,y,z);
				result += sampler.getVoxel();
			}
		}
	}

	//Test the sampler move functions
	typename VolumeType::Sampler xSampler(&testVolume);
	typename VolumeType::Sampler ySampler(&testVolume);
	typename VolumeType::Sampler zSampler(&testVolume);

	xSampler.setWrapMode(WrapModes::Border, 1);
	ySampler.setWrapMode(WrapModes::Clamp, 1);
	zSampler.setWrapMode(WrapModes::Border, -3);

	zSampler.setPosition(testVolume.getEnclosingRegion().getLowerX() - 4, testVolume.getEnclosingRegion().getLowerY() - 1, testVolume.getEnclosingRegion().getLowerZ() - 2);
	for(int z = testVolume.getEnclosingRegion().getLowerZ() - 2; z <= testVolume.getEnclosingRegion().getUpperZ() + 1; z++)
	{
		ySampler = zSampler;
		for(int y = testVolume.getEnclosingRegion().getLowerY() - 1; y <= testVolume.getEnclosingRegion().getUpperY() + 3; y++)
		{
			xSampler = ySampler;
			for(int x = testVolume.getEnclosingRegion().getLowerX() - 4; x <= testVolume.getEnclosingRegion().getUpperX() + 2; x++)
			{
				result += xSampler.getVoxel();
				xSampler.movePositiveX();
			}
			ySampler.movePositiveY();
		}
		zSampler.movePositiveZ();
	}

	xSampler.setWrapMode(WrapModes::Clamp);
	ySampler.setWrapMode(WrapModes::Border, 1);
	zSampler.setWrapMode(WrapModes::Clamp, -1);

	zSampler.setPosition(testVolume.getEnclosingRegion().getUpperX() + 2, testVolume.getEnclosingRegion().getUpperY() + 3, testVolume.getEnclosingRegion().getUpperZ() + 1);
	for(int z = 0; z < testVolume.getEnclosingRegion().getDepthInVoxels() + 8; z++)
	{
		ySampler = zSampler;
		for(int y = 0; y < testVolume.getEnclosingRegion().getHeightInVoxels() + 3; y++)
		{
			xSampler = ySampler;
			for(int x = 0; x < testVolume.getEnclosingRegion().getWidthInVoxels() + 5; x++)
			{
				result += xSampler.getVoxel();
				xSampler.moveNegativeX();
			}
			ySampler.moveNegativeY();
		}
		zSampler.moveNegativeZ();
	}

	return result;
}

void TestVolume::testLargeVolume()
{
	int32_t result = complexVolumeTest< LargeVolume<int32_t> >();
	QCOMPARE(result, static_cast<int32_t>(1244008559));
}

void TestVolume::testRawVolume()
{
	int32_t result = complexVolumeTest< RawVolume<int32_t> >();
	QCOMPARE(result, static_cast<int32_t>(1244008559));
}

void TestVolume::testSimpleVolume()
{
	int32_t result = complexVolumeTest< SimpleVolume<int32_t> >();
	QCOMPARE(result, static_cast<int32_t>(1244008559));
}

QTEST_MAIN(TestVolume)
