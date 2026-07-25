#version 450 core

uniform samplerCube uTexture;
uniform float uBrightness;

in vec3 fsTextureCoords;

out vec4 oColor;

void main()
{
    const vec3 Color = texture(uTexture, fsTextureCoords).rgb;
    oColor = vec4(Color * uBrightness, 1.0f);
}
