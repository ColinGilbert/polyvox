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

namespace PolyVox
{
	template <typename VoxelType>
	RawVolume<VoxelType>::Sampler::Sampler(RawVolume<VoxelType>* volume)
		:BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >(volume)
		,mCurrentVoxel(0)
		,m_uValidFlags(0)
	{
	}

	template <typename VoxelType>
	RawVolume<VoxelType>::Sampler::~Sampler()
	{
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::getVoxel(void) const
	{
		if(checkValidFlags(Current))
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

		updateValidFlagsState();
	}

	template <typename VoxelType>
	bool RawVolume<VoxelType>::Sampler::setVoxel(VoxelType tValue)
	{
		if(checkValidFlags(Current))
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

		// Update the valid position flags
		if(checkValidFlags(Current | PositiveX))
		{
			// We've just checked that the old 'Current' and old 'PositiveX' are both valid. That means we are not
			// leaving the volume, and we know we haven't moved along the other two axes. The new 'NegativeX' takes
			// on the value of the old 'Current', and the new 'Current' takes in the value of the old 'PositiveX'.
			// Because we know these are both set we can set both 'NegativeX' and 'Current' to true.
			m_uValidFlags |= (NegativeX | Current);

			// PositiveX is more tricky because it's a new voxel we haven't seen yet. It could be outside the volume,
			// but only in the 'X' direction because that's the way we are moving
			m_uValidFlags[PositiveXShift] = this->mXPosInVolume < this->mVolume->getEnclosingRegion().getUpperX();
		}
		else
		{
			// We're moving from a position which was outside the volume. Note that moving in 'X' can still cause
			// the validity of Y and Z to change as we could start grazing along a face of the volume. It's safest
			// just to compute all the flags fully.
			updateValidFlagsState();
		}
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::movePositiveY(void)
	{
		this->mYPosInVolume++;
		mCurrentVoxel += this->mVolume->getWidth();

		// Update the valid position flags
		if(checkValidFlags(Current | PositiveY))
		{
			// See comments in movePositiveX().
			m_uValidFlags |= (NegativeY | Current);
			m_uValidFlags[PositiveYShift] = this->mYPosInVolume < this->mVolume->getEnclosingRegion().getUpperY();
		}
		else
		{
			// See comments in movePositiveX().
			updateValidFlagsState();
		}
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::movePositiveZ(void)
	{
		this->mZPosInVolume++;
		mCurrentVoxel += this->mVolume->getWidth() * this->mVolume->getHeight();

		// Update the valid position flags
		if(checkValidFlags(Current | PositiveZ))
		{
			// See comments in movePositiveX().
			m_uValidFlags |= (NegativeZ | Current);
			m_uValidFlags[PositiveZShift] = this->mZPosInVolume < this->mVolume->getEnclosingRegion().getUpperZ();
		}
		else
		{
			// See comments in movePositiveX().
			updateValidFlagsState();
		}
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::moveNegativeX(void)
	{
		this->mXPosInVolume--;
		--mCurrentVoxel;

		// Update the valid position flags
		if(checkValidFlags(Current | NegativeX))
		{
			// See comments in movePositiveX().
			m_uValidFlags |= (PositiveX | Current);
			m_uValidFlags[NegativeXShift] = this->mXPosInVolume > this->mVolume->getEnclosingRegion().getLowerX();
		}
		else
		{
			// See comments in movePositiveX().
			updateValidFlagsState();
		}
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::moveNegativeY(void)
	{
		this->mYPosInVolume--;
		mCurrentVoxel -= this->mVolume->getWidth();

		// Update the valid position flags
		if(checkValidFlags(Current | NegativeY))
		{
			// See comments in movePositiveX().
			m_uValidFlags |= (PositiveY | Current);
			m_uValidFlags[NegativeYShift] = this->mYPosInVolume > this->mVolume->getEnclosingRegion().getLowerY();
		}
		else
		{
			// See comments in movePositiveX().
			updateValidFlagsState();
		}
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::moveNegativeZ(void)
	{
		this->mZPosInVolume--;
		mCurrentVoxel -= this->mVolume->getWidth() * this->mVolume->getHeight();

		// Update the valid position flags
		if(checkValidFlags(Current | NegativeZ))
		{
			// See comments in movePositiveX().
			m_uValidFlags |= (PositiveZ | Current);
			m_uValidFlags[NegativeZShift] = this->mZPosInVolume > this->mVolume->getEnclosingRegion().getLowerZ();
		}
		else
		{
			// See comments in movePositiveX().
			updateValidFlagsState();
		}
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1ny1nz(void) const
	{
		if(checkValidFlags(Current | NegativeX | NegativeY | NegativeZ))
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume-1,this->mYPosInVolume-1,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1ny0pz(void) const
	{
		if(checkValidFlags(Current | NegativeX | NegativeY))
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth());
		}
		return this->getVoxelAt(this->mXPosInVolume-1,this->mYPosInVolume-1,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1ny1pz(void) const
	{
		if(checkValidFlags(Current | NegativeX | NegativeY | PositiveZ))
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume-1,this->mYPosInVolume-1,this->mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx0py1nz(void) const
	{
		if(checkValidFlags(Current | NegativeX | NegativeZ))
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume-1,this->mYPosInVolume,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx0py0pz(void) const
	{
		if(checkValidFlags(Current | NegativeX))
		{
			return *(mCurrentVoxel - 1);
		}
		return this->getVoxelAt(this->mXPosInVolume-1,this->mYPosInVolume,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx0py1pz(void) const
	{
		if(checkValidFlags(Current | NegativeX | PositiveZ))
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume-1,this->mYPosInVolume,this->mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1py1nz(void) const
	{
		if(checkValidFlags(Current | NegativeX | PositiveY | NegativeZ))
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume-1,this->mYPosInVolume+1,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1py0pz(void) const
	{
		if(checkValidFlags(Current | NegativeX | PositiveY))
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth());
		}
		return this->getVoxelAt(this->mXPosInVolume-1,this->mYPosInVolume+1,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1py1pz(void) const
	{
		if(checkValidFlags(Current | NegativeX | PositiveY | PositiveZ))
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume-1,this->mYPosInVolume+1,this->mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1ny1nz(void) const
	{
		if(checkValidFlags(Current | NegativeY | NegativeZ))
		{
			return *(mCurrentVoxel - this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume,this->mYPosInVolume-1,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1ny0pz(void) const
	{
		if(checkValidFlags(Current | NegativeY))
		{
			return *(mCurrentVoxel - this->mVolume->getWidth());
		}
		return this->getVoxelAt(this->mXPosInVolume,this->mYPosInVolume-1,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1ny1pz(void) const
	{
		if(checkValidFlags(Current | NegativeY | PositiveZ))
		{
			return *(mCurrentVoxel - this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume,this->mYPosInVolume-1,this->mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px0py1nz(void) const
	{
		if(checkValidFlags(Current | NegativeZ))
		{
			return *(mCurrentVoxel - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume,this->mYPosInVolume,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px0py0pz(void) const
	{
		if(checkValidFlags(Current))
		{
			return *mCurrentVoxel;
		}
		return this->getVoxelAt(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px0py1pz(void) const
	{
		if(checkValidFlags(Current | PositiveZ))
		{
			return *(mCurrentVoxel + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume,this->mYPosInVolume,this->mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1py1nz(void) const
	{
		if(checkValidFlags(Current | PositiveY | NegativeZ))
		{
			return *(mCurrentVoxel + this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume,this->mYPosInVolume+1,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1py0pz(void) const
	{
		if(checkValidFlags(Current | PositiveY))
		{
			return *(mCurrentVoxel + this->mVolume->getWidth());
		}
		return this->getVoxelAt(this->mXPosInVolume,this->mYPosInVolume+1,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1py1pz(void) const
	{
		if(checkValidFlags(Current | PositiveY | PositiveZ))
		{
			return *(mCurrentVoxel + this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume,this->mYPosInVolume+1,this->mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1ny1nz(void) const
	{
		if(checkValidFlags(Current | PositiveX | NegativeY | NegativeZ))
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume+1,this->mYPosInVolume-1,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1ny0pz(void) const
	{
		if(checkValidFlags(Current | PositiveX | NegativeY))
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth());
		}
		return this->getVoxelAt(this->mXPosInVolume+1,this->mYPosInVolume-1,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1ny1pz(void) const
	{
		if(checkValidFlags(Current | PositiveX | NegativeY | PositiveZ))
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume+1,this->mYPosInVolume-1,this->mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px0py1nz(void) const
	{
		if(checkValidFlags(Current | PositiveX | NegativeZ))
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume+1,this->mYPosInVolume,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px0py0pz(void) const
	{
		if(checkValidFlags(Current | PositiveX))
		{
			return *(mCurrentVoxel + 1);
		}
		return this->getVoxelAt(this->mXPosInVolume+1,this->mYPosInVolume,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px0py1pz(void) const
	{
		if(checkValidFlags(Current | PositiveX | PositiveZ))
		{
			return *(mCurrentVoxel + 1 + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume+1,this->mYPosInVolume,this->mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1py1nz(void) const
	{
		if(checkValidFlags(Current | PositiveX | PositiveY | NegativeZ))
		{
			return *(mCurrentVoxel + 1 + this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->getVoxelAt(this->mXPosInVolume+1,this->mYPosInVolume+1,this->mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1py0pz(void) const
	{
		if(checkValidFlags(Current | PositiveX | PositiveY))
		{
			return *(mCurrentVoxel + 1 + this->mVolume->getWidth());
		}
		return this->getVoxelAt(this->mXPosInVolume+1,this->mYPosInVolume+1,this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1py1pz(void) const
	{
		if(checkValidFlags(Current | PositiveX | PositiveY | PositiveZ))
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
	inline bool RawVolume<VoxelType>::Sampler::checkValidFlags(std::bitset<7> uFlagsToCheck) const
	{
		return (m_uValidFlags & uFlagsToCheck) == uFlagsToCheck;
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::updateValidFlagsState(void)
	{
		int32_t xPos = this->mXPosInVolume;
		int32_t yPos = this->mYPosInVolume;
		int32_t zPos = this->mZPosInVolume;

		if(this->mVolume->getEnclosingRegion().containsPoint(xPos, yPos, zPos, 1))
		{
			// This is the most common case, where the position being set is well within the volume. We can just set all the flags to true.
			m_uValidFlags = Current | PositiveX | NegativeX | PositiveY | NegativeY | PositiveZ | NegativeZ;
		}
		else
		{	
			// We're not well inside the volume, so we could be on the edge of we could be outside. Determine which it is.
			m_uValidFlags[CurrentShift] = this->mVolume->getEnclosingRegion().containsPoint(xPos, yPos, zPos, 0);

			if(m_uValidFlags[Current])
			{
				// If we're on the edge then we can simplify the logic by only checking one bound as we know current position is valid.
				m_uValidFlags[PositiveXShift] = xPos < this->mVolume->getEnclosingRegion().getUpperX();
				m_uValidFlags[PositiveYShift] = yPos < this->mVolume->getEnclosingRegion().getUpperY();
				m_uValidFlags[PositiveZShift] = zPos < this->mVolume->getEnclosingRegion().getUpperZ();
				m_uValidFlags[NegativeXShift] = xPos > this->mVolume->getEnclosingRegion().getLowerX();
				m_uValidFlags[NegativeYShift] = yPos > this->mVolume->getEnclosingRegion().getLowerY();
				m_uValidFlags[NegativeZShift] = zPos > this->mVolume->getEnclosingRegion().getLowerZ();
			}
			else
			{
				// We're outside the volume... hard to optimise for this (uncommon) case so do the full calculations for each position.
				m_uValidFlags[PositiveXShift] = this->mVolume->getEnclosingRegion().containsPoint(xPos+1, yPos, zPos, 0);
				m_uValidFlags[PositiveYShift] = this->mVolume->getEnclosingRegion().containsPoint(xPos, yPos+1, zPos, 0);
				m_uValidFlags[PositiveZShift] = this->mVolume->getEnclosingRegion().containsPoint(xPos, yPos, zPos+1, 0);
				m_uValidFlags[NegativeXShift] = this->mVolume->getEnclosingRegion().containsPoint(xPos-1, yPos, zPos, 0);				
				m_uValidFlags[NegativeYShift] = this->mVolume->getEnclosingRegion().containsPoint(xPos, yPos-1, zPos, 0);				
				m_uValidFlags[NegativeZShift] = this->mVolume->getEnclosingRegion().containsPoint(xPos, yPos, zPos-1, 0);
			}
		}
	}
}