#pragma once

#include "Brain.hpp"
#include "BrainSimulator.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../neuromodulation/NeuromodulationEngine.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <functional>

namespace nlm {

class ParameterController {
public:
    ParameterController(std::shared_ptr<Brain> brain, std::shared_ptr<BrainSimulator> simulator);
    ~ParameterController();
    
    // Disable copying, enable moving
    ParameterController(const ParameterController&) = delete;
    ParameterController& operator=(const ParameterController&) = delete;
    ParameterController(ParameterController&&) noexcept = default;
    ParameterController& operator=(ParameterController&&) noexcept = default;
    
    // Initialization and shutdown
    bool initialize();
    void shutdown();
    
    // Dynamic parameter modification
    struct ParameterChange {
        std::string parameterName;
        std::string parameterType; // "neuron", "synapse", "plasticity", "neuromodulator", "development"
        uint64_t targetId; // NeuronId, SynapseId, PlasticityId, NeuromodulatorId, etc.
        std::string targetName;
        float oldValue;
        float newValue;
        float changeAmount;
        std::string units;
        double timestamp;
        bool isApplied;
        std::string changeReason;
        
        ParameterChange() : targetId(0), oldValue(0.0f), newValue(0.0f), changeAmount(0.0f), 
                          timestamp(0.0), isApplied(false), timestamp(0.0) {}
    };
    
    // Parameter modification API
    // Neural weight modification
    bool adjustWeight(SynapseId synapseId, float weightDelta, const std::string& reason = "");
    bool setWeight(SynapseId synapseId, float weight, const std::string& reason = "");
    bool adjustWeightByPrePost(NeuronId preId, NeuronId postId, float weightDelta, const std::string& reason = "");
    
    // Neuron threshold modification
    bool adjustThreshold(NeuronId neuronId, float thresholdDelta, const std::string& reason = "");
    bool setThreshold(NeuronId neuronId, float threshold, const std::string& reason = "");
    bool adjustThresholdByType(NeuronType type, float thresholdDelta, const std::string& reason = "");
    
    // Plasticity rule modification
    bool tunePlasticityRule(PlasticityId plasticityId, const PlasticityParameters& params, const std::string& reason = "");
    bool adjustSTDPParameters(float learningRate, float traceDecay, const std::string& reason = "");
    bool adjustHebbianParameters(float learningRate, float similarityThreshold, const std::string& reason = "");
    bool adjustStructuralPlasticityParameters(float synaptogenesisRate, float pruningRate, const std::string& reason = "");
    
    // Neuromodulator control
    bool modifyNeuromodulator(NeuromodulatorId neuromodulatorId, float level, const std::string& reason = "");
    bool setNeuromodulatorLevel(NeuromodulatorId neuromodulatorId, float level, const std::string& reason = "");
    bool adjustDopamineLevel(float delta, const std::string& reason = "");
    bool adjustAcetylcholineLevel(float delta, const std::string& reason = "");
    bool adjustNorepinephrineLevel(float delta, const std::string& reason = "");
    bool adjustSerotoninLevel(float delta, const std::string& reason = "");
    
    // Development system control
    bool setDevelopmentStage(DevelopmentalStage stage, const std::string& reason = "");
    bool adjustDevelopmentRate(float rateDelta, const std::string& reason = "");
    bool enableDevelopment(bool enable, const std::string& reason = "");
    bool setDevelopmentParameters(const DevelopmentParameters& params, const std::string& reason = "");
    
    // Advanced parameter control
    bool modifyPlasticityGating(float gatingFactor, const std::string& reason = "");
    bool adjustPlasticityThreshold(float threshold, const std::string& reason = "");
    bool modifySignalSynthesizedLevel(float level, const std::string& reason = "");
    bool adjustAdaptationRate(float rateDelta, const std::string& reason = "");
    bool modifyLearningRate(float rateDelta, const std::string& reason = "");
    
    // Batch parameter operations
    std::vector<ParameterChange> batchModifyParameters(const std::vector<ParameterChange>& changes);
    bool revertParameterChange(const ParameterChange& change);
    std::vector<ParameterChange> getParameterHistory() const;
    
    // Parameter validation
    bool validateParameterChange(const ParameterChange& change) const;
    std::vector<std::string> getValidationErrors() const;
    bool validateParameterRanges() const;
    bool validateParameterConsistency() const;
    
    // Parameter monitoring and tracking
    void enableParameterTracking(bool enable);
    bool isParameterTrackingEnabled() const;
    float getParameter(NeuronId neuronId, const std::string& parameterName) const;
    float getParameter(SynapseId synapseId, const std::string& parameterName) const;
    float getParameter(PlasticityId plasticityId, const std::string& parameterName) const;
    float getParameter(NeuromodulatorId neuromodulatorId, const std::string& parameterName) const;
    
    // Real-time modification control
    void pauseParameterModifications(bool pause);
    bool areParameterModificationsPaused() const;
    void setModificationRate(double rate); // modifications per second
    double getModificationRate() const;
    
    // Integration with simulation
    void applyChangesToSimulation();
    void commitChanges();
    void rollbackChanges();
    bool hasPendingChanges() const;
    
    // Session management
    void startParameterSession(const std::string& sessionName);
    void endParameterSession();
    bool saveParameterSession(const std::string& filename);
    bool loadParameterSession(const std::string& filename);
    std::string getCurrentSessionName() const;
    
    // Statistics and analysis
    struct ParameterStats {
        size_t totalChanges;
        size_t successfulChanges;
        size_t failedChanges;
        float averageChangeMagnitude;
        float parameterStabilityIndex;
        std::unordered_map<std::string, float> parameterVariances;
        std::unordered_map<std::string, float> parameterMeans;
        double sessionDuration;
        
        ParameterStats() : totalChanges(0), successfulChanges(0), failedChanges(0),
                         averageChangeMagnitude(0.0f), parameterStabilityIndex(1.0f),
                         sessionDuration(0.0) {}
    };
    
    ParameterStats getParameterStatistics() const;
    std::vector<float> getParameterDistribution(const std::string& parameterName) const;
    float calculateParameterSensitivity(NeuronId neuronId, const std::string& parameterName) const;
    
    // Configuration
    void setMaxParameterChangesPerSecond(size_t maxChanges);
    void setParameterChangeTimeout(double timeout);
    void enableParameterValidation(bool enable);
    bool isParameterValidationEnabled() const;
    void setSafetyMargin(float margin);
    float getSafetyMargin() const;
    
    // Thread safety
    void lockParameterController() const;
    void unlockParameterController() const;
    bool tryLockParameterController() const;
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<BrainSimulator> simulator_;
    std::shared_ptr<Config> config_;
    
    mutable std::mutex parameterMutex_;
    std::atomic<bool> trackingEnabled_;
    std::atomic<bool> modificationsPaused_;
    double modificationRate_;
    double modificationInterval_;
    size_t maxChangesPerSecond_;
    double parameterChangeTimeout_;
    bool validationEnabled_;
    float safetyMargin_;
    
    // Parameter history and changes
    std::vector<ParameterChange> parameterChanges_;
    std::string currentSessionName_;
    double sessionStartTime_;
    
    // Parameter values
    std::unordered_map<uint64_t, std::unordered_map<std::string, float>> neuronParameters_;
    std::unordered_map<uint64_t, std::unordered_map<std::string, float>> synapseParameters_;
    std::unordered_map<uint64_t, std::unordered_map<std::string, float>> plasticityParameters_;
    std::unordered_map<uint64_t, std::unordered_map<std::string, float>> neuromodulatorParameters_;
    
    // Validation errors
    std::vector<std::string> validationErrors_;
    
    // Internal helper methods
    void cleanup();
    bool applySingleParameterChange(const ParameterChange& change);
    void updateParameterValue(uint64_t targetId, const std::string& parameterName, float newValue);
    float getCurrentParameterValue(uint64_t targetId, const std::string& parameterName) const;
    bool validateParameterBounds(float value, const std::string& parameterName) const;
    void logParameterChange(const ParameterChange& change);
    void updateParameterStatistics();
    void enforceSafetyMargin();
    bool checkForConflicts(const ParameterChange& change) const;
    std::vector<std::string> getConflictingChanges(const ParameterChange& change) const;
};

} // namespace nlm