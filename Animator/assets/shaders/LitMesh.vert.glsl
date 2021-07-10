#version 430

layout (location=0) in vec3 a_Position;
layout (location=1) in vec3 a_Normal;
layout (location=2) in vec2 a_TexCoords;

struct LightInfo
{
	vec4 Ambient;
	vec4 Diffuse;
	vec4 Specular;
};

uniform LightInfo u_LightInfo;

struct Material
{
	vec4 Specular;
	float Shininess;
};

uniform Material u_Material;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ModelMatrix;

out vec2 v_TexCoords;
out vec3 v_LightIntensity;

void main()
{
	mat3 normalMatrix = transpose(inverse(mat3(u_ModelMatrix)));
	vec3 normal = normalize(normalMatrix * a_Normal);

	vec4 eyeCoords = u_ViewMatrix * u_ModelMatrix * vec4(a_Position, 1.0);
	vec3 s = normalize(vec3(vec4(0.0, 0.0, 1.0, 0.0) - eyeCoords));
	vec3 v = normalize(-eyeCoords.xyz);
	vec3 r = reflect(-s, normal);
	vec3 ambient = vec3(u_LightInfo.Ambient);
	float sDotN = max(dot(s, normal), 0.0);
	vec3 diffuse = vec3(u_LightInfo.Diffuse) * sDotN;
	vec3 specular = vec3(0.0);
	if (sDotN > 0.0)
	{
		specular = vec3(u_LightInfo.Specular) * vec3(u_Material.Specular) * pow(max(dot(r, v), 0.0), u_Material.Shininess);
	}

	v_LightIntensity = ambient + diffuse + specular;
	v_TexCoords = a_TexCoords;
	gl_Position = u_ProjectionMatrix * eyeCoords;
}
