#include "SurfacePatchRenderable.h"

#include "SurfaceEdge.h"
#include "SurfaceVertex.h"
#include "OgreVertexIndexData.h"

#include <limits>

namespace Ogre
{
	SurfacePatchRenderable::SurfacePatchRenderable(const String& name, IndexedSurfacePatch* patchToRender, const String& material)
		:SimpleRenderable(name)
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
		decl->addElement(0, 6 * sizeof(float), VET_FLOAT2, VES_TEXTURE_COORDINATES);
		

		this->setMaterial(material);

		setGeometry(patchToRender);
	}

	SurfacePatchRenderable::~SurfacePatchRenderable(void)
	{
		delete mRenderOp.vertexData;
		delete mRenderOp.indexData;
	}

	void SurfacePatchRenderable::updateWithNewSurfacePatch(IndexedSurfacePatch* patchToRender)
	{
		setGeometry(patchToRender);
	}

	void SurfacePatchRenderable::setGeometry(IndexedSurfacePatch* patchToRender)
	{
		std::vector<SurfaceVertex> vecVertices;
		std::vector<ushort> vecIndices;
		patchToRender->fillVertexAndIndexData(vecVertices,vecIndices);

		//LogManager::getSingleton().logMessage("No of Vertices = " + StringConverter::toString(vecVertices.size()));
		//LogManager::getSingleton().logMessage("No of Indices = " + StringConverter::toString(vecIndices.size()));

		//Initialization stuff
		mRenderOp.vertexData->vertexCount = vecVertices.size();		
		mRenderOp.indexData->indexCount = vecIndices.size();
		
		VertexBufferBinding *bind = mRenderOp.vertexData->vertexBufferBinding;

		HardwareVertexBufferSharedPtr vbuf =
			HardwareBufferManager::getSingleton().createVertexBuffer(
			mRenderOp.vertexData->vertexDeclaration->getVertexSize(0),
			mRenderOp.vertexData->vertexCount,
			HardwareBuffer::HBU_STATIC_WRITE_ONLY,
			false);

		bind->setBinding(0, vbuf);

		HardwareIndexBufferSharedPtr ibuf =
			HardwareBufferManager::getSingleton().createIndexBuffer(
			HardwareIndexBuffer::IT_16BIT, // type of index
			mRenderOp.indexData->indexCount, // number of indexes
			HardwareBuffer::HBU_STATIC_WRITE_ONLY, // usage
			false); // no shadow buffer	

		mRenderOp.indexData->indexBuffer = ibuf;	

		// Drawing stuff
		Vector3 vaabMin(std::numeric_limits<Real>::max(),std::numeric_limits<Real>::max(),std::numeric_limits<Real>::max());
		Vector3 vaabMax(0.0,0.0,0.0);
		
		Real *prPos = static_cast<Real*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));

		for(std::vector<SurfaceVertex>::iterator vertexIter = vecVertices.begin(); vertexIter != vecVertices.end(); ++vertexIter)
		{
			*prPos++ = vertexIter->getPosition().x/2.0f;
			*prPos++ = vertexIter->getPosition().y/2.0f;
			*prPos++ = vertexIter->getPosition().z/2.0f;

			*prPos++ = vertexIter->getNormal().x;
			*prPos++ = vertexIter->getNormal().y;
			*prPos++ = vertexIter->getNormal().z;

			*prPos++ = vertexIter->getMaterial();

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
		
		unsigned short* pIdx = static_cast<unsigned short*>(ibuf->lock(HardwareBuffer::HBL_DISCARD));
		//for(int i = 0; i < indexData.size(); i++)
		for(std::vector<ushort>::iterator indexIter = vecIndices.begin(); indexIter != vecIndices.end(); ++indexIter)
		{
			//*pIdx = indexData[i];
			*pIdx = *indexIter;
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

	const String& SurfacePatchRenderable::getMovableType(void) const
    {
        static String movType = "SurfacePatchRenderable";
        return movType;
    }

	//-----------------------------------------------------------------------
	/*String LightFactory::FACTORY_TYPE_NAME = "SurfacePatchRenderable";
	//-----------------------------------------------------------------------
	const String& SimplePatchRenderableFactory::getType(void) const
	{
		return FACTORY_TYPE_NAME;
	}
	//-----------------------------------------------------------------------
	MovableObject* SimplePatchRenderableFactory::createInstanceImpl( const String& name, 
		const NameValuePairList* params)
	{

		return new SurfacePatchRenderable(name);

	}
	//-----------------------------------------------------------------------
	void SimplePatchRenderableFactory::destroyInstance( MovableObject* obj)
	{
		delete obj;
	}*/
}
