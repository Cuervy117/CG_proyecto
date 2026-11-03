#version 330
// NUEVO (no viene en la practica): shader del reloj en pantalla (Reloj.cpp).
// Las coordenadas estan en pixeles (proyeccion ortogonal, sin camara) y toffset elige la celda del
// atlas de numeros, igual que los numeros de la practica 9.
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
