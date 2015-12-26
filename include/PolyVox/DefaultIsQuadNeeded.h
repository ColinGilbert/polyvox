/*******************************************************************************
* The MIT License (MIT)
*
* Copyright (c) 2015 David Williams and Matthew Williams
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*******************************************************************************/

#ifndef __PolyVox_DefaultIsQuadNeeded_H__
#define __PolyVox_DefaultIsQuadNeeded_H__

#include "Impl/PlatformDefinitions.h"

#include <cstdint>

namespace PolyVox
{
	template<typename VoxelType>
	class DefaultIsQuadNeeded
	{
	public:
		bool operator()(VoxelType back, VoxelType front, VoxelType& materialToUse)
		{
			if ((back > 0) && (front == 0))
			{
				materialToUse = static_cast<VoxelType>(back);
				return true;
			}
			else
			{
				return false;
			}
		}
	};
}

#endif //__PolyVox_DefaultIsQuadNeeded_H__