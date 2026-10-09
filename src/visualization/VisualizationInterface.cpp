#include "VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <fstream>
#include <iomanip>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    
    // Performance profiling data
    struct PerformanceData {
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        double cpuTime;
        double memoryUsage;
        size_t stepCount;
        size_t spikeCount;
        
        PerformanceData() : cpuTime(0.0), memoryUsage(0.0), stepCount(0), spikeCount(0) {}
    };
    
    PerformanceData currentPerfData;
    
    // Network visualization data
    struct NetworkData {
        std::vector<std::vector<double>> adjacencyMatrix;
        std::vector<std::string> neuronTypes;
        std::vector<std::string> regionNames;
        
        NetworkData(size_t neuronCount) {
            adjacencyMatrix.resize(neuronCount, std::vector<double>(neuronCount, 0.0));
            neuronTypes.resize(neuronCount, "Internal");
            regionNames.resize(neuronCount, "Region_1");
        }
    };
    
    NetworkData networkData;
    
    // Activity visualization data
    struct ActivityData {
        std::vector<double> neuronActivations;
        std::vector<double> membranePotentials;
        std::vector<double> firingRates;
        std::chrono::system_clock::time_point lastUpdate;
        
        ActivityData(size_t neuronCount) {
            neuronActivations.resize(neuronCount, 0.0);
            membranePotentials.resize(neuronCount, 0.0);
            firingRates.resize(neuronCount, 0.0);
        }
    };
    
    ActivityData activityData;
    
    // Spike visualization data
    std::vector<SpikeEvent> spikeHistory;
    size_t maxSpikeHistorySize;
    
    // Weight visualization data
    std::vector<std::vector<double>> weightMatrix;
    double minWeight;
    double maxWeight;
    
    // Debugging session data
    std::unordered_map<std::string, std::string> debugSessionData;
    bool debugMode;
    
    Impl() : active(false), updateRate(30.0), maxSpikeHistorySize(1000), minWeight(0.0), maxWeight(1.0), debugMode(false) {
        auto now = std::chrono::system_clock::now();
        currentPerfData.startTime = now;
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(std::make_unique<Impl>()) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    NLM_LOG_INFO("Initializing Visualization Interface...");
    
    try {
        // Initialize performance monitoring
        auto now = std::chrono::system_clock::now();
        pImpl->currentPerfData.startTime = now;
        pImpl->currentPerfData.endTime = now;
        
        // Initialize network data
        size_t neuronCount = 1000; // Default value - should be obtained from brain
        pImpl->networkData = Impl::NetworkData(neuronCount);
        
        // Initialize activity data
        pImpl->activityData = Impl::ActivityData(neuronCount);
        
        // Initialize weight matrix
        pImpl->weightMatrix.resize(neuronCount, std::vector<double>(neuronCount, 0.0));
        pImpl->minWeight = 0.0;
        pImpl->maxWeight = 1.0;
        
        pImpl->active = true;
        NLM_LOG_INFO("Visualization Interface initialized successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception initializing Visualization Interface: " + std::string(e.what()));
        return false;
    }
}

void VisualizationInterface::update() {
    if (!pImpl->active) {
        return;
    }
    
    auto now = std::chrono::system_clock::now();
    pImpl->currentPerfData.endTime = now;
    
    // Update performance data
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->currentPerfData.startTime);
    pImpl->currentPerfData.cpuTime = duration.count() / 1000.0; // Convert to seconds
    
    // Update activity data (placeholder)
    for (size_t i = 0; i < pImpl->activityData.neuronActivations.size(); ++i) {
        pImpl->activityData.neuronActivations[i] = (static_cast<double>(rand()) / RAND_MAX) * 0.1;
        pImpl->activityData.membranePotentials[i] = (static_cast<double>(rand()) / RAND_MAX) * 100.0 - 50.0;
        pImpl->activityData.firingRates[i] = (static_cast<double>(rand()) / RAND_MAX) * 50.0;
    }
    pImpl->activityData.lastUpdate = now;
    
    // Update spike history (keep only recent spikes)
    if (pImpl->spikeHistory.size() > pImpl->maxSpikeHistorySize) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin(), 
                               pImpl->spikeHistory.begin() + 
                               (pImpl->spikeHistory.size() - pImpl->maxSpikeHistorySize));
    }
    
    NLM_LOG_DEBUG("Visualization updated");
}

void VisualizationInterface::render() {
    if (!pImpl->active) {
        return;
    }
    
    try {
        // Generate visualization data
        NLM_LOG_DEBUG("Rendering visualization data");
        
        // Log performance statistics
        NLM_LOG_INFO("=== Performance Statistics ===");
        NLM_LOG_INFO("CPU time: " + std::to_string(pImpl->currentPerfData.cpuTime) + "s");
        NLM_LOG_INFO("Steps processed: " + std::to_string(pImpl->currentPerfData.stepCount));
        NLM_LOG_INFO("Spikes recorded: " + std::to_string(pImpl->currentPerfData.spikeCount));
        NLM_LOG_INFO("Memory usage: " + std::to_string(pImpl->currentPerfData.memoryUsage) + "MB");
        
        // Generate network visualization data
        NLM_LOG_DEBUG("Generating network visualization data");
        
        // Generate activity visualization data
        NLM_LOG_DEBUG("Generating activity visualization data");
        
        // Generate spike visualization data
        if (!pImpl->spikeHistory.empty()) {
            NLM_LOG_INFO("Spike activity: " + std::to_string(pImpl->spikeHistory.size()) + " spikes in history");
        }
        
        NLM_LOG_DEBUG("Visualization rendered successfully");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception rendering visualization: " + std::string(e.what()));
    }
}

void VisualizationInterface::close() {
    if (pImpl->active) {
        NLM_LOG_INFO("Closing Visualization Interface...");
        pImpl->active = false;
        NLM_LOG_INFO("Visualization Interface closed");
    }
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    NLM_LOG_INFO("Visualization update rate set to: " + std::to_string(hz) + " Hz");
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    if (!pImpl->active) {
        return;
    }
    
    try {
        NLM_LOG_INFO("Visualizing network structure...");
        
        // Get brain statistics
        size_t neuronCount = brain.getTotalNeuronCount();
        size_t regionCount = brain.getRegionCount();
        size_t synapseCount = brain.getTotalSynapseCount();
        
        NLM_LOG_INFO("Network statistics:");
        NLM_LOG_INFO("  Neurons: " + std::to_string(neuronCount));
        NLM_LOG_INFO("  Regions: " + std::to_string(regionCount));
        NLM_LOG_INFO("  Synapses: " + std::to_string(synapseCount));
        NLM_LOG_INFO("  E/I ratio: " + std::to_string(brain.getExcitationInhibitionRatio()));
        
        // Generate network visualization data
        pImpl->networkData.adjacencyMatrix.clear();
        pImpl->networkData.adjacencyMatrix.resize(neuronCount, std::vector<double>(neuronCount, 0.0));
        
        // Fill adjacency matrix with synaptic connections
        for (const auto& region : brain.getRegions()) {
            for (const auto& syn : region->getSynapses()) {
                size_t source = syn->getSourceNeuron().index();
                size_t target = syn->getDestinationNeuron().index();
                if (source < neuronCount && target < neuronCount) {
                    pImpl->networkData.adjacencyMatrix[source][target] = syn->getWeight();
                }
            }
        }
        
        // Generate region information
        pImpl->networkData.regionNames.clear();
        for (size_t i = 0; i < neuronCount; ++i) {
            pImpl->networkData.regionNames.push_back("Region_1"); // Simplified - should be actual region name
        }
        
        // Generate neuron type information
        pImpl->networkData.neuronTypes.clear();
        for (size_t i = 0; i < neuronCount; ++i) {
            pImpl->networkData.neuronTypes.push_back("Internal"); // Simplified - should be actual neuron type
        }
        
        NLM_LOG_INFO("Network visualization data generated");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in visualizeNetwork: " + std::string(e.what()));
    }
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (!pImpl->active) {
        return;
    }
    
    try {
        NLM_LOG_INFO("Visualizing neural activity...");
        
        size_t neuronCount = brain.getTotalNeuronCount();
        
        // Update activity data
        pImpl->activityData.neuronActivations.clear();
        pImpl->activityData.membranePotentials.clear();
        pImpl->activityData.firingRates.clear();
        
        pImpl->activityData.neuronActivations.resize(neuronCount, 0.0);
        pImpl->activityData.membranePotentials.resize(neuronCount, 0.0);
        pImpl->activityData.firingRates.resize(neuronCount, 0.0);
        
        // Get brain statistics
        pImpl->currentPerfData.stepCount = 0; // This should come from brain
        pImpl->currentPerfData.spikeCount = brain.getTotalSpikeCount();
        
        NLM_LOG_INFO("Activity statistics:");
        NLM_LOG_INFO("  Active neurons: " + std::to_string(brain.getActiveNeuronCount()));
        NLM_LOG_INFO("  Firing neurons: " + std::to_string(brain.getFiringNeuronCount()));
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain.getTotalSpikeCount()));
        NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain.getAverageFiringRate()));
        
        NLM_LOG_INFO("Activity visualization data generated");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in visualizeActivity: " + std::string(e.what()));
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (!pImpl->active) {
        return;
    }
    
    try {
        NLM_LOG_INFO("Visualizing spike activity...");
        
        // Add spikes to history
        pImpl->spikeHistory.insert(pImpl->spikeHistory.end(), spikes.begin(), spikes.end());
        
        NLM_LOG_INFO("Spike activity statistics:");
        NLM_LOG_INFO("  Spikes in history: " + std::to_string(pImpl->spikeHistory.size()));
        
        // Analyze spike patterns
        if (!pImpl->spikeHistory.empty()) {
            size_t maxSpikesPerNeuron = 0;
            size_t neuronsWithSpikes = 0;
            
            std::unordered_map<NeuronId, size_t> spikesPerNeuron;
            for (const auto& spike : pImpl->spikeHistory) {
                spikesPerNeuron[spike.neuronId]++;
            }
            
            for (const auto& pair : spikesPerNeuron) {
                if (pair.second > maxSpikesPerNeuron) {
                    maxSpikesPerNeuron = pair.second;
                }
                neuronsWithSpikes++;
            }
            
            NLM_LOG_INFO("  Neurons with spikes: " + std::to_string(neuronsWithSpikes));
            NLM_LOG_INFO("  Max spikes per neuron: " + std::to_string(maxSpikesPerNeuron));
        }
        
        NLM_LOG_INFO("Spike visualization data generated");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in visualizeSpikes: " + std::string(e.what()));
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (!pImpl->active) {
        return;
    }
    
    try {
        NLM_LOG_INFO("Visualizing synaptic weights...");
        
        size_t neuronCount = brain.getTotalNeuronCount();
        
        // Generate weight matrix
        pImpl->weightMatrix.clear();
        pImpl->weightMatrix.resize(neuronCount, std::vector<double>(neuronCount, 0.0));
        
        // Fill weight matrix
        double minWeight = std::numeric_limits<double>::max();
        double maxWeight = std::numeric_limits<double>::lowest();
        
        for (const auto& region : brain.getRegions()) {
            for (const auto& syn : region->getSynapses()) {
                size_t source = syn->getSourceNeuron().index();
                size_t target = syn->getDestinationNeuron().index();
                float weight = syn->getWeight();
                
                if (source < neuronCount && target < neuronCount) {
                    pImpl->weightMatrix[source][target] = weight;
                    
                    if (weight < minWeight) minWeight = weight;
                    if (weight > maxWeight) maxWeight = weight;
                }
            }
        }
        
        pImpl->minWeight = minWeight;
        pImpl->maxWeight = maxWeight;
        
        NLM_LOG_INFO("Weight visualization statistics:");
        NLM_LOG_INFO("  Neurons: " + std::to_string(neuronCount));
        NLM_LOG_INFO("  Minimum weight: " + std::to_string(minWeight));
        NLM_LOG_INFO("  Maximum weight: " + std::to_string(maxWeight));
        NLM_LOG_INFO("  Average weight: " + std::to_string((minWeight + maxWeight) / 2.0));
        
        NLM_LOG_INFO("Weight visualization data generated");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in visualizeWeights: " + std::string(e.what()));
    }
}

// Performance profiling helper functions
void VisualizationInterface::startPerformanceProfile() {
    if (pImpl->active) {
        auto now = std::chrono::system_clock::now();
        pImpl->currentPerfData.startTime = now;
        NLM_LOG_INFO("Performance profiling started");
    }
}

void VisualizationInterface::endPerformanceProfile() {
    if (pImpl->active) {
        auto now = std::chrono::system_clock::now();
        pImpl->currentPerfData.endTime = now;
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->currentPerfData.startTime);
        pImpl->currentPerfData.cpuTime = duration.count() / 1000.0;
        
        NLM_LOG_INFO("Performance profiling ended. Duration: " + std::to_string(pImpl->currentPerfData.cpuTime) + "s");
    }
}

double VisualizationInterface::getProfileDuration() const {
    if (pImpl->active && pImpl->currentPerfData.startTime.time_since_epoch().count() > 0) {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->currentPerfData.startTime);
        return duration.count() / 1000.0;
    }
    return 0.0;
}

// Debug session management
void VisualizationInterface::startDebugSession(const std::string& sessionName) {
    if (pImpl->active) {
        pImpl->debugMode = true;
        pImpl->debugSessionData["session_name"] = sessionName;
        pImpl->debugSessionData["start_time"] = std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
        
        NLM_LOG_INFO("Debug session started: " + sessionName);
    }
}

void VisualizationInterface::endDebugSession() {
    if (pImpl->debugMode && pImpl->active) {
        pImpl->debugSessionData["end_time"] = std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
        pImpl->debugMode = false;
        
        NLM_LOG_INFO("Debug session ended");
    }
}

bool VisualizationInterface::isDebugMode() const {
    return pImpl->debugMode && pImpl->active;
}

void VisualizationInterface::setDebugData(const std::string& key, const std::string& value) {
    if (pImpl->active) {
        pImpl->debugSessionData[key] = value;
    }
}

std::string VisualizationInterface::getDebugData(const std::string& key) const {
    if (pImpl->active) {
        auto it = pImpl->debugSessionData.find(key);
        if (it != pImpl->debugSessionData.end()) {
            return it->second;
        }
    }
    return "";
}

// Export debugging data
bool VisualizationInterface::exportDebugData(const std::string& filepath) {
    if (!pImpl->active || !pImpl->debugMode) {
        return false;
    }
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for debug data export: " + filepath);
            return false;
        }
        
        file << "# NLM Debug Data Export" << std::endl;
        file << "# Session: " << getDebugData("session_name") << std::endl;
        file << "# Start Time: " << getDebugData("start_time") << std::endl;
        if (isDebugMode()) {
            file << "# End Time: " << getDebugData("end_time") << std::endl;
        }
        file << std::endl;
        
        file << "=== Performance Data ===" << std::endl;
        file << "CPU Time: " << pImpl->currentPerfData.cpuTime << "s" << std::endl;
        file << "Memory Usage: " << pImpl->currentPerfData.memoryUsage << "MB" << std::endl;
        file << "Steps: " << pImpl->currentPerfData.stepCount << std::endl;
        file << "Spikes: " << pImpl->currentPerfData.spikeCount << std::endl;
        file << std::endl;
        
        file << "=== Debug Session Data ===" << std::endl;
        for (const auto& pair : pImpl->debugSessionData) {
            file << pair.first << " = " << pair.second << std::endl;
        }
        
        file << std::endl;
        file << "=== Spike History ===" << std::endl;
        for (size_t i = 0; i < pImpl->spikeHistory.size() && i < 100; ++i) {
            const auto& spike = pImpl->spikeHistory[i];
            file << "Spike " << i << ": Neuron ID " << spike.neuronId.index() 
                 << ", Time " << spike.time << ", Step " << spike.step << std::endl;
        }
        
        if (pImpl->spikeHistory.size() > 100) {
            file << "... (" << pImpl->spikeHistory.size() - 100 << " more spikes)" << std::endl;
        }
        
        NLM_LOG_INFO("Debug data exported to: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception exporting debug data: " + std::string(e.what()));
        return false;
    }
}

// Batch processing for debugging
void VisualizationInterface::batchProcess(const Brain& brain, size_t steps) {
    if (!pImpl->active) {
        return;
    }
    
    NLM_LOG_INFO("Starting batch processing: " + std::to_string(steps) + " steps");
    
    startPerformanceProfile();
    
    // Process steps in batches
    for (size_t i = 0; i < steps; ++i) {
        // In a real implementation, this would call brain.step(i)
        // For now, we'll simulate the processing
        
        // Simulate spike generation
        if ((rand() % 100) < 10) { // 10% chance of spike
            SpikeEvent spike(static_cast<NeuronId>(i % 100), static_cast<Timestamp>(i), static_cast<SimulationStep>(i));
            pImpl->spikeHistory.push_back(spike);
            pImpl->currentPerfData.spikeCount++;
        }
        
        // Update performance data
        pImpl->currentPerfData.stepCount++;
        
        // Periodically visualize
        if (i % 100 == 0) {
            NLM_LOG_DEBUG("Batch processing step " + std::to_string(i));
        }
    }
    
    endPerformanceProfile();
    
    NLM_LOG_INFO("Batch processing completed. Processed " + std::to_string(steps) + " steps");
}

} // namespace nlm
