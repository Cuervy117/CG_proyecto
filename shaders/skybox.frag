#version 330
in vec3 TexCoords;
out vec4 color;
uniform samplerCube skybox;

// Ambientacion por planeta (Skybox::SetAmbiente). Con los valores iniciales se ve igual que antes.
uniform vec3 tinteCielo = vec3(1.0);       // multiplica el cubemap: cambia el tono del cielo
uniform vec4 brumaHorizonte = vec4(0.0);   // rgb = color de la bruma, a = intensidad (0 = sin bruma)

void main()
{
	vec4 c = texture(skybox, TexCoords);
	c.rgb *= tinteCielo;

	// TexCoords = -pos en skybox.vert, asi que la direccion real de la vista es -TexCoords
	float altura = normalize(-TexCoords).y;
	float bruma = brumaHorizonte.a * (1.0 - smoothstep(-0.10, 0.55, altura));
	c.rgb = mix(c.rgb, brumaHorizonte.rgb, clamp(bruma, 0.0, 1.0));
	color = c;
}
