#version 330 core



/////////////////////////////////////
// Uniforms, Attributes, and Outputs
////////////////////////////////////
uniform mat4 u_Persp;

uniform sampler2D u_Depthtex;
uniform sampler2D gNormal;
uniform sampler2D gPosition;
uniform sampler2D u_Colortex;
uniform sampler2D u_RandomNormaltex;
uniform sampler2D u_RandomScalartex;

uniform float u_Far;
uniform float u_Near;
uniform int u_OcclusionType;
uniform int u_DisplayType;

uniform vec4 u_Light;
uniform float u_LightIl;

in vec2 TexCoords;
out float FragColor;



uniform sampler2D texNoise;

// Material properties

uniform float specular_power = 5.0;
uniform float ambient_intensity = 0.1f;
uniform vec3 light_pos = vec3(5.f,3.f,-2.f);

const vec2 noiseScale = vec2(800.0/4.0, 800.0/4.0);

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
uniform float zerothresh = 1.0f;
uniform float falloff = 0.1f;


uniform vec3 samples[64];
uniform mat4 projection;

void main(void)
{

    // depth data
    float exp_depth = texture(u_Depthtex, TexCoords).r;
    float lin_depth = linearizeDepth(exp_depth,u_Near,u_Far);


    // ambient light data
    vec3 light = u_Light.xyz;
    float strength = u_Light.w;

    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition,TexCoords).xyz;
    vec3 Normal = normalize(texture(gNormal,TexCoords).xyz);


    vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;

    vec3 tangent = normalize(randomVec - Normal*dot(randomVec,Normal));
    vec3 bitangent = cross(Normal, tangent);
    mat3 TBN = mat3(tangent,bitangent,Normal);

    // computer SSAO (screen space ambient occlusion)
    // float occlusion = sampleBlurredDepth(TexCoords);

    vec2 sz = textureSize(gPosition, 0);

    float hup    = 0.0;	float tup    = 0.0;
    float hdown  = 0.0;	float tdown  = 0.0;
    float hleft  = 0.0;	float tleft  = 0.0;
    float hright = 0.0;	float tright = 0.0;

    float occlusion =0.0;
    for (float i = 1.0; i <= 6.0; i += 1.0) {

        // neighbor positions
        vec3 posUp   = samplePos(vec2(TexCoords.s, TexCoords.t + (1.0 / sz.y)));
        vec3 posDown = samplePos(vec2(TexCoords.s, TexCoords.t - (1.0 / sz.y)));
        vec3 posRight = samplePos(vec2(TexCoords.s + (1.0 / sz.y), TexCoords.t));
        vec3 posLeft = samplePos(vec2(TexCoords.s - (1.0 / sz.y), TexCoords.t));

        // up occlusion
        vec3 Hup = posUp - position;
        vec3 Tup = cross(normal, vec3(0,1,0));
        if (Hup.z > 0.01) {
            hup += atan(Hup.z / length(Hup.xy)) / 6.0;
            tup += atan(Tup.z / length(Tup.xy)) / 6.0;
        }

        // down occlusion
        vec3 Hdown = posDown - position;
        vec3 Tdown = cross(normal, vec3(0,-1,0));
        if (Hdown.z > 0.01) {
            hdown += atan(Hdown.z / length(Hdown.xy)) / 6.0;
            tdown += atan(Tdown.z / length(Tdown.xy)) / 6.0;
        }

        // right occlusion
        vec3 Hright = posRight - position;
        vec3 Tright = cross(normal, vec3(1,0,0));
        if (Hright.z > 0.01) {
            hright += atan(Hright.z / length(Hright.xy)) / 6.0;
            tright += atan(Tright.z / length(Tright.xy)) / 6.0;
        }

        // left occlusion
        vec3 Hleft = posLeft - position;
        vec3 Tleft = cross(normal, vec3(-1,0,0));
        if (Hleft.z > 0.01) {
            hleft += atan(Hleft.z / length(Hleft.xy)) / 6.0;
            tleft += atan(Tleft.z / length(Tleft.xy)) / 6.0;
        }
    }
    // ambient occlusion term
    float occlusion = (sin(hup) - sin(tup)) / 4.0 + (sin(hdown) - sin(tdown)) / 4.0 + (sin(hright) - sin(tright)) / 4.0 + (sin(hleft) - sin(tleft)) / 4.0;
    FragColor = 1.f - occlusion;
}
/// stolen code :D

/////////////////////////////////////
//				UTILITY FUNCTIONS
/////////////////////////////////////

//Depth used in the Z buffer is not linearly related to distance from camera
//This restores linear depth
float linearizeDepth(float exp_depth, float near, float far) {
    return	(2 * near) / (far + near -  exp_depth * (far - near));
}


//Get a random normal vector  given a screen-space texture coordinate
//Actually accesses a texture of random vectors
vec3 getRandomNormal(vec2 texcoords) {
    ivec2 sz = textureSize(u_RandomNormaltex,0);
    return texture(u_RandomNormaltex,vec2(texcoords.s* (800)/sz.x,
    (texcoords.t)*(800)/sz.y)).rgb;
}

//Get a random scalar given a screen-space texture coordinate
//Fetches from a random texture
float getRandomScalar(vec2 texcoords) {
    ivec2 sz = textureSize(u_RandomScalartex,0);
    return texture(u_RandomScalartex,vec2(texcoords.s*800/sz.x,
    texcoords.t*800/sz.y)).r;
}

const float occlusion_strength = 1.5f;

