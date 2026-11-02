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
| H-DB0 | 2026-10-17 | No registrada | Recursos Dragon Ball preparados localmente | Se reunieron modelos de referencia de Gohan/Cámara del Tiempo y se generaron siete modelos sencillos propios. Vista previa y revisión local de OBJ/GLB listas; falta compartirlos en el repo y probar su carga en el motor. | `assets/dragon_ball/LEEME.md`; inventario, verificación y vista previa locales | No registradas | hecho local |
| H-C0 | 2026-10-31 | 18:42 CDMX | Clon y arranque local | Cloné el repo, cambié a `feat/c-avatar` e integré localmente la rama A. Preparé Rek’Sai y Gohan D3; el build quedó bloqueado por el paquete de dependencias y `include/`, así que no hay Release propio. | `Models/reksai.obj/.mtl`; `docs/recursos-C.md`; commit `aedcd41` | Por confirmar | En curso |
| H-C0 actualización | 2026-10-31 | 19:14 CDMX | Textura de Rek’Sai | Copié `reksai.png` desde Drive. Falta recibir dependencias y comprobar la carga en el ejecutable. | `Textures/reksai.png`; `docs/recursos-C.md` | Por confirmar | En curso |
| H-C1 parcial | 2026-10-31 | 19:57 CDMX | Entrada y controles | Añadí captura por frame y mapeo de teclas/mouse/rueda. Falta conectarlo a `Experience` y probarlo en el ejecutable. | `Input.cpp`, `Input.hpp`; commit `aedcd41` | Por confirmar | En curso |
| H-C2 | 2026-10-31 | 20:38 CDMX | Rek’Sai articulada y vista previa | Separé cuerpo, cabeza, cola y patas, con pivotes y ciclo diagonal; corregí la pose horizontal en Blender. Falta comprobar articulación y caminata en el ejecutable. | `src/game/avatar/Avatar.cpp`; `Models/c_reksai_articulada.blend`; `docs/capturas/reksai_preview.png` | ≈2 h entre 30–31 oct, estimación confirmada por Venancio | En curso |

---

## 3.1 Trabajo previo de recursos Dragon Ball

Este material se preparó localmente el 17-oct, antes de que el reparto vigente moviera a C a Rek’Sai/avatar e integración. Lo dejo registrado como trabajo previo de Venancio en los elementos Dragon Ball; no significa que ya esté integrado ni subido a esta rama.

- Se reunieron tres recursos descargables de Gohan SSJ2 y la Cámara del Tiempo, con sus ZIP, OBJ/MTL, texturas y procedencia documentada.
- Con apoyo de Codex se generaron versiones propias sencillas de cápsula saiyajin, estación inspirada en Freezer, domo de entrenamiento, cohete, llama, plataforma/torre y pista orbital, en OBJ/MTL/GLB con texturas.
- Se generó una vista previa y se ejecutó la verificación local de 17 archivos OBJ/GLB. Esto comprueba los archivos del paquete, no su carga en el ejecutable del equipo.
- Evidencia local: `assets/dragon_ball/LEEME.md`, `inventario_modelos_propios.json`, `verificacion.json` y `vista_previa_modelos_propios.png`. Esta carpeta está fuera del árbol versionado de esta rama; falta acordar y completar cómo compartir esos recursos con el repo.
- Horas: no quedaron registradas; no se asigna una cifra retroactiva.

---

## 4. Registro diario (formato del PDF)

| Fecha | Qué me propuse | Qué hice | Horas |
|---|---|---|---|
| 2026-10-17 | Preparar recursos de la parte Dragon Ball | Con apoyo de Codex se reunieron modelos descargables de Gohan/Cámara del Tiempo y se generaron modelos sencillos para cápsula, estación, domo, cohete, llama, plataforma/torre y pista. Se hizo vista previa y revisión local de OBJ/GLB; los archivos siguen fuera del repo. | No registradas |
| 2026-10-30 | H-C0, H-C1, H-C2 y H-C3 | Inicié la preparación del modelo de Rek’Sai para el avatar articulado; las horas se registran acumuladas en H-C2 porque no quedó desglose por día. | Incluidas en total H-C2; sin desglose |
| 2026-10-31 | H-C1/H-C2 y avanzar avatar | Clon y rama C preparados; integré localmente la rama A, preparé Gohan D3 y Rek’Sai con texturas. Dejé el mapeo base de controles en `Input.cpp/.hpp`; todavía falta conectarlo a `Experience` y probarlo en el ejecutable. Separé Rek’Sai por cuerpo, cabeza, cola y patas, y revisé en Blender la pose cuadrúpeda horizontal. El ZIP de dependencias sigue pendiente. | H-C2: ≈2 h acumuladas entre 30–31 oct, sin desglose diario; H-C1 y lo demás: por confirmar |
| 2026-11-01 | Animaciones y contenido según rol | No encontré commits de C para esta fecha; si hubo trabajo fuera del repo, falta anotarlo con el avance y las horas reales. | Sin registro; confirmar |
| 2026-11-02 | Revisar bitácora y estado del equipo | Con apoyo de Codex comparé los hitos de A/B/C y aclaré qué está hecho, qué va parcial y qué sigue sin integración. No se agregó implementación al ejecutable en esta revisión. | Horas de Venancio no registradas |
| 2026-11-03 | QA y freeze | | — |
| 2026-11-04 | Documentación de mi módulo | | — |
| 2026-11-05 | Videos y entrega preparada | | — |
| 2026-11-06 | Incidencias | | — |
| 2026-11-07 | Acuse de entrega | | — |

Las horas se anotan al trabajar; — significa sin registrar, no cero horas realizadas.

---

## 5. Pendientes y bloqueos

- [ ] Obtener de David el ZIP de dependencias y su SHA-256; las carpetas `utilidades` y `modelos` revisadas en Drive no contienen el ZIP.
- [x] Copiar `Textures/reksai.png` desde `Proyecto_final_CG/Textures`; queda pendiente verificar la carga en el ejecutable.
- [ ] Copiar `Models/`, `Textures/`, `include/`, `lib/`, `glm/` y DLLs y verificar Release x64 desde la raíz.
- [x] Generar mi `AGENTS.md` local con datos comprobados (`PLAN_MAESTRO.md` §§14–15.1; no se commitea).
- [x] Confirmar rama local `feat/c-avatar` y traer `origin/feat/a-motor` localmente.

---

## 6. Enlaces rápidos

- Plan maestro: `PLAN_MAESTRO.md` (§5 arquitectura, §6 calendario, §8 diseño, §16 reglas).
- Rúbrica: `PLAN_MAESTRO.md` §2.
- Bitácora de A: `docs/bitacora-David.md`. Bitácora de B: `docs/bitacora-B.md`.
