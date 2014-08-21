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

#include "PolyVoxCore/Array.h"

#include <QtTest>

using namespace PolyVox;

template <typename ElementType>
class Array2D
{
public:

	Array2D(uint32_t width, uint32_t height)
		:m_uWidth(width)
		,m_uHeight(height)
		,m_pData(0)
	{
		m_pData = new ElementType[m_uWidth * m_uHeight];
	}

	~Array2D()
	{
		delete[] m_pData;
	}

	ElementType& operator()(uint32_t x, uint32_t y)
	{
		return m_pData[y * m_uWidth + x];
	}

private:

	uint32_t m_uWidth;
	uint32_t m_uHeight;
	ElementType* m_pData;
};

void TestArray::testCArraySpeed()
{
	const int width = 128;
	const int height = 128;

	int cArray[width][height];

	QBENCHMARK
	{
		int ct = 1;
		int expectedTotal = 0;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				cArray[x][y] = ct;
				expectedTotal += cArray[x][y];
				ct++;
			}
		}
	}
}

void TestArray::testPolyVoxArraySpeed()
{
	const int width = 128;
	const int height = 128;

	Array<2, int> polyvoxArray(ArraySizes(width)(height));

	QBENCHMARK
	{
		int ct = 1;
		int expectedTotal = 0;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				polyvoxArray[x][y] = ct;
				expectedTotal += polyvoxArray[x][y];
				ct++;
			}
		}
	}
}

void TestArray::testPolyVoxArray2DSpeed()
{
	const int width = 128;
	const int height = 128;

	Array2D<int> polyvoxArray(width,height);

	QBENCHMARK
	{
		int ct = 1;
		int expectedTotal = 0;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				polyvoxArray(x,y) = ct;
				expectedTotal += polyvoxArray(x,y);
				ct++;
			}
		}
	}
}

void TestArray::testReadWrite()
{
	int width = 5;
	int height = 10;
	int depth = 20;

	Array<3, int> myArray(ArraySizes(width)(height)(depth));

	int ct = 1;
	int expectedTotal = 0;
	for(int z = 0; z < depth; z++)
	{
		for(int y = 0; y < height; y++)
		{
			for(int x = 0; x < width; x++)
			{
				myArray[x][y][z] = ct;
				expectedTotal += myArray[x][y][z];
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
				QCOMPARE(myArray[x][y][z], ct);
				total += myArray[x][y][z];
				ct++;
			}
		}
	}

	QCOMPARE(total, expectedTotal);
}

QTEST_MAIN(TestArray)
