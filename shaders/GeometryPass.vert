#version 330
layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 ViewPos;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
	ViewPos = vec3(view * model * vec4(position, 1.0f));
}