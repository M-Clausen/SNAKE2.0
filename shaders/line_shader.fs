#version 330 core
uniform vec4 received_color;
out vec4 color;
 
void main()
{
    color = received_color;
}