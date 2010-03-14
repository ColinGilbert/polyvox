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
#include "PolyVoxImpl/TypeDef.h"
#include "PolyVoxImpl/CPlusPlusZeroXSupport.h"
#pragma endregion

namespace PolyVox
{
	template<typename ElementType>
	class POLYVOXCORE_API Array2D
	{
	public:
		Array2D()
			:m_uWidth(0)
			,m_uHeight(0)
			,m_pData(0)
		{
		}

		Array2D(uint32_t width, uint32_t height)
			:m_uWidth(width)
			,m_uHeight(height)
			,m_pData(0)
		{
			m_pData = new ElementType[width * height];
			assert(m_pData);
		}

		/*Array2D(const Array2D& rhs)
			:m_uWidth(0)
			,m_uHeight(0)
			,m_pData(0)
		{
			*this = rhs;
		}*/

		~Array2D()
		{
			/*if(m_pData)
			{
				delete[] m_pData;
			}
			m_pData = 0;*/
		}

		/*Array2D& operator=(const Array2D& rhs)
		{
			if(this == &rhs)
			{
				return *this;
			}

			if((m_uWidth != rhs.m_uWidth) || (m_uHeight != rhs.m_uHeight))
			{
				if(m_pData)
				{
					delete m_pData;
				}
				m_pData = 0;

				m_uWidth = rhs.m_uWidth;
				m_uHeight = rhs.m_uHeight;

				m_pData = new ElementType[m_uWidth * m_uHeight];
			}

			std::memcpy(m_pData, rhs.m_pData, sizeof(ElementType) * m_uWidth * m_uHeight);
			return *this;
		}*/

		/*ElementType& operator() (uint32_t x, uint32_t y)
		{
			assert(x < m_uWidth);
			assert(y < m_uHeight);
			return m_pData[x * m_uWidth + y];
		}

		ElementType  operator() (uint32_t x, uint32_t y) const
		{
			assert(x < m_uWidth);
			assert(y < m_uHeight);
			return m_pData[x * m_uWidth + y];
		}*/

		ElementType& getElement(uint32_t x, uint32_t y) const
		{
			assert(x < m_uWidth);
			assert(y < m_uHeight);
			return m_pData[x + y * m_uWidth];
		}

		void setElement(uint32_t x, uint32_t y, ElementType value)
		{
			assert(x < m_uWidth);
			assert(y < m_uHeight);
			m_pData[x + y * m_uWidth] = value;
		}

		void swap(Array2D& rhs)
		{
			assert(m_uWidth == rhs.m_uWidth);
			assert(m_uHeight == rhs.m_uHeight);

			ElementType* temp = m_pData;
			m_pData = rhs.m_pData;
			rhs.m_pData = temp;
		}

	private:
		//Dimensions
		uint32_t m_uWidth;
		uint32_t m_uHeight;

		//Data
		ElementType* m_pData;
	};
}//namespace Thermite

#endif