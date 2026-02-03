#version 400
#define MAX_NUM_LIGHTS 10

in vec3 normal;
in vec3 f_pos;
in vec2 TexCoord;

out vec4 frag_color;

struct Material
{
	sampler2D texture;
	vec3 specular;
	float shininess;
};
uniform Material material;

uniform vec3 campos;

struct DirLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;
	float constant;
	float linear;
	float quadratic;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	float cutOff;      // cos(inner)
	float outerCutOff; // cos(outer)
	float constant;
	float linear;
	float quadratic;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

vec3 CalcDirLight(DirLight light, vec3 norm, vec3 viewDir, vec3 albedo, vec3 matSpec, float shininess)
{
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), max(shininess, 1e-4));
	vec3 ambient = light.ambient * albedo;
	vec3 diffuse = light.diffuse * diff * albedo;
	vec3 specular = light.specular * spec * matSpec;
	return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir, vec3 albedo, vec3 matSpec, float shininess)
{
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), max(shininess, 1e-4));

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient = light.ambient * albedo; // ambient обычно не затухает
	vec3 diffuse = light.diffuse * diff * albedo;
	vec3 specular = light.specular * spec * matSpec;

	diffuse *= attenuation;
	specular *= attenuation;

	return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir, vec3 albedo, vec3 matSpec, float shininess)
{
	vec3 lightDir = normalize(light.position - fragPos);

	float theta = dot(lightDir, normalize(-light.direction));

	float epsilon = max(light.cutOff - light.outerCutOff, 1e-6);
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	vec3 ambient = light.ambient * albedo;

	if (intensity <= 0.0)
	{
		return ambient;
	}

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), max(shininess, 1e-4));

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 diffuse = light.diffuse * diff * albedo;
	vec3 specular = light.specular * spec * matSpec;

	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return ambient + diffuse + specular;
}

struct LightsInfo
{
	DirLight dirLight;
	PointLight pointLights[MAX_NUM_LIGHTS];
	SpotLight spotLights[MAX_NUM_LIGHTS];
	int numPLights;
	int numSLights;
};
uniform LightsInfo lights;

void main()
{
	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(campos - f_pos);
	vec3 albedo = texture(material.texture, TexCoord).rgb;
	vec3 matSpec = material.specular;
	float shininess = material.shininess;

	vec3 result = CalcDirLight(lights.dirLight, norm, viewDir, albedo, matSpec, shininess);

	for (int i = 0; i < lights.numPLights; ++i)
		result += CalcPointLight(lights.pointLights[i], norm, f_pos, viewDir, albedo, matSpec, shininess);

	for (int i = 0; i < lights.numSLights; ++i)
		result += CalcSpotLight(lights.spotLights[i], norm, f_pos, viewDir, albedo, matSpec, shininess);

	frag_color = vec4(result, 1.0);
}