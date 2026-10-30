# Capturas de la Práctica 09

Las siete imágenes finales están en:

`C:\Users\David\Documents\2027-1\CGICH\capturas_p09`

Fueron capturadas del área del cliente, a `1366x749`, sin barra de título ni
bordes. Las capturas anteriores de loops, HUD y baliza fueron reemplazadas.

## Lista final

| # | Archivo | Qué muestra |
|---|---|---|
| 1 | `01_vista_general.png` | Vista general frontal elevada: planeta, pista cuadrada con esquinas redondeadas, guía cian, guía magenta del satélite, nave y satélite. |
| 2 | `02_vista_diagonal.png` | Vista general en diagonal: la pista cuadrada alrededor del planeta, la nave y el satélite con su moño. |
| 3 | `03_vista_alejada.png` | Vista general más alejada para mostrar el conjunto completo y el contexto de la escena. |
| 4 | `04_satelite_trayectoria.png` | Vista cenital: pista cian, trayectoria magenta completa en forma de moño, satélite blanco/azul y nave sobre la pista. |
| 5 | `05_satelite_closeup.png` | Acercamiento al satélite desde arriba: base blanca, paneles azules, guía magenta y guía cian. |
| 6 | `06_pista_lateral.png` | Acercamiento lateral de la nave sobre la pista: nave, llamas, guía cian y planeta de fondo. |
| 7 | `07_pista_descenso.png` | Acercamiento de la nave en la curva de descenso: nave, guía cian descendente y pendiente visible. |

## Cámaras utilizadas

Las cámaras siguientes se agregaron de forma temporal al código para tomar las
capturas. El bloque de presets ya se retiró y el ejecutable final conserva
únicamente la cámara libre.

| Captura | Cámara | Yaw | Pitch |
|---|---|---|---|
| 1 | `(0, 65, -95)` mirando a `(0, 6, 0)` | `90` | `-31.8` |
| 2 | `(55, 60, -55)` mirando a `(0, 6, 0)` | `135` | `-34.7` |
| 3 | `(0, 80, -125)` mirando a `(0, 6, 0)` | `90` | `-30.6` |
| 4 | `(0, 75, 8)` mirando a `(0, 26, 0)` | `-90` | `-80.7` |

Las tomas 5, 6 y 7 usaron cámaras de seguimiento temporales:

| Captura | Objetivo | Desplazamiento de cámara |
|---|---|---|
| 5 | Satélite | `(8, 14, -13)` respecto al satélite, mirando `+1.5` en Y |
| 6 | Nave | `(12, 5, -8)` respecto a la nave, mirando `+1.3` en Y |
| 7 | Nave | `(14, 8, 14)` respecto a la nave, mirando `+1.3` en Y |

## Script de captura

Se usó `capturar_p09.ps1`, que localiza la ventana `Practica 09: Animacion`,
captura solo el área del cliente y permite ráfagas para elegir la mejor pose.

Ejemplos de las ráfagas utilizadas:

```powershell
pwsh -File .\capturar_p09.ps1 -Prefix 01_vista_general -Count 8 -IntervalMs 300 -DelayMs 700
pwsh -File .\capturar_p09.ps1 -Prefix 02_vista_diagonal -Count 8 -IntervalMs 300 -DelayMs 700
pwsh -File .\capturar_p09.ps1 -Prefix 03_vista_alejada -Count 6 -IntervalMs 350 -DelayMs 700
pwsh -File .\capturar_p09.ps1 -Prefix 04_satelite_trayectoria -Count 15 -IntervalMs 250 -DelayMs 700
pwsh -File .\capturar_p09.ps1 -Prefix 05_satelite_closeup -Count 15 -IntervalMs 250 -DelayMs 700
pwsh -File .\capturar_p09.ps1 -Prefix 06_nave_lateral -Count 45 -IntervalMs 500 -DelayMs 600
pwsh -File .\capturar_p09.ps1 -Prefix 07_nave_descenso -Count 45 -IntervalMs 500 -DelayMs 600
```

Para las tomas 6 y 7 se recorrió una vuelta completa de la nave y se eligió el
cuadro donde la nave está sobre la subida o el descenso, respectivamente.

## Validación

- [x] Son siete archivos PNG y muestran solo el área de juego.
- [x] No aparece el cursor, una consola ni avisos del sistema.
- [x] La pista cian se ve completa en las vistas generales y cenital.
- [x] La guía magenta del satélite muestra los dos lóbulos.
- [x] El satélite se ve blanco con paneles azules y con iluminación.
- [x] La nave muestra las llamas de los motores en las tomas cercanas.
- [x] Los nombres coinciden con la lista final.
- [x] El bloque de cámaras temporales fue retirado y el proyecto se recompiló.

## Entrega

Las imágenes están listas en `capturas_p09`. Si se autoriza escribir en la
carpeta del reporte, copiarlas con el mismo nombre a:

`C:\Users\David\UNAM\reportes_CG\practica_09\img`
