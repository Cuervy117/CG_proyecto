# CGICH — Proyecto final 2027-1

Entrega: **7-nov-2026 antes de las 12:00 CDMX**. Lee [PLAN_MAESTRO.md](PLAN_MAESTRO.md): contratos §5, hitos §6, controles §8 y propiedad §10/§16. Los PDFs de docs/ son las referencias originales. En `develop` se integran el mundo, las cámaras y el avatar articulado.

| Integrante | Rol | Rama |
|---|---|---|
| David | A: motor, escena, cámaras, luces/build | feat/a-motor |
| Jonhatan | B: mundo, recursos/animación infraestructura | feat/b-mundo |
| Venancio | C: avatar, criatura, entrada/experiencias/main | feat/c-avatar |

## Arranque Windows

Requiere VS2022 o Build Tools, C++ de escritorio, v143, Windows SDK y Git. Tras publicar las ramas preparadas:

```powershell
git clone https://github.com/Cuervy117/CG_proyecto.git
cd CG_proyecto
git switch develop
```

Obtén de David `cgich-dependencias-2026-10-30-base.zip` y su `.sha256`. La ubicación compartida está pendiente de acordar; el paquete se prepara localmente en artefactos/. No está en GitHub. Son librerías y assets heredados de arranque, no contenido final autorizado.

Comprueba hash y extrae directamente en raíz; sustituye C:\ruta por la ubicación real:

```powershell
Get-FileHash 'C:\ruta\cgich-dependencias-2026-10-30-base.zip' -Algorithm SHA256
Expand-Archive -LiteralPath 'C:\ruta\cgich-dependencias-2026-10-30-base.zip' -DestinationPath .
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\Verify-Dependencies.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\Verify-Project-Assets.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\Build.ps1
Copy-Item -LiteralPath .\glew32.dll,.\glfw3.dll,.\assimp-vc143-mt.dll -Destination .\x64\Release\
.\x64\Release\LabCGeIHC.exe
```

ExecutionPolicy Bypass solo aplica a ese proceso. Para recompilar todo añade -Rebuild. Mantén raíz como directorio de trabajo: assets usan rutas relativas. No uses los .bat antiguos de build/, que apuntan a fuentes anteriores al movimiento a src/. Cada integrante ejecuta y registra captura, errores y horas reales: compilar no valida visualmente.

## Probar el mundo integrado

Ejecuta desde la raíz con los modelos y texturas del paquete actualizado superpuestos en `Models/` y `Textures/`. La escena comienza en la base espacial con Rek’Sai a pie.

| Controles | Acción |
|---|---|
| F1 / F2 / F3 / F4 | Planeta a pie, persecución de carrera, vuelo, observación |
| W / S y A / D en carrera (F2) | Acelerar o frenar y cambiar de carril con miNave. Las otras dos naves repiten la vuelta por roca, selva y océano. |
| Ratón en F1 / F2 / F3 | En F1 gira alrededor del avatar a altura constante. En F2 orbita respecto al rumbo de miNave. En F3 sigue la mirada de vuelo. |
| Tab / rueda | En F4, avanzar al siguiente objetivo disponible y ajustar el zoom |
| F5 / F6 / F7 / F8 | Accesos directos a selva, roca, océano y espacio |
| WASD / ratón | Movimiento y orientación |
| J / E | Embarcar o desembarcar junto a la nave / interactuar con el tránsito entre zonas |
| 1 / 2 / 3 | Cohete de lanzamiento / puerta de cápsula / paneles del satélite |
| 4 / 5 | Vuelo del Fighter_02 desde el hangar / puerta del hangar |
| 6 / 7 (dentro de Rek’Sai, F6) | Reproducir los keyframes del cohete de Jona / reiniciar. Está a la derecha del punto de llegada. |
| F / L / K | Linterna / faro de nave / foco de base |
| C / F9 | Alternar cámara libre / liberar o capturar el cursor |

**Recorrido R4.** Pulsa F4 para observar el cohete de Shinobi y 1 para lanzarlo. Tab recorre el hangar espacial con Fighter_02, el planeta rocoso, el selvático, el satélite y el cohete de la base antes de volver al lanzamiento. Enfoca el hangar y pulsa 4 para la salida y el regreso, o 5 para abrir y cerrar su puerta. La cámara muestra la zona del objetivo y el jugador conserva su posición durante la observación. F1, F2 y F3 retoman sus recorridos habituales.

## Trabajo diario

```powershell
git fetch origin
git merge origin/develop
# Implementar y verificar el hito; agregar solo archivos propios.
# Revisar git diff --cached antes del commit.
git commit -m "feat: describir el hito terminado"
git push
```

Si falta upstream: `git push -u origin feat/b-mundo` (o tu rama). A coordina merges a develop. Sin force ni borrado de ramas. B sí puede editar Load.*; C integra un único main.

## Recursos y publicación

El planeta Vacío usa un cubemap propio y una textura mineral en `Textures/Vacio/`. Los siete PNG viajan en el paquete de recursos compartido por Drive y el empaquetador los incluye ya generados en el juego distribuido. El cubemap de Cupertin diurno sigue asignado a Shinobi.

### Candidato ejecutable Windows x64

Desde un checkout limpio con los assets del proyecto, VS 2022/v143, su redistribuible x64 y Python 3, ejecuta `python scripts/Package-Release.py v0.2.0-rc.1`. El script compila Release y genera el ZIP y su SHA-256 en `artefactos/`. Para otra versión, cambia el nombre del candidato.

En el equipo de prueba (Windows x64 con OpenGL 3.3), compara el hash del ZIP con su archivo `.sha256`, extrae todo y ejecuta desde la carpeta extraída:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\Verify-Release.ps1
.\Jugar.cmd --probar-release
.\Jugar.cmd
```

La prueba automática dibuja las cuatro zonas y comprueba carrera, lanzamiento y hangar. Finaliza con código 0 si pasa. El manifiesto `release.json` identifica el commit y los archivos de esa distribución.

David genera recursos con `scripts/Package-Dependencies.ps1 -Version <version-nueva>`; no sobreescribe versiones. Compartir ZIP/hash por Drive/USB y registrar aquí la ubicación acordada. No subir assets/DLLs/ZIPs a Git. El manifiesto identifica el paquete; al editar recursos se publica otra versión. Las ocho texturas críticas nuevas se validan con `manifest-project-assets.json` y `scripts/Verify-Project-Assets.ps1`; su carpeta fuente es [Textures en Drive](https://drive.google.com/drive/folders/1794f4lffbV-lYnVEKhLvxA8gr8vDi-iH).

La preparación del 30-oct conserva el main remoto inicial y la base local por merge. La rama remota david no se modifica. main/develop/feat/* quedan preparadas localmente. Ver [publicación](docs/PREPARAR_PUSH.md).
