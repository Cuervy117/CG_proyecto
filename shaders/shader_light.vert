#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;

out vec4 vCol;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 vColor;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform vec3 color;
uniform vec2 toffset;	//textura con movimiento (practica 9): desplazamiento de las coordenadas UV


void main()
{
	gl_Position = projection * view * model * vec4(pos, 1.0);
	vCol = vec4(0.0, 1.0, 0.0, 1.0f);
	vColor=vec4(color,1.0f);
	TexCoord = tex + toffset;	//con toffset = (0,0) la textura queda quieta
	//para tomar las Transformaciones geometricas y la transpuesta es para que las escalas se inviertan en caso de escalas no uniformes en las normales
	Normal = mat3(transpose(inverse(model))) * norm;
	
	FragPos = (model * vec4(pos, 1.0)).xyz; //para obtener un vec3 solo posicion con respecto al origen en orden xyz: Swizzling 
}
