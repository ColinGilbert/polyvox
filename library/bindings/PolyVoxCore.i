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
%define VOLUMETYPES(shortname, class)
%template(shortname ## Density8) class<PolyVox::Density8>;
%template(shortname ## Material8) class<PolyVox::Material8>;
%template(shortname ## Material16) class<PolyVox::Material16>;
%template(shortname ## MaterialDensityPair44) class<PolyVox::MaterialDensityPair44>;
%template(shortname ## MaterialDensityPair88) class<PolyVox::MaterialDensityPair88>;
%enddef

%feature("autodoc", "1");

//This will rename "operator=" to "assign" since Python doesn't have assignment
%rename(assign) *::operator=;

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
//%include "TypeDef.i"
//%include "SubArray.i"
//%include "Array.i"
%include "VertexTypes.i"
%include "SurfaceMesh.i"
//%include "SimpleVolumeSampler.i"
%include "MarchingCubesSurfaceExtractor.i"
//%include "CubicSurfaceExtractor.i"
//%include "CubicSurfaceExtractorWithNormals.i"
//%include "MeshDecimator.i"

