#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gWireframe;

uniform sampler2D ssao;

// Material properties

uniform float specular_power = 5.0;
uniform float ambient_intensity = 0.35f;
uniform bool toggle_ssao = false;
uniform bool toggle_ambient_pass = false;
uniform vec3 light_pos = vec3(5.f,3.f,-2.f);

void main(void)
{
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;

    vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 View = normalize(-FragPos);
    vec3 Light = normalize(light_pos - FragPos);

    float ssaoTexture = texture(ssao,TexCoords).r;

    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    vec3 Reflection = reflect(-Light,Normal);
    vec3 ambient = Diffuse * ambient_intensity * ssaoTexture;
    if(toggle_ambient_pass) ambient = Diffuse * ambient_intensity;


    vec3 diffuse = max(dot(Normal,Light),0.0) * Diffuse;
    vec3 specular = pow(max(dot(Reflection,View),0.0), 60.0) * vec3(Specular);

    vec3 wireframe = texture(gWireframe, TexCoords).rgb ;

    float nearD = min(min(wireframe[0],wireframe[1]),wireframe[2]);
    float edgeIntensity = exp2(-1.0*nearD*nearD);


    // Write final color to the framebuffer

    // for normal rendering swap the commented lines
    vec4 FragOut = vec4(ambient + diffuse + specular,1.f);
    if(toggle_ssao) FragOut = vec4(ssaoTexture,ssaoTexture,ssaoTexture,1.0);
    FragColor = FragOut;
    //FragColor = vec4(ambient + diffuse + specular,1.f);
    //FragColor = (edgeIntensity * vec4(0.1,0.1,0.1,1.0)) + ((1.0-edgeIntensity) * vec4(ambient + diffuse + specular,1.f));
}