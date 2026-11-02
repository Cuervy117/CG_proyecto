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

Reparto confirmado por David: B = Jonhatan; C = Venancio. Esta planeación sustituye objetivos anteriores; no registra trabajo ejecutado ni horas de compañeros.

- Rol: **C — avatar e integración**.
- Objetivo: Rek’Sai articulada, criatura N5, entrada unificada, cuatro modos y tres ubicaciones, main y HUD.
- Rutas: `src/game/avatar/, src/game/anim/, src/game/Experience.*, src/game/main.cpp, src/ui/, Input.*`. Excepciones de vcxproj en PLAN_MAESTRO §10/§16.
- Hoy: **H-C0, H-C1, H-C2 y H-C3**, criterios y dependencias en PLAN_MAESTRO §6.1.
- Contratos §5: esperar headers compartidos antes de integrar; preparar datos/piezas mientras tanto.
- Rúbrica y aceptación §2/§7; controles únicos §8; no conservar controles de prácticas.
- Recursos: modelos en subcarpetas propias; texturas importadas temporalmente `Textures/c_nombre.ext` hasta validar el cargador.
- No usar rover/holocron/satélite de prácticas como contenido final.
- Documentación repartida según §10: cada uno redacta su sección y conclusión, no todo recae en C.

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

Esta tabla es el resultado **conjunto**, por eso una tarea queda `pendiente` mientras no esté integrada y comprobada en `develop`. Al 2-nov, David tiene H-A0 a H-A4 como hechos y H-E0 pendiente; la bitácora de B aún no registra hitos. `Parcial` significa que hay piezas hechas, pero el resultado completo todavía no.

---

## 3. Hitos

| ID | Fecha | Hora | Hito | Descripción | Evidencia | Horas | Estado |
|---|---|---|---|---|---|---|---|
| — | — | — | — | — | — | — | — |
| H-DB0 | 2026-10-17 | No registrada | Recursos Dragon Ball preparados localmente | Se reunieron modelos de referencia de Gohan/Cámara del Tiempo y se generaron siete modelos sencillos propios. Fue preparación local, no integración al mapa. Venancio aclara que el mapa actual no tiene modelo/personaje de Gohan; la propuesta aún enumera D3 como referencia y el equipo debe confirmar si se conserva. | `assets/dragon_ball/LEEME.md`; inventario, verificación y vista previa locales; fuera de `feat/c-avatar` y del Drive revisado | No registradas | Preparación local hecha; D3 por confirmar con el equipo |
| H-C0 | 2026-10-31 | 18:42 CDMX | Clon y arranque local | Cloné el repo, cambié a `feat/c-avatar` e integré localmente la rama A. Preparé Rek’Sai y Gohan D3; al cierre inicial el build seguía bloqueado por el paquete de dependencias y `include/`. | `Models/reksai.obj/.mtl`; `docs/recursos-C.md`; commit `aedcd41` | Incluida en el total H-C0/H-C1 del 31-oct (≈1 h, sin desglose) | Parcial en ese momento; cierre local registrado el 2-nov |
| H-C0 actualización | 2026-10-31 | 19:14 CDMX | Textura de Rek’Sai | Copié `reksai.png` desde Drive; faltaban las dependencias para comprobar la carga en el ejecutable. | `Textures/reksai.png`; `docs/recursos-C.md` | Incluida en el total H-C0/H-C1 del 31-oct (≈1 h, sin desglose) | Parcial en ese momento; cierre local registrado el 2-nov |
| H-C1 parcial | 2026-10-31 | 19:57 CDMX | Entrada y controles | Implementé `InputFrame`, captura de teclas/mouse/rueda y `mapControls` con flancos para WASD, Q/E, F1–F4 y acciones. Falta conectarlo al estado de modo/ubicación de `Experience` y probarlo en el ejecutable común. | `Input.cpp`, `Input.hpp`; commit `aedcd41` | Incluida en el total H-C0/H-C1 del 31-oct (≈1 h, sin desglose) | Parcial |
| H-C2 | 2026-10-31 | 20:38 CDMX | Rek’Sai articulada y vista previa | Separé cuerpo, cabeza, cola y patas, con pivotes y ciclo diagonal; corregí la pose horizontal en Blender. La comprobación de caminata en OpenGL se completó después, el 2-nov. | `src/game/avatar/Avatar.cpp`; `Models/c_reksai_articulada.blend`; `docs/capturas/reksai_preview.png` | ≈3 h entre 30–31 oct, total aproximado confirmado por Venancio, sin desglose diario | Hecho local; ver prueba OpenGL posterior |
| H-C2 actualización | 2026-11-02 | 11:03 CDMX | Textura de Rek’Sai preparada | Dejé `reksai.obj` y `reksai.png` en las rutas locales que usa el avatar. El código asigna la textura a las piezas; revisión del OBJ encontró UVs y las siete partes. Falta validarlo en el ejecutable. | `Models/reksai.obj`, `Textures/reksai.png`; hashes en `docs/recursos-C.md` | Incluida en el total diario 2-nov (≈1 h para el trabajo del día, sin desglose) | En curso en ese momento |
| H-C2 actualización | 2026-11-02 | Hora no registrada | Recursos Rek’Sai completados desde Drive | Localicé en el Drive compartido el OBJ, MTL y PNG del equipo; añadí el MTL faltante y ajusté su ruta de textura a `../Textures/reksai.png`. | Drive `Models/reksai/`, `Textures/reksai.png`; `Models/reksai.mtl` local | Incluida en el total diario 2-nov (≈1 h para el trabajo del día, sin desglose) | Preparación completada; prueba OpenGL registrada después |
| H-C2 actualización | 2026-11-02 | 11:20 CDMX | Prueba visual local de la textura | Importé `Models/reksai.obj` en Blender 5.2 con su MTL y la vista Material Preview mostró la textura sobre la malla. Confirma el mapeo OBJ/MTL/PNG en Blender; faltaba comprobar el cargador del proyecto. | Captura visible en Blender; `Models/reksai.obj`, `Models/reksai.mtl`, `Textures/reksai.png` | Incluida en el total diario 2-nov (≈1 h para el trabajo del día, sin desglose) | Prueba de Blender hecha; OpenGL comprobado después |
| H-C2 actualización | 2026-11-02 | 11:52 CDMX | Caminata de Rek’Sai en OpenGL | Conecté el avatar articulado al bucle de la escena de prueba: su desplazamiento alimenta dirección y velocidad de `Avatar::update`, que alterna patas y mueve cuerpo/cabeza/cola. La compilación y prueba OpenGL se completaron después ese mismo día. | `P09-2-424104230.cpp`; `src/game/avatar/Avatar.cpp`; modelo y textura en `Models/` y `Textures/` | Incluida en el total diario 2-nov (≈1 h para el trabajo del día, sin desglose) | En curso en ese momento; prueba posterior registrada abajo |
| H-C2 actualización | 2026-11-02 | 13:05 CDMX | Render completo y bundle local | Corregí `MeshModel::CreateMeshModel`: reservaba solo 1/8 del buffer intercalado. Compilé con VS 2026/v145 y ejecuté la escena OpenGL; Rek’Sai texturizada ya aparece completa y articulada. Preparé y subí un ZIP local con dependencias y ejecutable; falta validar compilación con el toolset v143 acordado para el equipo e integrar en P1/main común. | `src/engine/Mesh_tn.cpp`; `P09-2-424104230.cpp`; `x64/Release/LabCGeIHC.local.exe`; [ZIP en Drive](https://drive.google.com/file/d/1y0RRxMR6n6LyBR34hjpAqKC_uwOlasbz/view?usp=drivesdk); SHA-256 `C52AF0CC42C4A738278BDB35DDFB48FC9B59E84E857E4D00B95316565520B157` | Incluida en el total diario 2-nov (≈1 h para el trabajo del día, sin desglose) | Hecho local; integración común pendiente |
| H-C0 actualización | 2026-11-02 | 13:05 CDMX | Build local con recursos visibles | Después de recibir las dependencias compilé Release x64 con VS 2026/v145 y abrí la demo con Rek’Sai texturizada. Esto cierra el arranque/build local de H-C0; todavía falta confirmar compatibilidad con el toolset v143 del equipo. | `x64/Release/LabCGeIHC.local.exe`; [ZIP de dependencias en Drive](https://drive.google.com/file/d/1y0RRxMR6n6LyBR34hjpAqKC_uwOlasbz/view?usp=drivesdk) | Incluida en el total diario 2-nov (≈1 h para el trabajo del día, sin desglose) | H-C0 hecho local; v143 pendiente |
| H-C2 actualización | 2026-11-02 | 13:57 CDMX | Caminata articulada comprobada de perfil | Con apoyo de Codex corregí la vista de prueba para dejar a Rek’Sai quieta en el sitio y de perfil, y ajusté los pivotes del OBJ para hacer visibles las patas, la cabeza y la cola sin dejar piezas desprendidas. Compilé Release x64 con VS 2026/v145 y la abrí en OpenGL. La animación sigue siendo una prueba local; falta validar con v143 e integrarla al mapa/main común. | `P09-2-424104230.cpp`; `src/game/avatar/Avatar.cpp`; `Models/reksai.obj`; `Textures/reksai.png` | Incluida en el total diario 2-nov (≈1 h para el trabajo del día, sin desglose) | Comprobado local; integración común pendiente |
| H-C2 actualización | 2026-11-02 | 14:47 CDMX | Ajuste suave de la caminata | A petición de Venancio bajé el recorrido de la cola, hice más lento el movimiento de la cabeza y restauré las cuatro patas pequeñas a sus grupos OBJ originales, que quedan separadas de los grupos humanoides de hombros/brazos. Compilé y volví a abrir la vista en OpenGL; la integración en el ejecutable común sigue pendiente. | `src/game/avatar/Avatar.cpp`; captura en ventana OpenGL | Incluida en el total diario 2-nov (≈1 h para el trabajo del día, sin desglose) | Comprobado local; integración común pendiente |
| H-C2 actualización | 2026-11-02 | 15:05 CDMX | Cola más lenta y brazos delanteros | A petición de Venancio bajé otra vez la velocidad y amplitud de la cola, y separé los grupos OBJ de hombro/codo/mano en dos pivotes para que los brazos delanteros oscilen suavemente. Se conserva el ciclo aprobado de cuatro patas pequeñas y cabeza. Compilé y revisé la escena en OpenGL sin piezas desprendidas. | `src/game/avatar/Avatar.cpp`; ventana OpenGL local; [fuentes y README en Drive](https://drive.google.com/drive/folders/1BjhEMXsBatML2Dhk65OjgfRBtdywbhvU); [ZIP](https://drive.google.com/file/d/1pd8Ay-8fg0ZgM6L1xSWHFK8Sy9Y93ocD/view?usp=drivesdk) | Incluida en el total diario 2-nov (≈1 h para el trabajo del día, sin desglose) | H-C2 hecho en demo OpenGL local; integración H-C3 pendiente |
| H-C2 actualización | 2026-11-02 | 15:40 CDMX | Paquete completo de Rek’Sai en Drive | Empaqueté la demo OpenGL con sus fuentes y recursos OBJ/MTL/PNG, respetando las rutas `Models/` y `Textures/`. El código queda en esta rama; los assets se comparten por Drive conforme a PLAN_MAESTRO §16.4. Falta validar con toolset v143 e integrar en el main/mapa común. | [ZIP completo en Drive](https://drive.google.com/file/d/18WwExs-s2famGcs4KhbadmA4u2tA9Btv/view?usp=drivesdk); [carpeta de demo](https://drive.google.com/drive/folders/1BjhEMXsBatML2Dhk65OjgfRBtdywbhvU); `docs/recursos-C.md` | Incluida en el total diario 2-nov (≈1 h para el trabajo del día, sin desglose) | Paquete comprobado en Drive; integración común pendiente |

### Estado de los hitos C al 2-nov

| Hito | Estado actual según `feat/c-avatar` | Qué falta para cerrarlo |
|---|---|---|
| H-C0 — clonar, paquete y build local | **Hecho localmente**: Release x64 con recursos visibles usando VS 2026/v145. | Confirmar build con v143, que es el toolset acordado para el equipo. |
| H-C1 — entrada y estado de modo/ubicación | **Parcial**: `InputFrame` y `mapControls` ya capturan entradas y flancos. | Conectarlos al estado de `Experience` y comprobar que los controles no colisionen en el ejecutable común. |
| H-C2 — Rek’Sai articulada | **Hecho en la demo OpenGL local**: modelo completo con textura, cuatro patas, cabeza, cola y brazos delanteros animados; paquete completo en Drive. | Comprobar el mismo recurso/código con v143; su integración al mapa común se registra en H-C3. |
| Criatura N5 — animación C (PLAN_MAESTRO §8.3) | **Pendiente en esta rama**: no hay implementación de la criatura N5 ni su recorrido entre puntos. | B deja el ancla N5; C prepara/integra la criatura que camina y mueve patas/colas continuamente. |
| H-C3 — main, P1, avatar y FollowXZ | **Pendiente**: `P09-2-424104230.cpp` es una escena de prueba, no la integración al main común. | Insertar el avatar en P1, conectar cámara FollowXZ y límites y guardar captura del ejecutable común. |
| H-C4 — E entre mapas | **Pendiente en esta rama**. | Integrar interacción y salida con objetivo válido, con prioridad de E según el plan. |
| H-C5 — F1–F4 y objetivos | **Parcial**: el mapeo de teclas existe, pero no está conectado a experiencias/objetivos. | Conectar y verificar los cuatro modos sin objetivos nulos ni recargas. |
| H-C6 — HUD mínimo | **Pendiente en esta rama**. | Mostrar modo, ubicación y acción contextual. |

---

## 3.1 Trabajo previo de recursos Dragon Ball

Este material se preparó localmente el 17-oct, antes de que el reparto vigente moviera a C a Rek’Sai/avatar e integración. Lo dejo registrado como trabajo previo de Venancio en los elementos Dragon Ball; no significa que ya esté integrado ni subido a esta rama.

- Se reunieron tres recursos descargables de Gohan SSJ2 y la Cámara del Tiempo, con sus ZIP, OBJ/MTL, texturas y procedencia documentada.
- Con apoyo de Codex se generaron versiones propias sencillas de cápsula saiyajin, estación inspirada en Freezer, domo de entrenamiento, cohete, llama, plataforma/torre y pista orbital, en OBJ/MTL/GLB con texturas.
- Se generó una vista previa y se ejecutó la verificación local de 17 archivos OBJ/GLB. Esto comprueba los archivos del paquete, no su carga en el ejecutable del equipo.
- Evidencia anotada en el registro previo: `assets/dragon_ball/LEEME.md`, `inventario_modelos_propios.json`, `verificacion.json` y `vista_previa_modelos_propios.png`. No localicé estos archivos en el checkout de `feat/c-avatar` ni en el Drive compartido revisado. Como Gohan no aparece en el mapa actual según Venancio, confirmar con el equipo si D3 se conserva como referencia antes de tratar estos recursos como entrega pendiente.
- Aclaración de Venancio (2026-11-02): en el mapa actual no hay un modelo de Gohan; lo que se observa son naves/cápsulas que abren y cierran. La propuesta identifica D3 como referencia de Gohan, así que su inclusión final queda por confirmar en el grupo. El plan asigna a B las animaciones de infraestructura (naves/puertas/paneles); no cuentan como trabajo C.
- Horas: no quedaron registradas; no se asigna una cifra retroactiva.

---

## 4. Registro diario (formato del PDF)

| Fecha | Qué me propuse | Qué hice | Horas |
|---|---|---|---|
| 2026-10-17 | Preparar recursos de la parte Dragon Ball | Con apoyo de Codex se reunieron modelos descargables de Gohan/Cámara del Tiempo y se generaron modelos sencillos para cápsula, estación, domo, cohete, llama, plataforma/torre y pista. Se hizo vista previa y revisión local de OBJ/GLB; los archivos siguen fuera del repo. | No registradas |
| 2026-10-30 | H-C0, H-C1, H-C2 y H-C3 | Inicié la preparación del modelo de Rek’Sai para el avatar articulado. El tiempo de H-C2 se confirmó acumulado entre el 30 y 31 de octubre, sin desglose por día. | H-C2: incluido en ≈3 h acumuladas entre 30–31 oct; sin desglose diario |
| 2026-10-31 | H-C1/H-C2 y avanzar avatar | Clon y rama C preparados; integré localmente la rama A, preparé Gohan D3 y Rek’Sai con texturas. Dejé el mapeo base de controles en `Input.cpp/.hpp`; faltó conectarlo a `Experience`. Separé Rek’Sai por cuerpo, cabeza, cola y patas, y revisé en Blender la pose cuadrúpeda horizontal. Al cierre de ese día faltaban las dependencias y el build local; se resolvieron el 2-nov. | H-C0/H-C1: ≈1 h total el 31-oct; H-C2: parte de ≈3 h acumuladas entre 30–31 oct, sin desglose |
| 2026-11-01 | Animaciones y contenido según rol | Según recuerda Venancio, no se trabajó en el proyecto ese día. | 0 h reportadas por Venancio |
| 2026-11-02 | Revisar hitos y avanzar H-C2 | Con apoyo de Codex corregí el tamaño del buffer de las mallas, compilé con el toolset local v145 y corrí Rek’Sai en OpenGL: ya se ve completa, con textura y articulación. Después ajusté la cola y los brazos delanteros y subí al Drive compartido el paquete completo con las fuentes y los recursos OBJ/MTL/PNG. Falta comprobarlo con el toolset v143 y meterlo en el main/mapa común. | ≈1 h total del trabajo del día, aproximado confirmado por Venancio; sin desglose entre build, ajustes y Drive |
| 2026-11-03 | QA y freeze | | — |
| 2026-11-04 | Documentación de mi módulo | | — |
| 2026-11-05 | Videos y entrega preparada | | — |
| 2026-11-06 | Incidencias | | — |
| 2026-11-07 | Acuse de entrega | | — |

Las horas se anotan al trabajar; — significa sin registrar, no cero horas realizadas.

---

## 5. Pendientes y bloqueos

- [ ] Confirmar con David si el ZIP local v145 puede servir al equipo; el repo pide Assimp v143 y el paquete disponible se compiló con VS 2026/v145.
- [x] Copiar `Textures/reksai.png` desde el Drive compartido y comprobar su carga en la demo OpenGL local; la verificación con v143 sigue pendiente.
- [x] Preparar y subir a Drive un paquete local con `Models/`, `Textures/`, `include/`, `lib/`, `glm/`, DLLs y ejecutable (VS 2026/v145).
- [x] Preparar y subir a Drive el paquete completo de la demo OpenGL con fuentes y `Models/reksai.obj/.mtl` + `Textures/reksai.png`, conservando las rutas que espera el código.
- [ ] Verificar build con la configuración v143 del equipo.
- [ ] Cerrar H-C1 conectando `InputFrame`/`mapControls` al estado de modo/ubicación en `Experience`.
- [ ] Cerrar H-C3 integrando avatar en P1/main común con cámara FollowXZ, límites y captura.
- [ ] Implementar/integrar la criatura N5 de C: recorrido entre puntos con patas/colas en movimiento; requiere el ancla de B.
- [ ] Completar H-C4 (E entre mapas), H-C5 (F1–F4 conectados a objetivos) y H-C6 (HUD mínimo) conforme al plan.
- [x] Generar mi `AGENTS.md` local con datos comprobados (`PLAN_MAESTRO.md` §§14–15.1; no se commitea).
- [x] Confirmar rama local `feat/c-avatar` y traer `origin/feat/a-motor` localmente.

---

## 6. Enlaces rápidos

- Plan maestro: `PLAN_MAESTRO.md` (§5 arquitectura, §6 calendario, §8 diseño, §16 reglas).
- Rúbrica: `PLAN_MAESTRO.md` §2.
- Bitácora de A: `docs/bitacora-David.md`. Bitácora de B: `docs/bitacora-B.md`.
