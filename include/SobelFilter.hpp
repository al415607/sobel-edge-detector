#pragma once

#include <opencv2/core.hpp>

class SobelFilter
{
public:
    static cv::Mat applySequential(const cv::Mat& input);
    static cv::Mat applyParallel(const cv::Mat& input);
};