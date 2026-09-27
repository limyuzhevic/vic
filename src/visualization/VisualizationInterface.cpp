#include "VisualizationInterface.hpp"

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    std::string networkInfo;
    std::string activityInfo;
    std::string spikeInfo;
    std::string weightInfo;
    std::mutex dataMutex;
    std::chrono::steady_clock::time_point lastUpdate;
    
    Impl() : active(true), updateRate(30.0) {
        lastUpdate = std::chrono::steady_clock::now();
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Simple console-based visualization
    pImpl->active = true;
    pImpl->networkInfo.clear();
    pImpl->activityInfo.clear();
    pImpl->spikeInfo.clear();
    pImpl->weightInfo.clear();
    
    pImpl->lastUpdate = std::chrono::steady_clock::now();
    return pImpl->active;
}

void VisualizationInterface::update() {
    if (!pImpl->active) return;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration<double>(now - pImpl->lastUpdate).count();
    
    if (elapsed >= 1.0 / pImpl->updateRate) {
        // Update is called, we could refresh data here
        pImpl->lastUpdate = now;
    }
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    // In a real implementation, this would render to a window
    // For now, we'll just print to console
    std::cout << "=== NLM Visualization ===" << std::endl;
    std::cout << pImpl->networkInfo << std::endl;
    std::cout << pImpl->activityInfo << std::endl;
    std::cout << pImpl->spikeInfo << std::endl;
    std::cout << pImpl->weightInfo << std::endl;
    std::cout << "=========================" << std::endl << std::endl;
}

void VisualizationInterface::close() {
    pImpl->active = false;
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    if (hz > 0) {
        pImpl->updateRate = hz;
    }
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->networkInfo = getNetworkInfo(brain);
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->activityInfo = getActivityInfo(brain);
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->spikeInfo = getSpikeInfo(spikes);
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->weightInfo = getWeightInfo(brain);
}

std::string VisualizationInterface::getNetworkInfo(const Brain& brain) const {
    std::stringstream ss;
    ss << "Network Info:" << std::endl;
    ss << "  Regions: " << brain.getRegionCount() << std::endl;
    ss << "  Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
    ss << "  Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
    ss << "  E/I Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
    ss << "  Average Firing Rate: " << brain.getAverageFiringRate() << std::endl;
    ss << "  Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
    ss << "  Firing Neurons (this step): " << brain.getFiringNeuronCount() << std::endl;
    ss << "  Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
    ss << "  Pending Events: " << brain.getPendingSpikeEventCount() << std::endl;
    
    return ss.str();
}

std::string VisualizationInterface::getActivityInfo(const Brain& brain) const {
    std::stringstream ss;
    ss << "Activity Summary:" << std::endl;
    ss << "  Simulation Step: " << (brain.getTotalSpikeCount() > 0 ? "Active" : "Inactive") << std::endl;
    ss << "  Firing Neurons Ratio: " 
       << (brain.getActiveNeuronCount() > 0 ? 
           std::to_string(static_cast<double>(brain.getFiringNeuronCount()) / brain.getActiveNeuronCount()) : "0") 
       << std::endl;
    ss << "  Spike Frequency: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    ss << "  Network State: " << (brain.getExcitationInhibitionRatio() > 1.0 ? "Excited" : "Balanced") << std::endl;
    
    return ss.str();
}

std::string VisualizationInterface::getSpikeInfo(const std::vector<SpikeEvent>& spikes) const {
    std::stringstream ss;
    ss << "Spike Events:" << std::endl;
    ss << "  Total Spikes: " << spikes.size() << std::endl;
    
    if (!spikes.empty()) {
        ss << "  Recent Spikes:" << std::endl;
        size_t count = std::min(spikes.size(), size_t(10));
        for (size_t i = 0; i < count; ++i) {
            const auto& spike = spikes[spikes.size() - 1 - i]; // Show most recent first
            ss << "    Neuron " << spike.neuron_id << " at time " << spike.spike_time << std::endl;
        }
        if (spikes.size() > 10) {
            ss << "    ... and " << (spikes.size() - 10) << " more" << std::endl;
        }
    }
    
    return ss.str();
}

std::string VisualizationInterface::getWeightInfo(const Brain& brain) const {
    std::stringstream ss;
    ss << "Synaptic Weights:" << std::endl;
    ss << "  Average Weight: " << (brain.getTotalSynapseCount() > 0 ? 
                               brain.getExcitationInhibitionRatio() / (brain.getExcitationInhibitionRatio() + 1.0) : 0.0) 
       << std::endl;
    ss << "  Weight Distribution: Not available in Phase 2" << std::endl;
    
    // Simple weight visualization
    if (brain.getTotalSynapseCount() > 0) {
        ss << "  Weight Matrix:" << std::endl;
        for (size_t i = 0; i < 5 && i < brain.getRegionCount(); ++i) {
            ss << "    Region " << (i + 1) << ": ";
            auto* region = brain.getRegion(RegionId(i + 1));
            if (region) {
                float avgWeight = region->getAverageSynapticWeight();
                ss << "Avg Weight: " << avgWeight << ", ";
                ss << "Synapses: " << region->getSynapseCount() << ", ";
                ss << "Neurons: " << region->getTotalNeuronCount() << std::endl;
            }
        }
    }
    
    return ss.str();
}

} // namespace nlm
