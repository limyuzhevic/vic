#pragma once

/**
 * Performance Metrics Calculator
 * 
 * Utilities for analyzing performance metrics and detecting anomalies
 */

#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include "PerformanceMonitoring.hpp"

namespace nlm {

/**
 * Performance metrics calculator and analyzer
 */
class PerformanceAnalyzer {
public:
    static double calculateAverageFiringRate(const std::vector<PerformanceMetrics>& metrics);
    static double calculateCpuUsage(const std::vector<PerformanceMetrics>& metrics);
    static double calculateMemoryUsage(const std::vector<PerformanceMetrics>& metrics);
    static std::vector<double> calculateMovingAverage(const std::vector<double>& data, uint32_t window_size);
    static uint64_t findMaxValue(const std::vector<PerformanceMetrics>& metrics, 
                                double (PerformanceMetrics::* getter)() const);
    static uint64_t findMinValue(const std::vector<PerformanceMetrics>& metrics,
                                double (PerformanceMetrics::* getter)() const);
    static void detectAnomalies(const std::vector<PerformanceMetrics>& metrics,
                               std::vector<PerformanceAlert>& alerts);
};

} // namespace nlm