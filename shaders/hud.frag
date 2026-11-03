#version 330
// Sin luces ni niebla: la textura multiplicada por un color (rgb = tinte, a = opacidad)
in vec2 TexCoord;
out vec4 color;
uniform sampler2D theTexture;
uniform vec4 colorHud = vec4(1.0);
void main()
{
	vec4 c = texture(theTexture, TexCoord) * colorHud;
	if (c.a < 0.01)
		discard;
	color = c;
}
