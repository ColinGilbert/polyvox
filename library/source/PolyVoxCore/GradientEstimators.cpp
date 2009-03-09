#include "PolyVoxCore/GradientEstimators.h"
#include "PolyVoxCore/IndexedSurfacePatch.h"
#include "PolyVoxCore/SurfaceVertex.h"

#include "PolyVoxCore/PolyVoxCStdInt.h"

using namespace std;

namespace PolyVox
{
	POLYVOX_API void computeNormalsForVertices(BlockVolume<uint8>* volumeData, IndexedSurfacePatch& isp, NormalGenerationMethod normalGenerationMethod)
	{
		std::vector<SurfaceVertex>& vecVertices = isp.getRawVertexData();
		std::vector<SurfaceVertex>::iterator iterSurfaceVertex = vecVertices.begin();
		while(iterSurfaceVertex != vecVertices.end())
		{
			const Vector3DFloat& v3dPos = iterSurfaceVertex->getPosition() + static_cast<Vector3DFloat>(isp.m_v3dRegionPosition);
			const Vector3DInt32 v3dFloor = static_cast<Vector3DInt32>(v3dPos);

			BlockVolumeIterator<uint8> volIter(*volumeData);

			//Check all corners are within the volume, allowing a boundary for gradient estimation
			bool lowerCornerInside = volumeData->containsPoint(v3dFloor,2);
			bool upperCornerInside = volumeData->containsPoint(v3dFloor+Vector3DInt32(1,1,1),2);

			if(lowerCornerInside && upperCornerInside) //If this test fails the vertex will be left as it was
			{
				Vector3DFloat v3dGradient = computeNormal(volumeData, v3dPos, normalGenerationMethod);
				
				if(v3dGradient.lengthSquared() > 0.0001)
				{
					//If we got a normal of significant length then update it.
					//Otherwise leave it as it was (should be the 'simple' version)
					v3dGradient.normalise();
					iterSurfaceVertex->setNormal(v3dGradient);
				}
			} //(lowerCornerInside && upperCornerInside)
			++iterSurfaceVertex;
		}
	}

	Vector3DFloat computeNormal(BlockVolume<uint8>* volumeData, const Vector3DFloat& v3dPos, NormalGenerationMethod normalGenerationMethod)
	{
		Vector3DFloat v3dGradient; //To store the result

		BlockVolumeIterator<uint8> volIter(*volumeData);

			const Vector3DInt32 v3dFloor = static_cast<Vector3DInt32>(v3dPos);

			volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor));
			Vector3DFloat gradFloor;
			switch(normalGenerationMethod)
			{
			case SOBEL_SMOOTHED:
				gradFloor = computeSmoothSobelGradient(volIter);
				break;
			case CENTRAL_DIFFERENCE_SMOOTHED:
				gradFloor = computeSmoothCentralDifferenceGradient(volIter);
				break;
			case SOBEL:
				gradFloor = computeSobelGradient(volIter);
				break;
			case CENTRAL_DIFFERENCE:
				gradFloor = computeCentralDifferenceGradient(volIter);
				break;
			}

			if((v3dPos.getX() - v3dFloor.getX()) > 0.25) //The result should be 0.0 or 0.5
			{			
				volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor+Vector3DInt32(1,0,0)));
			}
			if((v3dPos.getY() - v3dFloor.getY()) > 0.25) //The result should be 0.0 or 0.5
			{			
				volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor+Vector3DInt32(0,1,0)));
			}
			if((v3dPos.getZ() - v3dFloor.getZ()) > 0.25) //The result should be 0.0 or 0.5
			{			
				volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor+Vector3DInt32(0,0,1)));					
			}

			Vector3DFloat gradCeil;
			switch(normalGenerationMethod)
			{
			case SOBEL_SMOOTHED:
				gradCeil = computeSmoothSobelGradient(volIter);
				break;
			case CENTRAL_DIFFERENCE_SMOOTHED:
				gradCeil = computeSmoothCentralDifferenceGradient(volIter);
				break;
			case SOBEL:
				gradCeil = computeSobelGradient(volIter);
				break;
			case CENTRAL_DIFFERENCE:
				gradCeil = computeCentralDifferenceGradient(volIter);
				break;
			}

			v3dGradient = (gradFloor + gradCeil);
			if(v3dGradient.lengthSquared() < 0.0001)
			{
				//Operation failed - fall back on simple gradient estimation
				normalGenerationMethod = SIMPLE;
			}

			if(normalGenerationMethod == SIMPLE)
			{
				volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor));
				const uint8 uFloor = volIter.getVoxel() > 0 ? 1 : 0;
				if((v3dPos.getX() - v3dFloor.getX()) > 0.25) //The result should be 0.0 or 0.5
				{					
					uint8 uCeil = volIter.peekVoxel1px0py0pz() > 0 ? 1 : 0;
					v3dGradient = Vector3DFloat(static_cast<float>(uFloor - uCeil),0.0,0.0);
				}
				else if((v3dPos.getY() - v3dFloor.getY()) > 0.25) //The result should be 0.0 or 0.5
				{
					uint8 uCeil = volIter.peekVoxel0px1py0pz() > 0 ? 1 : 0;
					v3dGradient = Vector3DFloat(0.0,static_cast<float>(uFloor - uCeil),0.0);
				}
				else if((v3dPos.getZ() - v3dFloor.getZ()) > 0.25) //The result should be 0.0 or 0.5
				{
					uint8 uCeil = volIter.peekVoxel0px0py1pz() > 0 ? 1 : 0;
					v3dGradient = Vector3DFloat(0.0, 0.0,static_cast<float>(uFloor - uCeil));					
				}
			}
			return v3dGradient;
	}
}
