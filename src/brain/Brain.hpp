#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

namespace nlm {

class Config;
class RandomGenerator;
class SimulationClock;
class Logger;

// Forward declarations for convenience methods
class SensoryInput;
class Action;

// Inter-regional connection (long-range connectivity)
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems
// Enhanced with convenience methods for batch operations and improved usability

class Brain {
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
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Batch operations for injecting current
    
    // Inject current to multiple neurons efficiently
    void injectCurrentToNeurons(const std::vector<NeuronId>& neurons, MembranePotential current);
    
    // Inject current to all neurons of a specific type (convenience overload)
    void injectCurrentToNeurons(NeuronType type, MembranePotential current, bool useTiming = false);
    
    // Run multiple simulation steps efficiently
    void runSteps(size_t numSteps, Timestamp timestep = 0.001f);
    
    // Run simulation with timing control
    void runSteps(size_t numSteps, const std::vector<Timestamp>& timings);
    
    // Statistical batch operations
    
    // Get statistics for all neuron types
    std::map<NeuronType, size_t> getNeuronCountByType() const;
    std::map<NeuronType, float> getFiringRateByType() const;
    std::map<NeuronType, float> getAveragePotentialByType() const;
    
    // Get regional statistics
    struct RegionStats {
        RegionId id;
        std::string name;
        size_t neuronCount;
        size_t synapseCount;
        float firingRate;
        float averagePotential;
        std::map<NeuronType, size_t> neuronTypeCounts;
    };
    
    RegionStats getRegionStats(RegionId regionId) const;
    std::vector<RegionStats> getAllRegionStats() const;
    
    // Memory system convenience methods
    
    // Working memory operations
    void updateWorkingMemory(const std::vector<float>& inputPattern);
    float getWorkingMemoryActivity(WorkingMemoryTraceId traceId) const;
    std::vector<float> getWorkingMemoryActivation() const;
    
    // Episodic memory operations
    void storeEpisodicMemory(const std::vector<float>& pattern, float importance = 1.0f);
    std::vector<float> retrieveEpisodicMemory(size_t episodeIndex = 0) const;
    float getEpisodicMemoryImportance(size_t episodeIndex) const;
    
    // Prediction system convenience methods
    
    // Prediction operations
    std::vector<float> computePrediction(const std::vector<float>& input) const;
    float getPredictionError() const;
    void updatePrediction(const std::vector<float>& prediction);
    
    // Neuromodulation convenience methods
    
    // Neuromodulator operations
    void applyRewardModulation(float reward, float predictedReward = 0.0f);
    void applyCuriosityModulation(float novelty, float explorationDrive = 0.0f);
    void applyDopamineModulation(float predictionError, float learningRate = 0.001f);
    
    // Development system convenience methods
    
    // Development operations
    void advanceDevelopmentStage();
    void accelerateDevelopment(float factor = 1.0f);
    void applyExperienceBasedDevelopment(const std::vector<float>& experience);
    
    // Cognition system convenience methods
    
    // Planning and decision making
    std::vector<ActionType> planActions(size_t planningHorizon = 5) const;
    float evaluateActionPlan(const std::vector<ActionType>& plan) const;
    std::vector<ActionType> refineActionPlan(const std::vector<ActionType>& currentPlan,
                                             const std::vector<float>& feedback);
    
    // Concept formation
    std::vector<float> formConcepts(const std::vector<std::vector<float>>& patterns) const;
    float getConceptSimilarity(const std::vector<float>& concept1, const std::vector<float>& concept2) const;
    
    // Attention operations
    std::vector<NeuronId> selectAttentiveFocus(const std::vector<float>& attentionWeights) const;
    void adjustAttentionGating(float gain, float threshold = 0.5f);
    
    // Spike system convenience methods
    
    // Spike analysis and control
    std::vector<NeuronId> getActiveNeurons() const;
    std::vector<NeuronId> getFiringNeurons() const;
    std::vector<NeuronId> getInactiveNeurons() const;
    
    // Spike statistics
    float getGlobalFiringRate() const;
    std::map<NeuronType, float> getFiringRateByNeuronType() const;
    std::map<NeuronType, float> getExcitatoryInhibitoryRatio() const;
    
    // Connectivity statistics
    float getAverageConnectionStrength() const;
    size_t getEffectiveConnectivityCount() const;
    float getRecurrentConnectionRatio() const;
    
    // Plasticity convenience methods
    
    // Plasticity operations
    void applySTDP(float learningRate = 0.001f, float tauPlus = 20.0f, float tauMinus = 20.0f);
    void applyHebbianLearning(float learningRate = 0.001f);
    void applyStructuralPlasticity(float growthRate = 0.0001f, float pruningRate = 0.00001f);
    
    // Synapse management
    void strengthenSynapses(const std::vector<std::pair<SynapseId, float>>& synapseWeightChanges);
    void weakenSynapses(const std::vector<std::pair<SynapseId, float>>& synapseWeightChanges);
    std::vector<SynapseId> getStrongSynapses(float threshold = 0.5f) const;
    std::vector<SynapseId> getWeakSynapses(float threshold = 0.1f) const;
    
    // Neuron convenience methods
    
    // Neuron management
    void addNeuronsToRegion(const std::vector<NeuronId>& neurons, RegionId regionId);
    void removeNeuronsFromRegion(const std::vector<NeuronId>& neurons, RegionId regionId);
    std::vector<NeuronId> getNeuronsInRegion(RegionId regionId) const;
    
    // Neuron state manipulation
    void setNeuronExcitability(NeuronId neuron, float excitability);
    void setNeuronInhibitoryStrength(NeuronId neuron, float strength);
    void setNeuronRefractoryPeriod(NeuronId neuron, float period);
    
    // Batch neuron operations
    void setRegionExcitability(RegionId regionId, float excitability);
    void setRegionInhibitoryStrength(RegionId regionId, float strength);
    float getRegionAverageExcitability(RegionId regionId) const;
    
    // Simulation control convenience methods
    
    // Simulation management
    void resetBrain();
    void resetToCheckpoint(const std::string& checkpointPath);
    bool saveToCheckpoint(const std::string& checkpointPath) const;
    
    // State analysis
    std::vector<float> getBrainStateVector() const;
    float calculateBrainComplexity() const;
    float calculateLearningProgress() const;
    
    // Time and step management
    void setSimulationTimestep(float timestep);
    float getSimulationTimestep() const;
    void advanceSimulationTime(float timeDelta);
    Timestamp getCurrentSimulationTime() const;
    SimulationStep getCurrentStep() const;
    
    // Performance and monitoring convenience methods
    
    // Performance analysis
    float getAverageMembranePotential() const;
    float getPeakMembranePotential() const;
    float getMinMembranePotential() const;
    std::pair<float, float> getMembranePotentialRange() const;
    
    // Energy and resource management
    float getTotalNeuralEnergy() const;
    float getEnergyEfficiency() const;
    void allocateResources(float energyBudget);
    bool checkResourceAvailability(float requiredEnergy) const;
    
    // Monitoring and logging convenience methods
    
    // Status reporting
    std::string getSystemStatus() const;
    std::vector<std::string> getSystemHealth() const;
    std::map<std::string, float> getPerformanceMetrics() const;
    
    // Alerts and warnings
    bool checkForAnomalies() const;
    std::vector<std::string> getActiveAlerts() const;
    void clearAlerts();
    
    // Configuration convenience methods
    
    // Configuration operations
    bool updateConfigurationParameter(const std::string& key, const ConfigValue& value);
    bool batchUpdateConfiguration(const std::map<std::string, ConfigValue>& configUpdates);
    ConfigValue getConfigurationParameter(const std::string& key) const;
    std::map<std::string, ConfigValue> getAllConfigurationParameters() const;
    
    // Configuration analysis
    std::vector<std::string> getActiveConfigurationSources() const;
    std::map<ConfigSource, size_t> getConfigurationSourceCounts() const;
    std::string getConfigurationSummary() const;
    
    // Error handling and recovery
    
    // Error detection and recovery
    bool detectErrors() const;
    std::vector<std::string> getRecentErrors() const;
    void clearErrors();
    bool attemptRecovery() const;
    
    // Diagnostic tools
    
    // Brain state diagnostics
    bool validateBrainIntegrity() const;
    std::vector<std::string> getBrainDiagnostics() const;
    std::map<std::string, float> getResourceUsage() const;
    
    // Performance profiling
    void startProfiling();
    void stopProfiling();
    bool isProfiling() const;
    std::map<std::string, float> getProfilingResults() const;
    
    // Original methods (keep all existing functionality)
    
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
    std::unique_ptr<Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const Neuromodulator& signal);
    
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
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    
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
    
    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
