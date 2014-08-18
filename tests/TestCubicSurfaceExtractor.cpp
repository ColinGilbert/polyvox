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

#include "TestCubicSurfaceExtractor.h"

#include "PolyVoxCore/Density.h"
#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/CubicSurfaceExtractor.h"

#include <QtTest>

using namespace PolyVox;

template<typename _VoxelType>
class CustomIsQuadNeeded
{
public:
	typedef _VoxelType VoxelType;

	bool operator()(VoxelType back, VoxelType front, VoxelType& materialToUse)
	{
		// Not a useful test - it just does something different 
		// to the DefaultIsQuadNeeded so we can check it compiles.
		if ((back > 1) && (front <= 1))
		{
			materialToUse = static_cast<VoxelType>(back);
			return true;
		}
		else
		{
			return false;
		}
	}
};


// These 'writeDensityValueToVoxel' functions provide a unified interface for writting densities to primative and class voxel types.
// They are conceptually the inverse of the 'convertToDensity' function used by the MarchingCubesSurfaceExtractor. They probably shouldn't be part
// of PolyVox, but they might be useful to other tests so we cold move them into a 'Tests.h' or something in the future.
template<typename VoxelType>
void writeDensityValueToVoxel(int valueToWrite, VoxelType& voxel)
{
	voxel = valueToWrite;
}

template<>
void writeDensityValueToVoxel(int valueToWrite, Density8& voxel)
{
	voxel.setDensity(valueToWrite);
}

template<>
void writeDensityValueToVoxel(int valueToWrite, MaterialDensityPair88& voxel)
{
	voxel.setDensity(valueToWrite);
}

template<typename VoxelType>
void writeMaterialValueToVoxel(int valueToWrite, VoxelType& voxel)
{
	//Most types don't have a material
	return;
}

template<>
void writeMaterialValueToVoxel(int valueToWrite, MaterialDensityPair88& voxel)
{
	voxel.setMaterial(valueToWrite);
}

// Runs the surface extractor for a given type. 
template <typename VoxelType>
uint32_t testForType(void)
{
	const int32_t uVolumeSideLength = 256;

	//Create empty volume
	SimpleVolume<VoxelType> volData(Region(Vector3DInt32(0,0,0), Vector3DInt32(uVolumeSideLength-1, uVolumeSideLength-1, uVolumeSideLength-1)), 128);

	//Fill the volume with data
	for (int32_t z = 0; z < uVolumeSideLength; z++)
	{
		for (int32_t y = 0; y < uVolumeSideLength; y++)
		{
			for (int32_t x = 0; x < uVolumeSideLength; x++)
			{
				if(x + y + z > uVolumeSideLength)
				{
					VoxelType voxelValue;
					writeDensityValueToVoxel<VoxelType>(100, voxelValue);
					writeMaterialValueToVoxel<VoxelType>(42, voxelValue);
					volData.setVoxelAt(x, y, z, voxelValue);
				}
			}
		}
	}

	uint32_t uTotalVertices = 0;
	uint32_t uTotalIndices = 0;

	//Run the surface extractor a number of times over differnt regions of the volume.
	const int32_t uRegionSideLength = 64;
	for (int32_t z = 0; z < uVolumeSideLength; z += uRegionSideLength)
	{
		for (int32_t y = 0; y < uVolumeSideLength; y += uRegionSideLength)
		{
			for (int32_t x = 0; x < uVolumeSideLength; x += uRegionSideLength)
			{
				Region regionToExtract(x, y, z, x + uRegionSideLength - 1, y + uRegionSideLength - 1, z + uRegionSideLength - 1);

				auto result = extractCubicMesh(&volData, regionToExtract);

				uTotalVertices += result.getNoOfVertices();
				uTotalIndices += result.getNoOfIndices();
			}
		}
	}

	// Just some value which is representative of the work we've done. It doesn't
	// matter what it is, just that it should be the same every time we run the test.
	return uTotalVertices + uTotalIndices;
}

// Runs the surface extractor for a given type. 
template <typename VoxelType>
SimpleVolume<VoxelType>* createAndFillVolumeWithNoise(VoxelType minValue, VoxelType maxValue)
{
	const int32_t uVolumeSideLength = 32;

	//Create empty volume
	SimpleVolume<VoxelType>* volData = new SimpleVolume<VoxelType>(Region(Vector3DInt32(0, 0, 0), Vector3DInt32(uVolumeSideLength - 1, uVolumeSideLength - 1, uVolumeSideLength - 1)), 16);

	srand(12345);

	//Fill the volume with data
	for (int32_t z = 0; z < uVolumeSideLength; z++)
	{
		for (int32_t y = 0; y < uVolumeSideLength; y++)
		{
			for (int32_t x = 0; x < uVolumeSideLength; x++)
			{
				if (minValue == maxValue)
				{
					// In this case we are filling the whole volume with a single value.
					volData->setVoxelAt(x, y, z, minValue);
				}
				else
				{
					// Otherwise we write random voxel values between zero and the requested maximum
					int voxelValue = (rand() % (maxValue - minValue + 1)) + minValue;
					volData->setVoxelAt(x, y, z, static_cast<VoxelType>(voxelValue));
				}
			}
		}
	}

	return volData;
}

void TestCubicSurfaceExtractor::testExecute()
{
	/*const static uint32_t uExpectedVertices = 6624;
	const static uint32_t uExpectedIndices = 9936;
	const static uint32_t uMaterialToCheck = 3000;
	const static float fExpectedMaterial = 42.0f;
	const static uint32_t uIndexToCheck = 2000;
	const static uint32_t uExpectedIndex = 1334;

	Mesh<CubicVertex> mesh;*/

	/*testForType<int8_t>(mesh);
	QCOMPARE(mesh.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(mesh.getNoOfIndices(), uExpectedIndices);
	QCOMPARE(mesh.getVertices()[uMaterialToCheck].getMaterial(), fNoMaterial);

	testForType<uint8_t>(mesh);
	QCOMPARE(mesh.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(mesh.getNoOfIndices(), uExpectedIndices);
	QCOMPARE(mesh.getVertices()[uMaterialToCheck].getMaterial(), fNoMaterial);

	testForType<int16_t>(mesh);
	QCOMPARE(mesh.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(mesh.getNoOfIndices(), uExpectedIndices);
	QCOMPARE(mesh.getVertices()[uMaterialToCheck].getMaterial(), fNoMaterial);

	testForType<uint16_t>(mesh);
	QCOMPARE(mesh.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(mesh.getNoOfIndices(), uExpectedIndices);
	QCOMPARE(mesh.getVertices()[uMaterialToCheck].getMaterial(), fNoMaterial);

	testForType<int32_t>(mesh);
	QCOMPARE(mesh.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(mesh.getNoOfIndices(), uExpectedIndices);
	QCOMPARE(mesh.getVertices()[uMaterialToCheck].getMaterial(), fNoMaterial);

	testForType<uint32_t>(mesh);
	QCOMPARE(mesh.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(mesh.getNoOfIndices(), uExpectedIndices);
	QCOMPARE(mesh.getVertices()[uMaterialToCheck].getMaterial(), fNoMaterial);

	testForType<float>(mesh);
	QCOMPARE(mesh.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(mesh.getNoOfIndices(), uExpectedIndices);
	QCOMPARE(mesh.getVertices()[uMaterialToCheck].getMaterial(), fNoMaterial);

	testForType<double>(mesh);
	QCOMPARE(mesh.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(mesh.getNoOfIndices(), uExpectedIndices);
	QCOMPARE(mesh.getVertices()[uMaterialToCheck].getMaterial(), fNoMaterial);*/

	/*testForType<Material8>(mesh);
	QCOMPARE(mesh.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(mesh.getNoOfIndices(), uExpectedIndices);
	QCOMPARE(mesh.getVertices()[uMaterialToCheck].getMaterial(), fNoMaterial);*/

	const static uint32_t uExpectedSumOfVerticesAndIndices = 704668;
	//const static uint32_t uExpectedSumOfVerticesAndIndices = 2792332;
	uint32_t result = 0;
	QBENCHMARK {
		result = testForType<MaterialDensityPair88>();
	}
	QCOMPARE(result, uExpectedSumOfVerticesAndIndices);

	// Test with default mesh and contoller types.
	auto uint8Vol = createAndFillVolumeWithNoise<uint8_t>(0, 2);
	auto uint8Mesh = extractCubicMesh(uint8Vol, uint8Vol->getEnclosingRegion());
	QCOMPARE(uint8Mesh.getNoOfVertices(), uint32_t(57687));
	QCOMPARE(uint8Mesh.getNoOfIndices(), uint32_t(216234));

	// Test with default mesh type but user-provided controller.
	auto int8Vol = createAndFillVolumeWithNoise<int8_t>(0, 2);
	auto int8Mesh = extractCubicMesh(int8Vol, int8Vol->getEnclosingRegion(), CustomIsQuadNeeded<int8_t>());
	QCOMPARE(int8Mesh.getNoOfVertices(), uint32_t(29027));
	QCOMPARE(int8Mesh.getNoOfIndices(), uint32_t(178356));

	// Test with default controller but user-provided mesh.
	auto uint32Vol = createAndFillVolumeWithNoise<uint32_t>(0, 2);
	Mesh< CubicVertex< uint32_t >, uint16_t > uint32Mesh;
	extractCubicMeshCustom(uint32Vol, uint32Vol->getEnclosingRegion(), &uint32Mesh);
	QCOMPARE(uint32Mesh.getNoOfVertices(), uint16_t(57687));
	QCOMPARE(uint32Mesh.getNoOfIndices(), uint32_t(216234));

	// Test with both mesh and controller being provided by the user.
	auto int32Vol = createAndFillVolumeWithNoise<int32_t>(0, 2);
	Mesh< CubicVertex< int32_t >, uint16_t > int32Mesh;
	extractCubicMeshCustom(int32Vol, int32Vol->getEnclosingRegion(), &int32Mesh, CustomIsQuadNeeded<int32_t>());
	QCOMPARE(int32Mesh.getNoOfVertices(), uint16_t(29027));
	QCOMPARE(int32Mesh.getNoOfIndices(), uint32_t(178356));

}

QTEST_MAIN(TestCubicSurfaceExtractor)
