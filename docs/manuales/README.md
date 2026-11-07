# Manuales del proyecto final

Fuentes editables en LaTeX: `usuario.tex` y `tecnico.tex`. Las imágenes se referencian desde `../capturas/`; no se duplican.

Desde esta carpeta, ejecutar `powershell -NoProfile -ExecutionPolicy Bypass -File .\compilar.ps1`. El script genera los PDF en `pdf/` y los archivos auxiliares en `build/`. Requiere XeLaTeX (MiKTeX o TeX Live).

Los manuales describen el candidato Release `v0.4.0-rc.2`, versión integrada actual. Antes de entregar otra versión, actualizar esa identificación, revisar los controles y sustituir capturas que hayan cambiado. Las conclusiones individuales del manual técnico deben ser ratificadas por cada integrante.
