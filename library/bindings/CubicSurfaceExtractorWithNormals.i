%module CubicSurfaceExtractorWithNormals
%{
#include "VolumeSampler.h"
//#include "CubicSurfaceExtractor.h"
#include "CubicSurfaceExtractorWithNormals.h"
%}

%include "VolumeSampler.h"
//%include "CubicSurfaceExtractor.h"
%include "CubicSurfaceExtractorWithNormals.h"

%template(CubicSurfaceExtractorWithNormalsMaterial8) PolyVox::CubicSurfaceExtractorWithNormals<PolyVox::Material8>;
%template(CubicSurfaceExtractorWithNormalsDensity8) PolyVox::CubicSurfaceExtractorWithNormals<PolyVox::Density8>;