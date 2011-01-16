%module PolyVoxCore

#define POLYVOX_API

%rename(equalTo) operator==;
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

%include "stdint.i"
%include "Material.i"
%include "Density.i"
%include "Vector.i"
%include "Region.i"
%include "Volume.i"
%include "TypeDef.i"
%include "VertexTypes.i"
%include "SurfaceMesh.i"
%include "VolumeSampler.i"
%include "SurfaceExtractor.i"
