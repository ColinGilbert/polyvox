#pragma region License
/******************************************************************************
This file is part of the PolyVox library
Copyright (C) 2006  David Williams

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
******************************************************************************/
#pragma endregion

#ifndef __PolyVox_Volume_H__
#define __PolyVox_Volume_H__

#pragma region Headers
#include "PolyVoxImpl/Block.h"
#include "PolyVoxForwardDeclarations.h"

#include "PolyVoxImpl/CPlusPlusZeroXSupport.h"

#include <limits>
#include <map>
#include <vector>

#pragma endregion

namespace PolyVox
{
	template <typename VoxelType>
	class Volume
	{
		//Make VolumeIterator a friend
		friend class VolumeIterator<VoxelType>;

	public:		
		Volume(uint16_t uWidth, uint16_t uHeight, uint16_t uDepth, uint16_t uBlockSideLength = 64);
		//Volume(const Volume& rhs);
		~Volume();	

		//Volume& operator=(const Volume& rhs);

		Region getEnclosingRegion(void) const;
		uint16_t getWidth(void) const;
		uint16_t getHeight(void) const;
		uint16_t getDepth(void) const;
		uint16_t getLongestSideLength(void) const;
		uint16_t getShortestSideLength(void) const;
		float getDiagonalLength(void) const;
		VoxelType getVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos) const;
		VoxelType getVoxelAt(const Vector3DUint16& v3dPos) const;

		void setVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos, VoxelType tValue);
		void setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue);

		void tidyUpMemory(uint32_t uNoOfBlocksToProcess = (std::numeric_limits<uint32_t>::max)());
		bool isRegionHomogenous(const Region& region);

	private:
		POLYVOX_SHARED_PTR< Block<VoxelType> > getHomogenousBlock(VoxelType tHomogenousValue) const;

		std::vector< POLYVOX_SHARED_PTR< Block<VoxelType> > > m_pBlocks;
		std::vector<bool> m_vecBlockIsPotentiallyHomogenous;

		//Note: We were once storing weak_ptr's in this map, so that the blocks would be deleted once they
		//were not being referenced by anyone else. However, this made it difficult to know when a block was
		//shared. A call to shared_ptr::unique() from within setVoxel was not sufficient as weak_ptr's did
		//not contribute to the reference count. Instead we store shared_ptr's here, and check if they
		//are used by anyone else (i.e are non-unique) when we tidy the volume.
		static std::map<VoxelType, POLYVOX_SHARED_PTR< Block<VoxelType> > > m_pHomogenousBlock;

		uint32_t m_uNoOfBlocksInVolume;

		uint16_t m_uWidthInBlocks;
		uint16_t m_uHeightInBlocks;
		uint16_t m_uDepthInBlocks;

		uint16_t m_uWidth;
		uint8_t m_uWidthPower;		

		uint16_t m_uHeight;
		uint8_t m_uHeightPower;	

		uint16_t m_uDepth;
		uint8_t m_uDepthPower;	

		uint8_t m_uBlockSideLengthPower;
		uint16_t m_uBlockSideLength;

		uint16_t m_uLongestSideLength;
		uint16_t m_uShortestSideLength;

		float m_fDiagonalLength;

		uint32_t m_uCurrentBlockForTidying;
	};

	//Required for the static member
	template <class VoxelType> std::map<VoxelType, POLYVOX_SHARED_PTR< Block<VoxelType> > > Volume<VoxelType>::m_pHomogenousBlock;


	//Some handy typedefs
	typedef Volume<float> FloatVolume;
	typedef Volume<uint8_t> UInt8Volume;
	typedef Volume<uint16_t> UInt16Volume;	
}

#include "Volume.inl"

#endif
