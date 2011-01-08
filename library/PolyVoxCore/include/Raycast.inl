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
namespace PolyVox
{
	template <typename VoxelType>
	Raycast<VoxelType>::Raycast(Volume<VoxelType>* volData, const Vector3DFloat& v3dStart, const Vector3DFloat& v3dDirection, float fMaxDistance)
		:m_volData(volData)
		,m_sampVolume(volData)
		,m_v3dStart(v3dStart)
		,m_v3dDirection(v3dDirection)
		,m_fMaxDistance(fMaxDistance)
	{
	}

	template <typename VoxelType>
	void Raycast<VoxelType>::execute(void)
	{
		Vector3DFloat v3dStart = m_v3dStart + Vector3DFloat(0.5f, 0.5f, 0.5f);
		Vector3DFloat v3dEnd = v3dStart + m_v3dDirection;
		hit = doRaycast(v3dStart.getX(), v3dStart.getY(), v3dStart.getZ(), v3dEnd.getX(), v3dEnd.getY(), v3dEnd.getZ());
	}

	template <typename VoxelType>
	bool Raycast<VoxelType>::doRaycast(float x1, float y1, float z1, float x2, float y2, float z2)
	{
		int i = (int)floorf(x1);
		int j = (int)floorf(y1);
		int k = (int)floorf(z1);

		int iend = (int)floorf(x2);
		int jend = (int)floorf(y2);
		int kend = (int)floorf(z2);

		int di = ((x1 < x2) ? 1 : ((x1 > x2) ? -1 : 0));
		int dj = ((y1 < y2) ? 1 : ((y1 > y2) ? -1 : 0));
		int dk = ((z1 < z2) ? 1 : ((z1 > z2) ? -1 : 0));

		float minx = floorf(x1), maxx = minx + 1.0f;
		float tx = ((x1 > x2) ? (x1 - minx) : (maxx - x1)) / abs(x2 - x1);
		float miny = floorf(y1), maxy = miny + 1.0f;
		float ty = ((y1 > y2) ? (y1 - miny) : (maxy - y1)) / abs(y2 - y1);
		float minz = floorf(z1), maxz = minz + 1.0f;
		float tz = ((z1 > z2) ? (z1 - minz) : (maxz - z1)) / abs(z2 - z1);

		float deltatx = 1.0f / abs(x2 - x1);
		float deltaty = 1.0f / abs(y2 - y1);
		float deltatz = 1.0f / abs(z2 - z1);

		m_sampVolume.setPosition(i,j,k);

		for(;;)
		{
			if(m_sampVolume.getVoxel().getDensity() > VoxelType::getThreshold())
			{
				x = i;
				y = j;
				z = k;
				return true;
			}

			if(tx <= ty && tx <= tz)
			{
				if(i == iend) break;
				tx += deltatx;
				i += di;

				if(di == 1) m_sampVolume.movePositiveX();
				if(di == -1) m_sampVolume.moveNegativeX();
			} else if (ty <= tz)
			{
				if(j == jend) break;
				ty += deltaty;
				j += dj;

				if(dj == 1) m_sampVolume.movePositiveY();
				if(dj == -1) m_sampVolume.moveNegativeY();
			} else 
			{
				if(k == kend) break;
				tz += deltatz;
				k += dk;

				if(dk == 1) m_sampVolume.movePositiveZ();
				if(dk == -1) m_sampVolume.moveNegativeZ();
			}
		}

		return false;
	}
}