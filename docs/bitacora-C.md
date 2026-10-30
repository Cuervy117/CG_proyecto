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
| Repositorio | `https://github.com/Cuervy117/CG_proyecto.git` (publicación de base preparada; verificar push) |
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
| 30-oct | Avatar en P1 con cámara dentro de ejecutable común | pendiente |
| 31-oct | Ambos planetas y cuatro modos básicos; paquete en otra PC | pendiente |
| 1-nov | Animaciones, iluminación y contenido real | pendiente |
| 2-nov | Demo completa de requisitos | pendiente |
| 3-nov | QA, Release externo y freeze 18:00 | pendiente |
| 4–5 nov | Manuales, videos, plano/propuesta finales | pendiente |
| 6-nov | Reserva de incidencias | pendiente |
| 7-nov | Acuse antes de 12:00 | pendiente |

---

## 3. Hitos

| ID | Fecha | Hora | Hito | Descripción | Evidencia | Horas | Estado |
|---|---|---|---|---|---|---|---|
| — | — | — | — | — | — | — | — |

---

## 4. Registro diario (formato del PDF)

| Fecha | Qué me propuse | Qué hice | Horas |
|---|---|---|---|
| 2026-10-30 | H-C0, H-C1, H-C2 y H-C3 | | — |
| 2026-10-31 | Ambos mapas y cuatro modos | | — |
| 2026-11-01 | Animaciones y contenido según rol | | — |
| 2026-11-02 | Demo completa | | — |
| 2026-11-03 | QA y freeze | | — |
| 2026-11-04 | Documentación de mi módulo | | — |
| 2026-11-05 | Videos y entrega preparada | | — |
| 2026-11-06 | Incidencias | | — |
| 2026-11-07 | Acuse de entrega | | — |

Las horas se anotan al trabajar; — significa sin registrar, no cero horas realizadas.

---

## 5. Pendientes y bloqueos

- [ ] Copiar `Models/`, `Textures/`, `include/`, `lib/`, `glm/` y DLLs desde la carpeta compartida.
- [ ] Verificar Release x64 (0 errores) y `.exe` desde la raíz.
- [ ] Generar mi `AGENTS.md` local con `PLAN_MAESTRO.md` §15.1 (no se commitea).
- [ ] Confirmar rama `feat/c-avatar`.

---

## 6. Enlaces rápidos

- Plan maestro: `PLAN_MAESTRO.md` (§5 arquitectura, §6 calendario, §8 diseño, §16 reglas).
- Rúbrica: `PLAN_MAESTRO.md` §2.
- Bitácora de A: `docs/bitacora-David.md`. Bitácora de B: `docs/bitacora-B.md`.
