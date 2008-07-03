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

//Dave, maybe make use of OgrePlatform.h instead?
// I think use _OgreExport instead of POLYVOX_API and define OGRE_NONCLIENT_BUILD instead of POLYVOX_EXPORT?
#ifndef __PolyVox_TypeDef_H__
#define __PolyVox_TypeDef_H__

#ifdef WIN32
	#ifdef POLYVOX_EXPORT
		#define POLYVOX_API __declspec(dllexport)
	#else
		#define POLYVOX_API __declspec(dllimport)
	#endif
#else
	#define POLYVOX_API __attribute__ ((visibility("default")))
#endif

#endif
