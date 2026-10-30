# CGICH — Proyecto final 2027-1

Entrega: **7-nov-2026 antes de las 12:00 CDMX**. Lee [PLAN_MAESTRO.md](PLAN_MAESTRO.md): contratos §5, hitos §6, controles §8 y propiedad §10/§16. Los PDFs de docs/ son las referencias originales. El ejecutable actual es la base de prácticas; el contenido final está pendiente.

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
git switch feat/b-mundo
# Venancio: git switch feat/c-avatar
```

Obtén de David `cgich-dependencias-2026-10-30-base.zip` y su `.sha256`. La ubicación compartida está pendiente de acordar; el paquete se prepara localmente en artefactos/. No está en GitHub. Son librerías y assets heredados de arranque, no contenido final autorizado.

Comprueba hash y extrae directamente en raíz; sustituye C:\ruta por la ubicación real:

```powershell
Get-FileHash 'C:\ruta\cgich-dependencias-2026-10-30-base.zip' -Algorithm SHA256
Expand-Archive -LiteralPath 'C:\ruta\cgich-dependencias-2026-10-30-base.zip' -DestinationPath .
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\Verify-Dependencies.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\Build.ps1
Copy-Item -LiteralPath .\glew32.dll,.\glfw3.dll,.\assimp-vc143-mt.dll -Destination .\x64\Release\
.\x64\Release\LabCGeIHC.exe
```

ExecutionPolicy Bypass solo aplica a ese proceso. Para recompilar todo añade -Rebuild. Mantén raíz como directorio de trabajo: assets usan rutas relativas. No uses los .bat antiguos de build/, que apuntan a fuentes anteriores al movimiento a src/. Cada integrante ejecuta y registra captura, errores y horas reales: compilar no valida visualmente.

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

David genera recursos con `scripts/Package-Dependencies.ps1 -Version <version-nueva>`; no sobreescribe versiones. Compartir ZIP/hash por Drive/USB y registrar aquí la ubicación acordada. No subir assets/DLLs/ZIPs a Git. El manifiesto identifica el paquete; al editar recursos se publica otra versión.

La preparación del 30-oct conserva el main remoto inicial y la base local por merge. La rama remota david no se modifica. main/develop/feat/* quedan preparadas localmente. Ver [publicación](docs/PREPARAR_PUSH.md).
