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

#include "TestVoxels.h"

#include "PolyVoxCore/Density.h"
#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/MaterialDensityPair.h"

#include <QtTest>

#include <cfloat> 

using namespace PolyVox;

void TestVoxels::testVoxelTypeLimits()
{	
	// It's worth testing these as they are not all explictily defined (e.g. Density8 is just a 
	// typedef of DensityI8), and in the future we might define then with bitwise magic or something.

	QCOMPARE(VoxelTypeTraits<Density8>::MinDensity, Density8::DensityType(0));
	QCOMPARE(VoxelTypeTraits<Density8>::MaxDensity, Density8::DensityType(255));
	
	QCOMPARE(VoxelTypeTraits<DensityI8>::MinDensity, DensityI8::DensityType(-127));
	QCOMPARE(VoxelTypeTraits<DensityI8>::MaxDensity, DensityI8::DensityType(127));
	
	QCOMPARE(VoxelTypeTraits<DensityU8>::MinDensity, DensityU8::DensityType(0));
	QCOMPARE(VoxelTypeTraits<DensityU8>::MaxDensity, DensityU8::DensityType(255));
	
	QCOMPARE(VoxelTypeTraits<Density16>::MinDensity, Density16::DensityType(0));
	QCOMPARE(VoxelTypeTraits<Density16>::MaxDensity, Density16::DensityType(65535));
	
	QCOMPARE(VoxelTypeTraits<DensityI16>::MinDensity, DensityI16::DensityType(-32767));
	QCOMPARE(VoxelTypeTraits<DensityI16>::MaxDensity, DensityI16::DensityType(32767));
	
	QCOMPARE(VoxelTypeTraits<DensityU16>::MinDensity, DensityU16::DensityType(0));
	QCOMPARE(VoxelTypeTraits<DensityU16>::MaxDensity, DensityU16::DensityType(65535));
	
	QCOMPARE(VoxelTypeTraits<DensityFloat>::MinDensity, FLT_MIN);
	QCOMPARE(VoxelTypeTraits<DensityFloat>::MaxDensity, FLT_MAX);
	
	QCOMPARE(VoxelTypeTraits<DensityDouble>::MinDensity, DBL_MIN);
	QCOMPARE(VoxelTypeTraits<DensityDouble>::MaxDensity, DBL_MAX);
	
	/*fValue = VoxelTypeTraits<DensityFloat>::MinDensity;
	QCOMPARE(fValue, FLT_MIN);
	fValue = VoxelTypeTraits<DensityFloat>::MaxDensity;
	QCOMPARE(fValue, FLT_MAX);*/
	
	/*iValue = VoxelTypeTraits<Material8>::MinDensity;
	QCOMPARE(iValue, 0);
	iValue = VoxelTypeTraits<Material8>::MaxDensity;
	QCOMPARE(iValue, 0);*/
	
	/*iValue = VoxelTypeTraits<MaterialDensityPair44>::MinDensity;
	QCOMPARE(iValue, 0);
	iValue = VoxelTypeTraits<MaterialDensityPair44>::MaxDensity;
	QCOMPARE(iValue, 15);*/
}

QTEST_MAIN(TestVoxels)
