#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture1;

struct PointLight {
    vec3 position; // 16

    vec3 ambient; // 16
    vec3 diffuse; // 16
    vec3 specular; // 16

    float constant; // 16
    float linear; // 16
    float quadratic; // 16
}; // 112

struct DirLight {
    vec3 direction; // 16

    vec3 ambient; // 16
    vec3 diffuse; // 16
    vec3 specular; // 16
}; // 64 total

#define MAX_DIR_LIGHTS 4
#define MAX_POINT_LIGHTS 8

layout (std140) uniform Lights {
    int dirLightsCount;
    DirLight dirLights[MAX_DIR_LIGHTS];
    int pointLightsCount;
    PointLight pointLights[MAX_POINT_LIGHTS];
};

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 pixel) {
    // ambient
    vec3 ambient = light.ambient * pixel;

    // diffuse
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * pixel;

    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;

    return ambient + diffuse;
}

vec3 CalulateDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 pixel) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0f);

    vec3 reflectDir = reflect(-lightDir, normal);

    vec3 ambient = light.ambient * pixel;
    vec3 diffuse = light.diffuse * diff * pixel;

    return (ambient + diffuse);
}

void main()
{
    vec4 texColor = texture(texture1, TexCoord);
    vec3 viewPos = vec3(0.0, 10.0, 10.0);

    if (texColor.a < 0.1)
    discard;

    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);
    for (int i = 0; i < dirLightsCount; i++) {
        result += CalulateDirLight(dirLights[i], normal, viewDir, vec3(texColor));
    }
    for (int i = 0; i < pointLightsCount; i++) {
        result += CalculatePointLight(pointLights[i], normal, viewDir, vec3(texColor));
    }

    FragColor = vec4(result, 1.0);
}