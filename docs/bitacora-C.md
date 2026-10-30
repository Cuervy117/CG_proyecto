---
title: Bitácora individual — C
autor: C (completar nombre)
materia: Computación Gráfica e Interacción Humano-Computadora
semestre: 2027-1
proyecto: Proyecto final CGICH
inicio: 2026-10-30
entrega: 2026-11-07
estado: en curso
---

# Bitácora individual — C

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
| Mi rol | **C — avatar, animación, experiencias, HUD y docs** |
| Rutas que edito | `src/game/avatar/`, `src/game/anim/`, `src/game/Experience.*`, `src/game/main.cpp`, `src/ui/`, `Input.*`, `docs/` |
| Stack | C++17 · OpenGL 4.3 (contexto actual; objetivo 3.3) · GLFW · GLEW · GLM · Assimp · Visual Studio 2022 |
| Rama | `feat/c-avatar` |
| Repositorio | Local en `C:\UNAM\CGICH` (remoto pendiente de que A lo suba) |
| Entrega | sábado 7-nov-2026, 12:00, Google Classroom |
| Herramientas | Visual Studio 2022, Git, IA como apoyo de planificación y revisión |

---

## 2. Planeación

### 2.1 Objetivo

Construir el avatar, las animaciones y la interfaz:

- Avatar jerárquico no humanoide (3 patas, 2 brazos, cresta, ojo) con caminata.
- Textura y material del avatar (specular/shininess, emisivo en cresta/ojos).
- Tres animaciones por teclado y tres continuas.
- Cuatro recorridos y cambio correcto de cámaras (con A).
- HUD, `Experience`, `main.cpp`.
- Manual de usuario, manual técnico, video y videoblog.

### 2.2 Reglas que aplican a mi rol

- Reglas compartidas: **`PLAN_MAESTRO.md` §5, §6, §8 y §16**.
- No toco `src/engine/`, `src/scene/`, `src/game/cameras/`, `src/game/world/`, `Load.*` ni `P09-*.cpp`.
- Assets nuevos en `Models/c/` y `Textures/c/`; nunca commiteo binarios.
- `.vcxproj`: solo agrego archivos en el bloque `C-avatar`; no commitear `.vcxproj.filters`.
- Cuando cree `src/game/main.cpp`, en el mismo commit excluyo `P09-2-424104230.cpp` del build.

### 2.3 Hitos y sprints

| Sprint | Fechas | Objetivo de mi rol | Estado |
|---|---|---|---|
| S1 | 30–31 oct | Avatar + caminata | pendiente |
| S2 | 31 oct–1 nov | Experiencias 1 y 4 | pendiente |
| S3 | 1–2 nov | 3 animaciones por teclado | pendiente |
| S4 | 2–3 nov | 3 animaciones continuas + partículas | pendiente |
| S5 | 3–4 nov | QA de cámaras y HUD | pendiente |
| S6 | 4–7 nov | Manuales, video, videoblog | pendiente |

### 2.4 Riesgos y recortes

- **Riesgo de tiempo:** el avatar puede llevar más de dos días. Plan B: 4 extremidades y piezas del rover.
- **Riesgo de integración:** dos `main` en el build; se resuelve en el commit de `src/game/main.cpp`.
- **Orden de sacrificio:** colisión de cámara → cámara 4 cinematográfica → variedad de animaciones.

---

## 3. Hitos

| ID | Fecha | Hora | Hito | Descripción | Evidencia | Horas | Estado |
|---|---|---|---|---|---|---|---|
| — | — | — | — | — | — | — | — |

---

## 4. Registro diario (formato del PDF)

| Fecha | Qué me propuse | Qué hice | Horas |
|---|---|---|---:|
| 2026-10-30 | Primer arranque: clonar, copiar assets, compilar y correr; avatar + caminata |  | 0 |
| 2026-10-31 | Experiencias 1 y 4 |  | 0 |
| 2026-11-01 | 3 animaciones por teclado |  | 0 |
| 2026-11-02 | 3 animaciones continuas y partículas |  | 0 |
| 2026-11-03 | QA de cámaras y HUD |  | 0 |
| 2026-11-04 | Manual de usuario |  | 0 |
| 2026-11-05 | Manual técnico y video |  | 0 |
| 2026-11-06 | Buffer / videoblog |  | 0 |
| 2026-11-07 | Entrega |  | 0 |

> Las horas de días futuros se llenan al cerrar cada día. No se estiman: se anotan las reales.

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
