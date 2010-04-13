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

#include "PolyVoxImpl/SubArray.h"
#include "PolyVoxImpl/CPlusPlusZeroXSupport.h"
#pragma endregion

namespace PolyVox
{
	template <uint32_t noOfDims, typename ElementType>
	class Array
	{
	public:
		Array<noOfDims, ElementType>();

		Array<noOfDims, ElementType>(const uint32_t (&pDimensions)[noOfDims]);

		~Array<noOfDims, ElementType>();

		SubArray<noOfDims-1, ElementType> operator[](uint32_t uIndex);

		const SubArray<noOfDims-1, ElementType> operator[](uint32_t uIndex) const;

		uint32_t getNoOfElements(void) const;

		ElementType* getRawData(void) const;

		void resize(const uint32_t (&pDimensions)[noOfDims]);

		void swap(Array<noOfDims, ElementType>& rhs);

	private:
		Array<noOfDims, ElementType>(const Array<noOfDims, ElementType>& rhs);

		Array<noOfDims, ElementType>& operator=(const Array<noOfDims, ElementType>& rhs);

		void deallocate(void);

		uint32_t * m_pDimensions;
		uint32_t * m_pOffsets;
		uint32_t m_uNoOfElements;
		ElementType * m_pElements;
	};

	template <typename ElementType>
	class Array<1, ElementType>
	{
	public:
		Array<1, ElementType>();

		Array<1, ElementType>(const uint32_t (&pDimensions)[1]);

		~Array<1, ElementType>();

		ElementType& operator[] (uint32_t uIndex);

		const ElementType& operator[] (uint32_t uIndex) const;

		uint32_t getNoOfElements(void) const;

		ElementType* getRawData(void) const;

		void resize(const uint32_t (&pDimensions)[1]);

		void swap(Array<1, ElementType>& rhs);
		
	private:
		Array<1, ElementType>(const Array<1, ElementType>& rhs);

		Array<1, ElementType>& operator=(const Array<1, ElementType>& rhs);

		void deallocate(void);

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
