#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    bool enabled;
    size_t stepCounter;
    
    Impl() : active(false), updateRate(30.0), enabled(true), stepCounter(0) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // BASIC IMPLEMENTATION: Phase 2 simplified interface
    pImpl->active = pImpl->enabled;
    if (pImpl->enabled) {
        NLM_LOG_INFO("Visualization interface initialized (simplified mode)");
    } else {
        NLM_LOG_INFO("Visualization interface disabled (simplified mode)");
    }
    return pImpl->active;
}

void VisualizationInterface::update() {
    // Basic implementation that can be extended
    if (!pImpl->enabled) return;
    
    pImpl->stepCounter++;
    
    // Log every 100 steps for debugging
    if (pImpl->stepCounter % 100 == 0) {
        NLM_LOG_INFO("VisualizationInterface updated " + std::to_string(pImpl->stepCounter) + " times");
    }
}

void VisualizationInterface::render() {
    // Simplified rendering: log basic brain stats
    if (!pImpl->enabled) return;
    
    // This is a simplified version that doesn't require GUI libraries
    // Real implementations would use OpenGL, GLFW, SDL, etc.
    NLM_LOG_INFO("Visualization render called (simplified output)");
}

void VisualizationInterface::close() {
    pImpl->active = false;
    NLM_LOG_INFO("Visualization interface closed");
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

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    // BASIC IMPLEMENTATION: Network visualization in simplified mode
    if (!pImpl->enabled) return;
    
    std::stringstream ss;
    ss << "Network Visualization (simplified): ";
    ss << "Regions=" << brain.getRegionCount();
    ss << ", Neurons=" << brain.getTotalNeuronCount();
    ss << ", Synapses=" << brain.getTotalSynapseCount();
    ss << ", Spikes=" << brain.getTotalSpikeCount();
    
    NLM_LOG_INFO(ss.str());
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    // BASIC IMPLEMENTATION: Activity visualization
    if (!pImpl->enabled) return;
    
    auto mostActive = brain.getMostActiveNeurons(5);
    std::stringstream ss;
    ss << "Activity Visualization (simplified): Most active neurons: ";
    for (size_t i = 0; i < mostActive.size(); ++i) {
        ss << mostActive[i];
        if (i < mostActive.size() - 1) ss << ", ";
    }
    
    NLM_LOG_INFO(ss.str());
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    // BASIC IMPLEMENTATION: Spike visualization
    if (!pImpl->enabled) return;
    
    if (spikes.empty()) {
        NLM_LOG_INFO("Spike Visualization: No spikes recorded");
        return;
    }
    
    // Create a simple spike raster representation
    size_t maxSpikesPerNeuron = 0;
    for (const auto& spike : spikes) {
        // Count spikes per neuron (simplified)
    }
    
    NLM_LOG_INFO("Spike Visualization: " + std::to_string(spikes.size()) + 
                " spike events recorded (simplified)");
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    // BASIC IMPLEMENTATION: Weight matrix visualization
    if (!pImpl->enabled) return;
    
    // Get weight statistics from first available region
    if (auto* region = brain.getRegion(RegionId(1))) {
        auto& synapses = region->getSynapses();
        if (!synapses.empty()) {
            float minWeight = synapses[0]->getWeight();
            float maxWeight = minWeight;
            float sumWeights = 0.0f;
            
            for (const auto& syn : synapses) {
                float w = syn->getWeight();
                minWeight = std::min(minWeight, w);
                maxWeight = std::max(maxWeight, w);
                sumWeights += w;
            }
            
            float avgWeight = sumWeights / synapses.size();
            NLM_LOG_INFO("Weight Visualization: min=" + std::to_string(minWeight) +
                        " avg=" + std::to_string(avgWeight) +
                        " max=" + std::to_string(maxWeight) +
                        " count=" + std::to_string(synapses.size()));
        }
    }
}

} // namespace nlm
