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

#include <QtGlobal>
#include <QtTest>

using namespace PolyVox;

// This is used to compute a value from a list of integers. We use it to 
// make sure we get the expected result from a series of volume accesses.
inline int32_t cantorTupleFunction(int32_t previousResult, int32_t value)
{
	return (( previousResult + value  ) * ( previousResult + value + 1 ) + value ) / 2;
}

template <typename VolumeType>
VolumeType* createAndFillVolume(void)
{
	//Create the volume
	VolumeType* volume = new VolumeType(Region(-57, -31, 12, 64, 96, 131)); // Deliberatly awkward size

	//Fill the volume with some data
	for(int z = volume->getEnclosingRegion().getLowerZ(); z <= volume->getEnclosingRegion().getUpperZ(); z++)
	{
		for(int y = volume->getEnclosingRegion().getLowerY(); y <= volume->getEnclosingRegion().getUpperY(); y++)
		{
			for(int x = volume->getEnclosingRegion().getLowerX(); x <= volume->getEnclosingRegion().getUpperX(); x++)
			{
				volume->setVoxelAt(x, y, z, x + y + z);
			}
		}
	}

	return volume;
}

template <typename VolumeType>
int32_t testDirectAccessWithWrapping(const VolumeType* volume)
{
	int32_t result = 0;

	for(int z = volume->getEnclosingRegion().getLowerZ() - 2; z <= volume->getEnclosingRegion().getUpperZ() + 4; z++)
	{
		for(int y = volume->getEnclosingRegion().getLowerY() - 3; y <= volume->getEnclosingRegion().getUpperY() + 5; y++)
		{
			for(int x = volume->getEnclosingRegion().getLowerX() - 1; x <= volume->getEnclosingRegion().getUpperX() + 2; x++)
			{
				//Three level loop now processes 27 voxel neighbourhood
				for(int innerZ = -1; innerZ <=1; innerZ++)
				{
					for(int innerY = -1; innerY <=1; innerY++)
					{
						for(int innerX = -1; innerX <=1; innerX++)
						{
							result = cantorTupleFunction(result, volume->getVoxelWithWrapping(x + innerX, y + innerY, z + innerZ, WrapModes::Border, 3));
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
int32_t testSamplersWithWrapping(VolumeType* volume)
{
	int32_t result = 0;

	typename VolumeType::Sampler sampler(volume);
	sampler.setWrapMode(WrapModes::Border, 3);

	for(int z = volume->getEnclosingRegion().getLowerZ() - 2; z <= volume->getEnclosingRegion().getUpperZ() + 4; z++)
	{
		for(int y = volume->getEnclosingRegion().getLowerY() - 3; y <= volume->getEnclosingRegion().getUpperY() + 5; y++)
		{
			for(int x = volume->getEnclosingRegion().getLowerX() - 1; x <= volume->getEnclosingRegion().getUpperX() + 2; x++)
			{
				sampler.setPosition(x, y, z);

				result = cantorTupleFunction(result, sampler.peekVoxel1nx1ny1nz());
				result = cantorTupleFunction(result, sampler.peekVoxel0px1ny1nz());
				result = cantorTupleFunction(result, sampler.peekVoxel1px1ny1nz());
				result = cantorTupleFunction(result, sampler.peekVoxel1nx0py1nz());
				result = cantorTupleFunction(result, sampler.peekVoxel0px0py1nz());
				result = cantorTupleFunction(result, sampler.peekVoxel1px0py1nz());
				result = cantorTupleFunction(result, sampler.peekVoxel1nx1py1nz());
				result = cantorTupleFunction(result, sampler.peekVoxel0px1py1nz());
				result = cantorTupleFunction(result, sampler.peekVoxel1px1py1nz());

				result = cantorTupleFunction(result, sampler.peekVoxel1nx1ny0pz());
				result = cantorTupleFunction(result, sampler.peekVoxel0px1ny0pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1px1ny0pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1nx0py0pz());
				result = cantorTupleFunction(result, sampler.peekVoxel0px0py0pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1px0py0pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1nx1py0pz());
				result = cantorTupleFunction(result, sampler.peekVoxel0px1py0pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1px1py0pz());

				result = cantorTupleFunction(result, sampler.peekVoxel1nx1ny1pz());
				result = cantorTupleFunction(result, sampler.peekVoxel0px1ny1pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1px1ny1pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1nx0py1pz());
				result = cantorTupleFunction(result, sampler.peekVoxel0px0py1pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1px0py1pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1nx1py1pz());
				result = cantorTupleFunction(result, sampler.peekVoxel0px1py1pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1px1py1pz());
			}
		}
	}

	return result;
}

template <typename VolumeType>
int32_t complexVolumeTest(void)
{
	VolumeType* testVolume = createAndFillVolume<VolumeType>();

	int32_t result = 0;

	//Test the getVoxel function
	for(int z = testVolume->getEnclosingRegion().getLowerZ(); z <= testVolume->getEnclosingRegion().getUpperZ(); z++)
	{
		for(int y = testVolume->getEnclosingRegion().getLowerY(); y <= testVolume->getEnclosingRegion().getUpperY(); y++)
		{
			for(int x = testVolume->getEnclosingRegion().getLowerX(); x <= testVolume->getEnclosingRegion().getUpperX(); x++)
			{
				result = cantorTupleFunction(result, testVolume->getVoxel(x, y, z));
			}
		}
	}

	//Test border wrap mode
	for(int z = testVolume->getEnclosingRegion().getLowerZ(); z <= testVolume->getEnclosingRegion().getUpperZ(); z++)
	{
		//Extending outside in y
		for(int y = testVolume->getEnclosingRegion().getLowerY() - 3; y <= testVolume->getEnclosingRegion().getUpperY() + 5; y++)
		{
			for(int x = testVolume->getEnclosingRegion().getLowerX(); x <= testVolume->getEnclosingRegion().getUpperX(); x++)
			{
				result = cantorTupleFunction(result, testVolume->getVoxelWithWrapping(x, y, z, WrapModes::Border, 3));
			}
		}
	}

	//Test clamp wrap mode
	for(int z = testVolume->getEnclosingRegion().getLowerZ(); z <= testVolume->getEnclosingRegion().getUpperZ(); z++)
	{
		for(int y = testVolume->getEnclosingRegion().getLowerY(); y <= testVolume->getEnclosingRegion().getUpperY(); y++)
		{
			//Extending outside in x
			for(int x = testVolume->getEnclosingRegion().getLowerX() - 2; x <= testVolume->getEnclosingRegion().getUpperX() + 4; x++)
			{
				result = cantorTupleFunction(result, testVolume->getVoxelWithWrapping(x, y, z, WrapModes::Clamp));
			}
		}
	}

	//Test the sampler setPosition
	typename VolumeType::Sampler sampler(testVolume);
	sampler.setWrapMode(WrapModes::Border, 1);

	for(int z = testVolume->getEnclosingRegion().getLowerZ() - 2; z <= testVolume->getEnclosingRegion().getUpperZ() + 1; z++)
	{
		for(int y = testVolume->getEnclosingRegion().getLowerY() - 1; y <= testVolume->getEnclosingRegion().getUpperY() + 3; y++)
		{
			for(int x = testVolume->getEnclosingRegion().getLowerX() - 4; x <= testVolume->getEnclosingRegion().getUpperX() + 2; x++)
			{
				sampler.setPosition(x,y,z);
				result = cantorTupleFunction(result, sampler.getVoxel());
			}
		}
	}

	//Test the sampler move functions
	typename VolumeType::Sampler xSampler(testVolume);
	typename VolumeType::Sampler ySampler(testVolume);
	typename VolumeType::Sampler zSampler(testVolume);

	xSampler.setWrapMode(WrapModes::Border, 1);
	ySampler.setWrapMode(WrapModes::Clamp, 1);
	zSampler.setWrapMode(WrapModes::Border, -3);

	zSampler.setPosition(testVolume->getEnclosingRegion().getLowerX() - 4, testVolume->getEnclosingRegion().getLowerY() - 1, testVolume->getEnclosingRegion().getLowerZ() - 2);
	for(int z = testVolume->getEnclosingRegion().getLowerZ() - 2; z <= testVolume->getEnclosingRegion().getUpperZ() + 1; z++)
	{
		ySampler = zSampler;
		for(int y = testVolume->getEnclosingRegion().getLowerY() - 1; y <= testVolume->getEnclosingRegion().getUpperY() + 3; y++)
		{
			xSampler = ySampler;
			for(int x = testVolume->getEnclosingRegion().getLowerX() - 4; x <= testVolume->getEnclosingRegion().getUpperX() + 2; x++)
			{
				result = cantorTupleFunction(result, xSampler.getVoxel());
				xSampler.movePositiveX();
			}
			ySampler.movePositiveY();
		}
		zSampler.movePositiveZ();
	}

	xSampler.setWrapMode(WrapModes::Clamp);
	ySampler.setWrapMode(WrapModes::Border, 1);
	zSampler.setWrapMode(WrapModes::Clamp, -1);

	zSampler.setPosition(testVolume->getEnclosingRegion().getUpperX() + 2, testVolume->getEnclosingRegion().getUpperY() + 3, testVolume->getEnclosingRegion().getUpperZ() + 1);
	for(int z = 0; z < testVolume->getEnclosingRegion().getDepthInVoxels() + 8; z++)
	{
		ySampler = zSampler;
		for(int y = 0; y < testVolume->getEnclosingRegion().getHeightInVoxels() + 3; y++)
		{
			xSampler = ySampler;
			for(int x = 0; x < testVolume->getEnclosingRegion().getWidthInVoxels() + 5; x++)
			{
				result = cantorTupleFunction(result, xSampler.getVoxel());
				xSampler.moveNegativeX();
			}
			ySampler.moveNegativeY();
		}
		zSampler.moveNegativeZ();
	}

	delete testVolume;

	return result;
}

TestVolume::TestVolume()
{
	Region region(-57, -31, 12, 64, 96, 131); // Deliberatly awkward size

	//Create the volumes
	m_pRawVolume = new RawVolume<int32_t>(region);
	m_pSimpleVolume = new SimpleVolume<int32_t>(region);
	m_pLargeVolume = new LargeVolume<int32_t>(region);

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
}

void TestVolume::testRawVolumeDirectAccess()
{
	int32_t result = 0;

	QBENCHMARK
	{
		result = testDirectAccessWithWrapping(m_pRawVolume);
	}
	QCOMPARE(result, static_cast<int32_t>(-928601007));
}

void TestVolume::testRawVolumeSamplers()
{
	int32_t result = 0;

	QBENCHMARK
	{
		result = testSamplersWithWrapping(m_pRawVolume);
	}
	QCOMPARE(result, static_cast<int32_t>(-928601007));
}

void TestVolume::testSimpleVolumeDirectAccess()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testDirectAccessWithWrapping(m_pSimpleVolume);
	}
	QCOMPARE(result, static_cast<int32_t>(-928601007));
}

void TestVolume::testSimpleVolumeSamplers()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testSamplersWithWrapping(m_pSimpleVolume);
	}
	QCOMPARE(result, static_cast<int32_t>(-928601007));
}

void TestVolume::testLargeVolumeDirectAccess()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testDirectAccessWithWrapping(m_pLargeVolume);
	}
	QCOMPARE(result, static_cast<int32_t>(-928601007));
}

void TestVolume::testLargeVolumeSamplers()
{
	int32_t result = 0;
	
	QBENCHMARK
	{
		result = testSamplersWithWrapping(m_pLargeVolume);
	}
	QCOMPARE(result, static_cast<int32_t>(-928601007));
}

QTEST_MAIN(TestVolume)
