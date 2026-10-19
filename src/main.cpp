#include <iostream>
#include <chrono>
#include <stdexcept>

#include <opencv2/opencv.hpp>

#include "SobelFilter.hpp"

int main(int argc, char* argv[])
{
    // Se comprueba el numero de argumentos antes de realizar cualquier operacion
    if (argc < 3 || argc > 4)
    {
        std::cerr << "Uso: sobel <imagen_entrada> <imagen_salida> [sequential|parallel]"
                  << std::endl;
        return -1;
    }

    const std::string inputPath = argv[1];
    const std::string outputPath = argv[2];

    // Si no se indica modo, se usa la version paralela por defecto
    const std::string modeArgument =
        (argc == 4) ? argv[3] : "parallel";

    ExecutionMode mode;

    // Se valida el modo antes de cargar o procesar la imagen
    if (modeArgument == "sequential")
    {
        mode = ExecutionMode::Sequential;
    }
    else if (modeArgument == "parallel")
    {
        mode = ExecutionMode::Parallel;
    }
    else
    {
        std::cerr << "Error: modo no valido. "
                  << "Usa sequential o parallel."
                  << std::endl;

        return -1;
    }

    cv::Mat image;

    try
    {
        image = cv::imread(
            inputPath,
            cv::IMREAD_UNCHANGED
        );
    }
    catch (const cv::Exception& exception)
    {
        std::cerr << "Error de OpenCV al cargar la imagen: "
                  << exception.what()
                  << std::endl;

        return -1;
    }

    if (image.empty())
    {
        std::cerr << "Error: no se pudo cargar la imagen"
                  << std::endl;

        return -1;
    }

    std::cout << "Imagen cargada correctamente"
              << std::endl;

    std::cout << "Resolucion: "
              << image.cols << " x " << image.rows
              << std::endl;

    std::cout << "Modo: "
              << modeArgument
              << std::endl;

    SobelFilter filter;
    FilterResult result;

    const auto totalStart =
        std::chrono::high_resolution_clock::now();

    try
    {
        result = filter.apply(image, mode);
    }
    catch (const std::invalid_argument& exception)
    {
        std::cerr << "Error de entrada en el filtro: "
                  << exception.what()
                  << std::endl;

        return -1;
    }
    catch (const cv::Exception& exception)
    {
        std::cerr << "Error de OpenCV durante el procesamiento: "
                  << exception.what()
                  << std::endl;

        return -1;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Error inesperado durante el procesamiento: "
                  << exception.what()
                  << std::endl;

        return -1;
    }

    const auto totalEnd =
        std::chrono::high_resolution_clock::now();

    const auto totalTime =
        std::chrono::duration_cast<std::chrono::microseconds>(
            totalEnd - totalStart
        );

    std::cout << "Tiempo Sobel: "
              << result.processingTime.count() / 1000.0
              << " ms"
              << std::endl;

    std::cout << "Tiempo total del filtro: "
              << totalTime.count() / 1000.0
              << " ms"
              << std::endl;

    try
    {
        if (!cv::imwrite(outputPath, result.image))
        {
            std::cerr << "Error: no se pudo guardar la imagen"
                      << std::endl;

            return -1;
        }
    }
    catch (const cv::Exception& exception)
    {
        std::cerr << "Error de OpenCV al guardar la imagen: "
                  << exception.what()
                  << std::endl;

        return -1;
    }

    std::cout << "Resultado guardado en: "
              << outputPath
              << std::endl;

    return 0;
}