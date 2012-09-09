*************
Prerequisites
*************
The PolyVox library is also quite low-level in terms of the functionality it provides, and as a result of these factors you will need some significant previous expeience in order to make use of the library effectively. In this document we summarise the key areas with which you will need to be familier, and explain why they are necesssary when using PolyVox. We also provide some links where you can study some background material.

You should also be aware that voxel terrain is still an open research area and had not yet seen widespread adoption in games and simulations. There are many questions to which we do not currently know the best answer and so you can expect to have to do some research and experimentation yourself when trying to opbtain your desired result. So pease do let us know if you come up with a trick or technique which you think could benefit other users.

Using the library
=================
This section describes some of the key principles which you may want to understand in order to make use of PolyVox. Not all of these are strictly required as it depends on exactly what you are trying to achive, but in general you should find them useful:

Volume graphics: 
Surface extaction (MC and our own cubic docs):
Mesh representation:
Image processing:

There are also some programming concepts with which you will need to be familiar:

C++: PolyVox is written using the C++ language and we expect this is what the majority of our users will be developing in. You will need to be familer with the basic process of building and linking against external libraires as well as setting up your development environment. Note that you do have the option of working with other languages via the SWIG bindings but you may not have as much flexibility with this approach.
Templates: PolyVox also makes heavy use of template programming in order to be both fast and generic, so familiarity with templates will be very useful. You should need to do much template programming yourself but an understanding of them will help you understand errors and resolve any problems.
Callbacks:

Rendering
=========
Runtime geometry creation: PolyVox is independant of any particular graphics API which means it outputs its data using API-neutral structures such as index/vertex buffers. You will need to write the code which converts these structures into a format which your API or engine can understand. This is not a difficult task but does require some knowledge of the rendering technology you are using.
Scene management:
	-Culling, organisation, LOD.

Shader programming
==================
Triplanar texturing:
Texture atlases:
Lighting:
