#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;
in vec3 fragPosition;

out vec4 finalColor;

uniform vec3 camPos;
uniform vec4 colDiffuse;
uniform vec3 lightDir;

void main() {
    vec3 viewDir = normalize(camPos - fragPosition);
    vec3 halfwayDir = normalize(viewDir + lightDir);

    float diffuseReflection = max(dot(fragNormal, lightDir), 0);
    float specularReflection = pow(max(dot(fragNormal, halfwayDir), 0), 128);

    vec3 diffuse = colDiffuse.xyz * diffuseReflection;
    vec3 specular = colDiffuse.xyz * specularReflection;

    //vec3 normalColor = (fragNormal + vec3(1)) / 2;

    finalColor = vec4(diffuse + specular, colDiffuse.a);
}
