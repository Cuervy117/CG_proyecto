# Plan de cobertura - Metodologia de desarrollo (120/120 puntos)

Fecha de corte: **7-nov-2026, hora de Ciudad de Mexico**
Entrega indicada por los lineamientos: **7-nov-2026 antes de las 12:00**
Alcance: solo el rubro **Metodologia de desarrollo (40 %)**.

## 1. Meta y regla de cierre

La metodologia suma **120 puntos**:

| Bloque | Evidencia evaluada | Puntos |
|---|---|---:|
| Planeacion | Documento de repositorio y trabajo colaborativo | 10 |
| Planeacion | Contenido y manejo del repositorio | 10 |
| Planeacion | Propuesta corregida | 10 |
| Planeacion | Plano/mapa final modificado | 10 |
| Desarrollo | Ejecutable entregado | 5 |
| Desarrollo | Ejecutable comprobado sin errores | 5 |
| Desarrollo | Manual de usuario | 10 |
| Desarrollo | Manual tecnico | 10 |
| Desarrollo | Bitacora individual | 10 |
| Desarrollo | Video demo: entrega | 10 |
| Desarrollo | Video demo: contenido completo | 10 |
| Desarrollo | Videoblog: entrega | 10 |
| Desarrollo | Videoblog: contenido y explicaciones | 10 |
| **Total** |  | **120** |

Un elemento solo se marca como cerrado cuando existe el archivo final, se abre correctamente, coincide con el Release entregado y esta incluido en Classroom. No basta con tener borradores, capturas sueltas o evidencia en una computadora.

## 2. Auditoria del estado actual

| Evidencia | Estado observado | Riesgo para el puntaje | Accion inmediata |
|---|---|---|---|
| Repositorio y ramas | Hay remoto, ramas por integrante, `develop`, commits de los tres autores y etiquetas de Release | Bajo en manejo; alto si no se explica | Preparar el documento de colaboracion con capturas y hashes reales |
| Propuesta corregida | El PDF existente contiene organizacion, cronograma, criterios, croquis corregidos, comentarios, conclusiones y bibliografia | Medio: debe coincidir con lo realmente entregado | Hacer revision final de consistencia y exportar una version final identificada |
| Plano/mapa final | Los croquis digitales estan dentro de la propuesta | Medio: la rubrica lo califica por separado | Entregar tambien un PDF independiente con leyenda y cambios finales |
| Ejecutable | Existe candidato `v0.3.1-rc.1`, verificado localmente | Alto: falta prueba limpia/otra PC registrada | Congelar, empaquetar y ejecutar sin Visual Studio desde una copia limpia |
| Manual de usuario | No se encontro artefacto final | Critico | Redactar con capturas del Release definitivo |
| Manual tecnico | No se encontro artefacto final | Critico | Redactar por requisito, con captura y fragmento de codigo |
| Bitacora David | Tiene evidencia amplia, pero conserva horas y conclusiones pendientes | Alto | Completar solo datos reales y cerrar el estado |
| Bitacora Jonhatan | Plantilla casi vacia | Critico | Jonhatan debe reconstruir solo con commits, mensajes, capturas y horas que recuerde con certeza |
| Bitacora Venancio | Detallada hasta el 2-nov; no refleja la integracion final | Alto | Actualizar 3-7 nov y cerrar pendientes con el estado real |
| Video demo | No se encontro archivo final | Critico | Grabar recorrido continuo con guion de cobertura |
| Videoblog | No se encontro archivo final | Critico | Grabar a los tres explicando planeacion, implementacion y ejecucion |

Esta auditoria no asigna una calificacion: identifica si la evidencia necesaria para defender cada punto ya esta lista.

## 3. Plan por criterio

### P1. Documento de repositorio y trabajo colaborativo - 10 puntos

**Responsable:** David coordina; cada integrante aporta evidencia de su rama.

Crear `Documento_Repositorio_Colaboracion.pdf` con:

1. Portada, integrantes, URL del repositorio y fecha de corte.
2. Herramientas utilizadas: Git/GitHub y almacenamiento adicional para archivos pesados.
3. Flujo real: ramas `feat/a-motor`, `feat/b-mundo`, `feat/c-avatar`, integracion en `develop`, liberacion en `main` y etiquetas.
4. Captura del grafo completo de commits con nombres de autores y fechas legibles.
5. Una subseccion por integrante: objetivo de su rama, 2-4 commits representativos y evidencia del resultado.
6. Capturas de ramas remotas y del historial de merges; no usar solo terminal sin contexto.
7. Organizacion del almacenamiento adicional: carpetas, responsables, convencion de nombres, version/hash del paquete y forma de compartirlo.
8. Ejemplo de un conflicto o integracion y como se resolvio.
9. Conclusion sobre coordinacion, respaldos y trazabilidad.

**Aceptacion:** todo hash mostrado existe; los nombres coinciden con Git; las capturas son legibles; no se atribuye trabajo a quien no lo hizo; los enlaces abren con los permisos previstos.

### P2. Contenido y manejo del repositorio - 10 puntos

**Responsable:** David.

1. Detener cambios funcionales salvo correcciones bloqueantes.
2. Confirmar que `develop` contiene el Release entregable.
3. Integrar `develop` a `main` conservando el historial; no hacer push forzado ni squash total.
4. Crear una etiqueta final estable, por ejemplo `v1.0.0-final`, sobre el mismo commit que se empaqueta.
5. Publicar `main`, la etiqueta y las ramas de integrantes.
6. Mantener `README.md` con instrucciones reales de ejecucion y controles.
7. Versionar codigo fuente, scripts, documentos y evidencias pequenas; mantener modelos/dependencias pesadas en el paquete externo documentado.
8. Verificar que no haya secretos, rutas personales, archivos temporales ni binarios accidentales en Git.
9. Capturar `git status` limpio, ramas remotas, etiqueta final y hash del commit entregado.

**Aceptacion:** el hash de Git, el hash registrado en los manuales y el ejecutable del ZIP corresponden a la misma version; el repo muestra contribucion y evolucion, no un unico volcado final.

### P3. Propuesta corregida - 10 puntos

**Responsables:** equipo; David ensambla.

Usar el PDF existente como base inmutable y generar `Propuesta_Corregida_Final.pdf`. Revisar:

1. Personajes/universos, avatar y nombres de integrantes.
2. Descripcion final de los dos planetas, espacio, pista, plataforma, estacion y evolucion de especies.
3. Cuatro recorridos, controles, iluminacion y seis animaciones obligatorias.
4. Reparto real del trabajo, sustituyendo asignaciones preliminares que cambiaron.
5. Tabla breve `Propuesto / Implementado / Cambio y justificacion`.
6. Comentario sobre las correcciones, conclusiones individuales, conclusion grupal y bibliografia.
7. Referencia explicita al plano final.

**Aceptacion:** no promete elementos ausentes ni describe controles de una demo antigua; conserva las correcciones y se puede contrastar con el Release.

### P4. Plano/mapa final modificado - 10 puntos

**Responsable:** Jonhatan prepara; el equipo valida.

Crear `Plano_Mapa_Final.pdf`, aunque tambien aparezca como anexo de la propuesta:

1. Vista general del espacio y una vista de cada superficie explorable.
2. Leyenda consistente para planetas, pista, estacion, plataforma/cohete, cuerpos celestes, puntos de entrada/salida, NPC y luminarias.
3. Rutas R1-R4 y posiciones de camara/observacion relevantes.
4. Identificacion de elementos que cambiaron frente al boceto inicial.
5. Pie de figura, norte/ejes o referencia de orientacion y autores.

**Aceptacion:** el mapa representa lo que aparece en el Release final; los simbolos se entienden sin explicacion oral; queda claro por que es una version modificada.

### D1. Ejecutable - 10 puntos

**Responsable:** David ejecuta; Jonhatan o Venancio realiza la prueba independiente.

1. Construir Release x64 desde el commit final.
2. Ejecutar las verificaciones de dependencias, recursos y paquete.
3. Generar ZIP con EXE, DLL, shaders, modelos, texturas y cualquier archivo requerido.
4. Copiar el ZIP a una carpeta limpia o a otra PC sin Visual Studio.
5. Extraer y abrir el EXE sin compilar.
6. Probar inicio, F1-F4, cambios de zona, avatar, carrera, vuelo, observacion, cohete y salida normal.
7. Registrar PC/SO, hash SHA-256 del ZIP, commit/tag, resultado y una captura con fecha.
8. Si falla un recurso, corregir y repetir la prueba completa; no cambiar el paquete despues sin regenerar hash y evidencia.

**Aceptacion:** se entregan los 5 puntos del archivo y se defiende los 5 de funcionamiento con prueba independiente reproducible.

### D2. Manual de usuario - 10 puntos

**Responsable:** Venancio redacta; David valida contra el Release.

Crear `Manual_de_Usuario.pdf` con lenguaje no tecnico:

1. Requisitos minimos y pasos para extraer/ejecutar.
2. Pantalla inicial y objetivo de la experiencia.
3. Tabla completa de teclado y mouse, basada en el Release final.
4. Tutorial de R1, R2, R3 y R4, con una captura propia por recorrido.
5. Como entrar/salir de planetas y activar cohete, puertas, paneles, luces y animaciones.
6. Como reconocer el modo y ubicacion en el HUD.
7. Solucion de problemas: ejecutar desde la carpeta correcta, DLL/recursos faltantes y controles sin foco.
8. Version/tag del programa usado para las capturas.

**Aceptacion:** una persona ajena al codigo puede abrir el programa y completar los cuatro recorridos siguiendo solo el manual.

### D3. Manual tecnico - 10 puntos

**Responsables:** cada integrante redacta su modulo; David integra.

Crear `Manual_Tecnico_Desarrollo.pdf`. Para cada requisito del escenario incluir una ficha:

- requisito y puntaje;
- resultado visible y captura del Release final;
- archivos/clases responsables;
- fragmento corto y legible de codigo;
- algoritmo o decisiones de implementacion;
- control o forma de comprobarlo;
- autor y evidencia Git.

Orden minimo: geometria y jerarquia; ambientacion/NPC/planetas; texturas/materiales; avatar y caminata; cuatro recorridos/camaras; iluminacion/skyboxes/spots; tres animaciones por teclado; tres animaciones continuas; integracion visual. Cerrar con arquitectura, proceso de build, pruebas, problemas resueltos, conclusiones individuales, conclusion grupal y bibliografia.

**Aceptacion:** todas las fichas usan el mismo Release final; no se llenan paginas con codigo completo; las capturas y fragmentos demuestran como se cubrio cada punto.

### D4. Bitacoras individuales - 10 puntos

**Responsable:** cada integrante firma y entrega su propia bitacora.

Cada fecha de trabajo debe indicar:

1. que me propuse hacer;
2. que hice realmente;
3. cuanto tiempo real trabaje;
4. evidencia: commit, captura, archivo o video;
5. resultado, pendiente o bloqueo.

Acciones particulares:

- **David:** reemplazar `por confirmar`, completar 1-7 nov, agregar conclusion y marcar la entrega real.
- **Jonhatan:** completar la plantilla con sus cuatro commits, entregas de mundo/recursos y evidencia disponible. Si no recuerda una duracion, escribir `no registrada`; nunca inventarla.
- **Venancio:** actualizar 3-7 nov con la integracion final del avatar/HUD y cerrar los pendientes que ya se integraron; agregar conclusion.

**Aceptacion:** las tres bitacoras son coherentes con Git y no presentan planes futuros como hechos. Exportar un PDF por alumno.

### D5. Video demo - 20 puntos

**Responsable:** Jonhatan opera; David controla la lista; Venancio edita.

Grabar el Release final, **sin narracion de voz**, en 1080p si el equipo lo permite. Usar rotulos breves. Guion recomendado:

1. Titulo, integrantes, version y controles esenciales.
2. Vista general del espacio: estacion, pista, plataforma, planetas y cuerpos celestes.
3. R1: avatar no humanoide, jerarquia, textura/material, caminata y cada planeta.
4. Evolucion diferenciada, NPC, ambientacion y texturas de los universos.
5. R2: nave del usuario y otras naves corriendo el circuito.
6. R3: vuelo entre planetas.
7. R4: cohete, naves entrando/saliendo y objetos celestes.
8. Cambio correcto de camaras.
9. Ciclo dia/noche, luces puntuales sincronizadas, skyboxes y spots por teclado.
10. Las tres animaciones por teclado, identificadas una por una.
11. Las tres animaciones continuas, identificadas una por una.
12. Cierre con creditos y version.

**Aceptacion:** el archivo abre, no tiene narracion, muestra el contenido completo y evita pantallas de carga, escritorio o errores. Revisarlo de principio a fin antes de subirlo.

### D6. Videoblog - 20 puntos

**Responsables:** participan David, Jonhatan y Venancio; una persona ensambla.

Guion de 4-7 minutos:

1. Presentacion del equipo y objetivo.
2. **Planeacion:** propuesta, plano, reparto, ramas y almacenamiento.
3. **Implementacion:** cada integrante explica que construyo y muestra una evidencia concreta.
4. **Ejecucion:** demostrar brevemente el Release y la prueba fuera del entorno de desarrollo.
5. Problema tecnico principal, solucion y aprendizaje.
6. Experiencia de colaboracion y conclusion de cada integrante.
7. Cierre grupal.

**Aceptacion:** aparecen o se escuchan los tres integrantes; no es una segunda demo muda; explica las tres fases exigidas y coincide con bitacoras/repositorio.

## 4. Orden de ejecucion para no perder puntos

Hacer trabajo en paralelo y en este orden critico:

| Prioridad | David | Jonhatan | Venancio | Salida |
|---:|---|---|---|---|
| 1 | Congelar commit y generar Release | Probar ZIP limpio/otra PC | Verificar controles y HUD | Release final + acta de prueba |
| 2 | Documento Git y capturas | Plano final | Manual de usuario | Tres PDF listos |
| 3 | Secciones motor/camaras/manual tecnico | Secciones mundo y capturas | Secciones avatar/HUD | Manual tecnico integrado |
| 4 | Cerrar bitacora propia | Cerrar bitacora propia | Cerrar bitacora propia | Tres bitacoras PDF |
| 5 | Lista de tomas y control de rubrica | Operar demo | Grabar/editar demo | Video demo revisado |
| 6 | Grabar intervencion | Grabar intervencion | Grabar intervencion/editar | Videoblog revisado |
| 7 | Integrar `main`, etiqueta y hashes | Revisar nombres/archivos | Revisar enlaces/archivos | Paquete de entrega |
| 8 | Subir y comprobar Classroom | Verificar archivos descargados | Guardar acuse | Entrega confirmada |

No dedicar tiempo a extras visuales mientras falte cualquiera de los artefactos de 10 o 20 puntos anteriores.

## 5. Estructura de entrega recomendada

```text
CGICH_Proyecto_Final_Equipo/
  01_Ejecutable/
    CGICH-v1.0.0-final-win64.zip
    SHA256.txt
  02_Planeacion/
    Documento_Repositorio_Colaboracion.pdf
    Propuesta_Corregida_Final.pdf
    Plano_Mapa_Final.pdf
  03_Manuales/
    Manual_de_Usuario.pdf
    Manual_Tecnico_Desarrollo.pdf
  04_Bitacoras/
    Bitacora_David.pdf
    Bitacora_Jonhatan.pdf
    Bitacora_Venancio.pdf
  05_Videos/
    Video_Demo.mp4
    Videoblog.mp4
  06_Evidencia/
    Prueba_Release_Limpio.pdf
    URL_Repositorio.txt
    Acuse_Classroom.pdf
```

Si Classroom tiene limite de tamano, subir videos/ZIP al medio permitido y colocar enlaces con permisos probados. No sustituir los documentos con una carpeta de capturas sueltas.

## 6. Control final 120/120

- [ ] Documento Git/colaboracion entregado y legible - 10.
- [ ] Repositorio final publicado, con ramas, `main` y etiqueta - 10.
- [ ] Propuesta corregida final coherente con el Release - 10.
- [ ] Plano final modificado entregado como evidencia identificable - 10.
- [ ] EXE/ZIP entregado - 5.
- [ ] EXE probado sin compilar en entorno limpio - 5.
- [ ] Manual de usuario validado por otra persona - 10.
- [ ] Manual tecnico cubre todos los requisitos, conclusiones y bibliografia - 10.
- [ ] Tres bitacoras individuales completas y honestas - 10.
- [ ] Video demo entregado - 10.
- [ ] Video demo muestra toda la funcionalidad - 10.
- [ ] Videoblog entregado - 10.
- [ ] Videoblog explica planeacion, implementacion y ejecucion - 10.
- [ ] Todos los archivos abren despues de descargarlos.
- [ ] Version, commit, etiqueta y hash coinciden en ZIP, manuales y videos.
- [ ] Se guardo evidencia/acuse de la entrega antes del limite.

**Cierre:** solo declarar 120/120 cuando las trece partidas de puntaje esten comprobadas en el paquete final y en Classroom.
