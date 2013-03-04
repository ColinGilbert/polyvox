#ifndef __PolyVox_RLECompressor_H__
#define __PolyVox_RLECompressor_H__

#include "PolyVoxCore/Compressor.h"

namespace PolyVox
{
	/**
	 * Performs compression of data using Run Length Encoding (RLE).
	 *
	 * This compressor is designed for voxel data which contains long runs of the same value. Minecraft-style terrain and other
	 * cubic-style terrains are likely to fall under this category, whereas density fields for Marching Cubes terrain will not. Please
	 * see the following article if you want more details of how RLE compression works: http://en.wikipedia.org/wiki/Run-length_encoding
	 *
	 * \sa MinizCompressor
	 */
	template<typename ValueType, typename LengthType>
	class RLECompressor : public Compressor
	{
		struct Run
		{
			ValueType value;
			LengthType length;
		};
	public:
		/// Constructor
		RLECompressor();
		/// Destructor
		~RLECompressor();

		// API documentation is in base class and gets inherited by Doxygen.
		uint32_t getMaxCompressedSize(uint32_t uUncompressedInputSize);
		uint32_t compress(void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength);
		uint32_t decompress(void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength);
	};
}

#include "RLECompressor.inl"

#endif //__PolyVox_RLECompressor_H__
