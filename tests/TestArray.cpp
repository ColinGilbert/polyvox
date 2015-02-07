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

#include "TestArray.h"

#include "PolyVox/Array.h"

#include <QtTest>

using namespace PolyVox;

void TestArray::testCArraySpeed()
{
	const int width = 64;
	const int height = 32;
	const int depth = 16;

	int cArray[width][height][depth];

	QBENCHMARK
	{
		int ct = 1;
		int expectedTotal = 0;
		for (int z = 0; z < depth; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					cArray[x][y][z] = ct;
					expectedTotal += cArray[x][y][z];
					ct++;
				}
			}
		}
	}
}

void TestArray::testPolyVoxArraySpeed()
{
	const int width = 64;
	const int height = 32;
	const int depth = 16;

	Array<3, int> polyvoxArray(width, height, depth);

	QBENCHMARK
	{
		int ct = 1;
		int expectedTotal = 0;
		for (int z = 0; z < depth; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					polyvoxArray(x, y, z) = ct;
					expectedTotal += polyvoxArray(x, y, z);
					ct++;
				}
			}
		}
	}
}

void TestArray::testReadWrite()
{
	int width = 5;
	int height = 10;
	int depth = 20;

	Array<3, int> myArray(width, height, depth);

	int ct = 1;
	int expectedTotal = 0;
	for(int z = 0; z < depth; z++)
	{
		for(int y = 0; y < height; y++)
		{
			for(int x = 0; x < width; x++)
			{
				myArray(x, y, z) = ct;
				expectedTotal += myArray(x, y, z);
				ct++;
			}
		}
	}

	ct = 1;
	int total = 0;
	for(int z = 0; z < depth; z++)
	{
		for(int y = 0; y < height; y++)
		{
			for(int x = 0; x < width; x++)
			{
				QCOMPARE(myArray(x, y, z), ct);
				total += myArray(x, y, z);
				ct++;
			}
		}
	}

	QCOMPARE(total, expectedTotal);
}

QTEST_MAIN(TestArray)
