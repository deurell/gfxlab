precision highp float;
precision highp int;

struct Material {
  sampler2D diffuse;
  vec3 diffuseFallback;
  vec3 ambientFallback;
  vec3 specular;
  float shininess;
};

struct DirLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct PointLight {
  vec3 position;
  float constant;
  float linear;
  float quadratic;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform float iTime;
uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
#define POINT_LIGHTS 2
uniform PointLight pointLights[POINT_LIGHTS];

out vec4 FragColor;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
  vec3 norm = normalize(Normal);
  vec3 viewDir = normalize(viewPos - FragPos);

  vec3 result = CalcDirLight(dirLight, norm, viewDir);

  for (int i = 0; i < POINT_LIGHTS; i++) {
    result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
  }

  FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.direction);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 ambient =
      (material.ambientFallback == vec3(0, 0, 0))
          ? light.ambient * vec3(texture(material.diffuse, TexCoords))
          : light.ambient * material.ambientFallback;
  vec3 diffuse =
      (material.diffuseFallback == vec3(0, 0, 0))
          ? light.diffuse * diff * vec3(texture(material.diffuse, TexCoords))
          : light.diffuse * diff * material.diffuseFallback;
  vec3 specular = light.specular * (spec * material.specular);
  return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance +
                             light.quadratic * (distance * distance));
  vec3 ambient =
      (material.ambientFallback == vec3(0, 0, 0))
          ? light.ambient * vec3(texture(material.diffuse, TexCoords))
          : light.ambient * material.ambientFallback;
  vec3 diffuse =
      (material.diffuseFallback == vec3(0, 0, 0))
          ? light.diffuse * diff * vec3(texture(material.diffuse, TexCoords))
          : light.diffuse * diff * material.diffuseFallback;
  vec3 specular = light.specular * (spec * material.specular);
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular);
}
