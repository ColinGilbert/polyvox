#include "PolyVoxCore/Impl/Compression.h"
#include "PolyVoxCore/Impl/ErrorHandling.h"

// The miniz library is supplied only as a single .c file without a header.
// Apparently the only way to use it would then be to #include it directly
// which is what the examples do.
#include "PolyVoxCore/Impl/miniz.c"

Data polyvox_compress(Data src)
{
	POLYVOX_ASSERT(src.ptr != 0, "Source data cannot be null");

	unsigned char* buffer;
	mz_ulong compressedDataLength = compressBound(src.length);
	buffer = new unsigned char[compressedDataLength];

	int iCompressionResult = compress(buffer, &compressedDataLength, static_cast<const unsigned char*>(src.ptr), src.length);

	// Debug more checking
	POLYVOX_ASSERT(iCompressionResult == Z_OK, "Data compression failed.");

	// Release mode checking
    if (iCompressionResult != Z_OK)
    {
		delete[] buffer;
		POLYVOX_THROW(std::runtime_error, "Data compression failed.");
    }

	Data dst;
	dst.length = compressedDataLength;
	dst.ptr = new uint8_t[dst.length];

	memcpy(dst.ptr, buffer, compressedDataLength);

	delete[] buffer;

	return dst;
}

Data polyvox_decompress(Data src)
{
	unsigned char* buffer;
	mz_ulong uncompressedDataLength = 1000000;
	buffer = new unsigned char[uncompressedDataLength];

	int iUncompressionResult = uncompress(buffer, &uncompressedDataLength, src.ptr, src.length);

	if (iUncompressionResult != Z_OK)
    {
		delete[] buffer;
		POLYVOX_THROW(std::runtime_error, "Data decompression failed.");
    }

	Data dst;
	dst.length = uncompressedDataLength;
	dst.ptr = new uint8_t[dst.length];

	memcpy(dst.ptr, buffer, uncompressedDataLength);

	delete[] buffer;

	return dst;
}