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
#include "PolyVoxForwardDeclarations.h"

#include "PolyVoxCStdInt.h"

#include <map>
#pragma endregion

namespace PolyVox
{
	template <typename VoxelType>
	class Block
	{
	public:
		BlockData<VoxelType>* m_pBlockData;
		VoxelType m_pHomogenousValue;
		bool m_pIsShared;
		bool m_pIsPotentiallySharable;
	};

	template <typename VoxelType>
	class Volume
	{
		//Make VolumeIterator a friend
		friend class VolumeIterator<VoxelType>;

	public:		
		Volume(uint16 uSideLength, uint16 uBlockSideLength = 64);
		Volume(const Volume& rhs);
		~Volume();	

		Volume& operator=(const Volume& rhs);

		Region getEnclosingRegion(void) const;
		uint16 getSideLength(void) const;
		VoxelType getVoxelAt(uint16 uXPos, uint16 uYPos, uint16 uZPos) const;
		VoxelType getVoxelAt(const Vector3DUint16& v3dPos) const;

		void setVoxelAt(uint16 uXPos, uint16 uYPos, uint16 uZPos, VoxelType tValue);
		void setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue);

		VolumeIterator<VoxelType> firstVoxel(void);
		void idle(uint32 uAmount);
		bool isRegionHomogenous(const Region& region);
		VolumeIterator<VoxelType> lastVoxel(void);

	private:
		BlockData<VoxelType>* getHomogenousBlock(VoxelType tHomogenousValue) const;

		//Block<VoxelType>** m_pBlocks;
		//bool* m_pIsShared;
		//bool* m_pIsPotentiallySharable;
		//VoxelType* m_pHomogenousValue;
		Block<VoxelType>* m_pBlocks;
		mutable std::map<VoxelType, BlockData<VoxelType>*> m_pHomogenousBlocks;

		uint32 m_uNoOfBlocksInVolume;
		uint16 m_uSideLengthInBlocks;

		uint8 m_uSideLengthPower;
		uint16 m_uSideLength;

		uint8 m_uBlockSideLengthPower;
		uint16 m_uBlockSideLength;
	};

	//Some handy typedefs
	typedef Volume<float> FloatVolume;
	typedef Volume<uint8> UInt8Volume;
	typedef Volume<uint16> UInt16Volume;	
}

#include "Volume.inl"

#endif
