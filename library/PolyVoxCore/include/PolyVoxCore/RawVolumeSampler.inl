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

#include "PolyVoxCore\Impl\Utility.h"

#define BORDER_LOWX(val) (val > this->mVolume->getEnclosingRegion().getLowerCorner().getX())
#define BORDER_HIGHX(val) (val < this->mVolume->getEnclosingRegion().getUpperCorner().getX())
#define BORDER_LOWY(val) (val > this->mVolume->getEnclosingRegion().getLowerCorner().getY())
#define BORDER_HIGHY(val) (val < this->mVolume->getEnclosingRegion().getUpperCorner().getY())
#define BORDER_LOWZ(val) (val > this->mVolume->getEnclosingRegion().getLowerCorner().getZ())
#define BORDER_HIGHZ(val) (val < this->mVolume->getEnclosingRegion().getUpperCorner().getZ())

namespace PolyVox
{
	template <typename VoxelType>
	RawVolume<VoxelType>::Sampler::Sampler(RawVolume<VoxelType>* volume)
		:BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >(volume)
		,mCurrentVoxel(0)
		,m_bIsCurrentPositionValidInX(false)
		,m_bIsCurrentPositionValidInY(false)
		,m_bIsCurrentPositionValidInZ(false)
	{
	}

	template <typename VoxelType>
	RawVolume<VoxelType>::Sampler::~Sampler()
	{
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::getVoxel(void) const
	{
		if(isCurrentPositionValid())
		{
			return *mCurrentVoxel;
		}
		else
		{
			return getVoxelAt(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::setPosition(const Vector3DInt32& v3dNewPos)
	{
		setPosition(v3dNewPos.getX(), v3dNewPos.getY(), v3dNewPos.getZ());
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::setPosition(int32_t xPos, int32_t yPos, int32_t zPos)
	{
		this->mXPosInVolume = xPos;
		this->mYPosInVolume = yPos;
		this->mZPosInVolume = zPos;

		const Vector3DInt32& v3dLowerCorner = this->mVolume->m_regValidRegion.getLowerCorner();
		int32_t iLocalXPos = xPos - v3dLowerCorner.getX();
		int32_t iLocalYPos = yPos - v3dLowerCorner.getY();
		int32_t iLocalZPos = zPos - v3dLowerCorner.getZ();

		const int32_t uVoxelIndex = iLocalXPos + 
				iLocalYPos * this->mVolume->getWidth() + 
				iLocalZPos * this->mVolume->getWidth() * this->mVolume->getHeight();

		mCurrentVoxel = this->mVolume->m_pData + uVoxelIndex;

		m_bIsCurrentPositionValidInX = this->mVolume->getEnclosingRegion().containsPointInX(xPos);
		m_bIsCurrentPositionValidInY = this->mVolume->getEnclosingRegion().containsPointInY(yPos);
		m_bIsCurrentPositionValidInZ = this->mVolume->getEnclosingRegion().containsPointInZ(zPos);
	}

	template <typename VoxelType>
	bool RawVolume<VoxelType>::Sampler::setVoxel(VoxelType tValue)
	{
		//return m_bIsCurrentPositionValid ? *mCurrentVoxel : this->mVolume->getBorderValue();
		if(m_bIsCurrentPositionValidInX && m_bIsCurrentPositionValidInY && m_bIsCurrentPositionValidInZ)
		{
			*mCurrentVoxel = tValue;
			return true;
		}
		else
		{
			return false;
		}
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::movePositiveX(void)
	{
		this->mXPosInVolume++;
		++mCurrentVoxel;
		m_bIsCurrentPositionValidInX = this->mVolume->getEnclosingRegion().containsPointInX(this->mXPosInVolume);
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::movePositiveY(void)
	{
		this->mYPosInVolume++;
		mCurrentVoxel += this->mVolume->getWidth();
		m_bIsCurrentPositionValidInY = this->mVolume->getEnclosingRegion().containsPointInY(this->mYPosInVolume);
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::movePositiveZ(void)
	{
		this->mZPosInVolume++;
		mCurrentVoxel += this->mVolume->getWidth() * this->mVolume->getHeight();
		m_bIsCurrentPositionValidInZ = this->mVolume->getEnclosingRegion().containsPointInZ(this->mZPosInVolume);
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::moveNegativeX(void)
	{
		this->mXPosInVolume--;
		--mCurrentVoxel;
		m_bIsCurrentPositionValidInX = this->mVolume->getEnclosingRegion().containsPointInX(this->mXPosInVolume);
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::moveNegativeY(void)
	{
		this->mYPosInVolume--;
		mCurrentVoxel -= this->mVolume->getWidth();
		m_bIsCurrentPositionValidInY = this->mVolume->getEnclosingRegion().containsPointInY(this->mYPosInVolume);
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::moveNegativeZ(void)
	{
		this->mZPosInVolume--;
		mCurrentVoxel -= this->mVolume->getWidth() * this->mVolume->getHeight();
		m_bIsCurrentPositionValidInZ = this->mVolume->getEnclosingRegion().containsPointInZ(this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1ny1nz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_LOWX(this->mXPosInVolume) && BORDER_LOWY(this->mYPosInVolume) && BORDER_LOWZ(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume-1,this->mYPosInVolume-1,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1ny0pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_LOWX(this->mXPosInVolume) && BORDER_LOWY(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth());
		}
		return this->getVoxelAt(this->mXPosInVolume-1,this->mYPosInVolume-1,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1ny1pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_LOWX(this->mXPosInVolume) && BORDER_LOWY(this->mYPosInVolume) && BORDER_HIGHZ(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume-1,this->mYPosInVolume-1,this->mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx0py1nz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_LOWX(this->mXPosInVolume) && BORDER_LOWZ(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume-1,this->mYPosInVolume,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx0py0pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_LOWX(this->mXPosInVolume) )
		{
			return *(mCurrentVoxel - 1);
		}
		return this->getVoxelAt(this->mXPosInVolume-1,this->mYPosInVolume,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx0py1pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_LOWX(this->mXPosInVolume) && BORDER_HIGHZ(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume-1,this->mYPosInVolume,this->mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1py1nz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_LOWX(this->mXPosInVolume) && BORDER_HIGHY(this->mYPosInVolume) && BORDER_LOWZ(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume-1,this->mYPosInVolume+1,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1py0pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_LOWX(this->mXPosInVolume) && BORDER_HIGHY(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth());
		}
		return this->getVoxelAt(this->mXPosInVolume-1,this->mYPosInVolume+1,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1py1pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_LOWX(this->mXPosInVolume) && BORDER_HIGHY(this->mYPosInVolume) && BORDER_HIGHZ(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume-1,this->mYPosInVolume+1,this->mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1ny1nz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_LOWX(this->mYPosInVolume) && BORDER_LOWZ(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume,this->mYPosInVolume-1,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1ny0pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_LOWY(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel - this->mVolume->getWidth());
		}
		return this->getVoxelAt(this->mXPosInVolume,this->mYPosInVolume-1,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1ny1pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_LOWY(this->mYPosInVolume) && BORDER_HIGHZ(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume,this->mYPosInVolume-1,this->mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px0py1nz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_LOWZ(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume,this->mYPosInVolume,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px0py0pz(void) const
	{
		if((this->isCurrentPositionValid()))
		{
			return *mCurrentVoxel;
		}
		return this->getVoxelAt(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px0py1pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGHZ(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume,this->mYPosInVolume,this->mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1py1nz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGHY(this->mYPosInVolume) && BORDER_LOWZ(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume,this->mYPosInVolume+1,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1py0pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGHY(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel + this->mVolume->getWidth());
		}
		return this->getVoxelAt(this->mXPosInVolume,this->mYPosInVolume+1,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1py1pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGHY(this->mYPosInVolume) && BORDER_HIGHZ(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume,this->mYPosInVolume+1,this->mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1ny1nz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGHX(this->mXPosInVolume) && BORDER_LOWY(this->mYPosInVolume) && BORDER_LOWZ(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume+1,this->mYPosInVolume-1,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1ny0pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGHX(this->mXPosInVolume) && BORDER_LOWY(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth());
		}
		return this->getVoxelAt(this->mXPosInVolume+1,this->mYPosInVolume-1,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1ny1pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGHX(this->mXPosInVolume) && BORDER_LOWY(this->mYPosInVolume) && BORDER_HIGHZ(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume+1,this->mYPosInVolume-1,this->mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px0py1nz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGHX(this->mXPosInVolume) && BORDER_LOWZ(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume+1,this->mYPosInVolume,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px0py0pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGHX(this->mXPosInVolume) )
		{
			return *(mCurrentVoxel + 1);
		}
		return this->getVoxelAt(this->mXPosInVolume+1,this->mYPosInVolume,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px0py1pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGHX(this->mXPosInVolume) && BORDER_HIGHZ(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume+1,this->mYPosInVolume,this->mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1py1nz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGHX(this->mXPosInVolume) && BORDER_HIGHY(this->mYPosInVolume) && BORDER_LOWZ(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume+1,this->mYPosInVolume+1,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1py0pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGHX(this->mXPosInVolume) && BORDER_HIGHY(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + this->mVolume->getWidth());
		}
		return this->getVoxelAt(this->mXPosInVolume+1,this->mYPosInVolume+1,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1py1pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGHX(this->mXPosInVolume) && BORDER_HIGHY(this->mYPosInVolume) && BORDER_HIGHZ(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume+1,this->mYPosInVolume+1,this->mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::getVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos) const
	{
		if(this->mVolume->getEnclosingRegion().containsPoint(Vector3DInt32(uXPos, uYPos, uZPos))) //Would be better if we didn't have to build the Vector but could pass seperate params.
		{
			return this->mVolume->getVoxelAt(uXPos, uYPos, uZPos);
		}
		else
		{
			switch(m_eWrapMode)
			{
				case WrapModes::Clamp:
				{
					const Vector3DInt32& lowerCorner = this->mVolume->m_regValidRegion.getLowerCorner();
					const Vector3DInt32& upperCorner = this->mVolume->m_regValidRegion.getUpperCorner();

					int32_t iClampedX = clamp(uXPos, lowerCorner.getX(), upperCorner.getX());
					int32_t iClampedY = clamp(uYPos, lowerCorner.getY(), upperCorner.getY());
					int32_t iClampedZ = clamp(uZPos, lowerCorner.getZ(), upperCorner.getZ());

					return this->mVolume->getVoxelAt(iClampedX, iClampedY, iClampedZ);
					//No need to break as we've returned
				}
				case WrapModes::Border:
				{
					return this->m_tBorder;
					//No need to break as we've returned
				}
				default:
				{
					//Should never happen
					assert(false);
					return VoxelType(0);
				}
			}
		}
	}

	template <typename VoxelType>
	bool RawVolume<VoxelType>::Sampler::isCurrentPositionValid(void) const
	{
		return m_bIsCurrentPositionValidInX && m_bIsCurrentPositionValidInY && m_bIsCurrentPositionValidInZ;
	}
}

#undef BORDER_LOWX
#undef BORDER_HIGHX
#undef BORDER_LOWY
#undef BORDER_HIGHY
#undef BORDER_LOWZ
#undef BORDER_HIGHZ
