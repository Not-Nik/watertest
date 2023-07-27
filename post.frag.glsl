#version 330 core

out vec4 finalColor;

in vec2 fragTexCoord;

uniform sampler2D color;
uniform sampler2D depth;

uniform mat4 invView;
uniform mat4 invProj;

uniform vec3 camPos;
uniform vec3 lightDir;

float near = 0.01;
float far  = 1000.0;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

vec3 WorldPosFromDepth(float depth, vec2 texCoord) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(texCoord * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invProj * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = invView * viewSpacePosition;

    return worldSpacePosition.xyz;
}

const float fogHeight = 400.f;
const float fogAttenuation = 1.2f;
const float fogDensity = .05f;
const float fogOffset = 10.f;

void main() {
    vec2 texCoord = fragTexCoord;
    texCoord.x = 1 - fragTexCoord.x;

    vec4 shadedColor = texture(color, texCoord);
    float dist = texture(depth, texCoord).r;
    vec3 worldPos = WorldPosFromDepth(dist, texCoord);
    vec3 viewDir = normalize(camPos - worldPos);

    float height = min(fogHeight, worldPos.y) / fogHeight;
    height = clamp(height, 0, 1) * 1.0f / fogAttenuation;

    float viewDistance = LinearizeDepth(dist);

    float fogFactor = (fogDensity / sqrt(log(2.f))) * max(0.0f, viewDistance - fogOffset);
    fogFactor = exp2(-fogFactor * fogFactor);

    vec4 fogColour = vec4(0.949, 0.973, 0.969, 1.0);

    vec3 sunDir = normalize(lightDir);
    sunDir.y = -sunDir.y;
    sunDir.x = -sunDir.x;
    vec4 sun = vec4(0.988, 0.898, 0.439, 1) * pow(max(0.0f, dot(viewDir, sunDir)), 4000.0f) * 0.5;

    finalColor = mix(fogColour, shadedColor, clamp(fogFactor + height, 0, 1)) + sun;
}
