#version 450 core

struct Curve
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

// Glow effect settings
struct GlowSettings
{
    float Intensity;      // Glow parlaklık yoğunluğu (0.0 - 3.0)
    float Pulse;          // Puls animasyon hızı (0.0 = kapalı)
    float Shimmer;        // Shimmer/parıltı efekti (0.0 - 1.0)
    float CoreBoost;      // Merkez parlaklık güçlendirmesi (0.0 - 2.0)
};

uniform Curve uCurve;
uniform Light uLight;
uniform GlowSettings uGlow;
uniform float uTime;

in vec3 fsWorldPosition;
in vec3 fsNormal;

layout(location = 0) out vec4 oColor;

void main()
{
    const float Ambient = uLight.Ambient * uCurve.Ambient;
    const float Diffuse = max(dot(fsNormal, -uLight.Direction), 0.0f) * uLight.Diffuse * uCurve.Diffuse;

    const vec3 BaseColor = (Ambient + Diffuse) * uCurve.Color * uLight.Color;
    
    float Pulse = 1.0;
    if (uGlow.Pulse > 0.0)
    {
        Pulse = 0.7 + 0.3 * sin(uTime * uGlow.Pulse * 2.0 + fsWorldPosition.x * 0.1);
    }
    
    float Shimmer = 1.0;
    if (uGlow.Shimmer > 0.0)
    {
        Shimmer = 0.8 + 0.2 * sin(uTime * 5.0 + fsWorldPosition.x * 0.3) * uGlow.Shimmer;
    }
    

    float CoreIntensity = 1.0;
    if (uGlow.CoreBoost > 0.0)
    {
        float fresnel = pow(1.0 - max(dot(normalize(fsNormal), normalize(-fsWorldPosition)), 0.0), 2.0);
        CoreIntensity = 1.0 + (1.0 - fresnel) * uGlow.CoreBoost;
    }
    
    // Combine glow effects
    vec3 GlowColor = BaseColor.rgb * uGlow.Intensity * Pulse * Shimmer * CoreIntensity;
    oColor = vec4(GlowColor, Pulse);
}
