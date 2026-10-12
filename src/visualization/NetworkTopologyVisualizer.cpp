#include "NetworkTopologyVisualizer.hpp"
#include <iostream>
#include <algorithm>

namespace nlm {

struct NetworkTopologyVisualizer::Impl {
    std::shared_ptr<Brain> brain;
    bool visible;
    int width;
    int height;
    std::string layoutAlgorithm;
    float nodeSize;
    float edgeThickness;
    bool showLabels;
    bool showConnectionWeights;
    bool highlightActiveConnections;
    bool showRegions;
    float regionOpacity;
    float regionBorderWidth;
    
    Impl() : visible(false), width(800), height(600), layoutAlgorithm("force-directed"),
             nodeSize(5.0f), edgeThickness(1.0f), showLabels(true), showConnectionWeights(false),
             highlightActiveConnections(true), showRegions(true), regionOpacity(0.3f), regionBorderWidth(2.0f) {}
};

NetworkTopologyVisualizer::NetworkTopologyVisualizer() : pImpl(std::make_unique<Impl>()) {}

bool NetworkTopologyVisualizer::initialize(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    pImpl->brain = brain;
    pImpl->visible = true;
    return true;
}

void NetworkTopologyVisualizer::update() {
    // Update layout calculations if needed
}

void NetworkTopologyVisualizer::render() {
    if (!pImpl->visible) return;
    
    std::cout << "Rendering Network Topology Visualization:" << std::endl;
    std::cout << "  Layout algorithm: " << pImpl->layoutAlgorithm << std::endl;
    std::cout << "  Node size: " << pImpl->nodeSize << std::endl;
    std::cout << "  Edge thickness: " << pImpl->edgeThickness << std::endl;
    std::cout << "  Labels shown: " << (pImpl->showLabels ? "yes" : "no") << std::endl;
    std::cout << "  Connection weights shown: " << (pImpl->showConnectionWeights ? "yes" : "no") << std::endl;
    std::cout << "  Active connections highlighted: " << (pImpl->highlightActiveConnections ? "yes" : "no") << std::endl;
    std::cout << "  Regions shown: " << (pImpl->showRegions ? "yes" : "no") << std::endl;
    
    if (pImpl->brain) {
        auto spikeSystem = pImpl->brain->getSpikeSystem();
        if (spikeSystem) {
            std::cout << "  Total neurons: " << pImpl->brain->getTotalSpikeCount() << std::endl;
        }
    }
}

void NetworkTopologyVisualizer::setVisible(bool visible) {
    pImpl->visible = visible;
}

bool NetworkTopologyVisualizer::isVisible() const {
    return pImpl->visible;
}

void NetworkTopologyVisualizer::setSize(int width, int height) {
    pImpl->width = width;
    pImpl->height = height;
}

void NetworkTopologyVisualizer::setLayoutAlgorithm(const std::string& algorithm) {
    pImpl->layoutAlgorithm = algorithm;
}

void NetworkTopologyVisualizer::setNodeSize(float size) {
    pImpl->nodeSize = size;
}

void NetworkTopologyVisualizer::setEdgeThickness(float thickness) {
    pImpl->edgeThickness = thickness;
}

void NetworkTopologyVisualizer::setShowLabels(bool show) {
    pImpl->showLabels = show;
}

void NetworkTopologyVisualizer::setShowConnectionWeights(bool show) {
    pImpl->showConnectionWeights = show;
}

void NetworkTopologyVisualizer::setHighlightActiveConnections(bool highlight) {
    pImpl->highlightActiveConnections = highlight;
}

void NetworkTopologyVisualizer::setShowRegions(bool show) {
    pImpl->showRegions = show;
}

void NetworkTopologyVisualizer::setRegionOpacity(float opacity) {
    pImpl->regionOpacity = opacity;
}

void NetworkTopologyVisualizer::setRegionBorderWidth(float width) {
    pImpl->regionBorderWidth = width;
}

} // namespace nlm