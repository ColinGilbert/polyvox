%module PolyVoxCore

#define POLYVOX_API
%include "Impl/TypeDef.h"
#define __attribute__(x) //Silence DEPRECATED errors

//This macro allows us to use Python properties on our classes
%define PROPERTY(type,name,getter,setter)
%extend type {
	%pythoncode %{
		__swig_getmethods__["name"] = getter
		__swig_setmethods__["name"] = setter
		if _newclass: name = property(getter, setter)
	%}
};
%enddef

//Put this in an %extend section to wrap operator<< as __str__
%define STR()
const char* __str__() {
	std::ostringstream out;
	out << *$self;
	return out.str().c_str();
}
%enddef

//Centralise this to avoid repeating ourselves
//This macro will be called in the volume interface files to define the various volume types.
%define VOLUMETYPES(class)
%template(class ## Density8) PolyVox::class<PolyVox::Density8>;
%template(class ## Material8) PolyVox::class<PolyVox::Material8>;
%template(class ## Material16) PolyVox::class<PolyVox::Material16>;
%template(class ## MaterialDensityPair44) PolyVox::class<PolyVox::MaterialDensityPair44>;
%template(class ## MaterialDensityPair88) PolyVox::class<PolyVox::MaterialDensityPair88>;
%enddef

//Template based on voxel type
%define EXTRACTOR(class, volumetype)
%template(class ## volumetype ## Density8) PolyVox::class<PolyVox::volumetype<PolyVox::Density8> >;
//%template(class ## volumetype ## Material8) PolyVox::class<PolyVox::volumetype<PolyVox::Material8> >;
//%template(class ## volumetype ## Material16) PolyVox::class<PolyVox::volumetype<PolyVox::Material16> >;
%template(class ## volumetype ## MaterialDensityPair44) PolyVox::class<PolyVox::volumetype<PolyVox::MaterialDensityPair44> >;
%template(class ## volumetype ## MaterialDensityPair88) PolyVox::class<PolyVox::volumetype<PolyVox::MaterialDensityPair88> >;
%enddef

//Template based on volume type
%define EXTRACTORS(shortname)
EXTRACTOR(shortname, SimpleVolume)
EXTRACTOR(shortname, RawVolume)
EXTRACTOR(shortname, LargeVolume)
%enddef

%feature("autodoc", "1");

#ifdef SWIGPYTHON
//This will rename "operator=" to "assign" since Python doesn't have assignment
%rename(assign) *::operator=;
#endif

%include "stdint.i"
%include "std_vector.i"
%include "Vector.i"
%include "DefaultMarchingCubesController.i"
%include "Density.i"
%include "Material.i"
%include "MaterialDensityPair.i"
%include "Region.i"
%include "BaseVolume.i"
%include "SimpleVolume.i"
%include "RawVolume.i"
%include "LargeVolume.i"
//%include "SubArray.i"
//%include "Array.i"
%include "VertexTypes.i"
%include "SurfaceMesh.i"
%include "MarchingCubesSurfaceExtractor.i"
//%include "CubicSurfaceExtractor.i"
