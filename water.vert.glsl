#version 330

#define PI 3.1415926538

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;
in vec3 vertexNormal;

out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragPosition;
out vec3 planePosition;

uniform mat4 mvp;
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
        float angle = tan(float(i));
        vec2 dir = normalize(vec2(cos(i), sin(i)));

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
    vec4 wave = CalculateWaves(vertexPosition);
    vec3 fragNormal = wave.xyz;
    float waveHeight = wave.w;

    vec3 wavePosition = vertexPosition + vec3(0, waveHeight, 0);

    planePosition = vertexPosition;
    fragPosition = wavePosition;
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    gl_Position = mvp * vec4(wavePosition, 1.0);
}
