# -*- coding: utf-8 -*-

import sys
sys.path.append("../library/bindings/")

import unittest
import PolyVoxCore

def test_functor(sampler):
	return sampler <= 0

class TestSurfaceExtractor(unittest.TestCase):
	def setUp(self):
		
		#Create a small volume
		r = PolyVoxCore.Region(PolyVoxCore.Vector3Dint32_t(0,0,0), PolyVoxCore.Vector3Dint32_t(31,31,31))
		self.vol = PolyVoxCore.SimpleVolumeuint8(r)
		#Set one single voxel to have a reasonably high density
		self.vol.setVoxelAt(PolyVoxCore.Vector3Dint32_t(5, 5, 5), 200)
	
	def test_hit_voxel(self):
		self.assertEqual(PolyVoxCore.raycastWithEndpointsSimpleVolumeuint8(self.vol, PolyVoxCore.Vector3Dfloat(0,0,0), PolyVoxCore.Vector3Dfloat(31,31,31), test_functor), 1)
	
	def test_miss_voxel(self):
		self.assertEqual(PolyVoxCore.raycastWithEndpointsSimpleVolumeuint8(self.vol, PolyVoxCore.Vector3Dfloat(0,0,0), PolyVoxCore.Vector3Dfloat(0,31,31), test_functor), 0)

if __name__ == '__main__':
	unittest.main()
