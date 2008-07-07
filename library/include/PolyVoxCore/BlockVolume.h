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

#ifndef __PolyVox_BlockVolume_H__
#define __PolyVox_BlockVolume_H__

#pragma region Headers
#include "PolyVoxForwardDeclarations.h"

#include "PolyVoxCStdInt.h"

#include <map>
#pragma endregion

namespace PolyVox
{
	template <typename VoxelType>
	class BlockVolume
	{
		//Make BlockVolumeIterator a friend
		friend class BlockVolumeIterator<VoxelType>;

	public:		
		BlockVolume(uint8 uSideLengthPower, uint8 uBlockSideLengthPower = 5);
		BlockVolume(const BlockVolume& rhs);
		~BlockVolume();	

		BlockVolume& operator=(const BlockVolume& rhs);

		Region getEnclosingRegion(void) const;
		uint16 getSideLength(void) const;
		VoxelType getVoxelAt(uint16 uXPos, uint16 uYPos, uint16 uZPos) const;
		VoxelType getVoxelAt(const Vector3DUint16& v3dPos) const;

		bool containsPoint(const Vector3DFloat& pos, float boundary) const;
		bool containsPoint(const Vector3DInt32& pos, uint16 boundary) const;
		BlockVolumeIterator<VoxelType> firstVoxel(void);
		void idle(uint32 uAmount);
		bool isRegionHomogenous(const Region& region);
		BlockVolumeIterator<VoxelType> lastVoxel(void);

	private:
		Block<VoxelType>* getHomogenousBlock(VoxelType tHomogenousValue) const;

		Block<VoxelType>** m_pBlocks;
		bool* m_pIsShared;
		bool* m_pIsPotentiallySharable;
		VoxelType* m_pHomogenousValue;
		mutable std::map<VoxelType, Block<VoxelType>*> m_pHomogenousBlocks;

		uint32 m_uNoOfBlocksInVolume;
		uint16 m_uSideLengthInBlocks;

		uint8 m_uSideLengthPower;
		uint16 m_uSideLength;

		uint8 m_uBlockSideLengthPower;
		uint16 m_uBlockSideLength;
	};

	//Some handy typedefs
	typedef BlockVolume<float> FloatBlockVolume;
	typedef BlockVolume<uint8> UInt8BlockVolume;
	typedef BlockVolume<uint16> UInt16BlockVolume;	
}

#include "BlockVolume.inl"

#endif
