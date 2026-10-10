// Neuromodulation persistence - neuromodulation state persistence
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <mutex>
#include "../neuromodulation/NeuromodulationEngine.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/DopamineSystem.hpp"
#include "../neuromodulation/CuriositySystem.hpp"
#include "../neuromodulation/NoveltySystem.hpp"

namespace nlm {

// Neuromodulation state types
enum class NeuromodulationStateType {
    NeuromodulatorState,
    DopamineState,
    CuriosityState,
    NoveltyState,
    NeuromodulationNetworkState
};

// Neuromodulation persistence result
struct NeuromodulationPersistenceResult {
    bool success;
    std::string message;
    uint64_t totalNeuromodulators;
    uint64_t totalNetworks;
    double elapsedTime;
    time_t timestamp;
    
    NeuromodulationPersistenceResult() : success(false), totalNeuromodulators(0), totalNetworks(0),
                                         elapsedTime(0.0), timestamp(0) {}
};

// Neuromodulation persistence options
struct NeuromodulationPersistenceOptions {
    std::vector<NeuromodulationStateType> stateTypesToPersist;
    bool compress;
    bool encrypt;
    std::string outputPath;
    bool validateAfterSave;
    std::chrono::seconds timeout;
    bool includeNetworkStates;
    bool includeSystemStates;
    
    NeuromodulationPersistenceOptions() : compress(true), encrypt(false),
                                          validateAfterSave(true),
                                          timeout(std::chrono::seconds(30)),
                                          includeNetworkStates(true),
                                          includeSystemStates(true) {
        stateTypesToPersist = {
            NeuromodulationStateType::NeuromodulatorState,
            NeuromodulationStateType::DopamineState,
            NeuromodulationStateType::CuriosityState,
            NeuromodulationStateType::NoveltyState,
            NeuromodulationStateType::NeuromodulationNetworkState
        };
    }
};

// Neuromodulation persistence statistics
struct NeuromodulationPersistenceStats {
    uint64_t neuromodulatorStatesSaved;
    uint64_t dopamineStatesSaved;
    uint64_t curiosityStatesSaved;
    uint64_t noveltyStatesSaved;
    uint64_t networkStatesSaved;
    uint64_t totalStatesSaved;
    uint64_t totalSizeSaved;
    double totalTimeSpent;
    uint32_t errorCount;
    
    NeuromodulationPersistenceStats() : neuromodulatorStatesSaved(0), dopamineStatesSaved(0),
                                        curiosityStatesSaved(0), noveltyStatesSaved(0),
                                        networkStatesSaved(0), totalStatesSaved(0),
                                        totalSizeSaved(0), totalTimeSpent(0.0), errorCount(0) {}
};

// Main neuromodulation persistence class
class NeuromodulationPersistence {
public:
    NeuromodulationPersistence();
    ~NeuromodulationPersistence();
    
    // Configuration
    bool configure(const std::shared_ptr<Config>& config);
    void shutdown();
    
    // Persistence operations
    bool saveNeuromodulatorState(const std::string& name, const NeuromodulationPersistenceOptions& options = NeuromodulationPersistenceOptions());
    bool saveDopamineState(const std::string& name, const NeuromodulationPersistenceOptions& options = NeuromodulationPersistenceOptions());
    bool saveCuriosityState(const std::string& name, const NeuromodulationPersistenceOptions& options = NeuromodulationPersistenceOptions());
    bool saveNoveltyState(const std::string& name, const NeuromodulationPersistenceOptions& options = NeuromodulationPersistenceOptions());
    bool saveNeuromodulationNetworkState(const std::string& name, const NeuromodulationPersistenceOptions& options = NeuromodulationPersistenceOptions());
    
    // Load operations
    bool loadNeuromodulatorState(const std::string& name);
    bool loadDopamineState(const std::string& name);
    bool loadCuriosityState(const std::string& name);
    bool loadNoveltyState(const std::string& name);
    bool loadNeuromodulationNetworkState(const std::string& name);
    
    // Bulk operations
    bool saveAllNeuromodulationStates(const std::string& name, const NeuromodulationPersistenceOptions& options = NeuromodulationPersistenceOptions());
    bool loadAllNeuromodulationStates(const std::string& name);
    bool deleteNeuromodulationStates(const std::string& name);
    
    // Neuromodulation system integration
    void setNeuromodulationEngine(std::shared_ptr<NeuromodulationEngine> engine) {
        neuromodulationEngine_ = engine;
    }
    
    std::shared_ptr<NeuromodulationEngine> getNeuromodulationEngine() const { return neuromodulationEngine_; }
    
    // Statistics and monitoring
    NeuromodulationPersistenceStats getStats() const { return stats_; }
    
    bool isHealthy() const { return errorCount_ == 0; }
    
    // Event handling
    using NeuromodulationEventCallback = std::function<void(NeuromodulationStateType, const std::string&, const NeuromodulationPersistenceResult&)>;
    void setNeuromodulationEventCallback(NeuromodulationEventCallback callback) { 
        neuromodulationEventCallback_ = callback; 
    }
    
    // Validation
    bool validateConfiguration() const;
    bool validateNeuromodulationIntegrity() const;
    std::vector<std::string> getValidationErrors() const { return validationErrors_; }
    
    // Thread safety
    void lock() const { mutex_.lock(); }
    void unlock() const { mutex_.unlock(); }
    bool tryLock() const { return mutex_.try_lock(); }

private:
    bool initialize();
    void cleanup();
    
    bool saveNeuromodulationStateType(NeuromodulationStateType type, const std::string& name, 
                                    const NeuromodulationPersistenceOptions& options);
    bool loadNeuromodulationStateType(NeuromodulationStateType type, const std::string& name);
    
    NeuromodulationPersistenceResult executeNeuromodulationOperation(NeuromodulationStateType type,
                                                                      const std::string& operation,
                                                                      const std::string& name,
                                                                      const NeuromodulationPersistenceOptions& options);
    
    void updateStats(const NeuromodulationPersistenceResult& result, NeuromodulationStateType type);
    void logEvent(NeuromodulationStateType type, const std::string& operation,
                  const std::string& name, const NeuromodulationPersistenceResult& result);
    
    std::string getNeuromodulationStatePath(NeuromodulationStateType type, const std::string& name) const;
    
    // Component references
    std::shared_ptr<NeuromodulationEngine> neuromodulationEngine_;
    
    // Configuration
    std::shared_ptr<Config> config_;
    
    // State
    mutable std::mutex mutex_;
    uint32_t errorCount_;
    
    // Statistics
    NeuromodulationPersistenceStats stats_;
    
    // Events
    NeuromodulationEventCallback neuromodulationEventCallback_;
    
    // Validation
    std::vector<std::string> validationErrors_;
};

} // namespace nlm
