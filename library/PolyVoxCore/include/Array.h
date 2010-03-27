#pragma region License
/*******************************************************************************
Copyright (c) 2005-2009 David Williams

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
#pragma endregion

#ifndef __PolyVox_Array_H__
#define __PolyVox_Array_H__

#pragma region Headers
#include "ArraySizes.h" //Not strictly required, but convienient

#include "PolyVoxImpl/TypeDef.h"
#include "PolyVoxImpl/CPlusPlusZeroXSupport.h"
#pragma endregion

namespace PolyVox
{
	template <uint32_t noOfDims, typename ElementType>
	class Array
	{
		friend class Array<noOfDims+1, ElementType>;

	public:
		Array<noOfDims, ElementType>(const uint32_t (&Dimensions)[noOfDims])
			: m_pElements(0)
			,m_pDimensions(0)
			,m_pOffsets(0)
			,m_uNoOfElements(0)
		{
			m_pDimensions = new uint32_t[noOfDims];
			m_pOffsets = new uint32_t[noOfDims];
			
			// Calculate all the information you need to use the array
			m_uNoOfElements = 1;
			for (uint32_t i = 0; i<noOfDims; i++)
			{
				assert(Dimensions[i] != 0);

				m_uNoOfElements *= Dimensions[i]; 
				m_pDimensions[i] = Dimensions[i];
				m_pOffsets[i] = 1; 
				for (int k=noOfDims-1; k>i; k--)
				{
					m_pOffsets[i] *= Dimensions[k];
				}
			} 
			// Allocate new elements, let exception propagate 
			m_pElements = new ElementType[m_uNoOfElements];
		} 

		Array<noOfDims-1, ElementType> operator [](uint32_t uIndex)
		{
			assert(uIndex<m_pDimensions[0]);
			return
				Array<noOfDims-1, ElementType>(&m_pElements[uIndex*m_pOffsets[0]],
				m_pDimensions+1, m_pOffsets+1);
		}
		const Array<noOfDims-1, ElementType>
			operator [](uint32_t uIndex) const
		{
			assert(uIndex<m_pDimensions[0]);
			return
				Array<noOfDims-1, ElementType>(&m_pElements[uIndex*m_pOffsets[0]],
				m_pDimensions+1, m_pOffsets+1);
		}

		ElementType* getRawData(void)
		{
			return m_pElements;
		}

		uint32_t getNoOfElements(void)
		{
			return m_uNoOfElements;
		}

	private:
		Array<noOfDims, ElementType>(ElementType * pElements, uint32_t * pDimensions, uint32_t * pOffsets)
			:m_pElements(pElements)
			,m_pDimensions(pDimensions)
			,m_pOffsets(pOffsets)
			,m_uNoOfElements(0)
		{
		} 

		uint32_t * m_pDimensions;
		uint32_t * m_pOffsets;
		uint32_t m_uNoOfElements;
		ElementType * m_pElements;
	};

	template <typename ElementType>
	class Array<1, ElementType>
	{
		friend class Array<2, ElementType>;

	public:
		Array<1, ElementType>(const uint32_t (&Dimensions)[1])
			: m_pElements(0)
			,m_pDimensions(0)
		{
			m_pDimensions = new uint32_t[1];			
			m_pDimensions[0] = Dimensions[0];

			// Allocate new elements, let exception propagate 
			m_pElements = new ElementType[m_pDimensions[0]];
		} 

		ElementType & operator [] (uint32_t uIndex)
		{
			assert(uIndex<m_pDimensions[0]);
			return m_pElements[uIndex];
		}

		const ElementType & operator [] (uint32_t uIndex) const
		{
			assert(uIndex<m_pDimensions[0]);
			return m_pElements[uIndex];
		}

		ElementType* getRawData(void)
		{
			return m_pElements;
		}

		uint32_t getNoOfElements(void)
		{
			return m_pDimensions[0];
		}
		
	private:
		Array<1, ElementType>(ElementType * pElements, uint32_t * pDimensions, uint32_t * /*pOffsets*/)
			:m_pDimensions(pDimensions)
			,m_pElements(pElements)			
		{
		}

		uint32_t * m_pDimensions;
		ElementType * m_pElements;
	};

	template <typename ElementType>
	class Array<0, ElementType>
	{
		//Zero dimensional array is meaningless.
	};

	//Some handy typedefs
	///A 1D Array of floats.
	typedef Array<1,float> Array1DFloat;
	///A 1D Array of doubles.
	typedef Array<1,double> Array1DDouble;
	///A 1D Array of signed 8-bit values.
	typedef Array<1,int8_t> Array1DInt8;
	///A 1D Array of unsigned 8-bit values.
	typedef Array<1,uint8_t> Array1DUint8;
	///A 1D Array of signed 16-bit values.
	typedef Array<1,int16_t> Array1DInt16;
	///A 1D Array of unsigned 16-bit values.
	typedef Array<1,uint16_t> Array1DUint16;
	///A 1D Array of signed 32-bit values.
	typedef Array<1,int32_t> Array1DInt32;
	///A 1D Array of unsigned 32-bit values.
	typedef Array<1,uint32_t> Array1DUint32;

	///A 2D Array of floats.
	typedef Array<2,float> Array2DFloat;
	///A 2D Array of doubles.
    typedef Array<2,double> Array2DDouble;
	///A 2D Array of signed 8-bit values.
	typedef Array<2,int8_t> Array2DInt8;
	///A 2D Array of unsigned 8-bit values.
	typedef Array<2,uint8_t> Array2DUint8;
	///A 2D Array of signed 16-bit values.
	typedef Array<2,int16_t> Array2DInt16;
	///A 2D Array of unsigned 16-bit values.
	typedef Array<2,uint16_t> Array2DUint16;
	///A 2D Array of signed 32-bit values.
	typedef Array<2,int32_t> Array2DInt32;
	///A 2D Array of unsigned 32-bit values.
	typedef Array<2,uint32_t> Array2DUint32;

	///A 3D Array of floats.
	typedef Array<3,float> Array3DFloat;
	///A 3D Array of doubles.
    typedef Array<3,double> Array3DDouble;
	///A 3D Array of signed 8-bit values.
	typedef Array<3,int8_t> Array3DInt8;
	///A 3D Array of unsigned 8-bit values.
	typedef Array<3,uint8_t> Array3DUint8;
	///A 3D Array of signed 16-bit values.
	typedef Array<3,int16_t> Array3DInt16;
	///A 3D Array of unsigned 16-bit values.
	typedef Array<3,uint16_t> Array3DUint16;
	///A 3D Array of signed 32-bit values.
	typedef Array<3,int32_t> Array3DInt32;
	///A 3D Array of unsigned 32-bit values.
	typedef Array<3,uint32_t> Array3DUint32;
}//namespace PolyVox

#include "Array.inl"

#endif