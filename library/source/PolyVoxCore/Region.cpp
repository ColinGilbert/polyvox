#include "PolyVoxCore/Region.h"

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
		return (pos.getX() <= m_v3dUpperCorner.getX() - boundary)
			&& (pos.getY() <= m_v3dUpperCorner.getY() - boundary)
			&& (pos.getZ() <= m_v3dUpperCorner.getZ() - boundary)
			&& (pos.getX() >= m_v3dLowerCorner.getX() + boundary)
			&& (pos.getY() >= m_v3dLowerCorner.getY() + boundary)
			&& (pos.getZ() >= m_v3dLowerCorner.getZ() + boundary);
	}

	bool Region::containsPoint(const Vector3DInt32& pos, uint8 boundary) const
	{
		return (pos.getX() <= m_v3dUpperCorner.getX() - boundary)
			&& (pos.getY() <= m_v3dUpperCorner.getY() - boundary) 
			&& (pos.getZ() <= m_v3dUpperCorner.getZ() - boundary)
			&& (pos.getX() >= m_v3dLowerCorner.getX() + boundary)
			&& (pos.getY() >= m_v3dLowerCorner.getY() + boundary)
			&& (pos.getZ() >= m_v3dLowerCorner.getZ() + boundary);
	}

	void Region::cropTo(const Region& other)
	{
		m_v3dLowerCorner.setX((std::max)(m_v3dLowerCorner.getX(), other.m_v3dLowerCorner.getX()));
		m_v3dLowerCorner.setY((std::max)(m_v3dLowerCorner.getY(), other.m_v3dLowerCorner.getY()));
		m_v3dLowerCorner.setZ((std::max)(m_v3dLowerCorner.getZ(), other.m_v3dLowerCorner.getZ()));
		m_v3dUpperCorner.setX((std::min)(m_v3dUpperCorner.getX(), other.m_v3dUpperCorner.getX()));
		m_v3dUpperCorner.setY((std::min)(m_v3dUpperCorner.getY(), other.m_v3dUpperCorner.getY()));
		m_v3dUpperCorner.setZ((std::min)(m_v3dUpperCorner.getZ(), other.m_v3dUpperCorner.getZ()));
	}

	int32 Region::depth(void) const
	{
		return m_v3dUpperCorner.getZ() - m_v3dLowerCorner.getZ();
	}

	int32 Region::height(void) const
	{
		return m_v3dUpperCorner.getY() - m_v3dLowerCorner.getY();
	}

	void Region::shift(const Vector3DInt32& amount)
	{
		m_v3dLowerCorner += amount;
		m_v3dUpperCorner += amount;
	}

	void Region::shiftLowerCorner(const Vector3DInt32& amount)
	{
		m_v3dLowerCorner += amount;
	}

	void Region::shiftUpperCorner(const Vector3DInt32& amount)
	{
		m_v3dUpperCorner += amount;
	}

	Vector3DInt32 Region::dimensions(void)
	{
		return m_v3dUpperCorner - m_v3dLowerCorner;
	}

	int32 Region::width(void) const
	{
		return m_v3dUpperCorner.getX() - m_v3dLowerCorner.getX();
	}
}
