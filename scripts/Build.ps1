param([switch]$Rebuild)
$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path $PSScriptRoot -Parent
foreach ($entry in @('include','lib','glm','Models','Textures','glew32.dll','glfw3.dll','assimp-vc143-mt.dll')) {
    if (-not (Test-Path -LiteralPath (Join-Path $repoRoot $entry))) { throw "Falta $entry. Extrae el paquete de dependencias en la raiz (README)." }
}
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if (-not (Test-Path -LiteralPath $vswhere)) { throw 'Instala VS2022 con C++ de escritorio.' }
$msbuild = & $vswhere -latest -products '*' -requires Microsoft.Component.MSBuild -find 'MSBuild\**\Bin\MSBuild.exe' | Select-Object -First 1
if (-not $msbuild) { throw 'No se encontro MSBuild.' }
$target = if ($Rebuild) { 'Rebuild' } else { 'Build' }
Push-Location $repoRoot
try {
    & $msbuild '.\LabCGeIHC.sln' "/t:$target" '/p:Configuration=Release' '/p:Platform=x64' '/nologo' '/verbosity:minimal'
    if ($LASTEXITCODE -ne 0) { throw "MSBuild fallo: $LASTEXITCODE" }
    Write-Host 'Build completado. Copia las DLLs junto al exe y ejecuta desde raiz: .\x64\Release\LabCGeIHC.exe'
} finally { Pop-Location }
