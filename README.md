# CGICH — Proyecto final 2027-1

Juego de exploración y carrera espacial en Windows x64. La versión integrada está en `main`. El repositorio contiene código y herramientas; los modelos, texturas, bibliotecas y DLL de ejecución se distribuyen por separado.

## Jugar sin compilar

Obtener `CGICH-v0.4.0-rc.2-win64.zip` y `CGICH-v0.4.0-rc.2-win64.zip.sha256`. El ZIP probado corresponde al commit `ef80c32` y su SHA-256 es `5a6d8bd9c0034b992488968de4c30f59e41cadd3c50165beeaddb15ca3e62ee1`. No está almacenado en GitHub. Requiere Windows x64 y un controlador compatible con OpenGL 3.3.

1. Antes de extraer, puedes comprobar el ZIP desde la carpeta que lo contiene con `Get-FileHash .\CGICH-v0.4.0-rc.2-win64.zip -Algorithm SHA256` y comparar el resultado con el hash anterior.
2. Extrae el ZIP completo en una carpeta propia. Conserva `Models/`, `Textures/`, `shaders/`, las DLL y el EXE juntos.
3. Abre PowerShell en la carpeta extraída y ejecuta:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\Verify-Release.ps1
.\Jugar.cmd --probar-release
.\Jugar.cmd
```

`Verify-Release.ps1` comprueba los archivos contra `release.json`. La prueba debe terminar con `[Release] OK`; luego `Jugar.cmd` inicia la partida. No ejecutes el EXE desde dentro del ZIP ni desde otro directorio: usa rutas relativas a los recursos.

## Controles

| Teclas | Acción |
|---|---|
| F1 / F2 / F3 / F4 | Caminar por planetas / carrera / vuelo / observación |
| F5 / F6 / F7 / F8 | Ir a selva / Rek’Sai (Vacío) / océano / espacio |
| WASD y ratón | Movimiento y cámara; en carrera W/S aceleran y frenan, A/D cambian de carril |
| Tab y rueda | En F4, cambiar objetivo y ajustar distancia |
| J / E | Abordar o bajar de la nave / interactuar o cambiar de zona |
| 1 / 2 / 3 | Lanzar cohete / puerta de cápsula / paneles del satélite |
| 4 / 5 | Salida y regreso de Fighter_02 / puerta del hangar |
| 6 / 7 | En Rek’Sai: reproducir / reiniciar el cohete de Jona por keyframes |
| 8 / 9 | En Rek’Sai: reproducir / reiniciar el cohete de David por keyframes |
| F / L / K | Linterna / faro de la nave / foco de la base |
| C / F9 / Esc | Cámara libre / liberar cursor / salir |

En F4, Tab lleva al hangar: allí `4` inicia el vuelo del Fighter y `5` abre o cierra la puerta cuando está estacionado. En Rek’Sai (F6), el cohete de Jona está a la derecha del punto de llegada y el de David en el claro de (-47,7), al oeste. La música cambia con la zona: espacio, Rek'Sai, Shinobi y océano; durante la carrera suena la pista de carrera y en F4 prevalece la zona observada. La música se escucha uniforme, sin atenuación por distancia, y cambia a la pista de la zona al usar F5–F8, incluso después de una carrera. La música usa volumen 32.5; el motor del jugador usa e_motor_nave_main y los rivales y el Fighter usan e_motor_nave. Los ambientes por zona usan a_pajaros en la selva, a_insectos en la roca y a_olas en el océano; cuando el avatar está a pie, a_reksai suena cada 15 s. Los efectos incluyen el motor de la nave del jugador, el motor del Fighter, la puerta del hangar, el portal y la llegada a Rek’Sai.

## Compilar desde `main`

Necesitas Visual Studio 2022 o Build Tools con C++ de escritorio, toolset v143 y Windows SDK, además de Python 3. Clona el repositorio y conserva `main`:

```powershell
git clone https://github.com/Cuervy117/CG_proyecto.git
cd CG_proyecto
git switch main
```

Extrae primero el paquete de dependencias compartido (`cgich-dependencias-2026-10-30-base.zip`) en la raíz y verifica su manifiesto desde allí:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\Verify-Dependencies.ps1
```

Después superpone `Models/` y `Textures/` del ZIP Release actual en la raíz; el paquete base no contiene los modelos añadidos después. Copia también los shaders del Release si hace falta. Compila y ejecuta desde la raíz:

```powershell
# Audio: el paquete de dependencias debe incluir include/SFML, lib/sfml-audio.lib y lib/sfml-system.lib. Copia también los WAV mono de audio/ al paquete.
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\Verify-Project-Assets.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\Build.ps1
Copy-Item .\glew32.dll,.\glfw3.dll,.\assimp-vc143-mt.dll,.\sfml-audio-3.dll,.\sfml-system-3.dll .\x64\Release\
.\x64\Release\LabCGeIHC.exe --probar-audio
.\x64\Release\LabCGeIHC.exe --probar-musica
.\x64\Release\LabCGeIHC.exe --probar-ambiente
.\x64\Release\LabCGeIHC.exe --probar-efectos
.\x64\Release\LabCGeIHC.exe --probar-release
```

La verificación de dependencias se hace **antes** de superponer modelos y texturas nuevos, porque el manifiesto del paquete base describe la versión original. Si modificas los recursos, vuelve a generar un ZIP Release con `python scripts/Package-Release.py <version-nueva>` desde un checkout limpio. El empaquetador incluye los modelos, texturas, keyframes, shaders y DLL necesarios y crea un archivo `.sha256` junto al ZIP.

Los [manuales de usuario y técnico](docs/manuales/) amplían los recorridos y la implementación. Los [lineamientos](<docs/Lineamientos del proyecto final laboratorio de Computación Gráfica e IHC grupos 2 y 3 2027-1 - Lineamientos del proyecto final laboratorio de Computación Gráfica e IHC grupos 2 y 3 2027-1.pdf>) son la referencia de la rúbrica.
