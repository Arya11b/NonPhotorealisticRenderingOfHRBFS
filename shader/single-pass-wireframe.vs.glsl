#version 410 core

// Per-vertex inputs
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

// Matrices we'll need
//layout (std140) uniform constants
//{
//    mat4 mv_matrix;
//    mat4 view_matrix;
//    mat4 proj_matrix;
//};

uniform mat4 m_matrix;
uniform mat4 v_matrix;
uniform mat4 proj_matrix;

// Inputs from vertex shader
out VS_OUT
{
    vec3 Normal;
    vec3 FragPos;
} vs_out;

void main(void)
{
    // Calculate view-space coordinate
    vec4 P = v_matrix * m_matrix * vec4(position,1.0);

    // Calculate normal in view-space
    vs_out.Normal = mat3(v_matrix * m_matrix) * normal;

    // Calculate view vector
    vs_out.FragPos = P.xyz;

    // Calculate the clip-space position of each vertex
    gl_Position = proj_matrix * P;


}
