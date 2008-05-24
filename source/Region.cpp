#include "Region.h"

namespace PolyVox
{
	Region::Region()
		:m_v3dLowerCorner(0,0,0)
		,m_v3dUpperCorner(0,0,0)
	{
	}

	Region::Region(const Vector3DInt32& v3dLowerCorner, const Vector3DInt32& v3dUpperCorner)
		:m_v3dLowerCorner(v3dLowerCorner)
		,m_v3dUpperCorner(v3dUpperCorner)
	{
	}

	const Vector3DInt32& Region::getLowerCorner(void) const
	{
		return m_v3dLowerCorner;
	}

	const Vector3DInt32& Region::getUpperCorner(void) const
	{
		return m_v3dUpperCorner;
	}	

	void Region::setLowerCorner(const Vector3DInt32& v3dLowerCorner)
	{
		m_v3dLowerCorner = v3dLowerCorner;
	}

	void Region::setUpperCorner(const Vector3DInt32& v3dUpperCorner)
	{
		m_v3dUpperCorner = v3dUpperCorner;
	}

	bool Region::containsPoint(const Vector3DFloat& pos, float boundary) const
	{
		return (pos.x() <= m_v3dUpperCorner.x() - boundary)
			&& (pos.y() <= m_v3dUpperCorner.y() - boundary) 
			&& (pos.z() <= m_v3dUpperCorner.z() - boundary)
			&& (pos.x() >= m_v3dLowerCorner.x() + boundary)
			&& (pos.y() >= m_v3dLowerCorner.y() + boundary)
			&& (pos.z() >= m_v3dLowerCorner.z() + boundary);
	}

	bool Region::containsPoint(const Vector3DInt32& pos, boost::uint8_t boundary) const
	{
		return (pos.x() <= m_v3dUpperCorner.x() - boundary)
			&& (pos.y() <= m_v3dUpperCorner.y() - boundary) 
			&& (pos.z() <= m_v3dUpperCorner.z() - boundary)
			&& (pos.x() >= m_v3dLowerCorner.x() + boundary)
			&& (pos.y() >= m_v3dLowerCorner.y() + boundary)
			&& (pos.z() >= m_v3dLowerCorner.z() + boundary);
	}
}