#include "SobelCore.hpp"

#include <algorithm>
#include <cmath>

void SobelCore::processRow(
    const std::uint8_t* previousRow,
    const std::uint8_t* currentRow,
    const std::uint8_t* nextRow,
    std::uint8_t* outputRow,
    int width
)
{
    for (
        int x = KERNEL_RADIUS;
        x < width - KERNEL_RADIUS;
        ++x
    )
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

        const float magnitude =
            std::sqrt(
                static_cast<float>(
                    gx * gx + gy * gy
                )
            );

        outputRow[x] =
            static_cast<std::uint8_t>(
                std::min(magnitude, 255.0f)
            );
    }
}