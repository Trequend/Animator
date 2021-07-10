#version 430

uniform vec3 u_Color;
uniform sampler2D u_Texture;

in vec2 v_TexCoords;

out vec4 FragColor;

void main()
{
	FragColor = texture(u_Texture, v_TexCoords) * vec4(u_Color, 1.0f);
}
