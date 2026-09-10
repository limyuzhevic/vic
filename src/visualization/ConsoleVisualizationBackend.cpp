// Console visualization backend implementation
#pragma once

#include "ConsoleVisualizationBackend.hpp"
#include <iostream>
#include <unordered_map>
#include "../core/Types/Types.hpp"

namespace nlm {

ConsoleVisualizationBackend::ConsoleVisualizationBackend() = default;
ConsoleVisualizationBackend::~ConsoleVisualizationBackend() = default;

bool ConsoleVisualizationBackend::initialize() {
    active_ = true;
    return true;
}

void ConsoleVisualizationBackend::shutdown() {
    active_ = false;
}

void ConsoleVisualizationBackend::update() {
    // In console mode, updates are handled directly in render()
}

void ConsoleVisualizationBackend::render() {
    if (!active_) return;
    
    // Clear screen (ANSI escape sequence)
    std::cout << "\033[2J\033[H";
    
    std::cout << "=== NLM BRAIN VISUALIZATION ===" << std::endl;
    std::cout << "Active neurons: " << neuronCount_ << std::endl;
    std::cout << "Firing rate: " << averageFiringRate_ << " Hz" << std::endl;
    
    if (!regionActivities_.empty()) {
        std::cout << "\nRegion Activities:" << std::endl;
        for (const auto& [regionId, activity] : regionActivities_) {
            std::cout << "  Region " << regionId << ": " << activity << "%" << std::endl;
        }
    }
}

void ConsoleVisualizationBackend::setSettings(const VisualizationSettings& settings) {
    settings_ = settings;
}

bool ConsoleVisualizationBackend::isActive() const {
    return active_;
}

void ConsoleVisualizationBackend::setWindowSize(int width, int height) {
    width_ = width;
    height_ = height;
}

int ConsoleVisualizationBackend::getWidth() const { return width_; }
int ConsoleVisualizationBackend::getHeight() const { return height_; }

void ConsoleVisualizationBackend::setBrainData(size_t neuronCount, float averageFiringRate,
                                            const std::unordered_map<RegionId, float>& regionActivities) {
    neuronCount_ = neuronCount;
    averageFiringRate_ = averageFiringRate;
    regionActivities_ = regionActivities;
}

} // namespace nlm