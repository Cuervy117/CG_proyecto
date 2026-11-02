---
title: Bitácora individual — Venancio (C)
autor: Venancio Miguel Tapia Verastegui
materia: Computación Gráfica e Interacción Humano-Computadora
semestre: 2027-1
proyecto: Proyecto final CGICH
inicio: 2026-10-30
entrega: 2026-11-07
estado: en curso
---

# Bitácora individual — Venancio Miguel Tapia Verastegui (C)

> [!info] Propósito
> Registro individual del trabajo en el proyecto final. Cumple el requisito del PDF: **qué me propuse hacer, qué hice y cuántas horas trabajé**, por día.
> Además se lleva un **registro por hitos** con fecha y hora: se anota al cerrar cada hito, con información suficiente para describirlo.

## 0. Regla de registro

1. **Se escribe al cerrar el hito**, no al final del proyecto.
2. Cada entrada lleva **fecha y hora**.
3. La descripción es de **1 a 3 líneas**: qué se logró y cómo se verificó.
4. Se adjunta **evidencia**: captura, commit, archivo o ruta.
5. Se anotan las **horas reales** trabajadas, aunque el hito se haya pasado de largo.
6. Si un hito no se termina, **no se marca como hecho**; se deja `en curso` y se anota el bloqueo.
7. Solo el dueño de esta bitácora la edita (§16.6 del plan).

> [!tip] Plantilla de una entrada
> `H-XX · 2026-10-30 · 14:35 · <hito> · <1–3 líneas> · Evidencia: <captura/commit> · <horas> h · hecho`

---

## 1. Datos del proyecto

| Campo | Valor |
|---|---|
| Proyecto | Ambiente virtual interactivo: viajes espaciales y evolución de especies intergalácticas |
| Equipo | 3 integrantes |
| Mi rol | **C — avatar, criatura, entrada, experiencias, HUD e integración** |
| Rutas que edito | `src/game/avatar/`, `src/game/anim/`, `src/game/Experience.*`, `src/game/main.cpp`, `src/ui/`, `Input.*`, `docs/` |
| Stack | C++17 · OpenGL 4.3 (contexto actual; objetivo 3.3) · GLFW · GLEW · GLM · Assimp · Visual Studio 2022 |
| Rama | `feat/c-avatar` |
| Repositorio | `https://github.com/Cuervy117/CG_proyecto.git`; rama `feat/c-avatar` |
| Entrega | sábado 7-nov-2026, 12:00, Google Classroom |
| Herramientas | Visual Studio 2022, Git, IA como apoyo de planificación y revisión |

---

## 2. Planeación vigente — 30-oct-2026

David confirmó el reparto: Jonhatan quedó en B y yo en C. Me toca hacer que Rek’Sai y la criatura N5 funcionen como avatar, conectar controles y modos, y después meterlos al programa y mostrar la información básica en pantalla. B se encarga del mundo y de sus naves/puertas. El plan completo está en `PLAN_MAESTRO.md`.

Los hitos con letra C son las tareas de mi rol. Las rutas `src/...` son carpetas/archivos del código donde se trabaja; no son tareas aparte. Los modelos y sus imágenes deben quedar en sus carpetas para que el programa los encuentre. Cada integrante redacta su parte de la documentación.

| Fecha | Resultado conjunto que debo apoyar | Estado |
|---|---|---|
| 30-oct | Avatar en P1 con cámara dentro de ejecutable común | parcial; falta integración común |
| 31-oct | Ambos planetas y cuatro modos básicos; paquete en otra PC | parcial; falta mundo B e integración |
| 1-nov | Animaciones, iluminación y contenido real | parcial; hay piezas y modelos locales, faltan ciclos e integración |
| 2-nov | Demo completa de requisitos | pendiente |
| 3-nov | QA, Release externo y freeze 18:00 | pendiente |
| 4–5 nov | Manuales, videos, plano/propuesta finales | pendiente |
| 6-nov | Reserva de incidencias | pendiente |
| 7-nov | Acuse antes de 12:00 | pendiente |

Esta tabla habla del resultado de **todo el equipo**, no solo de mi trabajo. `Parcial` quiere decir que ya hay avances, pero todavía falta unirlos o probarlos juntos. `Pendiente` quiere decir que ese resultado común todavía no está listo. Por eso Rek’Sai puede estar funcionando en mi demo y la fecha del equipo seguir marcada como parcial.

---

## 3. Hitos

En esta tabla, **Hora de referencia** es la hora del reloj que anoté para ese avance; no significa cuánto duró. **Tiempo trabajado** es cuánto trabajé realmente. Para no contar lo mismo varias veces, anoto el total por día una sola vez en la sección 4; las filas de hitos apuntan a ese registro.

| ID | Fecha | Hora de referencia | Hito | Qué hice, dicho sencillo | Evidencia | Tiempo trabajado | Estado |
|---|---|---|---|---|---|---|---|
| — | — | — | — | — | — | — | — |
| H-DB0 | 2026-10-17 | No registrada | Recursos Dragon Ball preparados localmente | Reuní modelos de referencia de Gohan/Cámara del Tiempo y preparé siete modelos sencillos propios. Hice esta parte en mi computadora; no la integré al mapa. En el mapa actual no hay un modelo/personaje de Gohan, aunque la propuesta todavía lo menciona como referencia D3; falta confirmarlo con el equipo. | `assets/dragon_ball/LEEME.md`; inventario, verificación y vista previa locales; fuera de `feat/c-avatar` y del Drive revisado | Sin registrar | Preparación local hecha; falta confirmar con el equipo si D3 se conserva |
| H-C0 | 2026-10-31 | 18:42 CDMX | Clon y arranque local | Cloné el proyecto, me cambié a mi rama y junté localmente los cambios de A. Preparé los archivos de Rek’Sai y Gohan D3. En ese primer intento no pude compilar porque faltaban archivos del paquete de dependencias. | `Models/reksai.obj/.mtl`; `docs/recursos-C.md`; commit `aedcd41` | Parte de la hora total de H-C0/H-C1 del 31-oct; ver sección 4 | Parcial en ese momento; build local completado el 2-nov |
| H-C0 actualización | 2026-10-31 | 19:14 CDMX | Textura de Rek’Sai | Copié la imagen de textura de Rek’Sai desde Drive. Aún no podía comprobarla en el programa porque faltaban dependencias. | `Textures/reksai.png`; `docs/recursos-C.md` | Parte de la hora total de H-C0/H-C1 del 31-oct; ver sección 4 | Parcial en ese momento; build local completado el 2-nov |
| H-C1 parcial | 2026-10-31 | 19:57 CDMX | Entrada y controles | Dejé preparada la lectura del teclado, mouse y rueda, con las teclas WASD, Q/E y F1–F4. Falta conectarla a los modos y ubicaciones del programa y probarla junto con el trabajo del equipo. | `Input.cpp`, `Input.hpp`; commit `aedcd41` | Parte de la hora total de H-C0/H-C1 del 31-oct; ver sección 4 | Parcial |
| H-C2 | 2026-10-31 | 20:38 CDMX | Rek’Sai articulada y vista previa | Separé las partes de Rek’Sai para poder moverlas y acomodé el modelo en posición horizontal. La caminata en OpenGL se probó después, el 2-nov. | `src/game/avatar/Avatar.cpp`; `Models/c_reksai_articulada.blend`; `docs/capturas/reksai_preview.png` | ≈3 h en total entre 30 y 31-oct; ver sección 4 (no sumar de nuevo) | Preparación hecha; caminata OpenGL registrada después |
| H-C2 actualización | 2026-11-02 | 11:03 CDMX | Textura de Rek’Sai preparada | Dejé el modelo y la imagen de textura en las carpetas que usa el avatar. Revisé que el modelo tuviera sus partes y datos para colocarle la imagen; faltaba probarlo en el programa. | `Models/reksai.obj`, `Textures/reksai.png`; datos de archivos en `docs/recursos-C.md` | Incluida en el total del 2-nov; ver sección 4 (no sumar de nuevo) | En curso en ese momento |
| H-C2 actualización | 2026-11-02 | Hora del día no anotada | Archivos de Rek’Sai preparados desde Drive | Encontré en el Drive del equipo el modelo y sus archivos de material/imagen. Añadí el archivo que faltaba y corregí dónde busca la imagen. | Drive `Models/reksai/`, `Textures/reksai.png`; `Models/reksai.mtl` local | Incluida en el total del 2-nov; ver sección 4 (no sumar de nuevo) | Preparación hecha; prueba OpenGL registrada después |
| H-C2 actualización | 2026-11-02 | 11:20 CDMX | Revisé la textura en Blender | Abrí el modelo con sus archivos de material e imagen; la textura se vio sobre Rek’Sai. Después también se comprobó en la demo OpenGL. | Captura en Blender; `Models/reksai.obj`, `Models/reksai.mtl`, `Textures/reksai.png` | Incluida en el total del 2-nov; ver sección 4 (no sumar de nuevo) | Revisión en Blender hecha; OpenGL comprobado después |
| H-C2 actualización | 2026-11-02 | 11:52 CDMX | Caminata de Rek’Sai en OpenGL | Conecté el avatar a la escena de prueba para que el movimiento active su caminata. En ese momento el código movía algunas partes; la compilación y revisión final se hicieron después ese día. | `P09-2-424104230.cpp`; `src/game/avatar/Avatar.cpp`; modelo e imagen en `Models/` y `Textures/` | Incluida en el total del 2-nov; ver sección 4 (no sumar de nuevo) | Avance parcial en ese momento; prueba final abajo |
| H-C2 actualización | 2026-11-02 | 13:05 CDMX | Rek’Sai completa en la demo OpenGL | Corregí un error que hacía que el modelo apareciera incompleto. Compilé y abrí la demo: Rek’Sai ya se ve completa, con textura y partes articuladas. También preparé el ZIP con el programa y sus dependencias; falta probarlo con la versión de compilación del equipo e integrarlo al mapa/programa común. | `src/engine/Mesh_tn.cpp`; `P09-2-424104230.cpp`; `x64/Release/LabCGeIHC.local.exe`; [ZIP en Drive](https://drive.google.com/file/d/1y0RRxMR6n6LyBR34hjpAqKC_uwOlasbz/view?usp=drivesdk) | Incluida en el total del 2-nov; ver sección 4 (no sumar de nuevo) | Hecho en mi demo; falta integración común |
| H-C0 actualización | 2026-11-02 | 13:05 CDMX | Programa local abre con los recursos | Cuando recibí los archivos que faltaban, compilé y abrí el programa con Rek’Sai texturizada. Esto completa el arranque local. Aún falta probarlo con la misma versión que usa el equipo. | `x64/Release/LabCGeIHC.local.exe`; [ZIP de dependencias en Drive](https://drive.google.com/file/d/1y0RRxMR6n6LyBR34hjpAqKC_uwOlasbz/view?usp=drivesdk) | Incluida en el total del 2-nov; ver sección 4 (no sumar de nuevo) | Hecho local; prueba con versión del equipo pendiente |
| H-C2 actualización | 2026-11-02 | 13:57 CDMX | Caminata revisada de perfil | Ajusté la vista para ver a Rek’Sai de lado y comprobé que caminaran sus patas, cabeza y cola sin que las piezas se vieran separadas. La abrí en OpenGL; sigue siendo una prueba local y falta unirla al programa/mapa del equipo. | `P09-2-424104230.cpp`; `src/game/avatar/Avatar.cpp`; `Models/reksai.obj`; `Textures/reksai.png` | Incluida en el total del 2-nov; ver sección 4 (no sumar de nuevo) | Comprobado local; integración común pendiente |
| H-C2 actualización | 2026-11-02 | 14:47 CDMX | Caminata más suave | Bajé el movimiento de la cola, hice más lento el de la cabeza y corregí las cuatro patas pequeñas para que quedaran bien pegadas. Volví a compilar y abrir la escena en OpenGL. Falta integrarla al programa común del equipo. | `src/game/avatar/Avatar.cpp`; captura en ventana OpenGL | Incluida en el total del 2-nov; ver sección 4 (no sumar de nuevo) | Comprobado local; integración común pendiente |
| H-C2 actualización | 2026-11-02 | 15:05 CDMX | Cola más lenta y brazos delanteros | Hice la cola todavía más lenta y agregué movimiento suave a los brazos delanteros. Dejé como estaban las cuatro patas pequeñas y la cabeza, que ya te gustaban. Compilé y revisé que no hubiera partes sueltas. | `src/game/avatar/Avatar.cpp`; ventana OpenGL local; [fuentes y README en Drive](https://drive.google.com/drive/folders/1BjhEMXsBatML2Dhk65OjgfRBtdywbhvU); [ZIP](https://drive.google.com/file/d/1pd8Ay-8fg0ZgM6L1xSWHFK8Sy9Y93ocD/view?usp=drivesdk) | Incluida en el total del 2-nov; ver sección 4 (no sumar de nuevo) | Hecho en mi demo OpenGL; falta integración al programa común |
| H-C2 actualización | 2026-11-02 | 15:40 CDMX | Paquete de Rek’Sai en Drive | Subí la demo OpenGL con el código y los archivos del modelo y su textura, organizados en las carpetas que espera el programa. El código está en esta rama y el paquete compartido está en Drive. Falta probarlo con la versión de compilación del equipo e integrarlo al mapa común. | [ZIP completo en Drive](https://drive.google.com/file/d/18WwExs-s2famGcs4KhbadmA4u2tA9Btv/view?usp=drivesdk); [carpeta de demo](https://drive.google.com/drive/folders/1BjhEMXsBatML2Dhk65OjgfRBtdywbhvU); `docs/recursos-C.md` | Incluida en el total del 2-nov; ver sección 4 (no sumar de nuevo) | Paquete en Drive; integración común pendiente |

### Estado de los hitos C al 2-nov

| Tarea | Cómo va | Qué falta |
|---|---|---|
| H-C0 — abrir el proyecto | **Listo en mi PC**: pude abrir el programa y ver los recursos. | Probarlo con la misma versión de herramientas que usa el equipo. |
| H-C1 — controles | **A medias**: ya se leen las teclas y el mouse. | Hacer que esas teclas cambien de verdad el modo y el lugar en el programa del equipo. |
| H-C2 — Rek’Sai | **Lista en mi demo OpenGL y en Drive**: se ve completa, con textura y se mueven patas, cabeza, cola y brazos delanteros. | Probarla con las herramientas del equipo y luego meterla al mapa común (H-C3). |
| Criatura N5 | **Pendiente**: todavía no está hecha en esta rama. | Cuando B deje el punto N5 en el mapa, preparar la criatura para que recorra puntos moviendo patas/cola. |
| H-C3 — avatar en el mapa | **Pendiente**: Rek’Sai funciona en una escena de prueba, pero todavía no está en el mapa/programa principal. | Ponerla en P1, hacer que la cámara la siga y comprobarlo en el programa del equipo. |
| H-C4 — pasar entre mapas | **Pendiente en esta rama**. | Hacer que la tecla E permita salir de un mapa y entrar al otro. |
| H-C5 — modos F1–F4 | **A medias**: las teclas ya están preparadas. | Conectarlas a los cuatro modos del proyecto y comprobar que funcionen. |
| H-C6 — información en pantalla | **Pendiente en esta rama**. | Mostrar en pantalla en qué modo y lugar estás y qué acción puedes hacer. |

### En sencillo: códigos y palabras que aparecen arriba

| Palabra/código | Qué significa aquí |
|---|---|
| H-C0 | Dejar el proyecto listo para abrir y compilar en mi computadora. |
| H-C1 | Hacer que el programa lea los controles del teclado y mouse. |
| H-C2 | Preparar a Rek’Sai y hacer que sus partes se muevan al caminar. |
| H-C3 | Poner el avatar en el mapa principal y hacer que la cámara lo siga. |
| H-C4 | Poder pasar de un mapa a otro con la tecla E. |
| H-C5 | Hacer que F1–F4 cambien entre los modos del proyecto. |
| H-C6 / HUD | Mostrar en pantalla el modo y el lugar donde estás. |
| N5 | Punto del mapa donde B debe dejar un lugar para la criatura que me toca animar. |
| `main` / programa común | La versión principal donde se juntan los trabajos del equipo. |
| P1 | El planeta donde el plan pide colocar primero al avatar. |
| OpenGL | La ventana del programa donde se dibuja y se probó Rek’Sai. |
| OBJ / MTL / PNG | Archivo del modelo / archivo que describe su material / imagen de textura. |
| v143 / v145 | Dos versiones de las herramientas de compilación; falta usar la misma que acordó el equipo. |
| Rama | Una línea separada de trabajo en GitHub; la mía es `feat/c-avatar`. |

---

## 3.1 Trabajo previo de recursos Dragon Ball

Preparé este material en mi computadora el 17-oct, antes de que el reparto vigente me asignara Rek’Sai/avatar e integración. Lo anoto como trabajo previo de Dragon Ball; todavía no está integrado al mapa ni subido a esta rama.

- Se reunieron tres recursos descargables de Gohan SSJ2 y la Cámara del Tiempo, con sus ZIP, OBJ/MTL, texturas y procedencia documentada.
- Con apoyo de Codex generé versiones propias sencillas de una cápsula saiyajin, una estación inspirada en Freezer, un domo de entrenamiento, un cohete, una llama, una plataforma/torre y una pista orbital, en OBJ/MTL/GLB con texturas.
- Se generó una vista previa y se ejecutó la verificación local de 17 archivos OBJ/GLB. Esto comprueba los archivos del paquete, no su carga en el ejecutable del equipo.
- Dejé como evidencia `assets/dragon_ball/LEEME.md`, `inventario_modelos_propios.json`, `verificacion.json` y `vista_previa_modelos_propios.png`. No encontré esos archivos en mi rama `feat/c-avatar` ni en el Drive compartido que revisé. Como en el mapa actual no hay un personaje Gohan y solo se ven naves/cápsulas que abren y cierran, tengo que confirmar en el grupo si D3 sigue siendo parte del proyecto.
- El plan asigna a B las animaciones de naves, puertas y paneles; esa parte no es de mi rol C.
- No anoté cuánto tiempo me tomó; no voy a inventar una cifra.

---

## 4. Registro diario (formato del PDF)

| Fecha | Qué me propuse | Qué hice | Tiempo que trabajé |
|---|---|---|---|
| 2026-10-17 | Preparar recursos de la parte Dragon Ball | Busqué modelos de Gohan y la Cámara del Tiempo y preparé modelos sencillos de cápsula, estación, domo, cohete, llama, plataforma/torre y pista. Hice una vista previa y revisé los archivos en mi computadora; todavía no los integro al programa del equipo. | Sin registrar; no anoté cuánto tiempo me tomó |
| 2026-10-30 | Empezar a preparar a Rek’Sai | Empecé a separar y preparar el modelo para poder animarlo. El total que confirmé para ese trabajo fue de unas 3 h entre el 30 y el 31; no recuerdo cuánto fue cada día. | Parte de ≈3 h entre 30–31 oct; el total se anota una sola vez |
| 2026-10-31 | Preparar el proyecto, controles y avatar | Dejé lista mi rama, junté localmente el trabajo de A, preparé los archivos de Rek’Sai y su textura y avancé los controles. También acomodé el modelo para verlo acostado, como corresponde. El programa todavía no abría porque faltaban archivos; lo resolví el 2-nov. | ≈1 h para clonar/preparar el proyecto y controles; además, este día forma parte de las ≈3 h de Rek’Sai anotadas entre 30–31 oct |
| 2026-11-01 | Animaciones y contenido según rol | No trabajé en el proyecto ese día. | 0 h |
| 2026-11-02 | Hacer que Rek’Sai se viera y caminara bien | Arreglé un problema que hacía que se vieran partes sueltas o faltantes, abrí a Rek’Sai completa y con textura en la demo OpenGL, ajusté la cola y los brazos delanteros, y subí el paquete al Drive compartido. Me falta probarlo con la versión de herramientas del equipo y unirlo al programa/mapa común. | ≈1 h en total; no separé el tiempo por actividad |
| 2026-11-03 | QA y freeze | | — |
| 2026-11-04 | Documentación de mi módulo | | — |
| 2026-11-05 | Videos y entrega preparada | | — |
| 2026-11-06 | Incidencias | | — |
| 2026-11-07 | Acuse de entrega | | — |

**Ojo:** la hora que aparece en la sección 3 es la hora del reloj asociada a una anotación; no es la duración del trabajo. Aquí, en “Tiempo que trabajé”, van las horas reales. “Sin registrar” quiere decir que no tenemos el dato; no quiere decir que hayan sido cero horas. El 17-oct sigue sin tiempo anotado.

---

## 5. Pendientes y bloqueos

- [ ] Preguntarle a David si el paquete que armé en mi computadora sirve para el equipo; está hecho con una versión distinta de la que pide el proyecto.
- [x] Copiar `Textures/reksai.png` desde el Drive compartido y comprobar su carga en la demo OpenGL local; la verificación con v143 sigue pendiente.
- [x] Preparar y subir a Drive un paquete local con `Models/`, `Textures/`, `include/`, `lib/`, `glm/`, DLLs y ejecutable (VS 2026/v145).
- [x] Preparar y subir a Drive el paquete completo de la demo OpenGL con fuentes y `Models/reksai.obj/.mtl` + `Textures/reksai.png`, conservando las rutas que espera el código.
- [ ] Volver a compilar con la versión de herramientas acordada por el equipo.
- [ ] Conectar los controles ya preparados con el cambio de modos y lugares.
- [ ] Poner Rek’Sai en P1/mapa principal, hacer que la cámara la siga y guardar una captura de la prueba común.
- [ ] Implementar/integrar la criatura N5 de C: recorrido entre puntos con patas/colas en movimiento; requiere el ancla de B.
- [ ] Terminar el cambio entre mapas con E, conectar F1–F4 a los modos y mostrar modo/lugar en pantalla.
- [x] Generar mi `AGENTS.md` local con datos comprobados (`PLAN_MAESTRO.md` §§14–15.1; no se commitea).
- [x] Confirmar rama local `feat/c-avatar` y traer `origin/feat/a-motor` localmente.

---

## 6. Enlaces rápidos

- Plan maestro: `PLAN_MAESTRO.md` (§5 arquitectura, §6 calendario, §8 diseño, §16 reglas).
- Rúbrica: `PLAN_MAESTRO.md` §2.
- Bitácora de A: `docs/bitacora-David.md`. Bitácora de B: `docs/bitacora-B.md`.
