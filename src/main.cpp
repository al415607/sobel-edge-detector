#include <iostream>

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
        const cv::Mat edges = SobelFilter::apply(image);

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