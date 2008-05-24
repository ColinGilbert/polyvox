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

#include "boost/cstdint.hpp"

#include <map>
#pragma endregion

namespace PolyVox
{
	template <typename VoxelType>
	class BlockVolume
	{
		//Make VolumeIterator a friend
		friend class VolumeIterator<VoxelType>;

	public:		
		BlockVolume(boost::uint8_t uSideLengthPower, boost::uint8_t uBlockSideLengthPower = 5);
		BlockVolume(const BlockVolume& rhs);
		~BlockVolume();	

		BlockVolume& operator=(const BlockVolume& rhs);

		boost::uint16_t getSideLength(void) const;
		VoxelType getVoxelAt(boost::uint16_t uXPos, boost::uint16_t uYPos, boost::uint16_t uZPos) const;
		VoxelType getVoxelAt(const Vector3DUint16& v3dPos) const;

		bool containsPoint(Vector3DFloat pos, float boundary) const;
		bool containsPoint(Vector3DInt32 pos, boost::uint16_t boundary) const;
		VolumeIterator<VoxelType> firstVoxel(void);
		void idle(boost::uint32_t uAmount);
		VolumeIterator<VoxelType> lastVoxel(void);

	private:
		Block<VoxelType>* getHomogenousBlock(VoxelType tHomogenousValue) const;

		Block<VoxelType>** m_pBlocks;
		bool* m_pIsShared;
		bool* m_pIsPotentiallySharable;
		VoxelType* m_pHomogenousValue;
		mutable std::map<VoxelType, Block<VoxelType>*> m_pHomogenousBlocks;

		boost::uint32_t m_uNoOfBlocksInVolume;
		boost::uint16_t m_uSideLengthInBlocks;

		boost::uint8_t m_uSideLengthPower;
		boost::uint16_t m_uSideLength;

		boost::uint8_t m_uBlockSideLengthPower;
		boost::uint16_t m_uBlockSideLength;
	};

	//Some handy typedefs
	typedef BlockVolume<float> FloatBlockVolume;
	typedef BlockVolume<boost::uint8_t> UInt8BlockVolume;
	typedef BlockVolume<boost::uint16_t> UInt16BlockVolume;	
}

#include "BlockVolume.inl"

#endif
