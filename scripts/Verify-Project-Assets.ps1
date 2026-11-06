$ErrorActionPreference = 'Stop'
$repoRoot = [System.IO.Path]::GetFullPath((Split-Path $PSScriptRoot -Parent))
$manifestPath = Join-Path $repoRoot 'manifest-project-assets.json'

if (-not (Test-Path -LiteralPath $manifestPath -PathType Leaf)) {
    throw 'No se encontro manifest-project-assets.json en la raiz del proyecto.'
}

$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
$mismatches = @()
foreach ($entry in $manifest.files) {
    $target = [System.IO.Path]::GetFullPath((Join-Path $repoRoot $entry.path))
    if (-not $target.StartsWith($repoRoot + [System.IO.Path]::DirectorySeparatorChar, [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Ruta fuera del checkout en manifiesto: $($entry.path)"
    }
    if (-not (Test-Path -LiteralPath $target -PathType Leaf)) {
        $mismatches += "Falta: $($entry.path)"
        continue
    }
    $actualSize = (Get-Item -LiteralPath $target).Length
    if ($actualSize -ne [long]$entry.size) {
        $mismatches += "Tamano distinto: $($entry.path) (esperado $($entry.size), actual $actualSize)"
        continue
    }
    $actualHash = (Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash
    if ($actualHash -ne $entry.sha256) {
        $mismatches += "Hash distinto: $($entry.path)"
    }
}

if ($mismatches.Count) { throw ($mismatches -join "`n") }
Write-Output "Assets $($manifest.version) verificados: $($manifest.files.Count) archivos."
