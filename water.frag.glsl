#version 330

#define PI 3.1415926538

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragPosition;
in vec3 planePosition;

out vec4 finalColor;

uniform vec3 camPos;
uniform vec4 colDiffuse;
uniform vec3 lightDir;

uniform float time;

uniform int waveCount = 16;
uniform float wavePeak = 1.f;
uniform float wavePeakOffset = 1.f;

uniform float initialFrequency = 1.f;
uniform float initialAmplitude = 1.f;
uniform float initialSpeed = 2.f;

uniform float frequencyMult = 1.18f;
uniform float amplitudeMult = 0.82f;
uniform float speedMult = 1.f;

vec4 CalculateWaves(vec3 pos) {
    float waveHeight = 0;
    vec2 waveNormal = vec2(0);

    float frequency = initialFrequency;
    float amplitude = initialAmplitude;
    float speed = initialSpeed;

    float amplitudeSum = 0;

    for (int i = 0; i < waveCount; i++) {
        float angle = tan(i);
        vec2 dir = normalize(vec2(cos(angle), sin(angle)));

        float x = dot(dir, pos.xz) * frequency + time * speed;
        float wave = amplitude * exp(wavePeak * sin(x) - wavePeakOffset);
        vec2 dw = frequency * dir * (wavePeak * wave * cos(x));

        waveHeight += wave;
        waveNormal += dw;

        pos.xz += -dw * amplitude * 0.5;

        amplitudeSum += amplitude;

        frequency *= frequencyMult;
        amplitude *= amplitudeMult;
        speed *= speedMult;
    }

    waveHeight /= amplitudeSum;
    waveNormal /= amplitudeSum;

    waveHeight *= 2;

    vec3 fragNormal = normalize(vec3(-waveNormal.x, 1.0, -waveNormal.y));

    return vec4(fragNormal, waveHeight);
}

void main() {
    vec4 wave = CalculateWaves(planePosition);
    vec3 fragNormal = wave.xyz;
    float waveHeight = wave.w;

    vec3 viewDir = normalize(camPos - fragPosition);
    vec3 halfwayDir = normalize(viewDir + lightDir);

    float diffuseReflection = max(dot(fragNormal, lightDir), 0);
    float specularReflection = pow(max(dot(fragNormal, halfwayDir), 0), 128);

    vec3 diffuse = colDiffuse.xyz * diffuseReflection;
    vec3 specular = vec3(0.988, 0.898, 0.439) * specularReflection;

    vec3 ambientColor = vec3(0.03f, 0.1f, 0.2f);
    vec3 tipColor = vec3(.714, 0.761, 0.698);

    float tipFactor = pow(3 * (-2 * pow(waveHeight, 3) + 3 * pow(waveHeight, 2)), 3) / 30;
    vec3 tip = tipColor * tipFactor;

    finalColor = vec4(ambientColor + diffuse + specular + tip, colDiffuse.a);
}
