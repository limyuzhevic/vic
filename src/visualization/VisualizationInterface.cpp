// Stub implementation for VisualizationInterface.cpp
// This is a placeholder to satisfy CMake build requirements
// Phase 2 implementations will replace this stub

#include "VisualizationInterface.hpp"
#include <iostream>

namespace nlm {

// VisualizationInterface implementation (stub)
VisualizationInterface::VisualizationInterface() {
    std::cout << "VisualizationInterface initialized (stub)" << std::endl;
}

VisualizationInterface::~VisualizationInterface() = default;

void VisualizationInterface::initialize() {
    // Stub implementation
    std::cout << "VisualizationInterface::initialize called (stub)" << std::endl;
}

void VisualizationInterface::update(const Brain& brain) {
    // Stub implementation - in real implementation, this would render neural activity
    std::cout << "VisualizationInterface::update called (stub)" << std::endl;
}

void VisualizationInterface::render() {
    // Stub implementation - in real implementation, this would render to screen
    std::cout << "VisualizationInterface::render called (stub)" << std::endl;
}

void VisualizationInterface::shutdown() {
    // Stub implementation
    std::cout << "VisualizationInterface::shutdown called (stub)" << std::endl;
}

void VisualizationInterface::setRenderingMode(RenderingMode mode) {
    // Stub implementation
    std::cout << "VisualizationInterface::setRenderingMode called (stub)" << std::endl;
}

void VisualizationInterface::setScale(float scale) {
    // Stub implementation
    std::cout << "VisualizationInterface::setScale called (stub)" << std::endl;
}

bool VisualizationInterface::isInitialized() const {
    return true;  // Stub - always returns true
}

} // namespace nlm