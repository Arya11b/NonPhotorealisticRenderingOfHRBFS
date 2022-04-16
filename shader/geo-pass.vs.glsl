#version 410 core

// Per-vertex inputs
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 TexCoords;

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
    vec2 TexCoords;
} vs_out;

void main(void)
{
    // Calculate view-space coordinate
    vec4 P = v_matrix * m_matrix * vec4(position,1.0);

    // Calculate normal in view-space
    vs_out.Normal = mat3(v_matrix * m_matrix) * normal;

    // Calculate view vector
    vs_out.FragPos = P.xyz;

    vs_out.TexCoords = TexCoords;

    // Calculate the clip-space position of each vertex
    gl_Position = proj_matrix * P;
	//gl_Position = gl_Position / gl_Position.w;
    //    vec4 worldPos = m_matrix *  position;
//    vs_out.FragPos = worldPos.xyz;
//
//    mat3 normalMatrix = transpose(inverse(mat3(m_matrix)));
//    vs_out.Normal = normalMatrix * normal;
//
//    gl_Position = proj_matrix * v_matrix * worldPos;
}
