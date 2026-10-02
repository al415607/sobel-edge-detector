## Uso

El programa recibe una imagen de entrada, una ruta de salida y opcionalmente, el modo de ejecución.

### Modo paralelo

```bash
sobel.exe <imagen_entrada> <imagen_salida> parallel
```

Ejemplo:

```bash
sobel.exe images/test.jpg output/sobel_parallel.jpg parallel
```

### Modo secuencial

```bash
sobel.exe <imagen_entrada> <imagen_salida> sequential
```

Ejemplo:

```bash
sobel.exe images/test.jpg output/sobel_sequential.jpg sequential
```

Si no se indica ningún modo, se utiliza la versión paralela por defecto:

```bash
sobel.exe images/test.jpg output/sobel.jpg
```

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