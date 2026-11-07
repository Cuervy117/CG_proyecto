"""Genera mapas UV reproducibles para la luna y el planeta RekSai."""
from pathlib import Path
from PIL import Image
import math
import random

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "Textures"
OUT.mkdir(exist_ok=True)
W, H = 512, 256

def ruido(x, y, semilla):
    n = (x * 73856093) ^ (y * 19349663) ^ semilla
    n = (n ^ (n >> 13)) * 1274126177
    return ((n ^ (n >> 16)) & 65535) / 65535.0

def campo(u, v, semilla):
    valor = 0.0
    peso = 0.0
    for escala, amplitud in ((5, .48), (13, .25), (31, .16), (71, .11)):
        x = u * escala
        y = v * escala
        ix, iy = math.floor(x), math.floor(y)
        fx, fy = x - ix, y - iy
        fx, fy = fx*fx*(3-2*fx), fy*fy*(3-2*fy)
        def r(dx, dy): return ruido((ix + dx) % escala, iy + dy, semilla)
        a = r(0,0)*(1-fx) + r(1,0)*fx
        b = r(0,1)*(1-fx) + r(1,1)*fx
        valor += (a*(1-fy) + b*fy)*amplitud
        peso += amplitud
    return valor / peso

rng = random.Random(2031)
crateres = [(rng.random(), .08 + rng.random()*.84, .006+rng.random()*.025) for _ in range(85)]
for nombre in ("Moon_procedural.png", "planet_reksai_procedural.png"):
    luna = nombre.startswith("Moon")
    pixels = []
    for y in range(H):
        v = y / (H-1)
        for x in range(W):
            u = x / W
            n = campo(u,v,17 if luna else 93)
            if luna:
                borde = 0.0
                for cu, cv, radio in crateres:
                    du = min(abs(u-cu), 1-abs(u-cu)) * max(.22, math.sin(math.pi*v))
                    d = math.hypot(du, v-cv) / radio
                    if d < 1.25:
                        borde += 24 if .76 < d < 1.17 else (-20 if d < .76 else 0)
                tono = max(0, min(255, int(125 + 95*(n-.5) + borde)))
                pixels.append((tono, min(255,tono+4), min(255,tono+10)))
            else:
                veta = math.sin((u*12 + v*4 + n*.8)*math.pi)*.5+.5
                brillo = max(0,min(1,.34 + .9*n + .18*veta))
                pixels.append((int(95*brillo), int(42*brillo), int(150*brillo)))
    image = Image.new("RGB",(W,H))
    image.putdata(pixels)
    image.save(OUT / nombre)
    print(OUT / nombre)

# Assimp enlaza usemtl por nombre exacto, no por el nombre del archivo MTL.
material_luna = ROOT / "Models" / "Moon.mtl"
material_luna.write_text("newmtl Material.001\nKd 1 1 1\nKs 0.08 0.08 0.08\nNs 32\nillum 2\nmap_Kd ../Textures/Moon_procedural.png\n", encoding="utf-8")
