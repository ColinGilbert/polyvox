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

#include "PolyVoxCore/Impl/Utility.h"

#include <cmath>

namespace PolyVox
{
	// Takes an interpolated sample of the volume data.
	template<typename VolumeType>
	typename VolumeType::VoxelType interpolatedSample(VolumeType* pVolume, float fPosX, float fPosY, float fPosZ, WrapMode eWrapMode, typename VolumeType::VoxelType tBorder)
	{
		float fFloorX = floor(fPosX);
		float fFloorY = floor(fPosY);
		float fFloorZ = floor(fPosZ);

		float fInterpX = fPosX - fFloorX;
		float fInterpY = fPosY - fFloorY;
		float fInterpZ = fPosZ - fFloorZ;

		// Conditional logic required to round negative floats correctly
		int32_t iX = static_cast<int32_t>(fFloorX > 0.0f ? fFloorX + 0.5f : fFloorX - 0.5f); 
		int32_t iY = static_cast<int32_t>(fFloorY > 0.0f ? fFloorY + 0.5f : fFloorY - 0.5f); 
		int32_t iZ = static_cast<int32_t>(fFloorZ > 0.0f ? fFloorZ + 0.5f : fFloorZ - 0.5f);

		const typename VolumeType::VoxelType& voxel000 = pVolume->getVoxelWithWrapping(iX, iY, iZ, eWrapMode, tBorder);
		const typename VolumeType::VoxelType& voxel001 = pVolume->getVoxelWithWrapping(iX, iY, iZ + 1, eWrapMode, tBorder);
		const typename VolumeType::VoxelType& voxel010 = pVolume->getVoxelWithWrapping(iX, iY + 1, iZ, eWrapMode, tBorder);
		const typename VolumeType::VoxelType& voxel011 = pVolume->getVoxelWithWrapping(iX, iY + 1, iZ + 1, eWrapMode, tBorder);
		const typename VolumeType::VoxelType& voxel100 = pVolume->getVoxelWithWrapping(iX + 1, iY, iZ, eWrapMode, tBorder);
		const typename VolumeType::VoxelType& voxel101 = pVolume->getVoxelWithWrapping(iX + 1, iY, iZ + 1, eWrapMode, tBorder);
		const typename VolumeType::VoxelType& voxel110 = pVolume->getVoxelWithWrapping(iX + 1, iY + 1, iZ, eWrapMode, tBorder);
		const typename VolumeType::VoxelType& voxel111 = pVolume->getVoxelWithWrapping(iX + 1, iY + 1, iZ + 1, eWrapMode, tBorder);

		typename VolumeType::VoxelType tInterpolatedValue = PolyVox::trilerp(voxel000,voxel100,voxel010,voxel110,voxel001,voxel101,voxel011,voxel111,fInterpX,fInterpY,fInterpZ);

		return tInterpolatedValue;
	}

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
		int32_t uSrcWidth = m_regSrc.getUpperX() - m_regSrc.getLowerX() + 1;
		int32_t uSrcHeight = m_regSrc.getUpperY() - m_regSrc.getLowerY() + 1;
		int32_t uSrcDepth = m_regSrc.getUpperZ() - m_regSrc.getLowerZ() + 1;

		int32_t uDstWidth = m_regDst.getUpperX() - m_regDst.getLowerX() + 1;
		int32_t uDstHeight = m_regDst.getUpperY() - m_regDst.getLowerY() + 1;
		int32_t uDstDepth = m_regDst.getUpperZ() - m_regDst.getLowerZ() + 1;

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
		for(int32_t sz = m_regSrc.getLowerZ(), dz = m_regDst.getLowerZ(); dz <= m_regDst.getUpperZ(); sz++, dz++)
		{
			for(int32_t sy = m_regSrc.getLowerY(), dy = m_regDst.getLowerY(); dy <= m_regDst.getUpperY(); sy++, dy++)
			{
				for(int32_t sx = m_regSrc.getLowerX(), dx = m_regDst.getLowerX(); dx <= m_regDst.getUpperX(); sx++,dx++)
				{
					const typename SrcVolumeType::VoxelType& tSrcVoxel = m_pVolSrc->getVoxel(sx,sy,sz, WrapModes::AssumeValid); // FIXME use templatised version of getVoxel(), but watch out for Linux compile issues.
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

		for(int32_t dz = m_regDst.getLowerZ(); dz <= m_regDst.getUpperZ(); dz++)
		{
			for(int32_t dy = m_regDst.getLowerY(); dy <= m_regDst.getUpperY(); dy++)
			{
				for(int32_t dx = m_regDst.getLowerX(); dx <= m_regDst.getUpperX(); dx++)
				{
					float sx = (dx - m_regDst.getLowerX()) * fScaleX;
					float sy = (dy - m_regDst.getLowerY()) * fScaleY;
					float sz = (dz - m_regDst.getLowerZ()) * fScaleZ;

					sx += m_regSrc.getLowerX();
					sy += m_regSrc.getLowerY();
					sz += m_regSrc.getLowerZ();

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

					typename SrcVolumeType::VoxelType result = m_pVolSrc->getVoxelWithWrapping(sx, sy, sz, WrapModes::Border);

					//typename SrcVolumeType::VoxelType result = interpolatedSample(m_pVolSrc, sx, sy, sz, WrapModes::Border, SrcVolumeType::VoxelType(0));

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

					typename SrcVolumeType::VoxelType result = volDownscaledX.getVoxelWithWrapping(sx, sy, sz, WrapModes::Border);

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
					float sLowerZ = ((tz - 1) - m_regDst.getLowerZ()) * fScaleZ;
					float sCentreZ = ((tz   ) - m_regDst.getLowerZ()) * fScaleZ;
					float sUpperZ = ((tz + 1) - m_regDst.getLowerZ()) * fScaleZ;

					float sumOfWeights = 0.0f;
					//typename SrcVolumeType::VoxelType tSum = SrcVolumeType::VoxelType(0);

					//We should be able to use a higher range MultiMaterial rather than needing to use a Vector of floats.
					//We shouold also probably support an Accumulation type rather than hard coding.
					Vector<4, float> vecSum(0.0, 0.0, 0.0, 0.0);

					for(float sz = sLowerZ; sz <= sUpperZ; sz += 1.0)
					{
						float weight = triangleFilter(sz - sCentreZ);
						sumOfWeights += weight;

						//This is wrong! There's no need to do interpolation. Just multiply the sameple by the correct kernel value.
						Vector<4, float> sample = interpolatedSample(&volDownscaledXAndY, sx, sy, sz, WrapModes::Border, SrcVolumeType::VoxelType());

						vecSum += (sample * weight);
					}

					vecSum /= sumOfWeights; //Should divide by 'norm'

					typename SrcVolumeType::VoxelType tResult = vecSum; //Should divide by 'norm'

					m_pVolDst->setVoxelAt(tx, ty, tz, tResult);
				}
			}
		}
	}
}
