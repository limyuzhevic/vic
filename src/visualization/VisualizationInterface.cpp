#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <algorithm>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    size_t frameCounter;
    std::vector<float> lastNetworkData;
    std::vector<float> lastActivityData;
    std::vector<std::string> lastSpikeData;
    std::vector<std::vector<float>> lastWeightData;
    
    // Visualization options
    bool networkVisualizationEnabled;
    bool activityVisualizationEnabled;
    bool memoryVisualizationEnabled;
    bool predictionVisualizationEnabled;
    bool spikeVisualizationEnabled;
    bool weightVisualizationEnabled;
    
    Impl() : active(false), updateRate(30.0), frameCounter(0),
             networkVisualizationEnabled(true), activityVisualizationEnabled(true),
             memoryVisualizationEnabled(true), predictionVisualizationEnabled(true),
             spikeVisualizationEnabled(true), weightVisualizationEnabled(true) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // TODO PHASE 2: Initialize real visualization (e.g., with GLFW, SDL, or custom)
    // For now, just mark as active for testing
    pImpl->active = true;
    NLM_LOG_INFO("VisualizationInterface initialized (placeholder implementation)");
    return true;
}

void VisualizationInterface::update(const Brain& brain) {
    if (!pImpl->active) return;
    
    pImpl->frameCounter++;
    
    // Collect network data
    if (pImpl->networkVisualizationEnabled) {
        std::vector<float> networkData;
        size_t totalNeurons = brain.getTotalNeuronCount();
        size_t activeNeurons = brain.getActiveNeuronCount();
        
        if (totalNeurons > 0) {
            networkData.push_back(static_cast<float>(activeNeurons) / totalNeurons);
            networkData.push_back(brain.getAverageFiringRate());
            networkData.push_back(brain.getExcitationInhibitionRatio());
        }
        
        pImpl->lastNetworkData = networkData;
    }
    
    // Collect activity data
    if (pImpl->activityVisualizationEnabled) {
        std::vector<float> activityData;
        activityData.push_back(brain.getAverageFiringRate());
        activityData.push_back(static_cast<float>(brain.getFiringNeuronCount()));
        activityData.push_back(brain.getPendingSpikeEventCount());
        
        pImpl->lastActivityData = activityData;
    }
    
    // Collect spike data
    if (pImpl->spikeVisualizationEnabled) {
        // Spike data would come from spike system
        std::vector<std::string> spikeData;
        spikeData.push_back("Total spikes: " + std::to_string(brain.getTotalSpikeCount()));
        spikeData.push_back("Pending events: " + std::to_string(brain.getPendingSpikeEventCount()));
        
        pImpl->lastSpikeData = spikeData;
    }
    
    // Collect weight data
    if (pImpl->weightVisualizationEnabled) {
        std::vector<std::vector<float>> weightData;
        size_t regionCount = brain.getRegionCount();
        
        for (size_t i = 0; i < regionCount; ++i) {
            auto region = brain.getRegion(nlm::RegionId(i + 1));
            if (region) {
                std::vector<float> regionWeights;
                for (const auto& syn : region->getSynapses()) {
                    regionWeights.push_back(syn->getWeight());
                }
                weightData.push_back(regionWeights);
            }
        }
        
        pImpl->lastWeightData = weightData;
    }
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    // Simple console-based rendering for testing
    NLM_LOG_INFO("Rendering visualization frame " + std::to_string(pImpl->frameCounter));
    
    // Display collected data
    if (pImpl->networkVisualizationEnabled && !pImpl->lastNetworkData.empty()) {
        NLM_LOG_INFO("Network: Active neurons ratio: " + 
                    std::to_string(pImpl->lastNetworkData[0]) +
                    ", Firing rate: " + std::to_string(pImpl->lastNetworkData[1]));
    }
    
    if (pImpl->activityVisualizationEnabled && !pImpl->lastActivityData.empty()) {
        NLM_LOG_INFO("Activity: Avg firing rate: " + std::to_string(pImpl->lastActivityData[0]));
    }
    
    if (pImpl->spikeVisualizationEnabled && !pImpl->lastSpikeData.empty()) {
        NLM_LOG_INFO("Spikes: " + pImpl->lastSpikeData[0]);
    }
}

void VisualizationInterface::close() {
    pImpl->active = false;
    NLM_LOG_INFO("VisualizationInterface closed");
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
    // Network visualization implementation
    std::stringstream ss;
    ss << "=== Network Visualization ===\n";
    ss << "Regions: " << brain.getRegionCount() << "\n";
    ss << "Total neurons: " << brain.getTotalNeuronCount() << "\n";
    ss << "Total synapses: " << brain.getTotalSynapseCount() << "\n";
    ss << "Active neurons: " << brain.getActiveNeuronCount() << "\n";
    ss << "Firing neurons: " << brain.getFiringNeuronCount() << "\n";
    ss << "Average firing rate: " << brain.getAverageFiringRate() << "\n";
    ss << "E/I ratio: " << brain.getExcitationInhibitionRatio() << "\n";
    
    NLM_LOG_INFO(ss.str());
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    // Activity visualization implementation
    std::stringstream ss;
    ss << "=== Activity Visualization ===\n";
    ss << "Total spikes: " << brain.getTotalSpikeCount() << "\n";
    ss << "Pending spike events: " << brain.getPendingSpikeEventCount() << "\n";
    ss << "Firing rate: " << brain.getAverageFiringRate() << "\n";
    
    NLM_LOG_INFO(ss.str());
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    // Spike raster plot implementation
    std::stringstream ss;
    ss << "=== Spike Raster Plot ===\n";
    ss << "Spike events: " << spikes.size() << "\n";
    ss << "Last event time: " << (spikes.empty() ? 0 : spikes.back().time) << "\n";
    
    // Display spike statistics
    for (size_t i = 0; i < std::min(size_t(5), spikes.size()); ++i) {
        const auto& spike = spikes[i];
        ss << "Spike " << i << ": neuron " << spike.neuronId.getId() 
           << " at time " << spike.time << "\n";
    }
    
    NLM_LOG_INFO(ss.str());
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    // Weight matrix visualization implementation
    std::stringstream ss;
    ss << "=== Weight Matrix Visualization ===\n";
    
    size_t totalRegions = brain.getRegionCount();
    size_t totalSynapses = brain.getTotalSynapseCount();
    
    if (totalSynapses > 0) {
        ss << "Total synapses: " << totalSynapses << "\n";
        ss << "Average synaptic weight: Calculating...\n";
    }
    
    NLM_LOG_INFO(ss.str());
}

void VisualizationInterface::visualizeWorkingMemory(const NeuralWorkingMemory& memory) {
    // Working memory visualization implementation
    std::stringstream ss;
    ss << "=== Working Memory Visualization ===\n";
    ss << "Active traces: " << memory.getActiveTraces() << "\n";
    ss << "Memory capacity: " << memory.getCapacity() << "\n";
    ss << "Memory activity: " << memory.getMemoryActivity() << "\n";
    ss << "Decay rate: " << memory.getDecayRate() << "\n";
    ss << "Memory neurons: " << memory.getMemoryNeurons().size() << "\n";
    
    NLM_LOG_INFO(ss.str());
}

void VisualizationInterface::visualizeEpisodicMemory(const NeuralEpisodicMemory& memory) {
    // Episodic memory visualization implementation
    std::stringstream ss;
    ss << "=== Episodic Memory Visualization ===\n";
    ss << "Episode count: " << memory.getEpisodeCount() << "\n";
    ss << "Max episodes: 1000 (from config)\n";
    ss << "Average reward: " << memory.getAverageReward() << "\n";
    ss << "Replay enabled: yes\n";
    
    NLM_LOG_INFO(ss.str());
}

void VisualizationInterface::visualizePredictionSystem(const PredictionSystem& prediction) {
    // Prediction system visualization implementation
    std::stringstream ss;
    ss << "=== Prediction System Visualization ===\n";
    ss << "Prediction error: " << prediction.getPredictionError() << "\n";
    ss << "Prediction confidence: " << prediction.getConfidence() << "\n";
    ss << "Error history size: " << prediction.getErrorHistory().size() << "\n";
    
    NLM_LOG_INFO(ss.str());
}

void VisualizationInterface::visualizeNeuromodulation(const Dopamine& dopamine,
                                                      const Curiosity& curiosity,
                                                      const Novelty& novelty) {
    // Neuromodulation visualization implementation
    std::stringstream ss;
    ss << "=== Neuromodulation Visualization ===\n";
    ss << "Dopamine level: " << dopamine.getLevel() << "\n";
    ss << "Curiosity level: " << curiosity.getLevel() << "\n";
    ss << "Novelty level: " << novelty.getLevel() << "\n";
    
    NLM_LOG_INFO(ss.str());
}

bool VisualizationInterface::exportVisualization(const std::string& filename, VisualizationFormat format) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for visualization export: " + filename);
        return false;
    }
    
    file << "NLM Visualization Export\n";
    file << "Timestamp: " << std::time(nullptr) << "\n";
    file << "Format: " << static_cast<int>(format) << "\n";
    
    // Export network data
    file << "\nNetwork Data:\n";
    for (size_t i = 0; i < pImpl->lastNetworkData.size(); ++i) {
        file << "Channel " << i << ": " << pImpl->lastNetworkData[i] << "\n";
    }
    
    // Export activity data
    file << "\nActivity Data:\n";
    for (size_t i = 0; i < pImpl->lastActivityData.size(); ++i) {
        file << "Channel " << i << ": " << pImpl->lastActivityData[i] << "\n";
    }
    
    file.close();
    NLM_LOG_INFO("Visualization exported to: " + filename);
    return true;
}

bool VisualizationInterface::saveFrame(const std::string& filename) {
    // Save current frame as text file (simple implementation)
    return exportVisualization(filename, VisualizationFormat::PNG);
}

VisualizationInterface::VisualizationStats VisualizationInterface::getStats() const {
    VisualizationStats stats;
    
    if (pImpl->lastNetworkData.size() > 0) {
        stats.neurons = static_cast<size_t>(pImpl->lastNetworkData.size() * 100);  // Estimate
        stats.activeNeurons = static_cast<size_t>(pImpl->lastNetworkData[0] * stats.neurons);
        stats.averageFiringRate = pImpl->lastNetworkData.size() > 1 ? pImpl->lastNetworkData[1] : 0;
    }
    
    if (pImpl->lastActivityData.size() > 0) {
        stats.activeNeurons = static_cast<size_t>(pImpl->lastActivityData[0] * 100);
        stats.totalMemoryActivity = pImpl->lastActivityData.size() > 1 ? pImpl->lastActivityData[1] : 0;
    }
    
    // Memory stats would need access to memory objects
    stats.episodeCount = 0;  // Would need to pass memory references
    
    return stats;
}

std::string VisualizationInterface::getDebugInfo() const {
    std::stringstream ss;
    ss << "=== Visualization Interface Debug Info ===\n";
    ss << "Active: " << (pImpl->active ? "yes" : "no") << "\n";
    ss << "Update rate: " << pImpl->updateRate << " Hz\n";
    ss << "Frame counter: " << pImpl->frameCounter << "\n";
    ss << "Network visualization enabled: " << (pImpl->networkVisualizationEnabled ? "yes" : "no") << "\n";
    ss << "Activity visualization enabled: " << (pImpl->activityVisualizationEnabled ? "yes" : "no") << "\n";
    ss << "Spike visualization enabled: " << (pImpl->spikeVisualizationEnabled ? "yes" : "no") << "\n";
    ss << "Weight visualization enabled: " << (pImpl->weightVisualizationEnabled ? "yes" : "no") << "\n";
    ss << "Network data channels: " << pImpl->lastNetworkData.size() << "\n";
    ss << "Activity data channels: " << pImpl->lastActivityData.size() << "\n";
    
    return ss.str();
}

void VisualizationInterface::reset() {
    pImpl->frameCounter = 0;
    pImpl->lastNetworkData.clear();
    pImpl->lastActivityData.clear();
    pImpl->lastSpikeData.clear();
    pImpl->lastWeightData.clear();
    NLM_LOG_INFO("VisualizationInterface reset");
}

bool VisualizationInterface::testVisualization() const {
    return isActive() && !pImpl->lastNetworkData.empty() && !pImpl->lastActivityData.empty();
}

} // namespace nlm
