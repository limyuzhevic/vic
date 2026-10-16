#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

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
    
    // Advanced plasticity control
    void enableRewardModulatedSTDP(bool enable);
    void enableTripletSTDP(bool enable);
    void configureHebbianMode(const std::string& mode);
    void setCuriosityParameters(float noveltyWeight, float predictionErrorWeight);
    
    // Advanced configuration methods
    void loadFromJSON(const std::string& jsonString, bool validate = false);
    void setWithSource(const std::string& key, const ConfigValue& value, ConfigSource source);
    void saveToJSON(std::string& jsonString) const;
    bool validateConfig(std::string& errorMessage) const;
    
    // Visualization and analysis methods
    void createVisualizationPipeline();
    void analyzeConnectivity() const;
    void computeGraphMetrics() const;
    void createEmbedding(size_t dimensions, std::vector<float>& embedding) const;
    
    // Batch processing methods
    std::vector<std::shared_ptr<Experiment>> runSimulationBatch(const std::vector<std::shared_ptr<Config>>& configs,
                                                                const std::shared_ptr<Environment>& environmentTemplate,
                                                                SimulationStep maxStepsPerSimulation);
    std::shared_ptr<Config> runHyperparameterSearch(const std::vector<std::string>& hyperparameterNames,
                                                  const std::vector<std::vector<ConfigValue>>& hyperparameterValues,
                                                  const std::shared_ptr<Environment>& environmentTemplate,
                                                  SimulationStep maxSteps);
    std::vector<std::shared_ptr<Experiment>> runCrossValidation(const std::vector<std::shared_ptr<Config>>& configs,
                                                                const std::shared_ptr<Environment>& environmentTemplate,
                                                                SimulationStep maxSteps,
                                                                double trainTestRatio = 0.8);
    
    // State management
    void saveSimulationState(const std::string& filepath, const std::string& metadata = "") const;
    bool loadSimulationState(const std::string& filepath);
    
    // Advanced agent capabilities
    std::shared_ptr<Brain> createCustomAgent(const std::vector<NeuronType>& neuronTypes,
                                            const std::vector<std::pair<RegionId, RegionId>>& connections,
                                            const std::shared_ptr<Config>& customConfig);
    std::vector<std::shared_ptr<Brain>> enableMultiAgentSystem(size_t agentCount,
                                                              const std::shared_ptr<Config>& templateConfig);
    std::shared_ptr<Brain> createHierarchicalAgent(const std::vector<size_t>& layerNeuronCounts,
                                                  const std::vector<std::string>& layerTypes,
                                                  const std::shared_ptr<Config>& hierarchicalConfig);
    
    // Advanced error handling and debugging
    void setVerboseLogging(bool enable);
    std::shared_ptr<Profiler> createProfiler(const std::string& name = "default");
    std::shared_ptr<ErrorAnalyzer> createErrorAnalyzer();
    
    // Status and monitoring
    void logDetailedStatus() const;
    std::string getSystemHealthReport() const;
    std::vector<std::string> getPerformanceMetrics() const;
    
    // Export/import
    bool exportModel(const std::string& filepath, const std::string& format = "binary") const;
    bool importModel(const std::string& filepath, const std::string& format = "binary");
    
    // Advanced features access
    std::shared_ptr<AdvancedSTDP> getAdvancedSTDP() const;
    std::shared_ptr<VisualizationInterface> getVisualizationInterface() const;
    std::shared_ptr<Profiler> getSystemProfiler() const;
    std::shared_ptr<ErrorAnalyzer> getErrorAnalyzer() const;
    
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
