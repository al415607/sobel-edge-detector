#pragma once

#include <cstdint>

namespace SobelCore
{
    constexpr int KERNEL_SIZE = 3;
    constexpr int KERNEL_RADIUS = KERNEL_SIZE / 2;

    void processRow(
        const std::uint8_t* previousRow,
        const std::uint8_t* currentRow,
        const std::uint8_t* nextRow,
        std::uint8_t* outputRow,
        int width
    );
}