#version 400
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 Texture;

uniform mat4 transform;
uniform mat4 project;
uniform mat4 view;

out vec2 TexCoord;
out vec3 f_pos;
out vec3 normal;

void main ()
{
	f_pos = vec3(transform * vec4(Position, 1.0));
	normal = mat3(transpose(inverse(transform))) * Normal;

	gl_Position = project * view * transform * vec4(Position, 1.0);
	TexCoord = Texture;
}