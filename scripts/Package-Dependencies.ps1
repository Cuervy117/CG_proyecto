param([string]$Version = '2026-10-30-base')
$ErrorActionPreference = 'Stop'
if ($Version -notmatch '^[a-zA-Z0-9._-]+$') { throw 'Version invalida.' }
$repoRoot = Split-Path $PSScriptRoot -Parent
$outputDir = Join-Path $repoRoot 'artefactos'
$zipPath = Join-Path $outputDir "cgich-dependencias-$Version.zip"
if (Test-Path -LiteralPath $zipPath) { throw 'La version ya existe. Elige una nueva.' }
$files = @()
foreach ($entry in @('Models','Textures','include','lib','glm','glew32.dll','glfw3.dll','assimp-vc143-mt.dll')) {
    $sourcePath = Join-Path $repoRoot $entry
    if (-not (Test-Path -LiteralPath $sourcePath)) { throw "Falta $sourcePath" }
    $files += Get-ChildItem -LiteralPath $sourcePath -Recurse -File
}
$files = $files | Sort-Object FullName
$records = @($files | ForEach-Object {
    [ordered]@{ path = $_.FullName.Substring($repoRoot.Length + 1).Replace('\','/'); bytes = $_.Length; sha256 = (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash }
})
$manifest = [ordered]@{ version = $Version; purpose = 'Base heredada para desarrollo; no es contenido final aprobado'; files = $records }
New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
Add-Type -AssemblyName System.IO.Compression
Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive = [System.IO.Compression.ZipFile]::Open($zipPath, [System.IO.Compression.ZipArchiveMode]::Create)
try {
    foreach ($file in $files) {
        $relative = $file.FullName.Substring($repoRoot.Length + 1).Replace('\','/')
        [System.IO.Compression.ZipFileExtensions]::CreateEntryFromFile($archive, $file.FullName, $relative, [System.IO.Compression.CompressionLevel]::Optimal) | Out-Null
    }
    $entry = $archive.CreateEntry('manifest-dependencies.json')
    $writer = New-Object System.IO.StreamWriter($entry.Open())
    try { $writer.Write(($manifest | ConvertTo-Json -Depth 5)) } finally { $writer.Dispose() }
} finally { $archive.Dispose() }
$manifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $outputDir "manifest-$Version.json") -Encoding utf8
$hash = (Get-FileHash -LiteralPath $zipPath -Algorithm SHA256).Hash
"$hash  $([System.IO.Path]::GetFileName($zipPath))" | Set-Content -LiteralPath "$zipPath.sha256" -Encoding ascii
Write-Output "Paquete: $zipPath"
Write-Output "Archivos: $($records.Count); SHA256: $hash"
