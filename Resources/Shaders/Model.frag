#version 450 core

struct Model
{
    vec3 Color;
    float Ambient;
    float Diffuse;
};

struct Light
{
    vec3 Color;
    vec3 Direction;
    float Ambient;
    float Diffuse;
};

uniform Model uModel;
uniform Light uLight;

in vec4 fsWorldPosition;
in vec3 fsNormal;

layout(location = 0) out vec4 oColor;

void main()
{
    // Ambient
    const float Ambient = uLight.Ambient * uModel.Ambient;

    // Diffuse
    const float Diffuse = max(dot(fsNormal, -uLight.Direction), 0.0f) * uLight.Diffuse * uModel.Diffuse;

    // Combine
    const vec3 Color = (Ambient + Diffuse) * uModel.Color * uLight.Color;
    oColor = vec4(Color, 1.0f);
}
