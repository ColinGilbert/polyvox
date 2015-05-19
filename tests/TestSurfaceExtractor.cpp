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

#include "TestSurfaceExtractor.h"

#include "PolyVox/Density.h"
#include "PolyVox/FilePager.h"
#include "PolyVox/MaterialDensityPair.h"
#include "PolyVox/RawVolume.h"
#include "PolyVox/PagedVolume.h"
#include "PolyVox/MarchingCubesSurfaceExtractor.h"

#include <QtTest>

#include <random>

using namespace PolyVox;

// Test our ability to modify the behaviour of the MarchingCubesSurfaceExtractor. This simple example only modifies
// the threshold (and actually this can be achieved by passing a parameter to the constructor of the
// DefaultMarchingCubesController) but you could implement custom behaviour in the other members
// if you wanted too. Actually, it's not clear if this ability is really useful because I can't think
// what you'd modify apart from the threshold but the ability is at least available. Also, the
// DefaultMarchingCubesController is templatised whereas this exmple shows that controllers don't
// have to be.
class CustomMarchingCubesController
{
public:
	typedef float DensityType;
	typedef float MaterialType;

	float convertToDensity(float voxel)
	{
		return voxel;
	}

	float convertToMaterial(float /*voxel*/)
	{
		return 1.0f;
	}

	float blendMaterials(float /*a*/, float /*b*/, float /*weight*/)
	{
		return 1.0f;
	}

	float getThreshold(void)
	{
		return 50.0f;
	}
};

// These 'writeDensityValueToVoxel' functions provide a unified interface for writting densities to primative and class voxel types.
// They are conceptually the inverse of the 'convertToDensity' function used by the MarchingCubesSurfaceExtractor. They probably shouldn't be part
// of PolyVox, but they might be usful to other tests so we cold move them into a 'Tests.h' or something in the future.
template<typename VoxelType>
void writeDensityValueToVoxel(int valueToWrite, VoxelType& voxel)
{
	voxel = valueToWrite;
}

template<>
void writeDensityValueToVoxel(int valueToWrite, MaterialDensityPair88& voxel)
{
	voxel.setDensity(valueToWrite);
}

template<typename VoxelType>
void writeMaterialValueToVoxel(int /*valueToWrite*/, VoxelType& /*voxel*/)
{
	//Most types don't have a material
	return;
}

template<>
void writeMaterialValueToVoxel(int valueToWrite, MaterialDensityPair88& voxel)
{
	voxel.setMaterial(valueToWrite);
}

template <typename VolumeType>
VolumeType* createAndFillVolume(void)
{
	const int32_t uVolumeSideLength = 64;

	//Create empty volume
	VolumeType* volData = new VolumeType(Region(0, 0, 0, uVolumeSideLength - 1, uVolumeSideLength - 1, uVolumeSideLength - 1));

	// Fill
	for (int32_t z = 0; z < uVolumeSideLength; z++)
	{
		for (int32_t y = 0; y < uVolumeSideLength; y++)
		{
			for (int32_t x = 0; x < uVolumeSideLength; x++)
			{
				// Create a density field which changes throughout the volume. It's
				// zero in the lower corner and increasing as the coordinates increase.
				typename VolumeType::VoxelType voxelValue;
				writeDensityValueToVoxel<typename VolumeType::VoxelType>(x + y + z, voxelValue);
				writeMaterialValueToVoxel<typename VolumeType::VoxelType>(z > uVolumeSideLength / 2 ? 42 : 79, voxelValue);
				volData->setVoxel(x, y, z, voxelValue);
			}
		}
	}

	return volData;
}

template <typename VolumeType>
VolumeType* createAndFillVolumeWithNoise(int32_t iVolumeWidthAndHeight, int32_t iVolumeDepth, float minValue, float maxValue)
{
	FilePager<float>* pager = new FilePager<float>(".");

	//Create empty volume
	VolumeType* volData = new VolumeType(pager);

	// Set up a random number generator
	std::mt19937 rng;

	// Fill
	for (int32_t z = 0; z < iVolumeDepth; z++)
	{
		for (int32_t y = 0; y < iVolumeWidthAndHeight; y++)
		{
			for (int32_t x = 0; x < iVolumeWidthAndHeight; x++)
			{
				// We can't use std distributions because they vary between platforms (breaking tests)
				float voxelValue = static_cast<float>(rng()) / static_cast<float>(std::numeric_limits<int32_t>::max()); // Float in range 0.0 to 1.0
				voxelValue = voxelValue	* (maxValue - minValue) + minValue; // Float in range minValue to maxValue

				volData->setVoxel(x, y, z, voxelValue);
			}
		}
	}

	return volData;
}

void TestSurfaceExtractor::testBehaviour()
{
	// These tests apply the Marching Cubes surface extractor to volumes of various voxel types. In addition we sometimes make use of custom controllers
	// and user-provided meshes to make sure these various combinations work as expected.
	//
	// It is also noted that the number of indices and vertices is varying quite significantly based on the voxel type. This seems unexpected, but could
	// be explained if some overflow is occuring when writing data into the volume, causing volumes of different voxel types to have different distributions.
	// Of course, the use of a custom controller will also make a significant diference, but this probably does need investigating further in the future.

	// This basic test just uses the default controller and automatically generates a mesh of the appropriate type.
	auto uintVol = createAndFillVolume< RawVolume<uint8_t> >();
	auto uintMesh = extractMarchingCubesMesh(uintVol, uintVol->getEnclosingRegion());
	QCOMPARE(uintMesh.getNoOfVertices(), uint32_t(6048)); // Verifies size of mesh and that we have 32-bit indices
	QCOMPARE(uintMesh.getNoOfIndices(), uint32_t(35157)); // Verifies size of mesh
	QCOMPARE(uintMesh.getIndex(100), uint32_t(24)); // Verifies that we have 32-bit indices
	QCOMPARE(uintMesh.getVertex(100).data, uint8_t(1)); // Not really meaningful for a primative type

	// This test makes use of a custom controller
	auto floatVol = createAndFillVolume< RawVolume<float> >();
	CustomMarchingCubesController floatCustomController;
	auto floatMesh = extractMarchingCubesMesh(floatVol, floatVol->getEnclosingRegion(), floatCustomController);
	QCOMPARE(floatMesh.getNoOfVertices(), uint32_t(3825)); // Verifies size of mesh and that we have 32-bit indices
	QCOMPARE(floatMesh.getNoOfIndices(), uint32_t(22053)); // Verifies size of mesh
	QCOMPARE(floatMesh.getIndex(100), uint32_t(119)); // Verifies that we have 32-bit indices
	QCOMPARE(floatMesh.getVertex(100).data, float(1.0f)); // Not really meaningful for a primative type

	// This test makes use of a user provided mesh. It uses the default controller, but we have to explicitly provide this because C++ won't let us
	// use a default for the second-to-last parameter but noot use a default for the last parameter.
	auto intVol = createAndFillVolume< RawVolume<int8_t> >();
	Mesh< MarchingCubesVertex< int8_t >, uint16_t > intMesh;
	extractMarchingCubesMeshCustom(intVol, intVol->getEnclosingRegion(), &intMesh);
	QCOMPARE(intMesh.getNoOfVertices(), uint16_t(5859)); // Verifies size of mesh and that we have 16-bit indices
	QCOMPARE(intMesh.getNoOfIndices(), uint32_t(34041)); // Verifies size of mesh
	QCOMPARE(intMesh.getIndex(100), uint16_t(29)); // Verifies that we have 16-bit indices
	QCOMPARE(intMesh.getVertex(100).data, int8_t(1)); // Not really meaningful for a primative type

	// This test makes use of a user-provided mesh and also a custom controller.
	auto doubleVol = createAndFillVolume< RawVolume<double> >();
	CustomMarchingCubesController doubleCustomController;
	Mesh< MarchingCubesVertex< double >, uint16_t > doubleMesh;
	extractMarchingCubesMeshCustom(doubleVol, doubleVol->getEnclosingRegion(), &doubleMesh, doubleCustomController);
	QCOMPARE(doubleMesh.getNoOfVertices(), uint16_t(3825)); // Verifies size of mesh and that we have 32-bit indices
	QCOMPARE(doubleMesh.getNoOfIndices(), uint32_t(22053)); // Verifies size of mesh
	QCOMPARE(doubleMesh.getIndex(100), uint16_t(119)); // Verifies that we have 32-bit indices
	QCOMPARE(doubleMesh.getVertex(100).data, double(1.0f)); // Not really meaningful for a primative type

	// This test ensures the extractor works on a non-primitive voxel type.
	auto materialVol = createAndFillVolume< RawVolume<MaterialDensityPair88> >();
	auto materialMesh = extractMarchingCubesMesh(materialVol, materialVol->getEnclosingRegion());
	QCOMPARE(materialMesh.getNoOfVertices(), uint32_t(6048)); // Verifies size of mesh and that we have 32-bit indices
	QCOMPARE(materialMesh.getNoOfIndices(), uint32_t(35157)); // Verifies size of mesh
	QCOMPARE(materialMesh.getIndex(100), uint32_t(24)); // Verifies that we have 32-bit indices
	QCOMPARE(materialMesh.getVertex(100).data.getMaterial(), uint16_t(79)); // Verify the data attached to the vertex
}

void TestSurfaceExtractor::testEmptyVolumePerformance()
{
	auto emptyVol = createAndFillVolumeWithNoise< PagedVolume<float> >(128, 512, -2.0f, -1.0f);
	Mesh< MarchingCubesVertex< float >, uint16_t > emptyMesh;
	QBENCHMARK{ extractMarchingCubesMeshCustom(emptyVol, Region(8, 8, 8, 119, 119, 503), &emptyMesh); }
	QCOMPARE(emptyMesh.getNoOfVertices(), uint16_t(0));
}

void TestSurfaceExtractor::testNoiseVolumePerformance()
{
	auto noiseVol = createAndFillVolumeWithNoise< PagedVolume<float> >(128, 128, -1.0f, 1.0f);
	Mesh< MarchingCubesVertex< float >, uint16_t > noiseMesh;
	QBENCHMARK{ extractMarchingCubesMeshCustom(noiseVol, Region(32, 32, 32, 63, 63, 63), &noiseMesh); }
	QCOMPARE(noiseMesh.getNoOfVertices(), uint16_t(35672));
}

QTEST_MAIN(TestSurfaceExtractor)
