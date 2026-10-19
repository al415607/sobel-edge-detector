#pragma once

#include <chrono>
#include <opencv2/core.hpp>

enum class ExecutionMode
{
    Sequential,
    Parallel
};

struct FilterResult
{
    cv::Mat image;
    std::chrono::microseconds processingTime;
};

class Filter
{
public:
    virtual FilterResult apply(
        const cv::Mat& input,
        ExecutionMode mode
    ) const = 0;

    virtual ~Filter() = default;
};