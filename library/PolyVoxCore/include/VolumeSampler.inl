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

#include "PolyVoxImpl/Block.h"
#include "Volume.h"
#include "Vector.h"
#include "Region.h"

#include <limits>
namespace PolyVox
{
	template <typename VoxelType>
	VolumeSampler<VoxelType>::VolumeSampler(Volume<VoxelType>* volume)
		:mVolume(volume)
	{
	}

	template <typename VoxelType>
	VolumeSampler<VoxelType>::~VolumeSampler()
	{
	}

	template <typename VoxelType>
	VolumeSampler<VoxelType>& VolumeSampler<VoxelType>::operator=(const VolumeSampler<VoxelType>& rhs) throw()
	{
		if(this == &rhs)
		{
			return *this;
		}
        mVolume = rhs.mVolume;
		mXPosInVolume = rhs.mXPosInVolume;
		mYPosInVolume = rhs.mYPosInVolume;
		mZPosInVolume = rhs.mZPosInVolume;
		mCurrentVoxel = rhs.mCurrentVoxel;
        return *this;
	}

	template <typename VoxelType>
	uint16_t VolumeSampler<VoxelType>::getPosX(void) const
	{
		return mXPosInVolume;
	}

	template <typename VoxelType>
	uint16_t VolumeSampler<VoxelType>::getPosY(void) const
	{
		return mYPosInVolume;
	}

	template <typename VoxelType>
	uint16_t VolumeSampler<VoxelType>::getPosZ(void) const
	{
		return mZPosInVolume;
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::getSubSampledVoxel(uint8_t uLevel) const
	{		
		if(uLevel == 0)
		{
			return getVoxel();
		}
		else if(uLevel == 1)
		{
			VoxelType tValue = getVoxel();
			tValue = (std::min)(tValue, peekVoxel1px0py0pz());
			tValue = (std::min)(tValue, peekVoxel0px1py0pz());
			tValue = (std::min)(tValue, peekVoxel1px1py0pz());
			tValue = (std::min)(tValue, peekVoxel0px0py1pz());
			tValue = (std::min)(tValue, peekVoxel1px0py1pz());
			tValue = (std::min)(tValue, peekVoxel0px1py1pz());
			tValue = (std::min)(tValue, peekVoxel1px1py1pz());
			return tValue;
		}
		else
		{
			const uint8_t uSize = 1 << uLevel;

			VoxelType tValue = (std::numeric_limits<VoxelType>::max)();
			for(uint8_t z = 0; z < uSize; ++z)
			{
				for(uint8_t y = 0; y < uSize; ++y)
				{
					for(uint8_t x = 0; x < uSize; ++x)
					{
						tValue = (std::min)(tValue, mVolume->getVoxelAt(mXPosInVolume + x, mYPosInVolume + y, mZPosInVolume + z));
					}
				}
			}
			return tValue;
		}
	}

	template <typename VoxelType>
	const Volume<VoxelType>* VolumeSampler<VoxelType>::getVolume(void) const
	{
		return mVolume;
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::getVoxel(void) const
	{
		return *mCurrentVoxel;
	}

	template <typename VoxelType>
	void VolumeSampler<VoxelType>::setPosition(const Vector3DInt16& v3dNewPos)
	{
		setPosition(v3dNewPos.getX(), v3dNewPos.getY(), v3dNewPos.getZ());
	}

	template <typename VoxelType>
	void VolumeSampler<VoxelType>::setPosition(uint16_t xPos, uint16_t yPos, uint16_t zPos)
	{
		mXPosInVolume = xPos;
		mYPosInVolume = yPos;
		mZPosInVolume = zPos;

		const uint16_t uXBlock = mXPosInVolume >> mVolume->m_uBlockSideLengthPower;
		const uint16_t uYBlock = mYPosInVolume >> mVolume->m_uBlockSideLengthPower;
		const uint16_t uZBlock = mZPosInVolume >> mVolume->m_uBlockSideLengthPower;

		const uint16_t uXPosInBlock = mXPosInVolume - (uXBlock << mVolume->m_uBlockSideLengthPower);
		const uint16_t uYPosInBlock = mYPosInVolume - (uYBlock << mVolume->m_uBlockSideLengthPower);
		const uint16_t uZPosInBlock = mZPosInVolume - (uZBlock << mVolume->m_uBlockSideLengthPower);

		const uint32_t uVoxelIndexInBlock = uXPosInBlock + 
				uYPosInBlock * mVolume->m_uBlockSideLength + 
				uZPosInBlock * mVolume->m_uBlockSideLength * mVolume->m_uBlockSideLength;

		if((uXBlock < mVolume->m_uWidthInBlocks) && (uYBlock < mVolume->m_uHeightInBlocks) && (uZBlock < mVolume->m_uDepthInBlocks))
		{
			const uint32_t uBlockIndexInVolume = uXBlock + 
				uYBlock * mVolume->m_uWidthInBlocks + 
				uZBlock * mVolume->m_uWidthInBlocks * mVolume->m_uHeightInBlocks;
			const Block<VoxelType>& currentBlock = mVolume->m_pBlocks[uBlockIndexInVolume];

			Block<VoxelType>* pUncompressedCurrentBlock = mVolume->getUncompressedBlock(uXBlock, uYBlock, uZBlock);

			mCurrentVoxel = pUncompressedCurrentBlock->m_tUncompressedData + uVoxelIndexInBlock;
		}
		else
		{
			mCurrentVoxel = mVolume->m_pUncompressedBorderData + uVoxelIndexInBlock;
		}
	}

	template <typename VoxelType>
	void VolumeSampler<VoxelType>::movePositiveX(void)
	{
		//Note the *pre* increament here
		if((++mXPosInVolume) % mVolume->m_uBlockSideLength != 0)
		{
			//No need to compute new block.
			++mCurrentVoxel;			
		}
		else
		{
			//We've hit the block boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(mXPosInVolume, mYPosInVolume, mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void VolumeSampler<VoxelType>::movePositiveY(void)
	{
		//Note the *pre* increament here
		if((++mYPosInVolume) % mVolume->m_uBlockSideLength != 0)
		{
			//No need to compute new block.
			mCurrentVoxel += mVolume->m_uBlockSideLength;
		}
		else
		{
			//We've hit the block boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(mXPosInVolume, mYPosInVolume, mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void VolumeSampler<VoxelType>::movePositiveZ(void)
	{
		//Note the *pre* increament here
		if((++mZPosInVolume) % mVolume->m_uBlockSideLength != 0)
		{
			//No need to compute new block.
			mCurrentVoxel += mVolume->m_uBlockSideLength * mVolume->m_uBlockSideLength;
		}
		else
		{
			//We've hit the block boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(mXPosInVolume, mYPosInVolume, mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void VolumeSampler<VoxelType>::moveNegativeX(void)
	{
		//Note the *post* decreament here
		if((mXPosInVolume--) % mVolume->m_uBlockSideLength != 0)
		{
			//No need to compute new block.
			--mCurrentVoxel;			
		}
		else
		{
			//We've hit the block boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(mXPosInVolume, mYPosInVolume, mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void VolumeSampler<VoxelType>::moveNegativeY(void)
	{
		//Note the *post* decreament here
		if((mYPosInVolume--) % mVolume->m_uBlockSideLength != 0)
		{
			//No need to compute new block.
			mCurrentVoxel -= mVolume->m_uBlockSideLength;
		}
		else
		{
			//We've hit the block boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(mXPosInVolume, mYPosInVolume, mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void VolumeSampler<VoxelType>::moveNegativeZ(void)
	{
		//Note the *post* decreament here
		if((mZPosInVolume--) % mVolume->m_uBlockSideLength != 0)
		{
			//No need to compute new block.
			mCurrentVoxel -= mVolume->m_uBlockSideLength * mVolume->m_uBlockSideLength;
		}
		else
		{
			//We've hit the block boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(mXPosInVolume, mYPosInVolume, mZPosInVolume);
		}
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1nx1ny1nz(void) const
	{
		if((mXPosInVolume%mVolume->m_uBlockSideLength != 0) && (mYPosInVolume%mVolume->m_uBlockSideLength != 0) && (mZPosInVolume%mVolume->m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - 1 - mVolume->m_uBlockSideLength - mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1nx1ny0pz(void) const
	{
		if((mXPosInVolume%mVolume->m_uBlockSideLength != 0) && (mYPosInVolume%mVolume->m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - 1 - mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1nx1ny1pz(void) const
	{
		if((mXPosInVolume%mVolume->m_uBlockSideLength != 0) && (mYPosInVolume%mVolume->m_uBlockSideLength != 0) && (mZPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel - 1 - mVolume->m_uBlockSideLength + mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1nx0py1nz(void) const
	{
		if((mXPosInVolume%mVolume->m_uBlockSideLength != 0) && (mZPosInVolume%mVolume->m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - 1 - mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1nx0py0pz(void) const
	{
		if((mXPosInVolume%mVolume->m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - 1);
		}
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1nx0py1pz(void) const
	{
		if((mXPosInVolume%mVolume->m_uBlockSideLength != 0) && (mZPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel - 1 + mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1nx1py1nz(void) const
	{
		if((mXPosInVolume%mVolume->m_uBlockSideLength != 0) && (mYPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1) && (mZPosInVolume%mVolume->m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - 1 + mVolume->m_uBlockSideLength - mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1nx1py0pz(void) const
	{
		if((mXPosInVolume%mVolume->m_uBlockSideLength != 0) && (mYPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel - 1 + mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1nx1py1pz(void) const
	{
		if((mXPosInVolume%mVolume->m_uBlockSideLength != 0) && (mYPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1) && (mZPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel - 1 + mVolume->m_uBlockSideLength + mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel0px1ny1nz(void) const
	{
		if((mYPosInVolume%mVolume->m_uBlockSideLength != 0) && (mZPosInVolume%mVolume->m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - mVolume->m_uBlockSideLength - mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel0px1ny0pz(void) const
	{
		if((mYPosInVolume%mVolume->m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel0px1ny1pz(void) const
	{
		if((mYPosInVolume%mVolume->m_uBlockSideLength != 0) && (mZPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel - mVolume->m_uBlockSideLength + mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel0px0py1nz(void) const
	{
		if((mZPosInVolume%mVolume->m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel0px0py0pz(void) const
	{
			return *mCurrentVoxel;
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel0px0py1pz(void) const
	{
		if((mZPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel0px1py1nz(void) const
	{
		if((mYPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1) && (mZPosInVolume%mVolume->m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel + mVolume->m_uBlockSideLength - mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel0px1py0pz(void) const
	{
		if((mYPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel0px1py1pz(void) const
	{
		if((mYPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1) && (mZPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + mVolume->m_uBlockSideLength + mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1px1ny1nz(void) const
	{
		if((mXPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1) && (mYPosInVolume%mVolume->m_uBlockSideLength != 0) && (mZPosInVolume%mVolume->m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel + 1 - mVolume->m_uBlockSideLength - mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1px1ny0pz(void) const
	{
		if((mXPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1) && (mYPosInVolume%mVolume->m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel + 1 - mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1px1ny1pz(void) const
	{
		if((mXPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1) && (mYPosInVolume%mVolume->m_uBlockSideLength != 0) && (mZPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + 1 - mVolume->m_uBlockSideLength + mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1px0py1nz(void) const
	{
		if((mXPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1) && (mZPosInVolume%mVolume->m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel + 1 - mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1px0py0pz(void) const
	{
		if((mXPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + 1);
		}
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1px0py1pz(void) const
	{
		if((mXPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1) && (mZPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + 1 + mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1px1py1nz(void) const
	{
		if((mXPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1) && (mYPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1) && (mZPosInVolume%mVolume->m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel + 1 + mVolume->m_uBlockSideLength - mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1px1py0pz(void) const
	{
		if((mXPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1) && (mYPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + 1 + mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1px1py1pz(void) const
	{
		if((mXPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1) && (mYPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1) && (mZPosInVolume%mVolume->m_uBlockSideLength != mVolume->m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + 1 + mVolume->m_uBlockSideLength + mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume+1);
	}
}
