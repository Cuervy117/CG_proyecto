---
title: Bitácora individual — B
autor: B (completar nombre)
materia: Computación Gráfica e Interacción Humano-Computadora
semestre: 2027-1
proyecto: Proyecto final CGICH
inicio: 2026-10-30
entrega: 2026-11-07
estado: en curso
---

# Bitácora individual — B

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
| Mi rol | **B — mundo** |
| Rutas que edito | `src/game/world/`, `src/engine/Load.*`, `Models/`, `Textures/` |
| Stack | C++17 · OpenGL 4.3 (contexto actual; objetivo 3.3) · GLFW · GLEW · GLM · Assimp · Visual Studio 2022 |
| Rama | `feat/b-mundo` |
| Repositorio | Local en `C:\UNAM\CGICH` (remoto pendiente de que A lo suba) |
| Entrega | sábado 7-nov-2026, 12:00, Google Classroom |
| Herramientas | Visual Studio 2022, Git, IA como apoyo de planificación y revisión |

---

## 2. Planeación

### 2.1 Objetivo

Construir el mundo del escenario:

- Pista de carreras nueva (óvalo irregular, subida y peralte) en el Planeta 1.
- Dos planetas con tres etapas de evolución de especies cada uno.
- Plataforma de lanzamiento, cohete y torre.
- Estación espacial, satélites, lunas y asteroides.
- NPCs (2–3 especies, 3–5 instancias).
- Texturizado de las zonas y material de los objetos del mundo.

### 2.2 Reglas que aplican a mi rol

- Reglas compartidas: **`PLAN_MAESTRO.md` §5, §6, §8 y §16**.
- No toco `src/engine/` (salvo `Load.*`), `src/scene/`, `src/game/cameras/`, `src/ui/`, `Input.*` ni el main.
- Assets nuevos en `Models/b/` y `Textures/b/`; nunca commiteo binarios.
- `.vcxproj`: solo agrego archivos en el bloque `B-world`.
- No commitear `.vcxproj.filters`.
- `Load.*` es mío, pero un cambio de firma requiere avisar a A y C.

### 2.3 Hitos y sprints

| Sprint | Fechas | Objetivo de mi rol | Estado |
|---|---|---|---|
| S1 | 30–31 oct | Pista nueva + Planeta 1 | pendiente |
| S2 | 31 oct–1 nov | Planeta 2 + plataforma + cohete | pendiente |
| S3 | 1–2 nov | Estación + satélites/lunas/asteroides | pendiente |
| S4 | 2–3 nov | NPCs + texturizado | pendiente |
| S5 | 3–4 nov | QA visual y correcciones | pendiente |
| S6 | 4–7 nov | Capturas para manuales/video | pendiente |

### 2.4 Riesgos y recortes

- **Riesgo de contenido:** que la pista o el satélite se reconozcan como práctica. Se rehacen geometría y disposición.
- **Riesgo de assets:** rutas relativas al CWD; ejecutar siempre desde la raíz.
- **Orden de sacrificio:** variedad de NPCs → detalle de texturas → asteroides extra.

---

## 3. Hitos

| ID | Fecha | Hora | Hito | Descripción | Evidencia | Horas | Estado |
|---|---|---|---|---|---|---|---|
| — | — | — | — | — | — | — | — |

---

## 4. Registro diario (formato del PDF)

| Fecha | Qué me propuse | Qué hice | Horas |
|---|---|---|---:|
| 2026-10-30 | Primer arranque: clonar, copiar assets, compilar y correr; pista nueva + Planeta 1 |  | 0 |
| 2026-10-31 | Planeta 2 + plataforma + cohete |  | 0 |
| 2026-11-01 | Estación + celestes |  | 0 |
| 2026-11-02 | NPCs + texturizado |  | 0 |
| 2026-11-03 | QA visual |  | 0 |
| 2026-11-04 | Correcciones y capturas |  | 0 |
| 2026-11-05 | Apoyo a manuales (capturas) |  | 0 |
| 2026-11-06 | Buffer |  | 0 |
| 2026-11-07 | Entrega |  | 0 |

> Las horas de días futuros se llenan al cerrar cada día. No se estiman: se anotan las reales.

---

## 5. Pendientes y bloqueos

- [ ] Copiar `Models/`, `Textures/`, `include/`, `lib/`, `glm/` y DLLs desde la carpeta compartida.
- [ ] Verificar Release x64 (0 errores) y `.exe` desde la raíz.
- [ ] Generar mi `AGENTS.md` local con `PLAN_MAESTRO.md` §15.1 (no se commitea).
- [ ] Confirmar rama `feat/b-mundo`.

---

## 6. Enlaces rápidos

- Plan maestro: `PLAN_MAESTRO.md` (§5 arquitectura, §6 calendario, §8 diseño, §16 reglas).
- Rúbrica: `PLAN_MAESTRO.md` §2.
- Bitácora de A: `docs/bitacora-David.md`. Bitácora de C: `docs/bitacora-C.md`.
