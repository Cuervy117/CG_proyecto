# Guion de video demo (sin narración)

**Objetivo:** mostrar la funcionalidad completa del proyecto final para defender los 20 puntos del video: archivo entregado (10) y contenido (10). Los lineamientos piden expresamente **sin narración de voz**. Duración sugerida: **7–9 minutos**; la rúbrica no fija duración.

**Fuente de imagen:** grabación continua o tomas cortas del **mismo Release que se entregue**. Indicar versión y commit reales en la primera y última tarjeta. Capturas fijas sirven como apoyo breve, nunca para sustituir la prueba de movimiento. Grabar el HUD y el reloj visibles. Rótulos cortos, sin afirmar que algo funciona si no se ve en pantalla.

## Preparación de la sesión

1. Abrir el paquete Release extraído con `Jugar.cmd`; hacer una prueba corta de teclado, ratón y grabación.
2. Grabar cada toma dos veces, dejando 2–3 segundos antes y después de la acción para facilitar los cortes.
3. Mantener resolución horizontal uniforme. Evitar cursor, notificaciones, escritorio y ventanas que tapen el escenario.
4. Conservar clips originales por separado (`01_espacio`, `02_roca`, etc.). No usar la prueba automática `--probar-release` como sustituto de las tomas guiadas.
5. Si la tecla o la posición no produce el efecto previsto, corregir la toma. No montar una secuencia que dé a entender una acción no ejecutada.

## Montaje propuesto

Los tiempos son aproximados y se ajustan a la duración real de cada animación. Texto entre comillas = rótulo que aparece en pantalla, no voz.

| Tiempo | Imagen y acción que debe quedar visible | Rótulo breve | Criterios cubiertos |
|---|---|---|---|
| 0:00–0:12 | Portada limpia con título, los tres nombres, versión y commit del Release. | “Viajes espaciales y evolución de especies intergalácticas” | Identificación del producto |
| 0:12–0:40 | Abrir el juego. En espacio, plano general de base, pista, estación, planetas, luna/asteroides y satélite. Girar la vista lentamente. | “Escenario 3D: espacio y planetas” | Geometría, ambientación, cuerpos celestes |
| 0:40–1:30 | F6 y F1: roca/Vacío. Caminar con WASD mostrando a Rek’Sai de perfil y por detrás; acercarse a dos criaturas diferentes, cristales, portal y luminarias. Dejar visible la animación de patas. | “R1 · Vacío: avatar articulado y fauna adaptada” | Avatar no humanoide, textura/material, caminata, NPC, planeta 1 |
| 1:30–2:15 | F5 y F1: selva/Shinobi. Caminar por el camino; mostrar NPC, vegetación y referencia del universo. Encadenar con una vista breve de F7 océano si aporta al entorno. | “R1 · Shinobi: ecosistema y especies distintas” | Planeta 2, evolución visual, ambientación, skybox |
| 2:15–2:55 | En una superficie, enfocar la cápsula y pulsar 2. Mostrar puerta cerrada, transición completa y abierta. | “Animación por teclado 1/3 · puerta de cápsula (2)” | Animación básica, texturas |
| 2:55–3:35 | F2: seguir la nave del jugador durante un tramo, W acelera y A/D cambia de carril. Encuadrar al menos una nave rival en movimiento. | “R2 · carrera: nave propia y rivales” | Circuito, cámaras, ciclo continuo 1/3 |
| 3:35–4:10 | F3: vuelo en espacio. Mostrar nave y cambio de rumbo; Q/E para bajar/subir, acercamiento a dos planetas. Mantener contexto espacial en cuadro. | “R3 · vuelo entre planetas” | Vuelo, cámara de tercera persona, cambios de vista |
| 4:10–5:05 | F4: cohete Shinobi. Mostrar estado inicial; pulsar 1 y conservar ignición, ascenso y órbita. No cortar antes de que se vea la transición. | “R4 · lanzamiento (1)” | Plataforma/cohete, animación por teclado 2/3 |
| 5:05–5:45 | Tab hasta el hangar. Mantener una toma suficiente para ver Fighter_02 saliendo y regresando tras pulsar 4, con la puerta abriéndose antes y cerrándose después. | “R4 · estación: entrada y salida” | Estación, máquina de estados, puerta por teclado |
| 5:45–6:15 | Tab hasta satélite. Pulsar 3 y mostrar paneles plegados y abiertos. Añadir una vista corta de asteroides y luna si la toma inicial no los dejó claros. | “Animación por teclado 3/3 · paneles (3), ciclo continuo 3/3 · satélite” | Satélite, cuerpos celestes, animación básica |
| 6:15–6:50 | Volver a roca o selva. Plano quieto de NPC con movimiento continuo durante varios ciclos; rótulo del tipo de movimiento visible. | “Ciclo continuo 2/3 · criatura” | Animación compleja basada en función |
| 6:50–7:30 | Comparar el cielo de roca y selva con F6/F5. Grabar un ciclo real de 120 s del reloj en una misma zona y acelerarlo en edición, declarando la aceleración; mostrar luces puntuales apagadas/encendidas. Alternar F/L/K en una toma nocturna para probar los focos. | “Día/noche: 120 s reales (reproducción ×4)” y “Luminarias y focos” | Sol, puntuales sincronizadas, skyboxes, spotlights |
| 7:30–7:45 | Montaje breve F1→F2→F3→F4 con el HUD de modo legible; cierre y créditos. | “Cuatro recorridos · un escenario” | Cambio correcto de cámaras |

Si el video supera 9 minutos por el cohete o la luz, conservar la prueba completa y recortar traslados sin acción. El segmento solar debe provenir de una toma real sin saltos; el rótulo debe explicar cualquier aceleración de reproducción.

## Orden de grabación recomendado

Grabar por ubicación para reducir cargas: (1) espacio/base, R2, R3, hangar y satélite; (2) roca, fauna, puerta y ciclo solar; (3) selva y cohete. El orden de edición puede ser el de la tabla. En F4, **Tab** cambia de objetivo; el cohete requiere un objetivo compatible y **1**. En F2 la nave y rivales deben verse moverse, no solo estar sobre la pista.

## Control antes de exportar

- [ ] No hay voz narrada en ningún segmento del video demo.
- [ ] Se ven ambos planetas, NPC, avatar caminando, estación, pista, cohete y cuerpos celestes.
- [ ] Se ven R1, R2, R3, R4 y el cambio F1–F4.
- [ ] Se distinguen **tres objetos** iniciados por 1, 2 y 3 y **tres objetos** en movimiento continuo.
- [ ] Se aprecia el ciclo día/noche y la reacción conjunta de luminarias, los cielos y focos por teclado.
- [ ] Los rótulos no cubren la acción ni el HUD y tienen tiempo de lectura.
- [ ] Versión y commit coinciden con el ZIP entregado; el MP4 se reproduce de principio a fin.

**Nota de honestidad:** una función que no esté visible o no responda en el Release final no se declara cumplida en los rótulos. Registrar cualquier diferencia real en el manual técnico y la entrega.
