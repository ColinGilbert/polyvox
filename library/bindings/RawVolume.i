%module RawVolume
%{
#include "Material.h"
#include "Density.h"
#include "RawVolume.h"
%}

%include "Material.h"
%include "Density.h"
%include "RawVolume.h"

VOLUMETYPES(RawVolume)
