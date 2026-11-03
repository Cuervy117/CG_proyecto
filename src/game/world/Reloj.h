#pragma once
// Reloj en pantalla (HUD), siempre visible encima de la escena:
//  - hora del ciclo de día y noche (HH:MM y AM/PM),
//  - marcador de la fase del día (luna - sol - luna),
//  - clima del lugar (espacio, selva o desierto) y número de día.
// Los dígitos usan la técnica de la práctica 9: un atlas de números en rejilla de 4 x 3
// (hud_digitos.png) y el uniform toffset para elegir qué número se ve en cada cuadro.

// Carga el shader del HUD y sus texturas. Llamar una vez, con OpenGL ya iniciado.
void InicializarReloj();

// horaDelDia: de 0 a 24. dia: contador de días.
// clima: 0 = espacio, 1 = selva (nublado), 2 = desierto, 3 = océano (despejado).
// De 6:00 a 18:00 el letrero dice DÍA con un sol; de 18:00 a 6:00 dice NOCHE con una luna.
// Dibujar al final del cuadro (después de toda la escena).
void DibujarReloj(int anchoVentana, int altoVentana, float horaDelDia, int dia, int clima);
