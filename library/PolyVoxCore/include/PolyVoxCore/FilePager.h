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

#ifndef __PolyVox_FilePager_H__
#define __PolyVox_FilePager_H__

#include "PolyVoxCore/Impl/TypeDef.h"

#include "PolyVoxCore/Pager.h"
#include "PolyVoxCore/Region.h"

#include <fstream>
#include <stdexcept>
#include <string>

namespace PolyVox
{
	/**
	 * Provides an interface for performing paging of data.
	 */
	template <typename VoxelType>
	class FilePager : public Pager<VoxelType>
	{
	public:
		/// Constructor
		FilePager(const std::string& strFolderName)
			:Pager<VoxelType>()
			,m_strFolderName(strFolderName)
		{
		}

		/// Destructor
		virtual ~FilePager() {};

		virtual void pageIn(const Region& region, Block<VoxelType>* pBlockData)
		{
			POLYVOX_ASSERT(pBlockData, "Attempting to page in NULL block");
			POLYVOX_ASSERT(pBlockData->hasUncompressedData() == false, "Block should not have uncompressed data");

			std::stringstream ss;
			ss << region.getLowerX() << "_" << region.getLowerY() << "_" << region.getLowerZ() << "_"
				 << region.getUpperX() << "_" << region.getUpperY() << "_" << region.getUpperZ();

			std::string filename = m_strFolderName + ss.str();

			// FIXME - This should be replaced by C++ style IO, but currently this causes problems with
			// the gameplay-cubiquity integration. See: https://github.com/blackberry/GamePlay/issues/919

			FILE* pFile = fopen(filename.c_str(), "rb");
			if(pFile)
			{
				logTrace() << "Paging in data for " << region;

				fseek(pFile, 0L, SEEK_END);
				size_t fileSizeInBytes = ftell(pFile);
				fseek(pFile, 0L, SEEK_SET);
				
				uint8_t* buffer = new uint8_t[fileSizeInBytes];
				fread(buffer, sizeof(uint8_t), fileSizeInBytes, pFile);
				pBlockData->setCompressedData(buffer, fileSizeInBytes);
				delete[] buffer;

				if(ferror(pFile))
				{
					POLYVOX_THROW(std::runtime_error, "Error reading in block data, even though a file exists.");
				}

				fclose(pFile);
			}
			else
			{
				logTrace() << "No data found for " << region << " during paging in.";
			}
		}

		virtual void pageOut(const Region& region, Block<VoxelType>* pBlockData)
		{
			POLYVOX_ASSERT(pBlockData, "Attempting to page out NULL block");
			POLYVOX_ASSERT(pBlockData->hasUncompressedData() == false, "Block should not have uncompressed data");

			logTrace() << "Paging out data for " << region;

			std::stringstream ss;
			ss << region.getLowerX() << "_" << region.getLowerY() << "_" << region.getLowerZ() << "_"
				 << region.getUpperX() << "_" << region.getUpperY() << "_" << region.getUpperZ();

			std::string filename = m_strFolderName + ss.str();

			// FIXME - This should be replaced by C++ style IO, but currently this causes problems with
			// the gameplay-cubiquity integration. See: https://github.com/blackberry/GamePlay/issues/919

			FILE* pFile = fopen(filename.c_str(), "wb");
			if(!pFile)
			{
				POLYVOX_THROW(std::runtime_error, "Unable to open file to write out block data.");
			}

			fwrite(pBlockData->getCompressedData(), sizeof(uint8_t), pBlockData->getCompressedDataLength(), pFile);

			if(ferror(pFile))
			{
				POLYVOX_THROW(std::runtime_error, "Error writing out block data.");
			}

			fclose(pFile);
		}

	protected:
		std::string m_strFolderName;
	};
}

#endif //__PolyVox_FilePager_H__
