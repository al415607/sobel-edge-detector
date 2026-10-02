## Rendimiento

Pruebas realizadas con una imagen de 4288 x 2848 píxeles.

| Versión | Tiempo aproximado |
|---|---:|
| Debug - implementación inicial | ~1040 ms |
| Release - implementación inicial | ~105 ms |
| Release - acceso optimizado a memoria | ~35 ms |

La versión optimizada utiliza acceso directo a las filas de la imagen mediante
punteros, evitando llamadas repetidas a `cv::Mat::at()` para cada píxel.