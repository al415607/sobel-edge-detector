#include "SobelFilter.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <opencv2/imgproc.hpp>

cv::Mat SobelFilter::applySequential(const cv::Mat& input)
{
    if (input.empty())
    {
        throw std::invalid_argument("La imagen de entrada esta vacia");
    }

    cv::Mat gray;

    // El filtro Sobel trabaja sobre intensidades, por eso se convierten
    // las imagenes en color a escala de grises antes de procesarlas
    if (input.channels() == 3)
    {
        cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    }
    else if (input.channels() == 1)
    {
        gray = input;
    }
    else
    {
        throw std::invalid_argument(
            "Numero de canales de imagen no soportado"
        );
    }

    // La salida tiene el mismo tamaño que la entrada
    // Se inicializa a cero para que los bordes exteriores queden negros
    cv::Mat edges = cv::Mat::zeros(
        gray.size(),
        CV_8UC1
    );

    for (int y = 1; y < gray.rows - 1; ++y)
    {
        // Se accede directamente a las filas necesarias para evitar llamadas
        // repetidas a at() en cada pixel
        const unsigned char* previousRow = gray.ptr<unsigned char>(y - 1);
        const unsigned char* currentRow = gray.ptr<unsigned char>(y);
        const unsigned char* nextRow = gray.ptr<unsigned char>(y + 1);

        unsigned char* outputRow = edges.ptr<unsigned char>(y);

        for (int x = 1; x < gray.cols - 1; ++x)
        {
            const int gx =
                -previousRow[x - 1]
                + previousRow[x + 1]
                - 2 * currentRow[x - 1]
                + 2 * currentRow[x + 1]
                - nextRow[x - 1]
                + nextRow[x + 1];

            const int gy =
                -previousRow[x - 1]
                - 2 * previousRow[x]
                - previousRow[x + 1]
                + nextRow[x - 1]
                + 2 * nextRow[x]
                + nextRow[x + 1];

            const double magnitude =
                std::sqrt(
                    static_cast<double>(gx * gx + gy * gy)
                );

            outputRow[x] =
                static_cast<unsigned char>(
                    std::min(magnitude, 255.0)
                );
        }
    }

    // Se mantiene el mismo numero de canales que la imagen de entrada
    if (input.channels() == 3)
    {
        cv::Mat colorEdges;
        cv::cvtColor(edges, colorEdges, cv::COLOR_GRAY2BGR);
        return colorEdges;
    }

    return edges;
}

cv::Mat SobelFilter::applyParallel(const cv::Mat& input)
{
    if (input.empty())
    {
        throw std::invalid_argument("La imagen de entrada esta vacia");
    }

    cv::Mat gray;

    if (input.channels() == 3)
    {
        cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    }
    else if (input.channels() == 1)
    {
        gray = input;
    }
    else
    {
        throw std::invalid_argument(
            "Numero de canales de imagen no soportado"
        );
    }

    cv::Mat edges = cv::Mat::zeros(
        gray.size(),
        CV_8UC1
    );

    // Cada iteracion procesa una fila distinta de la imagen de salida
    // La imagen de entrada solo se lee por lo que las filas se pueden
    // repartir entre varios hilos de forma independiente
    #pragma omp parallel for
    for (int y = 1; y < gray.rows - 1; ++y)
    {
        const unsigned char* previousRow =
            gray.ptr<unsigned char>(y - 1);

        const unsigned char* currentRow =
            gray.ptr<unsigned char>(y);

        const unsigned char* nextRow =
            gray.ptr<unsigned char>(y + 1);

        unsigned char* outputRow =
            edges.ptr<unsigned char>(y);

        for (int x = 1; x < gray.cols - 1; ++x)
        {
            const int gx =
                -previousRow[x - 1]
                + previousRow[x + 1]
                - 2 * currentRow[x - 1]
                + 2 * currentRow[x + 1]
                - nextRow[x - 1]
                + nextRow[x + 1];

            const int gy =
                -previousRow[x - 1]
                - 2 * previousRow[x]
                - previousRow[x + 1]
                + nextRow[x - 1]
                + 2 * nextRow[x]
                + nextRow[x + 1];

            const double magnitude =
                std::sqrt(
                    static_cast<double>(gx * gx + gy * gy)
                );

            outputRow[x] =
                static_cast<unsigned char>(
                    std::min(magnitude, 255.0)
                );
        }
    }

    // Se mantiene el mismo numero de canales que la imagen de entrada
    if (input.channels() == 3)
    {
        cv::Mat colorEdges;
        cv::cvtColor(edges, colorEdges, cv::COLOR_GRAY2BGR);
        return colorEdges;
    }

    return edges;
}