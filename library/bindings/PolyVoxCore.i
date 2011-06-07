%module PolyVoxCore

#define POLYVOX_API

%rename(equals) operator==;
%rename(notEqualTo) operator!=;
%rename(lessThan) operator<;
%rename(greaterThan) operator>;
%rename(lessThanOrEqualTo) operator<=;
%rename(greaterThanOrEqualTo) operator>=;
%rename(assignment) operator=;
%rename(additionAssignment) operator+=;
%rename(subtractionAssignment) operator-=;
%rename(multiplicationAssignment) operator*=;
%rename(divisionAssignment) operator/=;
%rename(arrayOperator) operator[];

%include "stdint.i"
%include "std_vector.i"
%include "Material.i"
%include "Density.i"
%include "Vector.i"
%include "Region.i"
%include "SimpleVolume.i"
%include "TypeDef.i"
//%include "SubArray.i"
//%include "Array.i"
%include "VertexTypes.i"
%include "SurfaceMesh.i"
//%include "SimpleVolumeSampler.i"
%include "SurfaceExtractor.i"
//%include "CubicSurfaceExtractor.i"
//%include "CubicSurfaceExtractorWithNormals.i"
//%include "MeshDecimator.i"