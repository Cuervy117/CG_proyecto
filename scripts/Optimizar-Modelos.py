"""Reduce modelos OBJ con el modificador Decimate de Blender.

Uso:
  blender --background --factory-startup --python scripts/Optimizar-Modelos.py -- \
      --input Models/npcSelva/pulpo.obj --output Models/_opt_tmp/pulpo.obj --target 30000

El OBJ de salida conserva el mismo nombre base que el original para que su linea
`mtllib` siga apuntando al MTL original. El MTL generado por Blender se deja en el
directorio temporal y no se usa: el juego conserva el MTL original, que ya tiene
las rutas de texturas resueltas por Model.cpp.
"""

import argparse
import json
import os
import sys

import bpy
from mathutils import Vector


def parse_args():
    args = sys.argv
    if "--" not in args:
        raise SystemExit("Faltan argumentos despues de --")
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", required=True)
    parser.add_argument("--output", required=True)
    parser.add_argument("--target", type=int, required=True)
    return parser.parse_args(args[args.index("--") + 1:])


def mesh_objects():
    return [obj for obj in bpy.context.scene.objects if obj.type == "MESH"]


def triangle_count():
    total = 0
    for obj in mesh_objects():
        obj.data.calc_loop_triangles()
        total += len(obj.data.loop_triangles)
    return total


def bounds():
    minimo = Vector((float("inf"),) * 3)
    maximo = Vector((float("-inf"),) * 3)
    for obj in mesh_objects():
        for corner in obj.bound_box:
            p = obj.matrix_world @ Vector(corner)
            for i in range(3):
                minimo[i] = min(minimo[i], p[i])
                maximo[i] = max(maximo[i], p[i])
    return [round(v, 6) for v in minimo], [round(v, 6) for v in maximo]


def main():
    opciones = parse_args()
    entrada = os.path.abspath(opciones.input)
    salida = os.path.abspath(opciones.output)
    os.makedirs(os.path.dirname(salida), exist_ok=True)

    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.wm.obj_import(
        filepath=entrada,
        forward_axis="NEGATIVE_Z",
        up_axis="Y",
        use_split_objects=False,
        use_split_groups=False,
    )

    objetos = mesh_objects()
    if not objetos:
        raise SystemExit("El OBJ no produjo objetos de malla")
    for obj in objetos:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = objetos[0]

    tris_antes = triangle_count()
    bbox_antes = bounds()
    ratio = max(0.0001, min(1.0, float(opciones.target) / max(tris_antes, 1)))

    for obj in objetos:
        mod = obj.modifiers.new("QA_Decimate", "DECIMATE")
        mod.decimate_type = "COLLAPSE"
        mod.ratio = ratio
        mod.use_collapse_triangulate = True
        bpy.context.view_layer.objects.active = obj
        bpy.ops.object.modifier_apply(modifier=mod.name)

    tris_despues = triangle_count()
    bbox_despues = bounds()

    bpy.ops.wm.obj_export(
        filepath=salida,
        export_selected_objects=True,
        export_uv=True,
        export_normals=True,
        export_materials=True,
        export_triangulated_mesh=True,
        path_mode="RELATIVE",
        forward_axis="NEGATIVE_Z",
        up_axis="Y",
    )

    resultado = {
        "input": opciones.input,
        "output": opciones.output,
        "target": opciones.target,
        "ratio": round(ratio, 6),
        "tris_antes": tris_antes,
        "tris_despues": tris_despues,
        "bytes_antes": os.path.getsize(entrada),
        "bytes_despues": os.path.getsize(salida),
        "bbox_antes": bbox_antes,
        "bbox_despues": bbox_despues,
        "objetos": len(objetos),
    }
    print("RESULTADO " + json.dumps(resultado))


if __name__ == "__main__":
    main()
