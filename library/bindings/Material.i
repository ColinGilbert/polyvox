%module Material
%{
#include "Material.h"
%}

%include "Material.h"

%template(Material8) PolyVox::Material<uint8_t>;
