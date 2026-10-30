# P09: Animación Básica

## Objetivo

Implementar dos animaciones automáticas dentro de una escena espacial:

1. Una nave recorre un circuito cerrado con forma de cuadrado de esquinas redondeadas, con rectas, curvas, una subida y una bajada.
2. Un satélite vuela describiendo una trayectoria en forma de moño, con sus paneles y su aspa animados automáticamente.

La implementación se concentra en `P09-424104230.cpp`. El código no lleva comentarios; esta especificación conserva las fórmulas, medidas y decisiones de diseño.

## Nota sobre las guías

El dibujo de las guías —la pista punteada de la nave y la trayectoria punteada del satélite— fue realizado con ayuda de IA.

## Escena

- Planeta estático en el origen, escala 7.
- Skybox nebular.
- Guía punteada cian para la nave y guía punteada magenta para los dos lóbulos del satélite.
- Nave espacial con las llamas de ambos motores siempre encendidas.
- Satélite jerárquico formado por base, panel derecho, panel izquierdo y aspa.
- Textura blanca base `Textures/plain.png` cargada explícitamente para la guía y las piezas que no traen textura propia.

## Circuito

Se usa una máquina de estados. Cada estado representa un tramo geométrico con longitud conocida y una función de evaluación que devuelve posición y tangente para una distancia `s` dentro del tramo.

### Medidas

- Medio lado del cuadrado: 34.
- Radio de cada esquina: 10.
- Longitud de cada lado recto: 48.
- Longitud de cada cuarto de circunferencia: `10 * pi / 2 = 15.707963`.
- Longitud total: `4 * 48 + 4 * 15.707963 = 254.831852`.
- Velocidad de la nave: 12 unidades por segundo.
- Tiempo aproximado por vuelta: 21.24 segundos.

### Centrado vertical

El modelo del planeta ocupa `y ∈ [-1, 1]`; con escala 7 queda `y ∈ [-7, 7]` y su centro es 0. El circuito usa un rango simétrico:

- Altura mínima: -12.
- Altura máxima: 12.
- Centro: `(12 + (-12)) / 2 = 0`, igual al centro del planeta.
- Desnivel total: 24.

Las pendientes resultantes son 0.25 (≈14°) en la subida derecha, -1.53 (≈57°) en el cuarto descendente y 0.5 (≈27°) en la rampa final. Las uniones presentan cambios bruscos de pendiente.

### Estados

| Índice | Estado | Recorrido | Longitud | Altura |
|---|---|---|---|---|
| 0 | `RectaInferior` | `(-24, 0, -34)` a `(24, 0, -34)` | 48 | 0 |
| 1 | `CuartoInferiorDerecho` | centro XZ `(24, -24)`, ángulo -90° a 0° | 15.707963 | 0 |
| 2 | `RectaDerecha` | `(34, 0, -24)` a `(34, 12, 24)` | 48 | sube de 0 a 12 |
| 3 | `CuartoSuperiorDerecho` | centro XZ `(24, 24)`, ángulo 0° a 90° | 15.707963 | baja de 12 a -12 |
| 4 | `RectaSuperior` | `(24, -12, 34)` a `(-24, -12, 34)` | 48 | -12 |
| 5 | `CuartoSuperiorIzquierdo` | centro XZ `(-24, 24)`, ángulo 90° a 180° | 15.707963 | -12 |
| 6 | `RectaFinal` | `(-34, -12, 24)` a `(-34, 0, -24)` | 48 | -12 y a mitad sube de -12 a 0 |
| 7 | `CuartoInferiorIzquierdo` | centro XZ `(-24, -24)`, ángulo 180° a 270° | 15.707963 | 0 |

### Evaluación de tramos

Cada cuarto de circunferencia se evalúa como:

```text
pos.x = centro.x + radio * cos(theta)
pos.z = centro.z + radio * sin(theta)
theta = theta_inicial + s / radio
tangente = (-sin(theta), pendiente_vertical, cos(theta))
```

Las rectas usan una dirección constante. En `RectaDerecha` la altura crece linealmente de 0 a 12. En `CuartoSuperiorDerecho` la altura decrece linealmente de 12 a -12 mientras el ángulo avanza. En `RectaFinal` la altura permanece en -12 durante la primera mitad y sube linealmente hasta 0 durante la segunda mitad.

La tangente de cada tramo incluye la componente vertical correspondiente a su pendiente. La nave suaviza su orientación con una interpolación exponencial hacia la tangente actual para evitar cambios bruscos de cabeceo en las uniones.

### Avance de la máquina

```text
offsetTramo += velocidad * dt

mientras offsetTramo >= longitudTramo(tramoActual):
    offsetTramo -= longitudTramo(tramoActual)
    tramoActual = (tramoActual + 1) mod 8
```

La guía punteada se precalcula una sola vez recorriendo cada tramo en pasos de 3 unidades.

## Nave

- Se coloca 1.3 unidades sobre la guía.
- Su orientación se construye con la tangente suavizada y el vector mundial `(0, 1, 0)`.
- No hay banqueo ni inclinación lateral.
- Las llamas se dibujan siempre en los dos motores, con un pulso leve.
- Las dos luces puntuales azules de los motores permanecen encendidas y siguen la posición de cada motor.

## Satélite

La máquina de estados tiene dos lóbulos. Una sola función evalúa la posición para el estado actual y un ángulo `theta`:

```text
lado = -1 si estado = 0, +1 si estado = 1

x = lado * 16 * (1 - cos(theta))
y = 26 + 2 * sin(2 * theta)
z = 12 * sin(theta)
```

La tangente se obtiene derivando la función:

```text
dx/dtheta = lado * 16 * sin(theta)
dy/dtheta = 4 * cos(2 * theta)
dz/dtheta = 12 * cos(theta)
```

En `theta = 0` y en `theta = 2*pi` ambos estados pasan por `(0, 26, 0)` con la misma tangente, por lo que el cambio de lóbulo es continuo. El conjunto de los dos recorridos se ve como un moño.

- `thetaSatelite` avanza con velocidad angular 0.35 rad/s.
- Al completar `2*pi`, resta `2*pi` y cambia al otro lóbulo.
- La base se orienta en yaw hacia la tangente.
- Los paneles oscilan con `25 * sin(0.9 * t)` grados, con signo opuesto entre ellos.
- El aspa gira de forma continua a 180 grados por segundo.

### Color e iluminación

El satélite usa una luz direccional blanca exclusiva, con ambiente 0.8 y difusa 0.9, orientada desde el satélite hacia la cámara. Esta luz se aplica únicamente al dibujar el satélite; después se restaura la luz principal.

- Base y aspa: color `(1.2, 1.2, 1.2)` sobre la textura blanca base.
- Paneles: color `(0.15, 0.4, 1.2)`.
- Base y aspa usan `materialBrillante`; los paneles usan `materialOpaco`.

El resultado es un satélite blanco con sombreado y paneles azules.

### Guía del satélite

La guía del satélite se muestrea con el mismo esquema que la pista:

- Para cada lóbulo se recorre `theta` de 0 a `2*pi`.
- En cada paso se guardan posición y tangente.
- El avance angular es `guiaPaso / |dP/dtheta|`, usando las derivadas de la trayectoria, para que las marcas queden aproximadamente cada 3 unidades.
- Se dibujan los dos lóbulos completos del moño.
- Color magenta/violeta `(1.0, 0.35, 0.85)`.
- Tamaño de marca: ancho 0.28, largo 1.5.
- Se usa blending aditivo, `plainTexture` y el quad unitario.

Las marcas se dibujan después de la guía de la nave y antes de la nave y el satélite.

## Cámara y luces

- Cámara libre con `W/A/S/D` y ratón.
- Posición inicial: `(0, 30, -85)`.
- Yaw inicial: 90 grados.
- Pitch inicial: -8 grados.
- Velocidad de desplazamiento: 20.
- Luz direccional blanca con ambiente 0.30 y difusa 0.70.
- Dos luces puntuales azules en los motores.
- Sin luces spot.

## Actualización temporal

`dt` se obtiene de GLFW y se limita al intervalo `[0, 0.05]`. Todas las animaciones usan ese mismo valor. No hay pausa.

## Validación

Antes de generar capturas se debe compilar y ejecutar la escena para comprobar:

- El circuito cierra completamente y tiene cuatro esquinas redondeadas.
- El rango vertical va de -12 a 12 y está centrado con respecto al planeta.
- El lado derecho sube, la esquina superior derecha baja y la recta final recupera la altura inicial.
- La nave recorre todo el circuito sin banqueo y con las llamas encendidas.
- El satélite se ve blanco, con paneles azules, describe el moño y mueve sus paneles y aspa.
- Se ven las dos guías: cian para la nave y magenta para los dos lóbulos del satélite.
- El planeta, el skybox y la guía punteada se ven correctamente.
- No hay elementos adicionales de interfaz ni efectos fuera de la especificación.

Las capturas se realizarán únicamente después de que el usuario valide la ejecución y dé el visto bueno.
