---
title: Bitácora individual — David
autor: David
materia: Computación Gráfica e Interacción Humano-Computadora
semestre: 2027-1
proyecto: Proyecto final CGICH
inicio: 2026-10-29
entrega: 2026-11-07
estado: en curso
---

# Bitácora individual — David

> [!info] Propósito
> Registro individual del trabajo en el proyecto final. Cumple el requisito del PDF: **qué me propuse hacer, qué hice y cuántas horas trabajé**, por día.
> Además, se lleva un **registro por hitos** con fecha y hora: cada vez que se cierra uno, se anota aquí en el mismo momento, con información **suficiente para describirlo**, no con ensayos.

## 0. Regla de registro

1. **Se escribe al cerrar el hito**, no al final del proyecto.
2. Cada entrada lleva **fecha y hora**.
3. La descripción es de **1 a 3 líneas**: qué se logró y cómo se verificó.
4. Se adjunta **evidencia**: captura, commit, archivo o ruta.
5. Se anotan las **horas reales** trabajadas, aunque el hito se haya pasado de largo.
6. Si un hito no se termina, **no se marca como hecho**; se deja `en curso` y se anota el bloqueo.
7. No se escribe narrativa larga. Si hace falta detalle, va en el manual técnico, no aquí.

> [!tip] Plantilla de una entrada
> `H-XX · 2026-10-30 · 14:35 · <hito> · <1–3 líneas> · Evidencia: <captura/commit> · <horas> h · hecho`

---

## 1. Datos del proyecto

| Campo | Valor |
|---|---|
| Proyecto | Ambiente virtual interactivo: viajes espaciales y evolución de especies intergalácticas |
| Equipo | 3 integrantes |
| Mi rol | **A — motor y cámaras** |
| Stack | C++17 · OpenGL 4.3 (contexto actual; objetivo 3.3) · GLFW · GLEW · GLM · Assimp · Visual Studio 2022 |
| Repositorio | Nuevo repo local en `C:\UNAM\CGICH` (sin remoto; David lo agrega). Repo anterior: `https://github.com/Cuervy117/CGeIHC.git`, rama `practica_9`, commit `843ab2f`. |
| Entrega | sábado 7-nov-2026, 12:00, Google Classroom |
| Herramientas | Visual Studio 2022, Git, IA como apoyo de planificación y revisión |

---

## 2. Planeación

### 2.1 Objetivo

Construir un escenario 3D interactivo con:

- Un **avatar jerárquico no humanoide** con caminata y material reactivo a la luz.
- **Cuatro recorridos**: planeta en tercera persona sobre XZ, nave de carrera, nave entre planetas y vista del lanzamiento.
- **Dos planetas** con etapas de evolución de especies distintas.
- **Pista de carreras**, **plataforma de lanzamiento**, **estación espacial**, **NPCs**, **satélites, lunas y asteroides**.
- **Ciclo día/noche** de 120 s, luces puntuales, spotlights por teclado y **skybox por planeta**.
- **Tres animaciones por teclado** y **tres animaciones continuas**.
- Ejecutable Release `.exe`, manuales, video, videoblog y bitácoras.

### 2.2 Estrategia técnica

| Decisión | Motivo |
|---|---|
| Reutilizar el motor de las prácticas | Ya funciona; reescribirlo costaría 4–5 días |
| Añadir solo 3 abstracciones: `SceneNode`, `CameraRig`, `Experience` | Cubren jerarquía, cámaras y modos sin refactor masivo |
| Un solo `main` + experiencias | Evita la explosión de clases por objeto |
| Avatar procedural, sin skinning | Menor riesgo en 9 días |
| Un mundo con zonas, no 4 escenas | Comparte assets y facilita el cambio de cámara |
| Rehacer pista, nave y sátelite | La regla del PDF excluye los ejercicios de clase |

La comparación "reescribir vs. reutilizar" y la selección de las tres abstracciones se discutieron con apoyo de IA a partir del análisis del PDF y del código heredado.

### 2.3 Hitos y sprints

| Sprint | Fechas | Objetivo | Responsable | Estado |
|---|---|---|---|---|
| S0 | 29–30 oct | Línea base: el proyecto de escritorio compila y corre; `AGENTS.md`; bitácora | A | en curso |
| S1 | 30–31 oct | `SceneNode`, cámaras 1 y 4, avatar caminando | A + C | pendiente |
| S2 | 31 oct–1 nov | Día/noche, spots, skybox, planeta 2, 2 recorridos | A + B + C | pendiente |
| S3 | 1–2 nov | Cámaras 2 y 3, estación, 3 animaciones por teclado | A + B + C | pendiente |
| S4 | 2–3 nov | Integración, NPCs, 3 continuas, texturizado | todos | pendiente |
| S5 | 3–4 nov | QA, feature freeze, Release en máquina limpia | A + B + C | pendiente |
| S6 | 4–5 nov | Manuales, video, videoblog, bitácoras | C + todos | pendiente |
| S7 | 6–7 nov | Buffer y entrega | todos | pendiente |

La descomposición de la rúbrica en hitos y la estimación de esfuerzo (~24 persona-días frente a 27 disponibles) se realizaron **con ayuda de IA, dada la falta de experiencia previa en un proyecto de esta escala**. La IA se usó como apoyo de planificación, análisis y revisión; la implementación y la validación son propias.

### 2.4 Reparto del equipo

| Rol | Responsabilidad |
|---|---|
| **A (David)** | Motor, escena, cámaras, iluminación, build y release |
| **B** | Mundo: pista, planetas, plataforma, cohete, estación, NPCs, texturas |
| **C** | Avatar, animaciones, `Experience`, HUD, manuales y video |

### 2.5 Riesgos y recortes

- **Riesgo principal:** que el proyecto de escritorio no sea el que realmente corre. Se resuelve en la Fase 0 de descubrimiento.
- **Riesgo de contenido:** que la pista o el sátelite se reconozcan como práctica. Se rehacen geometría y disposición.
- **Riesgo de tiempo:** otras materias y el sábado con poca disponibilidad. Se compensa con trabajo adelantado y buffer el viernes.

**Orden de sacrificio:** sombras → PBR → colisión de cámara → audio → variedad de NPCs → crossfade de skybox → cámara 4 cinematográfica.
**Nunca se sacrifica:** 2 planetas, 6 animaciones, 4 recorridos, avatar caminando, ciclo día/noche y `.exe`.

La priorización por peso de rúbrica y el orden de sacrificio se definieron con apoyo de IA para optimizar el tiempo disponible.

### 2.6 Apoyo de IA

Se utilizó IA para:

- Analizar el PDF de lineamientos y descomponer la rúbrica en requisitos verificables.
- Estimar esfuerzo por bloque y detectar el camino crítico.
- Revisar riesgos técnicos del código heredado (Assimp, OpenGL, build, rutas).
- Generar el plan maestro y esta bitácora.

La IA **no** sustituye la implementación, las pruebas ni la validación en la máquina de entrega.

---

## 3. Hitos previos (completar)

> Estos hitos ya se realizaron antes de esta bitácora. Falta anotar fecha, evidencia y horas.

| ID | Fecha | Hito | Descripción | Evidencia | Horas |
|---|---|---|---|---|---|
| HP-01 | por confirmar | Recolección de modelos 3D | Cohete, satélite, Landsat, planeta, asteroide, Fighter, rover, holocron. Se verificó que Assimp los cargara. | `Models/` | — |
| HP-02 | por confirmar | Recolección de texturas | Skybox nebular, cupertin-lake día/noche, panel, fuego, `rocket_diffuse`, etc. | `Textures/` | — |
| HP-03 | por confirmar | Modelado / texturizado propio | Piezas y texturas creadas para el proyecto (detallar cuáles). | por definir | — |
| HP-04 | por confirmar | Boceto del escenario | Plano/mapa con simbología, entregado el 19-sep. | Classroom | — |
| HP-05 | por confirmar | Documento de propuesta | Propuesta con correcciones, entregada el 26-sep. | Classroom | — |
| HP-06 | por confirmar | Prácticas 1–9 / motor base | Window, Input, Camera, Shader, Model, Texture, Mesh, Skybox, luces, pista y sátelite. | `artefactos/` · repo de prácticas | — |
| HP-07 | por confirmar | Repositorio y ramas por práctica | Historial con ramas `practica_1..9` como evidencia de avance. | repo de prácticas | — |

---

## 4. Registro de hitos

> Se llena **al cerrar cada hito**. La columna "Descripción" es la mínima necesaria para entender qué se logró.

| ID | Fecha | Hora | Hito | Descripción | Evidencia | Horas | Estado |
|---|---|---|---|---|---|---|---|
| H-00 | 2026-10-29 | 23:22 | Planificación y plan maestro | Análisis del PDF, rúbrica, arquitectura, riesgos y calendario. Se generó `PLAN_MAESTRO.md` y esta bitácora. | `PLAN_MAESTRO.md` · commit | 3.0 | hecho |
| H-01 | 2026-10-29 | 23:49 | Línea base en la PC de escritorio | Release x64 compila con 0 errores (64 warnings); P09-2 corre desde la raíz con skybox, piso texturizado, holocron y cohete. Se generó `AGENTS.md` y se reinició el repo. | `docs/capturas/fase0_p09.png` · commit `362f19b` · tag `v0.1-base` | 1.0 | hecho |
| H-02 | 2026-10-30 | — | `SceneNode` + `Scene` | Grafo de escena con transformaciones de mundo; un hijo se mueve sin mover al padre. | captura · commit | — | pendiente |
| H-03 | 2026-10-30 | — | Cámaras 1 y 4 + director | `FollowXZ` sigue al avatar en el plano XZ; `Orbit` gira alrededor del objetivo; transición suave. | video corto · commit | — | pendiente |
| H-04 | 2026-10-31 | — | Ciclo día/noche + spots + skybox | Sol de 120 s, puntuales automáticas, spots con `F` y cambio de cubemap por zona. | video · commit | — | pendiente |
| H-05 | 2026-11-01 | — | Cámaras 2 y 3 | Cámara de persecución de la nave de carrera y cámara de vuelo entre planetas. | video · commit | — | pendiente |
| H-06 | 2026-11-02 | — | Integración de los 4 recorridos | Cambio entre modos con `1`–`4`, HUD y sin glitches de cámara. | video · commit | — | pendiente |
| H-07 | 2026-11-03 | 18:00 | Feature freeze | No se agregan features; solo bugs y pulido. | tag `v1.0-rc` | — | pendiente |
| H-08 | 2026-11-04 | — | Release en máquina limpia | `.exe` Release x64 con DLLs y assets corre sin Visual Studio. | zip · captura | — | pendiente |
| H-09 | 2026-11-05 | — | Manuales, video y videoblog | Manual de usuario, manual técnico, video demo sin voz y videoblog. | PDFs · video | — | pendiente |
| H-10 | 2026-11-07 | 12:00 | Entrega | Proyecto subido a Classroom antes del mediodía. | comprobante | — | pendiente |

---

## 5. Registro diario (formato del PDF)

| Fecha | Qué me propuse | Qué hice | Horas |
|---|---|---|---:|
| 2026-10-29 | Analizar lineamientos, definir arquitectura y cerrar la línea base | Análisis del PDF, plan maestro, bitácora y reparto; Fase 0: build Release x64, P09-2 ejecutado, `AGENTS.md`, captura y repo nuevo | 4.0 |
| 2026-10-30 | Línea base, `AGENTS.md`, `SceneNode`, cámaras 1 y 4 |  | 0 |
| 2026-10-31 | Día/noche, spots, skybox, integración con B y C |  | 0 |
| 2026-11-01 | Cámaras 2 y 3, director, pruebas de los 4 recorridos |  | 0 |
| 2026-11-02 | Integración, rendimiento, bugs |  | 0 |
| 2026-11-03 | QA y feature freeze |  | 0 |
| 2026-11-04 | Release y prueba en máquina limpia |  | 0 |
| 2026-11-05 | Capturas técnicas y manual técnico |  | 0 |
| 2026-11-06 | Buffer e imprevistos |  | 0 |
| 2026-11-07 | Entrega |  | 0 |

> Las horas de los días futuros se llenan al cerrar cada día. No se estiman: se anotan las reales.

---

## 6. Conclusiones individuales (borrador)

> Se completan al final del proyecto, con lo aprendido sobre OpenGL, jerarquía, cámaras, iluminación y trabajo en equipo.

- Pendiente.

## 7. Bibliografía

- Documentación de OpenGL 3.3 / GLSL 330.
- Documentación de GLFW, GLEW, GLM y Assimp.
- Lineamientos del proyecto final CGICH 2027-1.
- Material del curso y prácticas 1–9.

---

## Anexo — Enlaces rápidos

- Plan maestro: `PLAN_MAESTRO.md`
- Contratos y arquitectura: `PLAN_MAESTRO.md`, secciones 5 y 8.
- Rúbrica: `PLAN_MAESTRO.md`, sección 2.
- Repositorio: nuevo repo local en `C:\UNAM\CGICH` (sin remoto; David lo agrega)
