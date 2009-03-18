#include "OpenGLImmediateModeSupport.h"
#include "OpenGLSupport.h"

#include "PolyVoxCore/IndexedSurfacePatch.h"

using namespace PolyVox;
using namespace std;

void renderRegionImmediateMode(PolyVox::IndexedSurfacePatch& isp)
{
	const vector<SurfaceVertex>& vecVertices = isp.getVertices();
	const vector<uint32>& vecIndices = isp.getIndices();

	glBegin(GL_TRIANGLES);
	for(vector<uint32>::const_iterator iterIndex = vecIndices.begin(); iterIndex != vecIndices.end(); ++iterIndex)
	{
		const SurfaceVertex& vertex = vecVertices[*iterIndex];
		const Vector3DFloat& v3dVertexPos = vertex.getPosition();
		//const Vector3DFloat v3dRegionOffset(uRegionX * g_uRegionSideLength, uRegionY * g_uRegionSideLength, uRegionZ * g_uRegionSideLength);
		const Vector3DFloat v3dFinalVertexPos = v3dVertexPos + static_cast<Vector3DFloat>(isp.m_v3dRegionPosition);




		uint8 material = vertex.getMaterial() + 0.5;

		OpenGLColour colour = convertMaterialIDToColour(material);

		glColor3f(colour.red, colour.green, colour.blue);
		glNormal3f(vertex.getNormal().getX(), vertex.getNormal().getY(), vertex.getNormal().getZ());
		glVertex3f(v3dFinalVertexPos.getX(), v3dFinalVertexPos.getY(), v3dFinalVertexPos.getZ());


	}
	glEnd();
}