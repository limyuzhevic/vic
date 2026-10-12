#include "StatisticalChartVisualizer.hpp"
#include <iostream>
#include <algorithm>

namespace nlm {

struct StatisticalChartVisualizer::Impl {
    std::shared_ptr<Brain> brain;
    bool visible;
    int width;
    int height;
    std::string chartType;
    std::string statisticalMeasure;
    size_t binCount;
    bool showOutliers;
    double confidenceInterval;
    std::string analysisMetric;
    bool groupByRegion;
    float correlationThreshold;
    
    Impl() : visible(false), width(800), height(600), chartType("histogram"), 
             statisticalMeasure("mean"), binCount(10), showOutliers(true),
             confidenceInterval(0.95), analysisMetric("firing_rate"),
             groupByRegion(false), correlationThreshold(0.5f) {}
};

StatisticalChartVisualizer::StatisticalChartVisualizer() : pImpl(std::make_unique<Impl>()) {}

bool StatisticalChartVisualizer::initialize(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    pImpl->brain = brain;
    pImpl->visible = true;
    return true;
}

void StatisticalChartVisualizer::update() {
    // Update statistical data from brain
}

void StatisticalChartVisualizer::render() {
    if (!pImpl->visible) return;
    
    std::cout << "Rendering Statistical Chart Visualization:" << std::endl;
    std::cout << "  Chart type: " << pImpl->chartType << std::endl;
    std::cout << "  Statistical measure: " << pImpl->statisticalMeasure << std::endl;
    std::cout << "  Bin count: " << pImpl->binCount << std::endl;
    std::cout << "  Show outliers: " << (pImpl->showOutliers ? "yes" : "no") << std::endl;
    std::cout << "  Confidence interval: " << pImpl->confidenceInterval << std::endl;
    std::cout << "  Analysis metric: " << pImpl->analysisMetric << std::endl;
    std::cout << "  Group by region: " << (pImpl->groupByRegion ? "yes" : "no") << std::endl;
    std::cout << "  Correlation threshold: " << pImpl->correlationThreshold << std::endl;
}

void StatisticalChartVisualizer::setVisible(bool visible) {
    pImpl->visible = visible;
}

bool StatisticalChartVisualizer::isVisible() const {
    return pImpl->visible;
}

void StatisticalChartVisualizer::setSize(int width, int height) {
    pImpl->width = width;
    pImpl->height = height;
}

void StatisticalChartVisualizer::setChartType(const std::string& type) {
    pImpl->chartType = type;
}

void StatisticalChartVisualizer::setStatisticalMeasure(const std::string& measure) {
    pImpl->statisticalMeasure = measure;
}

void StatisticalChartVisualizer::setBinCount(size_t bins) {
    pImpl->binCount = bins;
}

void StatisticalChartVisualizer::setShowOutliers(bool show) {
    pImpl->showOutliers = show;
}

void StatisticalChartVisualizer::setConfidenceInterval(double interval) {
    pImpl->confidenceInterval = interval;
}

void StatisticalChartVisualizer::setAnalysisMetric(const std::string& metric) {
    pImpl->analysisMetric = metric;
}

void StatisticalChartVisualizer::setGroupByRegion(bool group) {
    pImpl->groupByRegion = group;
}

void StatisticalChartVisualizer::setCorrelationThreshold(float threshold) {
    pImpl->correlationThreshold = threshold;
}

} // namespace nlm