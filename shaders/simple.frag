#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture1;

struct DirLight {
    vec3 direction; // 16

    vec3 ambient; // 16
    vec3 diffuse; // 16
    vec3 specular; // 16
}; // 64 total

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

    DirLight light;
    light.direction = vec3(0.0, -1.0, 0.0);
    light.ambient = vec3(0.4, 0.4, 0.4);
    light.diffuse = vec3(0.6, 0.6, 0.6);
    light.specular = vec3(1.0, 1.0, 1.0);

    result += CalulateDirLight(light, normal, viewDir, texColor.rgb);

    FragColor = vec4(result, 1.0);
}