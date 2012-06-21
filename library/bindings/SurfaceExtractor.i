%module SurfaceExtractor
%{
#include "SimpleVolume.h"
#include "Material.h"
#include "SurfaceExtractor.h"
%}

%include "SimpleVolume.h"
%include "SurfaceExtractor.h"

%template(SurfaceExtractorSimpleVolumeDensity8) PolyVox::SurfaceExtractor<PolyVox::SimpleVolume<PolyVox::Density8> >;
