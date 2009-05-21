#include "SurfaceAdjusters.h"

#include "VolumeSampler.h"
#include "GradientEstimators.h"
#include "IndexedSurfacePatch.h"
#include "PolyVoxImpl/Utility.h"
#include "VoxelFilters.h"

#include <vector>

using namespace std;

namespace PolyVox
{
	void smoothRegionGeometry(Volume<uint8_t>* volumeData, IndexedSurfacePatch& isp)
	{
		const uint8_t uSmoothingFactor = 2;
		const float fThreshold = 0.5f;

		VolumeSampler<uint8_t> volIter(*volumeData);

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
				bool lowerCornerInside = volumeData->getEnclosingRegion().containsPoint(v3dFloor,2);
				bool upperCornerInside = volumeData->getEnclosingRegion().containsPoint(v3dFloor+Vector3DInt32(1,1,1),2);

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

	void adjustDecimatedGeometry(Volume<uint8_t>* volumeData, IndexedSurfacePatch& isp, uint8_t val)
	{
		VolumeSampler<uint8_t> volIter(*volumeData);

		std::vector<SurfaceVertex>& vecVertices = isp.getRawVertexData();
		std::vector<SurfaceVertex>::iterator iterSurfaceVertex = vecVertices.begin();
		while(iterSurfaceVertex != vecVertices.end())
		{
			Vector3DFloat v3dPos = iterSurfaceVertex->getPosition() + static_cast<Vector3DFloat>(isp.m_v3dRegionPosition);
			Vector3DInt32 v3dFloor = static_cast<Vector3DInt32>(v3dPos);

			VolumeSampler<uint8_t> volIter(*volumeData);

			//Check all corners are within the volume, allowing a boundary for gradient estimation
			bool lowerCornerInside = volumeData->getEnclosingRegion().containsPoint(v3dFloor,1);
			bool upperCornerInside = volumeData->getEnclosingRegion().containsPoint(v3dFloor+Vector3DInt32(1,1,1),1);

			if(lowerCornerInside && upperCornerInside) //If this test fails the vertex will be left as it was
			{				
				//volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor));
				//const uint8_t uFloor = volIter.getVoxel();
				if(((v3dPos.getX() - v3dFloor.getX()) < 0.001) && ((v3dPos.getY() - v3dFloor.getY()) < 0.001) && ((v3dPos.getZ() - v3dFloor.getZ()) < 0.001))
				//int x = v3dPos.getX();
				//if(x % 2 != 0)
				//if((iterSurfaceVertex->getNormal().getX() > 0.5f) || (iterSurfaceVertex->getNormal().getX() < -0.5f))
				{	
					//exit(0);
					//volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor+Vector3DInt32(1,0,0)));
					//const uint8_t uCeil = volIter.getVoxel();
					//if(uFloor == uCeil) //In this case they must both be zero
					{						
						//if(iterSurfaceVertex->getNormal().getX() > 0)
						{
							iterSurfaceVertex->setPosition(iterSurfaceVertex->getPosition() - iterSurfaceVertex->getNormal() * 0.5f);
							v3dPos = iterSurfaceVertex->getPosition() + static_cast<Vector3DFloat>(isp.m_v3dRegionPosition);
							v3dFloor = static_cast<Vector3DInt32>(v3dPos);

							volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor));
							const uint8_t uFloor = volIter.getVoxel();

							uint8_t uCeil;
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

	IndexedSurfacePatch getSmoothedSurface(IndexedSurfacePatch ispInput)
	{
		IndexedSurfacePatch ispOutput = ispInput;

		for(vector<uint32_t>::iterator iterIndex = ispInput.m_vecTriangleIndices.begin(); iterIndex != ispInput.m_vecTriangleIndices.end();)
		{
			SurfaceVertex& v0 = ispOutput.m_vecVertices[*iterIndex];
			iterIndex++;
			SurfaceVertex& v1 = ispOutput.m_vecVertices[*iterIndex];
			iterIndex++;
			SurfaceVertex& v2 = ispOutput.m_vecVertices[*iterIndex];
			iterIndex++;

			//FIXME - instead of finding these opposite points (Opp) we could just use the midpoint?
			Vector3DFloat v0Opp = (v1.position + v2.position) / 2.0f;
			Vector3DFloat v1Opp = (v0.position + v2.position) / 2.0f;
			Vector3DFloat v2Opp = (v0.position + v1.position) / 2.0f;

			Vector3DFloat v0ToOpp = v0Opp - v0.position;
			v0ToOpp.normalise();
			Vector3DFloat v1ToOpp = v1Opp - v1.position;
			v1ToOpp.normalise();
			Vector3DFloat v2ToOpp = v2Opp - v2.position;
			v2ToOpp.normalise();

			Vector3DFloat n0 = v0.getNormal();
			n0.normalise();
			Vector3DFloat n1 = v1.getNormal();
			n1.normalise();
			Vector3DFloat n2 = v2.getNormal();
			n2.normalise();

			v0.position += (n0 * (n0.dot(v0ToOpp)) * 0.1f);
			v1.position += (n1 * (n1.dot(v1ToOpp)) * 0.1f);
			v2.position += (n2 * (n2.dot(v2ToOpp)) * 0.1f);

			/*Vector3DFloat triNormal = (v0.getPosition() - v1.getPosition()).cross(v2.getPosition() - v1.getPosition());
			triNormal.normalise();

			v0.position += (n0 * (triNormal.dot(n0)) * -0.01f);
			v1.position += (n1 * (triNormal.dot(n1)) * -0.01f);
			v2.position += (n2 * (triNormal.dot(n2)) * -0.01f);*/
		}

		return ispOutput;
	}
}