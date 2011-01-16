%module SurfaceExtractor
%{
#include "VolumeSampler.h"
#include "SurfaceExtractor.h"
%}

%include "VolumeSampler.h"
%include "SurfaceExtractor.h"

%template(SurfaceExtractorMaterial8) PolyVox::SurfaceExtractor<PolyVox::Material8>;
%template(SurfaceExtractorDensity8) PolyVox::SurfaceExtractor<PolyVox::Density8>;