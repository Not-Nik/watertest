#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;
in vec3 vertexNormal;

out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;

uniform mat4 mvp;
uniform float time;

struct Wave {
    float frequency, amplitude, phase;
    vec2 direction;
};

float GetWaveXY(vec3 wavePosition, Wave wave) {
    return wave.direction.x * wavePosition.x + wave.direction.y * wavePosition.z;
}

float GetWaveOffset(vec3 wavePosition, Wave wave) {
    float xy = GetWaveXY(wavePosition, wave);

    return wave.amplitude * sin(xy * wave.frequency + time * wave.phase);
}

vec3 GetWaveNormal(vec3 wavePosition, Wave wave) {
    float xy = GetWaveXY(wavePosition, wave);

    vec2 derivative = wave.amplitude * wave.frequency * wave.direction * cos(xy * wave.frequency + time * wave.phase);

    vec3 tangent = normalize(vec3(1, derivative.x, 0));
    vec3 binormal = normalize(vec3(0, derivative.y, 1));

    return normalize(cross(tangent, binormal));
}

void main() {
    const Wave waves[] = Wave[](
    Wave(1, 0.3f, 2, vec2(1, 0)),
    Wave(1, 0.05f, 3, vec2(-1, 1)),
    Wave(1, 0.1f, 1, vec2(0, -1))
    );

    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;

    float waveHeight = 0;
    vec3 waveNormal = vec3(0);

    for (int i = 0; i < 3; i++) {
        Wave wave = waves[i];

        waveHeight += GetWaveOffset(vertexPosition, wave);
        waveNormal += GetWaveNormal(vertexPosition, wave);
    }

    vec3 wavePosition = vertexPosition + vec3(0, waveHeight, 0);
    fragNormal = normalize(vertexNormal + waveNormal);

    gl_Position = mvp * vec4(wavePosition, 1.0);
}