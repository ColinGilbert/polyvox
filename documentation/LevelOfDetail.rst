***************
Level of Detail
***************
When the PolyVox surface extractors are applied to volume data the resulting mesh can contain a very high number of triangles. For large voxel worlds this can cause both performance and memory problems. The performance problems occur due the the load on the vertex shader which has to process a large number of vertices, and also because of the rendering of a large number of tiny (possibly sub-pixel) triangles. The memory costs result simply from have a large amount of data which does not actually contibute to the visual appeaance of the scene.

For these reasons it is desirable to reduce the triangle count of the meshes as far as possible, espessially as meshes move away from the camera. This document describes the various approaches which are available within PolyVox to achieve this. Generally these approches are different for cubic meshes vs smooth meshes and so we address these cases seperatly.

Cubic Meshes
============
A naive implementation of a cubic surface extractor would generate a mesh containing a quad for voxel face which lies on the boundary between a solid and an empty voxel. The CubicSurfaceExtractor is indeed capable of generating such a mesh, but it also provides the option to merge adjacent quads into a single quad subject to various conditions being satisfied (e.g. the faces must have the same material). This meging process drastically reduces the amount of geometry which must be drawn but does not modify the shape of the mesh. Because of these desirable properties such merging is performed by default, but it can be disabled if necessary.

To our knowledge the only drawback of performing this quad marging is that it can create T-junctions in the resulting mesh. T-junctions are an undesirable property of mesh geometry because they can cause tiny cracks (usually just seen as flickering pixels) to occur between quads. The figure below shows a mesh before quad merging, a mesh where the merged quads have caused T-junctions, and how the resulting rendering might look (note the single pixel holes along the quad border).

Vertices C and D are supposed to lie exactly along the line which has A and B as its end points, so in theory the mesh should be valid and should render correctly. The reason T-junctions cause a problem in practice is due to limitations of the floating point number representation. Ddepending on the transformations which are applied, it may be that the positions of C and/or D can not be represented precisely enough to exactly lie on the line between A and B. 

Demo correct mesh. mention we don't have a solution to generate it.

whether it's a problem inpractice depends on hardware precision (16/32 bit), distance from origin, number of transforms, etc.

Mentions Voxeliens soution.

Smooth Meshes
=============