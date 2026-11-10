#include "VisualizationInterface.hpp"

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    
    Impl() : active(false), updateRate(30.0) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // TODO PHASE 2: Initialize real visualization (e.g., with GLFW, SDL, or custom)
    // PLACEHOLDER: For now, use console-based visualization
    pImpl->active = true;
    return true;
}

void VisualizationInterface::update() {
    // PLACEHOLDER: No visualization in Phase 1
}

void VisualizationInterface::render() {
    // Real console-based visualization for neural activity
    std::cout << "[NLM Visualization] Active" << std::endl;
    std::cout << "Update rate: " << pImpl->updateRate << " Hz" << std::endl;
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

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    std::cout << "[Network Visualization] Displaying brain structure" << std::endl;
    std::cout << "  Regions: " << brain.getRegionCount() << std::endl;
    std::cout << "  Neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "  Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
    std::cout << "  Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    std::cout << "[Activity Visualization] Neural activity heatmap" << std::endl;
    float avgFiringRate = brain.getAverageFiringRate();
    std::cout << "  Average firing rate: " << avgFiringRate << " Hz" << std::endl;
    std::cout << "  Excitatory/Inhibitory ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    std::cout << "[Spike Visualization] Displaying " << spikes.size() << " spike events" << std::endl;
    if (!spikes.empty()) {
        std::cout << "  First spike time: " << spikes[0].timestamp << std::endl;
        std::cout << "  Last spike time: " << spikes.back().timestamp << std::endl;
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    std::cout << "[Weight Visualization] Synaptic weight distribution" << std::endl;
    float avgWeight = brain.getBrain()->getAverageSynapticWeight();
    std::cout << "  Average synaptic weight: " << avgWeight << std::endl;
}

} // namespace nlm
