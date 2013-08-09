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

#include "PolyVoxCore/FilePager.h"
#include "PolyVoxCore/LargeVolume.h"
#include "PolyVoxCore/MinizBlockCompressor.h"
#include "PolyVoxCore/RawVolume.h"
#include "PolyVoxCore/SimpleVolume.h"

#include <QtGlobal>
#include <QtTest>

using namespace PolyVox;

// This is used to compute a value from a list of integers. We use it to 
// make sure we get the expected result from a series of volume accesses.
inline int32_t cantorTupleFunction(int32_t previousResult, int32_t value)
{
	return (( previousResult + value  ) * ( previousResult + value + 1 ) + value ) / 2;
}

/*
 * Funtions for testing iteration in a forwards direction
 */

// We allow user provided offset in this function so we can test the case when all samples are inside a volume and also the case when some samples are outside.
// This is important because samplers are often slower when outside the volume as they have to fall back on directly accessing the volume data.
template <typename VolumeType>
int32_t testDirectAccessWithWrappingForwards(const VolumeType* volume, int lowXOffset, int lowYOffset, int lowZOffset, int highXOffset, int highYOffset, int highZOffset)
{
	int32_t result = 0;

	// If we know that we are only iterating over voxels internal to the volume then we can avoid calling the 'wrapping' function. This should be faster.
	bool bAllVoxelsInternal = (lowXOffset > 0) && (lowYOffset > 0) && (lowZOffset > 0) && (highXOffset < 0) && (highYOffset < 0) && (highZOffset < 0);

	for(int z = volume->getEnclosingRegion().getLowerZ() + lowZOffset; z <= volume->getEnclosingRegion().getUpperZ() + highZOffset; z++)
	{
		for(int y = volume->getEnclosingRegion().getLowerY() + lowYOffset; y <= volume->getEnclosingRegion().getUpperY() + highYOffset; y++)
		{
			for(int x = volume->getEnclosingRegion().getLowerX() + lowXOffset; x <= volume->getEnclosingRegion().getUpperX() + highXOffset; x++)
			{
				//Three level loop now processes 27 voxel neighbourhood
				for(int innerZ = -1; innerZ <=1; innerZ++)
				{
					for(int innerY = -1; innerY <=1; innerY++)
					{
						for(int innerX = -1; innerX <=1; innerX++)
						{
							// Deeply nested 'if', but this is just a unit test and we should still
							// see some performance improvement by skipping the wrapping versions.
							if(bAllVoxelsInternal)
							{
								result = cantorTupleFunction(result, volume->getVoxel(x + innerX, y + innerY, z + innerZ));
							}
							else
							{
								result = cantorTupleFunction(result, volume->getVoxel(x + innerX, y + innerY, z + innerZ, WrapModes::Border, 3));
							}
						}
					}
				}	
				//End of inner loops
			}
		}
	}

	return result;
}

template <typename VolumeType>
int32_t testSamplersWithWrappingForwards(VolumeType* volume, int lowXOffset, int lowYOffset, int lowZOffset, int highXOffset, int highYOffset, int highZOffset)
{
	int32_t result = 0;

	//Test the sampler move functions
	typename VolumeType::Sampler xSampler(volume);
	typename VolumeType::Sampler ySampler(volume);
	typename VolumeType::Sampler zSampler(volume);

	xSampler.setWrapMode(WrapModes::Border, 3);
	ySampler.setWrapMode(WrapModes::Border, 3);
	zSampler.setWrapMode(WrapModes::Border, 3);

	zSampler.setPosition(volume->getEnclosingRegion().getLowerX() + lowXOffset, volume->getEnclosingRegion().getLowerY() + lowYOffset, volume->getEnclosingRegion().getLowerZ() + lowZOffset);
	for(int z = volume->getEnclosingRegion().getLowerZ() + lowZOffset; z <= volume->getEnclosingRegion().getUpperZ() + highZOffset; z++)
	{
		ySampler = zSampler;
		for(int y = volume->getEnclosingRegion().getLowerY() + lowYOffset; y <= volume->getEnclosingRegion().getUpperY() + highYOffset; y++)
		{
			xSampler = ySampler;
			for(int x = volume->getEnclosingRegion().getLowerX() + lowXOffset; x <= volume->getEnclosingRegion().getUpperX() + highXOffset; x++)
			{
				xSampler.setPosition(x, y, z); // HACK - Accessing a volume through multiple samplers currently breaks the LargeVolume.

				result = cantorTupleFunction(result, xSampler.peekVoxel1nx1ny1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px1ny1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px1ny1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1nx0py1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px0py1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px0py1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1nx1py1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px1py1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px1py1nz());

				result = cantorTupleFunction(result, xSampler.peekVoxel1nx1ny0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px1ny0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px1ny0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1nx0py0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px0py0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px0py0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1nx1py0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px1py0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px1py0pz());

				result = cantorTupleFunction(result, xSampler.peekVoxel1nx1ny1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px1ny1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px1ny1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1nx0py1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px0py1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px0py1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1nx1py1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px1py1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px1py1pz());

				xSampler.movePositiveX();
			}
			ySampler.movePositiveY();
		}
		zSampler.movePositiveZ();
	}

	return result;
}

/*
 * Funtions for testing iteration in a backwards direction
 */

// We allow user provided offset in this function so we can test the case when all samples are inside a volume and also the case when some samples are outside.
// This is important because samplers are often slower when outside the volume as they have to fall back on directly accessing the volume data.
template <typename VolumeType>
int32_t testDirectAccessWithWrappingBackwards(const VolumeType* volume, int lowXOffset, int lowYOffset, int lowZOffset, int highXOffset, int highYOffset, int highZOffset)
{
	int32_t result = 0;

	// If we know that we are only iterating over voxels internal to the volume then we can avoid calling the 'wrapping' function. This should be faster.
	bool bAllVoxelsInternal = (lowXOffset > 0) && (lowYOffset > 0) && (lowZOffset > 0) && (highXOffset < 0) && (highYOffset < 0) && (highZOffset < 0);

	for(int z = volume->getEnclosingRegion().getUpperZ() + highZOffset; z >= volume->getEnclosingRegion().getLowerZ() + lowZOffset; z--)
	{
		for(int y = volume->getEnclosingRegion().getUpperY() + highYOffset; y >= volume->getEnclosingRegion().getLowerY() + lowYOffset; y--)
		{
			for(int x = volume->getEnclosingRegion().getUpperX() + highXOffset; x >= volume->getEnclosingRegion().getLowerX() + lowXOffset; x--)
			{
				//Three level loop now processes 27 voxel neighbourhood
				for(int innerZ = -1; innerZ <=1; innerZ++)
				{
					for(int innerY = -1; innerY <=1; innerY++)
					{
						for(int innerX = -1; innerX <=1; innerX++)
						{
							// Deeply nested 'if', but this is just a unit test and we should still
							// see some performance improvement by skipping the wrapping versions.
							if(bAllVoxelsInternal)
							{
								result = cantorTupleFunction(result, volume->getVoxel(x + innerX, y + innerY, z + innerZ));
							}
							else
							{
								result = cantorTupleFunction(result, volume->getVoxel(x + innerX, y + innerY, z + innerZ, WrapModes::Border, 3));
							}
						}
					}
				}	
				//End of inner loops
			}
		}
	}

	return result;
}

template <typename VolumeType>
int32_t testSamplersWithWrappingBackwards(VolumeType* volume, int lowXOffset, int lowYOffset, int lowZOffset, int highXOffset, int highYOffset, int highZOffset)
{
	int32_t result = 0;

	//Test the sampler move functions
	typename VolumeType::Sampler xSampler(volume);
	typename VolumeType::Sampler ySampler(volume);
	typename VolumeType::Sampler zSampler(volume);

	xSampler.setWrapMode(WrapModes::Border, 3);
	ySampler.setWrapMode(WrapModes::Border, 3);
	zSampler.setWrapMode(WrapModes::Border, 3);

	zSampler.setPosition(volume->getEnclosingRegion().getUpperX() + highXOffset, volume->getEnclosingRegion().getUpperY() + highYOffset, volume->getEnclosingRegion().getUpperZ() + highZOffset);
	for(int z = volume->getEnclosingRegion().getUpperZ() + highZOffset; z >= volume->getEnclosingRegion().getLowerZ() + lowZOffset; z--)
	{
		ySampler = zSampler;
		for(int y = volume->getEnclosingRegion().getUpperY() + highYOffset; y >= volume->getEnclosingRegion().getLowerY() + lowYOffset; y--)
		{
			xSampler = ySampler;
			for(int x = volume->getEnclosingRegion().getUpperX() + highXOffset; x >= volume->getEnclosingRegion().getLowerX() + lowXOffset; x--)
			{
				xSampler.setPosition(x, y, z); // HACK - Accessing a volume through multiple samplers currently breaks the LargeVolume.

				result = cantorTupleFunction(result, xSampler.peekVoxel1nx1ny1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px1ny1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px1ny1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1nx0py1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px0py1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px0py1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1nx1py1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px1py1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px1py1nz());

				result = cantorTupleFunction(result, xSampler.peekVoxel1nx1ny0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px1ny0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px1ny0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1nx0py0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px0py0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px0py0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1nx1py0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px1py0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px1py0pz());

				result = cantorTupleFunction(result, xSampler.peekVoxel1nx1ny1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px1ny1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px1ny1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1nx0py1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px0py1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px0py1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1nx1py1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px1py1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px1py1pz());

				xSampler.moveNegativeX();
			}
			ySampler.moveNegativeY();
		}
		zSampler.moveNegativeZ();
	}

	return result;
}

TestVolume::TestVolume()
{
	Region region(-57, -31, 12, 64, 96, 131); // Deliberatly awkward size

	//m_pCompressor = new RLECompressor<int32_t, uint16_t>;
	m_pBlockCompressor = new MinizBlockCompressor<int32_t>;
	m_pFilePager = new FilePager<int32_t>("./");

	//Create the volumes
	m_pRawVolume = new RawVolume<int32_t>(region);
	m_pSimpleVolume = new SimpleVolume<int32_t>(region);
	m_pLargeVolume = new LargeVolume<int32_t>(region, m_pBlockCompressor, m_pFilePager, 32);

	m_pLargeVolume->setMaxNumberOfBlocksInMemory(32);
	m_pLargeVolume->setMaxNumberOfUncompressedBlocks(16);

	//Fill the volume with some data
	for(int z = region.getLowerZ(); z <= region.getUpperZ(); z++)
	{
		for(int y = region.getLowerY(); y <= region.getUpperY(); y++)
		{
			for(int x = region.getLowerX(); x <= region.getUpperX(); x++)
			{
				int32_t value = x + y + z;
				m_pRawVolume->setVoxelAt(x, y, z, value);
				m_pSimpleVolume->setVoxelAt(x, y, z, value);
				m_pLargeVolume->setVoxelAt(x, y, z, value);
			}
		}
	}
}

TestVolume::~TestVolume()
{
	delete m_pRawVolume;
	delete m_pSimpleVolume;
	delete m_pLargeVolume;

	delete m_pFilePager;
	delete m_pBlockCompressor;
}

/*
 * RawVolume Tests
 */

void TestVolume::testRawVolumeDirectAccessAllInternalForwards()
{
	int32_t result = 0;

	QBENCHMARK
	{
		result = testDirectAccessWithWrappingForwards(m_pRawVolume, 4, 2, 2, -3, -1, -2);
	}
	QCOMPARE(result, static_cast<int32_t>(1004598054));
}

void TestVolume::testRawVolumeSamplersAllInternalForwards()
{
	int32_t result = 0;

	QBENCHMARK
	{
		result = testSamplersWithWrappingForwards(m_pRawVolume, 4, 2, 2, -3, -1, -2);
	}
	QCOMPARE(result, static_cast<int32_t>(1004598054));
}

void TestVolume::testRawVolumeDirectAccessWithExternalForwards()
{
	int32_t result = 0;

	QBENCHMARK
	{
		result = testDirectAccessWithWrappingForwards(m_pRawVolume, -1, -3, -2, 2, 5, 4);
	}
	QCOMPARE(result, static_cast<int32_t>(-928601007));
}

void TestVolume::testRawVolumeSamplersWithExternalForwards()
{
	int32_t result = 0;

	QBENCHMARK
	{
		result = testSamplersWithWrappingForwards(m_pRawVolume, -1, -3, -2, 2, 5, 4);
	}
	QCOMPARE(result, static_cast<int32_t>(-928601007));
}

void TestVolume::testRawVolumeDirectAccessAllInternalBackwards()
{
	int32_t result = 0;

	QBENCHMARK
	{
		result = testDirectAccessWithWrappingBackwards(m_pRawVolume, 4, 2, 2, -3, -1, -2);
	}
	QCOMPARE(result, static_cast<int32_t>(-269366578));
}

void TestVolume::testRawVolumeSamplersAllInternalBackwards()
{
	int32_t result = 0;

	QBENCHMARK
	{
		result = testSamplersWithWrappingBackwards(m_pRawVolume, 4, 2, 2, -3, -1, -2);
	}
	QCOMPARE(result, static_cast<int32_t>(-269366578));
}

void TestVolume::testRawVolumeDirectAccessWithExternalBackwards()
{
	int32_t result = 0;

	QBENCHMARK
	{
		result = testDirectAccessWithWrappingBackwards(m_pRawVolume, -1, -3, -2, 2, 5, 4);
	}
	QCOMPARE(result, static_cast<int32_t>(-769775893));
}

void TestVolume::testRawVolumeSamplersWithExternalBackwards()
{
	int32_t result = 0;

	QBENCHMARK
	{
		result = testSamplersWithWrappingBackwards(m_pRawVolume, -1, -3, -2, 2, 5, 4);
	}
	QCOMPARE(result, static_cast<int32_t>(-769775893));
}

/*
 * SimpleVolume Tests
 */

void TestVolume::testSimpleVolumeDirectAccessAllInternalForwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testDirectAccessWithWrappingForwards(m_pSimpleVolume, 4, 2, 2, -3, -1, -2);
	}
	QCOMPARE(result, static_cast<int32_t>(1004598054));
}

void TestVolume::testSimpleVolumeSamplersAllInternalForwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testSamplersWithWrappingForwards(m_pSimpleVolume, 4, 2, 2, -3, -1, -2);
	}
	QCOMPARE(result, static_cast<int32_t>(1004598054));
}

void TestVolume::testSimpleVolumeDirectAccessWithExternalForwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testDirectAccessWithWrappingForwards(m_pSimpleVolume, -1, -3, -2, 2, 5, 4);
	}
	QCOMPARE(result, static_cast<int32_t>(-928601007));
}

void TestVolume::testSimpleVolumeSamplersWithExternalForwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testSamplersWithWrappingForwards(m_pSimpleVolume, -1, -3, -2, 2, 5, 4);
	}
	QCOMPARE(result, static_cast<int32_t>(-928601007));
}

void TestVolume::testSimpleVolumeDirectAccessAllInternalBackwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testDirectAccessWithWrappingBackwards(m_pSimpleVolume, 4, 2, 2, -3, -1, -2);
	}
	QCOMPARE(result, static_cast<int32_t>(-269366578));
}

void TestVolume::testSimpleVolumeSamplersAllInternalBackwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testSamplersWithWrappingBackwards(m_pSimpleVolume, 4, 2, 2, -3, -1, -2);
	}
	QCOMPARE(result, static_cast<int32_t>(-269366578));
}

void TestVolume::testSimpleVolumeDirectAccessWithExternalBackwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testDirectAccessWithWrappingBackwards(m_pSimpleVolume, -1, -3, -2, 2, 5, 4);
	}
	QCOMPARE(result, static_cast<int32_t>(-769775893));
}

void TestVolume::testSimpleVolumeSamplersWithExternalBackwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testSamplersWithWrappingBackwards(m_pSimpleVolume, -1, -3, -2, 2, 5, 4);
	}
	QCOMPARE(result, static_cast<int32_t>(-769775893));
}

/*
 * LargeVolume Tests
 */

void TestVolume::testLargeVolumeDirectAccessAllInternalForwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testDirectAccessWithWrappingForwards(m_pLargeVolume, 4, 2, 2, -3, -1, -2);
	}
	QCOMPARE(result, static_cast<int32_t>(1004598054));
}

void TestVolume::testLargeVolumeSamplersAllInternalForwards()
{
	int32_t result = 0;
	
	QBENCHMARK
	{
		result = testSamplersWithWrappingForwards(m_pLargeVolume, 4, 2, 2, -3, -1, -2);
	}
	QCOMPARE(result, static_cast<int32_t>(1004598054));
}

void TestVolume::testLargeVolumeDirectAccessWithExternalForwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testDirectAccessWithWrappingForwards(m_pLargeVolume, -1, -3, -2, 2, 5, 4);
	}
	QCOMPARE(result, static_cast<int32_t>(-928601007));
}

void TestVolume::testLargeVolumeSamplersWithExternalForwards()
{
	int32_t result = 0;
	
	QBENCHMARK
	{
		result = testSamplersWithWrappingForwards(m_pLargeVolume, -1, -3, -2, 2, 5, 4);
	}
	QCOMPARE(result, static_cast<int32_t>(-928601007));
}

void TestVolume::testLargeVolumeDirectAccessAllInternalBackwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testDirectAccessWithWrappingBackwards(m_pLargeVolume, 4, 2, 2, -3, -1, -2);
	}
	QCOMPARE(result, static_cast<int32_t>(-269366578));
}

void TestVolume::testLargeVolumeSamplersAllInternalBackwards()
{
	int32_t result = 0;
	
	QBENCHMARK
	{
		result = testSamplersWithWrappingBackwards(m_pLargeVolume, 4, 2, 2, -3, -1, -2);
	}
	QCOMPARE(result, static_cast<int32_t>(-269366578));
}

void TestVolume::testLargeVolumeDirectAccessWithExternalBackwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testDirectAccessWithWrappingBackwards(m_pLargeVolume, -1, -3, -2, 2, 5, 4);
	}
	QCOMPARE(result, static_cast<int32_t>(-769775893));
}

void TestVolume::testLargeVolumeSamplersWithExternalBackwards()
{
	int32_t result = 0;
	
	QBENCHMARK
	{
		result = testSamplersWithWrappingBackwards(m_pLargeVolume, -1, -3, -2, 2, 5, 4);
	}
	QCOMPARE(result, static_cast<int32_t>(-769775893));
}

QTEST_MAIN(TestVolume)
