param([string]$Root = $PSScriptRoot)
$ErrorActionPreference = 'Stop'
$rootPath = (Resolve-Path -LiteralPath $Root).Path
$manifest = Get-Content -Raw -LiteralPath (Join-Path $rootPath 'release.json') | ConvertFrom-Json
if (-not $manifest.commit -or -not $manifest.files -or 'LabCGeIHC.exe' -notin $manifest.files.path) {
    throw 'Manifiesto Release incompleto.'
}
$errorsFound = foreach ($entry in $manifest.files) {
    $path = [IO.Path]::GetFullPath((Join-Path $rootPath $entry.path))
    if (-not $path.StartsWith($rootPath + '\', [StringComparison]::OrdinalIgnoreCase)) {
        throw "Ruta fuera del paquete: $($entry.path)"
    }
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { "Falta: $($entry.path)"; continue }
    if ((Get-Item -LiteralPath $path).Length -ne $entry.bytes -or
        (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash -ne $entry.sha256) {
        "Contenido distinto: $($entry.path)"
    }
}
if ($errorsFound) { throw ($errorsFound -join [Environment]::NewLine) }
"Release $($manifest.version) verificado: $($manifest.files.Count) archivos, commit $($manifest.commit)."
