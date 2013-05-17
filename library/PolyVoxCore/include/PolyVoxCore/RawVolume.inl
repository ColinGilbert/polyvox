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

namespace PolyVox
{
	////////////////////////////////////////////////////////////////////////////////
	/// This constructor creates a volume with a fixed size which is specified as a parameter.
	/// \param regValid Specifies the minimum and maximum valid voxel positions.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	RawVolume<VoxelType>::RawVolume(const Region& regValid)
		:BaseVolume<VoxelType>(regValid)
	{
		this->setBorderValue(VoxelType());

		//Create a volume of the right size.
		initialise(regValid);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should never be called. Copying volumes by value would be expensive, and we want to prevent users from doing
	/// it by accident (such as when passing them as paramenters to functions). That said, there are times when you really do want to
	/// make a copy of a volume and in this case you should look at the Volumeresampler.
	///
	/// \sa VolumeResampler
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	RawVolume<VoxelType>::RawVolume(const RawVolume<VoxelType>& /*rhs*/)
	{
		POLYVOX_THROW(not_implemented, "Volume copy constructor not implemented for performance reasons.");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Destroys the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	RawVolume<VoxelType>::~RawVolume()
	{
		delete[] m_pData;
		m_pData = 0;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should never be called. Copying volumes by value would be expensive, and we want to prevent users from doing
	/// it by accident (such as when passing them as paramenters to functions). That said, there are times when you really do want to
	/// make a copy of a volume and in this case you should look at the Volumeresampler.
	///
	/// \sa VolumeResampler
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	RawVolume<VoxelType>& RawVolume<VoxelType>::operator=(const RawVolume<VoxelType>& /*rhs*/)
	{
		POLYVOX_THROW(not_implemented, "Volume assignment operator not implemented for performance reasons.");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos The \c x position of the voxel
	/// \param uYPos The \c y position of the voxel
	/// \param uZPos The \c z position of the voxel
	/// \param eBoundsCheck Controls whether bounds checking is performed on voxel access. It's safest to
	/// set this to BoundsChecks::Full (the default), but if you are certain that the voxel you are accessing
	/// is inside the volume's enclosing region then you can skip this check to gain some performance.
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::getVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, BoundsCheck eBoundsCheck) const
	{
		// If bounds checking is enabled then we validate the
		// bounds, and throw an exception if they are violated.
		if(eBoundsCheck == BoundsChecks::Full)
		{
			if(this->m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)) == false)
			{
				POLYVOX_THROW(std::out_of_range, "Position is outside valid region");
			}
		}

		const Vector3DInt32& v3dLowerCorner = this->m_regValidRegion.getLowerCorner();
		int32_t iLocalXPos = uXPos - v3dLowerCorner.getX();
		int32_t iLocalYPos = uYPos - v3dLowerCorner.getY();
		int32_t iLocalZPos = uZPos - v3dLowerCorner.getZ();

		return m_pData
		[
			iLocalXPos + 
			iLocalYPos * this->getWidth() + 
			iLocalZPos * this->getWidth() * this->getHeight()
		];
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos The 3D position of the voxel
	/// \return The voxel value
	/// \param eBoundsCheck Controls whether bounds checking is performed on voxel access. It's safest to
	/// set this to BoundsChecks::Full (the default), but if you are certain that the voxel you are accessing
	/// is inside the volume's enclosing region then you can skip this check to gain some performance.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::getVoxel(const Vector3DInt32& v3dPos, BoundsCheck eBoundsCheck) const
	{
		return getVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), eBoundsCheck);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos The \c x position of the voxel
	/// \param uYPos The \c y position of the voxel
	/// \param uZPos The \c z position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::getVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos) const
	{
		if(this->m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)))
		{
			const Vector3DInt32& v3dLowerCorner = this->m_regValidRegion.getLowerCorner();
			int32_t iLocalXPos = uXPos - v3dLowerCorner.getX();
			int32_t iLocalYPos = uYPos - v3dLowerCorner.getY();
			int32_t iLocalZPos = uZPos - v3dLowerCorner.getZ();

			return m_pData
			[
				iLocalXPos + 
				iLocalYPos * this->getWidth() + 
				iLocalZPos * this->getWidth() * this->getHeight()
			];
		}
		else
		{
			return this->getBorderValue();
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos The 3D position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::getVoxelAt(const Vector3DInt32& v3dPos) const
	{
		return getVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos The \c x position of the voxel
	/// \param uYPos The \c y position of the voxel
	/// \param uZPos The \c z position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::getVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapMode eWrapMode, VoxelType tBorder) const
	{
		switch(eWrapMode)
		{
			case WrapModes::Clamp:
			{
				//Perform clamping
				uXPos = (std::max)(uXPos, this->m_regValidRegion.getLowerX());
				uYPos = (std::max)(uYPos, this->m_regValidRegion.getLowerY());
				uZPos = (std::max)(uZPos, this->m_regValidRegion.getLowerZ());
				uXPos = (std::min)(uXPos, this->m_regValidRegion.getUpperX());
				uYPos = (std::min)(uYPos, this->m_regValidRegion.getUpperY());
				uZPos = (std::min)(uZPos, this->m_regValidRegion.getUpperZ());

				//Get the voxel value
				return getVoxel(uXPos, uYPos, uZPos);
				//No need to break as we've returned
			}
			case WrapModes::Border:
			{
				if(this->m_regValidRegion.containsPoint(uXPos, uYPos, uZPos))
				{
					return getVoxel(uXPos, uYPos, uZPos);
				}
				else
				{
					return tBorder;
				}
				//No need to break as we've returned
			}
			default:
			{
				//Should never happen
				POLYVOX_THROW(std::invalid_argument, "Wrap mode parameter has an unrecognised value.");
				return VoxelType();
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos The 3D position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::getVoxel(const Vector3DInt32& v3dPos, WrapMode eWrapMode, VoxelType tBorder) const
	{
		return getVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), eWrapMode, tBorder);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos the \c x position of the voxel
	/// \param uYPos the \c y position of the voxel
	/// \param uZPos the \c z position of the voxel
	/// \param tValue the value to which the voxel will be set
	/// \param eBoundsCheck Controls whether bounds checking is performed on voxel access. It's safest to
	/// set this to BoundsChecks::Full (the default), but if you are certain that the voxel you are accessing
	/// is inside the volume's enclosing region then you can skip this check to gain some performance.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void RawVolume<VoxelType>::setVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue, BoundsCheck eBoundsCheck)
	{
		// If bounds checking is enabled then we validate the
		// bounds, and throw an exception if they are violated.
		if(eBoundsCheck == BoundsChecks::Full)
		{
			if(this->m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)) == false)
			{
				POLYVOX_THROW(std::out_of_range, "Position is outside valid region");
			}
		}

		const Vector3DInt32& v3dLowerCorner = this->m_regValidRegion.getLowerCorner();
		int32_t iLocalXPos = uXPos - v3dLowerCorner.getX();
		int32_t iLocalYPos = uYPos - v3dLowerCorner.getY();
		int32_t iLocalZPos = uZPos - v3dLowerCorner.getZ();

		m_pData
		[
			iLocalXPos + 
			iLocalYPos * this->getWidth() + 
			iLocalZPos * this->getWidth() * this->getHeight()
		] = tValue;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos the 3D position of the voxel
	/// \param tValue the value to which the voxel will be set
	/// \param eBoundsCheck Controls whether bounds checking is performed on voxel access. It's safest to
	/// set this to BoundsChecks::Full (the default), but if you are certain that the voxel you are accessing
	/// is inside the volume's enclosing region then you can skip this check to gain some performance.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void RawVolume<VoxelType>::setVoxel(const Vector3DInt32& v3dPos, VoxelType tValue, BoundsCheck eBoundsCheck)
	{
		setVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue, eBoundsCheck);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos the \c x position of the voxel
	/// \param uYPos the \c y position of the voxel
	/// \param uZPos the \c z position of the voxel
	/// \param tValue the value to which the voxel will be set
	/// \return whether the requested position is inside the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	bool RawVolume<VoxelType>::setVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue)
	{
		if(this->m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)))
		{
			const Vector3DInt32& v3dLowerCorner = this->m_regValidRegion.getLowerCorner();
			int32_t iLocalXPos = uXPos - v3dLowerCorner.getX();
			int32_t iLocalYPos = uYPos - v3dLowerCorner.getY();
			int32_t iLocalZPos = uZPos - v3dLowerCorner.getZ();

			m_pData
			[
				iLocalXPos + 
				iLocalYPos * this->getWidth() + 
				iLocalZPos * this->getWidth() * this->getHeight()
			] = tValue;

			//Return true to indicate that we modified a voxel.
			return true;
		}
		else
		{
			return false;
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos the 3D position of the voxel
	/// \param tValue the value to which the voxel will be set
	/// \return whether the requested position is inside the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	bool RawVolume<VoxelType>::setVoxelAt(const Vector3DInt32& v3dPos, VoxelType tValue)
	{
		return setVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should probably be made internal...
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void RawVolume<VoxelType>::initialise(const Region& regValidRegion)
	{
		this->m_regValidRegion = regValidRegion;

		if(this->getWidth() <= 0)
		{
			POLYVOX_THROW(std::invalid_argument, "Volume width must be greater than zero.");
		}
		if(this->getHeight() <= 0)
		{
			POLYVOX_THROW(std::invalid_argument, "Volume height must be greater than zero.");
		}
		if(this->getDepth() <= 0)
		{
			POLYVOX_THROW(std::invalid_argument, "Volume depth must be greater than zero.");
		}

		//Create the data
		m_pData = new VoxelType[this->getWidth() * this->getHeight()* this->getDepth()];

		//Other properties we might find useful later
		this->m_uLongestSideLength = (std::max)((std::max)(this->getWidth(),this->getHeight()),this->getDepth());
		this->m_uShortestSideLength = (std::min)((std::min)(this->getWidth(),this->getHeight()),this->getDepth());
		this->m_fDiagonalLength = sqrtf(static_cast<float>(this->getWidth() * this->getWidth() + this->getHeight() * this->getHeight() + this->getDepth() * this->getDepth()));
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Note: This function needs reviewing for accuracy...
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	uint32_t RawVolume<VoxelType>::calculateSizeInBytes(void)
	{
		return this->getWidth() * this->getHeight() * this->getDepth() * sizeof(VoxelType);
	}

}

