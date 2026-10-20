#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <optional>
#include <functional>
#include <chrono>
#include <unordered_map>

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
class CheckpointManager;

// Enhanced memory system information
struct MemorySystemStatus {
    size_t workingMemoryTraces;
    size_t episodicMemoryEpisodes;
    float conceptActivation;
    float associativeStrength;
};

// Brain configuration options
struct BrainConfig {
    uint64_t randomSeed;
    uint64_t maxNeurons;
    uint64_t maxRegions;
    float connectionProbability;
    float stdpLtpWeight;
    float stdpLtdWeight;
    float stdpTau;
    float synaptogenesisRate;
    float pruningRate;
    size_t replayInterval;
    size_t consolidationInterval;
    bool enableDevelopment;
    bool enableCheckpointing;
    std::string checkpointPath;
    std::string logLevel;
    
    BrainConfig()
        : randomSeed(42),
          maxNeurons(1000),
          maxRegions(1),
          connectionProbability(0.1f),
          stdpLtpWeight(0.01f),
          stdpLtdWeight(0.012f),
          stdpTau(20.0f),
          synaptogenesisRate(0.0001f),
          pruningRate(0.00001f),
          replayInterval(100),
          consolidationInterval(1000),
          enableDevelopment(true),
          enableCheckpointing(true),
          checkpointPath("./checkpoints/brain_checkpoint.bin"),
          logLevel("INFO") {}
};

// Brain status for monitoring and debugging
struct BrainStatus {
    size_t totalNeurons;
    size_t totalSynapses;
    size_t activeNeurons;
    size_t firingNeurons;
    size_t totalSpikes;
    size_t pendingSpikeEvents;
    float averageFiringRate;
    float excitationInhibitionRatio;
    uint64_t currentStep;
    double currentTime;
    DevelopmentalStage developmentalStage;
    std::unordered_map<std::string, float> memorySystemStats;
    std::unordered_map<std::string, float> neuromodulatorStats;
    
    BrainStatus()
        : totalNeurons(0),
          totalSynapses(0),
          activeNeurons(0),
          firingNeurons(0),
          totalSpikes(0),
          pendingSpikeEvents(0),
          averageFiringRate(0.0f),
          excitationInhibitionRatio(0.0f),
          currentStep(0),
          currentTime(0.0),
          developmentalStage(DevelopmentalStage::Initial) {}
};

// Brain builder for fluent construction
class BrainBuilder {
public:
    BrainBuilder();
    ~BrainBuilder();
    
    // Configuration setters
    BrainBuilder& withRandomSeed(uint64_t seed);
    BrainBuilder& withNeuronCount(size_t count);
    BrainBuilder& withRegionCount(size_t count);
    BrainBuilder& withConnectionProbability(float prob);
    BrainBuilder& withSTDPParameters(float ltp, float ltd, float tau);
    BrainBuilder& withDevelopmentEnabled(bool enabled);
    BrainBuilder& withCheckpointingEnabled(bool enabled);
    BrainBuilder& withCheckpointPath(const std::string& path);
    BrainBuilder& withLogLevel(const std::string& level);
    
    // Component configuration
    BrainBuilder& enableMemorySystem(bool enable);
    BrainBuilder& enablePredictionSystem(bool enable);
    BrainBuilder& enableCognitionSystems(bool enable);
    BrainBuilder& enableNeuromodulation(bool enable);
    BrainBuilder& enableDevelopment(bool enable);
    
    // Customization
    BrainBuilder& addCustomRegion(const std::string& name, size_t neuronCount);
    BrainBuilder& configureRegion(RegionId id, const std::string& name);
    
    // Validation
    bool validate() const;
    std::string validationError() const;
    
    // Build
    std::shared_ptr<Brain> build() const;
    
    // Post-construction configuration
    static std::shared_ptr<Brain> fromConfig(const std::shared_ptr<Config>& config);
    static std::shared_ptr<Brain> fromBrainConfig(const BrainConfig& config);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Brain: Enhanced artificial neural system with improved PIMPL
class Brain {
public:
    // Factory methods for easy creation
    static std::shared_ptr<Brain> create(const BrainConfig& config);
    static std::shared_ptr<Brain> create(const std::shared_ptr<Config>& config);
    static std::shared_ptr<Brain> createFromFile(const std::string& configFile);
    
    // Builder-based creation
    static std::shared_ptr<Brain> build() {
        return BrainBuilder().build();
    }
    
    // Traditional constructor for backward compatibility
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    bool initialize(const BrainConfig& config);
    
    // Main simulation step with enhanced error handling
    void step(SimulationStep currentStep);
    
    // Simulation time step with validation
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
    
    // Produce motor/action output
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules
    void updatePlasticity();
    
    // Apply developmental changes
    void develop();
    
    // Reset and checkpoint management
    void reset();
    void resetToCheckpoint(const std::string& checkpointPath);
    
    // Save and load
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1,
                                  ConnectivityType type = ConnectivityType::FeedForward,
                                  ConnectionStrength strength = Strength::Weak);
    void removeInterRegionConnection(RegionId source, RegionId target);
    std::vector<InterRegionConnection> getInterRegionConnections() const;
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Memory systems
    NeuralWorkingMemory* getWorkingMemory();
    const NeuralWorkingMemory* getWorkingMemory() const;
    
    NeuralEpisodicMemory* getEpisodicMemory();
    const NeuralEpisodicMemory* getEpisodicMemory() const;
    
    NeuralAssociativeMemory* getAssociativeMemory();
    const NeuralAssociativeMemory* getAssociativeMemory() const;
    
    // Prediction system
    PredictionSystem* getPredictionSystem();
    const PredictionSystem* getPredictionSystem() const;
    
    // Cognition systems
    NeuralPlanner* getPlanner();
    const NeuralPlanner* getPlanner() const;
    
    ConceptFormation* getConceptFormation();
    const ConceptFormation* getConceptFormation() const;
    
    AttentionalSelection* getAttention();
    const AttentionalSelection* getAttention() const;
    
    // Development system
    DevelopmentSystem* getDevelopmentSystem();
    const DevelopmentSystem* getDevelopmentSystem() const;
    
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Neuromodulation systems
    Dopamine* getDopamine();
    const Dopamine* getDopamine() const;
    
    Curiosity* getCuriosity();
    const Curiosity* getCuriosity() const;
    
    Novelty* getNovelty();
    const Novelty* getNovelty() const;
    
    PredictionError* getPredictionErrorSignal();
    const PredictionError* getPredictionErrorSignal() const;
    
    // Configuration
    std::shared_ptr<const Config> getConfig() const;
    BrainConfig getBrainConfig() const;
    
    // Random generator
    RandomGenerator* getRandomGenerator();
    const RandomGenerator* getRandomGenerator() const;
    
    // Simulation control
    SimulationStep getCurrentStep() const;
    Timestamp getCurrentTime() const;
    double getTimestep() const;
    
    // Logging and monitoring
    void logStatus() const;
    BrainStatus getStatus() const;
    
    // Validation
    bool validate() const;
    std::string validationReport() const;
    
    // Experimental features
    void enableReplay(bool enable);
    void enableConsolidation(bool enable);
    void enableDevelopment(bool enable);
    
    // Version and phase information
    static std::string getVersion();
    static std::string getPhase();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Enhanced brain factory functions
namespace brain {
    std::shared_ptr<Brain> createStandardBrain(size_t neuronCount = 500);
    std::shared_ptr<Brain> createResearchBrain();
    std::shared_ptr<Brain> createDemoBrain();
    
    bool validateBrainConfiguration(const BrainConfig& config);
    std::vector<std::string> getValidationErrors(const BrainConfig& config);
}

} // namespace nlm