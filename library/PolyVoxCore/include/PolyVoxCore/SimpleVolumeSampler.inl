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
#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/Vector.h"
#include "PolyVoxCore/Region.h"

#define BORDER_LOW(x) ((( x >> mVolume->m_uBlockSideLengthPower) << mVolume->m_uBlockSideLengthPower) != x)
#define BORDER_HIGH(x) ((( (x+1) >> mVolume->m_uBlockSideLengthPower) << mVolume->m_uBlockSideLengthPower) != (x+1))
//#define BORDER_LOW(x) (( x % mVolume->m_uBlockSideLength) != 0)
//#define BORDER_HIGH(x) (( x % mVolume->m_uBlockSideLength) != mVolume->m_uBlockSideLength - 1)

#include <limits>
namespace PolyVox
{
	template <typename VoxelType>
	SimpleVolume<VoxelType>::Sampler::Sampler(SimpleVolume<VoxelType>* volume)
		:Volume<VoxelType>::Sampler< SimpleVolume<VoxelType> >(volume)
	{
	}

	template <typename VoxelType>
	SimpleVolume<VoxelType>::Sampler::~Sampler()
	{
	}

	template <typename VoxelType>
	typename SimpleVolume<VoxelType>::Sampler& SimpleVolume<VoxelType>::Sampler::operator=(const typename SimpleVolume<VoxelType>::Sampler& rhs) throw()
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
	int32_t SimpleVolume<VoxelType>::Sampler::getPosX(void) const
	{
		return mXPosInVolume;
	}

	template <typename VoxelType>
	int32_t SimpleVolume<VoxelType>::Sampler::getPosY(void) const
	{
		return mYPosInVolume;
	}

	template <typename VoxelType>
	int32_t SimpleVolume<VoxelType>::Sampler::getPosZ(void) const
	{
		return mZPosInVolume;
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::getSubSampledVoxel(uint8_t uLevel) const
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
	VoxelType SimpleVolume<VoxelType>::Sampler::getVoxel(void) const
	{
		return *mCurrentVoxel;
	}

	template <typename VoxelType>
	void SimpleVolume<VoxelType>::Sampler::setPosition(const Vector3DInt32& v3dNewPos)
	{
		setPosition(v3dNewPos.getX(), v3dNewPos.getY(), v3dNewPos.getZ());
	}

	template <typename VoxelType>
	void SimpleVolume<VoxelType>::Sampler::setPosition(int32_t xPos, int32_t yPos, int32_t zPos)
	{
		mXPosInVolume = xPos;
		mYPosInVolume = yPos;
		mZPosInVolume = zPos;

		const int32_t uXBlock = mXPosInVolume >> mVolume->m_uBlockSideLengthPower;
		const int32_t uYBlock = mYPosInVolume >> mVolume->m_uBlockSideLengthPower;
		const int32_t uZBlock = mZPosInVolume >> mVolume->m_uBlockSideLengthPower;

		const uint16_t uXPosInBlock = mXPosInVolume - (uXBlock << mVolume->m_uBlockSideLengthPower);
		const uint16_t uYPosInBlock = mYPosInVolume - (uYBlock << mVolume->m_uBlockSideLengthPower);
		const uint16_t uZPosInBlock = mZPosInVolume - (uZBlock << mVolume->m_uBlockSideLengthPower);

		const uint32_t uVoxelIndexInBlock = uXPosInBlock + 
				uYPosInBlock * mVolume->m_uBlockSideLength + 
				uZPosInBlock * mVolume->m_uBlockSideLength * mVolume->m_uBlockSideLength;

		if(mVolume->m_regValidRegionInBlocks.containsPoint(Vector3DInt32(uXBlock, uYBlock, uZBlock)))
		{
			Block* pUncompressedCurrentBlock = mVolume->getUncompressedBlock(uXBlock, uYBlock, uZBlock);

			mCurrentVoxel = pUncompressedCurrentBlock->m_tUncompressedData + uVoxelIndexInBlock;
		}
		else
		{
			mCurrentVoxel = mVolume->m_pUncompressedBorderData + uVoxelIndexInBlock;
		}
	}

	template <typename VoxelType>
	void SimpleVolume<VoxelType>::Sampler::movePositiveX(void)
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
	void SimpleVolume<VoxelType>::Sampler::movePositiveY(void)
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
	void SimpleVolume<VoxelType>::Sampler::movePositiveZ(void)
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
	void SimpleVolume<VoxelType>::Sampler::moveNegativeX(void)
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
	void SimpleVolume<VoxelType>::Sampler::moveNegativeY(void)
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
	void SimpleVolume<VoxelType>::Sampler::moveNegativeZ(void)
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
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1nx1ny1nz(void) const
	{
		if(	BORDER_LOW(mXPosInVolume) && BORDER_LOW(mYPosInVolume) && BORDER_LOW(mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - mVolume->m_uBlockSideLength - mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1nx1ny0pz(void) const
	{
		if(	BORDER_LOW(mXPosInVolume) && BORDER_LOW(mYPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1nx1ny1pz(void) const
	{
		if(	BORDER_LOW(mXPosInVolume) && BORDER_LOW(mYPosInVolume) && BORDER_HIGH(mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - mVolume->m_uBlockSideLength + mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1nx0py1nz(void) const
	{
		if(	BORDER_LOW(mXPosInVolume) && BORDER_LOW(mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1nx0py0pz(void) const
	{
		if( BORDER_LOW(mXPosInVolume) )
		{
			return *(mCurrentVoxel - 1);
		}
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1nx0py1pz(void) const
	{
		if( BORDER_LOW(mXPosInVolume) && BORDER_HIGH(mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1nx1py1nz(void) const
	{
		if( BORDER_LOW(mXPosInVolume) && BORDER_HIGH(mYPosInVolume) && BORDER_LOW(mYPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + mVolume->m_uBlockSideLength - mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1nx1py0pz(void) const
	{
		if( BORDER_LOW(mXPosInVolume) && BORDER_HIGH(mYPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1nx1py1pz(void) const
	{
		if( BORDER_LOW(mXPosInVolume) && BORDER_HIGH(mYPosInVolume) && BORDER_HIGH(mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + mVolume->m_uBlockSideLength + mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel0px1ny1nz(void) const
	{
		if( BORDER_LOW(mYPosInVolume) && BORDER_LOW(mZPosInVolume) )
		{
			return *(mCurrentVoxel - mVolume->m_uBlockSideLength - mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel0px1ny0pz(void) const
	{
		if( BORDER_LOW(mYPosInVolume) )
		{
			return *(mCurrentVoxel - mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel0px1ny1pz(void) const
	{
		if( BORDER_LOW(mYPosInVolume) && BORDER_HIGH(mZPosInVolume) )
		{
			return *(mCurrentVoxel - mVolume->m_uBlockSideLength + mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel0px0py1nz(void) const
	{
		if( BORDER_LOW(mZPosInVolume) )
		{
			return *(mCurrentVoxel - mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel0px0py0pz(void) const
	{
			return *mCurrentVoxel;
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel0px0py1pz(void) const
	{
		if( BORDER_HIGH(mZPosInVolume) )
		{
			return *(mCurrentVoxel + mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel0px1py1nz(void) const
	{
		if( BORDER_HIGH(mYPosInVolume) && BORDER_LOW(mZPosInVolume) )
		{
			return *(mCurrentVoxel + mVolume->m_uBlockSideLength - mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel0px1py0pz(void) const
	{
		if( BORDER_HIGH(mYPosInVolume) )
		{
			return *(mCurrentVoxel + mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel0px1py1pz(void) const
	{
		if( BORDER_HIGH(mYPosInVolume) && BORDER_HIGH(mZPosInVolume) )
		{
			return *(mCurrentVoxel + mVolume->m_uBlockSideLength + mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1px1ny1nz(void) const
	{
		if( BORDER_HIGH(mXPosInVolume) && BORDER_LOW(mYPosInVolume) && BORDER_LOW(mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - mVolume->m_uBlockSideLength - mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1px1ny0pz(void) const
	{
		if( BORDER_HIGH(mXPosInVolume) && BORDER_LOW(mYPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1px1ny1pz(void) const
	{
		if( BORDER_HIGH(mXPosInVolume) && BORDER_LOW(mYPosInVolume) && BORDER_HIGH(mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - mVolume->m_uBlockSideLength + mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1px0py1nz(void) const
	{
		if( BORDER_HIGH(mXPosInVolume) && BORDER_LOW(mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1px0py0pz(void) const
	{
		if( BORDER_HIGH(mXPosInVolume) )
		{
			return *(mCurrentVoxel + 1);
		}
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1px0py1pz(void) const
	{
		if( BORDER_HIGH(mXPosInVolume) && BORDER_HIGH(mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1px1py1nz(void) const
	{
		if( BORDER_HIGH(mXPosInVolume) && BORDER_HIGH(mYPosInVolume) && BORDER_LOW(mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + mVolume->m_uBlockSideLength - mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1px1py0pz(void) const
	{
		if( BORDER_HIGH(mXPosInVolume) && BORDER_HIGH(mYPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1px1py1pz(void) const
	{
		if( BORDER_HIGH(mXPosInVolume) && BORDER_HIGH(mYPosInVolume) && BORDER_HIGH(mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + mVolume->m_uBlockSideLength + mVolume->m_uBlockSideLength*mVolume->m_uBlockSideLength);
		}
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume+1);
	}
}
