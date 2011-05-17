/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution. 	
*******************************************************************************/

#include "PolyVoxCore/Region.h"

#include <limits>

namespace PolyVox
{
	const Region Region::MaxRegion
	(
		Vector3DInt32((std::numeric_limits<int32_t>::min)(), (std::numeric_limits<int32_t>::min)(), (std::numeric_limits<int32_t>::min)()),
		Vector3DInt32((std::numeric_limits<int32_t>::max)(), (std::numeric_limits<int32_t>::max)(), (std::numeric_limits<int32_t>::max)())
	);


	Region::Region()
		:m_v3dLowerCorner(0,0,0)
		,m_v3dUpperCorner(0,0,0)
	{
	}

	Region::Region(const Vector3DInt32& v3dLowerCorner, const Vector3DInt32& v3dUpperCorner)
		:m_v3dLowerCorner(v3dLowerCorner)
		,m_v3dUpperCorner(v3dUpperCorner)
	{
		//Check the region is valid.
		assert(m_v3dUpperCorner.getX() >= m_v3dLowerCorner.getX());
		assert(m_v3dUpperCorner.getY() >= m_v3dLowerCorner.getY());
		assert(m_v3dUpperCorner.getZ() >= m_v3dLowerCorner.getZ());
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

	bool Region::containsPoint(const Vector3DInt32& pos, uint8_t boundary) const
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

	int32_t Region::depth(void) const
	{
		//This function is deprecated and wrong.
		assert(false);
		return m_v3dUpperCorner.getZ() - m_v3dLowerCorner.getZ();
	}

	int32_t Region::height(void) const
	{
		//This function is deprecated and wrong.
		assert(false);
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
		//This function is deprecated and wrong.
		assert(false);
		return m_v3dUpperCorner - m_v3dLowerCorner;
	}

	int32_t Region::width(void) const
	{
		//This function is deprecated and wrong.
		assert(false);
		return m_v3dUpperCorner.getX() - m_v3dLowerCorner.getX();
	}
}
