/******************************************************************************
This file is part of a voxel plugin for OGRE
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

#ifndef __PolyVox_RegionGeometry_H__
#define __PolyVox_RegionGeometry_H__

#include "PolyVoxForwardDeclarations.h"
#include "TypeDef.h"
#include "Vector.h"

namespace PolyVox
{
	class POLYVOX_API RegionGeometry
	{
	public:
		RegionGeometry();

		bool m_bIsEmpty;
		bool m_bContainsSingleMaterialPatch;
		bool m_bContainsMultiMaterialPatch;

		Vector3DInt32 m_v3dRegionPosition;
		IndexedSurfacePatch* m_patchSingleMaterial;
		IndexedSurfacePatch* m_patchMultiMaterial;

		long int getSizeInBytes(void);

	};	
}

#endif
