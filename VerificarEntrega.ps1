$ErrorActionPreference = 'Stop'
$raiz = $PSScriptRoot
$recursos = Get-ChildItem (Join-Path $raiz Models),(Join-Path $raiz Textures) -Recurse -File
$bytes = ($recursos | Measure-Object Length -Sum).Sum
if ($bytes -ge 100000000) { throw "Models + Textures exceden el limite: $bytes bytes" }
$archivos = Get-ChildItem $raiz -Recurse -File -Force | Where-Object { $_.FullName -notlike '*\.git\*' }
if ($archivos | Where-Object Length -ge 100000000) { throw 'Existe archivo individual de 100 MB o mas' }
if ($archivos | Where-Object Extension -eq '.zip') { throw 'Existe un ZIP en el proyecto' }
& (Join-Path $raiz scripts/Verify-Project-Assets.ps1)
Push-Location $raiz
try {
    & (Join-Path $raiz LabCGeIHC.exe) --probar-release
    if ($LASTEXITCODE -ne 0) { throw "Release fallo: $LASTEXITCODE" }
    & (Join-Path $raiz LabCGeIHC.exe) --probar-animaciones
    if ($LASTEXITCODE -ne 0) { throw "Animaciones fallo: $LASTEXITCODE" }
} finally { Pop-Location }
Write-Output "Entrega local verificada: $bytes bytes de modelos y texturas. No acredita otra PC."
