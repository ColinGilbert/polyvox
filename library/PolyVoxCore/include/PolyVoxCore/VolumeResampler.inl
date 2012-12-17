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

#include "PolyVoxCore/Interpolation.h"

#include <cmath>

namespace PolyVox
{
	/**
	 * \param pVolSrc
	 * \param regSrc
	 * \param[out] pVolDst
	 * \param regDst
	 */
	template< typename SrcVolumeType, typename DstVolumeType>
	VolumeResampler<SrcVolumeType, DstVolumeType>::VolumeResampler(SrcVolumeType* pVolSrc, const Region &regSrc, DstVolumeType* pVolDst, const Region& regDst)
		:m_pVolSrc(pVolSrc)
		,m_regSrc(regSrc)
		,m_pVolDst(pVolDst)
		,m_regDst(regDst)
	{
	}

	template< typename SrcVolumeType, typename DstVolumeType>
	void VolumeResampler<SrcVolumeType, DstVolumeType>::execute()
	{
		int32_t uSrcWidth = m_regSrc.getUpperCorner().getX() - m_regSrc.getLowerCorner().getX() + 1;
		int32_t uSrcHeight = m_regSrc.getUpperCorner().getY() - m_regSrc.getLowerCorner().getY() + 1;
		int32_t uSrcDepth = m_regSrc.getUpperCorner().getZ() - m_regSrc.getLowerCorner().getZ() + 1;

		int32_t uDstWidth = m_regDst.getUpperCorner().getX() - m_regDst.getLowerCorner().getX() + 1;
		int32_t uDstHeight = m_regDst.getUpperCorner().getY() - m_regDst.getLowerCorner().getY() + 1;
		int32_t uDstDepth = m_regDst.getUpperCorner().getZ() - m_regDst.getLowerCorner().getZ() + 1;

		/*if((uSrcWidth == uDstWidth) && (uSrcHeight == uDstHeight) && (uSrcDepth == uDstDepth))
		{
			resampleSameSize();
		}
		else
		{
			resampleArbitrary();
		}*/

		resampleBetter();
	}

	template< typename SrcVolumeType, typename DstVolumeType>
	void VolumeResampler<SrcVolumeType, DstVolumeType>::resampleSameSize()
	{
		for(int32_t sz = m_regSrc.getLowerCorner().getZ(), dz = m_regDst.getLowerCorner().getZ(); dz <= m_regDst.getUpperCorner().getZ(); sz++, dz++)
		{
			for(int32_t sy = m_regSrc.getLowerCorner().getY(), dy = m_regDst.getLowerCorner().getY(); dy <= m_regDst.getUpperCorner().getY(); sy++, dy++)
			{
				for(int32_t sx = m_regSrc.getLowerCorner().getX(), dx = m_regDst.getLowerCorner().getX(); dx <= m_regDst.getUpperCorner().getX(); sx++,dx++)
				{
					const typename SrcVolumeType::VoxelType& tSrcVoxel = m_pVolSrc->getVoxelAt(sx,sy,sz);
					const typename DstVolumeType::VoxelType& tDstVoxel = static_cast<typename DstVolumeType::VoxelType>(tSrcVoxel);
					m_pVolDst->setVoxelAt(dx,dy,dz,tDstVoxel);
				}
			}
		}
	}

	template< typename SrcVolumeType, typename DstVolumeType>
	void VolumeResampler<SrcVolumeType, DstVolumeType>::resampleArbitrary()
	{
		float srcWidth  = m_regSrc.getWidthInCells();
		float srcHeight = m_regSrc.getHeightInCells();
		float srcDepth  = m_regSrc.getDepthInCells();

		float dstWidth  = m_regDst.getWidthInCells();
		float dstHeight = m_regDst.getHeightInCells();
		float dstDepth  = m_regDst.getDepthInCells();
		
		float fScaleX = srcWidth / dstWidth;
		float fScaleY = srcHeight / dstHeight;
		float fScaleZ = srcDepth / dstDepth;

		typename SrcVolumeType::Sampler sampler(m_pVolSrc);

		for(int32_t dz = m_regDst.getLowerCorner().getZ(); dz <= m_regDst.getUpperCorner().getZ(); dz++)
		{
			for(int32_t dy = m_regDst.getLowerCorner().getY(); dy <= m_regDst.getUpperCorner().getY(); dy++)
			{
				for(int32_t dx = m_regDst.getLowerCorner().getX(); dx <= m_regDst.getUpperCorner().getX(); dx++)
				{
					float sx = (dx - m_regDst.getLowerCorner().getX()) * fScaleX;
					float sy = (dy - m_regDst.getLowerCorner().getY()) * fScaleY;
					float sz = (dz - m_regDst.getLowerCorner().getZ()) * fScaleZ;

					sx += m_regSrc.getLowerCorner().getX();
					sy += m_regSrc.getLowerCorner().getY();
					sz += m_regSrc.getLowerCorner().getZ();

					sampler.setPosition(sx,sy,sz);
					const typename SrcVolumeType::VoxelType& voxel000 = sampler.peekVoxel0px0py0pz();
					const typename SrcVolumeType::VoxelType& voxel001 = sampler.peekVoxel0px0py1pz();
					const typename SrcVolumeType::VoxelType& voxel010 = sampler.peekVoxel0px1py0pz();
					const typename SrcVolumeType::VoxelType& voxel011 = sampler.peekVoxel0px1py1pz();
					const typename SrcVolumeType::VoxelType& voxel100 = sampler.peekVoxel1px0py0pz();
					const typename SrcVolumeType::VoxelType& voxel101 = sampler.peekVoxel1px0py1pz();
					const typename SrcVolumeType::VoxelType& voxel110 = sampler.peekVoxel1px1py0pz();
					const typename SrcVolumeType::VoxelType& voxel111 = sampler.peekVoxel1px1py1pz();

					//FIXME - should accept all float parameters, but GCC complains?
					double dummy;
					sx = modf(sx, &dummy);
					sy = modf(sy, &dummy);
					sz = modf(sz, &dummy);

					typename SrcVolumeType::VoxelType tInterpolatedValue = trilerp(voxel000,voxel100,voxel010,voxel110,voxel001,voxel101,voxel011,voxel111,sx,sy,sz);

					typename DstVolumeType::VoxelType result = static_cast<typename DstVolumeType::VoxelType>(tInterpolatedValue);
					m_pVolDst->setVoxelAt(dx,dy,dz,result);
				}
			}
		}
	}

	template< typename SrcVolumeType, typename DstVolumeType>
	void VolumeResampler<SrcVolumeType, DstVolumeType>::resampleBetter()
	{
		float srcWidth  = m_regSrc.getWidthInCells();
		float srcHeight = m_regSrc.getHeightInCells();
		float srcDepth  = m_regSrc.getDepthInCells();

		float dstWidth  = m_regDst.getWidthInCells();
		float dstHeight = m_regDst.getHeightInCells();
		float dstDepth  = m_regDst.getDepthInCells();
		
		float fScaleX = srcWidth / dstWidth;
		float fScaleY = srcHeight / dstHeight;
		float fScaleZ = srcDepth / dstDepth;

		typename SrcVolumeType::Sampler sampler(m_pVolSrc);
		
		// Should use SrcVolumeType? Or new template parameter?
		Region regDownscaledX(0, 0, 0, m_regDst.getWidthInVoxels()-1, m_regSrc.getHeightInVoxels()-1, m_regSrc.getDepthInVoxels()-1);
		SrcVolumeType volDownscaledX(regDownscaledX);

		for(int32_t tz = regDownscaledX.getLowerZ(); tz <= regDownscaledX.getUpperZ(); tz++)
		{
			for(int32_t ty = regDownscaledX.getLowerY(); ty <= regDownscaledX.getUpperY(); ty++)
			{
				for(int32_t tx = regDownscaledX.getLowerX(); tx <= regDownscaledX.getUpperX(); tx++)
				{
					float sx = (tx * fScaleX) + m_regSrc.getLowerCorner().getX();
					float sy = (ty          ) + m_regSrc.getLowerCorner().getY();
					float sz = (tz          ) + m_regSrc.getLowerCorner().getZ();

					typename SrcVolumeType::VoxelType result = m_pVolSrc->getVoxelWithWrapping(sx, sy, sz, WrapModes::Clamp);

					volDownscaledX.setVoxelAt(tx, ty, tz, result);
				}
			}
		}

		//Now downscale in y
		Region regDownscaledXAndY(0, 0, 0, m_regDst.getWidthInVoxels()-1, m_regDst.getHeightInVoxels()-1, m_regSrc.getDepthInVoxels()-1);
		SrcVolumeType volDownscaledXAndY(regDownscaledXAndY);

		for(int32_t tz = regDownscaledXAndY.getLowerZ(); tz <= regDownscaledXAndY.getUpperZ(); tz++)
		{
			for(int32_t ty = regDownscaledXAndY.getLowerY(); ty <= regDownscaledXAndY.getUpperY(); ty++)
			{
				for(int32_t tx = regDownscaledXAndY.getLowerX(); tx <= regDownscaledXAndY.getUpperX(); tx++)
				{
					float sx = (tx          );
					float sy = (ty * fScaleY);
					float sz = (tz          );

					typename SrcVolumeType::VoxelType result = volDownscaledX.getVoxelWithWrapping(sx, sy, sz, WrapModes::Clamp);

					volDownscaledXAndY.setVoxelAt(tx, ty, tz, result);
				}
			}
		}

		//Now copy and downscale to dst.
		//Region regDst = m_pVolDst->getEnclosingRegion();

		for(int32_t tz = m_regDst.getLowerZ(); tz <= m_regDst.getUpperZ(); tz++)
		{
			for(int32_t ty = m_regDst.getLowerY(); ty <= m_regDst.getUpperY(); ty++)
			{
				for(int32_t tx = m_regDst.getLowerX(); tx <= m_regDst.getUpperX(); tx++)
				{
					float sx = (tx - m_regDst.getLowerX());
					float sy = (ty - m_regDst.getLowerY());
					float sz = (tz - m_regDst.getLowerZ()) * fScaleZ;

					typename SrcVolumeType::VoxelType result = volDownscaledXAndY.getVoxelWithWrapping(sx, sy, sz, WrapModes::Clamp);

					m_pVolDst->setVoxelAt(tx, ty, tz, result);
				}
			}
		}
	}
}
