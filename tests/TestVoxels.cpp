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

#include "PolyVoxCore/Vector.h"

#include <QtTest>

using namespace PolyVox;

void TestVoxels::testTraits()
{
	int iValue; float fValue; //Used for temporary storage as the QCOMPARE maro struggles with the complex types.
	
	
	iValue = VoxelTypeTraits<Density8>::MinDensity;
	QCOMPARE(iValue, 0);
	iValue = VoxelTypeTraits<Density8>::MaxDensity;
	QCOMPARE(iValue, 255);
	
	fValue = VoxelTypeTraits<DensityFloat>::MinDensity;
	QCOMPARE(fValue, -1000000.0f);
	fValue = VoxelTypeTraits<DensityFloat>::MaxDensity;
	QCOMPARE(fValue, 1000000.0f);
	
	iValue = VoxelTypeTraits<Material8>::MinDensity;
	QCOMPARE(iValue, 0);
	iValue = VoxelTypeTraits<Material8>::MaxDensity;
	QCOMPARE(iValue, 0);
	
	iValue = VoxelTypeTraits<MaterialDensityPair44>::MinDensity;
	QCOMPARE(iValue, 0);
	iValue = VoxelTypeTraits<MaterialDensityPair44>::MaxDensity;
	QCOMPARE(iValue, 15);
}

QTEST_MAIN(TestVoxels)
