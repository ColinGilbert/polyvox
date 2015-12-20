/*******************************************************************************
Copyright (c) 2010 Matt Williams

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

#ifndef __PolyVox_TestVolume_H__
#define __PolyVox_TestVolume_H__

#include "PolyVox/FilePager.h"
#include "PolyVox/PagedVolume.h"
#include "PolyVox/RawVolume.h"
#include "PolyVox/Region.h"

#include <QObject>

class TestVolume: public QObject
{
	Q_OBJECT

public:
	TestVolume();
	~TestVolume();
	
private slots:
	void testRawVolumeDirectAccessAllInternalForwards();
	void testRawVolumeSamplersAllInternalForwards();
	void testRawVolumeDirectAccessWithExternalForwards();
	void testRawVolumeSamplersWithExternalForwards();
	void testRawVolumeDirectAccessAllInternalBackwards();
	void testRawVolumeSamplersAllInternalBackwards();
	void testRawVolumeDirectAccessWithExternalBackwards();
	void testRawVolumeSamplersWithExternalBackwards();

	void testPagedVolumeDirectAccessAllInternalForwards();
	void testPagedVolumeSamplersAllInternalForwards();
	void testPagedVolumeDirectAccessWithExternalForwards();
	void testPagedVolumeSamplersWithExternalForwards();
	void testPagedVolumeDirectAccessAllInternalBackwards();
	void testPagedVolumeSamplersAllInternalBackwards();
	void testPagedVolumeDirectAccessWithExternalBackwards();
	void testPagedVolumeSamplersWithExternalBackwards();

	void testRawVolumeDirectRandomAccess();
	void testPagedVolumeDirectRandomAccess();

	void testPagedVolumeChunkLocalAccess();
	void testPagedVolumeChunkRandomAccess();

private:
	int32_t testPagedVolumeChunkAccess(uint16_t localityMask);

	static const uint16_t m_uChunkSideLength = 32;

	PolyVox::Region m_regVolume;
	PolyVox::Region m_regInternal;
	PolyVox::Region m_regExternal;
	PolyVox::FilePager<int32_t>* m_pFilePager;
	PolyVox::FilePager<int32_t>* m_pFilePagerHighMem;

	PolyVox::RawVolume<int32_t>* m_pRawVolume;
	PolyVox::PagedVolume<int32_t>* m_pPagedVolume;
	PolyVox::PagedVolume<int32_t>* m_pPagedVolumeHighMem;

	PolyVox::PagedVolume<uint32_t>::Chunk* m_pPagedVolumeChunk;
};

#endif
