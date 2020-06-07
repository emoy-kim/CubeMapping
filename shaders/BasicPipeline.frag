#version 460

struct MateralInfo {
   vec4 EmissionColor;
   vec4 AmbientColor;
   vec4 DiffuseColor;
   vec4 SpecularColor;
   float SpecularExponent;
};
uniform MateralInfo Material;

layout (binding = 0) uniform samplerCube BaseTexture;
uniform int UseTexture;

in vec3 tex_coord;

layout (location = 0) out vec4 final_color;

const float one = 1.0f;

void main()
{
   if (UseTexture == 0) final_color = vec4(one);
   else final_color = texture( BaseTexture, tex_coord );

   final_color *= Material.DiffuseColor;
}