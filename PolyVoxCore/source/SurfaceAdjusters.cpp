#include "SurfaceAdjusters.h"

#include "BlockVolumeIterator.h"
#include "GradientEstimators.h"
#include "IndexedSurfacePatch.h"
#include "RegionGeometry.h"
#include "Utility.h"

#include <vector>

namespace PolyVox
{
	void smoothRegionGeometry(BlockVolume<boost::uint8_t>* volumeData, RegionGeometry& regGeom)
	{
		const boost::uint8_t uSmoothingFactor = 2;
		const float fThreshold = 0.5f;

		BlockVolumeIterator<boost::uint8_t> volIter(*volumeData);

		std::vector<SurfaceVertex>& vecVertices = regGeom.m_patchSingleMaterial->m_vecVertices;
		std::vector<SurfaceVertex>::iterator iterSurfaceVertex = vecVertices.begin();
		while(iterSurfaceVertex != vecVertices.end())
		{
			for(int ct = 0; ct < uSmoothingFactor; ++ct)
			{
				const Vector3DFloat& v3dPos = iterSurfaceVertex->getPosition() + static_cast<Vector3DFloat>(regGeom.m_v3dRegionPosition);
				const Vector3DInt32 v3dFloor = static_cast<Vector3DInt32>(v3dPos);
				const Vector3DFloat& v3dRem = v3dPos - static_cast<Vector3DFloat>(v3dFloor);			

				//Check all corners are within the volume, allowing a boundary for gradient estimation
				bool lowerCornerInside = volumeData->containsPoint(v3dFloor,2);
				bool upperCornerInside = volumeData->containsPoint(v3dFloor+Vector3DInt32(1,1,1),2);

				if(lowerCornerInside && upperCornerInside) //If this test fails the vertex will be left as it was
				{
					volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor + Vector3DInt32(0,0,0)));
					const float v000 = computeSmoothedVoxel(volIter);
					Vector3DFloat grad000 = computeSmoothCentralDifferenceGradient(volIter);
					volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor + Vector3DInt32(1,0,0)));
					const float v100 = computeSmoothedVoxel(volIter);
					Vector3DFloat grad100 = computeSmoothCentralDifferenceGradient(volIter);

					volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor + Vector3DInt32(0,1,0)));
					const float v010 = computeSmoothedVoxel(volIter);
					Vector3DFloat grad010 = computeSmoothCentralDifferenceGradient(volIter);
					volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor + Vector3DInt32(1,1,0)));
					const float v110 = computeSmoothedVoxel(volIter);
					Vector3DFloat grad110 = computeSmoothCentralDifferenceGradient(volIter);

					volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor + Vector3DInt32(0,0,1)));
					const float v001 = computeSmoothedVoxel(volIter);
					Vector3DFloat grad001 = computeSmoothCentralDifferenceGradient(volIter);
					volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor + Vector3DInt32(1,0,1)));
					const float v101 = computeSmoothedVoxel(volIter);
					Vector3DFloat grad101 = computeSmoothCentralDifferenceGradient(volIter);

					volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor + Vector3DInt32(0,1,1)));
					const float v011 = computeSmoothedVoxel(volIter);
					Vector3DFloat grad011 = computeSmoothCentralDifferenceGradient(volIter);
					volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor + Vector3DInt32(1,1,1)));
					const float v111 = computeSmoothedVoxel(volIter);
					Vector3DFloat grad111 = computeSmoothCentralDifferenceGradient(volIter);

					float fInterVal = trilinearlyInterpolate(v000,v100,v010,v110,v001,v101,v011,v111,v3dRem.getX(),v3dRem.getY(),v3dRem.getZ());
					Vector3DFloat fInterGrad = trilinearlyInterpolate(grad000,grad100,grad010,grad110,grad001,grad101,grad011,grad111,v3dRem.getX(),v3dRem.getY(),v3dRem.getZ());

					fInterGrad.normalise();	
					float fDiff = fInterVal - fThreshold;
					iterSurfaceVertex->setPosition(iterSurfaceVertex->getPosition() + (fInterGrad * fDiff));				
					iterSurfaceVertex->setNormal(fInterGrad); //This is actually the gradient for the previous position, but it won't have moved much.
				} //if(lowerCornerInside && upperCornerInside)
			} //for(int ct = 0; ct < uSmoothingFactor; ++ct)
			++iterSurfaceVertex;
		} //while(iterSurfaceVertex != vecVertices.end())
	}

	float computeSmoothedVoxel(BlockVolumeIterator<boost::uint8_t>& volIter)
	{
		assert(volIter.getPosX() >= 1);
		assert(volIter.getPosY() >= 1);
		assert(volIter.getPosZ() >= 1);
		assert(volIter.getPosX() < volIter.getVolume().getSideLength() - 2);
		assert(volIter.getPosY() < volIter.getVolume().getSideLength() - 2);
		assert(volIter.getPosZ() < volIter.getVolume().getSideLength() - 2);

		float sum = 0.0;

		if(volIter.peekVoxel1nx1ny1nz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1nx1ny0pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1nx1ny1pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1nx0py1nz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1nx0py0pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1nx0py1pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1nx1py1nz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1nx1py0pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1nx1py1pz() != 0) sum += 1.0f;

		if(volIter.peekVoxel0px1ny1nz() != 0) sum += 1.0f;
		if(volIter.peekVoxel0px1ny0pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel0px1ny1pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel0px0py1nz() != 0) sum += 1.0f;
		if(volIter.getVoxel() != 0) sum += 1.0f;
		if(volIter.peekVoxel0px0py1pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel0px1py1nz() != 0) sum += 1.0f;
		if(volIter.peekVoxel0px1py0pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel0px1py1pz() != 0) sum += 1.0f;

		if(volIter.peekVoxel1px1ny1nz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1px1ny0pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1px1ny1pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1px0py1nz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1px0py0pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1px0py1pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1px1py1nz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1px1py0pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1px1py1pz() != 0) sum += 1.0f;

		sum /= 27.0f;
		return sum;
	}
}