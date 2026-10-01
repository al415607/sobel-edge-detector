#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    const std::string inputPath = "images/test.jpg";
    const std::string outputPath = "output/test_copia.jpg";

    cv::Mat image = cv::imread(inputPath, cv::IMREAD_COLOR);

    if (image.empty())
    {
        std::cerr << "Error: no se pudo cargar la imagen" << std::endl;
        return 1;
    }

    std::cout << "Imagen cargada correctamente" << std::endl;
    std::cout << "Resolucion: "
              << image.cols << " x " << image.rows
              << std::endl;

    if (!cv::imwrite(outputPath, image))
    {
        std::cerr << "Error: no se pudo guardar la imagen" << std::endl;
        return 1;
    }

    std::cout << "Imagen guardada en: "
              << outputPath
              << std::endl;

    return 0;
}