#include "SurfacePatchRenderable.h"

#include "SurfaceEdge.h"
#include "OgreVertexIndexData.h"

namespace Ogre
{
	SurfacePatchRenderable::SurfacePatchRenderable(SurfacePatch& patchToRender, const String& material)
	{
		//Set up what we can of the vertex data
		mRenderOp.vertexData = new VertexData();
		mRenderOp.vertexData->vertexStart = 0;
		mRenderOp.vertexData->vertexCount = 0;
		mRenderOp.operationType = RenderOperation::OT_TRIANGLE_LIST;

		//Set up what we can of the index data
		mRenderOp.indexData = new IndexData();
		mRenderOp.useIndexes = true;
		mRenderOp.indexData->indexStart = 0;
		mRenderOp.indexData->indexCount = 0;

		//Set up the vertex declaration
		VertexDeclaration *decl = mRenderOp.vertexData->vertexDeclaration;
		decl->removeAllElements();
		decl->addElement(0, 0, VET_FLOAT3, VES_POSITION);
		decl->addElement(0, 3 * sizeof(float), VET_FLOAT3, VES_NORMAL);
		decl->addElement(0, 6 * sizeof(float), VET_FLOAT1, VES_TEXTURE_COORDINATES);
		

		this->setMaterial(material);

		setGeometry(patchToRender);
	}

	SurfacePatchRenderable::~SurfacePatchRenderable(void)
	{
		delete mRenderOp.vertexData;
		delete mRenderOp.indexData;
	}

	void SurfacePatchRenderable::updateWithNewSurfacePatch(SurfacePatch& patchToRender)
	{		
		setGeometry(patchToRender);
	}

	void SurfacePatchRenderable::setGeometry(SurfacePatch& patchToRender)
	{
		//Initialization stuff
		mRenderOp.vertexData->vertexCount = patchToRender.getNoOfVertices();		
		mRenderOp.indexData->indexCount = patchToRender.getNoOfTriangles() * 3;

		
		VertexBufferBinding *bind = mRenderOp.vertexData->vertexBufferBinding;

		
		

		//LogManager::getSingleton().logMessage("Creating Vertex Buffer");
		HardwareVertexBufferSharedPtr vbuf =
			HardwareBufferManager::getSingleton().createVertexBuffer(
			mRenderOp.vertexData->vertexDeclaration->getVertexSize(0),
			mRenderOp.vertexData->vertexCount,
			HardwareBuffer::HBU_STATIC_WRITE_ONLY,
			false);

		bind->setBinding(0, vbuf);

		//LogManager::getSingleton().logMessage("Creating Index Buffer");
		HardwareIndexBufferSharedPtr ibuf =
			HardwareBufferManager::getSingleton().createIndexBuffer(
			HardwareIndexBuffer::IT_16BIT, // type of index
			mRenderOp.indexData->indexCount, // number of indexes
			HardwareBuffer::HBU_STATIC_WRITE_ONLY, // usage
			false); // no shadow buffer	

		mRenderOp.indexData->indexBuffer = ibuf;	

		std::vector<SurfaceVertex> vertexData;
		vertexData.resize(patchToRender.getNoOfVertices());
		std::copy(patchToRender.getVerticesBegin(), patchToRender.getVerticesEnd(), vertexData.begin());

		// Drawing stuff
		Vector3 vaabMin = vertexData[0].getPosition().toOgreVector3();
		Vector3 vaabMax = vertexData[vertexData.size()-1].getPosition().toOgreVector3();

		
		Real *prPos = static_cast<Real*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));

		//for(int i = 0; i < size; i++)		
		for(SurfaceVertexConstIterator vertexIter = patchToRender.getVerticesBegin(); vertexIter != patchToRender.getVerticesEnd(); ++vertexIter)
		{
			*prPos++ = vertexIter->getPosition().x/2.0f;
			*prPos++ = vertexIter->getPosition().y/2.0f;
			*prPos++ = vertexIter->getPosition().z/2.0f;

			*prPos++ = vertexIter->getNormal().x;
			*prPos++ = vertexIter->getNormal().y;
			*prPos++ = vertexIter->getNormal().z;

			*prPos++ = vertexIter->getAlpha();
		}		

		vbuf->unlock();

		vaabMin /= 2.0f;
		vaabMax /= 2.0f;

		mBox.setExtents(vaabMin, vaabMax);

		/*std::vector<SurfaceVertex> verticesToSet;
		std::vector<uint> indicesToSet;
		patchToRender.getVertexAndIndexData(verticesToSet, indicesToSet);*/

		unsigned short* pIdx = static_cast<unsigned short*>(ibuf->lock(HardwareBuffer::HBL_DISCARD));
		/*for(int i = 0; i < indicesToSet.size(); i++)
		{
			*pIdx = indicesToSet[i];
			LogManager::getSingleton().logMessage("Correct pIdx = " + StringConverter::toString(*pIdx));
			pIdx++;
		}*/		

		for(SurfaceTriangleConstIterator iterTriangles = patchToRender.getTrianglesBegin(); iterTriangles != patchToRender.getTrianglesEnd(); ++iterTriangles)
		{		
			std::vector<SurfaceVertex>::iterator iterVertex;
			SurfaceEdgeIterator edgeIter;
			
			edgeIter = iterTriangles->getEdge();
			iterVertex = lower_bound(vertexData.begin(), vertexData.end(), *(edgeIter->getTarget()));
			*pIdx = (iterVertex - vertexData.begin());
			//LogManager::getSingleton().logMessage("Wrong pIdx =   " + StringConverter::toString(*pIdx));
			pIdx++;

			edgeIter = edgeIter->getNextHalfEdge();
			iterVertex = lower_bound(vertexData.begin(), vertexData.end(), *(edgeIter->getTarget()));
			*pIdx = (iterVertex - vertexData.begin());
			//LogManager::getSingleton().logMessage("Wrong pIdx =   " + StringConverter::toString(*pIdx));
			pIdx++;

			edgeIter = edgeIter->getNextHalfEdge();
			iterVertex = lower_bound(vertexData.begin(), vertexData.end(), *(edgeIter->getTarget()));
			*pIdx = (iterVertex - vertexData.begin());
			//LogManager::getSingleton().logMessage("Wrong pIdx =   " + StringConverter::toString(*pIdx));
			pIdx++;	
		}
		ibuf->unlock();
	}

	Real SurfacePatchRenderable::getSquaredViewDepth(const Camera *cam) const
	{
		Vector3 vMin, vMax, vMid, vDist;
		vMin = mBox.getMinimum();
		vMax = mBox.getMaximum();
		vMid = ((vMin - vMax) * 0.5) + vMin;
		vDist = cam->getDerivedPosition() - vMid;

		return vDist.squaredLength();
	}

	Real SurfacePatchRenderable::getBoundingRadius(void) const
	{
		return Math::Sqrt((std::max)(mBox.getMaximum().squaredLength(), mBox.getMinimum().squaredLength()));
		//return mRadius;
	}
	/*
	void Line3D::getWorldTransforms(Matrix4 *xform) const
	{
	// return identity matrix to prevent parent transforms
	*xform = Matrix4::IDENTITY;
	}
	*/
	const Quaternion &SurfacePatchRenderable::getWorldOrientation(void) const
	{
		return Quaternion::IDENTITY;
	}

	const Vector3 &SurfacePatchRenderable::getWorldPosition(void) const
	{
		return Vector3::ZERO;
	}
}
