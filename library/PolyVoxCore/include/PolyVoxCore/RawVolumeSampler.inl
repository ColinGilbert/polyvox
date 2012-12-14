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

#define CAN_GO_NEG_X(val) (val > this->mVolume->getEnclosingRegion().getLowerCorner().getX())
#define CAN_GO_POS_X(val) (val < this->mVolume->getEnclosingRegion().getUpperCorner().getX())
#define CAN_GO_NEG_Y(val) (val > this->mVolume->getEnclosingRegion().getLowerCorner().getY())
#define CAN_GO_POS_Y(val) (val < this->mVolume->getEnclosingRegion().getUpperCorner().getY())
#define CAN_GO_NEG_Z(val) (val > this->mVolume->getEnclosingRegion().getLowerCorner().getZ())
#define CAN_GO_POS_Z(val) (val < this->mVolume->getEnclosingRegion().getUpperCorner().getZ())

namespace PolyVox
{
	template <typename VoxelType>
	RawVolume<VoxelType>::Sampler::Sampler(RawVolume<VoxelType>* volume)
		:BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >(volume)
		,mCurrentVoxel(0)
	{
	}

	template <typename VoxelType>
	RawVolume<VoxelType>::Sampler::~Sampler()
	{
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::getVoxel(void) const
	{
		if(this->isCurrentPositionValid())
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
		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::setPosition(xPos, yPos, zPos);

		// Then we update the voxel pointer
		if(this->isCurrentPositionValid())
		{
			const Vector3DInt32& v3dLowerCorner = this->mVolume->m_regValidRegion.getLowerCorner();
			int32_t iLocalXPos = xPos - v3dLowerCorner.getX();
			int32_t iLocalYPos = yPos - v3dLowerCorner.getY();
			int32_t iLocalZPos = zPos - v3dLowerCorner.getZ();

			const int32_t uVoxelIndex = iLocalXPos + 
					iLocalYPos * this->mVolume->getWidth() + 
					iLocalZPos * this->mVolume->getWidth() * this->mVolume->getHeight();

			mCurrentVoxel = this->mVolume->m_pData + uVoxelIndex;
		}
		else
		{
			mCurrentVoxel = 0;
		}
	}

	template <typename VoxelType>
	bool RawVolume<VoxelType>::Sampler::setVoxel(VoxelType tValue)
	{
		//return m_bIsCurrentPositionValid ? *mCurrentVoxel : this->mVolume->getBorderValue();
		if(this->m_bIsCurrentPositionValidInX && this->m_bIsCurrentPositionValidInY && this->m_bIsCurrentPositionValidInZ)
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
		// We'll need this in a moment...
		bool bIsOldPositionValid = this->isCurrentPositionValid();

		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::movePositiveX();

		// Then we update the voxel pointer
		if(this->isCurrentPositionValid() && bIsOldPositionValid )
		{
			++mCurrentVoxel;
		}
		else
		{
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::movePositiveY(void)
	{
		// We'll need this in a moment...
		bool bIsOldPositionValid = this->isCurrentPositionValid();

		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::movePositiveY();

		// Then we update the voxel pointer
		if(this->isCurrentPositionValid() && bIsOldPositionValid )
		{
			mCurrentVoxel += this->mVolume->getWidth();
		}
		else
		{
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::movePositiveZ(void)
	{
		// We'll need this in a moment...
		bool bIsOldPositionValid = this->isCurrentPositionValid();

		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::movePositiveZ();

		// Then we update the voxel pointer
		if(this->isCurrentPositionValid() && bIsOldPositionValid )
		{
			mCurrentVoxel += this->mVolume->getWidth() * this->mVolume->getHeight();
		}
		else
		{
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::moveNegativeX(void)
	{
		// We'll need this in a moment...
		bool bIsOldPositionValid = this->isCurrentPositionValid();

		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::moveNegativeX();

		// Then we update the voxel pointer
		if(this->isCurrentPositionValid() && bIsOldPositionValid )
		{
			--mCurrentVoxel;
		}
		else
		{
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::moveNegativeY(void)
	{
		// We'll need this in a moment...
		bool bIsOldPositionValid = this->isCurrentPositionValid();

		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::moveNegativeY();

		// Then we update the voxel pointer
		if(this->isCurrentPositionValid() && bIsOldPositionValid )
		{
			mCurrentVoxel -= this->mVolume->getWidth();
		}
		else
		{
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::moveNegativeZ(void)
	{
		// We'll need this in a moment...
		bool bIsOldPositionValid = this->isCurrentPositionValid();

		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::moveNegativeZ();

		// Then we update the voxel pointer
		if(this->isCurrentPositionValid() && bIsOldPositionValid )
		{
			mCurrentVoxel -= this->mVolume->getWidth() * this->mVolume->getHeight();
		}
		else
		{
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1ny1nz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume-1,this->mYPosInVolume-1,this->mZPosInVolume-1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1ny0pz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume-1,this->mYPosInVolume-1,this->mZPosInVolume,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1ny1pz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume-1,this->mYPosInVolume-1,this->mZPosInVolume+1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx0py1nz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume-1,this->mYPosInVolume,this->mZPosInVolume-1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx0py0pz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_NEG_X(this->mXPosInVolume) )
		{
			return *(mCurrentVoxel - 1);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume-1,this->mYPosInVolume,this->mZPosInVolume,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx0py1pz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume-1,this->mYPosInVolume,this->mZPosInVolume+1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1py1nz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume-1,this->mYPosInVolume+1,this->mZPosInVolume-1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1py0pz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume-1,this->mYPosInVolume+1,this->mZPosInVolume,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1py1pz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume-1,this->mYPosInVolume+1,this->mZPosInVolume+1,this->m_eWrapMode, this->m_tBorder);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1ny1nz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume,this->mYPosInVolume-1,this->mZPosInVolume-1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1ny0pz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_NEG_Y(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel - this->mVolume->getWidth());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume,this->mYPosInVolume-1,this->mZPosInVolume,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1ny1pz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume,this->mYPosInVolume-1,this->mZPosInVolume+1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px0py1nz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_NEG_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume,this->mYPosInVolume,this->mZPosInVolume-1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px0py0pz(void) const
	{
		if((this->isCurrentPositionValid()))
		{
			return *mCurrentVoxel;
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px0py1pz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_POS_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume,this->mYPosInVolume,this->mZPosInVolume+1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1py1nz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume,this->mYPosInVolume+1,this->mZPosInVolume-1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1py0pz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_POS_Y(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel + this->mVolume->getWidth());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume,this->mYPosInVolume+1,this->mZPosInVolume,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1py1pz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume,this->mYPosInVolume+1,this->mZPosInVolume+1,this->m_eWrapMode, this->m_tBorder);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1ny1nz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume+1,this->mYPosInVolume-1,this->mZPosInVolume-1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1ny0pz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume+1,this->mYPosInVolume-1,this->mZPosInVolume,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1ny1pz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume+1,this->mYPosInVolume-1,this->mZPosInVolume+1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px0py1nz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume+1,this->mYPosInVolume,this->mZPosInVolume-1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px0py0pz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_POS_X(this->mXPosInVolume) )
		{
			return *(mCurrentVoxel + 1);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume+1,this->mYPosInVolume,this->mZPosInVolume,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px0py1pz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume+1,this->mYPosInVolume,this->mZPosInVolume+1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1py1nz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume+1,this->mYPosInVolume+1,this->mZPosInVolume-1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1py0pz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + this->mVolume->getWidth());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume+1,this->mYPosInVolume+1,this->mZPosInVolume,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1py1pz(void) const
	{
		if((this->isCurrentPositionValid()) && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume+1,this->mYPosInVolume+1,this->mZPosInVolume+1,this->m_eWrapMode, this->m_tBorder);
	}
}

#undef CAN_GO_NEG_X
#undef CAN_GO_POS_X
#undef CAN_GO_NEG_Y
#undef CAN_GO_POS_Y
#undef CAN_GO_NEG_Z
#undef CAN_GO_POS_Z
