#include "PolyVoxCore/SurfaceAdjusters.h"

#include "PolyVoxCore/BlockVolumeIterator.h"
#include "PolyVoxCore/GradientEstimators.h"
#include "PolyVoxCore/IndexedSurfacePatch.h"
#include "PolyVoxCore/Utility.h"
#include "PolyVoxCore/VoxelFilters.h"

#include <vector>

using namespace std;

namespace PolyVox
{
	void smoothRegionGeometry(BlockVolume<uint8>* volumeData, IndexedSurfacePatch& isp)
	{
		const uint8 uSmoothingFactor = 2;
		const float fThreshold = 0.5f;

		BlockVolumeIterator<uint8> volIter(*volumeData);

		std::vector<SurfaceVertex>& vecVertices = isp.getRawVertexData();
		std::vector<SurfaceVertex>::iterator iterSurfaceVertex = vecVertices.begin();
		while(iterSurfaceVertex != vecVertices.end())
		{
			for(int ct = 0; ct < uSmoothingFactor; ++ct)
			{
				const Vector3DFloat& v3dPos = iterSurfaceVertex->getPosition() + static_cast<Vector3DFloat>(isp.m_v3dRegionPosition);
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

	void adjustDecimatedGeometry(BlockVolume<uint8>* volumeData, IndexedSurfacePatch& isp, uint8 val)
	{
		BlockVolumeIterator<uint8> volIter(*volumeData);

		std::vector<SurfaceVertex>& vecVertices = isp.getRawVertexData();
		std::vector<SurfaceVertex>::iterator iterSurfaceVertex = vecVertices.begin();
		while(iterSurfaceVertex != vecVertices.end())
		{
			Vector3DFloat v3dPos = iterSurfaceVertex->getPosition() + static_cast<Vector3DFloat>(isp.m_v3dRegionPosition);
			Vector3DInt32 v3dFloor = static_cast<Vector3DInt32>(v3dPos);

			BlockVolumeIterator<uint8> volIter(*volumeData);

			//Check all corners are within the volume, allowing a boundary for gradient estimation
			bool lowerCornerInside = volumeData->containsPoint(v3dFloor,1);
			bool upperCornerInside = volumeData->containsPoint(v3dFloor+Vector3DInt32(1,1,1),1);

			if(lowerCornerInside && upperCornerInside) //If this test fails the vertex will be left as it was
			{				
				//volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor));
				//const uint8 uFloor = volIter.getVoxel();
				if(((v3dPos.getX() - v3dFloor.getX()) < 0.001) && ((v3dPos.getY() - v3dFloor.getY()) < 0.001) && ((v3dPos.getZ() - v3dFloor.getZ()) < 0.001))
				//int x = v3dPos.getX();
				//if(x % 2 != 0)
				//if((iterSurfaceVertex->getNormal().getX() > 0.5f) || (iterSurfaceVertex->getNormal().getX() < -0.5f))
				{	
					//exit(0);
					//volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor+Vector3DInt32(1,0,0)));
					//const uint8 uCeil = volIter.getVoxel();
					//if(uFloor == uCeil) //In this case they must both be zero
					{						
						//if(iterSurfaceVertex->getNormal().getX() > 0)
						{
							iterSurfaceVertex->setPosition(iterSurfaceVertex->getPosition() - iterSurfaceVertex->getNormal() * 0.5f);
							v3dPos = iterSurfaceVertex->getPosition() + static_cast<Vector3DFloat>(isp.m_v3dRegionPosition);
							v3dFloor = static_cast<Vector3DInt32>(v3dPos);

							volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor));
							const uint8 uFloor = volIter.getVoxel();

							uint8 uCeil;
							if((iterSurfaceVertex->getNormal().getX() > 0.5f) || (iterSurfaceVertex->getNormal().getX() < -0.5f))
							{
								volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor+Vector3DInt32(1,0,0)));
								uCeil = volIter.getVoxel();
							}
							if((iterSurfaceVertex->getNormal().getY() > 0.5f) || (iterSurfaceVertex->getNormal().getY() < -0.5f))
							{
								volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor+Vector3DInt32(0,1,0)));
								uCeil = volIter.getVoxel();
							}
							if((iterSurfaceVertex->getNormal().getZ() > 0.5f) || (iterSurfaceVertex->getNormal().getZ() < -0.5f))
							{
								volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor+Vector3DInt32(0,0,1)));
								uCeil = volIter.getVoxel();
							}

							if(uFloor == uCeil)
							{
								//NOTE: The normal should actually be multiplied by 1.0f. This works 
								//for the simple cube but causes depth fighting on more complex shapes.
								iterSurfaceVertex->setPosition(iterSurfaceVertex->getPosition() - iterSurfaceVertex->getNormal() * 0.5f);
							}
						}
					}
				}
			}

			++iterSurfaceVertex;
		} //while(iterSurfaceVertex != vecVertices.end())
	}
}