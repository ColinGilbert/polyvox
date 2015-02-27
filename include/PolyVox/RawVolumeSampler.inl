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

#define CAN_GO_NEG_X(val) (val > this->mVolume->getEnclosingRegion().getLowerX())
#define CAN_GO_POS_X(val) (val < this->mVolume->getEnclosingRegion().getUpperX())
#define CAN_GO_NEG_Y(val) (val > this->mVolume->getEnclosingRegion().getLowerY())
#define CAN_GO_POS_Y(val) (val < this->mVolume->getEnclosingRegion().getUpperY())
#define CAN_GO_NEG_Z(val) (val > this->mVolume->getEnclosingRegion().getLowerZ())
#define CAN_GO_POS_Z(val) (val < this->mVolume->getEnclosingRegion().getUpperZ())

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
		return *mCurrentVoxel;
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

		const Vector3DInt32& v3dLowerCorner = this->mVolume->m_regValidRegion.getLowerCorner();
		int32_t iLocalXPos = xPos - v3dLowerCorner.getX();
		int32_t iLocalYPos = yPos - v3dLowerCorner.getY();
		int32_t iLocalZPos = zPos - v3dLowerCorner.getZ();

		const int32_t uVoxelIndex = iLocalXPos + 
				iLocalYPos * this->mVolume->getWidth() + 
				iLocalZPos * this->mVolume->getWidth() * this->mVolume->getHeight();

		mCurrentVoxel = this->mVolume->m_pData + uVoxelIndex;
	}

	template <typename VoxelType>
	bool RawVolume<VoxelType>::Sampler::setVoxel(VoxelType tValue)
	{
		*mCurrentVoxel = tValue;
		return true;
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::movePositiveX(void)
	{
		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::movePositiveX();

		// Then we update the voxel pointer
		++mCurrentVoxel;
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::movePositiveY(void)
	{
		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::movePositiveY();

		// Then we update the voxel pointer
		mCurrentVoxel += this->mVolume->getWidth();
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::movePositiveZ(void)
	{
		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::movePositiveZ();

		// Then we update the voxel pointer
		mCurrentVoxel += this->mVolume->getWidth() * this->mVolume->getHeight();
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::moveNegativeX(void)
	{
		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::moveNegativeX();

		// Then we update the voxel pointer
		--mCurrentVoxel;
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::moveNegativeY(void)
	{
		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::moveNegativeY();

		// Then we update the voxel pointer
		mCurrentVoxel -= this->mVolume->getWidth();
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::moveNegativeZ(void)
	{
		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::moveNegativeZ();

		// Then we update the voxel pointer
		mCurrentVoxel -= this->mVolume->getWidth() * this->mVolume->getHeight();
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1ny1nz(void) const
	{
		if( CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume-1,this->mYPosInVolume-1,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1ny0pz(void) const
	{
		if( CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume-1,this->mYPosInVolume-1,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1ny1pz(void) const
	{
		if( CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume-1,this->mYPosInVolume-1,this->mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx0py1nz(void) const
	{
		if( CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume-1,this->mYPosInVolume,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx0py0pz(void) const
	{
		if( CAN_GO_NEG_X(this->mXPosInVolume) )
		{
			return *(mCurrentVoxel - 1);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume-1,this->mYPosInVolume,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx0py1pz(void) const
	{
		if( CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume-1,this->mYPosInVolume,this->mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1py1nz(void) const
	{
		if( CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume-1,this->mYPosInVolume+1,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1py0pz(void) const
	{
		if( CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume-1,this->mYPosInVolume+1,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1py1pz(void) const
	{
		if( CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume-1,this->mYPosInVolume+1,this->mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1ny1nz(void) const
	{
		if( CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume,this->mYPosInVolume-1,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1ny0pz(void) const
	{
		if( CAN_GO_NEG_Y(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel - this->mVolume->getWidth());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume,this->mYPosInVolume-1,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1ny1pz(void) const
	{
		if( CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume,this->mYPosInVolume-1,this->mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px0py1nz(void) const
	{
		if( CAN_GO_NEG_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume,this->mYPosInVolume,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px0py0pz(void) const
	{
		return *mCurrentVoxel;
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px0py1pz(void) const
	{
		if( CAN_GO_POS_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume,this->mYPosInVolume,this->mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1py1nz(void) const
	{
		if( CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume,this->mYPosInVolume+1,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1py0pz(void) const
	{
		if( CAN_GO_POS_Y(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel + this->mVolume->getWidth());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume,this->mYPosInVolume+1,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1py1pz(void) const
	{
		if( CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume,this->mYPosInVolume+1,this->mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1ny1nz(void) const
	{
		if( CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume+1,this->mYPosInVolume-1,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1ny0pz(void) const
	{
		if( CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume+1,this->mYPosInVolume-1,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1ny1pz(void) const
	{
		if( CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume+1,this->mYPosInVolume-1,this->mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px0py1nz(void) const
	{
		if( CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume+1,this->mYPosInVolume,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px0py0pz(void) const
	{
		if( CAN_GO_POS_X(this->mXPosInVolume) )
		{
			return *(mCurrentVoxel + 1);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume+1,this->mYPosInVolume,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px0py1pz(void) const
	{
		if( CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume+1,this->mYPosInVolume,this->mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1py1nz(void) const
	{
		if( CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume+1,this->mYPosInVolume+1,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1py0pz(void) const
	{
		if( CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + this->mVolume->getWidth());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume+1,this->mYPosInVolume+1,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1py1pz(void) const
	{
		if( CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume+1,this->mYPosInVolume+1,this->mZPosInVolume+1);
	}
}

#undef CAN_GO_NEG_X
#undef CAN_GO_POS_X
#undef CAN_GO_NEG_Y
#undef CAN_GO_POS_Y
#undef CAN_GO_NEG_Z
#undef CAN_GO_POS_Z
