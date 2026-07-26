#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;
uniform mat4 uVP;

out vec3 fsWorldPosition;
out vec3 fsNormal;

void main()
{
    fsWorldPosition = (uModelMatrix * vec4(aPosition, 1.0f)).xyz;
    fsNormal = normalize(uNormalMatrix * aNormal);

    gl_Position = uVP * vec4(fsWorldPosition, 1.0f);
}
