# Publicación de base compartida — 30-oct-2026

origin: https://github.com/Cuervy117/CG_proyecto.git. Se conservan historial remoto/local y rama remota david. Las cinco ramas parten de la revisión compartida. refactor/src-layout conserva su referencia anterior; no hace falta publicarla. v0.1-base no se recrea.

```powershell
git status --short
git fetch origin
git push --dry-run --atomic origin main develop feat/a-motor feat/b-mundo feat/c-avatar refs/tags/v0.1-base
git push --atomic -u origin main develop feat/a-motor feat/b-mundo feat/c-avatar refs/tags/v0.1-base
```

Si hay cambios remotos nuevos, incorporarlos y revisar: nunca --force. Si la protección de main exige revisión, publicar develop/feat/* y usar ese flujo.

Tras publicar, compañeros clonan/cambian de rama siguiendo README. Además necesitan ZIP/hash de dependencias: Git no distribuye Models/Textures/include/lib/glm/DLLs. Un paquete local no implica distribución por Drive ni aprobación de contenido final.

Esta revisión no implementa SceneNode/cámaras/avatar ni las correcciones de motor; son los hitos de §6.
