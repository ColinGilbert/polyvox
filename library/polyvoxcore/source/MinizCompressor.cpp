#include "PolyVoxCore/MinizCompressor.h"

// Diable things we don't need, and in particular the zlib compatible names which
// would cause conflicts if a user application is using both PolyVox and zlib.
#define MINIZ_NO_STDIO
#define MINIZ_NO_ARCHIVE_APIS
#define MINIZ_NO_TIME
//#define MINIZ_NO_ZLIB_APIS
#define MINIZ_NO_ZLIB_COMPATIBLE_NAMES
//#define MINIZ_NO_MALLOC

// For some unknown reason the miniz library is supplied only as a 
// single .c file without a header. Apparently the only way to use 
// it is then to #include it directly which is what the examples do.
#include "PolyVoxCore/Impl/miniz.c"

namespace PolyVox
{
	MinizCompressor::MinizCompressor()
	{
	}

	MinizCompressor::~MinizCompressor()
	{
	}

	uint32_t MinizCompressor::compress(void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength)
	{
		mz_ulong ulDstLength = uDstLength;

		// Do the compression
		int result = mz_compress((unsigned char*)pDstData, &ulDstLength, (const unsigned char*) pSrcData, uSrcLength);
		assert(result == MZ_OK);

		// Return the number of bytes written to the output.
		return ulDstLength;
	}

	uint32_t MinizCompressor::decompress(void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength)
	{
		mz_ulong ulDstLength = uDstLength;

		int result = mz_uncompress((unsigned char*) pDstData, &ulDstLength, (const unsigned char*) pSrcData, uSrcLength);
		assert(result == MZ_OK);

		return ulDstLength;
	}
}
