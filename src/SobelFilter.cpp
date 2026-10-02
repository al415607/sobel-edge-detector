#include "SobelFilter.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <opencv2/imgproc.hpp>

cv::Mat SobelFilter::apply(const cv::Mat& input)
{
    if (input.empty())
    {
        throw std::invalid_argument("La imagen de entrada esta vacia.");
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
            "Numero de canales de imagen no soportado."
        );
    }

    cv::Mat edges = cv::Mat::zeros(
        gray.size(),
        CV_8UC1
    );

    for (int y = 1; y < gray.rows - 1; ++y)
    {
        for (int x = 1; x < gray.cols - 1; ++x)
        {
            const int gx =
                -gray.at<unsigned char>(y - 1, x - 1)
                + gray.at<unsigned char>(y - 1, x + 1)
                - 2 * gray.at<unsigned char>(y, x - 1)
                + 2 * gray.at<unsigned char>(y, x + 1)
                - gray.at<unsigned char>(y + 1, x - 1)
                + gray.at<unsigned char>(y + 1, x + 1);

            const int gy =
                -gray.at<unsigned char>(y - 1, x - 1)
                - 2 * gray.at<unsigned char>(y - 1, x)
                - gray.at<unsigned char>(y - 1, x + 1)
                + gray.at<unsigned char>(y + 1, x - 1)
                + 2 * gray.at<unsigned char>(y + 1, x)
                + gray.at<unsigned char>(y + 1, x + 1);

            const double magnitude =
                std::sqrt(
                    static_cast<double>(gx * gx + gy * gy)
                );

            edges.at<unsigned char>(y, x) =
                static_cast<unsigned char>(
                    std::min(magnitude, 255.0)
                );
        }
    }

    return edges;
}