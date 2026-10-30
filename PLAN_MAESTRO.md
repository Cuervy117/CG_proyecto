# Plan maestro — Proyecto final CGICH

Actualizado: **30-oct-2026**, hora CDMX. Entrega inaplazable: **7-nov-2026 antes de las 12:00**, Classroom.

## 0. Autoridad y uso

1. Los lineamientos originales en docs/ definen requisitos y evaluación.
2. `docs/Propuesta_Proyecto_Final_Grafica.pdf` define el producto: Vacío, Shinobi, Rek’Sai y Dragon Ball. Este plan concreta su ejecución, no sustituye esos universos ni el avatar.
3. Este acuerdo sustituye el calendario de septiembre y el plan del 29-oct. El código demuestra lo implementado; no puede eliminar requisitos del profesor.
4. README.md contiene el arranque compartido; AGENTS.md guarda datos locales de máquina y no se versiona.
5. Actualizar planeación pendiente sin reescribir hechos históricos ni inventar horas.

## 1. Equipo, estado y capacidad

| Rol confirmado | Integrante | Responsabilidad |
|---|---|---|
| A | David Díaz Antúnez | Motor, escena, cámaras, luces, build, merges y Release |
| B | Jonhatan Alexis Martínez Ojeda | Mundo, infraestructura, carga de recursos y animaciones de infraestructura |
| C | Venancio Miguel Tapia Verastegui | Rek’Sai, criatura animada, entrada, experiencias, main y HUD |

Reparto confirmado el 30-oct; sustituye la asignación preliminar de infraestructura a Venancio en la propuesta. La elección de universo es independiente del rol técnico: Gohan corresponde a Venancio según la propuesta; David/Jonhatan deben confirmar quién eligió Rek’Sai/Naruto y registrarlo.

Estado de partida: motor en src/engine; main activo P09-2-424104230.cpp; P09-424104230.cpp excluido. Las carpetas scene, world, cameras, avatar, anim y ui son esqueleto, no subsistemas implementados. La bitácora registra build y capturas de la base, no cumplimiento final.

Stack: VS2022/v143 x64, GLFW/GLEW/GLM/Assimp. Objetivo C++17/OpenGL 3.3; todavía verificar configuración: contexto heredado 4.3 y estándar C++17 no explícito en vcxproj. No reescribir motor ni introducir CMake, ECS, PBR, física, skinning o frameworks de UI.

La estimación antigua suma **21.5 persona-días**, no 24, y omite un desglose claro de integración, experiencias y animaciones continuas. No prometer 27 persona-días disponibles ni colchón automático: confirmar horas efectivas por persona. El plazo tiene riesgo alto. Primero versión mínima integrada, luego detalle. 6-nov reservado para incidencias. Los 200 MB son una meta interna, no exigencia del profesor.

## 2. Rúbrica y autoría

Escenario **60 %**: 410 puntos base, hasta 60 extra. Planeación **20 %**: 40 puntos. Desarrollo **20 %**: 80 puntos.

| Escenario | Puntos |
|---|---:|
| Geometría 3D | 10 |
| Jerarquía | 10 |
| Ambientación: tres universos y ambas temáticas | 40 |
| NPCs de otras especies | 10 |
| Circuito con naves corriendo | 20 |
| Plataforma con cohete que despega | 20 |
| Estación con entrada/salida de naves | 20 |
| Lunas, asteroides y satélites | 10 |
| Dos planetas con evolución diferenciada | 40 |
| Texturizado de todos los elementos | 20 |
| Avatar jerárquico no humanoide | 10 |
| Textura y material del avatar | 10 |
| Caminata del avatar | 10 |
| R1: recorrer cada planeta, tercera persona paralela a XZ | 10 |
| R2: tomar el rol de una nave en la carrera | 10 |
| R3: nave en tercera persona entre planetas | 10 |
| R4: lanzamiento, tránsito del hangar y celestes | 10 |
| Cambio correcto de cámaras | 10 |
| Ciclo direccional día/noche reflejado en materiales | 10 |
| Puntuales sincronizadas, ciclo máximo dos minutos | 10 |
| Skybox espacial distinto según planeta | 10 |
| Spots por teclado | 10 |
| Tres objetos animados por teclado | 30 |
| Tres objetos en ciclos continuos por funciones/algoritmos | 30 |
| Estilo visual integrado | 30 |
| Originalidad extra (opcional) | hasta 30 |
| Animaciones complejas extra (opcional) | hasta 30 |

Planeación: documento repositorio/colaboración (10), contenido/manejo del repositorio (10), propuesta corregida (10), plano final modificado (10). Conservar evidencia real del semestre y del repositorio anterior; no fabricar historial.

Desarrollo: ejecutable (5 entrega + 5 funcionamiento), manual usuario (10), manual técnico (10), bitácora individual (10), demo (10 entrega + 10 contenido), videoblog (10 entrega + 10 explicaciones).

No cuentan objetos, animaciones, texturas ni iluminación presentados en prácticas. El profesor indica CERO en escenario si los elementos no son creados por alumnos, con la excepción indicada de modelos optimizados y texturas. Registrar origen/licencia. Rover, holocron y satélite de prácticas solo son referencias, nunca contenido final de emergencia. Reutilizar infraestructura no equivale a autorización para reutilizar contenido evaluado. El PDF no define política de IA: aclarar su uso admisible con el profesor y declarar la ayuda realmente utilizada.

## 3. Producto mínimo conforme a los croquis

| Ubicación | Contenido |
|---|---|
| P1 Vacío | V1 spawn Rek’Sai; V2/V3 cráteres y entradas decorativas de túnel; V4 cristales; V5 árboles; V6/V7/V10 especies y letreros; V8 muelle; V9 luminarias |
| P2 Shinobi | N1 spawn; N2 cuatro edificios repetidos con emblema Hoja; N3 monumento sencillo; N4 árboles; N5 criatura tipo Kurama y variantes; N6 cápsula/puerta; N7 rocas; N8 luminarias/letreros; D1 plataforma/cohete; D2 domo; D3 referencia de Gohan |
| Espacio | E1 disco/cúpula/hangar inspirado en Freezer; E3 dos cápsulas en tránsito; E2 un óvalo con tres cápsulas; E4 nave de vuelo; E5 sol; E6 luna; E7 asteroides; E8 satélite nuevo/paneles |

Usar croquis páginas 9–12 de la propuesta; sus ejes son unidades de diseño. Probar escala/distancias con la velocidad del avatar antes de decorar. No construir interiores de estación o túneles.

Evolución: formas ancestrales y adaptadas con diferencias estructurales y letreros; P1 placas/extremidades excavadoras, P2 rasgos de bosque/colas/marcas. No limitar a tamaño juvenil/adulto. Mínimo interno: dos formas distinguibles por planeta. El profesor no exige tres etapas ni simulación biológica.

## 4. Arranque reproducible

Seguir README, elegir rama, extraer paquete de recursos, compilar con `scripts/Build.ps1` y ejecutar desde raíz. Registrar en bitácora propia: máquina, commit, build, warnings, captura y siguiente hito. Generar AGENTS.md local; no repetir la Fase 0 histórica ni recrear v0.1-base.

`scripts/Package-Dependencies.ps1` genera ZIP con Models/, Textures/, include/, lib/, glm/, tres DLLs y manifiesto SHA-256 en artefactos/ (ignorado). Es material para arrancar la base de prácticas, no paquete final ni declaración de autoría. David lo entrega por carpeta compartida/USB y registra la ubicación acordada. Un clon solo no compila. No inventar enlaces ni enviar archivos a terceros automáticamente.

## 5. Arquitectura y contratos mínimos

### 5.1 Límites

Un motor, tres ubicaciones lógicas (Espacio, Vacio, Shinobi), cuatro modos (Planeta, Carrera, Vuelo, Observacion). Ubicación y modo son estados independientes. Recursos cargados una vez; dibujar ubicación activa. Sin aterrizaje continuo ni caminar sobre esfera.

OpenGL en engine y shaders. Excepción limitada: Scene.cpp puede recorrer nodos, enviar uniforms y dibujar con el motor para evitar otra capa. B/C usan estas APIs. Main heredado conserva llamadas hasta sustituirse. Un único main.

### 5.2 A publica primero (H-A1)

Estas interfaces están **por implementar**, no existen todavía. Publicar headers mínimos compilables antes de conectar prefabs:

- SceneNode: posición, Euler en radianes, escala, nombre único, visible, material, matriz world. Hijos poseídos con unique_ptr; parent prestado. Local = T * Ry * Rx * Rz * S. Pivotes articulares mediante nodo padre y desplazamiento de geometría.
- Nodo dibujable: Model* **o** MeshModel* (nunca ambos), Texture* explícita para malla procedural. Recursos prestados y estables; el nodo no libera GPU. Model usa sus materiales importados.
- add(unique_ptr<SceneNode>) devuelve SceneNode*; updateWorld recorre DFS; find(nombre) devuelve puntero o null; render recibe vista/proyección. Ocultar padre oculta subárbol.
- CameraRig actualiza con dt y target, devuelve view; FOV en grados convertido a radianes para projection. CameraDirector posee unique_ptr<CameraRig>; las fábricas devuelven unique_ptr, no un puntero de propiedad ambigua.
- FollowXZ, Chase, Flight, Orbit. Objetivo desde world después del update. Altura de suelo por ubicación. Primero cambio directo correcto; después suavizado de posición/objetivo, nunca mezcla de matrices de vista.

### 5.3 Contenido e interacción (B/C)

B publica World.h: enum LocationId { Espacio, Vacio, Shinobi } y WorldBindings con raíces, spawns/salidas, límites XZ y referencias a cohete, puertaCapsula, panelIzq/panelDer, tres capsulasCarrera, dos capsulasHangar, naveVuelo y satelite. Nombres prefijados vacio./shinobi./espacio.; no depender de números de mesh. Bindings prestados, estables durante la escena. Un binding faltante deshabilita su modo con mensaje, nunca se desreferencia.

C publica Avatar.h: construir Rek’Sai y criatura N5 bajo padre, devolver raíz/pivotes y actualizar con dt/dirección/velocidad. B deja ancla N5 y C inserta criatura. B/C preparan piezas, texturas y layouts mientras A publica headers; no inventan otro SceneNode.

C adapta Input.hpp: hoy son funciones y EstadoEscena, no una clase Input. Crear InputFrame mínimo con teclas, flancos, mouse/rueda y update(dt, const InputFrame&) para experiencias. Consumir flancos una vez; interacción E tiene prioridad sobre ascenso.

**B implementa cinco animaciones de infraestructura en world/WorldAnimation.*; C implementa N5 y caminata en game/anim.** C llama ambos desde el único bucle. Así no carga todas las animaciones en C.

### 5.4 Frame

Poll → entrada → modo/ubicación → mundo/criatura/avatar → updateWorld → luces → cámara → skybox/opacos/transparencias/HUD → swap. Un reloj en main, dt máximo 0.05. Animaciones continuas conservan/actualizan estado aunque la ubicación esté oculta. Cambio de cámara no reinicia ni recarga recursos. Liberar GPU antes de destruir contexto.

## 6. Calendario y hitos de hoy

| Fecha | David A | Jonhatan B | Venancio C | Resultado |
|---|---|---|---|---|
| 30-oct | Contratos, bloqueantes, escena/FollowXZ | Entorno, recursos, blockout P1/P2 | Entrada/estado, Rek’Sai/main | Avatar camina en un mapa en ejecutable común |
| 31-oct | Chase/Flight/Orbit básicos, paquete en otra PC | Espacio/circuito/estación/plataforma básicos | E entre mapas, F1–F4, objetivos/HUD | Ambos mapas explorables y cuatro modos |
| 1-nov | Día/noche, spots, skyboxes | Cinco animaciones mundo, texturas | N5 continua, evolución con B | Contenido y animaciones reales |
| 2-nov | Integración/fallos/rendimiento | Texturizado/identidad visual | QA de entrada/modos | Primera demo de todos los requisitos |
| 3-nov | Release externo, correcciones | QA mundo/paquete | QA avatar/HUD | Freeze 18:00 |
| 4–5 nov | Manual motor/build, documento Git | Manual mundo/assets, demo/plano | Manual usuario y ensamble técnico | Paquete, PDFs y videos listos |
| 6-nov | Incidencias y comprobación de entrega | Incidencias | Incidencias | Reserva sin nuevas funciones |
| 7-nov | Equipo comprueba acuse antes de 12:00 | Equipo | Equipo | Entrega confirmada |

### 6.1 Hitos del 30-oct

Duraciones orientativas, no horas realizadas ni garantía de capacidad. Confirmar disponibilidad real. No exigir desvelo como método de planificación.

| ID | Dueño | Trabajo | Dependencia | Terminado cuando |
|---|---|---|---|---|
| H-A0 | David | Arranque y entregar paquete | Recursos locales | B/C reciben paquete; build y exe identificados |
| H-A1 | David | Headers SceneNode/Scene/CameraRig + ejemplo (60–90 min) | H-A0 | Headers compilables; hijo hereda padre |
| H-A2 | David | Materiales/texturas/FOV; rutas con B (60–120 min) | APIs actuales | Modelo de un material correcto; textura ausente detectada; base sin regresión |
| H-A3 | David | Render Model/Mesh + FollowXZ (90–150 min) | H-A1 | Cámara sigue raíz de avatar; pivotes correctos |
| H-B0 | Jonhatan | Clonar, paquete, build local (30–60 min) | H-A0 | Release propio con recursos visibles |
| H-B1 | Jonhatan | Inventario, escala, spawns y límites (45–90 min) | Propuesta | V1/V8, N1/N6, anclas D1/N5 definidas |
| H-B2 | Jonhatan | World.h y dos pisos texturizados (90–150 min) | H-A1 | Bindings disponibles; P1/P2 distinguibles; sin main propio |
| H-C0 | Venancio | Clonar, paquete, build local (30–60 min) | H-A0 | Release propio con recursos visibles |
| H-C1 | Venancio | InputFrame y estado modo/ubicación (45–90 min) | Contratos A/B | Flancos y controles sin colisiones |
| H-C2 | Venancio | Rek’Sai, pivotes, textura/material, caminata (90–180 min) | H-A1 | Raíz mueve conjunto; patas alternan solo al avanzar |
| H-C3 | Venancio | Main + P1 + avatar + FollowXZ (60–120 min) | H-A3/B2/C2 | Un main; avatar camina con cámara/límites; captura |
| H-E0 | Todos; A coordina | Merge y prueba (30–60 min) | Anteriores | develop compila; evidencia y pendientes registrados |

H-A1 desbloquea a B/C. Preparar assets/datos mientras llega. Si hoy solo alcanza un mapa integrado, cerrar P1 y preparar bindings P2 para mañana. No esperar arte terminado para integrar. R4 elaborado, partículas y pulido no son tareas de hoy.

## 7. Matriz de aceptación

Estado inicial de todas las filas: **pendiente de implementación final**. Progresión: implementado → integrado → demostrado; cada estado con commit/evidencia. Contenido de prácticas no acredita cumplimiento.

| ID | Dueño | Evidencia obligatoria |
|---|---|---|
| GEO | A/B/C | Volumen 3D; raíz mueve conjunto, articulación solo descendientes |
| AMB | B/C | Vacío, Naruto y Dragon Ball reconocibles por elementos, no solo colores |
| EVO | B/C | P1/P2 con especies ancestrales/adaptadas, diferencias estructurales y letreros |
| TEX | A/B/C | Todos los elementos texturizados, incluso procedurales; materiales responden a luces |
| AVA | C | Rek’Sai no humanoide, jerarquía/material y caminata con idle |
| PISTA | B/C/A | Tres cápsulas; usuario controla una y dos repiten circuito; cámara R2 |
| LANZ | B | D1/cohete, 1 inicia y permite repetir sin mantener tecla |
| EST | B | Dos cápsulas cruzan entrada/salida del hangar visible |
| CEL | B | Luna, asteroides y satélite nuevo observables |
| R1 | A/C/B | Recorrer ambos planetas sobre XZ; E entra/sale con objetivo válido |
| R3 | A/B/C | Nave controlable en tercera persona entre planetas |
| R4 | A/C/B | Tab observa D1 en P2, hangar en espacio y celestes, con ubicación dibujada correcta |
| CAM | A/C | F1–F4 sin objetivos nulos ni reinicios accidentales |
| LUZ | A | Ciclo ≤120 s, puntuales simultáneas y spots L/K |
| SKY | A/B | Fondo espacial y fondos diferentes P1/P2, oscurecimiento nocturno |
| ANIM | B/C | B1/B2/B3 por teclado, C1/C2/C3 continuas; caminata aparte |
| REL | A/todos | Release con DLLs/assets/shaders en otra PC sin compilar |
| DOC | Todos | Checklist §13 y contenido verificable |

Prioridad: interfaces/integración y todos los requisitos base. Ambientación + evolución + animación suman 140 puntos; eso no elimina avatar, cámaras o metodología. Extras después de demo completa.

## 8. Diseño y controles únicos

### 8.1 Ubicación/modo

R1 funciona en Vacio/Shinobi. F1 desde espacio entra al último planeta (P1 por defecto) en spawn seguro; la ruta normal de viaje es R3 + E cerca de planeta. F2/F3 seleccionan espacio y cápsula/nave. F4 observa; Tab recorre D1 en Shinobi, hangar en espacio y celestes. Cambiar ubicación dibujada según objetivo conserva animaciones. Volver a R1 restaura una posición válida. Deshabilitar modos cuyos bindings no existan.

### 8.2 Controles

| Entrada | Acción |
|---|---|
| F1/F2/F3/F4 | Planeta / carrera / vuelo / observación |
| WASD + mouse | Movimiento/orientación del objetivo activo |
| Q/E en R3 | Descenso/ascenso; E cercano a planeta prioriza entrar |
| E en cápsula de superficie | Volver al espacio; indicación contextual |
| 1 | Cohete D1, también durante observación del lanzamiento |
| 2 | Puerta N6 |
| 3 | Paneles E8 |
| L/K | Spot nave / plataforma |
| Tab + rueda R4 | Siguiente objetivo / distancia orbital |
| Esc | Salir |

No usar 1–4 para cámaras ni F1 para debug. HUD muestra modo, ubicación y acciones. Pausa/debug adicionales se aplazan hasta documentarlos sin colisiones.

### 8.3 Animaciones

| ID | Objeto | Dueño | Mínimo |
|---|---|---|---|
| B1 | Cohete D1 | B | Reposo → llama → ascenso → final/reinicio por 1 |
| B2 | Puerta N6 | B | Giro de bisagra abrir/cerrar por 2 |
| B3 | Paneles E8 | B | Pivotes abrir/cerrar por 3 |
| C1 | Cápsulas E2 | B | Dos repiten óvalo; tercera controlable, límites sencillos de pista |
| C2 | Cápsulas E3 | B | Llegada/cruce hangar/salida/repetición |
| C3 | Criatura N5 | C | Camina entre puntos y mueve patas/colas continuamente |
| Avatar | Rek’Sai | C | Caminata ligada a velocidad y orientación; independiente de seis anteriores |

No hacen falta splines, física, órbitas de asteroides o estación animada. No recargar recursos al reiniciar.

### 8.4 Luces y recursos

A implementa sol/ciclo 120 s y puntuales; B proporciona posiciones/skyboxes. Motor actual: 3 puntuales y 4 spots; reutilizar según ubicación. Cambiar máximos en C++ y shader juntos si se justifica. Todas las piezas de avatar/estación/terreno tienen textura y material; tint no sustituye textura.

Rek’Sai por cuerpo/cabeza/cola/patas; sin skinning/combate. Geometría/puerta/paneles usan pivotes explícitos. Reutilizar prefabs de edificio/árbol/cristal.

Models/ y Textures/ siguen en raíz. Modelos nuevos en b/c. **Hasta corregir y verificar el cargador**, texturas importadas en `Textures/b_nombre.ext` o `Textures/c_nombre.ext`: actualmente elimina subcarpetas. H-A2 acuerda soporte de rutas con B; después se permiten subcarpetas verificadas. No convertir todo a GLB sin probar transformaciones/texturas embebidas.

## 9. Correcciones técnicas presupuestadas (pendientes)

| Dueño | Hallazgo | Aceptación |
|---|---|---|
| A | Model::RenderModel usa !materialIndex < size | Índices correctos, un material funciona, inválido no accede fuera de rango |
| A | Texture devuelve true al fallar stbi_load | Retorna fallo y fallback explícito |
| A/B | Rutas pierden subcarpetas | Contrato §8.4 verificado en otro checkout |
| A | FOV sin convertir a radianes | Proyección conforme al ángulo previsto |
| A | Texturas compartidas borradas por entrada | Dueño único antes de activar limpieza; no añadir destructor a ciegas |
| A | Normales negadas y convención de luz | Probar normales y shader juntos, no invertir ciegamente |
| A | Assimp ignora transformaciones; GLB embebido sin tratamiento | Exportación/piezas verificadas o parche mínimo para assets usados |
| A | Skybox/flip global | Caras +X,-X,+Y,-Y,+Z,-Z y flip controlado |
| A | GL4.3/estándar implícito | Alinear objetivo 3.3/C++17 y validar en PCs del equipo |

No limpiar warnings masivamente; clasificar los que afecten retornos, índices o recursos. Esta revisión no declara corregidos los fallos.

## 10. Propiedad y documentación

| Rol | Rutas | Documentación |
|---|---|---|
| A David | engine excepto Load.*, scene, game/cameras, shaders, scripts, configuración global VS | Motor/build/Release, Git, plan/README |
| B Jonhatan | game/world incluida WorldAnimation, engine/Load.*, recursos mundo | Inventario/licencias, sección mundo, demo/plano |
| C Venancio | game/avatar, game/anim, Experience.*, game/main.cpp, Input.*, ui | Manual usuario, avatar/interacción, ensamble manual técnico |

Cada uno escribe su bitácora/conclusión y participa en videoblog. C no redacta el trabajo de todos.

Excepciones: cada rol registra sus fuentes en su ItemGroup vcxproj; C cambia el main en el mismo commit que lo introduce; A resuelve conflictos del XML. B puede editar Load.* aunque esté en engine. Cambios de interfaz requieren actualizar contrato y avisar consumidores.

## 11. Riesgos y recortes

- 31-oct sin caminata: simplificar piezas conservando silueta de Rek’Sai; nunca rover de prácticas.
- 31-oct sin modos: geometría provisional nueva; contratos y recorrido antes de decorar.
- 2-nov falta requisito: detener detalle y resolver dependencia. R4 puede usar encuadres fijos, pero debe mostrar lanzamiento/hangar/celestes.
- Fallo en otra PC el 31-oct: arreglar runtime/rutas antes de ampliar; repetir validación completa 3-nov.
- Reducir variedad NPC/vegetación, interiores, monumento, efectos/transiciones; no quitar NPCs, texturas, universos, evolución, cuatro recorridos o seis animaciones.
- B concentra mundo: prefabs repetidos, B anima infraestructura, C entrega criatura/variantes. Reasignar archivos explícitamente antes de editarlos.

## 12. Decisiones vigentes

Propuesta define apariencia; lineamientos mandan; plan define ejecución. Un motor/tres ubicaciones; avatar por piezas; estación sin interior; una pista orbital; seis animaciones y caminata aparte. Integración diaria. Un main/VS/Release x64. README y referencias versionados, AGENTS local, recursos en paquete identificado.

## 13. Checklist final

- [ ] Release, DLLs, shaders y assets usados: probado en otra PC sin compilar.
- [ ] Fuentes comentadas/estructuradas e instrucciones reproducibles.
- [ ] Manual usuario con capturas, teclado/mouse y todos los recorridos.
- [ ] Manual técnico: captura/código por requisito, conclusiones individuales y grupal, bibliografía.
- [ ] Bitácoras diarias: objetivo, resultado, horas reales y evidencia por hito.
- [ ] Demo sin voz de todos los elementos.
- [ ] Videoblog de los tres: planeación, implementación y ejecución.
- [ ] Documento Git y evidencia real de ramas/colaboración del semestre/almacenamiento.
- [ ] Propuesta corregida y plano final contrastados con implementación; septiembre no basta.
- [ ] Origen/licencias y contenido propio verificados.
- [ ] Paquete/videos listos antes del 6-nov; acuse Classroom antes del 7-nov 12:00.

## 14. AGENTS.md local

Incluir rol/rutas §10, rama, ruta real, build/run verificados, GPU/contexto, estado y siguiente hito, restricciones y referencia a este plan. No copiar rutas de otra PC ni afirmar stubs terminados.

## 15. Prompt de arranque

```text
Mi rol es <A David / B Jonhatan / C Venancio>. Lee README y PLAN_MAESTRO,
especialmente §5 contratos, §6 hitos, §8 controles y §10/§16 propiedad.
Revisa headers reales y verifica rama, dependencias y Release x64.
Genera/actualiza AGENTS.md local con datos comprobados.
Implementa el siguiente hito pendiente de mi rol; no inventes APIs ni otro main.
Respeta rutas; B sí puede editar Load.*. Prepara datos mientras esperas contratos.
Valida en ejecutable común y registra evidencia/horas reales en mi bitácora.
Si no conoces horas déjalas por confirmar; no declares listo sin criterio cumplido.
```

### 15.1 Arranque B/C

Usar el mismo prompt con rol correcto. Verificar entorno propio, no repetir Fase 0 histórica. Preparar assets y layout mientras llega H-A1, sin inventar interfaces incompatibles.

## 16. Git y colaboración

### 16.1 Ramas

Remoto: https://github.com/Cuervy117/CG_proyecto.git. main = base/releases; develop = integración compilable; feat/a-motor, feat/b-mundo, feat/c-avatar = trabajo individual. Conservar rama david e historial existente. Preparación inicial une main remoto y base local; nunca push forzado ni recrear tags. Preparado localmente no significa publicado.

### 16.2 Merges

Ventanas orientativas 13:00 y 19:00 CDMX; si una pasó, acordar siguiente corte. Headers temprano. Incorporar develop antes de pedir merge; A coordina. No rebase de ramas compartidas. Push diario de trabajo verificable.

### 16.3 Visual Studio

Cada uno registra fuentes en su ItemGroup A-engine/A-scene/A-cameras/B-world/C-avatar. Load.* ya está en A-engine pero es de B: no duplicarlo. C modifica main-test al sustituir main. Filters está versionado: evitar cambios cosméticos y dejar organización a A; gitignore no desversiona archivos existentes.

### 16.4 Assets

Models, Textures, include, lib, glm y DLLs ignorados. ZIP/manifiesto en artefactos se entregan aparte. Registrar versión/hash y ubicación compartida. Publicar otro paquete cuando cambien recursos usados por compañeros. Ver nombres temporales de texturas §8.4.

### 16.5 Un main

C introduce game/main.cpp y excluye P09-2 del build en el mismo commit, sin borrar fuente de referencia. A/B no crean mains alternos. Main heredado solo prueba infraestructura.

### 16.6 Documentos

Versionar plan, README, PDFs originales, bitácoras, capturas y documentos de entrega. Cada persona registra solo sus hechos/horas. Esta revisión actualiza la planeación pendiente por autorización del usuario; no inventa actividad de compañeros. PDFs originales inmutables: correcciones finales en archivos distintos.

### 16.7 Local

AGENTS ignorado, README compartido. No depender de Obsidian ni copiar secretos/configuración personal.

### 16.8 DoD de merge

Release x64 cero errores, sin warnings nuevos no explicados; ejecutar con recursos visibles y comportamiento del hito; un main/rutas propias; contratos vigentes; bitácora/evidencia reales; probar unión en develop, no solo rama aislada.

### 16.9 Límites

Sin reescribir motor/refactor general/dependencias nuevas. Correcciones §9 sí presupuestadas. No contenido de prácticas como final. No editar trabajo ajeno salvo integración/asignación expresa.

### 16.10 Primer arranque

Tras publicar preparación: clonar, cambiar rama propia, extraer paquete en raíz, Build.ps1, ejecutar desde raíz, registrar resultado y comenzar H-B1/H-C1. Ver README para comandos.
