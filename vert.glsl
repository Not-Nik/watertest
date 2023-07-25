#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;

out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;

uniform mat4 mvp;
uniform float time;

const float speed = 1.f;
const float wavelength = 1.f;
const float amplitude = 0.4f;
const float phase = speed * 2/wavelength;

void main() {
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;

    vec3 wavePosition = vertexPosition;

    wavePosition.y = amplitude * sin(wavePosition.x + time * phase);

    vec2 derivative = vec2(1, 0) * amplitude * cos(wavePosition.x + time * phase);

    vec3 tangent = vec3(1, 0, derivative.x);
    vec3 binormal = vec3(0, 1, derivative.y);

    fragNormal = normalize(cross(tangent, binormal));

    gl_Position = mvp*vec4(wavePosition, 1.0);
}