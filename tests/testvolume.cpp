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
	//VolumeType testVolume(Region(-57, -31, 12, 64, 96, 131)); // Deliberatly awkward size
	VolumeType testVolume(Region(0, 0, 0, 63, 63, 63));
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

	int32_t sum = 0;

	for(int z = testVolume.getEnclosingRegion().getLowerZ(); z <= testVolume.getEnclosingRegion().getUpperZ(); z++)
	{
		for(int y = testVolume.getEnclosingRegion().getLowerY(); y <= testVolume.getEnclosingRegion().getUpperY(); y++)
		{
			for(int x = testVolume.getEnclosingRegion().getLowerX(); x <= testVolume.getEnclosingRegion().getUpperX(); x++)
			{
				sum += testVolume.getVoxelAt(x, y, z);
			}
		}
	}

	return sum;
}

void TestVolume::testLargeVolume()
{
	int32_t result = complexVolumeTest< LargeVolume<int32_t> >();
	QCOMPARE(result, static_cast<int32_t>(24772608));
}

void TestVolume::testRawVolume()
{
	int32_t result = complexVolumeTest< RawVolume<int32_t> >();
	QCOMPARE(result, static_cast<int32_t>(24772608));
}

void TestVolume::testSimpleVolume()
{
	int32_t result = complexVolumeTest< SimpleVolume<int32_t> >();
	QCOMPARE(result, static_cast<int32_t>(24772608));
}

QTEST_MAIN(TestVolume)
