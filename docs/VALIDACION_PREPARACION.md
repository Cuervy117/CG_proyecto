# Validación de preparación — 30-oct-2026

- Rebuild Release x64 mediante `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/Build.ps1 -Rebuild`: terminó con código 0. Ejecutable en `x64/Release/LabCGeIHC.exe`.
- Persisten warnings heredados de conversiones, formatos printf, comprobación de índice en Model y retorno ausente en Window::Initialise. No se modificó código de motor para esta preparación.
- No se ejecutó una nueva validación visual ni una prueba en otra PC. Las capturas previas son evidencia histórica; B/C deben registrar su propia ejecución.
- `Package-Dependencies.ps1` ejecutado con Windows PowerShell; ZIP de **753 archivos de recursos/dependencias + manifiesto**, tamaño **151059635 bytes** (~144 MiB).
- Se leyeron las 753 entradas del ZIP y se comparó tamaño/SHA-256 con el manifiesto. `Verify-Dependencies.ps1` verificó además los archivos locales correctamente.
- Paquete local ignorado: `artefactos/cgich-dependencias-2026-10-30-base.zip`.
- SHA-256 del ZIP: `E813BBB5A8F4F20FF2126D9E5A769D2A260C1E37C9817206ADAF105BB1814097`.
- El paquete contiene recursos heredados para reproducir la base, no sustituye la selección de contenido nuevo exigida por el profesor.
- La publicación Git y la distribución efectiva del ZIP son pasos distintos. Preparar archivos localmente no acredita que los compañeros los hayan recibido.
