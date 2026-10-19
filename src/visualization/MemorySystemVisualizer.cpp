#include "VisualizationSystem.hpp"

namespace nlm {

struct MemorySystemVisualizer::Impl {
    bool showWorkingMemoryTrails;
    bool showEpisodicReplay;
    bool showMemoryTimeSeries;
    double timeWindowSeconds;
    float replayAnimationSpeed;
    
    std::vector<WorkingMemoryViz> workingMemory;
    std::vector<EpisodicMemoryViz> episodicMemory;
    std::vector<MemoryTimeSeries> timeSeries;
    
    // Simulation state
    double currentReplayTime;
    bool replaying;
};

MemorySystemVisualizer::MemorySystemVisualizer() : pImpl(std::make_unique<Impl>()) {
    pImpl->showWorkingMemoryTrails = true;
    pImpl->showEpisodicReplay = true;
    pImpl->showMemoryTimeSeries = true;
    pImpl->timeWindowSeconds = 300.0; // 5 minutes
    pImpl->replayAnimationSpeed = 1.0f;
    pImpl->currentReplayTime = 0.0;
    pImpl->replaying = false;
}

MemorySystemVisualizer::~MemorySystemVisualizer() = default;

void MemorySystemVisualizer::setShowWorkingMemoryTrails(bool show) {
    pImpl->showWorkingMemoryTrails = show;
}

void MemorySystemVisualizer::setShowEpisodicReplay(bool show) {
    pImpl->showEpisodicReplay = show;
}

void MemorySystemVisualizer::setShowMemoryTimeSeries(bool show) {
    pImpl->showMemoryTimeSeries = show;
}

void MemorySystemVisualizer::setMemoryTimeWindow(double timeWindowSeconds) {
    pImpl->timeWindowSeconds = timeWindowSeconds;
}

void MemorySystemVisualizer::setReplayAnimationSpeed(float speed) {
    pImpl->replayAnimationSpeed = speed;
}

void MemorySystemVisualizer::updateWorkingMemory(const std::vector<WorkingMemoryViz>& traces) {
    pImpl->workingMemory = traces;
}

void MemorySystemVisualizer::updateEpisodicMemory(const std::vector<EpisodicMemoryViz>& episodes) {
    pImpl->episodicMemory = episodes;
}

void MemorySystemVisualizer::updateMemoryTimeSeries(const std::vector<MemoryTimeSeries>& series) {
    pImpl->timeSeries = series;
}

float MemorySystemVisualizer::calculateMemoryCapacityUtilization() const {
    // Calculate utilization based on number of active traces vs capacity
    if (pImpl->workingMemory.empty()) return 0.0f;
    
    float totalImportance = 0.0f;
    for (const auto& trace : pImpl->workingMemory) {
        totalImportance += trace.importance;
    }
    
    return std::min(1.0f, totalImportance / pImpl->workingMemory.size());
}

float MemorySystemVisualizer::calculateMemoryConsolidationRate() const {
    // Calculate consolidation rate from episodic memory
    if (pImpl->episodicMemory.empty()) return 0.0f;
    
    float totalReplayStrength = 0.0f;
    for (const auto& episode : pImpl->episodicMemory) {
        totalReplayStrength += episode.replayStrength;
    }
    
    return totalReplayStrength / pImpl->episodicMemory.size();
}

float MemorySystemVisualizer::calculateReplayEfficiency() const {
    // Calculate efficiency of memory replay
    // Placeholder implementation
    return 0.5f;
}

std::vector<size_t> MemorySystemVisualizer::findMemoryClusters() const {
    // Find clusters of related memory traces
    std::vector<size_t> clusters;
    // Placeholder implementation
    return clusters;
}

void MemorySystemVisualizer::simulateMemoryReplay(double timeStep) {
    if (!pImpl->replaying) return;
    
    pImpl->currentReplayTime += timeStep * pImpl->replayAnimationSpeed;
    
    // Update episodic memory replay visualization
    for (auto& episode : pImpl->episodicMemory) {
        // Calculate replay progress based on episode significance
        double progress = std::fmod(pImpl->currentReplayTime / episode.endTime, 1.0);
        episode.replayStrength = static_cast<float>(1.0 - std::abs(progress - 0.5) * 2.0);
    }
}

void MemorySystemVisualizer::animateMemoryConsolidation(double timeStep) {
    // Animate memory consolidation process
    for (auto& trace : pImpl->workingMemory) {
        // Decay activation over time
        trace.activationLevel *= std::pow(0.95f, static_cast<float>(timeStep));
    }
}

bool MemorySystemVisualizer::exportMemoryGraph(const std::string& filepath) {
    // Export memory graph to file
    return true;
}

bool MemorySystemVisualizer::exportTimeSeries(const std::string& filepath) {
    // Export time series data to file
    return true;
}

} // namespace nlm
