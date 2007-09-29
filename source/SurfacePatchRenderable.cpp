#include "SurfacePatchRenderable.h"

#include "SurfaceEdge.h"
#include "OgreVertexIndexData.h"

namespace Ogre
{
	SurfacePatchRenderable::SurfacePatchRenderable(SurfacePatch& patchToRender, const String& material)
	{
		mRenderOp.vertexData = new VertexData();
		mRenderOp.indexData = new IndexData();

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


		//LogManager::getSingleton().logMessage("In setGeometry()");
		//Initialization stuff
		mRenderOp.vertexData->vertexCount = patchToRender.getNoOfVertices();
		mRenderOp.vertexData->vertexStart = 0;
		mRenderOp.operationType = RenderOperation::OT_TRIANGLE_LIST; // OT_LINE_LIST, OT_LINE_STRIP
		mRenderOp.useIndexes = true;
		mRenderOp.indexData->indexStart = 0;
		mRenderOp.indexData->indexCount = patchToRender.getNoOfTriangles() * 3;

		//LogManager::getSingleton().logMessage("Finished initialisaing stuff");

		VertexDeclaration *decl = mRenderOp.vertexData->vertexDeclaration;
		VertexBufferBinding *bind = mRenderOp.vertexData->vertexBufferBinding;

		//FIXME - this should be moved to constructor?
		//LogManager::getSingleton().logMessage("Creating Vertex Declaration");
		decl->removeAllElements();
		decl->addElement(0, 0, VET_FLOAT3, VES_POSITION);
		decl->addElement(0, 3 * sizeof(float), VET_FLOAT3, VES_NORMAL);
		decl->addElement(0, 6 * sizeof(float), VET_FLOAT1, VES_TEXTURE_COORDINATES);

		//LogManager::getSingleton().logMessage("Creating Vertex Buffer");
		HardwareVertexBufferSharedPtr vbuf =
			HardwareBufferManager::getSingleton().createVertexBuffer(
			decl->getVertexSize(0),
			mRenderOp.vertexData->vertexCount,
			HardwareBuffer::HBU_STATIC_WRITE_ONLY);

		bind->setBinding(0, vbuf);

		//LogManager::getSingleton().logMessage("Creating Index Buffer");
		HardwareIndexBufferSharedPtr ibuf =
			HardwareBufferManager::getSingleton().createIndexBuffer(
			HardwareIndexBuffer::IT_16BIT, // type of index
			mRenderOp.indexData->indexCount, // number of indexes
			HardwareBuffer::HBU_STATIC_WRITE_ONLY, // usage
			false); // no shadow buffer	

		mRenderOp.indexData->indexBuffer = ibuf;		

		// Drawing stuff
		//int size = verticesToSet.size();
		Vector3 vaabMin;
		Vector3 vaabMax;
		vaabMin.x = patchToRender.getVerticesBegin()->getPosition().x/2.0f;
		vaabMin.y = patchToRender.getVerticesBegin()->getPosition().y/2.0f;
		vaabMin.z = patchToRender.getVerticesBegin()->getPosition().z/2.0f;
		vaabMax.x = patchToRender.getVerticesBegin()->getPosition().x/2.0f;
		vaabMax.y = patchToRender.getVerticesBegin()->getPosition().y/2.0f;
		vaabMax.z = patchToRender.getVerticesBegin()->getPosition().z/2.0f;

		//LogManager::getSingleton().logMessage("Setting Vertex Data of size " + StringConverter::toString(size));

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

			if(vertexIter->getPosition().x < vaabMin.x)
				vaabMin.x = vertexIter->getPosition().x;
			if(vertexIter->getPosition().y < vaabMin.y)
				vaabMin.y = vertexIter->getPosition().y;
			if(vertexIter->getPosition().z < vaabMin.z)
				vaabMin.z = vertexIter->getPosition().z;

			if(vertexIter->getPosition().x > vaabMax.x)
				vaabMax.x = vertexIter->getPosition().x;
			if(vertexIter->getPosition().y > vaabMax.y)
				vaabMax.y = vertexIter->getPosition().y;
			if(vertexIter->getPosition().z > vaabMax.z)
				vaabMax.z = vertexIter->getPosition().z;
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

		std::vector<SurfaceVertex> vertexData;
		vertexData.resize(patchToRender.getNoOfVertices());
		std::copy(patchToRender.getVerticesBegin(), patchToRender.getVerticesEnd(), vertexData.begin());

		for(SurfaceTriangleConstIterator iterTriangles = patchToRender.getTrianglesBegin(); iterTriangles != patchToRender.getTrianglesEnd(); ++iterTriangles)
		{		
			std::vector<SurfaceVertex>::iterator iterVertex;
			SurfaceEdgeIterator edgeIter;
			
			edgeIter = iterTriangles->getEdge();
			iterVertex = find(vertexData.begin(), vertexData.end(), *(edgeIter->getTarget()));
			*pIdx = (iterVertex - vertexData.begin());
			//LogManager::getSingleton().logMessage("Wrong pIdx =   " + StringConverter::toString(*pIdx));
			pIdx++;

			edgeIter = edgeIter->getNextHalfEdge();
			iterVertex = find(vertexData.begin(), vertexData.end(), *(edgeIter->getTarget()));
			*pIdx = (iterVertex - vertexData.begin());
			//LogManager::getSingleton().logMessage("Wrong pIdx =   " + StringConverter::toString(*pIdx));
			pIdx++;

			edgeIter = edgeIter->getNextHalfEdge();
			iterVertex = find(vertexData.begin(), vertexData.end(), *(edgeIter->getTarget()));
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
