// brain/BrainImpl.hpp - Brain implementation class declaration
// Forward declaration for the pImpl implementation

#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

/**
 * @class Brain::Impl
 * @brief Implementation of Brain's pImpl class to reduce compilation dependencies
 * 
 * This implementation class contains all the details of the Brain class,
 * allowing the header (Brain.hpp) to have minimal dependencies. All the
 * concrete implementation details are separated here to reduce compilation
 * dependencies and improve modularity.
 * 
 * The implementation class includes:
 * - All neural system implementations (working memory, episodic memory, etc.)
 * - Simulation state and parameters
 * - Statistics and monitoring
 * - Configuration management
 * - Integration state variables
 * - Spike systems and plasticity mechanisms
 * 
 * This pImpl (pointer to implementation) pattern allows Brain users to
 * interact only with the interface while keeping the implementation details
 * hidden in this separate class.
 * 
 * @note The implementation class is used as pImpl (pointer to implementation)
 *       in the Brain class to reduce compilation dependencies and improve
 *       modularity.
 * 
 * @see nlm::Brain for the interface class
 * @see nlm::Brain::Impl for the actual implementation
 * 
 * The implementation follows the Phase 6 (Final Integration) architecture:
 * - Integrated memory systems (working, episodic, associative)
 * - Prediction systems for sensory prediction and error computation
 * - Cognition systems (neural planning, concept formation, attention)
 * - Development systems for plasticity modulation over time
 * - Neuromodulation systems (dopamine, curiosity, novelty)
 * - Structural and functional plasticity mechanisms
 */
class Brain::Impl {
public:
    // Constructor only - implementations are in separate .cpp files
    Impl(std::shared_ptr<Config> cfg);
    
    // ========== INITIALIZATION ==========
    bool initialize();
    
    // ========== STEP PROCESSING ==========
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // ========== ACCESSORS ==========
    float getExcitationInhibitionRatio() const;
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getAverageFiringRate() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    size_t getTotalSpikesTotal() const;
    size_t getTotalSpikesThisStep() const;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    std::vector<RegionId> getRegionIds() const;
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    std::optional<RegionId> addRegion(const std::string& name = "");
    size_t getRegionCount() const;
    void addInterRegionConnection(RegionId source, RegionId target,
                                 float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // ========== UTILITY METHODS ==========
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // ========== SYSTEMS ACCESS ==========
    std::unique_ptr<Action> produceAction();
    void applyNeuromodulation(const Neuromodulator& signal);
    void updatePlasticity();
    void develop();
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // ========== MEMORY ACCESS ==========
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    DevelopmentSystem* getDevelopmentSystem();
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // ========== STATISTICS ==========
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    SpikeSystem* getSpikeSystem();
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    void logStatus() const;
    
    // Integration state variables
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // ========== INTEGRATED MEMORY SYSTEMS ==========
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // ========== INTEGRATED PREDICTION SYSTEM ==========
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // ========== INTEGRATED COGNITION SYSTEMS ==========
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // ========== DEVELOPMENT SYSTEM ==========
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Simulation parameters
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    // Statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // Sensory neurons for input injection
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Integration state
    bool isResting;  // For sleep/rest cycle
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

} // namespace nlm