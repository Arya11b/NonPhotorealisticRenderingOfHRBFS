#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;


uniform vec2 ratio = vec2(800.0,800.0);
in VS_OUT
{
    vec3 Normal;
    vec3 FragPos;
} gs_in[];

out GS_OUT{
    vec3 Normal;
    vec3 FragPos;
    vec3 dist;
}gs_out;

vec2 p0 = vec2(0.0);
vec2 p1 = vec2(0.0);
vec2 p2 = vec2(0.0);
vec3 dist = vec3(0.0);
void main()
{

    p0 = ratio * gl_in[0].gl_Position.xy/gl_in[0].gl_Position.w;
    p1 = ratio * gl_in[1].gl_Position.xy/gl_in[1].gl_Position.w;
    p2 = ratio * gl_in[2].gl_Position.xy/gl_in[2].gl_Position.w;
//    vec2 p1 = ratio * gs_in[1].FragPos;
//    vec2 p2 = ratio * gs_in[2].FragPos;

    vec2 v0 = p2 - p1;
    vec2 v1 = p2 - p0;
    vec2 v2 = p1 - p0;
    float area = abs(v1.x*v2.y - v1.y * v2.x);

    dist = vec3(area/length(v0),0,0);
    gs_out.FragPos = gs_in[0].FragPos;
    gs_out.Normal = gs_in[0].Normal;
    gl_Position = gl_in[0].gl_Position;
    gs_out.dist = dist;
    EmitVertex();

    dist = vec3(0,area/length(v1),0);
    gs_out.FragPos = gs_in[1].FragPos;
    gs_out.Normal = gs_in[1].Normal;
    gl_Position = gl_in[1].gl_Position;
    gs_out.dist = dist;
    EmitVertex();

    dist = vec3(0,0,area/length(v2));
    gs_out.FragPos = gs_in[2].FragPos;
    gs_out.Normal = gs_in[2].Normal;
    gl_Position = gl_in[2].gl_Position;
    gs_out.dist = dist;
    EmitVertex();

    EndPrimitive();
}