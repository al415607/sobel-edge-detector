#pragma once

#include "Filter.hpp"

class SobelFilter : public Filter
{
public:
    FilterResult apply(
        const cv::Mat& input,
        ExecutionMode mode
    ) const override;

private:
    static void processRow(
        const cv::Mat& gray,
        cv::Mat& edges,
        int y
    );
};