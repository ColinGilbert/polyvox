%module Volume
%{
#include "Material.h"
#include "Density.h"
#include "Volume.h"
%}

%include "Material.h"
%include "Density.h"
%include "Volume.h"

%template(VolumeMaterial8) PolyVox::Volume<PolyVox::Material8>;
%template(VolumeDensity8) PolyVox::Volume<PolyVox::Density8>;