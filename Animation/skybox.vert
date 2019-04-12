#version 330 core

in  vec3 in_Position;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 modelview;

void main()
{
    TexCoords = in_Position;
    gl_Position = projection * modelview * vec4(in_Position, 1.0);
}  