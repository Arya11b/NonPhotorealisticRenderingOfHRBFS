#version 410 core

// Output
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;


// Input from vertex shader
in VS_OUT
{
    vec3 Normal;
    vec3 FragPos;
    vec2 TexCoords;
} fs_in;

// Material properties
uniform vec3 k_d = vec3(120.0/255.0, 130.0/255.0, 200.0/255.0);
uniform float k_s = 60.0/255.0;


void main(void)
{
    gPosition = fs_in.FragPos;

    gNormal = fs_in.Normal;

    gAlbedoSpec = vec4(k_d,k_s);


}
