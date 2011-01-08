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
		,m_v3dStart(v3dStart)
		,m_v3dDirection(v3dDirection)
		,m_fMaxDistance(fMaxDistance)
	{
	}

	template <typename VoxelType>
	void Raycast<VoxelType>::execute(void)
	{
		Vector3DFloat v3dEnd = m_v3dStart + m_v3dDirection;
		hit = doRaycast(m_v3dStart.getX(), m_v3dStart.getY(), m_v3dStart.getZ(), v3dEnd.getX(), v3dEnd.getY(), v3dEnd.getZ());
	}

	template <typename VoxelType>
	bool Raycast<VoxelType>::doRaycast(float x1, float y1, float z1, float x2, float y2, float z2)
	{
		const float CELL_SIDE = 1.0f;

		int i = (int)floorf(x1 / CELL_SIDE);
		int j = (int)floorf(y1 / CELL_SIDE);
		int k = (int)floorf(z1 / CELL_SIDE);

		int iend = (int)floorf(x2 / CELL_SIDE);
		int jend = (int)floorf(y2 / CELL_SIDE);
		int kend = (int)floorf(z2 / CELL_SIDE);

		int di = ((x1 < x2) ? 1 : ((x1 > x2) ? -1 : 0));
		int dj = ((y1 < y2) ? 1 : ((y1 > y2) ? -1 : 0));
		int dk = ((z1 < z2) ? 1 : ((z1 > z2) ? -1 : 0));

		float minx = CELL_SIDE * floorf(x1/CELL_SIDE), maxx = minx + CELL_SIDE;
		float tx = ((x1 > x2) ? (x1 - minx) : (maxx - x1)) / abs(x2 - x1);
		float miny = CELL_SIDE * floorf(y1/CELL_SIDE), maxy = miny + CELL_SIDE;
		float ty = ((y1 > y2) ? (y1 - miny) : (maxy - y1)) / abs(y2 - y1);
		float minz = CELL_SIDE * floorf(z1/CELL_SIDE), maxz = minz + CELL_SIDE;
		float tz = ((z1 > z2) ? (z1 - minz) : (maxz - z1)) / abs(z2 - z1);

		float deltatx = CELL_SIDE / abs(x2 - x1);
		float deltaty = CELL_SIDE / abs(y2 - y1);
		float deltatz = CELL_SIDE / abs(z2 - z1);

		for(;;)
		{
			//cout << i << ", " << j << ", " << k << endl;
			if(m_volData->getVoxelAt(i,j,k).getDensity() > VoxelType::getThreshold())
			{
				x = i;
				y = j;
				z = k;
				return true;
			}

			if(tx <= ty && tx <= tz) {
				if(i == iend) break;
				tx += deltatx;
				i += di;
			} else if (ty <= tz){
				if(j == jend) break;
				ty += deltaty;
				j += dj;
			} else {
				if(k == kend) break;
				tz += deltatz;
				k += dk;
			}
		}

		return false;
	}
}