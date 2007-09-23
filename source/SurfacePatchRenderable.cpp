#include "SurfacePatchRenderable.h"

#include "OgreVertexIndexData.h"

namespace Ogre
{
	SurfacePatchRenderable::SurfacePatchRenderable(const String& material)
	{
	   mRenderOp.vertexData = new VertexData();
	   mRenderOp.indexData = new IndexData();

	   this->setMaterial(material);
	}

	SurfacePatchRenderable::~SurfacePatchRenderable(void)
	{
	   delete mRenderOp.vertexData;
	   delete mRenderOp.indexData;
	}

	void SurfacePatchRenderable::setGeometry(std::vector<SurfaceVertex> verticesToSet, std::vector<uint> indicesToSet)
	{
				
		//LogManager::getSingleton().logMessage("In setGeometry()");
	   // Initialization stuff
	   mRenderOp.vertexData->vertexCount = verticesToSet.size();
	   mRenderOp.vertexData->vertexStart = 0;
	   mRenderOp.operationType = RenderOperation::OT_TRIANGLE_LIST; // OT_LINE_LIST, OT_LINE_STRIP
	   mRenderOp.useIndexes = true;
	   mRenderOp.indexData->indexStart = 0;
	   mRenderOp.indexData->indexCount = indicesToSet.size();

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
	   int size = verticesToSet.size();
	   Vector3 vaabMin;
	   Vector3 vaabMax;
	   vaabMin.x = verticesToSet[0].getPosition().x/2.0f;
	   vaabMin.y = verticesToSet[0].getPosition().y/2.0f;
	   vaabMin.z = verticesToSet[0].getPosition().z/2.0f;
	   vaabMax.x = verticesToSet[0].getPosition().x/2.0f;
	   vaabMax.y = verticesToSet[0].getPosition().y/2.0f;
	   vaabMax.z = verticesToSet[0].getPosition().z/2.0f;

	   //LogManager::getSingleton().logMessage("Setting Vertex Data of size " + StringConverter::toString(size));

	   Real *prPos = static_cast<Real*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));

	   for(int i = 0; i < size; i++)
	   {
		  *prPos++ = verticesToSet[i].getPosition().x/2.0f;
		  *prPos++ = verticesToSet[i].getPosition().y/2.0f;
		  *prPos++ = verticesToSet[i].getPosition().z/2.0f;

		  *prPos++ = verticesToSet[i].getNormal().x;
		  *prPos++ = verticesToSet[i].getNormal().y;
		  *prPos++ = verticesToSet[i].getNormal().z;

		  *prPos++ = verticesToSet[i].alpha;

		  if(verticesToSet[i].getPosition().x < vaabMin.x)
			 vaabMin.x = verticesToSet[i].getPosition().x;
		  if(verticesToSet[i].getPosition().y < vaabMin.y)
			 vaabMin.y = verticesToSet[i].getPosition().y;
		  if(verticesToSet[i].getPosition().z < vaabMin.z)
			 vaabMin.z = verticesToSet[i].getPosition().z;

		  if(verticesToSet[i].getPosition().x > vaabMax.x)
			 vaabMax.x = verticesToSet[i].getPosition().x;
		  if(verticesToSet[i].getPosition().y > vaabMax.y)
			 vaabMax.y = verticesToSet[i].getPosition().y;
		  if(verticesToSet[i].getPosition().z > vaabMax.z)
			 vaabMax.z = verticesToSet[i].getPosition().z;
	   }

	   vbuf->unlock();

	   vaabMin /= 2.0f;
	   vaabMax /= 2.0f;

	   mBox.setExtents(vaabMin, vaabMax);

	   unsigned short* pIdx = static_cast<unsigned short*>(ibuf->lock(HardwareBuffer::HBL_DISCARD));
	   for(int i = 0; i < indicesToSet.size(); i++)
	   {
		   *pIdx = indicesToSet[i];
		   pIdx++;
		   /**pIdx = indicesToSet[i].v1;
		   pIdx++;
		   *pIdx = indicesToSet[i].v2;
		   pIdx++;*/
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
