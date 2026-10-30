# Plan maestro — Proyecto final CGICH

> **Qué es este archivo.** El plan completo, autocontenido y portable, para llevarlo a la PC de escritorio (Windows).
> **Qué NO es.** No es la bóveda de Obsidian ni un diario. La bóveda se queda en Linux para gestión del equipo; este archivo es el traspaso operativo.
> **Destino sugerido:** `C:\CG\PLAN_MAESTRO.md`
> **Fecha de este documento:** 29-oct-2026
> **Entrega:** sábado **7-nov-2026, 12:00**, Google Classroom.

---

## 0. Cómo usar este archivo

### Para ti (A)
1. Cópialo a la PC de escritorio, en la raíz del proyecto, como `PLAN_MAESTRO.md`.
2. Abre un agente de código en esa carpeta y pégale el **prompt de la sección 15**.
3. El agente debe ejecutar la **Fase 0** (sección 4) y generar `AGENTS.md` con datos reales del entorno.
4. No dejes que escriba código antes de terminar la Fase 0.

### Para el agente
1. Lee este archivo completo antes de tocar nada.
2. Ejecuta la Fase 0 y **genera `AGENTS.md`** con lo que encuentres. No inventes rutas, versiones ni comandos.
3. Si el proyecto ya compila y corre, **no cambies el build**. Solo agrega lo que falte.
4. El código manda sobre este plan. Si hay contradicción, gana el código y se actualiza el plan.
5. No leas la bóveda de Obsidian: no está en esta máquina y no hace falta.

---

## 1. Resumen ejecutivo

**Proyecto:** ambiente virtual interactivo 3D con temáticas "Viajes espaciales" y "Evolución de especies intergalácticas".
**Equipo:** 3 personas (A, B, C).
**Stack:** C++17, OpenGL 3.3 core, GLFW, GLEW, GLM, Assimp, Visual Studio 2022 (`v143`, x64).
**Estado de partida real:** el proyecto original ya está en la PC de escritorio y **ya corre**. No hay que reconstruir ninguna unión de carpetas.
**Regla del PDF que condiciona todo:** los objetos, animaciones, texturas e iluminación de las prácticas **no cuentan** como contenido del proyecto final.

**Lo que ya existe (motor heredado):** ventana, input, cámara libre, shaders con luz direccional/puntual/spot, materiales, carga de OBJ/FBX/GLB, skybox, geometría procedural, pista por tramos, sátelite jerárquico, partículas.

**Lo que falta (el trabajo real):**

| Bloque | Persona-días |
|---|---:|
| Grafo de escena (`SceneNode`) | 1.0 |
| 4 cámaras + director + transiciones | 2.0 |
| Avatar jerárquico + caminata | 2.0 |
| 2 planetas con evolución | 3.0 |
| Pista nueva + naves | 1.5 |
| Plataforma + despegue | 1.5 |
| Estación + atraque | 1.5 |
| NPCs | 1.0 |
| Ciclo día/noche + spots + skybox | 1.0 |
| 3 animaciones por teclado | 1.0 |
| Texturizado | 1.5 |
| HUD + pulido | 1.0 |
| Release + QA | 1.5 |
| Manuales, video, bitácoras | 2.0 |
| **Total** | **~24** |

**Disponible:** 3 personas × 9 días ≈ **27 persona-días**. Colchón ~3 días. Viable **solo sin reescribir el motor**.

---

## 2. Reglas del juego

### 2.1 Ponderación

- **60 %** elementos del escenario (410 pts, normalizados a 10; hasta 470 → 11.46).
- **20 %** planeación (40 pts).
- **20 %** desarrollo (80 pts).

### 2.2 Rúbrica del escenario

| Rubro | Pts |
|---|---:|
| Geometría 3D | 10 |
| Jerarquía | 10 |
| Ambientación (3 universos + 2 temáticas) | 40 |
| NPCs | 10 |
| Pista de carreras | 20 |
| Plataforma de lanzamiento | 20 |
| Estación espacial | 20 |
| Satélites, lunas, asteroides | 10 |
| 2 planetas con evolución | 40 |
| Texturizado | 20 |
| Avatar jerárquico | 10 |
| Avatar: textura + material | 10 |
| Avatar: caminata | 10 |
| Recorrido 1: planeta, cámara 3ª persona en XZ | 10 |
| Recorrido 2: nave de carrera | 10 |
| Recorrido 3: nave entre planetas | 10 |
| Recorrido 4: lanzamiento/estación/celestes | 10 |
| Cambio correcto de cámaras | 10 |
| Ciclo día/noche direccional | 10 |
| Puntuales on/off por ciclo | 10 |
| Skybox por planeta | 10 |
| Spotlights por teclado | 10 |
| Animación básica: 3 por teclado | 30 |
| Animación compleja: 3 continuas | 30 |
| Estilo visual | 30 |
| Originalidad (extra) | ≤30 |
| Animaciones adicionales (extra) | ≤30 |

### 2.3 Metodología (40 %)

**Planeación (40 pts):**
- Documento del repositorio con capturas: ramas por alumno, organización, carpeta de almacenamiento.
- Documento de propuesta corregido (ya entregado).
- Plano/mapa final (ya entregado).

**Desarrollo (80 pts):**
- Ejecutable Release `.exe` + funcionamiento sin errores (10).
- Manual de usuario (10).
- Manual técnico (10).
- Bitácora individual: qué me propuse / qué hice / horas (10).
- Video demo sin narración (20).
- Videoblog (20).

### 2.4 La regla que cambia la estrategia

> "NO se contarán los objetos, animaciones, texturas, iluminación presentada como ejercicios de clase o reportes de prácticas. Si los elementos presentados (excluyendo modelos optimizados y texturas) no son creados por los alumnos, automáticamente el apartado de *Elementos a incluir dentro del Escenario* tendrá calificación de CERO."

**Consecuencia:** se reutiliza el motor y los patrones; se rehacen la pista, la nave, el sátelite y las texturas del profesor que se presenten como contenido final.

---

## 3. Estado de partida

- El proyecto original está en la PC de escritorio y **compila y corre**.
- **No** se reconstruye la unión de los snapshots. Ese trabajo era para el escenario de "solo tengo los artefactos"; ya no aplica.
- Los artefactos siguen sirviendo como **inventario de assets** y como **referencia de patrones** (pista, sátelite, partículas). Ver Anexo A.
- El repo de GitHub del equipo es `https://github.com/Cuervy117/CG_proyecto`. Verificar si la carpeta local ya está conectada; si no, inicializar git y subir la línea base.
- La bóveda de Obsidian se queda en Linux. En Windows se trabaja con `PLAN_MAESTRO.md` + `AGENTS.md` + un `docs/bitacora-David.md`.

**Riesgo principal del estado de partida:** que la carpeta de escritorio sea la carpeta de prácticas (varios `main`, varios `.vcxproj`, assets mezclados). La Fase 0 debe identificar **el main y el proyecto que realmente corren**, y no tocar el resto.

---

## 4. FASE 0 — Descubrimiento en la PC (bloqueante)

> **Objetivo:** saber exactamente qué hay, cómo se compila y cómo se corre, antes de escribir una línea.
> **Tiempo:** 45–90 min.
> **Salida:** `AGENTS.md` generado + commit `v0.1-base`.
> **Regla:** si algo compila pero da warnings, se anota; no se "limpia".

### 4.1 Comandos de inventario (PowerShell)

```powershell
cd C:\CG                      # ajustar a la ruta real
Get-Location
Get-CimInstance Win32_VideoController | Select-Object Name, DriverVersion
Get-ChildItem -Recurse -Filter *.vcxproj | Select-Object FullName
Get-ChildItem -Recurse -Filter *.sln | Select-Object FullName
Get-ChildItem -Recurse -Filter *.cpp | Where-Object { $_.FullName -notmatch '\\x64\\|\\build\\|\\.vs\\' } | Select-Object FullName
Select-String -Path (Get-ChildItem -Recurse -Filter *.cpp).FullName -Pattern 'int\s+main' | Select-Object Path, LineNumber
Get-ChildItem Models, Textures, shaders -ErrorAction SilentlyContinue | Select-Object FullName
Get-ChildItem *.dll | Select-Object Name, Length
git status; git branch -a; git remote -v; git log --oneline -5
```

### 4.2 Preguntas que el agente debe responder

| # | Pregunta | Dónde se responde |
|---|---|---|
| 1 | ¿Ruta absoluta del proyecto? | `Get-Location` |
| 2 | ¿Qué `.sln`/`.vcxproj` es el vigente? | archivos + última fecha |
| 3 | ¿Qué `.cpp` tiene el `main` que corre? | `Select-String 'int main'` |
| 4 | ¿Qué otros `main` hay y están excluidos del build? | `.vcxproj` |
| 5 | ¿Release x64 compila sin errores? | build |
| 6 | ¿Cuántos warnings? ¿Cuáles son críticos? | salida del build |
| 7 | ¿El `.exe` corre y desde qué directorio? | ejecución |
| 8 | ¿Se ven modelos, texturas y skybox? | captura |
| 9 | ¿Faltan assets o hay rutas rotas? | consola |
| 10 | ¿Qué GPU y versión de OpenGL reporta? | driver / log |
| 11 | ¿La carpeta está en git? ¿rama? ¿remoto? | `git` |
| 12 | ¿Qué requisitos de la rúbrica ya se ven hoy? | captura + tabla |

### 4.3 Criterios de terminado de la Fase 0

- [ ] Un comando reproducible compila **Release x64** sin errores.
- [ ] Un comando reproducible ejecuta el `.exe` desde la raíz del proyecto.
- [ ] `AGENTS.md` creado con datos reales (sección 14).
- [ ] `docs/bitacora-David.md` creada, con la planeación y la regla de registro por hito.
- [ ] `PLAN_MAESTRO.md` copiado al repo.
- [ ] `git status` limpio; commit `chore: línea base del proyecto` + tag `v0.1-base`.
- [ ] Captura del estado actual (para el antes/después del manual técnico).
- [ ] Tabla de cobertura inicial (sección 7) marcada con lo que ya se ve.

### 4.4 Si no compila

Orden de arreglo, sin refactorizar:

1. `.vcxproj` incluye un `main` que no es el vigente → excluirlo.
2. Falta un `.cpp`/`.h` → recuperarlo de `artefactos/` (Anexo B).
3. Faltan `assimp-vc143-mt.dll`, `glew32.dll` o `glfw3.dll` junto al `.exe`.
4. Rutas de assets relativas al CWD → ejecutar desde la raíz.
5. `glfw3.lib` no existe (solo `glfw3_mt.lib`) → ajustar el link.
6. Diferencias Debug/Release con Assimp → compilar **Release**.

Si tras 90 minutos no compila, **no se escribe código nuevo**. Se documenta el error exacto y se resuelve al día siguiente.

### 4.5 Bitácora de David

Archivo: `docs/bitacora-David.md`.

- Se escribe **al cerrar cada hito**, no al final del proyecto.
- Cada entrada lleva **fecha y hora**, una descripción de **1–3 líneas**, evidencia y **horas reales**.
- Si un hito no se termina, queda `en curso`; no se marca como hecho.
- Tiene dos registros: **hitos** (granular) y **día** (formato del PDF: qué me propuse, qué hice, horas).
- La sección **Hitos previos** queda para completar la recolección de modelos y texturas ya realizada.
- La sección **Planeación** ya incluye objetivo, estrategia, sprints, reparto, riesgos y el apoyo de IA declarado.

---

## 5. Arquitectura objetivo

### 5.1 Decisión de fondo

> **No se reescribe el motor.** Se conserva la estructura "una clase por responsabilidad" que ya existe y se añaden **solo tres abstracciones**: grafo de escena, cámaras y experiencias.

- Nada de ECS, `AssetHandle`, CMake, PBR ni física.
- El build sigue en Visual Studio; `build.bat` (o el `.vcxproj`) es la verdad.
- Un solo `main`.
- OpenGL solo en `src/engine` y `Load.cpp`.

### 5.2 Estructura de carpetas

```
CG/
├─ src/
│  ├─ main.cpp
│  ├─ engine/     Window, Camera, Shader, Shader_light, Texture,
│  │              Mesh, Model, Skybox, luces, Material, Create
│  ├─ scene/      SceneNode, Scene, CameraRig, CameraDirector, DayNight
│  ├─ game/       Experience, cameras/, world/, avatar/, anim/
│  └─ ui/         Hud
├─ Models/
├─ Textures/
├─ shaders/
├─ include/  lib/  glm/  *.dll
├─ docs/
│  ├─ bitacora-David.md
│  └─ capturas/
├─ reference/     prácticas antiguas, NO compila
├─ AGENTS.md
├─ PLAN_MAESTRO.md
└─ build.bat
```

> Los assets se quedan en `Models/`, `Textures/` y `shaders/` en la raíz. Moverlos obliga a tocar todas las rutas y no da puntos.

### 5.3 Las tres abstracciones

#### `SceneNode` / `Scene`

```cpp
// src/scene/SceneNode.h
struct SceneNode {
    glm::vec3 position{0}, rotationEuler{0}, scale{1};
    SceneNode* parent = nullptr;
    std::vector<std::unique_ptr<SceneNode>> children;

    Model* model = nullptr;          // prestado; NO se libera aquí
    glm::vec3 tint{1};
    float specular = 0.3f, shininess = 4.0f;
    bool visible = true;
    std::string name;

    glm::mat4 world{1};
    glm::mat4 local() const;
    SceneNode* add(std::unique_ptr<SceneNode> child);
};
```

```cpp
// src/scene/Scene.h
class Scene {
public:
    SceneNode root;
    void updateWorld();                       // DFS, una vez por frame
    void render(Shader& s, const glm::mat4& view,
                const glm::mat4& proj);
    SceneNode* find(const std::string& name);
};
```

- `local()` = traslación → rotación Y/X/Z → escala.
- `updateWorld()` = `world = parent->world * local()`.
- Render recursivo con `model`, `tint`, `specular/shininess`, `visible`.
- 1–2 mil nodos: recalcular todo por frame es trivial.

#### `CameraRig` / `CameraDirector`

```cpp
// src/scene/CameraRig.h
class CameraRig {
public:
    virtual ~CameraRig() = default;
    virtual void update(float dt, const SceneNode& target) = 0;
    virtual glm::mat4 view() const = 0;
    float fov = 45.0f, nearZ = 0.1f, farZ = 2000.0f;
    glm::mat4 projection(float aspect) const;
protected:
    glm::vec3 m_pos{0}, m_target{0}, m_up{0,1,0};
};
```

```cpp
// src/scene/CameraDirector.h
class CameraDirector {
public:
    void setRig(std::unique_ptr<CameraRig> rig);
    void update(float dt, const SceneNode& target);
    glm::mat4 view() const;
    bool transitioning() const;
};
```

> **Corrección importante:** `glm::mix` **no existe para `mat4`** en GLM. La transición se hace interpolando `position` y `target`, o con un fade a negro de 0.15 s + cambio de rig + fade de vuelta.

#### `Experience`

```cpp
// src/game/Experience.h
enum class ExperienceId { Planeta = 0, Carrera, Vuelo, Lanzamiento };

class Experience {
public:
    virtual ~Experience() = default;
    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void update(float dt, Input& input) = 0;
    virtual CameraRig* makeCamera() = 0;
    virtual const char* hudText() const = 0;
};
```

Si el tiempo aprieta, puede ser un `switch` dentro de `App`. Lo que **no** se negocia es que cada modo tenga su `CameraRig` y su bloque de update.

### 5.4 Orden del frame

```
glfwPollEvents
→ input.update()
→ experiencia activa (input + animaciones + estado)
→ scene.updateWorld()
→ dayNight.update(dt)
→ cameraDirector.update(dt)
→ render: skybox → opacos → transparentes/partículas → HUD
→ swapBuffers
```

### 5.5 Reglas invariantes

1. OpenGL solo en `src/engine` y `Load.cpp`.
2. Todo update recibe `dt`; prohibido `glfwGetTime()` dentro de updates. `dt` limitado a 0.05.
3. Nada de clases por objeto (`Planeta1`, `Cohete`, `NPC2`): prefabs y funciones.
4. `Model*` en nodos es prestado; nunca `delete`.
5. Un dueño por archivo.
6. No cambiar firmas sin actualizar los contratos.
7. Assets binarios fuera del repo (o LFS).
8. No tocar `Recursos`, `Mesh`, `Load.cpp` "por limpieza".
9. Los snippets de este plan son indicativos: verificar la API real antes de copiarlos.
10. Si algo no compila, se reporta el error exacto; no se parchea a ciegas.

---

## 6. Plan de 9 días

> Hoy es **29-oct**. La Fase 0 sustituye al "día 0" original porque el proyecto ya corre.

| Día | Fecha | A — motor | B — mundo | C — avatar/animación | Hito |
|---|---|---|---|---|---|
| 0 | 29-oct | Fase 0, AGENTS.md, baseline git | Inventario de assets, layout | Repo/docs, esqueleto HUD | Base reproducida |
| 1 | 30-oct | `SceneNode` + cámaras 1 y 4 | Pista nueva + planeta 1 | Avatar + caminata | Avatar camina |
| 2 | 31-oct | Día/noche + spots + skybox | Planeta 2 + plataforma + cohete | Experiencias 1 y 4 | 2 recorridos |
| 3 | 1-nov | Cámaras 2 y 3 + director | Estación + celestes | 3 animaciones por teclado | 4 recorridos |
| 4 | 2-nov | Integración, fps, bugs | NPCs + texturizado | 3 continuas + partículas | Todo presente |
| 5 | 3-nov | QA render | QA visual | QA cámaras | **Freeze 18:00** |
| 6 | 4-nov | Release + máquina limpia | Video demo | Manual de usuario | `.exe` validado |
| 7 | 5-nov | Capturas técnicas | Capturas | Manual técnico + subida | Entrega lista |
| 8 | 6-nov | Buffer | Buffer | Bitácoras/videoblog | Colchón |
| 9 | 7-nov | — | — | — | **12:00 entrega** |

### 6.1 Ruta de A para hoy/esta noche (si se desvela)

| Bloque | Tiempo | Entregable | Criterio |
|---|---|---:|---|
| Fase 0 + baseline | 60 min | `AGENTS.md`, commit, tag | Build Release x64 verde |
| `SceneNode` + `Scene` | 120 min | jerarquía + render recursivo | Un hijo se mueve sin mover al padre |
| Cámaras 1 y 4 + director | 120 min | `FollowXZ`, `Orbit`, fade | `1` sigue en XZ; `4` orbita; cambio suave |
| Día/noche + puntuales | 90 min | `DayNight` 120 s | El sol se mueve y las luces cambian |
| Spots + skybox por zona | 60 min | `F` + cubemap | Se nota el cambio |
| Cierre | 45 min | tag `v0.2-motor`, handoff | Push + bitácora |

**Si algo se atora:** commitea lo que compile y documenta el stub. Un `v0.2` verde vale más que un `v0.5` roto.

---

## 7. Rúbrica: dónde vive cada cosa

| Rubro | Pts | Módulo | Estado inicial |
|---|---:|---|---|
| Geometría 3D | 10 | assets + escena | ✅ |
| Jerarquía | 10 | `SceneNode` | ⚠️ matrices manuales |
| Ambientación | 40 | mundo + assets | ⚠️ |
| NPCs | 10 | mundo | ❌ |
| Pista | 20 | mundo | ✅ patrón, contenido nuevo |
| Plataforma | 20 | mundo | ⚠️ cohete sí, plataforma no |
| Estación | 20 | mundo | ❌ |
| Satélites/celestes | 10 | mundo | ✅ |
| 2 planetas | 40 | mundo | ⚠️ |
| Texturizado | 20 | assets | ⚠️ |
| Avatar (3 rubros) | 30 | avatar | ❌ |
| Recorridos (4+1) | 50 | cámaras | ⚠️ solo libre |
| Iluminación (4 rubros) | 40 | luces | ⚠️ |
| Animación (2 rubros) | 60 | animación | ⚠️ |
| Estilo visual | 30 | arte | ⚠️ |
| Adicionales | ≤60 | varios | ✅ potencial |

**Orden de prioridad:** ambientación + planetas + animación (160) → recorridos + iluminación + estilo (120) → pista + plataforma + estación (60) → avatar (30) → resto (60).

**Orden de sacrificio:** sombras → PBR → colisión de cámara → audio → variedad de NPCs → crossfade de skybox → cámara 4 cinematográfica.

**Nunca sacrificar:** 2 planetas, 6 animaciones, 4 recorridos, avatar caminando, ciclo día/noche, `.exe`.

---

## 8. Diseño por subsistema

### 8.1 Escena y jerarquía

- Cubre **jerarquía (10)** y habilita **avatar (30)** y **estación**.
- Ejemplo de árbol del avatar:

```
avatar
├── cuerpo
│   ├── cresta
│   ├── ojo_sensor
│   ├── brazo_izq → antebrazo → pinza
│   └── brazo_der → antebrazo → pinza
├── pata_izq → espinilla → pie
├── pata_der → espinilla → pie
└── pata_tras → espinilla → pie
```

- Demostración en video: rotar `brazo_izq` sin mover el cuerpo; mover la raíz mueve todo.

### 8.2 Cámaras y recorridos (50 pts)

| # | Rig | Comportamiento |
|---|---|---|
| 1 | `FollowXZ` | avatar, offset `(0,2.2,-5)`, `y` fija al piso XZ, `lookAt` al avatar |
| 2 | `Chase` | detrás de la nave de carrera, `lerp` 6–8 Hz, FOV 55–60 |
| 3 | `Flight` | órbita amortiguada alrededor de la nave, distancia 8–15 |
| 4 | `Orbit/Launch` | órbita o 3 anclas; mira cohete/estación |
| debug | `Free` | WASD + mouse, `F1`, no cuenta |

- Teclas `1`–`4`; HUD confirma el modo.
- Transición: fade 0.15 s o interpolación de posición/target. **No** interpolar matrices con `glm::mix`.
- Cámara 1: clamp al piso; no atravesar el terreno.
- Cámara 2: `lookAt` al frente de la nave, no a la nave.

### 8.3 Iluminación (40 pts)

```cpp
struct DayNight {
    static constexpr float PERIOD = 120.0f;   // ≤ 2 min
    float phase = 0.30f;
    void update(float dt) { phase = fmodf(phase + dt / PERIOD, 1.0f); }
    glm::vec3 sunDir() const;
    float height() const;
    bool isNight() const { return height() < 0.05f; }
};
```

- `sunDir` orbita; color ámbar → blanco → azul; ambiente 0.35 → 0.08 (nunca 0).
- 3 puntuales: `isNight() ? 1.4 : 0.0`.
- 4 spots: `F` alterna; apagados = `SetSpotLights(nullptr, 0)`.
- Skybox: dos cubemaps y cambio por zona. Crossfade solo si sobra tiempo.
- Orden de caras: `+X, -X, +Y, -Y, +Z, -Z`.
- `stbi_set_flip_vertically_on_load(false)` antes del cubemap.
- `MAX_POINT_LIGHTS` debe coincidir en `CommonValues.h` y `shader_light.frag`.

### 8.4 Assets

- `Model` (Assimp) + `Texture` (stb) + `Mesh` + `Skybox` + `Recursos`.
- Cargar una vez al inicio; `Model*` prestado.
- Formatos: OBJ/FBX/GLB; texturas TGA/PNG/JPG.
- Correcciones: normales negadas por modelo; `aiProcess_FlipUVs` si las UV salen al revés; fugas en `~Model`.
- Release: convertir OBJ gigantes a GLB; quitar `.blend` y `All_Textures`; presupuesto < 200 MB.
- Rutas relativas a la raíz; nombres en minúsculas.

### 8.5 Animación (60 pts)

| Tipo | Objeto | Implementación |
|---|---|---|
| Teclado | Cohete | `Idle → Ignición → Ascenso → Órbita` + humo |
| Teclado | NPC | saludo: bob + giro + aleteo, 2 s |
| Teclado | Nave de carrera | boost: llama + inclinación + velocidad 2 s |
| Continua | Asteroides | órbita + ruido |
| Continua | Estación | anillo + brazos + naves en `FollowPath` |
| Continua | Naves | spline por tramos con velocidad variable |

- Flanco de tecla, no estado.
- `P` pausa: `dt = 0` para animaciones.
- Partículas: textura procedural 128×128, billboard, blend aditivo, vida 2 s.

### 8.6 Avatar (30 pts)

- Especie radial: 3 patas, 2 brazos, cresta flotante, ojo emisivo. **No humanoide.**
- Piezas procedurales o FBX por separado; sin skinning.
- Material: `specular 4.0`, `shininess 128`, emisivo en cresta/ojos.
- Caminata:

```cpp
float w = 2.0f * PI * 1.6f * speed * t;
pata_izq->rotationEuler.x  =  0.45f * sinf(w);
pata_der->rotationEuler.x  =  0.45f * sinf(w + PI);
pata_tras->rotationEuler.x =  0.45f * sinf(w + PI * 0.5f);
brazo_izq->rotationEuler.x =  0.30f * sinf(w + PI);
brazo_der->rotationEuler.x =  0.30f * sinf(w);
cuerpo->position.y = 0.85f + 0.05f * fabsf(sinf(w));
cresta->rotationEuler.y = 0.15f * sinf(0.7f * w);
```

- `E` saluda; `speed = 0` → idle.
- Pivotes en las articulaciones; si no, las piezas se separan.

### 8.7 Mundo

| Zona | Contenido |
|---|---|
| Planeta 1 (cristal) | terreno, 3 etapas de evolución, pista de hielo |
| Planeta 2 (volcánico) | terreno, 3 etapas, pista de obsidiana |
| Plataforma | cohete, torre, luces |
| Pista | óvalo irregular con subida y peralte |
| Estación | anillo, módulos, paneles, naves |
| Cinturón | asteroides, lunas, sátelites |
| NPCs | 2–3 especies, 3–5 instancias |

- Un solo mundo con zonas y `TriggerZone`; no 4 escenas.
- Pista nueva, no la del P09.
- Cohete: `rocket.glb` o procedural.
- Estación: toro + módulos; reutilizar el patrón del sátelite, no la geometría.

### 8.8 Release

- Build **Release x64**; `.exe` + DLLs + `Models/` + `Textures/` + `shaders/`.
- Prueba en máquina limpia el **día 4**, no el 7.
- Ruta del `.exe` sin espacios problemáticos; ejecutar desde la raíz.
- Tamaño objetivo < 200 MB.
- Workflow de GitHub Actions opcional para reproducibilidad.

---

## 9. Trampas conocidas

| # | Trampa | Parche |
|---|---|---|
| 1 | Normales negadas en `Model::LoadMesh` | bandera `flipNormals` por modelo |
| 2 | `LoadNode` aplana la jerarquía de Assimp | construir por partes con `SceneNode` |
| 3 | Skybox: orden de caras y `stbi` flip global | `+X,-X,+Y,-Y,+Z,-Z`; resetear flip |
| 4 | `skybox.frag` puede faltar en el snapshot 001 | copiar de 002 si falta |
| 5 | `Texture` copiable con `GLuint` | regla de 3 o `unique_ptr` |
| 6 | `~Model` vacío, `new Shader` sin liberar | `ClearModel()` al salir |
| 7 | `MAX_POINT_LIGHTS` duplicado | editar `.h` y `.frag` |
| 8 | GL 4.3 pedido sin necesidad | pedir 3.3 |
| 9 | `main` de 560 líneas | `App` + `Experience` |
| 10 | Rutas relativas al CWD | ejecutar desde raíz o resolver desde el exe |
| 11 | `glm::mix` para `mat4` no existe | interpolar posición/target |
| 12 | Debug CRT + Assimp release | compilar Release |

---

## 10. Reparto y git

| Rol | Dueño de | Entrega |
|---|---|---|
| **A** | `src/engine`, `src/scene`, `src/game/cameras`, build, release | escena, cámaras, luces, `.exe` |
| **B** | `src/game/world`, `Load.cpp`, `assets` | pista, planetas, plataforma, estación, NPCs, texturas |
| **C** | `src/game/avatar`, `anim`, `Experience`, `ui`, `Input`, `docs` | avatar, 6 animaciones, HUD, manuales/video |

**Git:**
- Ramas: `main`, `develop`, `feat/a-motor`, `feat/b-mundo`, `feat/c-avatar`.
- Ventanas de merge: 13:00 y 19:00.
- `develop` siempre compila.
- Tags: `v0.1-base`, `v0.5-alpha`, `v1.0-entrega`.
- Commits convencionales; push diario.

**Anti-colisión:**
1. Un dueño por ruta; nadie más edita.
2. `main.cpp` y `Experience` son de C.
3. `config/input` es de C.
4. Assets por subcarpeta de dueño.
5. Bitácora individual; nadie más la edita.

---

## 11. Riesgos y planes B

| Riesgo | Prob. | Impacto | Plan B |
|---|---|---|---|
| El proyecto de escritorio no es el que corre / hay varios main | Media | Alto | Fase 0 identifica el vigente; no borrar nada |
| Avatar se lleva > 2 días | Alta | Alto | 4 extremidades y piezas del rover |
| Cámara 4 no queda | Media | Medio | órbita fija |
| Rendimiento con partículas | Media | Medio | bajar counts; sin sombras |
| Contenido de práctica reconocible | Media | **Alto** | rehacer pista/nave/sátelite y documentarlo |
| `.exe` falla en máquina limpia | Media | Alto | probar el día 4 |
| Falta de tiempo por otras materias | Alta | Medio | recortar adicionales y NPCs |
| Conflictos de merge | Media | Bajo | un dueño por ruta |

**Disparadores:**
- 29-oct 23:59: si la Fase 0 no cierra → el viernes se dedica a eso.
- 31-oct: si el avatar no camina → simplificarlo.
- 2-nov: si no hay 4 recorridos → cámara 4 a órbita fija.
- 4-nov: si el `.exe` no corre → congelar y arreglar empaquetado.

---

## 12. Decisiones (ADR)

| # | Decisión | Consecuencia |
|---|---|---|
| 1 | No reescribir el motor | se ahorran 4–5 días; se hereda deuda |
| 2 | Visual Studio + `.vcxproj` | build nativo Windows; sin CMake |
| 3 | Conservar "una clase por responsabilidad" | cero refactor de 2 días |
| 4 | Solo 3 abstracciones | ~1.5 días; cubren jerarquía/cámaras/modos |
| 5 | Contenido de prácticas excluido | rehacer pista/nave/sátelite/texturas |
| 6 | Un `main` + `Experience` | flujo legible; sin clases por objeto |
| 7 | Avatar procedural, sin skinning | cumple 30 pts; bajo riesgo |
| 8 | Assets en la raíz | cero cambios de rutas |
| 9 | Sin PBR, sombras ni física | se prioriza lo calificado |
| 10 | Release en Windows | paridad con B/C y con el PDF |

---

## 13. Entregables y checklist final

- [ ] `.exe` Release + DLLs + assets + código comentado.
- [ ] Manual de usuario con capturas de teclado/mouse.
- [ ] Manual técnico: capturas + bloques de código por rubro + conclusiones + bibliografía.
- [ ] Bitácoras individuales (qué me propuse / qué hice / horas).
- [ ] Bitácora de David con hitos fechados y evidencia (`docs/bitacora-David.md`).
- [ ] Video demo sin narración, mostrando **todos** los elementos.
- [ ] Videoblog.
- [ ] Documento del repositorio: ramas, capturas, organización, carpeta de almacenamiento.
- [ ] Propuesta y plano corregidos (ya entregados, incluir).
- [ ] Subida a Classroom antes del 7-nov 12:00.

---

## 14. Plantilla de `AGENTS.md`

> El agente debe **llenar los `<>` con datos reales** de la Fase 0. No dejar placeholders.

```markdown
# AGENTS.md — Proyecto final CGICH

## 0. TL;DR
Ambiente espacial 3D en C++17 + OpenGL 3.3. Entrega: 7-nov-2026 12:00.
Windows + Visual Studio 2022 (v143, x64). Build: <comando exacto>.
No CMake, no ECS, no PBR.

## 1. Tu rol
Ayudas a A (motor y cámaras). No toques archivos de B ni de C
salvo que la tarea lo diga.

## 2. Datos duros
- Repo: <ruta absoluta>   Rama: <rama>
- Build: <comando>  → <ruta del exe>
- Run: <comando> desde <directorio>
- Main vigente: <archivo>
- GL/GPU: <versión> / <GPU>
- Assets: <carpeta> (ya presentes; no los muevas)

## 3. Estado actual (actualizar al cerrar cada sesión)
- Compila: <SÍ/NO>. Último commit: <hash>.
- Funciona: <...>
- Stub: <...>
- Siguiente tarea: <...>

## 4. Lee antes de codificar
1. PLAN_MAESTRO.md (secciones 5, 6, 8, 9)
2. <headers de los módulos que vas a tocar>

## 5. Reglas invariantes
- No reescribir el motor ni introducir capas nuevas.
- OpenGL solo en src/engine y Load.cpp.
- Todo update recibe dt; prohibido glfwGetTime dentro de updates.
- Nada de clases por objeto: SceneNode + funciones.
- Model* en nodos es prestado; nunca delete.
- Un dueño por archivo.
- No cambiar firmas sin actualizar los contratos.
- No meter assets binarios al repo.
- No tocar la bóveda de Obsidian.
- Los snippets del plan son indicativos: verifica la API real.

## 6. Mapa de archivos
<árbol real, no el ideal>

## 7. Tarea de hoy
Objetivo: <...>
Pasos: <...>
DoD:
- [ ] <...>
No hacer: <...>

## 8. Cómo verificar
- Compila: <comando> (0 errores)
- Corre: <comando>; se ve <...>
- <prueba específica>

## 9. Trampas
<las 12 de la sección 9, filtradas a lo que tocarás>

## 10. Formato de respuesta
1. Plan en ≤5 bullets si el cambio > 30 líneas.
2. Al terminar: archivos tocados, cómo probar, pendientes.
3. Si no compila: error literal + hipótesis + parche mínimo.
4. No decir "listo" sin compilar.

## 11. Prohibido
- Añadir dependencias, CMake, ImGui, ECS, PBR.
- Refactorizar Recursos o Load.cpp "por limpieza".
- Cambios masivos de includes o nombres.
- Escribir en docs/ o en la bóveda.
```

---

## 15. Prompt de arranque para el agente de escritorio

```text
Eres un ingeniero C++/OpenGL. Ayudas a A en el proyecto final CGICH.
Trabajas en <RUTA>, rama <RAMA>. El proyecto ya compila y corre: no lo reescribas.

Antes de escribir código:
1. Lee PLAN_MAESTRO.md completo.
2. Ejecuta la FASE 0 (sección 4) y responde la tabla de 12 preguntas.
3. Genera AGENTS.md con la plantilla de la sección 14, con datos reales.
4. Muéstrame el AGENTS.md y un plan de 5 bullets para la tarea de hoy.
5. Espera mi OK.

Reglas: no CMake, no ECS, no PBR; OpenGL solo en src/engine; un dueño por archivo;
no inventes rutas ni APIs; no digas "listo" sin haber compilado.

Al terminar cada sesión: actualiza en AGENTS.md "Estado actual"
(compila, último commit, funciona, stub, siguiente tarea).
```

---

## Anexo A — Inventario de assets

> Solo como referencia. Los assets ya están en la PC; verificar cuáles existen.

| Asset | Uso | Nota |
|---|---|---|
| `rocket.obj` / `rocket.glb` + `rocket_diffuse.jpg` | Cohete | GLB preferido; OBJ 28 MB |
| `Landsat 1, 2, and 3.glb` | Estación | 16 MB |
| `satelite_*.fbx` | Sátelites / patrón de estación | |
| `Stylized_Planets.obj` | 2 planetas | |
| `Rocky_Asteroid_6.obj` | Asteroides | |
| `Fighter_03` | Naves de carrera | solo BaseColor |
| `spacenebulaskybox/*` | Skybox espacial | 6 caras |
| `Skybox/cupertin-lake_*` y `-night_*` | Skybox día/noche | |
| `panel.jpg`, `fuego.png` | Estación, llamas | |
| `flechas.tga`, `numerosbase.tga` | UV animadas | material del profesor: placeholder |
| `uh60.obj`, rover, holocron | Props/NPCs | |

**Presupuesto release:** modelos < 120 MB, texturas < 80 MB, total < 200 MB.

---

## Anexo B — Si falta un archivo fuente

> **No aplica al flujo normal.** El proyecto de escritorio ya corre. Esto es solo para recuperar un archivo perdido.

Los dos snapshots del curso son complementarios:

- **001** tiene: `Camera.*`, `Create.*`, `Mesh_tn.*`, `Model.cpp`, `Shader*`, `DirectionalLight.*`, `Light.*`, `SpotLight.*`, `stb_image.h`, `CommonValues.h`, `Load.hpp`, `Window.h`, `P09`.
- **002** tiene: `Input.*`, `Load.cpp`, `Material.*`, `PointLight.*`, `Texture.*`, `Window.cpp`, `Skybox.*`, `Model.h`, `Shader_light.h`, `Sphere.h`, `E09-2`, assets del cohete y skybox completo.

Ruta de los artefactos: `/data/CG_proyecto/artefactos/` (Linux). Copiar solo el archivo faltante, no la carpeta completa.

---

## Anexo C — Comandos Windows

```powershell
# Compilar (si existe build.bat)
.\build.bat

# O con MSBuild (ajustar ruta de VS)
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" `
  .\LabCGeIHC.sln /p:Configuration=Release /p:Platform=x64

# Ejecutar desde la raíz (assets relativos)
.\build\CGICH.exe

# Git
git status
git checkout -b feat/a-motor
git add -A
git commit -m "chore: línea base del proyecto"
git tag v0.1-base
git push -u origin feat/a-motor

# Evitar que Defender ralentice el build
Add-MpPreference -ExclusionPath "C:\CG"
```

> Ajustar `C:\CG`, `build\CGICH.exe` y el nombre del `.sln` a lo que exista realmente. **No inventar rutas.**
