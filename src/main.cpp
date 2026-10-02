#include <iostream>
#include <chrono>

#include <opencv2/opencv.hpp>

#include "SobelFilter.hpp"

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Uso: sobel <imagen_entrada> <imagen_salida> [sequential|parallel]"
                  << std::endl;
        return 1;
    }

    const std::string inputPath = argv[1];
    const std::string outputPath = argv[2];

    // Si no se indica modo, se usa la version paralela por defecto
    std::string mode = "parallel";

    if (argc >= 4)
    {
        mode = argv[3];
    }

    // OpenCV para leer la imagen
    cv::Mat image = cv::imread(inputPath, cv::IMREAD_COLOR);

    if (image.empty())
    {
        std::cerr << "Error: no se pudo cargar la imagen"
                  << std::endl;
        return 1;
    }

    std::cout << "Imagen cargada correctamente" << std::endl;
    std::cout << "Resolucion: "
              << image.cols << " x " << image.rows
              << std::endl;

    std::cout << "Modo: "
              << mode
              << std::endl;

    try
    {
        cv::Mat edges;

        // Se mide solo el tiempo del filtro Sobel,
        // sin contar la lectura ni la escritura de la imagen
        const auto start =
            std::chrono::high_resolution_clock::now();

        if (mode == "sequential")
        {
            edges = SobelFilter::applySequential(image);
        }
        else if (mode == "parallel")
        {
            edges = SobelFilter::applyParallel(image);
        }
        else
        {
            std::cerr << "Error: modo no valido. "
                      << "Usa sequential o parallel."
                      << std::endl;
            return 1;
        }

        const auto end =
            std::chrono::high_resolution_clock::now();

        const auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                end - start
            );

        std::cout << "Tiempo de procesamiento: "
                  << duration.count()
                  << " ms"
                  << std::endl;

        if (!cv::imwrite(outputPath, edges))
        {
            std::cerr << "Error: no se pudo guardar la imagen"
                      << std::endl;
            return 1;
        }
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Error: "
                  << exception.what()
                  << std::endl;

        return 1;
    }

    std::cout << "Resultado guardado en: "
              << outputPath
              << std::endl;

    return 0;
}