"""Blender en segundo plano: blender -b --factory-startup --python-exit-code 1 -P scripts/Convertir-Vacio.py"""
from pathlib import Path
import tempfile
import bpy

ROOT = Path(__file__).resolve().parents[1] / "Models/reksai"
NAMES = ("voidmite", "voidgrub", "void_spawn", "rift_herald")


def convert(name, folder):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.gltf(filepath=str(ROOT / (name + ".glb")))
    bpy.context.scene.frame_set(1)
    meshes = [obj for obj in bpy.context.scene.objects if obj.type == "MESH"]
    materials = {slot.material for obj in meshes for slot in obj.material_slots}
    if not meshes or len(materials) != 1:
        raise ValueError(f"{name}: se esperaba una malla texturizada con un único material")
    mat = materials.pop()
    images = {n.image for n in mat.node_tree.nodes if n.type == "TEX_IMAGE" and n.image}
    if len(images) != 1:
        raise ValueError(f"{name}: falta la imagen de color")
    img = images.pop()
    png = folder / (name + "_diffuse.png")
    png.write_bytes(bytes(img.packed_file.data))
    if not png.read_bytes().startswith(b"\x89PNG\r\n\x1a\n"):
        raise ValueError(f"{name}: el color incrustado no es PNG")
    mat.name = name + "_material"
    bpy.ops.object.select_all(action="DESELECT")
    for mesh in meshes:
        mesh.select_set(True)
    obj_path = folder / (name + ".obj")
    bpy.ops.wm.obj_export(filepath=str(obj_path), export_selected_objects=True,
                          export_uv=True, export_normals=True, export_materials=True,
                          export_triangulated_mesh=True, apply_modifiers=True,
                          forward_axis="NEGATIVE_Z", up_axis="Y")
    # glTF usa materiales unlit. El shader del juego lee el color difuso del MTL.
    obj_path.with_suffix(".mtl").write_text(
        f"newmtl {mat.name}\nKd 1 1 1\nmap_Kd {png.name}\n", encoding="ascii")
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.wm.obj_import(filepath=str(obj_path), forward_axis="NEGATIVE_Z", up_axis="Y")
    faces = 0
    for mesh in bpy.context.selected_objects:
        if mesh.type != "MESH":
            continue
        faces += len(mesh.data.polygons)
        if not mesh.data.uv_layers.active:
            raise ValueError(f"{name}: faltan UV tras reimportar")
        for slot in mesh.material_slots:
            if not slot.material:
                raise ValueError(f"{name}: falta el material")
            tree = slot.material.node_tree
            shader = next(n for n in tree.nodes if n.type == "BSDF_PRINCIPLED")
            image = shader.inputs["Base Color"].links[0].from_node.image
            actual = Path(bpy.path.abspath(image.filepath)).resolve()
            if actual != png or min(image.size) <= 0:
                raise ValueError(f"{name}: textura importada incorrecta: {actual}")
    if not faces:
        raise ValueError(f"{name}: OBJ vacío")
    print(f"[OBJ OK] {name}: {faces} caras, UV y textura {png.name}")


with tempfile.TemporaryDirectory() as tmp:
    folder = Path(tmp).resolve()
    for name in NAMES:
        convert(name, folder)
    # Sólo publicar los tres recursos de cada criatura. Preservar ediciones locales.
    outputs = [(folder / (name + suffix), ROOT / (name + suffix))
               for name in NAMES for suffix in (".obj", ".mtl", "_diffuse.png")]
    for source, target in outputs:
        if target.exists() and source.read_bytes() != target.read_bytes():
            raise FileExistsError(f"La conversión difiere de {target}")
    for source, target in outputs:
        if not target.exists():
            target.write_bytes(source.read_bytes())
