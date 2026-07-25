#version 450 core

uniform int uCurveIndex;

layout(location = 0) out ivec4 oCurveInfo;

void main()
{
    oCurveInfo = ivec4(uCurveIndex, 0, 0, 1);
}
