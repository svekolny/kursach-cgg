#version 400
in vec3 vertex_position;
in vec3 vertex_normal;
uniform mat4 transform;
uniform mat4 project;
uniform mat4 view;

out vec3 normal;
out vec3 f_pos;

void main ()
{
 normal = mat3 (transpose(inverse(transform))) * vertex_normal;
 f_pos = vec3 (transform * vec4(vertex_position, 1.0));
 gl_Position = project * view * transform * vec4 (vertex_position, 1.0);
}
