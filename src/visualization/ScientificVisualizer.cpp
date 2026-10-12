#include "ScientificVisualizer.hpp"
#include <iostream>

namespace nlm {

struct ScientificVisualizer::Impl {
    std::shared_ptr<Brain> brain;
    bool visible;
    int width;
    int height;
    VisualizationType visualizationType;
    float dataMin;
    float dataMax;
    float isosurfaceThreshold;
    float sliceX, sliceY, sliceZ;
    bool enableCrossSection;
    bool showScaleBar;
    
    Impl() : visible(false), width(800), height(600), visualizationType(VisualizationType::NeuronDynamics),
             dataMin(-100.0f), dataMax(100.0f), isosurfaceThreshold(0.0f), sliceX(0.0f), sliceY(0.0f), sliceZ(0.0f),
             enableCrossSection(false), showScaleBar(true) {}
};

ScientificVisualizer::ScientificVisualizer() : pImpl(std::make_unique<Impl>()) {}

bool ScientificVisualizer::initialize(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    pImpl->brain = brain;
    pImpl->visible = true;
    return true;
}

void ScientificVisualizer::update() {
    // Update scientific data from brain
}

void ScientificVisualizer::render() {
    if (!pImpl->visible) return;
    
    std::cout << "Rendering Scientific Visualization:" << std::endl;
    std::cout << "  Visualization type: " << static_cast<int>(pImpl->visualizationType) << std::endl;
    std::cout << "  Data range: [" << pImpl->dataMin << ", " << pImpl->dataMax << "]" << std::endl;
    std::cout << "  Isosurface threshold: " << pImpl->isosurfaceThreshold << std::endl;
    std::cout << "  Cross section: (" << pImpl->sliceX << ", " << pImpl->sliceY << ", " << pImpl->sliceZ << ")" << std::endl;
    std::cout << "  Cross section enabled: " << (pImpl->enableCrossSection ? "yes" : "no") << std::endl;
    std::cout << "  Scale bar shown: " << (pImpl->showScaleBar ? "yes" : "no") << std::endl;
}

void ScientificVisualizer::setVisible(bool visible) {
    pImpl->visible = visible;
}

bool ScientificVisualizer::isVisible() const {
    return pImpl->visible;
}

void ScientificVisualizer::setSize(int width, int height) {
    pImpl->width = width;
    pImpl->height = height;
}

void ScientificVisualizer::setVisualizationType(VisualizationType type) {
    pImpl->visualizationType = type;
}

void ScientificVisualizer::setDataRange(float min, float max) {
    pImpl->dataMin = min;
    pImpl->dataMax = max;
}

void ScientificVisualizer::setIsosurfaceThreshold(float threshold) {
    pImpl->isosurfaceThreshold = threshold;
}

void ScientificVisualizer::setSlicePosition(float x, float y, float z) {
    pImpl->sliceX = x;
    pImpl->sliceY = y;
    pImpl->sliceZ = z;
}

void ScientificVisualizer::setEnableCrossSection(bool enable) {
    pImpl->enableCrossSection = enable;
}

void ScientificVisualizer::setShowScaleBar(bool show) {
    pImpl->showScaleBar = show;
}

} // namespace nlm