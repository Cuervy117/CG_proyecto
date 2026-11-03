#version 330
// Reloj en pantalla: coordenadas en pixeles (proyección ortogonal) y toffset para elegir la celda
// del atlas, igual que los números de la práctica 9.
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
out vec2 TexCoord;
uniform mat4 model;
uniform mat4 projection;
uniform vec2 toffset;
void main()
{
	gl_Position = projection * model * vec4(pos, 1.0);
	TexCoord = tex + toffset;
}
