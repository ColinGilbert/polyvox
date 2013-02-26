#include "PolyVoxCore/MinizCompressor.h"

#include "PolyVoxCore/Impl/Utility.h"

// Diable things we don't need, and in particular the zlib compatible names which
// would cause conflicts if a user application is using both PolyVox and zlib.
#define MINIZ_NO_STDIO
#define MINIZ_NO_ARCHIVE_APIS
#define MINIZ_NO_TIME
#define MINIZ_NO_ZLIB_APIS
#define MINIZ_NO_ZLIB_COMPATIBLE_NAMES
#define MINIZ_NO_MALLOC

#include "PolyVoxCore/Impl/ErrorHandling.h"
// For some unknown reason the miniz library is supplied only as a 
// single .c file without a header. Apparently the only way to use 
// it is then to #include it directly which is what the examples do.
// We also disable some warnings as I don't want to fix external code.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wno-enum-compare"
#include "PolyVoxCore/Impl/miniz.c"
#pragma GCC diagnostic pop

#include <sstream>

using namespace std;

namespace PolyVox
{
	// Compression levels: 0-9 are the standard zlib-style levels, 10 is best possible compression (not zlib compatible, and may be very slow) 
	MinizCompressor::MinizCompressor(int iCompressionLevel)
		:m_pDeflator(0)
	{
		// Create and store the deflator.
		tdefl_compressor* pDeflator = new tdefl_compressor;
		m_pDeflator = reinterpret_cast<void*>(pDeflator);

		// The number of dictionary probes to use at each compression level (0-10). 0=implies fastest/minimal possible probing.
		// The discontinuity is unsettling but may be explained by the 'iCompressionLevel <= 3' check later?
		static const mz_uint s_tdefl_num_probes[11] = { 0, 1, 6, 32,  16, 32, 128, 256,  512, 768, 1500 };

		// Create tdefl() compatible flags (we have to compose the low-level flags ourselves, or use tdefl_create_comp_flags_from_zip_params() but that means MINIZ_NO_ZLIB_APIS can't be defined).
		m_uCompressionFlags = TDEFL_WRITE_ZLIB_HEADER | s_tdefl_num_probes[MZ_MIN(10, iCompressionLevel)] | ((iCompressionLevel <= 3) ? TDEFL_GREEDY_PARSING_FLAG : 0);
		if (!iCompressionLevel)
		{
			m_uCompressionFlags |= TDEFL_FORCE_ALL_RAW_BLOCKS;
		}
	}

	MinizCompressor::~MinizCompressor()
	{
		// Delete the deflator
		tdefl_compressor* pDeflator = reinterpret_cast<tdefl_compressor*>(m_pDeflator);
		delete pDeflator;
	}

	uint32_t MinizCompressor::getMaxCompressedSize(uint32_t uUncompressedInputSize)
	{
		// The contents of this function are copied from miniz's 'mz_deflateBound()'
		// (which we can't use because it is part of the zlib-style higher level API).
		unsigned long source_len = uUncompressedInputSize;

		// This is really over conservative. (And lame, but it's actually pretty tricky to compute a true upper bound given the way tdefl's blocking works.)
		return MZ_MAX(128 + (source_len * 110) / 100, 128 + source_len + ((source_len / (31 * 1024)) + 1) * 5);
	}

	uint32_t MinizCompressor::compress(void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength)
	{
		//Get the deflator
		tdefl_compressor* pDeflator = reinterpret_cast<tdefl_compressor*>(m_pDeflator);

		// It seems we have to reinitialise the deflator for each fresh dataset (it's probably intended for streaming, which we're not doing here)
		tdefl_status status = tdefl_init(pDeflator, NULL, NULL, m_uCompressionFlags);
		if (status != TDEFL_STATUS_OKAY)
		{
			stringstream ss;
			ss << "tdefl_init() failed with return code '" << status << "'";
			POLYVOX_THROW(std::runtime_error, ss.str());
		}

		// Change the type to avoid compiler warnings
		size_t ulSrcLength = uSrcLength;
		size_t ulDstLength = uDstLength;

		// Compress as much of the input as possible (or all of it) to the output buffer.
		status = tdefl_compress(pDeflator, pSrcData, &ulSrcLength, pDstData, &ulDstLength, TDEFL_FINISH);

		//Check whther the compression was successful.
		if (status != TDEFL_STATUS_DONE)
		{
			stringstream ss;
			ss << "tdefl_compress() failed with return code '" << status << "'";
			POLYVOX_THROW(std::runtime_error, ss.str());
		}

		// The compression modifies 'ulDstLength' to hold the new length.
		return ulDstLength;
	}

	uint32_t MinizCompressor::decompress(void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength)
	{
		// I don't know exactly why this limitation exists but it's an implementation detail of miniz. It shouldn't matter for our purposes 
		// as our detination is a Block and those are always a power of two. If you need to use this class for other purposes then you'll
		// probably have to scale up your dst buffer to the nearest appropriate size. Alternatively you can use the mz_uncompress function,
		// but that means enabling parts of the miniz API which are #defined out at the top of this file.
		POLYVOX_ASSERT(isPowerOf2(uDstLength), "Miniz decompressor requires the destination buffer to have a size which is a power of two.");
		if(isPowerOf2(uDstLength) == false)
		{
			POLYVOX_THROW(std::invalid_argument, "Miniz decompressor requires the destination buffer to have a size which is a power of two.");
		}

		// Change the type to avoid compiler warnings
		size_t ulSrcLength = uSrcLength;
		size_t ulDstLength = uDstLength;

		// Create and initialise the decompressor (I believe this is much small than the compressor).
		tinfl_decompressor inflator;
		tinfl_init(&inflator);

		// Do the decompression. In some scenarios 'tinfl_decompress' would be called multiple times with the same dest buffer but
		// different locations within it. In our scenario it's only called once so the start and the location are the same (both pDstData).
		tinfl_status status = tinfl_decompress(&inflator, (const mz_uint8 *)pSrcData, &ulSrcLength, (mz_uint8 *)pDstData, (mz_uint8 *)pDstData, &ulDstLength, TINFL_FLAG_PARSE_ZLIB_HEADER);

		//Check whther the decompression was successful.
		if (status != TINFL_STATUS_DONE)
		{
			stringstream ss;
			ss << "tinfl_decompress() failed with return code '" << status << "'";
			POLYVOX_THROW(std::runtime_error, ss.str());
		}

		// The decompression modifies 'ulDstLength' to hold the new length.
		return ulDstLength;
	}
}
