shader_type spatial;

#define NON_DIRECTIONAL
#include "trinity_standard.gdshaderinc"
void light() { 
    DIFFUSE_LIGHT += LIGHT_COLOR * ATTENUATION / PI; 
}
