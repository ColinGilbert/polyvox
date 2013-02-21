#include "PolyVoxCore/MinizCompressor.h"

// Diable things we don't need, and in particular the zlib compatible names which
// would cause conflicts if a user application is using both PolyVox and zlib.
#define MINIZ_NO_STDIO
#define MINIZ_NO_ARCHIVE_APIS
#define MINIZ_NO_TIME
//#define MINIZ_NO_ZLIB_APIS
#define MINIZ_NO_ZLIB_COMPATIBLE_NAMES
//#define MINIZ_NO_MALLOC

#include "PolyVoxCore/Impl/ErrorHandling.h"
// For some unknown reason the miniz library is supplied only as a 
// single .c file without a header. Apparently the only way to use 
// it is then to #include it directly which is what the examples do.
#include "PolyVoxCore/Impl/miniz.c"

#include <sstream>

using namespace std;

namespace PolyVox
{
	MinizCompressor::MinizCompressor()
	{
	}

	MinizCompressor::~MinizCompressor()
	{
	}

	uint32_t MinizCompressor::getMaxCompressedSize(uint32_t uUncompressedInputSize)
	{
		return static_cast<uint32_t>(mz_compressBound(static_cast<mz_ulong>(uUncompressedInputSize)));
	}

	/*uint32_t MinizCompressor::compress(void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength)
	{
		mz_ulong ulDstLength = uDstLength;

		// Do the compression
		int result = mz_compress((unsigned char*)pDstData, &ulDstLength, (const unsigned char*) pSrcData, uSrcLength);
		if(result != MZ_OK)
		{
			stringstream ss;
			ss << "mz_compress() failed with return code '" << result << "'";
			POLYVOX_THROW(std::runtime_error, ss.str());
		}

		// Return the number of bytes written to the output.
		return ulDstLength;
	}*/

	uint32_t MinizCompressor::compress(void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength)
	{
		int level = 9;

		// tdefl_compressor contains all the state needed by the low-level compressor so it's a pretty big struct (~300k).
		// This example makes it a global vs. putting it on the stack, of course in real-world usage you'll probably malloc() or new it.
		tdefl_compressor g_deflator;

		// The number of dictionary probes to use at each compression level (0-10). 0=implies fastest/minimal possible probing.
		static const mz_uint s_tdefl_num_probes[11] = { 0, 1, 6, 32,  16, 32, 128, 256,  512, 768, 1500 };

		// create tdefl() compatible flags (we have to compose the low-level flags ourselves, or use tdefl_create_comp_flags_from_zip_params() but that means MINIZ_NO_ZLIB_APIS can't be defined).
		mz_uint comp_flags = TDEFL_WRITE_ZLIB_HEADER | s_tdefl_num_probes[MZ_MIN(10, level)] | ((level <= 3) ? TDEFL_GREEDY_PARSING_FLAG : 0);
		if (!level)
			comp_flags |= TDEFL_FORCE_ALL_RAW_BLOCKS;

		tdefl_status status = tdefl_init(&g_deflator, NULL, NULL, comp_flags);
		assert(status == TDEFL_STATUS_OKAY);
		if (status != TDEFL_STATUS_OKAY)
		{
			stringstream ss;
			ss << "tdefl_init() failed with return code '" << status << "'";
			POLYVOX_THROW(std::runtime_error, ss.str());
		}

		size_t ulDstLength = uDstLength;
		// Compress as much of the input as possible (or all of it) to the output buffer.
		status = tdefl_compress(&g_deflator, pSrcData, &uSrcLength, pDstData, &ulDstLength, TDEFL_FINISH);

		assert(status == TDEFL_STATUS_DONE);
		if (status != TDEFL_STATUS_DONE)
		{
			stringstream ss;
			ss << "tdefl_compress() failed with return code '" << status << "'";
			POLYVOX_THROW(std::runtime_error, ss.str());
		}

		return ulDstLength;
	}

	/*uint32_t MinizCompressor::decompress(void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength)
	{
		mz_ulong ulDstLength = uDstLength;

		int result = mz_uncompress((unsigned char*) pDstData, &ulDstLength, (const unsigned char*) pSrcData, uSrcLength);
		if(result != MZ_OK)
		{
			stringstream ss;
			ss << "mz_uncompress() failed with return code '" << result << "'";
			POLYVOX_THROW(std::runtime_error, ss.str());
		}

		return ulDstLength;
	}*/

	uint32_t MinizCompressor::decompress(void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength)
	{
		size_t ulDstLength = uDstLength;

		//int result = mz_uncompress((unsigned char*) pDstData, &ulDstLength, (const unsigned char*) pSrcData, uSrcLength);
		//assert(result == MZ_OK);

		//Check dest length is power of two! If it's a problem we could fall back on mz_uncompress at the expense of performance and bringing in more of the library.

		tinfl_decompressor inflator;
		tinfl_init(&inflator);

		// Do the decompression. In some scenarios 'tinfl_decompress' would be called multiple times with the same dest buffer but
		// different locations within it. In our scenario it's only called once so the start and the location are the same (both pDstData).
		tinfl_status status = tinfl_decompress(&inflator, (const mz_uint8 *)pSrcData, &uSrcLength, (mz_uint8 *)pDstData, (mz_uint8 *)pDstData, &ulDstLength, TINFL_FLAG_PARSE_ZLIB_HEADER);

		if (status != TINFL_STATUS_DONE)
		{
			stringstream ss;
			ss << "tinfl_decompress() failed with return code '" << status << "'";
			POLYVOX_THROW(std::runtime_error, ss.str());
		}

		return ulDstLength;
	}
}
