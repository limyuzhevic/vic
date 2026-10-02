#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
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

// ========== REFACTORED: NEURAL DYNAMICS MODULE ==========
class NeuralDynamicsStep {
public:
    void updateNeuronDynamics(
        std::vector<std::unique_ptr<NeuralRegion>>& regions,
        const TimestepDuration& timestep,
        Timestamp currentTime);
    
    void detectAndQueueSpikes(
        std::vector<std::unique_ptr<NeuralRegion>>& regions,
        SpikeSystem* spikeSystem,
        const TimestepDuration& timestep,
        Timestamp currentTime,
        SimulationStep currentStep,
        size_t& totalSpikesThisStep);
    
    void processSpikes(
        SpikeSystem* spikeSystem,
        SimulationStep currentStep);
    
    void injectSensoryInput(
        std::vector<Neuron*>& sensoryNeurons,
        const class SensoryInput& input);
    
    void injectCurrentToNeurons(
        std::vector<std::unique_ptr<NeuralRegion>>& regions,
        NeuronType type,
        MembranePotential current);
    
private:
    void updateIndividualNeuronLIF(
        Neuron* neuron,
        const TimestepDuration& timestep,
        Timestamp currentTime);
    
    void handleNeuronFiring(
        Neuron* neuron,
        NeuralRegion* region,
        SpikeSystem* spikeSystem,
        const TimestepDuration& timestep,
        Timestamp currentTime,
        SimulationStep currentStep,
        size_t& totalSpikesThisStep);
};

// ========== REFACTORED: MEMORY SYSTEMS MODULE ==========
class MemorySystemsStep {
public:
    void updateWorkingMemory(
        NeuralWorkingMemory* workingMemory,
        const TimestepDuration& timestep);
    
    void updateEpisodicMemory(
        NeuralEpisodicMemory* episodicMemory,
        size_t stepsSinceLastEpisode,
        Timestamp currentTime,
        const TimestepDuration& timestep,
        const Dopamine* dopamine,
        std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    void processMemoryReplay(
        NeuralEpisodicMemory* episodicMemory,
        size_t replayInterval,
        size_t currentStep);
    
    void consolidateMemories(
        NeuralEpisodicMemory* episodicMemory,
        size_t consolidationInterval,
        size_t currentStep,
        float consolidationStrength = 0.3f);
    
    void updateWorkingMemoryFromSensory(
        NeuralWorkingMemory* workingMemory,
        std::vector<Neuron*>& sensoryNeurons,
        const class SensoryInput& input);
    
private:
    void captureCurrentStateForEpisode(
        NeuralEpisodicMemory* episodicMemory,
        Timestamp currentTime,
        const Dopamine* dopamine,
        std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    void prepareMemoryForReplay(
        NeuralEpisodicMemory* episodicMemory,
        size_t episodeCount);
};

// ========== REFACTORED: NEUROMODULATION MODULE ==========
class NeuromodulationStep {
public:
    void updateNoveltyDetection(
        Novelty* novelty,
        const TimestepDuration& timestep);
    
    void updateCuriositySignals(
        Curiosity* curiosity,
        const TimestepDuration& timestep);
    
    void updateDopamineSystem(
        Dopamine* dopamine,
        const TimestepDuration& timestep);
    
    void applyDopamineToExcitability(
        Dopamine* dopamine,
        std::vector<std::unique_ptr<NeuralRegion>>& regions,
        const TimestepDuration& timestep);
    
    void applyNeuromodulationToPlasticity(
        Dopamine* dopamine,
        STDP* stdp);
    
    void integratePredictionError(
        PredictionError* predictionError,
        PredictionSystem* predictionSystem,
        const TimestepDuration& timestep);
    
private:
    void calculateNoveltySignals(
        Novelty* novelty,
        std::vector<std::unique_ptr<NeuralRegion>>& regions,
        float& noveltyLevel);
};

// ========== REFACTORED: PLASTICITY MODULE ==========
class PlasticityStep {
public:
    void applySTDPPlasticity(
        std::vector<std::unique_ptr<NeuralRegion>>& regions,
        STDP* stdp,
        float plasticityMod,
        const TimestepDuration& timestep);
    
    void applyHebbianPlasticity(
        std::vector<std::unique_ptr<NeuralRegion>>& regions,
        Hebbian* hebbian,
        const TimestepDuration& timestep);
    
    void applyStructuralPlasticity(
        StructuralPlasticity* structuralPlasticity,
        size_t currentStep,
        DevelopmentalStage developmentalStage,
        const TimestepDuration& timestep);
    
    void updateSynapseState(
        std::vector<std::unique_ptr<NeuralRegion>>& regions,
        const TimestepDuration& timestep,
        Timestamp currentTime);
    
    void processSpikeForPlasticity(
        Neuron* neuron,
        NeuralRegion* region,
        STDP* stdp,
        Hebbian* hebbian,
        const TimestepDuration& timestep,
        Timestamp currentTime);
    
private:
    void applyDopamineModulation(
        STDP* stdp,
        float dopamineLevel);
};

// ========== REFACTORED: COGNITION MODULE ==========
class CognitionStep {
public:
    void updateAttentionalSelection(
        AttentionalSelection* attention,
        NeuralWorkingMemory* workingMemory,
        const TimestepDuration& timestep);
    
    void updateNeuralPlanning(
        NeuralPlanner* planner,
        const TimestepDuration& timestep);
    
    void updateConceptFormation(
        ConceptFormation* conceptFormation,
        std::vector<std::unique_ptr<NeuralRegion>>& regions,
        const TimestepDuration& timestep);
    
    void applyAttentionToWorkingMemory(
        AttentionalSelection* attention,
        NeuralWorkingMemory* workingMemory);
    
    void processCognitiveOutput(
        NeuralPlanner* planner,
        ConceptFormation* conceptFormation);
    
private:
    void encodeCurrentStateForConceptLearning(
        std::vector<std::unique_ptr<NeuralRegion>>& regions,
        ConceptFormation* conceptFormation,
        const TimestepDuration& timestep);
};

// ========== REFACTORED: PREDICTION MODULE ==========
class PredictionStep {
public:
    void updatePredictionSystem(
        PredictionSystem* predictionSystem,
        const class SensoryInput& input,
        PredictionError* predictionError,
        const TimestepDuration& timestep);
    
    void processPredictionErrors(
        PredictionError* predictionError,
        PredictionSystem* predictionSystem);
    
    void integratePredictionsWithNeurons(
        PredictionSystem* predictionSystem,
        std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    void updateExpectedStates(
        PredictionSystem* predictionSystem,
        Timestamp currentTime);
    
private:
    void generatePredictions(
        PredictionSystem* predictionSystem,
        const class SensoryInput& input);
    
    void computePredictionErrors(
        PredictionError* predictionError,
        PredictionSystem* predictionSystem,
        const class SensoryInput& input);
};

// ========== REFACTORED: DEVELOPMENT MODULE ==========
class DevelopmentStep {
public:
    void applyDevelopmentEffects(
        DevelopmentSystem* developmentSystem,
        StructuralPlasticity* structuralPlasticity,
        size_t currentStep,
        const TimestepDuration& timestep,
        DevelopmentalStage& developmentalStage);
    
    void calculateDevelopmentModulation(
        DevelopmentalStage stage,
        float& plasticityMod,
        float& synaptogenesisRate,
        float& pruningRate);
    
    void adaptSystemParameters(
        DevelopmentSystem* developmentSystem,
        StructuralPlasticity* structuralPlasticity,
        DevelopmentalStage stage);
    
    void updateMaturationTimeline(
        size_t currentStep,
        DevelopmentalStage& developmentalStage);
    
private:
    void initializeDevelopmentalParameters(
        DevelopmentSystem* developmentSystem);
};

// ========== REFACTORED: CHECKPOINT MODULE ==========
class CheckpointStep {
public:
    void manageCheckpoints(
        CheckpointManager* checkpointManager,
        size_t currentStep,
        Timestamp currentTime);
    
    void saveBrainState(
        const std::string& filepath,
        Brain* brain,
        const TimestepDuration& timestep);
    
    void loadBrainState(
        const std::string& filepath,
        Brain* brain);
    
    void cleanupOldCheckpoints(
        CheckpointManager* checkpointManager,
        int maxCheckpoints,
        size_t currentStep);
    
private:
    void createCheckpointData(
        CheckpointWriter& writer,
        const Brain* brain,
        const TimestepDuration& timestep);
    
    void restoreCheckpointData(
        CheckpointReader& reader,
        Brain* brain);
};

// ========== REFACTORED: STATISTICS MODULE ==========
class StatisticsStep {
public:
    void collectAndReportStatistics(
        const std::vector<std::unique_ptr<NeuralRegion>>& regions,
        size_t totalSpikesThisStep,
        size_t totalSpikesTotal,
        SpikeSystem* spikeSystem,
        const TimestepDuration& timestep,
        Timestamp currentTime);
    
    void updateMemoryStatistics(
        const NeuralWorkingMemory* workingMemory,
        const NeuralEpisodicMemory* episodicMemory);
    
    void updateNeuromodulationStatistics(
        const Dopamine* dopamine,
        const Curiosity* curiosity,
        const Novelty* novelty,
        const PredictionError* predictionError);
    
    void updatePlasticityStatistics(
        const STDP* stdp,
        const Hebbian* hebbian,
        const StructuralPlasticity* structuralPlasticity);
    
    void logSystemStatus(
        const std::vector<std::unique_ptr<NeuralRegion>>& regions,
        size_t totalSpikesThisStep,
        size_t totalSpikesTotal,
        SpikeSystem* spikeSystem,
        const TimestepDuration& timestep,
        Timestamp currentTime);
    
private:
    void calculateEIEratio(
        const std::vector<std::unique_ptr<NeuralRegion>>& regions,
        float& eiratio);
    
    void formatMemoryStats(
        const NeuralWorkingMemory* workingMemory,
        const NeuralEpisodicMemory* episodicMemory,
        std::string& memoryStats);
    
    void formatNeuromodulationStats(
        const Dopamine* dopamine,
        const Curiosity* curiosity,
        const Novelty* novelty,
        const PredictionError* predictionError,
        std::string& neuromodStats);
};

// Main Brain class with modular step() method
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
    
    // Main simulation step - now delegates to modular components
    void step(SimulationStep currentStep);
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
    
    // ========== MODULAR COMPONENTS ==========
    NeuralDynamicsStep neuralDynamicsStep;
    MemorySystemsStep memorySystemsStep;
    NeuromodulationStep neuromodulationStep;
    PlasticityStep plasticityStep;
    CognitionStep cognitionStep;
    PredictionStep predictionStep;
    DevelopmentStep developmentStep;
    CheckpointStep checkpointStep;
    StatisticsStep statisticsStep;
    
    // Private helper methods for step delegation
    void executeStepPhase1(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep);
    void executeStepPhase2(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep);
    void executeStepPhase3(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep);
    void executeStepPhase4(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep);
    void executeStepPhase5(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep);
    void executeStepPhase6(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep);
    void executeStepPhase7(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep);
    void executeStepPhase8(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep);
    void executeStepPhase9(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep);
    void executeStepPhase10(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep);
    void executeStepPhase11(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep);
    void executeStepPhase12(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep);
    void executeStepPhase13(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep);
    void executeStepPhase14(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep);
    void executeStepPhase15(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep);
    
    // Initialize modules with brain instance
    void initializeModules();
};

} // namespace nlm

#endif // BRAIN_REFACTORED_HPP