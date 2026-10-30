# AGENTS.md — Proyecto final CGICH

> **Jerarquía de verdad:** el código > este archivo > `PLAN_MAESTRO.md`.
> **Fecha de esta línea base:** 2026-10-29.
> **Entrega:** sábado 2026-11-07, 12:00, Google Classroom.

## 0. TL;DR

Ambiente espacial 3D en C++17 + OpenGL (contexto actual 4.3 core; objetivo del plan 3.3).
Windows 10 Pro + Visual Studio 2022 Community (`v143`, x64).
Build: `MSBuild .\LabCGeIHC.sln /p:Configuration=Release /p:Platform=x64`.
No CMake, no ECS, no PBR, no ImGui, no física.

## 1. Tu rol

Ayudas a **A (David)**: motor, escena, cámaras, iluminación, build y release.
No toques archivos de B ni de C salvo que la tarea lo diga.

| Rol | Dueño de | No tocar sin permiso |
|---|---|---|
| **A** | Motor, escena, cámaras, build y release; todos los archivos actuales de la raíz | — |
| **B** | Mundo: pista, planetas, plataforma, estación, NPCs, texturas | `Load.cpp` y assets de mundo |
| **C** | Avatar, animación, `Experience`, UI/HUD, `Input.*`, docs | `Input.cpp`, `Input.hpp`, manuales |

> `P09-2-424104230.cpp` es el **main de prueba actual**; lo usa A para integrar el motor. Cuando exista el `main.cpp` del proyecto final, será de C.

## 2. Datos duros

| Campo | Valor real |
|---|---|
| Ruta absoluta | `C:\UNAM\CGICH` |
| SO | Windows 10 Pro 10.0.19045, x64 |
| IDE / toolset | Visual Studio 2022 Community, `v143`, x64 |
| Estándar | C++17 |
| OpenGL | Contexto pedido en `Window.cpp`: **4.3 core**. Driver reporta **4.3.0 NVIDIA 581.80**; GLSL **4.30**. El plan objetivo dice 3.3 (trampa #8); no se cambió en esta fase. |
| GPU | NVIDIA GeForce GTX 1660 SUPER/PCIe/SSE2 (driver Windows 32.0.15.8180) |
| Proyecto vigente | `LabCGeIHC.sln` + `LabCGeIHC.vcxproj` (único `.sln`/`.vcxproj`) |
| Main vigente | `P09-2-424104230.cpp`, `int main()` en línea 382 |
| Main P09 alterno | `P09-424104230.cpp` (pista/nave/satélite) se conserva como referencia, **excluido del build** |
| Build | `& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" .\LabCGeIHC.sln /p:Configuration=Release /p:Platform=x64 /m /v:minimal` |
| Salida | `C:\UNAM\CGICH\x64\Release\LabCGeIHC.exe` |
| Run | `.\x64\Release\LabCGeIHC.exe` con CWD `C:\UNAM\CGICH` |
| Assets | `Models/`, `Textures/`, `shaders/` en la raíz; rutas relativas al CWD. Verificado: **todos** los paths de `Load.cpp` existen. |
| Repo | **Nuevo repo local** en `C:\UNAM\CGICH`, rama `main`, **sin remoto** (David lo agrega). |
| Repo anterior | `https://github.com/Cuervy117/CGeIHC.git`, rama `practica_9`, commit `843ab2f`. Su `.git` se respaldó fuera del workspace y se descartó del proyecto. |
| DLLs | `glew32.dll`, `glfw3.dll`, `assimp-vc143-mt.dll` en la raíz (locales; no versionadas). |
| Librerías | `include/`, `lib/`, `glm/` en la raíz (locales; no versionadas). |

## 3. Estado actual

> Actualizar al cerrar cada sesión. Es la única sección volátil de este archivo.

| Campo | Estado |
|---|---|
| Compila | **SÍ**: Release x64, **0 errores**, **64 warnings**. |
| Último commit | Línea base: `362f19b` (tag `v0.1-base`). Cierre de sesión: ver `git log -1 --oneline`. |
| Funciona | P09-2 corre desde la raíz: ventana `Practica 09: Animacion`, skybox cupertin-lake, piso con textura de ladrillo, holocron jerárquico animado, cohete con despegue/caída y llama billboard, cámara libre y pausa con `P`. |
| Stub / roto | No existe `SceneNode`/`Scene`/`CameraRig`/`Experience`. No hay ciclo día/noche, spots, avatar, HUD ni los 4 recorridos. `P09-424104230.cpp` (pista/nave/satélite) no está compilado. |
| Siguiente tarea | Esperando OK para `SceneNode` + `Scene` (plan de 5 bullets en la sección 7). |
| Riesgo abierto | 64 warnings (C4715 en `Window::Initialise` es el más serio); OpenGL 4.3 vs objetivo 3.3; assets ~307 MB fuera del repo; repo nuevo sin remoto. |

**Warnings (conteo real del build):** 36× C4244, 16× C4477, 9× C4267, 1× C4474, 1× C4715, 1× C4804.
**Críticos a vigilar:** C4715 (`Window::Initialise` sin `return` en todas las rutas), C4474/C4477 (formatos `printf`/`snprintf`), C4804 (`Model.cpp:51`, comparación con `bool`).

## 4. Fase 0 — resultados (2026-10-29)

| # | Pregunta | Respuesta |
|---|---|---|
| 1 | ¿Ruta absoluta del proyecto? | `C:\UNAM\CGICH` |
| 2 | ¿Qué `.sln`/`.vcxproj` es el vigente? | `LabCGeIHC.sln` / `LabCGeIHC.vcxproj` (únicos; `.vcxproj` modificado 2026-10-29 19:41) |
| 3 | ¿Qué `.cpp` tiene el `main` que corre? | `P09-2-424104230.cpp` (línea 382), incluido en el `.vcxproj` |
| 4 | ¿Qué otros `main` existen y están excluidos? | Se retiraron del árbol `E02`, `E06` (+3 `.bak`), `E09`, `E09-2`, `mainbase`, `P01`, `P05`, `P06`, `P07`, `P08` (+`.bak_refactor`), `practica4_sinjerarqu\u00eda`, `Previo06`, `Previo07`, `T03`, `T04`. Respaldo en `C:\Users\David\AppData\Local\Temp\opencode\cgich_backup_2026-10-29\mains_eliminados`. Solo quedan `P09-2` (compilado) y `P09-424104230.cpp` (referencia, excluido). |
| 5 | ¿Release x64 compila sin errores? | **SÍ** (MSBuild exit 0) |
| 6 | ¿Cuántos warnings? ¿Cuáles son críticos? | 64 warnings; 0 errores. Crítico: C4715; formatos C4474/C4477; C4804. El resto son conversiones C4244/C4267. |
| 7 | ¿El `.exe` corre y desde qué directorio? | **SÍ**, `.\x64\Release\LabCGeIHC.exe` con CWD `C:\UNAM\CGICH`; ventana `Practica 09: Animacion`, responde; sin salida de error en consola. |
| 8 | ¿Se ven modelos, texturas y skybox? | **SÍ**: skybox cupertin-lake, piso con textura, holocron azul/dorado y cohete blanco. Evidencia: `docs/capturas/fase0_p09.png`. |
| 9 | ¿Faltan assets o hay rutas rotas? | **No**: los 61 paths de `Load.cpp` (modelos y texturas) existen. |
| 10 | ¿Qué GPU y versión de OpenGL reporta? | NVIDIA GeForce GTX 1660 SUPER/PCIe/SSE2; OpenGL **4.3.0 NVIDIA 581.80**; GLSL **4.30**. |
| 11 | ¿Git: rama, remoto, estado? | Antes: rama `practica_9`, remoto `https://github.com/Cuervy117/CGeIHC.git`, 88 cambios sin commit. Ahora: repo nuevo local, rama `main`, sin remoto; `.git` anterior respaldado. |
| 12 | ¿Qué rubros de la rúbrica ya se ven hoy? | Geometría 3D ✅; texturizado ⚠️ (piso + skybox); iluminación ⚠️ (direccional + 1 puntual); animación ⚠️ (cohete/holocron); jerarquía ⚠️ (matrices manuales, sin `SceneNode`); ambientación ⚠️; el resto ❌ (NPCs, pista, plataforma dedicada, estación, 2 planetas, avatar, 4 recorridos, día/noche, spots, HUD). |

## 5. Lee antes de codificar

1. `PLAN_MAESTRO.md` (secciones 5, 6, 8 y 9).
2. Headers reales de los módulos que vas a tocar: `Window.h`, `Camera.h`, `Shader_light.h`, `Model.h`, `Mesh_tn.h`, `Create.hpp`, `Load.hpp`.
3. `docs/bitacora-David.md` (regla de registro por hito).
4. Si tocas P09-2: `P09-2-424104230.cpp` completo.

## 6. Reglas invariantes

- No reescribir el motor ni introducir capas nuevas.
- **OpenGL solo en el motor existente** (hoy en la raíz: `Window`, `Shader*`, `Texture`, `Mesh_tn`, `Model`, `Skybox`, `Create`, `Load`) y en el main de prueba. `src/engine` aún no existe; **no se mueve nada en esta fase**. El código nuevo de escena/cámaras no debe llamar `gl*`.
- Todo update recibe `dt`; prohibido `glfwGetTime` dentro de updates. `dt` limitado a 0.05.
- Nada de clases por objeto (`Planeta1`, `Cohete`, `NPC2`): `SceneNode` + funciones/prefabs.
- `Model*` en nodos es prestado; nunca `delete`.
- Un dueño por archivo.
- No cambiar firmas sin actualizar los contratos del `PLAN_MAESTRO.md` y avisar.
- No meter assets binarios nuevos al repo; los assets actuales viven en la PC y están fuera de git.
- No tocar la bóveda de Obsidian.
- Los snippets del plan son indicativos: verifica la API real antes de copiarlos.
- No CMake, no ECS, no PBR, no ImGui, no vcpkg, no física.

## 7. Tarea de hoy

**Objetivo:** cerrar la línea base y, con OK, empezar `SceneNode` + `Scene`.

**Plan de 5 bullets (pendiente de OK):**
1. Cerrar Fase 0: `AGENTS.md`, `docs/bitacora-David.md`, captura en `docs/capturas/`, repo nuevo y retiro de mains de prácticas (hecho; verificar commit/tag).
2. Crear `src/scene/SceneNode.{h,cpp}` y `src/scene/Scene.{h,cpp}` sin OpenGL; agregarlos al `.vcxproj` sin mover el motor existente.
3. Implementar `local()` = traslación → rotación Y/X/Z → escala, y `updateWorld()` DFS (`world = parent->world * local()`).
4. Implementar render recursivo con `Model`/`Shader_light` y un nodo de prueba padre/hijo en P09-2: rotar el hijo sin mover al padre; mover la raíz mueve todo.
5. Compilar Release x64, correr desde la raíz, capturar evidencia y actualizar `Estado actual` + bitácora; commit.

**DoD:**
- [ ] Build Release x64 con 0 errores.
- [ ] `.exe` corre desde `C:\UNAM\CGICH`.
- [ ] Un hijo rota solo; mover la raíz mueve a todos los hijos.
- [ ] `AGENTS.md` y bitácora actualizados.

**No hacer:** sombras, PBR, CMake, ECS, ImGui; pista/avatar/HUD/planetas (son de B/C); refactorizar `Recursos`, `Load.cpp` o includes "por limpieza".

## 8. Cómo verificar

```powershell
# Compilar
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" `
  .\LabCGeIHC.sln /p:Configuration=Release /p:Platform=x64 /m /v:minimal

# Ejecutar desde la raíz (assets relativos)
.\x64\Release\LabCGeIHC.exe
```

- [ ] Compila con **0 errores**.
- [ ] Corre y se ve la ventana `Practica 09: Animacion`.
- [ ] Se ven skybox, texturas y modelos (no todo blanco).
- [ ] La cámara responde (WASD + mouse); `P` pausa.
- [ ] `SceneNode` (cuando exista): rotar el hijo no mueve al padre; mover la raíz mueve todo.

Si algo falla: reporta el **error literal**, una hipótesis y el **parche mínimo**. No reescribas el módulo.

## 9. Trampas conocidas

| # | Trampa | Parche |
|---|---|---|
| 1 | Normales negadas en `Model::LoadMesh` | bandera `flipNormals` por modelo |
| 2 | `LoadNode` aplana la jerarquía de Assimp | construir por partes con `SceneNode` |
| 3 | Skybox: orden de caras y `stbi` flip global | `+X,-X,+Y,-Y,+Z,-Z`; resetear flip |
| 4 | Puede faltar `shaders/skybox.frag` | ya existe; no borrarlo |
| 5 | `Texture` copiable con `GLuint` | regla de 3 o `unique_ptr` |
| 6 | `~Model` vacío y `new Shader` sin liberar | `ClearModel()` al salir |
| 7 | `MAX_POINT_LIGHTS` duplicado | editar `CommonValues.h` y `shader_light.frag` |
| 8 | GL 4.3 pedido sin necesidad | el plan pide 3.3; no se cambió aún |
| 9 | `main` de 560 líneas | `App` + `Experience` (futuro) |
| 10 | Rutas relativas al CWD | ejecutar desde la raíz |
| 11 | `glm::mix` no existe para `mat4` | interpolar posición/target |
| 12 | Debug CRT + Assimp release | compilar Release x64 |

## 10. Formato de respuesta

1. Si el cambio supera ~30 líneas: **plan de ≤5 bullets antes de tocar código** y esperar OK.
2. Al terminar: **archivos tocados**, **cómo compilar/probar**, **qué quedó pendiente**.
3. Si algo falla: error literal + hipótesis + parche mínimo.
4. No resumir el proyecto en cada respuesta; ve al grano.
5. No decir "listo" sin haber compilado y ejecutado.

## 11. Prohibido

- Añadir dependencias, CMake, vcpkg, ImGui, ECS, PBR o física.
- Refactorizar `Recursos`, `Load.cpp`, `Mesh` o los includes "por limpieza".
- Cambios masivos de nombres o estilo.
- Tocar `docs/` (salvo `docs/bitacora-David.md`) o la bóveda de Obsidian.
- Meter assets binarios nuevos al repo.
- Inventar rutas, versiones, APIs o resultados de compilación.
- Modificar `AGENTS.md` para que "parezca" completo.

## 12. Al cerrar cada sesión

1. Actualizar `## 3. Estado actual`: compila, último commit, funciona, stub, siguiente tarea.
2. Añadir el hito o la entrada del día en `docs/bitacora-David.md` con fecha, hora, descripción, evidencia y horas reales.
3. Commit y push (cuando exista remoto).
4. Si se cambió una firma o una regla, actualizar `PLAN_MAESTRO.md` y avisar a B/C.
