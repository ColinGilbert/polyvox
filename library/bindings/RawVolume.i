%module RawVolume
%{
#include "Material.h"
#include "Density.h"
#include "RawVolume.h"
%}

%include "Material.h"
%include "Density.h"
%include "RawVolume.h"

%template(RawVolumeDensity8) PolyVox::RawVolume<PolyVox::Density8>;

%template(RawVolumeMaterial8) PolyVox::RawVolume<PolyVox::Material8>;

%template(RawVolumeMaterial16) PolyVox::RawVolume<PolyVox::Material16>;

%template(RawVolumeMaterialDensityPair44) PolyVox::RawVolume<PolyVox::MaterialDensityPair44>;

%template(RawVolumeMaterialDensityPair88) PolyVox::RawVolume<PolyVox::MaterialDensityPair88>;
