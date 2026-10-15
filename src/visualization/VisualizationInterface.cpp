#include "VisualizationInterface.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active = false;
    double updateRate = 30.0;
    std::chrono::steady_clock::time_point lastUpdate;
    double accumulatedTime = 0.0;
    
    // Performance tracking
    double totalUpdateTime = 0.0;
    int updateCount = 0;
    
    Impl() {
        lastUpdate = std::chrono::steady_clock::now();
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    pImpl->active = true;
    pImpl->lastUpdate = std::chrono::steady_clock::now();
    return true;
}

void VisualizationInterface::update(double deltaTime) {
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = now - pImpl->lastUpdate;
    pImpl->lastUpdate = now;
    
    // Track performance
    pImpl->accumulatedTime += elapsed.count();
    pImpl->totalUpdateTime += deltaTime;
    pImpl->updateCount++;
}

void VisualizationInterface::render() {
    // Default implementation - nothing to render
}

void VisualizationInterface::close() {
    pImpl->active = false;
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

bool VisualizationInterface::exportData(const ExportOptions& options) {
    std::ofstream file(options.filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << getCurrentFrame();
    file.close();
    return true;
}

bool VisualizationInterface::importData(const std::string& filepath) {
    // Default implementation - nothing to import
    return false;
}

std::string VisualizationInterface::getCurrentFrame() const {
    std::stringstream ss;
    ss << "=== Visualization Frame ===" << std::endl;
    ss << "Active: " << (isActive() ? "true" : "false") << std::endl;
    ss << "Update Rate: " << std::fixed << std::setprecision(2) << getUpdateRate() << " Hz" << std::endl;
    
    if (pImpl->updateCount > 0) {
        double avgDeltaTime = pImpl->totalUpdateTime / pImpl->updateCount;
        ss << "Average update time: " << std::scientific << avgDeltaTime << "s" << std::endl;
        ss << "Actual update rate: " << std::fixed << (1.0 / avgDeltaTime) << " Hz" << std::endl;
    }
    
    ss << getPerformanceStats() << std::endl;
    return ss.str();
}

std::string VisualizationInterface::getPerformanceStats() const {
    std::stringstream ss;
    ss << "Performance Statistics:" << std::endl;
    ss << "  Updates processed: " << pImpl->updateCount << std::endl;
    if (pImpl->updateCount > 0) {
        ss << "  Average delta time: " << (pImpl->totalUpdateTime / pImpl->updateCount) << "s" << std::endl;
    }
    return ss.str();
}

} // namespace nlm
