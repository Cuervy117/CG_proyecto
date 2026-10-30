$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path $PSScriptRoot -Parent
$manifestPath = Join-Path $repoRoot 'manifest-dependencies.json'
if (-not (Test-Path -LiteralPath $manifestPath)) { throw 'Extrae primero el ZIP de dependencias en la raiz.' }
$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
$mismatches = @()
foreach ($entry in $manifest.files) {
    $target = [System.IO.Path]::GetFullPath((Join-Path $repoRoot $entry.path))
    if (-not $target.StartsWith($repoRoot + [System.IO.Path]::DirectorySeparatorChar, [System.StringComparison]::OrdinalIgnoreCase)) { throw 'Ruta fuera del checkout en manifiesto.' }
    if (-not (Test-Path -LiteralPath $target -PathType Leaf)) { $mismatches += "Falta: $($entry.path)"; continue }
    if ((Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash -ne $entry.sha256) { $mismatches += "Difiere: $($entry.path)" }
}
if ($mismatches.Count) { throw ($mismatches -join "`n") }
Write-Output "Paquete $($manifest.version) verificado: $($manifest.files.Count) archivos."
