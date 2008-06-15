#include "GradientEstimators.h"
#include "IndexedSurfacePatch.h"
#include "RegionGeometry.h"
#include "SurfaceVertex.h"

#include "boost/cstdint.hpp"

using namespace boost;

namespace PolyVox
{
	POLYVOX_API void computeNormalsForVertices(BlockVolume<boost::uint8_t>* volumeData, RegionGeometry& regGeom, NormalGenerationMethod normalGenerationMethod)
	{
		std::vector<SurfaceVertex>& vecVertices = regGeom.m_patchSingleMaterial->m_vecVertices;
		std::vector<SurfaceVertex>::iterator iterSurfaceVertex = vecVertices.begin();
		while(iterSurfaceVertex != vecVertices.end())
		{
			const Vector3DFloat& v3dPos = iterSurfaceVertex->getPosition() + static_cast<Vector3DFloat>(regGeom.m_v3dRegionPosition);
			const Vector3DInt32 v3dFloor = static_cast<Vector3DInt32>(v3dPos);

			BlockVolumeIterator<boost::uint8_t> volIter(*volumeData);

			//Check all corners are within the volume, allowing a boundary for gradient estimation
			bool lowerCornerInside = volumeData->containsPoint(v3dFloor,1);
			bool upperCornerInside = volumeData->containsPoint(v3dFloor+Vector3DInt32(1,1,1),1);

			if(lowerCornerInside && upperCornerInside) //If this test fails the vertex will be left as it was
			{
				Vector3DFloat v3dGradient; //To store the result

				if(normalGenerationMethod == SOBEL)
				{
					volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor));
					const Vector3DFloat gradFloor = computeSobelGradient(volIter);
					if((v3dPos.getX() - v3dFloor.getX()) > 0.001)
					{			
						volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor+Vector3DInt32(1,0,0)));
					}
					if((v3dPos.getY() - v3dFloor.getY()) > 0.001)
					{			
						volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor+Vector3DInt32(0,1,0)));
					}
					if((v3dPos.getZ() - v3dFloor.getZ()) > 0.001)
					{			
						volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor+Vector3DInt32(0,0,1)));					
					}
					const Vector3DFloat gradCeil = computeSobelGradient(volIter);
					v3dGradient = (gradFloor + gradCeil);				
				}
				if(normalGenerationMethod == CENTRAL_DIFFERENCE)
				{
					volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor));
					const Vector3DFloat gradFloor = computeCentralDifferenceGradient(volIter);
					if((v3dPos.getX() - v3dFloor.getX()) > 0.001)
					{			
						volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor+Vector3DInt32(1,0,0)));
					}
					if((v3dPos.getY() - v3dFloor.getY()) > 0.001)
					{			
						volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor+Vector3DInt32(0,1,0)));
					}
					if((v3dPos.getZ() - v3dFloor.getZ()) > 0.001)
					{			
						volIter.setPosition(static_cast<Vector3DInt16>(v3dFloor+Vector3DInt32(0,0,1)));					
					}
					const Vector3DFloat gradCeil = computeCentralDifferenceGradient(volIter);
					v3dGradient = (gradFloor + gradCeil);
				}
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

	Vector3DFloat computeNormal(BlockVolume<uint8_t>* volumeData, const Vector3DFloat& position, NormalGenerationMethod normalGenerationMethod)
	{
		const float posX = position.getX();
		const float posY = position.getY();
		const float posZ = position.getZ();

		const uint16_t floorX = static_cast<uint16_t>(posX);
		const uint16_t floorY = static_cast<uint16_t>(posY);
		const uint16_t floorZ = static_cast<uint16_t>(posZ);

		//Check all corners are within the volume, allowing a boundary for gradient estimation
		bool lowerCornerInside = volumeData->containsPoint(Vector3DInt32(floorX, floorY, floorZ),1);
		bool upperCornerInside = volumeData->containsPoint(Vector3DInt32(floorX+1, floorY+1, floorZ+1),1);
		if((!lowerCornerInside) || (!upperCornerInside))
		{
			normalGenerationMethod = SIMPLE;
		}

		Vector3DFloat result;

		BlockVolumeIterator<boost::uint8_t> volIter(*volumeData); //FIXME - save this somewhere - could be expensive to create?


		if(normalGenerationMethod == SOBEL)
		{
			volIter.setPosition(static_cast<uint16_t>(posX),static_cast<uint16_t>(posY),static_cast<uint16_t>(posZ));
			const Vector3DFloat gradFloor = computeSobelGradient(volIter);
			if((posX - floorX) > 0.25) //The result should be 0.0 or 0.5
			{			
				volIter.setPosition(static_cast<uint16_t>(posX+1.0),static_cast<uint16_t>(posY),static_cast<uint16_t>(posZ));
			}
			if((posY - floorY) > 0.25) //The result should be 0.0 or 0.5
			{			
				volIter.setPosition(static_cast<uint16_t>(posX),static_cast<uint16_t>(posY+1.0),static_cast<uint16_t>(posZ));
			}
			if((posZ - floorZ) > 0.25) //The result should be 0.0 or 0.5
			{			
				volIter.setPosition(static_cast<uint16_t>(posX),static_cast<uint16_t>(posY),static_cast<uint16_t>(posZ+1.0));					
			}
			const Vector3DFloat gradCeil = computeSobelGradient(volIter);
			result = ((gradFloor + gradCeil) * -1.0f);
			if(result.lengthSquared() < 0.0001)
			{
				//Operation failed - fall back on simple gradient estimation
				normalGenerationMethod = SIMPLE;
			}
		}
		if(normalGenerationMethod == CENTRAL_DIFFERENCE)
		{
			volIter.setPosition(static_cast<uint16_t>(posX),static_cast<uint16_t>(posY),static_cast<uint16_t>(posZ));
			const Vector3DFloat gradFloor = computeCentralDifferenceGradient(volIter);
			if((posX - floorX) > 0.25) //The result should be 0.0 or 0.5
			{			
				volIter.setPosition(static_cast<uint16_t>(posX+1.0),static_cast<uint16_t>(posY),static_cast<uint16_t>(posZ));
			}
			if((posY - floorY) > 0.25) //The result should be 0.0 or 0.5
			{			
				volIter.setPosition(static_cast<uint16_t>(posX),static_cast<uint16_t>(posY+1.0),static_cast<uint16_t>(posZ));
			}
			if((posZ - floorZ) > 0.25) //The result should be 0.0 or 0.5
			{			
				volIter.setPosition(static_cast<uint16_t>(posX),static_cast<uint16_t>(posY),static_cast<uint16_t>(posZ+1.0));					
			}
			const Vector3DFloat gradCeil = computeCentralDifferenceGradient(volIter);
			result = ((gradFloor + gradCeil) * -1.0f);
			if(result.lengthSquared() < 0.0001)
			{
				//Operation failed - fall back on simple gradient estimation
				normalGenerationMethod = SIMPLE;
			}
		}
		if(normalGenerationMethod == SIMPLE)
		{
			volIter.setPosition(static_cast<uint16_t>(posX),static_cast<uint16_t>(posY),static_cast<uint16_t>(posZ));
			const uint8_t uFloor = volIter.getVoxel() > 0 ? 1 : 0;
			if((posX - floorX) > 0.25) //The result should be 0.0 or 0.5
			{					
				uint8_t uCeil = volIter.peekVoxel1px0py0pz() > 0 ? 1 : 0;
				result = Vector3DFloat(static_cast<float>(uFloor - uCeil),0.0,0.0);
			}
			else if((posY - floorY) > 0.25) //The result should be 0.0 or 0.5
			{
				uint8_t uCeil = volIter.peekVoxel0px1py0pz() > 0 ? 1 : 0;
				result = Vector3DFloat(0.0,static_cast<float>(uFloor - uCeil),0.0);
			}
			else if((posZ - floorZ) > 0.25) //The result should be 0.0 or 0.5
			{
				uint8_t uCeil = volIter.peekVoxel0px0py1pz() > 0 ? 1 : 0;
				result = Vector3DFloat(0.0, 0.0,static_cast<float>(uFloor - uCeil));					
			}
		}
		return result;
	}
}
