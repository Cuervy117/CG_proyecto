"""Genera el ZIP ejecutable de Windows a partir del checkout y sus assets locales."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import tempfile
from zipfile import ZipFile, ZipInfo, ZIP_DEFLATED

ROOT = Path(__file__).resolve().parents[1]
MODELS = """
arbol/arbol asteroide/asteroide asteroide/Rocky_Asteroid_6 asteroide/Stylized_Planets
capsula/capsulaCuerpo capsula/capsulaPuerta Fighter_03_Obj/Fighter_03 miNave/miNave
npcRoca/Cerbero npcRoca/escorpion npcRoca/monstruoNegro npcRoca/serpienteAlas
npcSelva/caballoDelfin npcSelva/calamardoGuapo npcSelva/naruto npcSelva/pulpo
npcSelva/tigreFuego npcSelva/zorroBlanco pista/pista planetaOceano/planetaOceano
planetaRoca/planetaRoca planetaSelva/planetaSelvatico portal/asteroide reksai/reksai rocket/rocket
""".split()
DIRECT_TEXTURES = """
arbolTexture asteroideTexture baseTexture fuego hud_reloj hud_digitos hud_iconos humo
pistaTexture planetaOceanoTexture planetaRocaTexture planetaSelvaTexture plain reksai sueloRocoso sueloSelva
""".split()


def add_model(files, name):
    obj = Path("Models", name + ".obj")
    files[obj.as_posix()] = ROOT / obj
    # Los exportadores del proyecto declaran mtllib antes de los vértices.
    with (ROOT / obj).open(encoding="utf-8-sig", errors="replace") as stream:
        for line in stream:
            if line.startswith("v "):
                return
            if not line.startswith("mtllib "):
                continue
            rel = obj.parent / line[7:].strip()
            source = ROOT / rel
            if not source.is_file():
                if name == "arbol/arbol":
                    source = ROOT / obj.with_suffix(".mtl")
                elif name in ("asteroide/Rocky_Asteroid_6", "asteroide/Stylized_Planets"):
                    # Estas dos exportaciones vienen sin MTL. Assimp ya usa material neutro.
                    return
            material = source.read_text(encoding="utf-8-sig", errors="strict")
            portable = []
            for row in material.splitlines():
                parts = row.strip().split(None, 1)
                if len(parts) == 2 and parts[0].lower() == "map_kd":
                    filename = Path(parts[1].replace("\\", "/")).name
                    candidates = [Path("Textures") / filename, rel.parent / filename]
                    tex = next((p for p in candidates if (ROOT / p).is_file()), None)
                    if tex is None:
                        raise FileNotFoundError(f"{source}: {filename}")
                    files[tex.as_posix()] = ROOT / tex
                    row = "map_Kd " + filename
                elif parts and (parts[0].lower().startswith("map_") or parts[0].lower() == "bump"):
                    continue  # El shader del juego sólo consume el mapa difuso.
                portable.append(row)
            files[rel.as_posix()] = ("\n".join(portable) + "\n").encode()


def runtime_files():
    files = {}
    for model in MODELS:
        add_model(files, model)
    for part in ("base", "panel_der", "panel_izq", "aspa_g"):
        path = f"Models/satelite/satelite_{part}.fbx"
        files[path] = ROOT / path
    for name in DIRECT_TEXTURES:
        path = f"Textures/{name}.png"
        files[path] = ROOT / path
    water = "Textures/aguaOceanoTexture.png"
    if (ROOT / water).is_file():
        files[water] = ROOT / water
    for side in ("ft", "bk", "dn", "up", "rt", "lf"):
        for path in (f"Textures/spacenebulaskybox/nebula{side}.png",
                     f"Textures/Skybox/cupertin-lake_{side}.tga",
                     f"Textures/Skybox/cupertin-lake-night_{side}.tga"):
            files[path] = ROOT / path
    for shader in ("hud", "shader_light", "skybox"):
        for ext in ("vert", "frag"):
            path = f"shaders/{shader}.{ext}"
            files[path] = ROOT / path
    files["LabCGeIHC.exe"] = ROOT / "x64/Release/LabCGeIHC.exe"
    for dll in ("glew32.dll", "assimp-vc143-mt.dll"):
        files[dll] = ROOT / dll
    vswhere = Path(os.environ["ProgramFiles(x86)"], "Microsoft Visual Studio/Installer/vswhere.exe")
    vs = subprocess.check_output([vswhere, "-latest", "-products", "*", "-requires",
                                  "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
                                  "-property", "installationPath"], text=True).strip()
    redists = list((Path(vs) / "VC/Redist/MSVC").glob("*/x64/Microsoft.VC143.CRT"))
    if not redists:
        raise FileNotFoundError("Se necesita el redistribuible x64 v143 de Visual Studio.")
    crt = max(redists, key=lambda d: tuple(map(int, d.parents[1].name.split("."))))
    for name in ("msvcp140.dll", "vcruntime140.dll", "vcruntime140_1.dll"):
        files[name] = crt / name
    files["Verify-Release.ps1"] = ROOT / "scripts/Verify-Release.ps1"
    files["LEEME.txt"] = (
        "CGICH - candidato Windows x64. Se necesita un controlador con OpenGL 3.3.\r\n"
        "Extrae el ZIP completo antes de jugar. Desde esta carpeta ejecuta:\r\n"
        "  powershell -NoProfile -ExecutionPolicy Bypass -File .\\Verify-Release.ps1\r\n"
        "  Jugar.cmd --probar-release\r\n"
        "La prueba recorre las zonas y devuelve 0 al terminar correctamente.\r\n"
        "Para jugar, abre Jugar.cmd. F1-F4 cambian de recorrido. Esc sale.\r\n"
        "Para reportar la prueba externa, indica el commit de release.json, Windows, GPU y resultado.\r\n"
    ).encode("utf-8")
    files["Jugar.cmd"] = b'@echo off\r\ncd /d "%~dp0"\r\nLabCGeIHC.exe %*\r\nexit /b %errorlevel%\r\n'
    return files


def put(archive, name, data):
    entry = ZipInfo(name, (2026, 1, 1, 0, 0, 0))
    entry.compress_type = ZIP_DEFLATED
    archive.writestr(entry, data)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("version")
    args = parser.parse_args()
    if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9._-]*", args.version):
        parser.error("Version invalida")
    out = ROOT / "artefactos" / f"CGICH-{args.version}-win64.zip"
    out.parent.mkdir(exist_ok=True)
    checksum = out.with_suffix(".zip.sha256")
    if out.exists() or checksum.exists():
        raise FileExistsError("Ya existe esa versión.")
    if subprocess.check_output(["git", "status", "--porcelain"], cwd=ROOT).strip():
        raise RuntimeError("Empaquetar desde un checkout limpio para identificar el código.")
    commit = subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=ROOT, text=True).strip()
    # Windows PowerShell debe resolver sus propios módulos aun si Python se inició desde pwsh.
    ps_env = {k: v for k, v in os.environ.items() if k.upper() != "PSMODULEPATH"}
    for script in ("Verify-Project-Assets.ps1", "Build.ps1"):
        subprocess.run(["powershell", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File",
                        ROOT / "scripts" / script], cwd=ROOT, env=ps_env, check=True)
    with tempfile.TemporaryDirectory(dir=out.parent) as temp:
        archive = Path(temp) / out.name
        rows = []
        with ZipFile(archive, "w") as zipfile:
            for name, source in sorted(runtime_files().items()):
                data = source if isinstance(source, bytes) else source.read_bytes()
                put(zipfile, name, data)
                rows.append(dict(path=name, bytes=len(data), sha256=hashlib.sha256(data).hexdigest()))
            put(zipfile, "release.json", json.dumps(dict(version=args.version, commit=commit,
                files=rows), indent=2).encode())
        digest = hashlib.sha256(archive.read_bytes()).hexdigest()
        archive.rename(out)
        checksum.write_text(f"{digest}  {out.name}\n", encoding="ascii")
    print(f"{out}: {len(rows)} archivos verificados por manifiesto, SHA256 {digest}")


if __name__ == "__main__":
    main()
