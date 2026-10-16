#pragma once

#include <brain/Brain.hpp>
#include <brain/Communicator.hpp>
#include <core/Types/Types.hpp>
#include <vector>
#include <unordered_map>
#include <string>
#include <mutex>
#include <atomic>

namespace nlm {

// Load balancing strategies
enum class LoadBalancingStrategy {
    Static,           // Static partitioning of neurons
    Dynamic,          // Dynamic load redistribution
    WorkStealing,     // Work stealing algorithm
    RoundRobin,       // Round-robin work distribution
    Adaptive         // Adaptive based on performance metrics
};

// Workload metrics for a node
struct NodeWorkload {
    int nodeId;
    int neuronCount;
    int activeNeuronCount;
    int spikeCount;
    float memoryUsageMB;
    float cpuUsagePercent;
    float communicationTime;    // milliseconds
    float computationTime;      // milliseconds
    float loadScore;            // normalized load (0.0 to 1.0)
    uint64_t timestamp;
    
    NodeWorkload() : nodeId(0), neuronCount(0), activeNeuronCount(0), spikeCount(0),
                    memoryUsageMB(0.0f), cpuUsagePercent(0.0f), communicationTime(0.0f),
                    computationTime(0.0f), loadScore(0.0f), timestamp(0) {}
};

// Work unit for distributed processing
struct WorkUnit {
    NeuronId id;
    NeuronType type;
    OperationType operation;    // e.g., step, plasticity, checkpoint
    uint64_t timestamp;
    float priority;            // Higher priority tasks
    std::string data;          // Serialized data if needed
    
    WorkUnit() : id(), type(NeuronType::Internal), operation(OperationType::Step),
                 timestamp(0), priority(0.0f) {}
};

// Work stealing request/response
struct WorkStealRequest {
    int requesterId;
    int stealAmount;           // Number of work units to steal
    std::vector<WorkUnit> stolenWork;
    uint64_t timestamp;
};

struct WorkStealResponse {
    int responderId;
    bool accepted;
    std::vector<WorkUnit> stolenWork;
    uint64_t timestamp;
};

// Partition information for distributed brain
struct Partition {
    int nodeId;
    std::vector<NeuronId> neuronIds;
    std::vector<SynapseId> synapseIds;
    float loadScore;
    uint64_t createdAt;
    uint64_t lastUpdated;
    
    Partition() : nodeId(0), loadScore(0.0f), createdAt(0), lastUpdated(0) {}
};

// Load balancing configuration
struct LoadBalancingConfig {
    LoadBalancingStrategy strategy;
    float rebalancingThreshold;    // Threshold for triggering rebalancing (0.0 to 1.0)
    float stealThreshold;          // Load threshold for work stealing (0.0 to 1.0)
    int maxWorkQueueSize;          // Maximum size of work queue
    float communicationOverhead;   // Estimated communication overhead
    float computationCost;         // Estimated computation cost per work unit
    uint64_t rebalancingInterval;   // Steps between rebalancing operations
    bool enableWorkStealing;        // Enable work stealing algorithm
    bool adaptiveLoadBalancing;    // Use adaptive load balancing
    float smoothingFactor;         // For smoothing load metrics
    
    LoadBalancingConfig() : strategy(LoadBalancingStrategy::Adaptive),
                           rebalancingThreshold(0.8f), stealThreshold(0.9f),
                           maxWorkQueueSize(1000), communicationOverhead(0.1f),
                           computationCost(1.0f), rebalancingInterval(100),
                           enableWorkStealing(true), adaptiveLoadBalancing(true),
                           smoothingFactor(0.3f) {}
};

// Load balancer for distributed neural computation
class LoadBalancer {
public:
    // Create load balancer with configuration
    explicit LoadBalancer(const LoadBalancingConfig& config);
    
    ~LoadBalancer();
    
    // Disable copying, enable moving
    LoadBalancer(const LoadBalancer&) = delete;
    LoadBalancer& operator=(const LoadBalancer&) = delete;
    LoadBalancer(LoadBalancer&&) noexcept;
    LoadBalancer& operator=(LoadBalancer&&) noexcept;
    
    // Initialize load balancer
    bool initialize(Brain* brain);
    
    // Rebalance workload across nodes
    bool rebalance(Brain* brain, const std::vector<NodeWorkload>& currentWorkloads);
    
    // Perform work stealing
    bool workStealing(const WorkStealRequest& request, WorkStealResponse& response);
    
    // Get recommended partition for a neuron (local node)
    NeuronId getRecommendedPartition(NeuronId neuronId) const;
    
    // Update node workload metrics
    void updateWorkload(const NodeWorkload& workload);
    
    // Get current node workload
    NodeWorkload getCurrentWorkload() const;
    
    // Get workload statistics
    float getAverageLoad() const;
    float getLoadStandardDeviation() const;
    int getNumNodes() const { return nodeWorkloads.size(); }
    
    // Set load balancing strategy
    void setStrategy(LoadBalancingStrategy strategy);
    
    // Configure rebalancing parameters
    void configure(float threshold, uint64_t interval);
    
    // Check if rebalancing is needed
    bool needsRebalancing() const;
    
    // Get partition for neuron distribution
    const Partition* getPartition(NeuronId neuronId) const;
    
    // Add new node to system
    void addNode(int nodeId);
    
    // Remove node from system
    void removeNode(int nodeId);
    
    // Get all active nodes
    std::vector<int> getActiveNodes() const;
    
    // Get current partition information
    std::vector<Partition> getPartitions() const;
    
    // Estimate computation time for operation
    float estimateComputationTime(const WorkUnit& work) const;
    
    // Estimate communication time for operation
    float estimateCommunicationTime(int targetNode) const;
    
    // Generate work distribution plan
    std::vector<std::pair<NeuronId, int>> generateWorkPlan(
        const std::vector<NeuronId>& neurons) const;
    
    // Monitor and adjust load balancing parameters
    void monitorAndAdjust(const std::vector<NodeWorkload>& workloads);
    
    // Get workload prediction for future steps
    std::vector<float> predictWorkload(int stepsAhead) const;
    
    // Optimize partitioning based on current metrics
    void optimizePartitioning();
    
    // Save/load load balancer state
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);

private:
    struct Impl;
    Impl* pImpl;
};

// Work item queue for distributed processing
class WorkQueue {
public:
    WorkQueue();
    ~WorkQueue();
    
    // Disable copying, enable moving
    WorkQueue(const WorkQueue&) = delete;
    WorkQueue& operator=(const WorkQueue&) = delete;
    WorkQueue(WorkQueue&&) noexcept;
    WorkQueue& operator=(WorkQueue&&) noexcept;
    
    // Add work item to queue
    bool enqueue(const WorkUnit& work);
    
    // Remove work item from queue
    bool dequeue(WorkUnit& work);
    
    // Get number of work items
    size_t size() const;
    
    // Check if queue is empty
    bool empty() const;
    
    // Get all work items
    std::vector<WorkUnit> getAllWork() const;
    
    // Clear queue
    void clear();
    
    // Set maximum queue size
    void setMaxSize(size_t maxSize);
    
    // Get current queue size limit
    size_t getMaxSize() const;
    
    // Prioritize work by priority score
    void prioritize(float threshold);
    
    // Get work items by type
    std::vector<WorkUnit> getWorkByType(OperationType type) const;
    
    // Merge another work queue
    bool merge(WorkQueue& other);
    
    // Distribute work to other nodes
    bool distributeToNode(int targetNode, const Communicator& comm) const;
    
    // Receive work from other nodes
    bool receiveFromNode(int sourceNode, const Communicator& comm);

private:
    struct Impl;
    Impl* pImpl;
};

// Dynamic partitioning algorithm for distributed neural networks
class PartitionAlgorithm {
public:
    virtual ~PartitionAlgorithm() = default;
    
    // Generate partition based on neuron characteristics and system metrics
    virtual std::vector<Partition> partition(
        const std::vector<Neuron*>& neurons,
        const std::vector<NodeWorkload>& workloads) = 0;
    
    // Evaluate quality of a partition
    virtual float evaluate(const std::vector<Partition>& partitions) const = 0;
    
    // Optimize existing partition
    virtual bool optimize(std::vector<Partition>& partitions) = 0;
};

// Greedy partitioning algorithm (simple and fast)
class GreedyPartitionAlgorithm : public PartitionAlgorithm {
public:
    GreedyPartitionAlgorithm();
    ~GreedyPartitionAlgorithm() override;
    
    std::vector<Partition> partition(
        const std::vector<Neuron*>& neurons,
        const std::vector<NodeWorkload>& workloads) override;
    
    float evaluate(const std::vector<Partition>& partitions) const override;
    
    bool optimize(std::vector<Partition>& partitions) override;
    
    // Set parameters
    void setLoadBalancing(float loadBalance) { loadBalanceFactor = loadBalance; }
    void setMaxPartitionSize(size_t maxSize) { maxPartitionSize = maxSize; }
    
private:
    struct Impl;
    Impl* pImpl;
    float loadBalanceFactor;
    size_t maxPartitionSize;
};

// Genetic algorithm partitioning (more accurate but slower)
class GeneticPartitionAlgorithm : public PartitionAlgorithm {
public:
    GeneticPartitionAlgorithm();
    ~GeneticPartitionAlgorithm() override;
    
    std::vector<Partition> partition(
        const std::vector<Neuron*>& neurons,
        const std::vector<NodeWorkload>& workloads) override;
    
    float evaluate(const std::vector<Partition>& partitions) const override;
    
    bool optimize(std::vector<Partition>& partitions) override;
    
    // Set genetic algorithm parameters
    void setPopulationSize(size_t size) { populationSize = size; }
    void setGenerations(size_t gens) { generations = gens; }
    void setMutationRate(float rate) { mutationRate = rate; }
    void setCrossoverRate(float rate) { crossoverRate = rate; }
    
private:
    struct Impl;
    Impl* pImpl;
    size_t populationSize;
    size_t generations;
    float mutationRate;
    float crossoverRate;
};

} // namespace nlm
