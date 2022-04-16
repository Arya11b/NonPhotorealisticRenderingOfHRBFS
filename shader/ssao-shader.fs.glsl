#version 330 core

// part of the code is reused from https://learnopengl.com/Advanced-Lighting/SSAO

out float FragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;

uniform sampler2D texNoise;

// Material properties

uniform float specular_power = 5.0;
uniform float ambient_intensity = 0.1f;
uniform vec3 light_pos = vec3(5.f,3.f,-2.f);

const vec2 noiseScale = vec2(800.0/4.0, 800.0/4.0);

// TODO: Make uniform [done]
uniform int kernelSize = 64;
uniform float radius = 0.5;
uniform float bias = 0.025;


uniform vec3 samples[64];
uniform mat4 projection;

void main(void)
{
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;

    vec3 tangent = normalize(randomVec - Normal*dot(randomVec,Normal));
    vec3 bitangent = cross(Normal, tangent);
    mat3 TBN = mat3(tangent,bitangent,Normal);

    float occlusion =0.0;
    for(int i = 0; i < kernelSize;i++){
        // get sample position
        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
        samplePos = FragPos + samplePos * radius;

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        // get sample depth
        float sampleDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(FragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / kernelSize);

    // Write final color to the framebuffer

    // for normal rendering swap the commented lines
    FragColor = occlusion;
}