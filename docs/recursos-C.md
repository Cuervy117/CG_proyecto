# Recursos de Venancio (C)

## Gohan SSJ2 para D3

La propuesta entregada ubica a Gohan como referencia Dragon Ball en D3 (imagen o silueta junto al domo). No lo define como avatar ni incluye caminata. El OBJ disponible es estático y se prepara como recurso de referencia; no declara rig ni animaciones.

- Modelo: Models/gohan_ssj2.obj y Models/gohan_ssj2.mtl.
- Texturas planas: Textures/c_gohan_ssj2_clothes1.png, Textures/c_gohan_ssj2_clothes2.png, Textures/c_gohan_ssj2_eyes.png y Textures/c_gohan_ssj2_face.png.
- El OBJ/MTL se renombraron y sus referencias se ajustaron para seguir las rutas planas del proyecto. No se copió Gohan.png, que no está referenciada por el MTL.
- Fuente: Dragon Ball Z: Budokai, asset 286914, descargado de https://models.spriters-resource.com/gamecube/dragonballzbudokai/asset/286914/; extracción atribuida a Roxas358.
- Licencia: el readme de extracción dice que no requiere crédito, pero no se encontró una licencia abierta que cubra los derechos del videojuego o del personaje. Mantener esta procedencia y confirmar el uso académico con el profesor si hace falta.

## Rek'Sai recibido del equipo

`Models/reksai.obj`, `Models/reksai.mtl` y `Textures/reksai.png` están colocados en esas rutas en el checkout local de `feat/c-avatar`. El OBJ/MTL y el PNG se localizaron en el Drive compartido del equipo. El OBJ trae coordenadas UV y grupos que el avatar reparte entre cuerpo, cabeza, cola y cuatro patas; `Avatar::attach` carga el PNG y lo asigna a cada pieza. La revisión del archivo encontró UVs y geometría para las siete piezas, pero esto no sustituye probarlo en el ejecutable.

- SHA-256 local del OBJ: `4ABFE2537CFCF4EB05D475BA652041941703806D85470646983FF4CE69C0D4B3`.
- SHA-256 local de la textura: `9C9984C51848EB7306DE0DC0F9C01C930D4F7DA99B642774A175460007979FFB`.
- El OBJ referencia `reksai.mtl`; el MTL apunta a `../Textures/reksai.png`, consistente con la estructura `Models/` y `Textures/`. El cargador de C usa el PNG explícito y no lee el MTL; este sirve al abrir el OBJ directamente en Blender/otro visor.
- `Models/` y `Textures/` siguen ignorados por Git; los archivos permanecen locales y deben compartirse con el paquete acordado por el equipo.

## Empaquetado

Models/ y Textures/ están ignorados por Git según el acuerdo del equipo; compartir estos recursos mediante la carpeta/paquete acordado. Los hashes no sustituyen la verificación de carga en la aplicación.
