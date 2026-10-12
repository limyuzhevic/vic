#include "TimeSeriesVisualizer.hpp"
#include <iostream>

namespace nlm {

struct TimeSeriesVisualizer::Impl {
    std::shared_ptr<Brain> brain;
    bool visible;
    int width;
    int height;
    std::vector<std::string> timeSeriesNames;
    std::vector<std::vector<float>> timeSeriesData;
    std::map<std::string, std::pair<float, float>> yAxisRanges;
    bool showGrid;
    bool showLegend;
    bool smoothLines;
    double timeWindow;
    double samplingRate;
    bool autoScroll;
    
    Impl() : visible(false), width(800), height(600), showGrid(true), showLegend(true),
             smoothLines(true), timeWindow(1000.0), samplingRate(30.0), autoScroll(true) {}
};

TimeSeriesVisualizer::TimeSeriesVisualizer() : pImpl(std::make_unique<Impl>()) {}

bool TimeSeriesVisualizer::initialize(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    pImpl->brain = brain;
    pImpl->visible = true;
    return true;
}

void TimeSeriesVisualizer::update() {
    // Update time series data from brain
}

void TimeSeriesVisualizer::render() {
    if (!pImpl->visible) return;
    
    std::cout << "Rendering Time Series Visualization:" << std::endl;
    std::cout << "  Time series count: " << pImpl->timeSeriesNames.size() << std::endl;
    std::cout << "  Time window: " << pImpl->timeWindow << " ms" << std::endl;
    std::cout << "  Sampling rate: " << pImpl->samplingRate << " Hz" << std::endl;
    std::cout << "  Auto scroll: " << (pImpl->autoScroll ? "yes" : "no") << std::endl;
    std::cout << "  Grid shown: " << (pImpl->showGrid ? "yes" : "no") << std::endl;
    std::cout << "  Legend shown: " << (pImpl->showLegend ? "yes" : "no") << std::endl;
    std::cout << "  Lines smooth: " << (pImpl->smoothLines ? "yes" : "no") << std::endl;
}

void TimeSeriesVisualizer::setVisible(bool visible) {
    pImpl->visible = visible;
}

bool TimeSeriesVisualizer::isVisible() const {
    return pImpl->visible;
}

void TimeSeriesVisualizer::setSize(int width, int height) {
    pImpl->width = width;
    pImpl->height = height;
}

void TimeSeriesVisualizer::addTimeSeries(const std::string& name, const std::vector<float>& data) {
    pImpl->timeSeriesNames.push_back(name);
    pImpl->timeSeriesData.push_back(data);
}

void TimeSeriesVisualizer::removeTimeSeries(const std::string& name) {
    for (size_t i = 0; i < pImpl->timeSeriesNames.size(); ++i) {
        if (pImpl->timeSeriesNames[i] == name) {
            pImpl->timeSeriesNames.erase(pImpl->timeSeriesNames.begin() + i);
            pImpl->timeSeriesData.erase(pImpl->timeSeriesData.begin() + i);
            break;
        }
    }
}

void TimeSeriesVisualizer::clearTimeSeries() {
    pImpl->timeSeriesNames.clear();
    pImpl->timeSeriesData.clear();
}

void TimeSeriesVisualizer::setYAxis(const std::string& name, float min, float max) {
    pImpl->yAxisRanges[name] = std::make_pair(min, max);
}

void TimeSeriesVisualizer::setShowGrid(bool show) {
    pImpl->showGrid = show;
}

void TimeSeriesVisualizer::setShowLegend(bool show) {
    pImpl->showLegend = show;
}

void TimeSeriesVisualizer::setSmoothLines(bool smooth) {
    pImpl->smoothLines = smooth;
}

void TimeSeriesVisualizer::setTimeWindow(double window) {
    pImpl->timeWindow = window;
}

void TimeSeriesVisualizer::setSamplingRate(double rate) {
    pImpl->samplingRate = rate;
}

void TimeSeriesVisualizer::setAutoScroll(bool enable) {
    pImpl->autoScroll = enable;
}

} // namespace nlm