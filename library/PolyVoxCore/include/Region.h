#pragma region License
/******************************************************************************
This file is part of the PolyVox library
Copyright (C) 2006  David Williams

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
******************************************************************************/
#pragma endregion

#ifndef __PolyVox_Region_H__
#define __PolyVox_Region_H__

#pragma region Headers
#include "PolyVoxImpl/TypeDef.h"
#include "Vector.h"
#pragma endregion

namespace PolyVox
{
	class POLYVOXCORE_API Region
	{
	public:
		Region();
		Region(const Vector3DInt16& v3dLowerCorner, const Vector3DInt16& v3dUpperCorner);

		const Vector3DInt16& getLowerCorner(void) const;
		const Vector3DInt16& getUpperCorner(void) const;

		void setLowerCorner(const Vector3DInt16& v3dLowerCorner);
		void setUpperCorner(const Vector3DInt16& v3dUpperCorner);

		bool containsPoint(const Vector3DFloat& pos, float boundary) const;
		bool containsPoint(const Vector3DInt16& pos, uint8_t boundary) const;
		void cropTo(const Region& other);
		int16_t depth(void) const;
		int16_t height(void) const;
		void shift(const Vector3DInt16& amount);
		void shiftLowerCorner(const Vector3DInt16& amount);
		void shiftUpperCorner(const Vector3DInt16& amount);
		Vector3DInt16 dimensions(void);
		int16_t width(void) const;

	private:
		Vector3DInt16 m_v3dLowerCorner;
		Vector3DInt16 m_v3dUpperCorner;
	};
}

#endif
