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

#ifndef __PolyVox_Compressor_H__
#define __PolyVox_Compressor_H__

#include "PolyVoxCore/Impl/TypeDef.h"

namespace PolyVox
{
	/**
	 * Provides an interface for performing compression of data.
	 *
	 * This class provides an interface which can be implemented by derived classes which perform data compression.
	 * The main purpose of this is to allow the user to change the compression algorithm which is used by a LargeVolume,
	 * based on the kind of voxel data it is storing. Users may also wish to use Compressor subclasses in more general
	 * compression-related scenarios but this is not well tested.
	 *
	 * If you wish to implement your own compression algorithms for use in PolyVox then you should begin by subclassing this class.
	 *
	 * \sa MinizCompressor, RLECompressor
	 */
	class Compressor
	{
	public:
		/// Constructor
		Compressor() {};
		/// Destructor
		virtual ~Compressor() {};

		/**
		 * Computes a worst-case scenario for how big the output can be for a given input size.
		 *
		 * If necessary you can use this as a destination buffer size, though it may be somewhat
		 * wasteful. It is not guarenteed that compression actually shrinks the data, so the 
		 * worst-case value returned by this function may be bigger than the input size.
		 *
		 * \param uUncompressedInputSize The size of the uncompressed input data
		 * \return The largest possible size of the compressed output data.
		 */
		virtual uint32_t getMaxCompressedSize(uint32_t uUncompressedInputSize) = 0;

		/**
		 * Compresses the data.
		 * 
		 * Performs compression of the data pointed to by pSrcData and stores the result in pDstData.
		 * The user is responsible for allocating both buffers and for making sure that the destination
		 * buffer is large enough to hold the result. If you don't know how big the compressed data
		 * will be (and you probably won't know this) then you can call getMaxCompressedSize() to get
		 * an upper bound. The *actual* compressed size is then returned by this function and you can
		 * use this to copy your compressed data to a more suitably size buffer.
		 *
		 * \param pSrcData A pointer to the data to be compressed.
		 * \param uSrcLength The length of the data to be compressed.
		 * \param pDstData A pointer to the memory where the result should be stored.
		 * \param uDstLength The length of the destination buffer (compression will fail if this isn't big enough).
		 * \return The size of the resulting compressed data.
		 */
		virtual uint32_t compress(const void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength) = 0;

		/**
		 * Decompresses the data.
		 *  
		 * Performs decompression of the data pointed to by pSrcData and stores the result in pDstData.
		 * The user is responsible for allocating both buffers and for making sure that the destination
		 * buffer is large enough to hold the result. This means you need to know how large the resulting
		 * data might be, so before you compress the data it may be worth storing this information somewhere.
		 * The *actual* decompressed size is then returned by this function
		 * 
		 * \param pSrcData A pointer to the data to be decompressed.
		 * \param uSrcLength The length of the data to be decompressed.
		 * \param pDstData A pointer to the memory where the result should be stored.
		 * \param uDstLength The length of the destination buffer (decompression will fail if this isn't big enough).
		 * \return The size of the resulting uncompressed data.
		 */
		virtual uint32_t decompress(const void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength) = 0;
	};
}

#endif //__PolyVox_Compressor_H__
