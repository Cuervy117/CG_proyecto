#version 330
in vec3 TexCoords;
out vec4 color;
uniform samplerCube skybox;

// NUEVO (no viene en la practica): ambientacion de cada lugar (Skybox::SetAmbiente y SetCieloSolido).
// Con los valores iniciales el skybox se ve igual que en la practica.
uniform vec3 tinteCielo = vec3(1.0);		// multiplica el cubemap: cambia el tono del cielo
uniform vec4 brumaHorizonte = vec4(0.0);	// rgb = color de la bruma, a = intensidad (0 = sin bruma)
uniform vec4 cieloSolido = vec4(0.0);		// cielo despejado: rgb = color arriba, a = cuanto reemplaza al cubemap

void main()
{
	vec4 c = texture(skybox, TexCoords);
	c.rgb *= tinteCielo;

	// TexCoords = -pos en skybox.vert, asi que la direccion real de la vista es -TexCoords
	float altura = normalize(-TexCoords).y;
	float bruma = brumaHorizonte.a * (1.0 - smoothstep(-0.10, 0.55, altura));
	c.rgb = mix(c.rgb, brumaHorizonte.rgb, clamp(bruma, 0.0, 1.0));

	// cielo despejado (planeta oceano): degradado del horizonte hacia arriba
	if (cieloSolido.a > 0.0)
	{
		vec3 degradado = mix(brumaHorizonte.rgb, cieloSolido.rgb, smoothstep(0.0, 0.6, max(altura, 0.0)));
		c.rgb = mix(c.rgb, degradado, cieloSolido.a);
	}
	color = c;
}
