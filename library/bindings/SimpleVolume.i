%module SimpleVolume
%{
#include "Material.h"
#include "Density.h"
#include "SimpleVolume.h"
%}

%include "Material.h"
%include "Density.h"
%include "SimpleVolume.h"

VOLUMETYPES(SimpleVolume)
