/*******************************************************************************
Copyright (c) 2005-20014 David Williams

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

#ifndef __PolyVox_Array2D_H__
#define __PolyVox_Array2D_H__

namespace PolyVox
{
	template <uint32_t noOfDims, typename ElementType>
	class Array2D
	{
	public:

		Array2D(uint32_t width, uint32_t height)
			:m_pData(0)
		{
			m_uDimensions[0] = width;
			m_uDimensions[1] = height;

			initialize();
		}

		// These are deleted to avoid accidental copying.
		Array2D<noOfDims, ElementType>(const Array2D<noOfDims, ElementType>&) = delete;
		Array2D<noOfDims, ElementType>& operator=(const Array2D<noOfDims, ElementType>&) = delete;

		~Array2D()
		{
			delete[] m_pData;
		}

		ElementType operator()(uint32_t x, uint32_t y) const
		{
			POLYVOX_ASSERT((x < m_uDimensions[0] && y < m_uDimensions[1]), "Array access is out-of-range.");
			return m_pData[y * m_uDimensions[0] + x];
		}

		ElementType& operator()(uint32_t x, uint32_t y)
		{
			POLYVOX_ASSERT((x < m_uDimensions[0] && y < m_uDimensions[1]), "Array access is out-of-range.");
			return m_pData[y * m_uDimensions[0] + x];
		}

		ElementType* getRawData()
		{
			return m_pData;
		}

		size_t getNoOfElements()
		{
			return m_uNoOfElements;
		}

		void swap(Array2D& other)
		{
			ElementType* temp = other.m_pData;
			other.m_pData = m_pData;
			m_pData = temp;
		}

	private:

		void initialize(void)
		{
			// Calculate the total number of elements in the array.
			m_uNoOfElements = 1;
			for (uint32_t i = 0; i < noOfDims; i++)
			{
				m_uNoOfElements *= m_uDimensions[i];
			}
			m_pData = new ElementType[m_uNoOfElements];
		}

		uint32_t m_uDimensions[noOfDims];
		uint32_t m_uNoOfElements;
		ElementType* m_pData;
	};

	///A 2D Array of floats.
	typedef Array2D<2, float> Array2DFloat;
	///A 2D Array of doubles.
	typedef Array2D<2, double> Array2DDouble;
	///A 2D Array of signed 8-bit values.
	typedef Array2D<2, int8_t> Array2DInt8;
	///A 2D Array of unsigned 8-bit values.
	typedef Array2D<2, uint8_t> Array2DUint8;
	///A 2D Array of signed 16-bit values.
	typedef Array2D<2, int16_t> Array2DInt16;
	///A 2D Array of unsigned 16-bit values.
	typedef Array2D<2, uint16_t> Array2DUint16;
	///A 2D Array of signed 32-bit values.
	typedef Array2D<2, int32_t> Array2DInt32;
	///A 2D Array of unsigned 32-bit values.
	typedef Array2D<2, uint32_t> Array2DUint32;
}

#endif //__PolyVox_Array2D_H__