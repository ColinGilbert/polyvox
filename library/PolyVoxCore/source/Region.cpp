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

	const Region Region::InvertedRegion
	(
		Vector3DInt32((std::numeric_limits<int32_t>::max)(), (std::numeric_limits<int32_t>::max)(), (std::numeric_limits<int32_t>::max)()),
		Vector3DInt32((std::numeric_limits<int32_t>::min)(), (std::numeric_limits<int32_t>::min)(), (std::numeric_limits<int32_t>::min)())
	);

	Region::Region()
		:m_iLowerX(0)
		,m_iLowerY(0)
		,m_iLowerZ(0)
		,m_iUpperX(0)
		,m_iUpperY(0)
		,m_iUpperZ(0)
	{
	}

	Region::Region(const Vector3DInt32& v3dLowerCorner, const Vector3DInt32& v3dUpperCorner)
		:m_iLowerX(v3dLowerCorner.getX())
		,m_iLowerY(v3dLowerCorner.getY())
		,m_iLowerZ(v3dLowerCorner.getZ())
		,m_iUpperX(v3dUpperCorner.getX())
		,m_iUpperY(v3dUpperCorner.getY())
		,m_iUpperZ(v3dUpperCorner.getZ())
	{
	}

	Region::Region(int32_t iLowerX, int32_t iLowerY, int32_t iLowerZ, int32_t iUpperX, int32_t iUpperY, int32_t iUpperZ)
		:m_iLowerX(iLowerX)
		,m_iLowerY(iLowerY)
		,m_iLowerZ(iLowerZ)
		,m_iUpperX(iUpperX)
		,m_iUpperY(iUpperY)
		,m_iUpperZ(iUpperZ)
	{
	}

	/**
    Checks whether two Regions are equal.
    \param rhs The Region to compare to.
    \return true if the Regions match.
    \see operator!=
    */
    bool Region::operator==(const Region& rhs) const
    {
		return ((m_iLowerX == rhs.m_iLowerX) && (m_iLowerY == rhs.m_iLowerY) && (m_iLowerZ == rhs.m_iLowerZ)
			&&  (m_iUpperX == rhs.m_iUpperX) && (m_iUpperY == rhs.m_iUpperY) && (m_iUpperZ == rhs.m_iUpperZ));
    }

	/**
    Checks whether two Regions are not equal.
    \param rhs The Region to compare to.
    \return true if the Regions do not match.
    \see operator==
    */
    bool Region::operator!=(const Region& rhs) const
    {
		return !(*this == rhs);
    }

	int32_t Region::getLowerX(void) const
	{
		return m_iLowerX;
	}

	int32_t Region::getLowerY(void) const
	{
		return m_iLowerY;
	}

	int32_t Region::getLowerZ(void) const
	{
		return m_iLowerZ;
	}

	int32_t Region::getUpperX(void) const
	{
		return m_iUpperX;
	}

	int32_t Region::getUpperY(void) const
	{
		return m_iUpperY;
	}

	int32_t Region::getUpperZ(void) const
	{
		return m_iUpperZ;
	}

	Vector3DInt32 Region::getLowerCorner(void) const
	{
		return Vector3DInt32(m_iLowerX, m_iLowerY, m_iLowerZ);
	}

	Vector3DInt32 Region::getUpperCorner(void) const
	{
		return Vector3DInt32(m_iUpperX, m_iUpperY, m_iUpperZ);
	}

	int32_t Region::getWidthInVoxels(void) const
	{
		return getWidthInCells() + 1;
	}

	int32_t Region::getHeightInVoxels(void) const
	{
		return getHeightInCells() + 1;
	}

	int32_t Region::getDepthInVoxels(void) const
	{
		return getDepthInCells() + 1;
	}

	Vector3DInt32 Region::getDimensionsInVoxels(void) const
	{
		return getDimensionsInCells() + Vector3DInt32(1, 1, 1);
	}

	int32_t Region::getWidthInCells(void) const
	{
		return m_iUpperX - m_iLowerX;
	}

	int32_t Region::getHeightInCells(void) const
	{
		return m_iUpperY - m_iLowerY;
	}

	Vector3DInt32 Region::getDimensionsInCells(void) const
	{
		return Vector3DInt32(getWidthInCells(), getHeightInCells(), getDepthInCells());
	}

	int32_t Region::getDepthInCells(void) const
	{
		return m_iUpperZ - m_iLowerZ;
	}

	bool Region::isValid(void)
	{
		return (m_iUpperX >= m_iLowerX) && (m_iUpperY >= m_iLowerY) && (m_iUpperZ >= m_iLowerZ);
	}

	void Region::setLowerX(int32_t iX)
	{
		m_iLowerX = iX;
	}

	void Region::setLowerY(int32_t iY)
	{
		m_iLowerY = iY;
	}

	void Region::setLowerZ(int32_t iZ)
	{
		m_iLowerZ = iZ;
	}

	void Region::setUpperX(int32_t iX)
	{
		m_iUpperX = iX;
	}

	void Region::setUpperY(int32_t iY)
	{
		m_iUpperY = iY;
	}

	void Region::setUpperZ(int32_t iZ)
	{
		m_iUpperZ = iZ;
	}

	void Region::setLowerCorner(const Vector3DInt32& v3dLowerCorner)
	{
		m_iLowerX = v3dLowerCorner.getX();
		m_iLowerY = v3dLowerCorner.getY();
		m_iLowerZ = v3dLowerCorner.getZ();
	}

	void Region::setUpperCorner(const Vector3DInt32& v3dUpperCorner)
	{
		m_iUpperX = v3dUpperCorner.getX();
		m_iUpperY = v3dUpperCorner.getY();
		m_iUpperZ = v3dUpperCorner.getZ();
	}

	bool Region::containsPoint(const Vector3DFloat& pos, float boundary) const
	{
		return (pos.getX() <= m_iUpperX - boundary)
			&& (pos.getY() <= m_iUpperY - boundary)
			&& (pos.getZ() <= m_iUpperZ - boundary)
			&& (pos.getX() >= m_iLowerX + boundary)
			&& (pos.getY() >= m_iLowerY + boundary)
			&& (pos.getZ() >= m_iLowerZ + boundary);
	}

	bool Region::containsPoint(const Vector3DInt32& pos, uint8_t boundary) const
	{
		return (pos.getX() <= m_iUpperX - boundary)
			&& (pos.getY() <= m_iUpperY - boundary) 
			&& (pos.getZ() <= m_iUpperZ - boundary)
			&& (pos.getX() >= m_iLowerX + boundary)
			&& (pos.getY() >= m_iLowerY + boundary)
			&& (pos.getZ() >= m_iLowerZ + boundary);
	}

	bool Region::containsPointInX(float pos, float boundary) const
	{
		return (pos <= m_iUpperX - boundary)
			&& (pos >= m_iLowerX + boundary);
	}

	bool Region::containsPointInX(int32_t pos, uint8_t boundary) const
	{
		return (pos <= m_iUpperX - boundary)
			&& (pos >= m_iLowerX + boundary);
	}

	bool Region::containsPointInY(float pos, float boundary) const
	{
		return (pos <= m_iUpperY - boundary)
			&& (pos >= m_iLowerY + boundary);
	}

	bool Region::containsPointInY(int32_t pos, uint8_t boundary) const
	{
		return (pos <= m_iUpperY - boundary) 
			&& (pos >= m_iLowerY + boundary);
	}

	bool Region::containsPointInZ(float pos, float boundary) const
	{
		return (pos <= m_iUpperZ - boundary)
			&& (pos >= m_iLowerZ + boundary);
	}

	bool Region::containsPointInZ(int32_t pos, uint8_t boundary) const
	{
		return (pos <= m_iUpperZ - boundary)
			&& (pos >= m_iLowerZ + boundary);
	}

	void Region::cropTo(const Region& other)
	{
		m_iLowerX = ((std::max)(m_iLowerX, other.m_iLowerX));
		m_iLowerY = ((std::max)(m_iLowerY, other.m_iLowerY));
		m_iLowerZ = ((std::max)(m_iLowerZ, other.m_iLowerZ));
		m_iUpperX = ((std::min)(m_iUpperX, other.m_iUpperX));
		m_iUpperY = ((std::min)(m_iUpperY, other.m_iUpperY));
		m_iUpperZ = ((std::min)(m_iUpperZ, other.m_iUpperZ));
	}

	void Region::shift(const Vector3DInt32& amount)
	{
		shiftLowerCorner(amount);
		shiftUpperCorner(amount);
	}

	void Region::shiftLowerCorner(const Vector3DInt32& amount)
	{
		m_iLowerX += amount.getX();
		m_iLowerY += amount.getY();
		m_iLowerZ += amount.getZ();
	}

	void Region::shiftUpperCorner(const Vector3DInt32& amount)
	{
		m_iUpperX += amount.getX();
		m_iUpperY += amount.getY();
		m_iUpperZ += amount.getZ();
	}

	void Region::dilate(int32_t amount)
	{
		m_iLowerX -= amount;
		m_iLowerY -= amount;
		m_iLowerZ -= amount;

		m_iUpperX += amount;
		m_iUpperY += amount;
		m_iUpperZ += amount;
	}

	void Region::erode(int32_t amount)
	{
		m_iLowerX += amount;
		m_iLowerY += amount;
		m_iLowerZ += amount;

		m_iUpperX -= amount;
		m_iUpperY -= amount;
		m_iUpperZ -= amount;
	}
}
