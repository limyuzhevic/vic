#include "LoadBalancer.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>
#include <unordered_set>

namespace nlm {

struct LoadBalancer::Impl {
    LoadBalancingConfig config;
    Brain* brain;
    
    // Current node workload
    NodeWorkload currentWorkload;
    
    // Workload history for analysis
    std::vector<NodeWorkload> workloadHistory;
    std::mutex historyMutex;
    
    // Partition information
    std::vector<Partition> partitions;
    std::unordered_map<NeuronId, Partition*> neuronToPartition;
    std::mutex partitionMutex;
    
    // Work queue
    std::queue<WorkUnit> workQueue;
    std::mutex queueMutex;
    size_t maxQueueSize;
    
    // Load balancing statistics
    std::vector<float> loadHistory;
    std::chrono::high_resolution_clock::time_point lastRebalanceTime;
    uint64_t rebalanceCount;
    
    // Algorithm selection
    std::unique_ptr<PartitionAlgorithm> partitionAlgorithm;
    
    // Random number generator for partitioning
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    
    // Performance metrics
    float totalComputationTime;
    float totalCommunicationTime;
    int computationOperations;
    int communicationOperations;
    
    Impl(const LoadBalancingConfig& cfg) 
        : config(cfg), brain(nullptr), currentWorkload(), maxQueueSize(cfg.maxWorkQueueSize),
          rebalanceCount(0), totalComputationTime(0.0f), totalCommunicationTime(0.0f),
          computationOperations(0), communicationOperations(0),
          uniformDist(0.0f, 1.0f), normalDist(0.0f, 1.0f) {
        
        // Initialize random number generator with time-based seed
        std::random_device rd;
        rng.seed(rd() ^ std::chrono::high_resolution_clock::now().time_since_epoch().count());
        
        // Initialize partition algorithm based on strategy
        switch (config.strategy) {
            case LoadBalancingStrategy::Static:
                // Simple greedy algorithm
                partitionAlgorithm = std::make_unique<GreedyPartitionAlgorithm>();
                break;
            case LoadBalancingStrategy::Dynamic:
                // Use greedy with periodic optimization
                partitionAlgorithm = std::make_unique<GreedyPartitionAlgorithm>();
                break;
            case LoadBalancingStrategy::WorkStealing:
                // Use greedy algorithm with work stealing support
                partitionAlgorithm = std::make_unique<GreedyPartitionAlgorithm>();
                break;
            case LoadBalancingStrategy::RoundRobin:
                // Simple round-robin assignment
                partitionAlgorithm = std::make_unique<GreedyPartitionAlgorithm>();
                break;
            case LoadBalancingStrategy::Adaptive:
                // Use genetic algorithm for better optimization
                partitionAlgorithm = std::make_unique<GeneticPartitionAlgorithm>();
                break;
            default:
                partitionAlgorithm = std::make_unique<GreedyPartitionAlgorithm>();
        }
        
        // Configure algorithm parameters
        if (auto* greedy = dynamic_cast<GreedyPartitionAlgorithm*>(partitionAlgorithm.get())) {
            greedy->setLoadBalancing(0.8f);
            greedy->setMaxPartitionSize(1000);
        }
    }
};

LoadBalancer::LoadBalancer(const LoadBalancingConfig& config) : pImpl(new Impl(config)) {}

LoadBalancer::~LoadBalancer() = default;

LoadBalancer::LoadBalancer(LoadBalancer&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

LoadBalancer& LoadBalancer::operator=(LoadBalancer&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool LoadBalancer::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("LoadBalancer: Invalid brain pointer");
        return false;
    }
    
    pImpl->brain = brain;
    pImpl->lastRebalanceTime = std::chrono::high_resolution_clock::now();
    
    // Initialize current workload with brain metrics
    pImpl->currentWorkload.nodeId = 0;  // Will be set by MPI rank
    pImpl->currentWorkload.neuronCount = brain->getTotalNeuronCount();
    pImpl->currentWorkload.activeNeuronCount = brain->getActiveNeuronCount();
    pImpl->currentWorkload.spikeCount = brain->getTotalSpikeCount();
    pImpl->currentWorkload.memoryUsageMB = 0.0f;  // Estimate based on brain size
    pImpl->currentWorkload.cpuUsagePercent = 0.0f;  // Will be measured
    pImpl->currentWorkload.communicationTime = 0.0f;
    pImpl->currentWorkload.computationTime = 0.0f;
    pImpl->currentWorkload.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    // Calculate initial load score
    float maxNeurons = 10000.0f;  // Maximum expected neuron count
    pImpl->currentWorkload.loadScore = std::min(1.0f, 
        static_cast<float>(pImpl->currentWorkload.neuronCount) / maxNeurons);
    
    NLM_LOG_INFO("LoadBalancer initialized with " + std::to_string(pImpl->currentWorkload.neuronCount) + 
                " neurons, load score: " + std::to_string(pImpl->currentWorkload.loadScore));
    
    return true;
}

bool LoadBalancer::rebalance(Brain* brain, const std::vector<NodeWorkload>& currentWorkloads) {
    if (!brain || currentWorkloads.empty()) {
        return false;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Calculate average load
    float totalLoad = 0.0f;
    for (const auto& workload : currentWorkloads) {
        totalLoad += workload.loadScore;
    }
    float avgLoad = totalLoad / static_cast<float>(currentWorkloads.size());
    
    // Check if rebalancing is needed
    float loadStdDev = getLoadStandardDeviation();
    if (loadStdDev < config.rebalancingThreshold * 0.1f && 
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now() - pImpl->lastRebalanceTime).count() < 
        config.rebalancingInterval) {
        // Not enough imbalance, skip rebalancing
        NLM_LOG_INFO("LoadBalancer: Skipping rebalancing (low imbalance: " + 
                    std::to_string(loadStdDev) + ")");
        return true;
    }
    
    NLM_LOG_INFO("LoadBalancer: Rebalancing workload across " + 
                std::to_string(currentWorkloads.size()) + " nodes (avg load: " + 
                std::to_string(avgLoad) + ", std dev: " + std::to_string(loadStdDev) + ")");
    
    // Get neuron information from brain
    std::vector<Neuron*> allNeurons = getAllNeurons(brain);
    
    // Generate new partitions using partition algorithm
    std::vector<Partition> newPartitions = pImpl->partitionAlgorithm->partition(
        allNeurons, currentWorkloads);
    
    if (newPartitions.empty()) {
        NLM_LOG_ERROR("LoadBalancer: Partition algorithm returned empty partitions");
        return false;
    }
    
    // Update partitions
    {
        std::lock_guard<std::mutex> lock(pImpl->partitionMutex);
        pImpl->partitions = std::move(newPartitions);
        
        // Clear old mapping
        pImpl->neuronToPartition.clear();
        
        // Build new mapping
        for (auto& partition : pImpl->partitions) {
            for (NeuronId neuronId : partition.neuronIds) {
                pImpl->neuronToPartition[neuronId] = &partition;
            }
        }
    }
    
    // Update rebalance statistics
    pImpl->rebalanceCount++;
    pImpl->lastRebalanceTime = std::chrono::high_resolution_clock::now();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    float rebalanceTime = std::chrono::duration<float>(endTime - startTime).count() * 1000.0f;  // milliseconds
    
    NLM_LOG_INFO("LoadBalancer: Rebalancing completed in " + std::to_string(rebalanceTime) + 
                "ms, generated " + std::to_string(pImpl->partitions.size()) + " partitions");
    
    return true;
}

bool LoadBalancer::workStealing(const WorkStealRequest& request, WorkStealResponse& response) {
    if (!config.enableWorkStealing) {
        response.accepted = false;
        return true;
    }
    
    // Calculate if this node should steal work
    float currentLoad = getCurrentWorkload().loadScore;
    
    if (currentLoad > config.stealThreshold) {
        // This node has high load, consider stealing
        response.accepted = true;
        response.responderId = pImpl->currentWorkload.nodeId;
        response.stolenWork = std::vector<WorkUnit>();  // Would steal actual work
        response.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        NLM_LOG_INFO("LoadBalancer: Node " + std::to_string(pImpl->currentWorkload.nodeId) + 
                    " stealing work from high-load nodes");
    } else {
        response.accepted = false;
        response.responderId = -1;
        response.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    return true;
}

NeuronId LoadBalancer::getRecommendedPartition(NeuronId neuronId) const {
    std::lock_guard<std::mutex> lock(pImpl->partitionMutex);
    
    auto it = pImpl->neuronToPartition.find(neuronId);
    if (it != pImpl->neuronToPartition.end()) {
        return neuronId;  // Same neuron ID across nodes (abstract)
    }
    
    // Return a default partition if not found
    if (!pImpl->partitions.empty()) {
        return pImpl->partitions[0].neuronIds.front();
    }
    
    return neuronId;
}

void LoadBalancer::updateWorkload(const NodeWorkload& workload) {
    std::lock_guard<std::mutex> lock(pImpl->historyMutex);
    
    pImpl->currentWorkload = workload;
    
    // Add to history (keep last 100 entries)
    if (pImpl->workloadHistory.size() >= 100) {
        pImpl->workloadHistory.erase(pImpl->workloadHistory.begin());
    }
    pImpl->workloadHistory.push_back(workload);
    
    // Update load history
    pImpl->loadHistory.push_back(workload.loadScore);
    if (pImpl->loadHistory.size() > 100) {
        pImpl->loadHistory.erase(pImpl->loadHistory.begin());
    }
}

NodeWorkload LoadBalancer::getCurrentWorkload() const {
    return pImpl->currentWorkload;
}

float LoadBalancer::getAverageLoad() const {
    if (pImpl->loadHistory.empty()) return 0.0f;
    
    float sum = std::accumulate(pImpl->loadHistory.begin(), pImpl->loadHistory.end(), 0.0f);
    return sum / static_cast<float>(pImpl->loadHistory.size());
}

float LoadBalancer::getLoadStandardDeviation() const {
    if (pImpl->loadHistory.size() < 2) return 0.0f;
    
    float avg = getAverageLoad();
    float sumSquaredDiffs = 0.0f;
    
    for (float load : pImpl->loadHistory) {
        float diff = load - avg;
        sumSquaredDiffs += diff * diff;
    }
    
    return std::sqrt(sumSquaredDiffs / static_cast<float>(pImpl->loadHistory.size()));
}

void LoadBalancer::setStrategy(LoadBalancingStrategy strategy) {
    config.strategy = strategy;
    
    // Recreate partition algorithm based on new strategy
    switch (strategy) {
        case LoadBalancingStrategy::Static:
            pImpl->partitionAlgorithm = std::make_unique<GreedyPartitionAlgorithm>();
            break;
        case LoadBalancingStrategy::Dynamic:
            pImpl->partitionAlgorithm = std::make_unique<GreedyPartitionAlgorithm>();
            break;
        case LoadBalancingStrategy::WorkStealing:
            pImpl->partitionAlgorithm = std::make_unique<GreedyPartitionAlgorithm>();
            break;
        case LoadBalancingStrategy::RoundRobin:
            pImpl->partitionAlgorithm = std::make_unique<GreedyPartitionAlgorithm>();
            break;
        case LoadBalancingStrategy::Adaptive:
            pImpl->partitionAlgorithm = std::make_unique<GeneticPartitionAlgorithm>();
            break;
        default:
            pImpl->partitionAlgorithm = std::make_unique<GreedyPartitionAlgorithm>();
    }
}

void LoadBalancer::configure(float threshold, uint64_t interval) {
    config.rebalancingThreshold = threshold;
    config.rebalancingInterval = interval;
}

bool LoadBalancer::needsRebalancing() const {
    float loadStdDev = getLoadStandardDeviation();
    return loadStdDev >= config.rebalancingThreshold;
}

const Partition* LoadBalancer::getPartition(NeuronId neuronId) const {
    std::lock_guard<std::mutex> lock(pImpl->partitionMutex);
    
    auto it = pImpl->neuronToPartition.find(neuronId);
    if (it != pImpl->neuronToPartition.end()) {
        return it->second;
    }
    
    return nullptr;
}

void LoadBalancer::addNode(int nodeId) {
    NodeWorkload workload;
    workload.nodeId = nodeId;
    workload.loadScore = 0.0f;
    workload.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    updateWorkload(workload);
}

void LoadBalancer::removeNode(int nodeId) {
    // Remove from current workload if it matches
    if (pImpl->currentWorkload.nodeId == nodeId) {
        pImpl->currentWorkload = NodeWorkload();
    }
    
    // Remove from history
    std::lock_guard<std::mutex> lock(pImpl->historyMutex);
    pImpl->workloadHistory.erase(
        std::remove_if(pImpl->workloadHistory.begin(), pImpl->workloadHistory.end(),
                      [nodeId](const NodeWorkload& w) { return w.nodeId == nodeId; }),
        pImpl->workloadHistory.end());
}

std::vector<int> LoadBalancer::getActiveNodes() const {
    std::lock_guard<std::mutex> lock(pImpl->historyMutex);
    
    std::vector<int> activeNodes;
    for (const auto& workload : pImpl->workloadHistory) {
        if (std::find(activeNodes.begin(), activeNodes.end(), workload.nodeId) == activeNodes.end()) {
            activeNodes.push_back(workload.nodeId);
        }
    }
    
    return activeNodes;
}

std::vector<Partition> LoadBalancer::getPartitions() const {
    std::lock_guard<std::mutex> lock(pImpl->partitionMutex);
    return pImpl->partitions;
}

float LoadBalancer::estimateComputationTime(const WorkUnit& work) const {
    // Simple estimation based on work type and size
    float baseTime = 0.001f;  // 1ms base for simple operations
    
    switch (work.operation) {
        case OperationType::Step:
            return baseTime * 1.5f;  // Step operations are more expensive
        case OperationType::Plasticity:
            return baseTime * 2.0f;  // Plasticity calculations are expensive
        case OperationType::Memory:
            return baseTime * 3.0f;  // Memory operations are most expensive
        case OperationType::Prediction:
            return baseTime * 2.5f;  // Prediction is moderately expensive
        case OperationType::Development:
            return baseTime * 4.0f;  // Development is very expensive
        default:
            return baseTime;
    }
}

float LoadBalancer::estimateCommunicationTime(int targetNode) const {
    // Estimate communication time based on network characteristics
    float baseLatency = 0.001f;  // 1ms base latency
    float bandwidthFactor = 1.0f / (config.networkBandwidth > 0 ? config.networkBandwidth : 1000.0f);
    
    // Add some jitter to simulate network variability
    float jitter = (pImpl->rng() / (float)RAND_MAX - 0.5f) * 0.5f * baseLatency;
    
    return (baseLatency + jitter) * bandwidthFactor;
}

std::vector<std::pair<NeuronId, int>> LoadBalancer::generateWorkPlan(
    const std::vector<NeuronId>& neurons) const {
    std::vector<std::pair<NeuronId, int>> workPlan;
    
    // Simple round-robin assignment for now
    int numNodes = getActiveNodes().size();
    if (numNodes == 0) {
        numNodes = 1;
    }
    
    for (size_t i = 0; i < neurons.size(); ++i) {
        int nodeId = i % numNodes;
        workPlan.emplace_back(neurons[i], nodeId);
    }
    
    return workPlan;
}

void LoadBalancer::monitorAndAdjust(const std::vector<NodeWorkload>& workloads) {
    if (workloads.size() < 2) return;
    
    // Calculate load statistics
    float avgLoad = 0.0f;
    float maxLoad = 0.0f;
    float minLoad = 1.0f;
    
    for (const auto& workload : workloads) {
        avgLoad += workload.loadScore;
        maxLoad = std::max(maxLoad, workload.loadScore);
        minLoad = std::min(minLoad, workload.loadScore);
    }
    avgLoad /= static_cast<float>(workloads.size());
    
    // Calculate load imbalance
    float loadImbalance = (maxLoad - minLoad) / avgLoad;
    
    // Adjust configuration based on imbalance
    if (loadImbalance > 0.5f) {
        // High imbalance, increase rebalancing frequency
        config.rebalancingInterval = std::max<uint64_t>(10, config.rebalancingInterval / 2);
        NLM_LOG_INFO("LoadBalancer: High imbalance detected (" + std::to_string(loadImbalance) + ")");
    } else if (loadImbalance < 0.1f) {
        // Low imbalance, can reduce rebalancing frequency
        config.rebalancingInterval = std::min<uint64_t>(1000, config.rebalancingInterval * 2);
    }
    
    // Adjust steal threshold based on average load
    if (avgLoad > 0.8f) {
        config.stealThreshold = 0.85f;  // Lower threshold for stealing
    } else if (avgLoad < 0.5f) {
        config.stealThreshold = 0.9f;  // Higher threshold for stealing
    }
}

std::vector<float> LoadBalancer::predictWorkload(int stepsAhead) const {
    std::vector<float> predictions;
    
    if (pImpl->loadHistory.size() < 2) {
        // Not enough history for prediction
        for (int i = 0; i <= stepsAhead; ++i) {
            predictions.push_back(pImpl->currentWorkload.loadScore);
        }
        return predictions;
    }
    
    // Simple linear extrapolation for now
    float lastLoad = pImpl->loadHistory.back();
    
    for (int i = 0; i <= stepsAhead; ++i) {
        // Add some random walk component
        float noise = (pImpl->rng() / (float)RAND_MAX - 0.5f) * 0.2f;
        predictions.push_back(lastLoad + noise * i);
    }
    
    return predictions;
}

void LoadBalancer::optimizePartitioning() {
    if (!pImpl->brain) return;
    
    // Get current neuron information
    std::vector<Neuron*> allNeurons = getAllNeurons(pImpl->brain);
    
    // Get current workload (simplified - in real implementation would get from all nodes)
    std::vector<NodeWorkload> currentWorkloads;
    currentWorkloads.push_back(pImpl->currentWorkload);
    
    // Optimize partitions using partition algorithm
    pImpl->partitionAlgorithm->optimize(pImpl->partitions);
    
    // Rebuild neuron to partition mapping
    pImpl->neuronToPartition.clear();
    for (auto& partition : pImpl->partitions) {
        for (NeuronId neuronId : partition.neuronIds) {
            pImpl->neuronToPartition[neuronId] = &partition;
        }
    }
    
    NLM_LOG_INFO("LoadBalancer: Partitioning optimized, " + std::to_string(pImpl->partitions.size()) + 
                " partitions");
}

bool LoadBalancer::save(const std::string& filepath) const {
    // For now, just save a simple representation
    std::ofstream file(filepath, std::ios::binary);
    if (!file) {
        return false;
    }
    
    try {
        // Save configuration
        file.write(reinterpret_cast<const char*>(&config), sizeof(LoadBalancingConfig));
        
        // Save current workload
        file.write(reinterpret_cast<const char*>(&pImpl->currentWorkload), sizeof(NodeWorkload));
        
        // Save partition count
        size_t partitionCount = pImpl->partitions.size();
        file.write(reinterpret_cast<const char*>(&partitionCount), sizeof(size_t));
        
        // Save partitions
        for (const auto& partition : pImpl->partitions) {
            file.write(reinterpret_cast<const char*>(&partition), sizeof(Partition));
        }
        
        // Save load history size and data
        size_t historySize = pImpl->loadHistory.size();
        file.write(reinterpret_cast<const char*>(&historySize), sizeof(size_t));
        file.write(reinterpret_cast<const char*>(pImpl->loadHistory.data()), 
                  historySize * sizeof(float));
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error saving load balancer: ") + e.what());
        return false;
    }
    
    return true;
}

bool LoadBalancer::load(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        return false;
    }
    
    try {
        // Load configuration
        file.read(reinterpret_cast<char*>(&config), sizeof(LoadBalancingConfig));
        
        // Load current workload
        file.read(reinterpret_cast<char*>(&pImpl->currentWorkload), sizeof(NodeWorkload));
        
        // Load partition count
        size_t partitionCount;
        file.read(reinterpret_cast<char*>(&partitionCount), sizeof(size_t));
        
        // Load partitions
        pImpl->partitions.clear();
        for (size_t i = 0; i < partitionCount; ++i) {
            Partition partition;
            file.read(reinterpret_cast<char*>(&partition), sizeof(Partition));
            pImpl->partitions.push_back(partition);
        }
        
        // Rebuild neuron to partition mapping
        pImpl->neuronToPartition.clear();
        for (auto& partition : pImpl->partitions) {
            for (NeuronId neuronId : partition.neuronIds) {
                pImpl->neuronToPartition[neuronId] = &partition;
            }
        }
        
        // Load load history
        size_t historySize;
        file.read(reinterpret_cast<char*>(&historySize), sizeof(size_t));
        pImpl->loadHistory.resize(historySize);
        file.read(reinterpret_cast<char*>(pImpl->loadHistory.data()), 
                 historySize * sizeof(float));
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error loading load balancer: ") + e.what());
        return false;
    }
    
    return true;
}

std::vector<Neuron*> LoadBalancer::getAllNeurons(Brain* brain) {
    std::vector<Neuron*> neurons;
    
    if (!brain) return neurons;
    
    // Collect neurons from all regions
    const auto& regions = brain->getRegions();
    for (const auto& region : regions) {
        const auto& populations = region->getPopulations();
        for (const auto& population : populations) {
            const auto& neuronsInPop = population->getNeurons();
            neurons.insert(neurons.end(), neuronsInPop.begin(), neuronsInPop.end());
        }
    }
    
    return neurons;
}

} // namespace nlm
