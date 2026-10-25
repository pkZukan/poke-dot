shader_type spatial;

#define NON_DIRECTIONAL
#define TRINITY_CUSTOM_LIGHT
#include "trinity_standard.gdshaderinc"
void light() { 
    DIFFUSE_LIGHT += LIGHT_COLOR * TRINITY_ATTENUATION(1) / PI; 
}
