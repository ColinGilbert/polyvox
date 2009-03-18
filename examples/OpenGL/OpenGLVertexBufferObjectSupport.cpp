#include "OpenGLVertexBufferObjectSupport.h"

#include "PolyVoxCore/IndexedSurfacePatch.h"

using namespace PolyVox;
using namespace std;

OpenGLSurfacePatch BuildOpenGLSurfacePatch(IndexedSurfacePatch& isp)
{
	OpenGLSurfacePatch result;

	const vector<SurfaceVertex>& vecVertices = isp.getVertices();
	const vector<uint32>& vecIndices = isp.getIndices();

	glGenBuffers(1, &result.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.indexBuffer);
	int s = vecIndices.size() * sizeof(GLint);
	if(s != 0)
	{
		GLvoid* blah = (GLvoid*)(&(vecIndices[0]));				
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, s, blah, GL_STATIC_DRAW);
	}

	result.noOfIndices = vecIndices.size();

	glGenBuffers(1, &result.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, result.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vecVertices.size() * sizeof(GLfloat) * 9, 0, GL_STATIC_DRAW);
	GLfloat* ptr = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

	for(vector<SurfaceVertex>::const_iterator iterVertex = vecVertices.begin(); iterVertex != vecVertices.end(); ++iterVertex)
	{
		const SurfaceVertex& vertex = *iterVertex;
		const Vector3DFloat& v3dVertexPos = vertex.getPosition();
		//const Vector3DFloat v3dRegionOffset(uRegionX * g_uRegionSideLength, uRegionY * g_uRegionSideLength, uRegionZ * g_uRegionSideLength);
		const Vector3DFloat v3dFinalVertexPos = v3dVertexPos + static_cast<Vector3DFloat>(isp.m_v3dRegionPosition);

		*ptr = v3dFinalVertexPos.getX();
		ptr++;
		*ptr = v3dFinalVertexPos.getY();
		ptr++;
		*ptr = v3dFinalVertexPos.getZ();
		ptr++;

		*ptr = vertex.getNormal().getX();
		ptr++;
		*ptr = vertex.getNormal().getY();
		ptr++;
		*ptr = vertex.getNormal().getZ();
		ptr++;

		GLfloat red = 0.0f;
		GLfloat green = 0.0f;
		GLfloat blue = 0.0f;

		uint8 material = vertex.getMaterial() + 0.5;

		switch(material)
		{
		case 1:
			red = 1.0;
			green = 0.0;
			blue = 0.0;
			break;
		case 2:
			red = 0.0;
			green = 1.0;
			blue = 0.0;
			break;
		case 3:
			red = 0.0;
			green = 0.0;
			blue = 1.0;
			break;
		case 4:
			red = 1.0;
			green = 1.0;
			blue = 0.0;
			break;
		case 5:
			red = 1.0;
			green = 0.0;
			blue = 1.0;
			break;
		}

		*ptr = red;
		ptr++;
		*ptr = green;
		ptr++;
		*ptr = blue;
		ptr++;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	return result;
}

void renderRegionVertexBufferObject(const OpenGLSurfacePatch& openGLSurfacePatch)
{
	glBindBuffer(GL_ARRAY_BUFFER, openGLSurfacePatch.vertexBuffer);
	glVertexPointer(3, GL_FLOAT, 36, 0);
	glNormalPointer(GL_FLOAT, 36, (GLvoid*)12);
	glColorPointer(3, GL_FLOAT, 36, (GLvoid*)24);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openGLSurfacePatch.indexBuffer);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glDrawElements(GL_TRIANGLES, openGLSurfacePatch.noOfIndices, GL_UNSIGNED_INT, 0);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}