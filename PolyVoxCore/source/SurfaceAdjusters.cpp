#include "SurfaceAdjusters.h"

#include "IndexedSurfacePatch.h"
#include "RegionGeometry.h"

#include <vector>

namespace PolyVox
{
	void smoothRegionGeometry(BlockVolume<boost::uint8_t>* volumeData, RegionGeometry& regGeom)
	{
		std::vector<SurfaceVertex>& vecVertices = regGeom.m_patchSingleMaterial->m_vecVertices;
		std::vector<SurfaceVertex>::iterator iterSurfaceVertex = vecVertices.begin();
		while(iterSurfaceVertex != vecVertices.end())
		{
			iterSurfaceVertex->setPosition(iterSurfaceVertex->getPosition() + iterSurfaceVertex->getNormal());
			//iterSurfaceVertex->setPosition(iterSurfaceVertex->getPosition() + Vector3DFloat(10.0f,0.0f,0.0f));
			++iterSurfaceVertex;
		}
	}
}