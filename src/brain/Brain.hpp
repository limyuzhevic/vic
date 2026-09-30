#pragma once

// ============================================================================
// Enhanced Brain with Resource Cleanup, RAII Patterns, and Memory Monitoring
// ============================================================================

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <atomic>
#include <mutex>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <optional>
#include <stdexcept>
#include <cassert>

namespace nlm {

// ============================================================================
// Enhanced Error Handling for Brain Operations
// ============================================================================

// Brain operation exception hierarchy
class BrainException : public std::runtime_error {
public:
    explicit BrainException(const std::string& message) 
        : std::runtime_error("Brain error: " + message) {}
};

class BrainInitializationException : public BrainException {
public:
    explicit BrainInitializationException(const std::string& message)
        : BrainException("Brain initialization error: " + message) {}
};

class BrainOperationException : public BrainException {
public:
    explicit BrainOperationException(const std::string& message)
        : BrainException("Brain operation error: " + message) {}
};

class BrainMemoryException : public BrainException {
public:
    explicit BrainMemoryException(const std::string& message)
        : BrainException("Brain memory error: " + message) {}
};

// ============================================================================
// Brain Resource Management and RAII
// ============================================================================

// Smart resource wrapper for safe cleanup
class BrainResource {
protected:
    bool valid;
    std::string resourceId;
    
public:
    BrainResource(const std::string& id) : valid(true), resourceId(id) {}
    virtual ~BrainResource() {
        valid = false;
        cleanup();
    }
    
    virtual void cleanup() = 0;
    virtual bool isValid() const { return valid; }
    virtual std::string getResourceId() const { return resourceId; }
    
    // Disable copying to prevent resource leaks
    BrainResource(const BrainResource&) = delete;
    BrainResource& operator=(const BrainResource&) = delete;
};

// ============================================================================
// Memory Monitoring and Tracking
// ============================================================================

// Memory tracking for resource monitoring
class BrainMemoryTracker {
private:
    struct AllocationRecord {
        void* address;
        size_t size;
        std::string source;
        std::chrono::system_clock::time_point timestamp;
        std::thread::id threadId;
        bool deallocated;
        
        AllocationRecord(void* addr, size_t sz, const std::string& src)
            : address(addr), size(sz), source(src), 
              timestamp(std::chrono::system_clock::now()),
              threadId(std::this_thread::get_id()), deallocated(false) {}
    };
    
    std::vector<AllocationRecord> allocations;
    mutable std::shared_mutex trackerMutex;
    std::atomic<size_t> totalMemoryAllocated{0};
    std::atomic<size_t> totalMemoryFreed{0};
    std::chrono::system_clock::time_point lastSnapshotTime;
    
public:
    BrainMemoryTracker() : lastSnapshotTime(std::chrono::system_clock::now()) {}
    
    // Track allocation
    void trackAllocation(void* address, size_t size, const std::string& source = "unknown") {
        std::lock_guard<std::shared_mutex> lock(trackerMutex);
        allocations.emplace_back(address, size, source);
        totalMemoryAllocated.fetch_add(size);
    }
    
    // Track deallocation
    void trackDeallocation(void* address) {
        std::lock_guard<std::shared_mutex> lock(trackerMutex);
        totalMemoryFreed.fetch_add(1); // Note: this should track size, but we only have pointer
        
        for (auto& alloc : allocations) {
            if (alloc.address == address && !alloc.deallocated) {
                alloc.deallocated = true;
                break;
            }
        }
    }
    
    // Get current memory stats
    struct MemoryStats {
        size_t totalAllocated;
        size_t totalFreed;
        size_t currentTracked;
        size_t currentMemoryUsage;
        size_t peakMemoryUsage;
        std::chrono::system_clock::time_point lastUpdate;
    };
    
    MemoryStats getMemoryStats() {
        std::lock_guard<std::shared_mutex> lock(trackerMutex);
        
        size_t currentTracked = 0;
        size_t currentMemoryUsage = 0;
        size_t peakMemoryUsage = 0;
        
        for (const auto& alloc : allocations) {
            if (!alloc.deallocated) {
                currentTracked++;
                currentMemoryUsage += alloc.size;
                peakMemoryUsage = std::max(peakMemoryUsage, currentMemoryUsage);
            }
        }
        
        return MemoryStats{
            totalMemoryAllocated.load(),
            totalMemoryFreed.load(),
            currentTracked,
            currentMemoryUsage,
            peakMemoryUsage,
            lastSnapshotTime
        };
    }
    
    // Get allocations by thread
    std::unordered_map<std::string, std::vector<AllocationRecord>> getAllocationsByThread() {
        std::lock_guard<std::shared_mutex> lock(trackerMutex);
        std::unordered_map<std::string, std::vector<AllocationRecord>> byThread;
        
        for (const auto& alloc : allocations) {
            std::string threadIdStr = std::to_string(std::hash<std::thread::id>{}(alloc.threadId));
            byThread[threadIdStr].push_back(alloc);
        }
        
        return byThread;
    }
    
    // Generate memory report
    std::string generateMemoryReport() {
        std::lock_guard<std::shared_mutex> lock(trackerMutex);
        
        auto stats = getMemoryStats();
        std::ostringstream oss;
        
        oss << "Brain Memory Report\n";
        oss << "===================\n\n";
        oss << "Total allocated: " << stats.totalAllocated << " bytes\n";
        oss << "Total freed: " << stats.totalFreed << " allocations\n";
        oss << "Current tracked: " << stats.currentTracked << " allocations\n";
        oss << "Current memory usage: " << stats.currentMemoryUsage << " bytes\n";
        oss << "Peak memory usage: " << stats.peakMemoryUsage << " bytes\n";
        oss << "Last update: " << 
            std::chrono::duration<double>(std::chrono::system_clock::now() - stats.lastUpdate).count() << 
            " seconds ago\n";
        
        return oss.str();
    }
    
    // Cleanup tracked allocations
    void cleanupAll() {
        std::lock_guard<std::shared_mutex> lock(trackerMutex);
        allocations.clear();
        totalMemoryAllocated.store(0);
        totalMemoryFreed.store(0);
    }
};

// ============================================================================
// Enhanced Brain with Resource Management
// ============================================================================

class Brain {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Resource management
    std::vector<std::unique_ptr<BrainResource>> resources;
    mutable std::mutex resourceMutex;
    
    // Memory tracking
    BrainMemoryTracker memoryTracker;
    mutable std::mutex memoryTrackerMutex;
    
    // Error tracking
    std::vector<std::string> errorLog;
    mutable std::mutex errorLogMutex;
    
    // Performance monitoring
    std::chrono::system_clock::time_point lastPerformanceSnapshot;
    std::unordered_map<std::string, double> performanceMetrics;
    mutable std::mutex performanceMutex;
    
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Reset brain state
    void reset();
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========

    // Complete dopamine system
    Dopamine* getDopamine();
    
    // Serotonin system
    Serotonin* getSerotonin();
    
    // Norepinephrine system
    Norepinephrine* getNorepinephrine();
    
    // Acetylcholine system
    Acetylcholine* getAcetylcholine();
    
    // GABA system
    GABA* getGABA();
    
    // Glutamate system
    Glutamate* getGlutamate();
    
    // Endorphins system
    Endorphins* getEndorphins();
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Connect Cognition systems (neural planner, concept formation, attentional selection)
    void integrateCognitionSystems();
    
    // Process attention in integrated cognitive architecture
    void processAttentionWithWorkingMemory();
    
    // Update concept formation based on working memory patterns
    void updateConceptsFromWorkingMemory();
    
    // Integrate prediction error with attentional selection
    void applyPredictionErrorToAttention();
    
    // Enhance episodic memory storage with sensory context
    void enhanceEpisodicMemoryWithSensoryContext();
    
    // Apply neuromodulatory effects on neural excitability
    void applyNeuromodulationToNeuralDynamics();
    
    // Process working memory through attentional competition
    void processWorkingMemoryCompetition();
    
    // Integrate development effects on plasticity
    void applyDevelopmentToPlasticity();
    
    // Prepare brain for checkpoint/save
    void prepareForCheckpoint() const;
    
    // Restore brain from checkpoint
    bool restoreFromCheckpoint(const std::string& filepath);
    
    // Add configuration for advanced experiment parameters
    void configureForExperiment(const std::string& experimentName);
    
    // ========== RESOURCE MANAGEMENT ==========
    
    // Add resource for management
    void addResource(std::unique_ptr<BrainResource> resource);
    
    // Cleanup all resources
    void cleanupResources();
    
    // Get resource count
    size_t getResourceCount() const;
    
    // ========== MEMORY MONITORING ==========
    
    // Get memory statistics
    BrainMemoryTracker::MemoryStats getMemoryStats() const;
    
    // Get formatted memory report
    std::string getMemoryReport() const;
    
    // ========== ERROR HANDLING ==========
    
    // Log error
    void logError(const std::string& error);
    
    // Get error log
    std::vector<std::string> getErrorLog() const;
    
    // Clear error log
    void clearErrorLog();
    
    // Check if there were any errors
    bool hasErrors() const;
    
    // ========== PERFORMANCE MONITORING ==========
    
    // Record performance metric
    void recordPerformanceMetric(const std::string& name, double value);
    
    // Get performance metrics
    std::unordered_map<std::string, double> getPerformanceMetrics() const;
    
    // Get formatted performance report
    std::string getPerformanceReport() const;
    
    // ========== UTILITY FUNCTIONS ==========
    
    // Get system uptime
    std::chrono::duration<double> getSystemUptime() const;
    
    // Check if brain is healthy
    bool isHealthy() const;
    
    // Get formatted status report
    std::string getStatusReport() const;
    
    // === COMPATIBILITY FUNCTIONS ===
    
    // Legacy functions for backward compatibility
    SpikeSystem* getSpikeSystem() { return pImpl->spikeSystem.get(); }
    const SpikeSystem* getSpikeSystem() const { return pImpl->spikeSystem.get(); }
    STDP* getSTDP() { return pImpl->stdp.get(); }
    Hebbian* getHebbian() { return pImpl->hebbian.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return pImpl->structuralPlasticity.get(); }
    NeuralWorkingMemory* getWorkingMemory() { return pImpl->workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return pImpl->episodicMemory.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return pImpl->associativeMemory.get(); }
    PredictionSystem* getPredictionSystem() { return pImpl->predictionSystem.get(); }
    NeuralPlanner* getPlanner() { return pImpl->planner.get(); }
    ConceptFormation* getConceptFormation() { return pImpl->conceptFormation.get(); }
    AttentionalSelection* getAttention() { return pImpl->attention.get(); }
    DevelopmentSystem* getDevelopmentSystem() { return pImpl->developmentSystem.get(); }
    Dopamine* getDopamine() { return pImpl->dopamine.get(); }
    Serotonin* getSerotonin() { return pImpl->serotonin.get(); }
    Norepinephrine* getNorepinephrine() { return pImpl->norepinephrine.get(); }
    Acetylcholine* getAcetylcholine() { return pImpl->acetylcholine.get(); }
    GABA* getGABA() { return pImpl->gaba.get(); }
    Glutamate* getGlutamate() { return pImpl->glutamate.get(); }
    Endorphins* getEndorphins() { return pImpl->endorphins.get(); }
    Curiosity* getCuriosity() { return pImpl->curiosity.get(); }
    Novelty* getNovelty() { return pImpl->novelty.get(); }
    PredictionError* getPredictionErrorSignal() { return pImpl->predictionError.get(); }
    
    // Status logging
    void logStatus() const;
    
    // Legacy region access (for compatibility)
    NeuralRegion* getRegion(RegionId id) { return pImpl->regions[id.index() - 1].get(); }
    const NeuralRegion* getRegion(RegionId id) const { return pImpl->regions[id.index() - 1].get(); }
    
    // Helper methods
    void logErrorInternal(const std::string& context, const std::string& error);
    bool validateMemoryState() const;
    void cleanupInvalidResources();
    void updatePerformanceMetrics();
    std::string formatMemoryUsage(size_t bytes) const;
    std::string formatDuration(std::chrono::duration<double> dur) const;
};

// ============================================================================
// Enhanced Error Handling Utilities
// ============================================================================

namespace ErrorHandling {
    // Format brain operation error
    std::string formatBrainError(const std::string& operation, const std::string& error);
    
    // Format memory error
    std::string formatMemoryError(const std::string& allocation, size_t size, const std::string& error);
    
    // Format performance error
    std::string formatPerformanceError(const std::string& operation, double duration, const std::string& error);
    
    // Safe resource cleanup
    template<typename T, typename... Args>
    std::unique_ptr<T> safeCreateResource(Args&&... args) {
        try {
            return std::make_unique<T>(std::forward<Args>(args)...);
        } catch (const std::exception& e) {
            // Log error and return null pointer
            Logger::getGlobal().error("Failed to create resource: " + std::string(e.what()));
            return nullptr;
        }
    }
    
    // Safe delete with tracking
    template<typename T>
    void safeDelete(std::unique_ptr<T>& ptr, const std::string& resourceName) {
        if (ptr) {
            try {
                // Track deallocation
                // In a real implementation, this would track memory cleanup
                ptr.reset();
                Logger::getGlobal().debug("Resource cleaned up: " + resourceName);
            } catch (const std::exception& e) {
                Logger::getGlobal().error("Failed to cleanup resource " + resourceName + ": " + std::string(e.what()));
            }
        }
    }
}

// ============================================================================
// Enhanced Checkpoint System with Resource Management
// ============================================================================

class EnhancedCheckpointManager {
private:
    struct CheckpointEntry {
        std::string id;
        std::chrono::system_clock::time_point timestamp;
        size_t memoryUsage;
        std::string description;
        bool valid;
        
        CheckpointEntry(const std::string& checkpointId, const std::string& desc)
            : id(checkpointId), timestamp(std::chrono::system_clock::now()),
              memoryUsage(0), description(desc), valid(true) {}
    };
    
    std::vector<CheckpointEntry> checkpoints;
    mutable std::shared_mutex checkpointMutex;
    std::string currentCheckpointId;
    
public:
    EnhancedCheckpointManager() {}
    
    // Create checkpoint
    std::string createCheckpoint(const std::string& description = "") {
        std::lock_guard<std::shared_mutex> lock(checkpointMutex);
        
        currentCheckpointId = "checkpoint_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        checkpoints.emplace_back(currentCheckpointId, description);
        
        return currentCheckpointId;
    }
    
    // Get current checkpoint
    std::string getCurrentCheckpoint() const {
        std::lock_guard<std::shared_mutex> lock(checkpointMutex);
        return currentCheckpointId;
    }
    
    // Get checkpoint history
    std::vector<CheckpointEntry> getCheckpointHistory() const {
        std::lock_guard<std::shared_mutex> lock(checkpointMutex);
        return checkpoints;
    }
    
    // Cleanup old checkpoints
    void cleanupOldCheckpoints(size_t maxKeep = 10) {
        std::lock_guard<std::shared_mutex> lock(checkpointMutex);
        
        if (checkpoints.size() <= maxKeep) return;
        
        // Remove oldest checkpoints
        size_t toRemove = checkpoints.size() - maxKeep;
        checkpoints.erase(checkpoints.begin(), checkpoints.begin() + toRemove);
    }
    
    // Generate checkpoint report
    std::string generateReport() const {
        std::lock_guard<std::shared_mutex> lock(checkpointMutex);
        
        std::ostringstream oss;
        oss << "Checkpoint Manager Report\n";
        oss << "=========================\n\n";
        oss << "Total checkpoints: " << checkpoints.size() << "\n";
        oss << "Current checkpoint: " << currentCheckpointId << "\n";
        
        if (!checkpoints.empty()) {
            oss << "\nRecent checkpoints:\n";
            for (const auto& checkpoint : checkpoints) {
                oss << "  " << checkpoint.id << ": " << checkpoint.description << "\n";
                oss << "    Created: " << 
                    std::chrono::duration<double>(std::chrono::system_clock::now() - checkpoint.timestamp).count() << 
                    " seconds ago\n";
            }
        }
        
        return oss.str();
    }
};

// ============================================================================
// Brain Factory for Enhanced Resource Creation
// ============================================================================

class BrainFactory {
public:
    // Create brain with enhanced error handling
    static std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) {
        try {
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw BrainInitializationException("Failed to initialize brain");
            }
            Logger::getGlobal().info("Brain created successfully");
            return brain;
        } catch (const BrainException& e) {
            Logger::getGlobal().error(std::string("Brain factory error: ") + e.what());
            throw;
        } catch (const std::exception& e) {
            Logger::getGlobal().error("Unexpected error creating brain: " + std::string(e.what()));
            throw BrainInitializationException("Unexpected error creating brain");
        }
    }
    
    // Create brain with additional resource management
    static std::shared_ptr<Brain> createBrainWithResources(std::shared_ptr<Config> config,
                                                           size_t numResources = 0) {
        auto brain = createBrain(config);
        
        // In a real implementation, this would add additional resources
        // For now, just log the action
        Logger::getGlobal().info("Brain created with resource management: " + std::to_string(numResources) + " resources");
        
        return brain;
    }
};

// ============================================================================
// Brain Integration Utilities
// ============================================================================

// Integration helper for connecting brain components
class BrainIntegrationHelper {
public:
    // Integrate all brain systems
    static void integrateAllSystems(std::shared_ptr<Brain> brain) {
        if (!brain) {
            Logger::getGlobal().error("Cannot integrate null brain");
            return;
        }
        
        try {
            // Integrate cognition systems
            brain->integrateCognitionSystems();
            
            // Process initial attention
            brain->processAttentionWithWorkingMemory();
            
            // Update concepts from working memory
            brain->updateConceptsFromWorkingMemory();
            
            // Apply prediction error to attention
            brain->applyPredictionErrorToAttention();
            
            Logger::getGlobal().info("Brain systems integrated successfully");
        } catch (const std::exception& e) {
            Logger::getGlobal().error("Failed to integrate brain systems: " + std::string(e.what()));
        }
    }
    
    // Prepare brain for simulation
    static void prepareBrainForSimulation(std::shared_ptr<Brain> brain) {
        if (!brain) {
            Logger::getGlobal().error("Cannot prepare null brain for simulation");
            return;
        }
        
        try {
            brain->prepareForCheckpoint();
            Logger::getGlobal().info("Brain prepared for simulation");
        } catch (const std::exception& e) {
            Logger::getGlobal().error("Failed to prepare brain for simulation: " + std::string(e.what()));
        }
    }
    
    // Validate brain state
    static bool validateBrainState(std::shared_ptr<Brain> brain) {
        if (!brain) {
            Logger::getGlobal().error("Cannot validate null brain");
            return false;
        }
        
        bool valid = true;
        
        // Check memory state
        if (!brain->validateMemoryState()) {
            valid = false;
            Logger::getGlobal().error("Brain memory state validation failed");
        }
        
        // Check for errors
        if (brain->hasErrors()) {
            valid = false;
            auto errors = brain->getErrorLog();
            for (const auto& error : errors) {
                Logger::getGlobal().error("Brain validation error: " + error);
            }
        }
        
        return valid;
    }
};

// ============================================================================
// Enhanced Logging Macros for Brain Operations
// ============================================================================

#define NLM_BRAIN_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug("Brain: " + msg, __FILE__, __LINE__, __func__)
#define NLM_BRAIN_LOG_INFO(msg) nlm::Logger::getGlobal().info("Brain: " + msg, __FILE__, __LINE__, __func__)
#define NLM_BRAIN_LOG_WARNING(msg) nlm::Logger::getGlobal().warning("Brain: " + msg, __FILE__, __LINE__, __func__)
#define NLM_BRAIN_LOG_ERROR(msg) nlm::Logger::getGlobal().error("Brain: " + msg, __FILE__, __LINE__, __func__)
#define NLM_BRAIN_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical("Brain: " + msg, __FILE__, __LINE__, __func__)

// ============================================================================
// Brain Resource Management Macros
// ============================================================================

#define NLM_BRAIN_RESOURCE(resource_name, resource_ptr) \
    nlm::ErrorHandling::safeDelete(resource_ptr, resource_name)

#define NLM_BRAIN_CHECK(condition, error_message) \
    if (!(condition)) { \
        nlm::Logger::getGlobal().error("Brain check failed: " + std::string(error_message)); \
        return false; \
    }

#define NLM_BRAIN_THROW(exception_type, message) \
    throw exception_type("Brain: " + std::string(message))

} // namespace nlm

#endif // ENHANCED_BRAIN_HPP