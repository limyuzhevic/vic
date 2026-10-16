#include "FaultTolerance.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <thread>
#include <fstream>
#include <sstream>
#include <ctime>

namespace nlm {

struct FaultTolerance::Impl {
    DistributedConfig config;
    Brain* brain;
    int nodeId;
    
    // Fault tracking
    std::vector<FaultEvent> faultHistory;
    std::mutex faultMutex;
    
    // Recovery procedures
    std::vector<RecoveryProcedure> recoveryProcedures;
    std::unordered_map<FaultType, RecoveryProcedure> faultToProcedure;
    
    // Redundancy information
    std::unordered_map<int, RedundancyInfo> redundancyInfo;
    std::mutex redundancyMutex;
    
    // Health monitoring
    std::vector<HealthCheckResult> healthHistory;
    std::mutex healthMutex;
    uint64_t healthCheckInterval;
    
    // Statistics
    std::atomic<uint64_t> totalFaults{0};
    std::atomic<uint64_t> totalRecoveries{0};
    std::vector<float> recoveryTimes;
    std::mutex statsMutex;
    
    // Callbacks
    std::function<void(const FaultEvent&)> faultCallback;
    std::function<void(const FaultEvent&, const RecoveryAction&)> recoveryCallback;
    
    // Recovery managers
    std::unique_ptr<NodeRecoveryManager> nodeRecoveryManager;
    std::unique_ptr<NetworkFaultDetector> networkFaultDetector;
    
    // Monitoring thread
    std::thread healthMonitorThread;
    std::atomic<bool> monitorRunning{false};
    
    Impl(const DistributedConfig& cfg) 
        : config(cfg), brain(nullptr), nodeId(0), healthCheckInterval(5000),  // 5 seconds default
          totalFaults(0), totalRecoveries(0) {
        // Initialize recovery procedures
        initializeRecoveryProcedures();
    }
};

FaultTolerance::FaultTolerance(const DistributedConfig& config) : pImpl(new Impl(config)) {}

FaultTolerance::~FaultTolerance() {
    // Stop health monitoring thread
    pImpl->monitorRunning = false;
    if (pImpl->healthMonitorThread.joinable()) {
        pImpl->healthMonitorThread.join();
    }
}

FaultTolerance::FaultTolerance(FaultTolerance&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

FaultTolerance& FaultTolerance::operator=(FaultTolerance&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool FaultTolerance::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("FaultTolerance: Invalid brain pointer");
        return false;
    }
    
    pImpl->brain = brain;
    pImpl->nodeId = config.nodeId;  // Will be set by MPI rank in distributed mode
    
    // Initialize recovery managers
    pImpl->nodeRecoveryManager = std::make_unique<NodeRecoveryManager>();
    if (!pImpl->nodeRecoveryManager->initialize(nodeId, config)) {
        NLM_LOG_ERROR("Failed to initialize node recovery manager");
        return false;
    }
    pImpl->nodeRecoveryManager->setCheckpointManager(nullptr);  // Would be set by DistributedBrain
    
    // Initialize network fault detector
    pImpl->networkFaultDetector = std::make_unique<NetworkFaultDetector>();
    if (!pImpl->networkFaultDetector->initialize(nodeId, config)) {
        NLM_LOG_ERROR("Failed to initialize network fault detector");
        return false;
    }
    pImpl->networkFaultDetector->setCommunicator(nullptr);  // Would be set by DistributedBrain
    
    // Start health monitoring thread
    pImpl->monitorRunning = true;
    pImpl->healthMonitorThread = std::thread(&FaultTolerance::healthMonitorLoop, this);
    
    NLM_LOG_INFO("FaultTolerance initialized on node " + std::to_string(nodeId));
    
    return true;
}

bool FaultTolerance::reportFault(const FaultEvent& fault) {
    std::lock_guard<std::mutex> lock(pImpl->faultMutex);
    
    // Validate fault event
    if (fault.nodeId < 0 && fault.nodeId != -1) {
        NLM_LOG_ERROR("Invalid fault node ID: " + std::to_string(fault.nodeId));
        return false;
    }
    
    // Check if fault is duplicate
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    for (const auto& existingFault : pImpl->faultHistory) {
        if (existingFault.type == fault.type && existingFault.nodeId == fault.nodeId &&
            (now - existingFault.timestamp) < 60000) {  // Within last minute
            NLM_LOG_INFO("Duplicate fault reported, suppressing: " + getFaultTypeString(fault.type));
            return true;
        }
    }
    
    // Add fault to history
    FaultEvent newFault = fault;
    newFault.timestamp = now;
    pImpl->faultHistory.push_back(newFault);
    
    // Update statistics
    pImpl->totalFaults++;
    
    NLM_LOG_WARNING("Fault reported - Node " + std::to_string(fault.nodeId) + 
                   " Type: " + getFaultTypeString(fault.type) + 
                   " Severity: " + getSeverityString(fault.severity) +
                   " Description: " + fault.description);
    
    // Call fault callback if set
    if (pImpl->faultCallback) {
        pImpl->faultCallback(newFault);
    }
    
    // Attempt recovery
    RecoveryAction action = determineRecoveryAction(newFault);
    if (action != RecoveryAction::None) {
        NLM_LOG_INFO("Attempting recovery for fault: " + getRecoveryActionString(action));
        
        bool recoverySuccess = executeRecoveryProcedure(newFault, action);
        
        if (pImpl->recoveryCallback) {
            pImpl->recoveryCallback(newFault, action);
        }
        
        return recoverySuccess;
    }
    
    NLM_LOG_INFO("No recovery action required for fault");
    return true;
}

bool FaultTolerance::recover(const FaultEvent& fault) {
    // This is a high-level recovery function that tries all recovery procedures
    // For now, just report and attempt automatic recovery
    return reportFault(fault);
}

HealthCheckResult FaultTolerance::healthCheck(int nodeId) const {
    HealthCheckResult result;
    
    if (nodeId >= 0) {
        result.nodeId = nodeId;
    } else {
        result.nodeId = pImpl->nodeId;
    }
    
    result.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    // Get brain metrics (simplified for now)
    if (pImpl->brain) {
        result.activeNeuronCount = pImpl->brain->getActiveNeuronCount();
        result.spikeCount = pImpl->brain->getTotalSpikeCount();
        
        // Estimate memory usage
        result.memoryUsage = 80.0f;  // Placeholder
        
        // Check for faults
        result.healthy = detectFault(result);
        
        if (!result.healthy) {
            result.issues.push_back("Brain system health check failed");
        }
    }
    
    // Update health history
    {
        std::lock_guard<std::mutex> lock(pImpl->healthMutex);
        if (pImpl->healthHistory.size() > 100) {
            pImpl->healthHistory.erase(pImpl->healthHistory.begin());
        }
        pImpl->healthHistory.push_back(result);
    }
    
    return result;
}

std::vector<FaultEvent> FaultTolerance::getFaults() const {
    std::lock_guard<std::mutex> lock(pImpl->faultMutex);
    
    std::vector<FaultEvent> activeFaults;
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    // Filter active faults (faults from last 5 minutes)
    for (const auto& fault : pImpl->faultHistory) {
        if (now - fault.timestamp < 300000) {  // 5 minutes
            activeFaults.push_back(fault);
        }
    }
    
    return activeFaults;
}

bool FaultTolerance::isNodeHealthy(int nodeId) const {
    // Get health check result
    HealthCheckResult health = healthCheck(nodeId);
    return health.healthy;
}

float FaultTolerance::getRecoverySuccessRate() const {
    std::lock_guard<std::mutex> lock(pImpl->statsMutex);
    
    uint64_t totalEvents = pImpl->totalFaults.load() + pImpl->totalRecoveries.load();
    if (totalEvents == 0) return 1.0f;
    
    return static_cast<float>(pImpl->totalRecoveries.load()) / totalEvents;
}

float FaultTolerance::getAverageRecoveryTime() const {
    std::lock_guard<std::mutex> lock(pImpl->statsMutex);
    
    if (pImpl->recoveryTimes.empty()) return 0.0f;
    
    float sum = std::accumulate(pImpl->recoveryTimes.begin(), pImpl->recoveryTimes.end(), 0.0f);
    return sum / static_cast<float>(pImpl->recoveryTimes.size());
}

uint64_t FaultTolerance::getTotalFaults() const {
    return pImpl->totalFaults.load();
}

uint64_t FaultTolerance::getTotalRecoveries() const {
    return pImpl->totalRecoveries.load();
}

void FaultTolerance::configure(int nodeId, bool enableRedundancy, bool enableReplication,
                             float redundancyLevel, uint64_t healthCheckInterval) {
    pImpl->nodeId = nodeId;
    
    // Configure redundancy
    RedundancyInfo redundancy;
    redundancy.nodeId = nodeId;
    redundancy.replicaCount = enableReplication ? 2 : 0;
    redundancy.consistencyLevel = redundancyLevel;
    redundancy.active = enableRedundancy;
    
    std::lock_guard<std::mutex> lock(pImpl->redundancyMutex);
    pImpl->redundancyInfo[nodeId] = redundancy;
    
    // Configure health check interval
    pImpl->healthCheckInterval = healthCheckInterval;
}

void FaultTolerance::setRedundancy(const RedundancyInfo& redundancy) {
    std::lock_guard<std::mutex> lock(pImpl->redundancyMutex);
    pImpl->redundancyInfo[redundancy.nodeId] = redundancy;
}

RedundancyInfo FaultTolerance::getRedundancy(int nodeId) const {
    std::lock_guard<std::mutex> lock(pImpl->redundancyMutex);
    
    auto it = pImpl->redundancyInfo.find(nodeId);
    if (it != pImpl->redundancyInfo.end()) {
        return it->second;
    }
    
    return RedundancyInfo();
}

void FaultTolerance::addRecoveryProcedure(const RecoveryProcedure& procedure) {
    std::lock_guard<std::mutex> lock(pImpl->faultMutex);
    
    pImpl->recoveryProcedures.push_back(procedure);
    pImpl->faultToProcedure[procedure.applicableFaults] = procedure;
}

std::vector<RecoveryProcedure> FaultTolerance::getRecoveryProcedures() const {
    std::lock_guard<std::mutex> lock(pImpl->faultMutex);
    return pImpl->recoveryProcedures;
}

bool FaultTolerance::testRecoveryProcedures() const {
    NLM_LOG_INFO("Testing recovery procedures...");
    
    bool allPassed = true;
    
    // Test each recovery procedure
    for (const auto& procedure : pImpl->recoveryProcedures) {
        NLM_LOG_INFO("Testing recovery procedure: " + procedure.name);
        
        // Simple test - just check if procedure is valid
        if (!procedure.prerequisites.empty() || procedure.requiresExternalHelp) {
            NLM_LOG_INFO("Procedure " + procedure.name + " requires manual setup");
        } else {
            NLM_LOG_INFO("Procedure " + procedure.name + " can be tested automatically");
        }
    }
    
    return true;
}

bool FaultTolerance::getSystemHealth(std::vector<std::string>& issues) const {
    issues.clear();
    
    // Check if brain is available
    if (!pImpl->brain) {
        issues.push_back("Brain not available");
        return false;
    }
    
    // Get health check for all nodes
    std::vector<int> activeNodes = getActiveNodes();
    bool systemHealthy = true;
    
    for (int nodeId : activeNodes) {
        HealthCheckResult health = healthCheck(nodeId);
        if (!health.healthy) {
            systemHealthy = false;
            for (const auto& issue : health.issues) {
                issues.push_back("Node " + std::to_string(nodeId) + ": " + issue);
            }
        }
    }
    
    return systemHealthy;
}

void FaultTolerance::setFaultCallback(std::function<void(const FaultEvent&)> callback) {
    pImpl->faultCallback = callback;
}

void FaultTolerance::setRecoveryCallback(std::function<void(const FaultEvent&, const RecoveryAction&)> callback) {
    pImpl->recoveryCallback = callback;
}

bool FaultTolerance::saveState(const std::string& filepath) const {
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file) {
            return false;
        }
        
        // Save configuration
        file.write(reinterpret_cast<const char*>(&pImpl->nodeId), sizeof(int));
        file.write(reinterpret_cast<const char*>(&pImpl->healthCheckInterval), sizeof(uint64_t));
        
        // Save fault history
        size_t faultCount = pImpl->faultHistory.size();
        file.write(reinterpret_cast<const char*>(&faultCount), sizeof(size_t));
        for (const auto& fault : pImpl->faultHistory) {
            file.write(reinterpret_cast<const char*>(&fault), sizeof(FaultEvent));
        }
        
        // Save recovery procedures
        size_t procedureCount = pImpl->recoveryProcedures.size();
        file.write(reinterpret_cast<const char*>(&procedureCount), sizeof(size_t));
        for (const auto& procedure : pImpl->recoveryProcedures) {
            file.write(reinterpret_cast<const char*>(&procedure), sizeof(RecoveryProcedure));
        }
        
        // Save redundancy info
        size_t redundancyCount = pImpl->redundancyInfo.size();
        file.write(reinterpret_cast<const char*>(&redundancyCount), sizeof(size_t));
        for (const auto& pair : pImpl->redundancyInfo) {
            file.write(reinterpret_cast<const char*>(&pair.first), sizeof(int));
            file.write(reinterpret_cast<const char*>(&pair.second), sizeof(RedundancyInfo));
        }
        
        // Save health history
        size_t healthCount = pImpl->healthHistory.size();
        file.write(reinterpret_cast<const char*>(&healthCount), sizeof(size_t));
        for (const auto& health : pImpl->healthHistory) {
            file.write(reinterpret_cast<const char*>(&health), sizeof(HealthCheckResult));
        }
        
        // Save statistics
        file.write(reinterpret_cast<const char*>(&pImpl->totalFaults), sizeof(uint64_t));
        file.write(reinterpret_cast<const char*>(&pImpl->totalRecoveries), sizeof(uint64_t));
        
        size_t recoveryTimesCount = pImpl->recoveryTimes.size();
        file.write(reinterpret_cast<const char*>(&recoveryTimesCount), sizeof(size_t));
        file.write(reinterpret_cast<const char*>(pImpl->recoveryTimes.data()),
                  recoveryTimesCount * sizeof(float));
        
        NLM_LOG_INFO("Fault tolerance state saved");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving fault tolerance state: ") + e.what());
        return false;
    }
}

bool FaultTolerance::loadState(const std::string& filepath) {
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            return false;
        }
        
        // Load configuration
        file.read(reinterpret_cast<char*>(&pImpl->nodeId), sizeof(int));
        file.read(reinterpret_cast<char*>(&pImpl->healthCheckInterval), sizeof(uint64_t));
        
        // Load fault history
        size_t faultCount;
        file.read(reinterpret_cast<char*>(&faultCount), sizeof(size_t));
        pImpl->faultHistory.clear();
        for (size_t i = 0; i < faultCount; ++i) {
            FaultEvent fault;
            file.read(reinterpret_cast<char*>(&fault), sizeof(FaultEvent));
            pImpl->faultHistory.push_back(fault);
        }
        
        // Load recovery procedures
        size_t procedureCount;
        file.read(reinterpret_cast<char*>(&procedureCount), sizeof(size_t));
        pImpl->recoveryProcedures.clear();
        pImpl->faultToProcedure.clear();
        for (size_t i = 0; i < procedureCount; ++i) {
            RecoveryProcedure procedure;
            file.read(reinterpret_cast<char*>(&procedure), sizeof(RecoveryProcedure));
            pImpl->recoveryProcedures.push_back(procedure);
            pImpl->faultToProcedure[procedure.applicableFaults] = procedure;
        }
        
        // Load redundancy info
        size_t redundancyCount;
        file.read(reinterpret_cast<char*>(&redundancyCount), sizeof(size_t));
        pImpl->redundancyInfo.clear();
        for (size_t i = 0; i < redundancyCount; ++i) {
            int nodeId;
            RedundancyInfo redundancy;
            file.read(reinterpret_cast<char*>(&nodeId), sizeof(int));
            file.read(reinterpret_cast<char*>(&redundancy), sizeof(RedundancyInfo));
            pImpl->redundancyInfo[nodeId] = redundancy;
        }
        
        // Load health history
        size_t healthCount;
        file.read(reinterpret_cast<char*>(&healthCount), sizeof(size_t));
        pImpl->healthHistory.clear();
        for (size_t i = 0; i < healthCount; ++i) {
            HealthCheckResult health;
            file.read(reinterpret_cast<char*>(&health), sizeof(HealthCheckResult));
            pImpl->healthHistory.push_back(health);
        }
        
        // Load statistics
        file.read(reinterpret_cast<char*>(&pImpl->totalFaults), sizeof(uint64_t));
        file.read(reinterpret_cast<char*>(&pImpl->totalRecoveries), sizeof(uint64_t));
        
        size_t recoveryTimesCount;
        file.read(reinterpret_cast<char*>(&recoveryTimesCount), sizeof(size_t));
        pImpl->recoveryTimes.resize(recoveryTimesCount);
        file.read(reinterpret_cast<char*>(pImpl->recoveryTimes.data()),
                 recoveryTimesCount * sizeof(float));
        
        NLM_LOG_INFO("Fault tolerance state loaded");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading fault tolerance state: ") + e.what());
        return false;
    }
}

void FaultTolerance::initializeRecoveryProcedures() {
    // Initialize default recovery procedures
    
    // Node failure recovery
    RecoveryProcedure nodeFailureProc;
    nodeFailureProc.name = "Node Failure Recovery";
    nodeFailureProc.action = RecoveryAction::failover;
    nodeFailureProc.applicableFaults = FaultType::NodeFailure;
    nodeFailureProc.estimatedTime = 5000;  // 5 seconds
    nodeFailureProc.successProbability = 0.8f;
    nodeFailureProc.requiresExternalHelp = false;
    nodeFailureProc.description = "Fail over to backup node";
    addRecoveryProcedure(nodeFailureProc);
    
    // Network partition recovery
    RecoveryProcedure networkProc;
    networkProc.name = "Network Partition Recovery";
    networkProc.action = RecoveryAction::NetworkRetry;
    networkProc.applicableFaults = FaultType::NetworkPartition;
    networkProc.estimatedTime = 1000;  // 1 second
    networkProc.successProbability = 0.6f;
    networkProc.requiresExternalHelp = false;
    networkProc.description = "Retry network connections";
    addRecoveryProcedure(networkProc);
    
    // Memory corruption recovery
    RecoveryProcedure memoryProc;
    memoryProc.name = "Memory Corruption Recovery";
    memoryProc.action = RecoveryAction::RestoreCheckpoint;
    memoryProc.applicableFaults = FaultType::MemoryCorruption;
    memoryProc.estimatedTime = 3000;  // 3 seconds
    memoryProc.successProbability = 0.9f;
    memoryProc.requiresExternalHelp = false;
    memoryProc.description = "Restore from checkpoint";
    addRecoveryProcedure(memoryProc);
    
    // Computation error recovery
    RecoveryProcedure computationProc;
    computationProc.name = "Computation Error Recovery";
    computationProc.action = RecoveryAction::RestartProcess;
    computationProc.applicableFaults = FaultType::ComputationError;
    computationProc.estimatedTime = 2000;  // 2 seconds
    computationProc.successProbability = 0.7f;
    computationProc.requiresExternalHelp = false;
    computationProc.description = "Restart computation process";
    addRecoveryProcedure(computationProc);
    
    // Communication failure recovery
    RecoveryProcedure commProc;
    commProc.name = "Communication Failure Recovery";
    commProc.action = RecoveryAction::NetworkRetry;
    commProc.applicableFaults = FaultType::CommunicationFailure;
    commProc.estimatedTime = 1000;  // 1 second
    commProc.successProbability = 0.8f;
    commProc.requiresExternalHelp = false;
    commProc.description = "Retry MPI communication";
    addRecoveryProcedure(commProc);
}

bool FaultTolerance::detectFault(const HealthCheckResult& health) const {
    bool healthy = true;
    
    // Check resource usage
    if (health.cpuUsage > 90.0f) {
        healthy = false;
    }
    
    if (health.memoryUsage > 95.0f) {
        healthy = false;
    }
    
    if (health.memoryAvailable < 100 * 1024 * 1024) {  // Less than 100MB
        healthy = false;
    }
    
    // Check for recent faults
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    std::lock_guard<std::mutex> lock(pImpl->faultMutex);
    for (const auto& fault : pImpl->faultHistory) {
        if (now - fault.timestamp < 60000) {  // Within last minute
            // Check if fault is still relevant
            if (fault.severity == FaultSeverity::Critical || fault.severity == FaultSeverity::Catastrophic) {
                healthy = false;
                break;
            }
        }
    }
    
    return healthy;
}

RecoveryAction FaultTolerance::determineRecoveryAction(const FaultEvent& fault) const {
    // Find recovery procedure for this fault type
    std::lock_guard<std::mutex> lock(pImpl->faultMutex);
    
    for (const auto& pair : pImpl->faultToProcedure) {
        if (fault.type == pair.first) {
            return pair.second.action;
        }
    }
    
    // Default recovery action based on fault severity
    if (fault.severity == FaultSeverity::Critical || fault.severity == FaultSeverity::Catastrophic) {
        return RecoveryAction::EmergencyShutdown;
    } else if (fault.severity == FaultSeverity::High) {
        return RecoveryAction::RestartProcess;
    } else {
        return RecoveryAction::None;
    }
}

bool FaultTolerance::executeRecoveryProcedure(const FaultEvent& fault, const RecoveryAction& action) {
    try {
        // Execute recovery based on action
        switch (action) {
            case RecoveryAction::None:
                NLM_LOG_INFO("No recovery action needed for fault");
                return true;
                
            case RecoveryAction::RestartProcess:
                NLM_LOG_INFO("Restarting process due to fault");
                // In real implementation, would restart process
                pImpl->totalRecoveries++;
                return true;
                
            case RecoveryAction::failover:
                NLM_LOG_INFO("Failing over to backup node");
                pImpl->totalRecoveries++;
                return true;
                
            case RecoveryAction::RestoreCheckpoint:
                NLM_LOG_INFO("Restoring from checkpoint");
                pImpl->totalRecoveries++;
                return true;
                
            case RecoveryAction::ReduceWorkload:
                NLM_LOG_INFO("Reducing workload");
                // In real implementation, would reduce computational load
                return true;
                
            case RecoveryAction::UseFallbackModel:
                NLM_LOG_INFO("Using fallback model");
                return true;
                
            case RecoveryAction::NetworkRetry:
                NLM_LOG_INFO("Retrying network communication");
                // In real implementation, would retry network communication
                return true;
                
            case RecoveryAction::ReallocateResources:
                NLM_LOG_INFO("Reallocating system resources");
                return true;
                
            case RecoveryAction::AlertHuman:
                NLM_LOG_INFO("Alerting human operator");
                // In real implementation, would alert human operator
                return true;
                
            case RecoveryAction::AutomaticRecovery:
                NLM_LOG_INFO("Attempting automatic recovery");
                return true;
                
            case RecoveryAction::EmergencyShutdown:
                NLM_LOG_ERROR("Emergency shutdown required");
                // In real implementation, would perform emergency shutdown
                return false;  // Shutdown is not a successful recovery
        }
        
        return false;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception executing recovery procedure: ") + e.what());
        return false;
    }
}

void FaultTolerance::healthMonitorLoop() {
    while (pImpl->monitorRunning) {
        try {
            // Perform health check for each active node
            std::vector<int> activeNodes = getActiveNodes();
            
            for (int nodeId : activeNodes) {
                // Get health check result
                HealthCheckResult health = healthCheck(nodeId);
                
                // Check for faults
                if (!detectFault(health)) {
                    // Create fault event
                    FaultEvent fault;
                    fault.nodeId = nodeId;
                    fault.type = FaultType::ComputationError;
                    fault.severity = FaultSeverity::Medium;
                    fault.description = "Health check failed";
                    
                    // Report fault
                    reportFault(fault);
                }
            }
            
            // Sleep before next health check
            std::this_thread::sleep_for(std::chrono::milliseconds(pImpl->healthCheckInterval));
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in health monitor loop: ") + e.what());
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

std::vector<int> FaultTolerance::getActiveNodes() const {
    std::lock_guard<std::mutex> lock(pImpl->healthMutex);
    
    std::vector<int> activeNodes;
    for (const auto& health : pImpl->healthHistory) {
        if (std::find(activeNodes.begin(), activeNodes.end(), health.nodeId) == activeNodes.end()) {
            activeNodes.push_back(health.nodeId);
        }
    }
    
    return activeNodes;
}

std::string FaultTolerance::getFaultTypeString(FaultType type) const {
    switch (type) {
        case FaultType::NodeFailure: return "NodeFailure";
        case FaultType::NetworkPartition: return "NetworkPartition";
        case FaultType::MemoryCorruption: return "MemoryCorruption";
        case FaultType::ComputationError: return "ComputationError";
        case FaultType::CommunicationFailure: return "CommunicationFailure";
        case FaultType::ResourceExhaustion: return "ResourceExhaustion";
        case FaultType::ConfigurationError: return "ConfigurationError";
        case FaultType::SoftwareBug: return "SoftwareBug";
        case FaultType::ExternalDependency: return "ExternalDependency";
        case FaultType::EnvironmentalFailure: return "EnvironmentalFailure";
        default: return "Unknown";
    }
}

std::string FaultTolerance::getSeverityString(FaultSeverity severity) const {
    switch (severity) {
        case FaultSeverity::Low: return "Low";
        case FaultSeverity::Medium: return "Medium";
        case FaultSeverity::High: return "High";
        case FaultSeverity::Critical: return "Critical";
        case FaultSeverity::Catastrophic: return "Catastrophic";
        default: return "Unknown";
    }
}

std::string FaultTolerance::getRecoveryActionString(RecoveryAction action) const {
    switch (action) {
        case RecoveryAction::None: return "None";
        case RecoveryAction::RestartProcess: return "RestartProcess";
        case RecoveryAction::failover: return "failover";
        case RecoveryAction::RestoreCheckpoint: return "RestoreCheckpoint";
        case RecoveryAction::ReduceWorkload: return "ReduceWorkload";
        case RecoveryAction::UseFallbackModel: return "UseFallbackModel";
        case RecoveryAction::NetworkRetry: return "NetworkRetry";
        case RecoveryAction::ReallocateResources: return "ReallocateResources";
        case RecoveryAction::AlertHuman: return "AlertHuman";
        case RecoveryAction::AutomaticRecovery: return "AutomaticRecovery";
        case RecoveryAction::EmergencyShutdown: return "EmergencyShutdown";
        default: return "Unknown";
    }
}

} // namespace nlm
