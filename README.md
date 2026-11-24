# 320324187 - GPO06 - Computacion Grafica (Teoria)

## Entrega al profesor

La entrega completa esta en esta rama de GitHub. En la tarea de Classroom se
adjunta la liga de la rama. Los documentos tambien estan en el repositorio.

- Release: [LabCGeIHC.exe](LabCGeIHC.exe), en la raiz.
- Inicio: [Jugar.cmd](Jugar.cmd). Conservar todas las carpetas y DLL.
- Documentacion individual: [ES](docs/teoria/ES/) y [EN](docs/teoria/EN/).
- [Propuesta historica anonimizada](docs/teoria/ANTECEDENTES/320324187_PropuestaHistorica_GPO06.pdf).
- [Requerimientos originales LAB](docs/teoria/ANTECEDENTES/320324187_RequerimientosLaboratorio_GPO06.pdf).

Los antecedentes LAB se adjuntan por la rubrica de Teoria. No hay acuse que
acredite el envio de la propuesta antes del 20 de octubre.
Para ejecutar el Release no hace falta Visual Studio. Descargar solamente
el EXE desde GitHub no incluye sus recursos: obtener la rama completa.

```powershell
git clone --single-branch --branch entrega/320324187-teoria https://github.com/Cuervy117/CG_proyecto.git
cd CG_proyecto
.\Jugar.cmd
```

Entorno espacial Windows x64: exploracion, carrera, vuelo y observacion.
Documentacion bilingue en docs/teoria/. Base 41ba92c; preparacion en
entrega/320324187-teoria.

## Ejecutar

Requiere Windows x64 y controlador con OpenGL 4.3 Core. Abre Jugar.cmd
o LabCGeIHC.exe de la raiz. Conserva Models, Textures, shaders y las DLL.
El EXE localiza recursos desde su propia ubicacion. No se requieren ZIPs.

## Compilar

Visual Studio/Build Tools con C++ de escritorio, toolset v143 y Windows SDK.
include, lib y glm contienen las dependencias locales; Models y Textures
contienen los recursos optimizados.

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/Build.ps1
Copy-Item *.dll x64/Release/
Copy-Item x64/Release/LabCGeIHC.exe LabCGeIHC.exe
.\LabCGeIHC.exe --probar-release
.\LabCGeIHC.exe --probar-animaciones
```

La primera prueba verifica estados y dibuja nueve pasos. La segunda usa el
callback de teclado, 3360 frames simulados a 60 Hz, comprueba cuadros clave,
paneles, puerta y marcha, y captura en evidencias-release/. Solo ese modo fija
un encuadre del cohete; el juego conserva las camaras interactivas.
VerificarEntrega.ps1 agrega pesos, ausencia de ZIPs y hashes de recursos.
La prueba independiente en otra computadora sigue pendiente.

## Controles

F1-F4: planeta, carrera, vuelo, observacion. F5-F8: selva, roca, oceano, espacio.
WASD y raton: movimiento/camara. Q/E: descenso/ascenso en vuelo; E contextual.
J: abordar/bajar. C: camara libre. Tab y rueda en F4: objetivo y distancia.
1: cohete procedural; 2: puerta; 3: paneles; 4: vuelo Fighter; 5: hangar.
6/7 en roca: cohete por cuadros clave/reinicio. F/L/K: luces. F9: cursor.
Esc: salir.

## Alcance

Release compilado en Windows con warnings de conversiones registrados.
Indices OBJ y valores finitos comprobados; no se certifican mallas cerradas
ni ausencia universal de intersecciones. Las repeticiones de animacion se agrupan.
Recursos optimizados; originales intactos en el proyecto de origen.

Historial y atribuciones/licencias conservados. Algunos identificadores internos
son creditos de colaboradores; los documentos finales usan solo 320324187 y
GPO06 para identificacion. No se presume una licencia comercial de recursos.
El precio eventual de 120 MXN es una intencion, no una venta.
