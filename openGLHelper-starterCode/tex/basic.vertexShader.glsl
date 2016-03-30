#version 150

in vec3 vpos;
in vec2 vtex;
out vec2 tex;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

void main()
{
  tex=vtex;
  gl_Position=projectionMatrix*modelViewMatrix*vec4(vpos,1.0);
}

