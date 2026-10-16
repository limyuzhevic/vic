#pragma once

#include <brain/Brain.hpp>
#include <brain/CheckpointManager.hpp>
#include <brain/Communicator.hpp>
#include <core/Logger/Logger.hpp>
#include <vector>
#include <unordered_map>
#include <string>
#include <mutex>
#include <atomic>
#include <chrono>
#include <thread>

namespace nlm {

// Fault types for neural simulation
enum class FaultType {
    NodeFailure,           // Node process failure
    NetworkPartition,      // Network connectivity loss
    MemoryCorruption,      // Memory corruption
    ComputationError,      // Computation error or crash
    CommunicationFailure,   // MPI communication failure
    ResourceExhaustion,    // CPU/memory/resource exhaustion
    ConfigurationError,    // Configuration error
    SoftwareBug,           // Software bug or crash
    ExternalDependency,    // External dependency failure
    EnvironmentalFailure    // Power/network/environmental failure
};

// Fault severity levels
enum class FaultSeverity {
    Low,       // Minor issue, system continues to operate
    Medium,    // Moderate issue, some functionality affected
    High,      // Major issue, significant functionality affected
    Critical,  // Critical issue, system cannot continue
    Catastrophic // System completely failed
};

// Recovery actions
enum class RecoveryAction {
    None,              // No recovery action
    RestartProcess,    // Restart failed process
    failover,           // Fail over to backup node
    RestoreCheckpoint, // Restore from checkpoint
    ReduceWorkload,    // Reduce computational load
    UseFallbackModel,  // Use simpler fallback model
    NetworkRetry,      // Retry network communication
    ReallocateResources, // Reallocate system resources
    AlertHuman,        // Alert human operator
    AutomaticRecovery, // Automatic recovery procedure
    EmergencyShutdown  // Emergency shutdown
};

// Fault event information
struct FaultEvent {
    int nodeId;
    FaultType type;
    FaultSeverity severity;
    std::string description;
    uint64_t timestamp;
    std::string source;           // Where fault originated
    std::string impact;           // What is affected
    bool recoverable;              // Whether fault can be recovered
    RecoveryAction suggestedAction; // Suggested recovery action
    uint64_t duration;              // Duration of fault (if known)
    
    FaultEvent() : nodeId(0), type(FaultType::NodeFailure), 
                  severity(FaultSeverity::Medium), timestamp(0), 
                  recoverable(true), suggestedAction(RecoveryAction::None), 
                  duration(0) {}
};

// Recovery procedure information
struct RecoveryProcedure {
    std::string name;
    RecoveryAction action;
    FaultType applicableFaults;   // Bitmask of applicable fault types
    uint64_t estimatedTime;       // Estimated recovery time in milliseconds
    float successProbability;     // Probability of successful recovery
    std::string prerequisites;    // Prerequisites for recovery
    std::string description;      // Description of recovery procedure
    bool requiresExternalHelp;    // Whether human intervention is required
    
    RecoveryProcedure() : action(RecoveryAction::None), 
                         applicableFaults(FaultType::NodeFailure),
                         estimatedTime(0), successProbability(1.0f),
                         requiresExternalHelp(false) {}
};

// Health check result
struct HealthCheckResult {
    int nodeId;
    uint64_t timestamp;
    bool healthy;                   // Overall health status
    float cpuUsage;                 // CPU usage percentage (0-100)
    float memoryUsage;              // Memory usage percentage (0-100)
    uint64_t memoryAvailable;       // Available memory in bytes
    int activeNeuronCount;          // Number of active neurons
    int spikeCount;                 // Total spike count
    float communicationTime;        // Average communication time
    std::vector<std::string> issues; // Health issues or warnings
    
    HealthCheckResult() : nodeId(0), timestamp(0), healthy(true),
                         cpuUsage(0.0f), memoryUsage(0.0f), 
                         memoryAvailable(0), activeNeuronCount(0),
                         spikeCount(0), communicationTime(0.0f) {}
};

// Redundancy and replication information
struct RedundancyInfo {
    int nodeId;
    int replicaCount;               // Number of replicas
    float consistencyLevel;         // Consistency level (0.0 to 1.0)
    uint64_t lastSynchronized;       // Last synchronization time
    float synchronizationTime;      // Average synchronization time
    std::vector<int> replicas;       // Node IDs of replicas
    bool active;                    // Whether replication is active
    
    RedundancyInfo() : nodeId(0), replicaCount(0), consistencyLevel(1.0f),
                      lastSynchronized(0), synchronizationTime(0.0f), active(true) {}
};

// Distributed fault tolerance system
class FaultTolerance {
public:
    // Create fault tolerance system with configuration
    explicit FaultTolerance(const DistributedConfig& config);
    
    ~FaultTolerance();
    
    // Disable copying, enable moving
    FaultTolerance(const FaultTolerance&) = delete;
    FaultTolerance& operator=(const FaultTolerance&) = delete;
    FaultTolerance(FaultTolerance&&) noexcept;
    FaultTolerance& operator=(FaultTolerance&&) noexcept;
    
    // Initialize fault tolerance system
    bool initialize(Brain* brain);
    
    // Report fault to fault tolerance system
    bool reportFault(const FaultEvent& fault);
    
    // Recover from fault
    bool recover(const FaultEvent& fault);
    
    // Perform health check
    HealthCheckResult healthCheck(int nodeId = -1) const;
    
    // Get all reported faults
    std::vector<FaultEvent> getFaults() const;
    
    // Get current node health status
    bool isNodeHealthy(int nodeId) const;
    
    // Get fault tolerance statistics
    float getRecoverySuccessRate() const;
    float getAverageRecoveryTime() const;
    uint64_t getTotalFaults() const;
    uint64_t getTotalRecoveries() const;
    
    // Configure fault tolerance
    void configure(int nodeId, bool enableRedundancy, bool enableReplication,
                   float redundancyLevel, uint64_t healthCheckInterval);
    
    // Set redundancy information
    void setRedundancy(const RedundancyInfo& redundancy);
    
    // Get redundancy information
    RedundancyInfo getRedundancy(int nodeId) const;
    
    // Add recovery procedure
    void addRecoveryProcedure(const RecoveryProcedure& procedure);
    
    // Get recovery procedures
    std::vector<RecoveryProcedure> getRecoveryProcedures() const;
    
    // Test fault tolerance system
    bool testRecoveryProcedures() const;
    
    // Get system health status
    bool getSystemHealth(std::vector<std::string>& issues) const;
    
    // Set fault tolerance callback
    void setFaultCallback(std::function<void(const FaultEvent&)> callback);
    
    // Set recovery callback
    void setRecoveryCallback(std::function<void(const FaultEvent&, const RecoveryAction&)> callback);
    
    // Save fault tolerance state
    bool saveState(const std::string& filepath) const;
    
    // Load fault tolerance state
    bool loadState(const std::string& filepath);

private:
    struct Impl;
    Impl* pImpl;
    
    // Internal helper methods
    bool detectFault(const HealthCheckResult& health) const;
    RecoveryAction determineRecoveryAction(const FaultEvent& fault) const;
    bool executeRecoveryProcedure(const FaultEvent& fault, const RecoveryAction& action);
    bool synchronizeWithReplicas(int nodeId) const;
    bool updateReplicationStatus() const;
    std::string getFaultTypeString(FaultType type) const;
    std::string getSeverityString(FaultSeverity severity) const;
    std::string getRecoveryActionString(RecoveryAction action) const;
    std::chrono::high_resolution_clock::time_point getCurrentTime() const;
};

// Fault recovery manager for individual nodes
class NodeRecoveryManager {
public:
    NodeRecoveryManager();
    ~NodeRecoveryManager();
    
    // Disable copying, enable moving
    NodeRecoveryManager(const NodeRecoveryManager&) = delete;
    NodeRecoveryManager& operator=(const NodeRecoveryManager&) = delete;
    NodeRecoveryManager(NodeRecoveryManager&&) noexcept;
    NodeRecoveryManager& operator=(NodeRecoveryManager&&) noexcept;
    
    // Initialize node recovery manager
    bool initialize(int nodeId, const DistributedConfig& config);
    
    // Handle node failure
    bool handleNodeFailure(const FaultEvent& fault);
    
    // Perform checkpoint-based recovery
    bool checkpointRecovery(int stepNumber, const std::string& checkpointPath);
    
    // Perform memory recovery
    bool memoryRecovery();
    
    // Perform computation recovery
    bool computationRecovery();
    
    // Update node state
    void updateState(const Brain* brain);
    
    // Get node state
    std::string getNodeState() const;
    
    // Set checkpoint manager
    void setCheckpointManager(CheckpointManager* checkpointManager);
    
    // Set health check callback
    void setHealthCheckCallback(std::function<HealthCheckResult()> callback);

private:
    struct Impl;
    Impl* pImpl;
};

// Network fault detection and recovery
class NetworkFaultDetector {
public:
    NetworkFaultDetector();
    ~NetworkFaultDetector();
    
    // Disable copying, enable moving
    NetworkFaultDetector(const NetworkFaultDetector&) = delete;
    NetworkFaultDetector& operator=(const NetworkFaultDetector&) = delete;
    NetworkFaultDetector(NetworkFaultDetector&&) noexcept;
    NetworkFaultDetector& operator=(NetworkFaultDetector&&) noexcept;
    
    // Initialize network fault detector
    bool initialize(int nodeId, const DistributedConfig& config);
    
    // Perform network health check
    bool networkHealthCheck() const;
    
    // Detect network partitions
    std::vector<FaultEvent> detectNetworkPartitions() const;
    
    // Measure network latency
    float measureNetworkLatency(int targetNode) const;
    
    // Test network connectivity
    bool testNetworkConnectivity(const std::vector<int>& nodes) const;
    
    // Set communicator
    void setCommunicator(Communicator* communicator);
    
    // Set fault report callback
    void setFaultReportCallback(std::function<void(const FaultEvent&)> callback);

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
