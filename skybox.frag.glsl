#version 330 core

out vec4 finalColor;

in vec3 fragTexCoord;

uniform samplerCube skybox;

void main() {
    finalColor = texture(skybox, fragTexCoord + vec3(0, 0, 0));
}
