#version 330 core

in vec3 wPos;
in vec3 wNormal;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float p;
uniform float gamma;

uniform float Ia;
uniform float Id;

out vec4 fColor;

// Shading
vec3 shade(vec3 pos, vec3 normal, vec3 lightPos, vec3 viewPos,
           vec3 ka, vec3 kd, vec3 ks, float p, float gamma,
           float Ia, float Id) {
    vec3 n = normalize(normal);
    vec3 l = normalize(lightPos - pos);
    vec3 v = normalize(viewPos - pos);
    vec3 h = normalize(l + v);

    vec3 ambient  = ka * Ia;
    vec3 diffuse  = kd * Id * max(dot(n, l), 0.0);
    vec3 specular = ks * Id * pow(max(dot(n, h), 0.0), p);

    vec3 color = ambient + diffuse + specular;
    color = pow(color, vec3(1.0 / gamma));
    return clamp(color, 0.0, 1.0);
}

void main() {
    vec3 color = shade(wPos, wNormal, lightPos, viewPos, ka, kd, ks, p, gamma, Ia, Id);
    fColor = vec4(color, 1.0);
}