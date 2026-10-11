// CheckpointPolicy.hpp
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <algorithm>

namespace nlm {

/**
 * Checkpoint policy system for automated checkpoint lifecycle management
 * 
 * Features:
 * - Multiple checkpoint strategies (time-based, size-based, event-based, hybrid)
 * - Configurable checkpoint policies
 * - Automatic checkpoint cleanup and aging
 * - Checkpoint lifecycle management
 * - Policy evaluation and optimization
 * - Historical policy tracking
 * - Policy-based checkpoint scheduling
 * - Resource-aware checkpointing
 * 
 * Policy Types:
 * - TimeBased: Create checkpoints at regular intervals
 * - SizeBased: Create checkpoints when size threshold reached
 * - StepBased: Create checkpoints at regular step intervals
 * - EventBased: Create checkpoints on specific events
 * - Hybrid: Combination of multiple strategies
 */

enum class CheckpointStrategy {
    None,
    TimeBased,
    SizeBased,
    StepBased,
    EventBased,
    Hybrid,
    Adaptive,
    Custom
};

enum class CleanupPolicy {
    None,
    AgeBased,
    SizeBased,
    AgeAndSize,
    Manual
};

enum class CheckpointPriority {
    Low,
    Normal,
    High,
    Critical
};

struct CheckpointRule {
    std::string name;                    // Rule name
    CheckpointStrategy strategy;         // Strategy type
    uint64_t interval{0};                // Time or step interval
    uint64_t sizeThreshold{0};           // Size threshold (bytes)
    std::vector<std::string> triggerEvents; // Events that trigger checkpoints
    CheckpointPriority priority{CheckpointPriority::Normal}; // Priority level
    bool enabled{true};                  // Is rule enabled
    double weight{1.0};                 // Weight for adaptive strategies
    std::map<std::string, std::string> parameters; // Additional parameters
    
    CheckpointRule() = default;
    CheckpointRule(const std::string& n, CheckpointStrategy s) : 
        name(n), strategy(s), enabled(true) {}
    
    bool shouldCreateCheckpoint(
        uint64_t currentStep,
        double currentTime,
        uint64_t currentSize,
        const std::string& event,
        const std::unordered_map<std::string, uint64_t>& state) const {
        
        if (!enabled) {
            return false;
        }
        
        switch (strategy) {
            case CheckpointStrategy::TimeBased:
                return static_cast<uint64_t>(currentTime) >= interval;
            
            case CheckpointStrategy::SizeBased:
                return currentSize >= sizeThreshold;
            
            case CheckpointStrategy::StepBased:
                return currentStep >= interval;
            
            case CheckpointStrategy::EventBased:
                return std::find(triggerEvents.begin(), triggerEvents.end(), event) != triggerEvents.end();
            
            case CheckpointStrategy::Hybrid:
                return (static_cast<uint64_t>(currentTime) >= interval) || 
                       (currentSize >= sizeThreshold) ||
                       (currentStep >= interval) ||
                       (std::find(triggerEvents.begin(), triggerEvents.end(), event) != triggerEvents.end());
            
            case CheckpointStrategy::Adaptive:
                // Adaptive logic based on state
                return evaluateAdaptiveRule(currentStep, currentTime, currentSize, event, state);
            
            default:
                return false;
        }
    }
    
protected:
    virtual bool evaluateAdaptiveRule(
        uint64_t currentStep,
        double currentTime,
        uint64_t currentSize,
        const std::string& event,
        const std::unordered_map<std::string, uint64_t>& state) const {
        // Default adaptive implementation
        // Can be overridden by derived classes
        return currentStep >= interval;
    }
};

struct CheckpointSchedule {
    uint64_t nextCheckpointStep{0};          // Next checkpoint step
    time_t nextCheckpointTime{0};           // Next checkpoint time
    uint64_t consecutiveCheckpoints{0};     // Number of consecutive checkpoints
    uint64_t totalCheckpoints{0};           // Total checkpoints created
    double totalSize{0.0};                  // Total size of checkpoints
    double averageCheckpointTime{0.0};     // Average checkpoint time
    time_t lastCheckpointTime{0};          // Last checkpoint time
    std::string lastCheckpointName;       // Name of last checkpoint
    
    CheckpointSchedule() : nextCheckpointStep(0), nextCheckpointTime(0),
                          consecutiveCheckpoints(0), totalCheckpoints(0),
                          totalSize(0.0), averageCheckpointTime(0.0),
                          lastCheckpointTime(0) {}
    
    void recordCheckpoint(uint64_t step, double time, uint64_t size, const std::string& name) {
        ++consecutiveCheckpoints;
        ++totalCheckpoints;
        totalSize += size;
        
        if (consecutiveCheckpoints > 0) {
            averageCheckpointTime = totalSize / consecutiveCheckpoints;
        }
        
        lastCheckpointTime = time;
        lastCheckpointName = name;
        
        // Calculate next checkpoint times
        nextCheckpointStep = step + 10000; // Default 10k step interval
        nextCheckpointTime = time + 60.0;  // Default 60 second interval
    }
    
    void resetConsecutiveCount() {
        consecutiveCheckpoints = 0;
    }
};

struct CleanupRule {
    CleanupPolicy policy{CleanupPolicy::None};      // Cleanup policy type
    uint64_t minCheckpointAge{0};                 // Minimum age before cleanup (seconds)
    uint64_t maxCheckpointAge{86400};             // Maximum age before mandatory cleanup (seconds)
    uint64_t minSize{0};                           // Minimum size before cleanup (bytes)
    uint64_t maxSize{100 * 1024 * 1024};          // Maximum size before mandatory cleanup (bytes)
    uint64_t retentionCount{10};                  // Number of checkpoints to retain
    bool cleanupInvalid{true};                    // Clean invalid checkpoints
    bool cleanupCorrupted{true};                  // Clean corrupted checkpoints
    bool cleanupOld{true};                        // Clean old checkpoints
    std::vector<std::string> excludeTags;         // Tags to exclude from cleanup
    std::vector<std::string> includeTags;         // Tags to include for cleanup
    
    CleanupRule() = default;
    CleanupRule(CleanupPolicy p) : policy(p) {}
    
    bool shouldCleanup(const std::vector<CheckpointMetadata>& checkpoints) const {
        switch (policy) {
            case CleanupPolicy::AgeBased:
                return evaluateAgeBasedCleanup(checkpoints);
            
            case CleanupPolicy::SizeBased:
                return evaluateSizeBasedCleanup(checkpoints);
            
            case CleanupPolicy::AgeAndSize:
                return evaluateAgeAndSizeCleanup(checkpoints);
            
            case CleanupPolicy::Manual:
                return false; // Manual cleanup requires explicit trigger
            
            default:
                return false;
        }
    }
    
protected:
    bool evaluateAgeBasedCleanup(const std::vector<CheckpointMetadata>& checkpoints) const {
        if (!cleanupOld) {
            return false;
        }
        
        time_t now = std::time(nullptr);
        size_t oldCount = 0;
        
        for (const auto& metadata : checkpoints) {
            if (metadata.getCheckpointAge() >= minCheckpointAge) {
                ++oldCount;
            }
        }
        
        return oldCount >= retentionCount;
    }
    
    bool evaluateSizeBasedCleanup(const std::vector<CheckpointMetadata>& checkpoints) const {
        if (!cleanupOld) {
            return false;
        }
        
        double totalSize = 0.0;
        for (const auto& metadata : checkpoints) {
            totalSize += metadata.fileSize;
        }
        
        return totalSize >= maxSize;
    }
    
    bool evaluateAgeAndSizeCleanup(const std::vector<CheckpointMetadata>& checkpoints) const {
        return evaluateAgeBasedCleanup(checkpoints) && evaluateSizeBasedCleanup(checkpoints);
    }
};

struct CheckpointMetadata {
    std::string name;                    // Human-readable name
    uint64_t step{0};                    // Simulation step
    double simulationTime{0.0};         // Simulation time
    uint64_t neuronCount{0};            // Number of neurons
    uint64_t synapseCount{0};           // Number of synapses
    uint64_t regionCount{0};            // Number of regions
    uint64_t fileSize{0};               // File size in bytes
    time_t timestamp{0};                // Creation timestamp
    std::string author;                 // Who created this checkpoint
    std::string description;            // Description/notes
    std::vector<std::string> tags;     // Metadata tags for categorization
    bool isValid{true};                 // Is checkpoint valid
    bool isCompressed{false};          // Was checkpoint compressed
    uint32_t compressionAlgorithm{0};  // Compression algorithm used
    double memoryUsage{0.0};           // Memory usage during checkpoint
    double checkpointTime{0.0};        // Time taken to create checkpoint
    
    CheckpointMetadata() : step(0), simulationTime(0.0), neuronCount(0),
                          synapseCount(0), regionCount(0), fileSize(0),
                          timestamp(0), isValid(true), isCompressed(false),
                          compressionAlgorithm(0), memoryUsage(0.0), 
                          checkpointTime(0.0) {}
    
    std::string getId() const {
        // Generate unique ID from timestamp and step
        std::stringstream ss;
        ss << "ckpt_" << timestamp << "_" << step;
        return ss.str();
    }
    
    double getCheckpointAge() const {
        // Get age in seconds since checkpoint was created
        time_t now = std::time(nullptr);
        return static_cast<double>(now - timestamp);
    }
};

class CheckpointPolicy {
public:
    CheckpointPolicy();
    ~CheckpointPolicy();
    
    // Disable copying
    CheckpointPolicy(const CheckpointPolicy&) = delete;
    CheckpointPolicy& operator=(const CheckpointPolicy&) = delete;
    
    /**
     * Add checkpoint rule
     * @param rule Checkpoint rule to add
     */
    void addRule(const CheckpointRule& rule);
    
    /**
     * Remove checkpoint rule
     * @param ruleName Name of rule to remove
     */
    void removeRule(const std::string& ruleName);
    
    /**
     * Update checkpoint rule
     * @param rule Updated checkpoint rule
     */
    void updateRule(const CheckpointRule& rule);
    
    /**
     * Get checkpoint rule
     * @param ruleName Name of rule
     * @return Checkpoint rule or nullptr if not found
     */
    const CheckpointRule* getRule(const std::string& ruleName) const;
    
    /**
     * Check if checkpoint should be created
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time
     * @param currentSize Current memory usage
     * @param event Optional event
     * @param state Current system state
     * @return true if checkpoint should be created
     */
    bool shouldCreateCheckpoint(
        uint64_t currentStep,
        double currentTime,
        uint64_t currentSize = 0,
        const std::string& event = "",
        const std::unordered_map<std::string, uint64_t>& state = {}
    ) const;
    
    /**
     * Set cleanup rule
     * @param rule Cleanup rule
     */
    void setCleanupRule(const CleanupRule& rule) { cleanupRule_ = rule; }
    
    /**
     * Get cleanup rule
     */
    const CleanupRule& getCleanupRule() const { return cleanupRule_; }
    
    /**
     * Set checkpoint schedule
     * @param schedule Checkpoint schedule
     */
    void setSchedule(const CheckpointSchedule& schedule) { schedule_ = schedule; }
    
    /**
     * Get checkpoint schedule
     */
    const CheckpointSchedule& getSchedule() const { return schedule_; }
    
    /**
     * Record checkpoint creation
     * @param step Simulation step
     * @param time Simulation time
     * @param size Checkpoint size
     * @param name Checkpoint name
     */
    void recordCheckpoint(uint64_t step, double time, uint64_t size, const std::string& name);
    
    /**
     * Should cleanup checkpoints
     * @param checkpoints List of checkpoints to consider
     * @return true if cleanup should be performed
     */
    bool shouldCleanup(const std::vector<CheckpointMetadata>& checkpoints) const;
    
    /**
     * Get checkpoint recommendations
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time
     * @param currentSize Current memory usage
     * @param event Optional event
     * @param state Current system state
     * @return Vector of recommended checkpoint names
     */
    std::vector<std::string> getRecommendations(
        uint64_t currentStep,
        double currentTime,
        uint64_t currentSize = 0,
        const std::string& event = "",
        const std::unordered_map<std::string, uint64_t>& state = {}
    ) const;
    
    /**
     * Get checkpoint count
     */
    size_t getRuleCount() const { return rules_.size(); }
    
    /**
     * Get total checkpoints created
     */
    uint64_t getTotalCheckpointsCreated() const { return schedule_.totalCheckpoints; }
    
    /**
     * Get average checkpoint size
     */
    double getAverageCheckpointSize() const { return schedule_.totalCheckpoints > 0 ? 
        schedule_.totalSize / schedule_.totalCheckpoints : 0.0; }
    
    /**
     * Get checkpoint frequency (checkpoints per second)
     */
    double getCheckpointFrequency() const {
        if (schedule_.totalCheckpoints <= 1 || schedule_.lastCheckpointTime == 0) {
            return 0.0;
        }
        
        return static_cast<double>(schedule_.totalCheckpoints - 1) / schedule_.lastCheckpointTime;
    }
    
    /**
     * Get policy statistics
     */
    struct PolicyStatistics {
        size_t totalRules{0};               // Total rules
        size_t enabledRules{0};             // Enabled rules
        size_t disabledRules{0};            // Disabled rules
        uint64_t checkpointsCreated{0};    // Checkpoints created
        double averageSize{0.0};           // Average checkpoint size
        double frequency{0.0};             // Checkpoint frequency
        time_t lastRuleUpdate{0};          // Last rule update time
        
        double getEnabledRuleRatio() const {
            return totalRules > 0 ? static_cast<double>(enabledRules) / totalRules : 0.0;
        }
    };
    
    PolicyStatistics getStatistics() const;
    
    /**
     * Export rules to file
     * @param filepath Output file path
     */
    bool exportRules(const std::string& filepath) const;
    
    /**
     * Import rules from file
     * @param filepath Input file path
     */
    bool importRules(const std::string& filepath);
    
    /**
     * Get version string
     */
    static std::string getVersion() { return "CheckpointPolicy v1.0"; }
    
private:
    // Policy components
    std::unordered_map<std::string, CheckpointRule> rules_;      // Rules by name
    CleanupRule cleanupRule_;                                   // Cleanup rule
    CheckpointSchedule schedule_;                              // Checkpoint schedule
    
    // Statistics
    mutable std::mutex statsMutex_;
    size_t rulesCreated_{0};                                     // Number of rules created
    time_t lastUpdateTime_{0};                                   // Last update time
    
    // Policy evaluation helpers
    uint64_t calculateAdaptiveWeight(
        const CheckpointRule& rule,
        uint64_t currentStep,
        double currentTime,
        uint64_t currentSize,
        const std::string& event,
        const std::unordered_map<std::string, uint64_t>& state) const;
    
    double evaluateRulePriority(const CheckpointRule& rule) const;
    
    // File I/O
    bool writeRulesToFile(const std::string& filepath) const;
    bool readRulesFromFile(std::string& filepath);
    
    // Statistics
    void updateStatistics() const;
};