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
	/// This is protected because you should never create a BaseVolume directly, you should instead use one of the derived classes.
	///
	/// \sa RawVolume, SimpleVolume, LargeVolume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	BaseVolume<VoxelType>::BaseVolume(const Region& regValid)
		:m_regValidRegion(regValid)
		,m_tBorderValue()
	{
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should never be called. Copying volumes by value would be expensive, and we want to prevent users from doing
	/// it by accident (such as when passing them as paramenters to functions). That said, there are times when you really do want to
	/// make a copy of a volume and in this case you should look at the VolumeResampler.
	///
	/// \sa VolumeResampler
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	BaseVolume<VoxelType>::BaseVolume(const BaseVolume<VoxelType>& /*rhs*/)
	{
		POLYVOX_THROW(not_implemented, "Volume copy constructor not implemented for performance reasons.");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Destroys the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	BaseVolume<VoxelType>::~BaseVolume()
	{
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should never be called. Copying volumes by value would be expensive, and we want to prevent users from doing
	/// it by accident (such as when passing them as paramenters to functions). That said, there are times when you really do want to
	/// make a copy of a volume and in this case you should look at the VolumeResampler.
	///
	/// \sa VolumeResampler
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	BaseVolume<VoxelType>& BaseVolume<VoxelType>::operator=(const BaseVolume<VoxelType>& /*rhs*/)
	{
		POLYVOX_THROW(not_implemented, "Volume assignment operator not implemented for performance reasons.");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// The border value is returned whenever an attempt is made to read a voxel which
	/// is outside the extents of the volume.
	/// \return The value used for voxels outside of the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType BaseVolume<VoxelType>::getBorderValue(void) const
	{
		return m_tBorderValue;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return A Region representing the extent of the volume.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	const Region& BaseVolume<VoxelType>::getEnclosingRegion(void) const
	{
		return m_regValidRegion;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The width of the volume in voxels. Note that this value is inclusive, so that if the valid range is e.g. 0 to 63 then the width is 64.
	/// \sa getHeight(), getDepth()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	int32_t BaseVolume<VoxelType>::getWidth(void) const
	{
		return m_regValidRegion.getUpperX() - m_regValidRegion.getLowerX() + 1;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The height of the volume in voxels. Note that this value is inclusive, so that if the valid range is e.g. 0 to 63 then the height is 64.
	/// \sa getWidth(), getDepth()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	int32_t BaseVolume<VoxelType>::getHeight(void) const
	{
		return m_regValidRegion.getUpperY() - m_regValidRegion.getLowerY() + 1;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The depth of the volume in voxels. Note that this value is inclusive, so that if the valid range is e.g. 0 to 63 then the depth is 64.
	/// \sa getWidth(), getHeight()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	int32_t BaseVolume<VoxelType>::getDepth(void) const
	{
		return m_regValidRegion.getUpperZ() - m_regValidRegion.getLowerZ() + 1;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The length of the shortest side in voxels. For example, if a volume has
	/// dimensions 256x512x1024 this function will return 256.
	/// \sa getLongestSideLength(), getDiagonalLength()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	int32_t BaseVolume<VoxelType>::getShortestSideLength(void) const
	{
		return m_uShortestSideLength;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The length of the longest side in voxels. For example, if a volume has
	/// dimensions 256x512x1024 this function will return 1024.
	/// \sa getShortestSideLength(), getDiagonalLength()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	int32_t BaseVolume<VoxelType>::getLongestSideLength(void) const
	{
		return m_uLongestSideLength;
	}	

	////////////////////////////////////////////////////////////////////////////////
	/// \return The length of the diagonal in voxels. For example, if a volume has
	/// dimensions 256x512x1024 this function will return sqrt(256*256+512*512+1024*1024)
	/// = 1173.139. This value is computed on volume creation so retrieving it is fast.
	/// \sa getShortestSideLength(), getLongestSideLength()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	float BaseVolume<VoxelType>::getDiagonalLength(void) const
	{
		return m_fDiagonalLength;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This version of the function requires the wrap mode to be specified as a
	/// template parameter, which can provide better performance.
	/// \param uXPos The \c x position of the voxel
	/// \param uYPos The \c y position of the voxel
	/// \param uZPos The \c z position of the voxel
	/// \tparam eWrapMode Specifies the behaviour when the requested position is outside of the volume.
	/// \param tBorder The border value to use if the wrap mode is set to 'Border'.
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	template <WrapMode eWrapMode>
	VoxelType BaseVolume<VoxelType>::getVoxel(int32_t /*uXPos*/, int32_t /*uYPos*/, int32_t /*uZPos*/, VoxelType /*tBorder*/) const
	{
		POLYVOX_ASSERT(false, "You should never call the base class version of this function.");
		return VoxelType();
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This version of the function requires the wrap mode to be specified as a
	/// template parameter, which can provide better performance.
	/// \param uXPos The \c x position of the voxel
	/// \param uYPos The \c y position of the voxel
	/// \param uZPos The \c z position of the voxel
	/// \tparam eWrapMode Specifies the behaviour when the requested position is outside of the volume.
	/// \param tBorder The border value to use if the wrap mode is set to 'Border'.
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	template <WrapMode eWrapMode>
	VoxelType BaseVolume<VoxelType>::getVoxel(const Vector3DInt32& /*v3dPos*/, VoxelType /*tBorder*/) const
	{
		POLYVOX_ASSERT(false, "You should never call the base class version of this function.");
		return VoxelType();
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This version of the function is provided so that the wrap mode does not need
	/// to be specified as a template parameter, as it may be confusing to some users.
	/// \param uXPos The \c x position of the voxel
	/// \param uYPos The \c y position of the voxel
	/// \param uZPos The \c z position of the voxel
	/// \param eWrapMode Specifies the behaviour when the requested position is outside of the volume.
	/// \param tBorder The border value to use if the wrap mode is set to 'Border'.
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType BaseVolume<VoxelType>::getVoxel(int32_t /*uXPos*/, int32_t /*uYPos*/, int32_t /*uZPos*/, WrapMode /*eWrapMode*/, VoxelType /*tBorder*/) const
	{
		POLYVOX_ASSERT(false, "You should never call the base class version of this function.");
		return VoxelType();
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This version of the function is provided so that the wrap mode does not need
	/// to be specified as a template parameter, as it may be confusing to some users.
	/// \param v3dPos The 3D position of the voxel
	/// \param eWrapMode Specifies the behaviour when the requested position is outside of the volume.
	/// \param tBorder The border value to use if the wrap mode is set to 'Border'.
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType BaseVolume<VoxelType>::getVoxel(const Vector3DInt32& /*v3dPos*/, WrapMode /*eWrapMode*/, VoxelType /*tBorder*/) const
	{
		POLYVOX_ASSERT(false, "You should never call the base class version of this function.");
		return VoxelType();
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos The \c x position of the voxel
	/// \param uYPos The \c y position of the voxel
	/// \param uZPos The \c z position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType BaseVolume<VoxelType>::getVoxelAt(int32_t /*uXPos*/, int32_t /*uYPos*/, int32_t /*uZPos*/) const
	{
		POLYVOX_THROW(not_implemented, "You should never call the base class version of this function.");
		return VoxelType();
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos The 3D position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType BaseVolume<VoxelType>::getVoxelAt(const Vector3DInt32& /*v3dPos*/) const
	{
		POLYVOX_THROW(not_implemented, "You should never call the base class version of this function.");
		return VoxelType();
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param tBorder The value to use for voxels outside the volume.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void BaseVolume<VoxelType>::setBorderValue(const VoxelType& tBorder) 
	{
		m_tBorderValue = tBorder;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos the \c x position of the voxel
	/// \param uYPos the \c y position of the voxel
	/// \param uZPos the \c z position of the voxel
	/// \param tValue the value to which the voxel will be set
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void BaseVolume<VoxelType>::setVoxel(int32_t /*uXPos*/, int32_t /*uYPos*/, int32_t /*uZPos*/, VoxelType /*tValue*/, WrapMode /*eWrapMode*/)
	{
		POLYVOX_THROW(not_implemented, "You should never call the base class version of this function.");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos the 3D position of the voxel
	/// \param tValue the value to which the voxel will be set
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void BaseVolume<VoxelType>::setVoxel(const Vector3DInt32& /*v3dPos*/, VoxelType /*tValue*/, WrapMode /*eWrapMode*/)
	{
		POLYVOX_THROW(not_implemented, "You should never call the base class version of this function.");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos the \c x position of the voxel
	/// \param uYPos the \c y position of the voxel
	/// \param uZPos the \c z position of the voxel
	/// \param tValue the value to which the voxel will be set
	/// \return whether the requested position is inside the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	bool BaseVolume<VoxelType>::setVoxelAt(int32_t /*uXPos*/, int32_t /*uYPos*/, int32_t /*uZPos*/, VoxelType /*tValue*/)
	{
		POLYVOX_THROW(not_implemented, "You should never call the base class version of this function.");
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos the 3D position of the voxel
	/// \param tValue the value to which the voxel will be set
	/// \return whether the requested position is inside the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	bool BaseVolume<VoxelType>::setVoxelAt(const Vector3DInt32& /*v3dPos*/, VoxelType /*tValue*/)
	{
		POLYVOX_THROW(not_implemented, "You should never call the base class version of this function.");
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Note: This function needs reviewing for accuracy...
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	uint32_t BaseVolume<VoxelType>::calculateSizeInBytes(void)
	{
		return getWidth() * getHeight() * getDepth() * sizeof(VoxelType);
	}	
}

