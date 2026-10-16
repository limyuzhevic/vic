#include "DistributedBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <thread>
#include <chrono>
#include <algorithm>
#include <numeric>

namespace nlm {

struct DistributedBrain::Impl {
    DistributedConfig config;
    std::shared_ptr<Config> coreConfig;
    std::unique_ptr<Brain> brain;
    std::unique_ptr<Communicator> communicator;
    std::unique_ptr<LoadBalancer> loadBalancer;
    std::unique_ptr<CheckpointManager> checkpointManager;
    std::unique_ptr<FaultTolerance> faultTolerance;
    
    // Node management
    struct NodeInfo {
        int id;
        std::string host;
        int port;
        bool active;
        float lastHeartbeat;
        int eventCount;\n    };
    
    std::unordered_map<int, NodeInfo> nodes;
    mutable std::mutex nodesMutex;
    
    // Communication statistics
    std::vector<float> communicationTimes;
    std::vector<float> synchronizationTimes;
    std::atomic<size_t> bufferedEventsCount{0};
    std::mutex statsMutex;
    
    // Neural data buffers for communication
    std::unordered_map<int, std::vector<float>> receivedNeuralData;
    std::mutex neuralDataMutex;
    
    // Spike event buffers for communication
    std::vector<SpikeEvent> spikeBuffer;
    std::mutex spikeBufferMutex;
    
    // Performance monitoring
    std::chrono::high_resolution_clock::time_point lastUpdate;
    
    Impl(std::shared_ptr<Config> cfg) 
        : coreConfig(cfg), lastUpdate(std::chrono::high_resolution_clock::now()) {
        config.nodeId = 0;  // Will be set by MPI rank
        config.totalNodes = 1;
        config.enableDistributedMode = false;
        nodes[0] = {0, "localhost", 0, true, 0.0f, 0};
    }
};

DistributedBrain::DistributedBrain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

DistributedBrain::~DistributedBrain() = default;

DistributedBrain::DistributedBrain(DistributedBrain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

DistributedBrain& DistributedBrain::operator=(DistributedBrain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool DistributedBrain::initialize(bool useMpi) {
    try {
        NLM_LOG_INFO("Initializing DistributedBrain (Node " + std::to_string(pImpl->config.nodeId) + ")");
        
        // Initialize underlying brain
        pImpl->brain = std::make_unique<Brain>(pImpl->coreConfig);
        if (!pImpl->brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize underlying brain");
            return false;
        }
        
        // Configure distributed components based on mode
        if (pImpl->config.enableDistributedMode && useMpi) {
            NLM_LOG_INFO("Initializing MPI communication layer...");
            pImpl->communicator = std::make_unique<Communicator>(pImpl->config);
            if (!pImpl->communicator->initialize()) {
                NLM_LOG_ERROR("Failed to initialize MPI communicator");
                return false;
            }
            
            // Update node configuration from MPI
            pImpl->config.nodeId = pImpl->communicator->getRank();
            pImpl->config.totalNodes = pImpl->communicator->getSize();
            
            // Add other nodes to node list
            for (int i = 0; i < pImpl->config.totalNodes; ++i) {
                if (i != pImpl->config.nodeId) {
                    addNode(i, "localhost", pImpl->config.mpiPort.empty() ? 12345 : std::stoi(pImpl->config.mpiPort));
                }
            }
            
            // Initialize load balancer for distributed computation
            pImpl->loadBalancer = std::make_unique<LoadBalancer>(pImpl->config);
            pImpl->loadBalancer->initialize(pImpl->brain.get());
            
            // Initialize checkpoint manager for distributed systems
            if (pImpl->config.enableCheckpointing) {
                pImpl->checkpointManager = std::make_unique<CheckpointManager>(pImpl->config);
                pImpl->checkpointManager->initialize();
            }
            
            // Initialize fault tolerance system
            if (pImpl->config.enableFaultTolerance) {
                pImpl->faultTolerance = std::make_unique<FaultTolerance>(pImpl->config);
                pImpl->faultTolerance->initialize(pImpl->brain.get());
            }
        }
        
        NLM_LOG_INFO("DistributedBrain initialization complete (Node " + std::to_string(pImpl->config.nodeId) + "/" + 
                    std::to_string(pImpl->config.totalNodes) + ")");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception initializing DistributedBrain: ") + e.what());
        return false;
    }
}

void DistributedBrain::step(SimulationStep currentStep, Timestamp currentTime) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (pImpl->config.enableDistributedMode) {
        // Distributed computation mode
        // 1. Synchronize neural state if needed
        if (currentStep % static_cast<SimulationStep>(1.0f / pImpl->config.communicationInterval) == 0) {
            synchronizeState();
        }
        
        // 2. Process local computation
        pImpl->brain->step(currentStep, currentTime);
        
        // 3. Load balancing and work distribution
        if (pImpl->loadBalancer && 
            currentStep % static_cast<SimulationStep>(1.0f / pImpl->config.loadBalancingInterval) == 0) {
            pImpl->loadBalancer->rebalanceWork(pImpl->brain.get());
        }
        
        // 4. Send spike events to other nodes
        broadcastSpikeEvents(std::vector<SpikeEvent>());  // Simplified - would collect from brain
        
        // 5. Receive neural data from other nodes
        for (auto& [nodeId, node] : pImpl->nodes) {
            if (node.active && nodeId != pImpl->config.nodeId) {
                auto data = requestNeuralData(nodeId, std::vector<int>());  // Simplified
            }
        }
        
        // 6. Apply fault tolerance recovery if needed
        if (pImpl->faultTolerance) {
            pImpl->faultTolerance->checkHealth(pImpl->brain.get());
        }
        
        // 7. Checkpoint if needed
        if (pImpl->checkpointManager && 
            currentStep % static_cast<SimulationStep>(1000.0f) == 0) {  // Every 1000th step
            std::string checkpointPath = pImpl->config.checkpointDir + "/step_" + 
                                        std::to_string(currentStep) + "_node_" + 
                                        std::to_string(pImpl->config.nodeId) + ".bin";
            save(checkpointPath);
        }
    } else {
        // Single-node mode (backward compatibility)
        pImpl->brain->step(currentStep, currentTime);
    }
    
    // Update performance metrics
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<float>(endTime - startTime);
    
    std::lock_guard<std::mutex> lock(pImpl->statsMutex);
    pImpl->communicationTimes.push_back(duration.count());
    if (pImpl->communicationTimes.size() > 100) {
        pImpl->communicationTimes.erase(pImpl->communicationTimes.begin());
    }
}

void DistributedBrain::receiveSensoryInput(const class SensoryInput& input) {
    pImpl->brain->receiveSensoryInput(input);
}

void DistributedBrain::injectCurrent(NeuronId neuron, MembranePotential current) {
    pImpl->brain->injectCurrent(neuron, current);
}

void DistributedBrain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    pImpl->brain->injectCurrentToNeurons(type, current);
}

std::vector<float> DistributedBrain::requestNeuralData(int sourceNode, const std::vector<int>& neuronIds) {
    std::vector<float> result;
    
    if (pImpl->config.enableDistributedMode && pImpl->communicator) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Simulate neural data request (in real implementation, this would use MPI)
        // For now, just return empty vector
        result = std::vector<float>(neuronIds.size(), 0.0f);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<float>(endTime - startTime);
        
        std::lock_guard<std::mutex> lock(pImpl->statsMutex);
        pImpl->synchronizationTimes.push_back(duration.count());
        if (pImpl->synchronizationTimes.size() > 100) {
            pImpl->synchronizationTimes.erase(pImpl->synchronizationTimes.begin());
        }
    }
    
    return result;
}

void DistributedBrain::broadcastSpikeEvents(const std::vector<SpikeEvent>& events) {
    if (pImpl->config.enableDistributedMode && pImpl->communicator) {
        std::lock_guard<std::mutex> lock(pImpl->spikeBufferMutex);
        pImpl->spikeBuffer.insert(pImpl->spikeBuffer.end(), events.begin(), events.end());
        
        // Remove excess events to prevent memory issues
        if (pImpl->spikeBuffer.size() > static_cast<size_t>(pImpl->config.maxBufferedEvents)) {
            pImpl->spikeBuffer.erase(pImpl->spikeBuffer.begin(), 
                                   pImpl->spikeBuffer.begin() + 
                                   (pImpl->spikeBuffer.size() - pImpl->config.maxBufferedEvents));
        }
        
        pImpl->bufferedEventsCount += events.size();
    }
}

void DistributedBrain::synchronizeState() {
    if (!pImpl->config.enableDistributedMode || !pImpl->communicator) return;
    
    // Simulate state synchronization (in real implementation, this would use MPI_Allgather or similar)
    // For now, just log the operation
    if (pImpl->config.nodeId == 0) {
        NLM_LOG_INFO("Synchronizing neural state across " + std::to_string(pImpl->config.totalNodes) + " nodes");
    }
}

float DistributedBrain::getExcitationInhibitionRatio() const {
    return pImpl->brain->getExcitationInhibitionRatio();
}

size_t DistributedBrain::getTotalSpikeCount() const {
    return pImpl->brain->getTotalSpikeCount();
}

size_t DistributedBrain::getPendingSpikeEventCount() const {
    return pImpl->brain->getPendingSpikeEventCount();
}

std::unique_ptr<class Action> DistributedBrain::produceAction() {
    return pImpl->brain->produceAction();
}

void DistributedBrain::applyNeuromodulation(const class Neuromodulator& signal) {
    pImpl->brain->applyNeuromodulation(signal);
}

void DistributedBrain::updatePlasticity() {
    pImpl->brain->updatePlasticity();
}

void DistributedBrain::develop() {
    pImpl->brain->develop();
}

void DistributedBrain::reset() {
    pImpl->brain->reset();
    
    // Clear distributed buffers
    {
        std::lock_guard<std::mutex> lock(pImpl->neuralDataMutex);
        pImpl->receivedNeuralData.clear();
    }
    
    {
        std::lock_guard<std::mutex> lock(pImpl->spikeBufferMutex);
        pImpl->spikeBuffer.clear();
    }
    
    std::lock_guard<std::mutex> statsLock(pImpl->statsMutex);
    pImpl->bufferedEventsCount = 0;
}

bool DistributedBrain::save(const std::string& filepath) const {
    bool coreSuccess = pImpl->brain->save(filepath);
    
    if (pImpl->config.enableDistributedMode && pImpl->checkpointManager) {
        // Save distributed-specific state
        // For now, just save core state
        return coreSuccess;
    }
    
    return coreSuccess;
}

bool DistributedBrain::load(const std::string& filepath) {
    bool coreSuccess = pImpl->brain->load(filepath);
    
    if (pImpl->config.enableDistributedMode && pImpl->checkpointManager) {
        // Load distributed-specific state
        // For now, just load core state
    }
    
    return coreSuccess;
}

void DistributedBrain::setDistributedConfig(const DistributedConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->nodesMutex);
    pImpl->config = config;
}

const DistributedConfig& DistributedBrain::getDistributedConfig() const {
    return pImpl->config;
}

void DistributedBrain::addNode(int nodeId, const std::string& host, int port) {
    std::lock_guard<std::mutex> lock(pImpl->nodesMutex);
    
    NodeInfo node;
    node.id = nodeId;
    node.host = host;
    node.port = port;
    node.active = true;
    node.lastHeartbeat = 0.0f;
    node.eventCount = 0;
    
    pImpl->nodes[nodeId] = node;
}

void DistributedBrain::removeNode(int nodeId) {
    std::lock_guard<std::mutex> lock(pImpl->nodesMutex);
    pImpl->nodes.erase(nodeId);
}

bool DistributedBrain::isNodeActive(int nodeId) const {
    std::lock_guard<std::mutex> lock(pImpl->nodesMutex);
    auto it = pImpl->nodes.find(nodeId);
    return it != pImpl->nodes.end() && it->second.active;
}

std::vector<int> DistributedBrain::getActiveNodes() const {
    std::lock_guard<std::mutex> lock(pImpl->nodesMutex);
    std::vector<int> activeNodes;
    for (const auto& pair : pImpl->nodes) {
        if (pair.second.active) {
            activeNodes.push_back(pair.first);
        }
    }
    return activeNodes;
}

float DistributedBrain::getAverageCommunicationTime() const {
    std::lock_guard<std::mutex> lock(pImpl->statsMutex);
    if (pImpl->communicationTimes.empty()) return 0.0f;
    
    float sum = std::accumulate(pImpl->communicationTimes.begin(), 
                               pImpl->communicationTimes.end(), 0.0f);
    return sum / static_cast<float>(pImpl->communicationTimes.size());
}

float DistributedBrain::getAverageSynchronizationTime() const {
    std::lock_guard<std::mutex> lock(pImpl->statsMutex);
    if (pImpl->synchronizationTimes.empty()) return 0.0f;
    
    float sum = std::accumulate(pImpl->synchronizationTimes.begin(), 
                               pImpl->synchronizationTimes.end(), 0.0f);
    return sum / static_cast<float>(pImpl->synchronizationTimes.size());
}

size_t DistributedBrain::getBufferedEventsCount() const {
    return pImpl->bufferedEventsCount.load();
}

void DistributedBrain::logStatus() const {
    pImpl->brain->logStatus();
    
    NLM_LOG_INFO("=== DistributedBrain Status (Node " + std::to_string(pImpl->config.nodeId) + "/" + 
                std::to_string(pImpl->config.totalNodes) + ") ===");
    NLM_LOG_INFO("Distributed Mode: " + std::string(pImpl->config.enableDistributedMode ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("Node ID: " + std::to_string(pImpl->config.nodeId));
    NLM_LOG_INFO("Total Nodes: " + std::to_string(pImpl->config.totalNodes));
    NLM_LOG_INFO("Active Nodes: " + std::to_string(getActiveNodes().size()));
    NLM_LOG_INFO("Buffered Events: " + std::to_string(getBufferedEventsCount()));
    NLM_LOG_INFO("Average Communication Time: " + std::to_string(getAverageCommunicationTime() * 1000.0f) + "ms");
    NLM_LOG_INFO("Average Synchronization Time: " + std::to_string(getAverageSynchronizationTime() * 1000.0f) + "ms");
}

} // namespace nlm
