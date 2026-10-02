#pragma once

#include <opencv2/core.hpp>

class SobelFilter
{
public:
    static cv::Mat apply(const cv::Mat& input);
};