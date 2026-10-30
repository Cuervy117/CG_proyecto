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
| Repositorio | Repo local en `C:\UNAM\CGICH`; origin `https://github.com/Cuervy117/CG_proyecto.git` (base preparada para publicar). Repo anterior: `https://github.com/Cuervy117/CGeIHC.git`, rama `practica_9`, commit `843ab2f`. |
| Entrega | sábado 7-nov-2026, 12:00, Google Classroom |
| Herramientas | Visual Studio 2022, Git, IA como apoyo de planificación y revisión |

---

## 2. Planeación vigente — 30-oct-2026

La revisión del 30-oct sustituye el calendario/alcance anterior sin alterar los hitos realizados. Fuentes: lineamientos originales y Propuesta_Proyecto_Final_Grafica.pdf. El detalle operativo vive en PLAN_MAESTRO §5/§6/§8/§10/§16.

- A David: motor, escena, cámaras, luces, build/Release y coordinación de merges.
- B Jonhatan: mundo, infraestructura y sus cinco animaciones.
- C Venancio: avatar Rek’Sai, criatura N5, entrada/experiencias/main/HUD.
- Hoy: H-A0 a H-A3 y coordinación H-E0 (§6.1). Primero headers y una escena integrada.
- 31-oct ambos mapas, cuatro modos básicos y prueba en otra PC; 2-nov demo completa; 3-nov freeze 18:00; 4–5 nov documentos/videos; 6-nov reserva.
- Tres ubicaciones lógicas: espacio, Vacío y Shinobi. Propuesta conserva universos y Rek’Sai; sin skinning, interiores ni aterrizaje continuo.
- La estimación antigua de 24/27 persona-días no acredita capacidad; confirmar horas efectivas de todos.
- La IA se utilizó en esta revisión para contrastar documentos, actualizar planificación y preparar Git/scripts. No equivale a implementación final ni aprobación del profesor; declarar ayuda real y aclarar límites de uso.

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
| H-01B | 2026-10-30 | 08:30 | Estructura del repo para el equipo | Motor movido a `src/engine/`, esqueleto de `src/scene`, `src/game` y `src/ui`, `ItemGroups` por dueño y reglas B/C en `PLAN_MAESTRO.md` §16; build Release x64 y P09-2 verificados. | `docs/capturas/refactor_src_01.png` · commits `c895008` y `26dce1d` | — | hecho |
| H-01C | 2026-10-30 | por confirmar | Plan y preparación compartida con asistencia IA | Plan alineado a propuesta/lineamientos; README, scripts, PDFs y ramas preparados. Rebuild Release x64 sin errores, con warnings heredados; ZIP de 753 archivos verificado. Sin nueva prueba visual. | README · docs/PREPARAR_PUSH.md · docs/VALIDACION_PREPARACION.md | por confirmar | hecho |
| H-A0 | 2026-10-30 | — | Arranque y distribución del paquete | Ver PLAN §6.1; recepción por compañeros pendiente. | README/scripts | — | pendiente |
| H-A1 | 2026-10-30 | — | Contratos de escena/cámara | Headers compilables y ejemplo jerárquico. | por generar | — | pendiente |
| H-A2 | 2026-10-30 | — | Bloqueantes de motor | Materiales, texturas, FOV y contrato de rutas. | por generar | — | pendiente |
| H-A3 | 2026-10-30 | — | Escena y FollowXZ | Render Model/Mesh y seguimiento de avatar. | por generar | — | pendiente |
| H-E0 | 2026-10-30 | — | Integración común | Avatar camina en P1; develop compilable. | por generar | — | pendiente |

---

## 5. Registro diario (formato del PDF)

| Fecha | Qué me propuse | Qué hice | Horas |
|---|---|---|---:|
| 2026-10-29 | Analizar lineamientos, definir arquitectura y cerrar la línea base | Análisis del PDF, plan maestro, bitácora y reparto; Fase 0: build Release x64, P09-2 ejecutado, `AGENTS.md`, captura y repo nuevo | 4.0 |
| 2026-10-30 | Organizar repo, actualizar plan y preparar H-A0 a H-A3 | Motor movido a `src/engine/`, esqueleto de `src/`, `ItemGroups` por dueño, reglas B/C en §16, bitácoras B/C y docs locales fuera de git; build Release 0 errores y P09-2 ejecutado | 0 |
| 2026-10-31 | Cámaras restantes, ambos mapas/modos y paquete en otra PC |  | — |
| 2026-11-01 | Día/noche, spots, skyboxes e integración |  | — |
| 2026-11-02 | Integración, rendimiento, bugs |  | 0 |
| 2026-11-03 | Release en otra PC, QA y freeze 18:00 |  | — |
| 2026-11-04 | Documentación motor/build/Git y correcciones |  | — |
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
- Repositorio: `https://github.com/Cuervy117/CG_proyecto.git` (base preparada para publicar)
