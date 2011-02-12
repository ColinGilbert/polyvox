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

#include "OpenGLWidget.h"

#include "MaterialDensityPair.h"
#include "CubicSurfaceExtractorWithNormals.h"
#include "SurfaceMesh.h"
#include "Volume.h"

#include <QApplication>

//Use the PolyVox namespace
using namespace PolyVox;

#include <stdlib.h>


#define SAMPLE_SIZE 1024

class Perlin
{
public:

  Perlin(int octaves,float freq,float amp,int seed);


  float Get(float x,float y)
  {
    float vec[2];
    vec[0] = x;
    vec[1] = y;
    return perlin_noise_2D(vec);
  };

  float Get3D(float x,float y,float z)
  {
    float vec[3];
    vec[0] = x;
    vec[1] = y;
	vec[2] = z;
    return perlin_noise_3D(vec);
  };

private:
  void init_perlin(int n,float p);
  float perlin_noise_2D(float vec[2]);
  float perlin_noise_3D(float vec[3]);

  float noise1(float arg);
  float noise2(float vec[2]);
  float noise3(float vec[3]);
  void normalize2(float v[2]);
  void normalize3(float v[3]);
  void init(void);

  int   mOctaves;
  float mFrequency;
  float mAmplitude;
  int   mSeed;

  int p[SAMPLE_SIZE + SAMPLE_SIZE + 2];
  float g3[SAMPLE_SIZE + SAMPLE_SIZE + 2][3];
  float g2[SAMPLE_SIZE + SAMPLE_SIZE + 2][2];
  float g1[SAMPLE_SIZE + SAMPLE_SIZE + 2];
  bool  mStart;

};

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define B SAMPLE_SIZE
#define BM (SAMPLE_SIZE-1)

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

#define s_curve(t) ( t * t * (3.0f - 2.0f * t) )
#define lerp(t, a, b) ( a + t * (b - a) )

#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.0f;

float Perlin::noise1(float arg)
{
	int bx0, bx1;
	float rx0, rx1, sx, t, u, v, vec[1];

	vec[0] = arg;

	if (mStart)
  {
    srand(mSeed);
		mStart = false;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);

	sx = s_curve(rx0);

	u = rx0 * g1[ p[ bx0 ] ];
	v = rx1 * g1[ p[ bx1 ] ];

	return lerp(sx, u, v);
}

float Perlin::noise2(float vec[2])
{
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	int i, j;

	if (mStart)
  {
    srand(mSeed);
		mStart = false;
		init();
	}

	setup(0,bx0,bx1,rx0,rx1);
	setup(1,by0,by1,ry0,ry1);

	i = p[bx0];
	j = p[bx1];

	b00 = p[i + by0];
	b10 = p[j + by0];
	b01 = p[i + by1];
	b11 = p[j + by1];

	sx = s_curve(rx0);
	sy = s_curve(ry0);

  #define at2(rx,ry) ( rx * q[0] + ry * q[1] )

	q = g2[b00];
	u = at2(rx0,ry0);
	q = g2[b10];
	v = at2(rx1,ry0);
	a = lerp(sx, u, v);

	q = g2[b01];
	u = at2(rx0,ry1);
	q = g2[b11];
	v = at2(rx1,ry1);
	b = lerp(sx, u, v);

	return lerp(sy, a, b);
}

float Perlin::noise3(float vec[3])
{
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	int i, j;

	if (mStart)
  {
    srand(mSeed);
		mStart = false;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);
	setup(2, bz0,bz1, rz0,rz1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	t  = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);

  #define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

	q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
	q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
	a = lerp(t, u, v);

	q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
	q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
	b = lerp(t, u, v);

	c = lerp(sy, a, b);

	q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
	q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
	a = lerp(t, u, v);

	q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
	q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
	b = lerp(t, u, v);

	d = lerp(sy, a, b);

	return lerp(sz, c, d);
}

void Perlin::normalize2(float v[2])
{
	float s;

	s = (float)sqrt(v[0] * v[0] + v[1] * v[1]);
  s = 1.0f/s;
	v[0] = v[0] * s;
	v[1] = v[1] * s;
}

void Perlin::normalize3(float v[3])
{
	float s;

	s = (float)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  s = 1.0f/s;

	v[0] = v[0] * s;
	v[1] = v[1] * s;
	v[2] = v[2] * s;
}

void Perlin::init(void)
{
	int i, j, k;

	for (i = 0 ; i < B ; i++)
  {
		p[i] = i;
		g1[i] = (float)((rand() % (B + B)) - B) / B;
		for (j = 0 ; j < 2 ; j++)
			g2[i][j] = (float)((rand() % (B + B)) - B) / B;
		normalize2(g2[i]);
		for (j = 0 ; j < 3 ; j++)
			g3[i][j] = (float)((rand() % (B + B)) - B) / B;
		normalize3(g3[i]);
	}

	while (--i)
  {
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i = 0 ; i < B + 2 ; i++)
  {
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		for (j = 0 ; j < 2 ; j++)
			g2[B + i][j] = g2[i][j];
		for (j = 0 ; j < 3 ; j++)
			g3[B + i][j] = g3[i][j];
	}

}


float Perlin::perlin_noise_2D(float vec[2])
{
  int terms    = mOctaves;
	float freq   = mFrequency;
	float result = 0.0f;
  float amp = mAmplitude;

  vec[0]*=mFrequency;
  vec[1]*=mFrequency;

	for( int i=0; i<terms; i++ )
	{
		result += noise2(vec)*amp;
		vec[0] *= 2.0f;
		vec[1] *= 2.0f;
    amp*=0.5f;
	}


	return result;
}

float Perlin::perlin_noise_3D(float vec[3])
{
  int terms    = mOctaves;
	float freq   = mFrequency;
	float result = 0.0f;
  float amp = mAmplitude;

  vec[0]*=mFrequency;
  vec[1]*=mFrequency;
  vec[2]*=mFrequency;

	for( int i=0; i<terms; i++ )
	{
		result += noise3(vec)*amp;
		vec[0] *= 2.0f;
		vec[1] *= 2.0f;
		vec[2] *= 2.0f;
    amp*=0.5f;
	}


	return result;
}

Perlin::Perlin(int octaves,float freq,float amp,int seed)
{
  mOctaves = octaves;
  mFrequency = freq;
  mAmplitude = amp;
  mSeed = seed;
  mStart = true;
}

void createPerlinVolumeSlow(Volume<MaterialDensityPair44>& volData)
{
	Perlin perlin(2,8,1,234);

	for(int z = 1; z < volData.getDepth()-1; z++)
	{
		std::cout << z << std::endl;
		for(int y = 1; y < volData.getHeight()-1; y++)
		{
			for(int x = 1; x < volData.getWidth()-1; x++) 
			{							
				float perlinVal = perlin.Get3D(x /static_cast<float>(volData.getWidth()-1), (y) / static_cast<float>(volData.getHeight()-1), z / static_cast<float>(volData.getDepth()-1));

				perlinVal += 1.0f;
				perlinVal *= 0.5f;
				perlinVal *= MaterialDensityPair44::getMaxDensity();

				MaterialDensityPair44 voxel;

				voxel.setMaterial(245);
				voxel.setDensity(perlinVal);

				/*if(perlinVal < 0.0f)
				{
					voxel.setMaterial(245);
					voxel.setDensity(MaterialDensityPair44::getMaxDensity());
				}
				else
				{
					voxel.setMaterial(0);
					voxel.setDensity(MaterialDensityPair44::getMinDensity());
				}*/

				volData.setVoxelAt(x, y, z, voxel);
			}
		}
	}
}

void createPerlinVolumeFast(Volume<MaterialDensityPair44>& volData)
{
	Perlin perlin(2,8,1,234);

	for(int blockZ = 0; blockZ < volData.m_uDepthInBlocks; blockZ++)
	{		
		std::cout << blockZ << std::endl;
		for(int blockY = 0; blockY < volData.m_uHeightInBlocks; blockY++)
		{
			for(int blockX = 0; blockX < volData.m_uWidthInBlocks; blockX++)
			{
				for(int offsetz = 0; offsetz < volData.m_uBlockSideLength; offsetz++)
				{
					for(int offsety = 0; offsety < volData.m_uBlockSideLength; offsety++)
					{
						for(int offsetx = 0; offsetx < volData.m_uBlockSideLength; offsetx++) 
						{							
							int x = blockX * volData.m_uBlockSideLength + offsetx;
							int y = blockY * volData.m_uBlockSideLength + offsety;
							int z = blockZ * volData.m_uBlockSideLength + offsetz;

							if((x == 0) || (x == volData.getWidth()-1)) continue;
							if((y == 0) || (y == volData.getHeight()-1)) continue;
							if((z == 0) || (z == volData.getDepth()-1)) continue;

							float perlinVal = perlin.Get3D(x /static_cast<float>(volData.getWidth()-1), (y) / static_cast<float>(volData.getHeight()-1), z / static_cast<float>(volData.getDepth()-1));

							MaterialDensityPair44 voxel;
							if(perlinVal < 0.0f)
							{
								voxel.setMaterial(245);
								voxel.setDensity(MaterialDensityPair44::getMaxDensity());
							}
							else
							{
								voxel.setMaterial(0);
								voxel.setDensity(MaterialDensityPair44::getMinDensity());
							}

							volData.setVoxelAt(x, y, z, voxel);
						}
					}
				}
			}
		}			
	}
}

void createPerlinTerrain(Volume<MaterialDensityPair44>& volData)
{
	Perlin perlin(2,2,1,234);

	for(int x = 1; x < volData.getWidth()-1; x++)
	{
		std::cout << x << std::endl;
		for(int y = 1; y < volData.getHeight()-1; y++)
		{
			float perlinVal = perlin.Get(x / static_cast<float>(volData.getHeight()-1), y / static_cast<float>(volData.getDepth()-1));
			perlinVal += 1.0f;
			perlinVal *= 0.5f;
			perlinVal *= volData.getWidth();
			for(int z = 1; z < volData.getDepth()-1; z++) 
			{							
				MaterialDensityPair44 voxel;
				if(z < perlinVal)
				{
					voxel.setMaterial(245);
					voxel.setDensity(MaterialDensityPair44::getMaxDensity());
				}
				else
				{
					voxel.setMaterial(0);
					voxel.setDensity(MaterialDensityPair44::getMinDensity());
				}

				volData.setVoxelAt(x, y, z, voxel);
			}
		}
	}
}

void createSphereInVolume(Volume<MaterialDensityPair44>& volData, Vector3DFloat v3dVolCenter, float fRadius)
{
	//This vector hold the position of the center of the volume
	//Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);

	int iRadius = fRadius;

	//This three-level for loop iterates over every voxel in the volume
	for (int z = v3dVolCenter.getZ() - iRadius; z <= v3dVolCenter.getZ() + iRadius; z++)
	{
		for (int y = v3dVolCenter.getY() - iRadius; y <= v3dVolCenter.getY() + iRadius; y++)
		{
			for (int x = v3dVolCenter.getX() - iRadius; x <= v3dVolCenter.getX() + iRadius; x++)
			{
				//Store our current position as a vector...
				Vector3DFloat v3dCurrentPos(x,y,z);	
				//And compute how far the current position is from the center of the volume
				float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();

				//If the current voxel is less than 'radius' units from the center then we make it solid.
				if(fDistToCenter <= fRadius)
				{
					//Our new density value
					uint8_t uDensity = MaterialDensityPair44::getMaxDensity();

					//Get the old voxel
					MaterialDensityPair44 voxel = volData.getVoxelAt(x,y,z);

					//Modify the density
					voxel.setDensity(uDensity);

					//Wrte the voxel value into the volume	
					volData.setVoxelAt(x, y, z, voxel);
				}
			}
		}
	}
}

int main(int argc, char *argv[])
{
	//Create and show the Qt OpenGL window
	QApplication app(argc, argv);
	OpenGLWidget openGLWidget(0);
	openGLWidget.show();

	//Create an empty volume and then place a sphere in it
	Volume<MaterialDensityPair44> volData(2048, 2048, 256);
	//createSphereInVolume(volData, 30);
	createPerlinTerrain(volData);
	//createPerlinVolumeSlow(volData);
	std::cout << "Memory usage: " << volData.sizeInBytes() << std::endl;
	volData.setBlockCacheSize(8);
	std::cout << "Memory usage: " << volData.sizeInBytes() << std::endl;

	/*srand(12345);
	for(int ct = 0; ct < 1000; ct++)
	{
		std::cout << ct << std::endl;
		int x = rand() % volData.getWidth();
		int y = rand() % volData.getHeight();
		int z = rand() % volData.getDepth();

		int r = rand() % 20;

		createSphereInVolume(volData, Vector3DFloat(x,y,z), r);
	}*/

	//Extract the surface
	SurfaceMesh<PositionMaterialNormal> mesh;
	CubicSurfaceExtractorWithNormals<MaterialDensityPair44> surfaceExtractor(&volData, volData.getEnclosingRegion(), &mesh);
	surfaceExtractor.execute();

	//Pass the surface to the OpenGL window
	openGLWidget.setSurfaceMeshToRender(mesh);

	//Run the message pump.
	return app.exec();
} 
