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

#include "testvector.h"

#include "PolyVox/Vector.h"

#include <QtTest>

using namespace PolyVox;

Vector3DFloat incrementVector(Vector3DFloat input)
{
	Vector3DFloat result = input += 1.0f;
	return result;
}

void TestVector::testLength()
{
	Vector3DInt8 vec(3, 4, 5);
	QCOMPARE(vec.lengthSquared(), int32_t(3*3+4*4+5*5)); // QCOMPARE is strict on types. For an int8 vector, the OperationType is int32_t.
}

void TestVector::testDotProduct()
{
	Vector3DInt8 vecxy(3, 4, 0);
	Vector3DInt8 vecz(0, 0, 1);
	
	QCOMPARE(vecxy.dot(vecz), int32_t(0)); // QCOMPARE is strict on types. For an int8 vector, the OperationType is int32_t .
}

void TestVector::testEquality()
{
	Vector3DInt8 vecxy(3, 4, 0);
	Vector3DInt8 vecz(0, 0, 1);
	
	QCOMPARE(vecxy != vecz, true);
}

void TestVector::testPerformance()
{
	Vector3DFloat vec(1.0, 1.0, 1.0);

	QBENCHMARK
	{
		for(uint32_t ct = 0; ct < 10000000; ct++)
		{
			vec = incrementVector(vec);
		}
	}

	// Use the result so the calls don't get optimized away.
	QCOMPARE(vec.lengthSquared() > 0.0f, true);
}

QTEST_MAIN(TestVector)
