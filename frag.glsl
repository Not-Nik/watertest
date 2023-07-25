#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

out vec4 finalColor;

uniform vec4 colDiffuse;
uniform vec3 lightDirection;

void main() {
    float lambertianDiffuse = dot(fragNormal, lightDirection);
    finalColor = vec4(colDiffuse.xyz * lambertianDiffuse, colDiffuse.a);
}