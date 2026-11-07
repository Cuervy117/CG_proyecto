$ErrorActionPreference = 'Stop'
$base = Split-Path -Parent $MyInvocation.MyCommand.Path
$build = Join-Path $base 'build'
$pdf = Join-Path $base 'pdf'
New-Item -ItemType Directory -Force -Path $build, $pdf | Out-Null
Push-Location $base
try {
foreach ($name in @('usuario', 'tecnico')) {
    & xelatex -interaction=nonstopmode -halt-on-error -file-line-error "-output-directory=$build" (Join-Path $base "$name.tex")
    if ($LASTEXITCODE -ne 0) { throw "Fallo XeLaTeX: $name" }
    & xelatex -interaction=nonstopmode -halt-on-error -file-line-error "-output-directory=$build" (Join-Path $base "$name.tex")
    if ($LASTEXITCODE -ne 0) { throw "Fallo segunda pasada: $name" }
    Copy-Item -LiteralPath (Join-Path $build "$name.pdf") -Destination (Join-Path $pdf "$name.pdf") -Force
}
} finally { Pop-Location }
Write-Output "Manuales PDF: $pdf"
