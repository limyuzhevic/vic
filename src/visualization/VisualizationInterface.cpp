#include "VisualizationInterface.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <random>

namespace nlm {

struct VisualizationInterface::Impl {
    bool running;
    double updateRate;
    int port;
    std::thread serverThread;
    std::atomic<bool> stopFlag;
    
    // Brain data storage
    std::unordered_map<std::string, std::shared_ptr<Brain>> brains;
    std::unordered_map<std::string, std::vector<NeuralActivityData>> neuralActivityHistory;
    std::unordered_map<std::string, std::vector<SynapseData>> synapseHistory;
    std::unordered_map<std::string, std::vector<SpikeData>> spikeHistory;
    
    // WebSocket client connections (simplified)
    std::unordered_map<int, std::string> clients;
    int nextClientId;
    
    // Thread safety
    std::mutex dataMutex;
    std::mutex clientsMutex;
    
    Impl() : running(false), updateRate(30.0), port(8080), 
             stopFlag(false), nextClientId(1) {
    }
    
    ~Impl() {
        stop();
    }
    
    void stop() {
        stopFlag = true;
        if (serverThread.joinable()) {
            serverThread.join();
        }
        running = false;
    }
    
    void addBrain(const std::string& brainId, const Brain& brain) {
        std::lock_guard<std::mutex> lock(dataMutex);
        
        // Create a copy of the brain for visualization
        // In a real implementation, you'd want to pass references or smart pointers
        // For now, we'll create a simple wrapper
        auto brainCopy = std::make_shared<Brain>(brain);
        brains[brainId] = brainCopy;
        
        // Initialize history
        neuralActivityHistory[brainId] = std::vector<NeuralActivityData>();
        synapseHistory[brainId] = std::vector<SynapseData>();
        spikeHistory[brainId] = std::vector<SpikeData>();
    }
    
    NeuralActivityData collectNeuralActivity(const std::string& brainId) const {
        NeuralActivityData activity;
        activity.timestamp = std::chrono::duration<double>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        if (brains.find(brainId) != brains.end()) {
            auto& brain = brains.at(brainId);
            
            // Get regions and neurons (simplified implementation)
            auto regions = brain->getRegions();
            for (const auto& region : regions) {
                auto neurons = region->getAllNeurons();
                auto populations = region->getPopulations();
                
                for (const auto& neuron : neurons) {
                    activity.neuronIds.push_back(neuron->getId());
                    activity.regionIds.push_back(region->getId());
                    activity.membranePotentials.push_back(neuron->getMembranePotential());
                    activity.isFiring.push_back(neuron->isFiring());
                    
                    // Determine neuron type from population
                    for (const auto& pop : populations) {
                        if (std::find(pop->getNeurons().begin(), pop->getNeurons().end(), neuron) != pop->getNeurons().end()) {
                            activity.neuronTypes.push_back(pop->getNeuronType() == NeuronType::Motor ? "Motor" : 
                                                        pop->getNeuronType() == NeuronType::Sensory ? "Sensory" :
                                                        pop->getNeuronType() == NeuronType::Internal ? "Internal" : "Unknown");
                            break;
                        }
                    }
                }
            }
        }
        
        return activity;
    }
    
    std::vector<SynapseData> collectSynapseData(const std::string& brainId) const {
        std::vector<SynapseData> synapses;
        
        if (brains.find(brainId) != brains.end()) {
            auto& brain = brains.at(brainId);
            auto regions = brain->getRegions();
            
            for (const auto& region : regions) {
                auto synaps = region->getSynapses();
                auto neurons = region->getAllNeurons();
                
                for (const auto& syn : synaps) {
                    SynapseData synData;
                    
                    // Get synapse information
                    // This is a simplified version - in a real implementation
                    // you'd need to access synapse source and target neurons
                    synData.sourceNeuronId = 0; // Placeholder
                    synData.targetNeuronId = 0; // Placeholder
                    synData.weight = syn->getWeight();
                    synData.isActive = syn->getEligibilityTrace() > 0.001f;
                    synData.lastUpdateTime = std::chrono::duration<double>(
                        std::chrono::system_clock::now().time_since_epoch()).count();
                    
                    synapses.push_back(synData);
                }
            }
        }
        
        return synapses;
    }
    
    std::vector<SpikeData> collectSpikeData(const std::string& brainId, double timeWindowSeconds) const {
        std::vector<SpikeData> spikes;
        auto now = std::chrono::system_clock::now();
        auto timeWindow = std::chrono::duration<double>(timeWindowSeconds);
        
        if (brains.find(brainId) != brains.end()) {
            auto& brain = brains.at(brainId);
            auto regions = brain->getRegions();
            
            for (const auto& region : regions) {
                auto neurons = region->getAllNeurons();
                
                for (const auto& neuron : neurons) {
                    // Get neuron spike history (simplified)
                    // In a real implementation, you'd track spike events
                    auto spikeHistory = neuron->getSpikeHistory();
                    
                    for (const auto& timestamp : spikeHistory) {
                        auto spikeTime = std::chrono::duration<double>(timestamp);
                        auto timeDiff = now - (now - spikeTime);
                        
                        if (timeDiff <= timeWindow) {
                            SpikeData spike;
                            spike.neuronId = neuron->getId();
                            spike.timestamp = std::chrono::duration<double>(timestamp).count();
                            spike.step = timestamp; // Using timestamp as step for simplicity
                            spike.regionId = region->getId();
                            spike.neuronType = "Unknown"; // Would need to determine from population
                            
                            spikes.push_back(spike);
                        }
                    }
                }
            }
        }
        
        return spikes;
    }
    
    std::unordered_map<std::string, std::string> getBrainStats(const std::string& brainId) const {
        std::unordered_map<std::string, std::string> stats;
        
        if (brains.find(brainId) != brains.end()) {
            auto& brain = brains.at(brainId);
            
            stats["neuron_count"] = std::to_string(brain->getTotalNeuronCount());
            stats["synapse_count"] = std::to_string(brain->getTotalSynapseCount());
            stats["active_neuron_count"] = std::to_string(brain->getActiveNeuronCount());
            stats["firing_neuron_count"] = std::to_string(brain->getFiringNeuronCount());
            stats["average_firing_rate"] = std::to_string(brain->getAverageFiringRate());
            stats["excitatory_inhibition_ratio"] = std::to_string(brain->getExcitationInhibitionRatio());
            stats["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
            stats["pending_spike_events"] = std::to_string(brain->getPendingSpikeEventCount());
        }
        
        return stats;
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(std::make_unique<Impl>()) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize(int port) {
    pImpl->port = port;
    pImpl->running = true;
    pImpl->stopFlag = false;
    
    // Start the visualization server in a separate thread
    pImpl->serverThread = std::thread(&VisualizationInterface::start, this);
    
    NLM_LOG_INFO("Visualization server initialized on port " + std::to_string(port));
    return true;
}

void VisualizationInterface::start() {
    NLM_LOG_INFO("Visualization server started");
    
    // Main server loop
    auto lastUpdate = std::chrono::steady_clock::now();
    double updateInterval = 1.0 / pImpl->updateRate; // Convert Hz to seconds
    
    while (!pImpl->stopFlag) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(now - lastUpdate);
        
        // Update data if enough time has passed
        if (elapsed >= updateInterval) {
            update();
            lastUpdate = now;
        }
        
        // Sleep for a short time to avoid busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    NLM_LOG_INFO("Visualization server stopped");
}

void VisualizationInterface::stop() {
    if (pImpl->running) {
        pImpl->stopFlag = true;
        if (pImpl->serverThread.joinable()) {
            pImpl->serverThread.join();
        }
        pImpl->running = false;
        NLM_LOG_INFO("Visualization server stopped");
    }
}

bool VisualizationInterface::isRunning() const {
    return pImpl->running;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    NLM_LOG_INFO("Visualization update rate set to " + std::to_string(hz) + " Hz");
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::addBrain(const std::string& brainId, const Brain& brain) {
    pImpl->addBrain(brainId, brain);
    NLM_LOG_INFO("Brain '" + brainId + "' added to visualization");
}

void VisualizationInterface::removeBrain(const std::string& brainId) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    if (pImpl->brains.find(brainId) != pImpl->brains.end()) {
        pImpl->brains.erase(brainId);
        pImpl->neuralActivityHistory.erase(brainId);
        pImpl->synapseHistory.erase(brainId);
        pImpl->spikeHistory.erase(brainId);
        NLM_LOG_INFO("Brain '" + brainId + "' removed from visualization");
    }
}

NeuralActivityData VisualizationInterface::getNeuralActivity(const std::string& brainId) const {
    return pImpl->collectNeuralActivity(brainId);
}

std::vector<SynapseData> VisualizationInterface::getSynapseData(const std::string& brainId) const {
    return pImpl->collectSynapseData(brainId);
}

std::vector<SpikeData> VisualizationInterface::getSpikeData(const std::string& brainId, double timeWindowSeconds) const {
    return pImpl->collectSpikeData(brainId, timeWindowSeconds);
}

std::unordered_map<std::string, std::string> VisualizationInterface::getBrainStats(const std::string& brainId) const {
    return pImpl->getBrainStats(brainId);
}

void VisualizationInterface::broadcastData() {
    // This would send data to all connected WebSocket clients
    // For now, it's a placeholder
}

void VisualizationInterface::handleConnection(int clientId) {
    // Handle new WebSocket connection
    // This is a placeholder for WebSocket connection handling
}

void VisualizationInterface::handleMessage(int clientId, const std::string& message) {
    // Handle WebSocket message from client
    // This is a placeholder for WebSocket message handling
}

void VisualizationInterface::update() {
    // Update visualization data for all brains
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    double currentTime = std::chrono::duration<double>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    for (const auto& pair : pImpl->brains) {
        const std::string& brainId = pair.first;
        const auto& brain = pair.second;
        
        // Collect neural activity
        auto neuralActivity = pImpl->collectNeuralActivity(brainId);
        pImpl->neuralActivityHistory[brainId].push_back(neuralActivity);
        
        // Keep history size limited
        if (pImpl->neuralActivityHistory[brainId].size() > 1000) {
            pImpl->neuralActivityHistory[brainId].erase(
                pImpl->neuralActivityHistory[brainId].begin());
        }
        
        // Collect synapse data
        auto synapseData = pImpl->collectSynapseData(brainId);
        pImpl->synapseHistory[brainId] = synapseData;
        
        // Collect spike data
        auto spikeData = pImpl->collectSpikeData(brainId, 10.0); // Last 10 seconds
        pImpl->spikeHistory[brainId] = spikeData;
        
        // Log update for demonstration
        NLM_LOG_INFO("Updated visualization data for brain '" + brainId + "': " +
                     std::to_string(neuralActivity.neuronIds.size()) + " neurons, " +
                     std::to_string(spikeData.size()) + " spikes in last 10s");
    }
}

} // namespace nlm