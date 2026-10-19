#include "SobelFilter.hpp"
#include "SobelCore.hpp"

#include <stdexcept>
#include <chrono>

#include <opencv2/imgproc.hpp>

namespace
{
    constexpr int SOBEL_KERNEL_SIZE = 3;
    constexpr int SOBEL_KERNEL_RADIUS =
        SOBEL_KERNEL_SIZE / 2;
}

FilterResult SobelFilter::apply(
    const cv::Mat& input,
    ExecutionMode mode
) const
{
    if (input.empty())
    {
        throw std::invalid_argument(
            "La imagen de entrada esta vacia"
        );
    }

    if (input.depth() != CV_8U)
    {
        throw std::invalid_argument(
            "Solo se admiten imagenes de 8 bits por canal"
        );
    }

    if (
    input.rows < SOBEL_KERNEL_SIZE ||
    input.cols < SOBEL_KERNEL_SIZE
    )
    {
        throw std::invalid_argument(
            "La imagen es demasiado pequena para el kernel Sobel"
        );
    }

    cv::Mat gray;

    // El filtro Sobel trabaja sobre intensidades, por eso se convierten
    // las imagenes en color a escala de grises antes de procesarlas
    switch (input.channels())
    {
        case 1:
            gray = input;
            break;

        case 3:
            cv::cvtColor(
                input,
                gray,
                cv::COLOR_BGR2GRAY
            );
            break;

        case 4:
            cv::cvtColor(
                input,
                gray,
                cv::COLOR_BGRA2GRAY
            );
            break;

        default:
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

    const auto start =
        std::chrono::high_resolution_clock::now();

    switch (mode)
    {
        case ExecutionMode::Sequential:
        {
            for (
                int y = SOBEL_KERNEL_RADIUS;
                y < gray.rows - SOBEL_KERNEL_RADIUS;
                ++y
            )
            {
                processRow(gray, edges, y);
            }

            break;
        }

        case ExecutionMode::Parallel:
        {
            // Cada iteracion procesa una fila distinta de la imagen de salida
            // La imagen de entrada solo se lee por lo que las filas se pueden
            // repartir entre varios hilos de forma independiente
            #pragma omp parallel for
            for (
                int y = SOBEL_KERNEL_RADIUS;
                y < gray.rows - SOBEL_KERNEL_RADIUS;
                ++y
            )
            {
                processRow(gray, edges, y);
            }

            break;
        }
    }

    const auto end =
        std::chrono::high_resolution_clock::now();

    const auto processingTime =
        std::chrono::duration_cast<std::chrono::microseconds>(
            end - start
        );

    // Se mantiene el mismo numero de canales que la imagen de entrada
    switch (input.channels())
    {
        case 1:
            return {edges, processingTime};

        case 3:
        {
            cv::Mat colorEdges;

            cv::cvtColor(
                edges,
                colorEdges,
                cv::COLOR_GRAY2BGR
            );

            return {colorEdges, processingTime};
        }

        case 4:
        {
            cv::Mat colorEdges;

            cv::cvtColor(
                edges,
                colorEdges,
                cv::COLOR_GRAY2BGRA
            );

            return {colorEdges, processingTime};
        }
    }

    throw std::runtime_error(
        "Error inesperado al generar la imagen de salida"
    );
}

void SobelFilter::processRow(
    const cv::Mat& gray,
    cv::Mat& edges,
    int y
)
{
    const std::uint8_t* previousRow =
        gray.ptr<std::uint8_t>(y - 1);

    const std::uint8_t* currentRow =
        gray.ptr<std::uint8_t>(y);

    const std::uint8_t* nextRow =
        gray.ptr<std::uint8_t>(y + 1);

    std::uint8_t* outputRow =
        edges.ptr<std::uint8_t>(y);

    SobelCore::processRow(
        previousRow,
        currentRow,
        nextRow,
        outputRow,
        gray.cols
    );
}