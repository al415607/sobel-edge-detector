#include <iostream>
#include <chrono>

#include <opencv2/opencv.hpp>

#include "SobelFilter.hpp"

int main()
{
    const std::string inputPath = "images/test.jpg";
    const std::string outputPath = "output/sobel.jpg";

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

    try
    {
        // Se mide solo el tiempo del filtro Sobel,
        // sin contar la lectura ni la escritura de la imagen
        const auto start = std::chrono::high_resolution_clock::now();

        const cv::Mat edges = SobelFilter::apply(image);

        const auto end = std::chrono::high_resolution_clock::now();

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