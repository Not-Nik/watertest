#version 330 core

in vec3 vertexPosition;

out vec3 fragTexCoord;

uniform mat4 matModel;
uniform mat4 matProjection;
uniform mat4 matView;

mat3 rotateY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat3(
    c, 0, -s,
    0, 1, 0,
    s, 0, c
    );
}


void main() {
    mat4 rotView = mat4(mat3(matView));
    fragTexCoord = rotateY(-10.4) * vertexPosition;
    gl_Position = matProjection * rotView * vec4(vertexPosition, 1.0);
}