#include "VisualizationSystemIntegration.hpp"
#include "VisualizationSystem.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <chrono>

namespace nlm {

struct VisualizationSystemIntegration::Impl {
    Brain* brain;
    VisualizationSystem* visualizer;
    bool active;
    
    // Integration configuration
    double updateRate;
    int syncMode;  // 0: per-step, 1: per-frame, 2: adaptive
    int visualizationMode;
    
    // Performance tracking
    double lastUpdateTime;
    std::chrono::high_resolution_clock::time_point lastFrameTime;
    
    // Statistics
    IntegrationStats stats;
    std::vector<double> frameTimes;
    size_t maxFrameHistory;
    
    Impl()
        : brain(nullptr)
        , visualizer(nullptr)
        , active(false)
        , updateRate(60.0)
        , syncMode(1)  // Default to frame-based sync
        , visualizationMode(0)  // NetworkOnly
        , lastUpdateTime(0.0)
        , maxFrameHistory(100) {
        frameTimes.reserve(maxFrameHistory);
        stats.framesRendered = 0;
        stats.updatesProcessed = 0;
        stats.droppedFrames = 0;
    }
};

VisualizationSystemIntegration::VisualizationSystemIntegration() : pImpl(new Impl) {}
VisualizationSystemIntegration::~VisualizationSystemIntegration() = default;

bool VisualizationSystemIntegration::initialize(Brain* brain, VisualizationSystem* visualizer) {
    if (!brain || !visualizer) {
        return false;
    }
    
    pImpl->brain = brain;
    pImpl->visualizer = visualizer;
    pImpl->active = true;
    
    // Initialize visualizer
    pImpl->visualizer->setUpdateRate(pImpl->updateRate);
    pImpl->visualizer->setVisualizationMode(
        static_cast<VisualizationSystem::VisualizationMode>(pImpl->visualizationMode)
    );
    
    return pImpl->visualizer->initialize();
}

void VisualizationSystemIntegration::update(double timestep) {
    if (!pImpl->active || !pImpl->brain || !pImpl->visualizer) {
        return;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Synchronize with Brain state
    synchronize();
    
    // Update visualization
    pImpl->visualizer->update(*pImpl->brain, timestep);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    // Update statistics
    pImpl->stats.integrationTime += duration;
    pImpl->stats.updatesProcessed++;
    
    // Track frame time
    pImpl->frameTimes.push_back(duration);
    if (pImpl->frameTimes.size() > pImpl->maxFrameHistory) {
        pImpl->frameTimes.erase(pImpl->frameTimes.begin());
    }
    
    pImpl->lastUpdateTime = 0.0;  // TODO: Get from clock
    pImpl->lastFrameTime = std::chrono::high_resolution_clock::now();
    
    // Update visualizer
    pImpl->visualizer->render();
    
    pImpl->stats.framesRendered++;
}

void VisualizationSystemIntegration::synchronize() {
    if (!pImpl->brain || !pImpl->visualizer) {
        return;
    }
    
    auto syncStartTime = std::chrono::high_resolution_clock::now();
    
    // Synchronize based on sync mode
    switch (pImpl->syncMode) {
        case 0:  // Per-step
            // Synchronize for each simulation step
            // TODO: Get current step from brain
            break;
        case 1:  // Per-frame
            // Synchronize once per visualization frame
            // TODO: Get current frame time
            break;
        case 2:  // Adaptive
            // Synchronize based on performance
            if (pImpl->stats.averageFrameTime > 16.67) {  // Less than 60 FPS
                synchronize();  // Simplified
            }
            break;
    }
    
    auto syncEndTime = std::chrono::high_resolution_clock::now();
    auto syncDuration = std::chrono::duration<double, std::milli>(syncEndTime - syncStartTime).count();
    
    pImpl->stats.syncTime += syncDuration;
}

void VisualizationSystemIntegration::close() {
    if (pImpl->visualizer) {
        pImpl->visualizer->close();
    }
    
    pImpl->active = false;
}

void VisualizationSystemIntegration::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    if (pImpl->visualizer) {
        pImpl->visualizer->setUpdateRate(hz);
    }
}

void VisualizationSystemIntegration::setSyncMode(int mode) {
    pImpl->syncMode = mode;
}

void VisualizationSystemIntegration::setVisualizationMode(int mode) {
    pImpl->visualizationMode = mode;
    if (pImpl->visualizer) {
        pImpl->visualizer->setVisualizationMode(
            static_cast<VisualizationSystem::VisualizationMode>(mode)
        );
    }
}

bool VisualizationSystemIntegration::isActive() const {
    return pImpl->active;
}

double VisualizationSystemIntegration::getUpdateRate() const {
    return pImpl->updateRate;
}

Brain* VisualizationSystemIntegration::getBrain() {
    return pImpl->brain;
}

VisualizationSystemIntegration::IntegrationStats VisualizationSystemIntegration::getStats() const {
    IntegrationStats stats = pImpl->stats;
    
    // Calculate derived statistics
    if (!pImpl->frameTimes.empty()) {
        double sum = 0.0, sumSq = 0.0;
        for (double t : pImpl->frameTimes) {
            sum += t;
            sumSq += t * t;
        }
        size_t count = pImpl->frameTimes.size();
        stats.averageFrameTime = sum / count;
        stats.frameRate = 1000.0 / stats.averageFrameTime;
        
        double variance = (sumSq / count) - (stats.averageFrameTime * stats.averageFrameTime);
        stats.cpuUsage = std::min(100.0, variance * 10.0);  // Rough estimate
    }
    
    // Estimate memory usage (placeholder)
    stats.memoryUsage = 100.0;  // MB
    
    return stats;
}

} // namespace nlm
