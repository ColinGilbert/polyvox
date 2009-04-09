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

#include <map>
#pragma endregion

namespace PolyVox
{
	template <typename VoxelType>
	class Volume
	{
		//Make VolumeIterator a friend
		friend class VolumeIterator<VoxelType>;

	public:		
		Volume(uint16_t uSideLength, uint16_t uBlockSideLength = 64);
		//Volume(const Volume& rhs);
		~Volume();	

		//Volume& operator=(const Volume& rhs);

		Region getEnclosingRegion(void) const;
		uint16_t getSideLength(void) const;
		VoxelType getVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos) const;
		VoxelType getVoxelAt(const Vector3DUint16& v3dPos) const;

		void setVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos, VoxelType tValue);
		void setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue);

		void idle(uint32_t uAmount);
		bool isRegionHomogenous(const Region& region);

	private:
		POLYVOX_SHARED_PTR< BlockData<VoxelType> > getHomogenousBlockData(VoxelType tHomogenousValue) const;

		Block<VoxelType>* m_pBlocks;
		static std::map<VoxelType, POLYVOX_WEAK_PTR< BlockData<VoxelType> > > m_pHomogenousBlockData;

		uint32_t m_uNoOfBlocksInVolume;
		uint16_t m_uSideLengthInBlocks;

		uint8_t m_uSideLengthPower;
		uint16_t m_uSideLength;

		uint8_t m_uBlockSideLengthPower;
		uint16_t m_uBlockSideLength;
	};

	//Required for the static member
	template <class VoxelType> std::map<VoxelType, POLYVOX_WEAK_PTR< BlockData<VoxelType> > > Volume<VoxelType>::m_pHomogenousBlockData;


	//Some handy typedefs
	typedef Volume<float> FloatVolume;
	typedef Volume<uint8_t> UInt8Volume;
	typedef Volume<uint16_t> UInt16Volume;	
}

#include "Volume.inl"

#endif
