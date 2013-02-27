%module CubicSurfaceExtractor
%{
#include "CubicSurfaceExtractor.h"
%}

%include "CubicSurfaceExtractor.h"

%template(CubicSurfaceExtractorSimpleVolumeuint8) PolyVox::CubicSurfaceExtractor<PolyVox::SimpleVolume<uint8_t>, PolyVox::DefaultIsQuadNeeded<uint8_t> >;
//EXTRACTORS(CubicSurfaceExtractor)
