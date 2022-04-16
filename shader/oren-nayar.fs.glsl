#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gWireframe;


// Material properties

uniform float specular_power = 5.0;
uniform float roughness = 1.0;
uniform float ambient_intensity = 0.1f;
uniform vec3 light_pos = vec3(5.f,3.f,-2.f);

void main(void)
{
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;

    vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 View = normalize(-FragPos);
    vec3 Light = normalize(light_pos - FragPos);

    float LdotV = dot(Light,View);
    float NdotL = dot(Light,Normal);
    float NdotV = dot(Normal, View);

    float Specular = texture(gAlbedoSpec, TexCoords).a;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;

    float s = LdotV - NdotL * NdotV;
    float t = mix(1.0,max(NdotL,NdotV),step(0.0,s));

    float sigma2 = roughness * roughness;
    float A = 1. - .5 * (sigma2/(((sigma2 + .33) + .000001)));
    float B = .45 * sigma2 / ((sigma2 + .09) + .00001);
    float ga = dot(View-Normal*NdotV,Normal-Normal*NdotL);
    float fin =  max(0., NdotL) * (A + B * max(0., ga) * sqrt(max((1.0-NdotV*NdotV)*(1.0-NdotL*NdotL), 0.)) / max(NdotL, NdotV));


    vec3 Reflection = reflect(-Light,Normal);
    vec3 ambient = Diffuse * ambient_intensity;
    vec3 diffuse = fin * Diffuse;
    vec3 specular = pow(max(dot(Reflection,View),0.0), 60.0) * vec3(Specular);


    // Write final color to the framebuffer
    FragColor = vec4( diffuse + ambient + specular,1.f);
}