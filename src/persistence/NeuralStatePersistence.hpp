// Neural state persistence - neural network state persistence
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <map>

namespace nlm {

// Neural state types
enum class NeuralStateType {
    NeuronState,
    SynapseState,
    ConnectivityState,
    PlasticityState,
    SpikeHistoryState
};

// Neural state persistence result
struct NeuralStatePersistenceResult {
    bool success;
    std::string message;
    uint64_t totalNeurons;
    uint64_t totalSynapses;
    uint64_t totalConnections;
    double elapsedTime;
    time_t timestamp;
    
    NeuralStatePersistenceResult() : success(false), totalNeurons(0), totalSynapses(0),
                                     totalConnections(0), elapsedTime(0.0), timestamp(0) {}
};

// Neural state persistence options
struct NeuralStatePersistenceOptions {
    std::vector<NeuralStateType> stateTypesToPersist;
    bool compress;
    bool encrypt;
    std::string outputPath;
    bool validateAfterSave;
    std::chrono::seconds timeout;
    bool includeConnectivity;
    bool includePlasticity;
    bool includeSpikeHistory;
    
    NeuralStatePersistenceOptions() : compress(true), encrypt(false),
                                      validateAfterSave(true),
                                      timeout(std::chrono::seconds(30)),
                                      includeConnectivity(true),
                                      includePlasticity(true),
                                      includeSpikeHistory(true) {
        stateTypesToPersist = {
            NeuralStateType::NeuronState,
            NeuralStateType::SynapseState,
            NeuralStateType::ConnectivityState,
            NeuralStateType::PlasticityState,
            NeuralStateType::SpikeHistoryState
        };
    }
};

// Neural state statistics
struct NeuralStatePersistenceStats {
    uint64_t neuronStatesSaved;
    uint64_t synapseStatesSaved;
    uint64_t connectivityStatesSaved;
    uint64_t plasticityStatesSaved;
    uint64_t spikeHistoryStatesSaved;
    uint64_t totalStatesSaved;
    uint64_t totalSizeSaved;
    double totalTimeSpent;
    uint32_t errorCount;
    
    NeuralStatePersistenceStats() : neuronStatesSaved(0), synapseStatesSaved(0),
                                    connectivityStatesSaved(0), plasticityStatesSaved(0),
                                    spikeHistoryStatesSaved(0), totalStatesSaved(0),
                                    totalSizeSaved(0), totalTimeSpent(0.0), errorCount(0) {}
};

// Main neural state persistence class
class NeuralStatePersistence {
public:
    NeuralStatePersistence();
    ~NeuralStatePersistence();
    
    // Configuration
    bool configure(const std::shared_ptr<Config>& config);
    void shutdown();
    
    // Persistence operations
    bool saveNeuronState(const std::string& name, const NeuralStatePersistenceOptions& options = NeuralStatePersistenceOptions());
    bool saveSynapseState(const std::string& name, const NeuralStatePersistenceOptions& options = NeuralStatePersistenceOptions());
    bool saveConnectivityState(const std::string& name, const NeuralStatePersistenceOptions& options = NeuralStatePersistenceOptions());
    bool savePlasticityState(const std::string& name, const NeuralStatePersistenceOptions& options = NeuralStatePersistenceOptions());
    bool saveSpikeHistoryState(const std::string& name, const NeuralStatePersistenceOptions& options = NeuralStatePersistenceOptions());
    
    // Load operations
    bool loadNeuronState(const std::string& name);
    bool loadSynapseState(const std::string& name);
    bool loadConnectivityState(const std::string& name);
    bool loadPlasticityState(const std::string& name);
    bool loadSpikeHistoryState(const std::string& name);
    
    // Bulk operations
    bool saveAllNeuralStates(const std::string& name, const NeuralStatePersistenceOptions& options = NeuralStatePersistenceOptions());
    bool loadAllNeuralStates(const std::string& name);
    bool deleteNeuralStates(const std::string& name);
    
    // Neural system integration
    void setBrain(std::shared_ptr<Brain> brain) {
        brain_ = brain;
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
    // Statistics and monitoring
    NeuralStatePersistenceStats getStats() const { return stats_; }
    
    bool isHealthy() const { return errorCount_ == 0; }
    
    // Event handling
    using NeuralStateEventCallback = std::function<void(NeuralStateType, const std::string&, const NeuralStatePersistenceResult&)>;
    void setNeuralStateEventCallback(NeuralStateEventCallback callback) { 
        neuralStateEventCallback_ = callback; 
    }
    
    // Validation
    bool validateConfiguration() const;
    bool validateNeuralStateIntegrity() const;
    std::vector<std::string> getValidationErrors() const { return validationErrors_; }
    
    // Thread safety
    void lock() const { mutex_.lock(); }
    void unlock() const { mutex_.unlock(); }
    bool tryLock() const { return mutex_.try_lock(); }

private:
    bool initialize();
    void cleanup();
    
    bool saveNeuralStateType(NeuralStateType type, const std::string& name, 
                           const NeuralStatePersistenceOptions& options);
    bool loadNeuralStateType(NeuralStateType type, const std::string& name);
    
    NeuralStatePersistenceResult executeNeuralStateOperation(NeuralStateType type,
                                                             const std::string& operation,
                                                             const std::string& name,
                                                             const NeuralStatePersistenceOptions& options);
    
    void updateStats(const NeuralStatePersistenceResult& result, NeuralStateType type);
    void logEvent(NeuralStateType type, const std::string& operation,
                  const std::string& name, const NeuralStatePersistenceResult& result);
    
    std::string getNeuralStatePath(NeuralStateType type, const std::string& name) const;
    
    // Component references
    std::shared_ptr<Brain> brain_;
    
    // Configuration
    std::shared_ptr<Config> config_;
    
    // State
    mutable std::mutex mutex_;
    uint32_t errorCount_;
    
    // Statistics
    NeuralStatePersistenceStats stats_;
    
    // Events
    NeuralStateEventCallback neuralStateEventCallback_;
    
    // Validation
    std::vector<std::string> validationErrors_;
};

} // namespace nlm
