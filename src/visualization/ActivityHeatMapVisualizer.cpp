#include "ActivityHeatMapVisualizer.hpp"
#include <iostream>
#include <algorithm>

namespace nlm {

struct ActivityHeatMapVisualizer::Impl {
    std::shared_ptr<Brain> brain;
    bool visible;
    int width;
    int height;
    std::string heatMapType;
    std::vector<std::string> colorGradient;
    float opacity;
    float threshold;
    bool normalize;
    double timeWindow;
    size_t historicalDataSize;
    
    Impl() : visible(false), width(800), height(600), heatMapType("spike_rate"),
             colorGradient({"blue", "green", "yellow", "red"}), opacity(0.8f), threshold(0.0f),
             normalize(true), timeWindow(1000.0), historicalDataSize(1000) {}
};

ActivityHeatMapVisualizer::ActivityHeatMapVisualizer() : pImpl(std::make_unique<Impl>()) {}

bool ActivityHeatMapVisualizer::initialize(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    pImpl->brain = brain;
    pImpl->visible = true;
    return true;
}

void ActivityHeatMapVisualizer::update() {
    // Update heat map data from brain
}

void ActivityHeatMapVisualizer::render() {
    if (!pImpl->visible) return;
    
    std::cout << "Rendering Activity Heat Map Visualization:" << std::endl;
    std::cout << "  Heat map type: " << pImpl->heatMapType << std::endl;
    std::cout << "  Opacity: " << pImpl->opacity << std::endl;
    std::cout << "  Threshold: " << pImpl->threshold << std::endl;
    std::cout << "  Normalize: " << (pImpl->normalize ? "yes" : "no") << std::endl;
    std::cout << "  Time window: " << pImpl->timeWindow << " ms" << std::endl;
    std::cout << "  Historical data size: " << pImpl->historicalDataSize << std::endl;
}

void ActivityHeatMapVisualizer::setVisible(bool visible) {
    pImpl->visible = visible;
}

bool ActivityHeatMapVisualizer::isVisible() const {
    return pImpl->visible;
}

void ActivityHeatMapVisualizer::setSize(int width, int height) {
    pImpl->width = width;
    pImpl->height = height;
}

void ActivityHeatMapVisualizer::setHeatMapType(const std::string& type) {
    pImpl->heatMapType = type;
}

void ActivityHeatMapVisualizer::setColorGradient(const std::vector<std::string>& colors) {
    pImpl->colorGradient = colors;
}

void ActivityHeatMapVisualizer::setOpacity(float opacity) {
    pImpl->opacity = opacity;
}

void ActivityHeatMapVisualizer::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

void ActivityHeatMapVisualizer::setNormalize(bool normalize) {
    pImpl->normalize = normalize;
}

void ActivityHeatMapVisualizer::setTimeWindow(double window) {
    pImpl->timeWindow = window;
}

void ActivityHeatMapVisualizer::setHistoricalDataSize(size_t size) {
    pImpl->historicalDataSize = size;
}

std::vector<std::vector<float>> ActivityHeatMapVisualizer::getHeatMapData() const {
    return std::vector<std::vector<float>>();
}

std::vector<float> ActivityHeatMapVisualizer::getActivityData() const {
    return std::vector<float>();
}

} // namespace nlm