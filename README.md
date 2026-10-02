## Rendimiento

Pruebas realizadas con una imagen de 4288 x 2848 píxeles.

| Versión | Tiempo aproximado |
|---|---:|
| Debug - implementación inicial | ~1040 ms |
| Release - implementación inicial | ~105 ms |
| Release - acceso optimizado a memoria | ~35 ms |
| Release - OpenMP | ~20 ms |

La versión optimizada evita accesos repetidos mediante `cv::Mat::at()` y utiliza
punteros a filas para recorrer la imagen de forma más eficiente.

La versión paralela reparte las filas de la imagen entre varios hilos mediante
OpenMP. Para la imagen utilizada, el tiempo de procesamiento pasa de unos 35 ms a unos
20 ms respecto a la versión secuencial optimizada.