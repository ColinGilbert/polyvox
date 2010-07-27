Welcome to PolyVox's documentation!
===================================

Contents:

.. toctree::
	:maxdepth: 1

	install

Introduction
------------
This tutorial covers the basic use of the PolyVox API. After reading this tutorial you should have a good idea how to create a PolyVox volume and fill it with data, extract a triangle mesh representing the surface, and render the result. This tutorial assumes you are already familiar with the basic concepts behind PolyVox (see the :doc:`principles of polyvox <principles>` document if not), and are reasonably confident with 3D graphics and C++. It also assumes you have already got PolyVox installed on your system, if this is not the case then please consult :doc:`installation guide <install>`.

The code samples and text in this tutorial correspond directly to the basic OpenGL example. This example uses the Qt toolkit for window and input handling, and for providing an OpenGL context to render into. In this tutorial we will omit code which performs these tasks and will instead focus on on the PolyVox code. You can consult the `Qt documentation <http://doc.qt.nokia.com/latest/>`_ if you want more information about these other aspects of the system.

Creating a volume
-----------------

*Mention required headers*

The most fundamental construct when working with PolyVox is that of the volume. This is represented by the :polyvox:`Volume` class and stores a 3D grid of voxels. Our basic example application creates a volume with the following line of code:

.. code-block:: c++

	Volume<MaterialDensityPair44> volData(64, 64, 64);

As can be seen, the Volume class is templated upon the voxel type. This means it is straight forward to create a volume of integers, floats, or a custom voxel type (see the :polyvox:`Volume documentation <PolyVox::Volume>` for more details). In this particular case we have created a volume in which each voxel is an instance of :polyvox:`MaterialDensityPair44`. Each instance of MaterialDensityPair44 holds both a material and a density and uses four bits of data for each. This means that both the material and the density have a range of 0-15, and each voxel requires one byte of storage. For more information about materials and densities please consult the :doc:`principles of polyvox <principles>` document.

Each voxel is initialised using its default constructor, which in the case of MaterialDensityPair44 will mean that both the material and the density are set to zero. This corresponds to a volume full of empty space because the density of each voxel is below the threshold.


Extracting the surface
----------------------


Rendering the surface
---------------------


Indices and tables
==================

* :ref:`genindex`
* :ref:`search`

