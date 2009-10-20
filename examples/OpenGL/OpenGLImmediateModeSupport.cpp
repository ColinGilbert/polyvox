#include "OpenGLImmediateModeSupport.h"
#include "OpenGLSupport.h"

#include "IndexedSurfacePatch.h"

using namespace PolyVox;
using namespace std;

void renderRegionImmediateMode(PolyVox::IndexedSurfacePatch& isp, unsigned int uLodLevel)
{
	const vector<SurfaceVertex>& vecVertices = isp.getVertices();
	const vector<PolyVox::uint32_t>& vecIndices = isp.getIndices();

	int beginIndex = isp.m_vecLodRecords[uLodLevel].beginIndex;
	int endIndex = isp.m_vecLodRecords[uLodLevel].endIndex;

	glBegin(GL_TRIANGLES);
	//for(vector<PolyVox::uint32_t>::const_iterator iterIndex = vecIndices.begin(); iterIndex != vecIndices.end(); ++iterIndex)
	for(int index = beginIndex; index < endIndex; ++index)
	{
		const SurfaceVertex& vertex = vecVertices[vecIndices[index]];
		const Vector3DFloat& v3dVertexPos = vertex.getPosition();
		//const Vector3DFloat v3dRegionOffset(uRegionX * g_uRegionSideLength, uRegionY * g_uRegionSideLength, uRegionZ * g_uRegionSideLength);
		const Vector3DFloat v3dFinalVertexPos = v3dVertexPos + static_cast<Vector3DFloat>(isp.m_Region.getLowerCorner());




		PolyVox::uint8_t material = vertex.getMaterial() + 0.5;

		OpenGLColour colour = convertMaterialIDToColour(material);

		glColor3f(colour.red, colour.green, colour.blue);
		glNormal3f(vertex.getNormal().getX(), vertex.getNormal().getY(), vertex.getNormal().getZ());
		glVertex3f(v3dFinalVertexPos.getX(), v3dFinalVertexPos.getY(), v3dFinalVertexPos.getZ());


	}
	glEnd();
}