#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Brain;
class NeuralRegion;
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
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

// Integration state
struct IntegrationState {
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    bool spikeHandlersRegistered;
    
    IntegrationState()
        : isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)
        , consolidationInterval(1000)
        , spikeHandlersRegistered(false) {}
};

// Simulation state
struct SimulationState {
    SimulationStep currentStep;
    Timestamp currentTime;
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    float learnedPreference;
    
    SimulationState()
        : currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , learnedPreference(0.0f) {}
};

// Brain System: Main coordinator of all brain systems
class BrainSystem {
public:
    BrainSystem(std::shared_ptr<Config> config);
    ~BrainSystem();
    
    bool initialize(Brain* brain);
    void reset();
    void logStatus() const;
    
    // System accessors
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory_.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory_.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory_.get(); }
    PredictionSystem* getPredictionSystem() { return predictionSystem_.get(); }
    NeuralPlanner* getPlanner() { return planner_.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation_.get(); }
    AttentionalSelection* getAttention() { return attention_.get(); }
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem_.get(); }
    Dopamine* getDopamine() { return dopamine_.get(); }
    Curiosity* getCuriosity() { return curiosity_.get(); }
    Novelty* getNovelty() { return novelty_.get(); }
    PredictionError* getPredictionError() { return predictionError_.get(); }
    SpikeSystem* getSpikeSystem() { return spikeSystem_.get(); }
    STDP* getSTDP() { return stdp_.get(); }
    Hebbian* getHebbian() { return hebbian_.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity_.get(); }
    CheckpointManager* getCheckpointManager() { return checkpointManager_.get(); }
    
    // Neural system
    std::vector<NeuralRegion*>& getRegions() { return regions_; }
    std::vector<InterRegionConnection>& getInterRegionConnections() { return interRegionConnections_; }
    std::unordered_map<uint64_t, Neuron*>& getNeuronById() { return neuronById_; }
    std::vector<Neuron*>& getSensoryNeurons() { return sensoryNeurons_; }
    std::vector<Neuron*>& getMotorNeurons() { return motorNeurons_; }
    
    // Simulation state
    SimulationState& getSimulationState() { return simulationState_; }
    IntegrationState& getIntegrationState() { return integrationState_; }
    
    // Configuration
    std::shared_ptr<Config> getConfig() { return config_; }
    RandomGenerator* getRandomGenerator() { return rng_.get(); }
    
    // Step processing
    void processStep(Brain* brain);
    
private:
    std::shared_ptr<Config> config_;
    std::unique_ptr<RandomGenerator> rng_;
    
    // Neural systems
    std::vector<NeuralRegion*> regions_;
    std::vector<InterRegionConnection> interRegionConnections_;
    std::unordered_map<uint64_t, Neuron*> neuronById_;
    std::vector<Neuron*> sensoryNeurons_;
    std::vector<Neuron*> motorNeurons_;
    
    // Integrated memory systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory_;
    
    // Prediction system
    std::unique_ptr<PredictionSystem> predictionSystem_;
    
    // Cognition systems
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<AttentionalSelection> attention_;
    
    // Development system
    std::unique_ptr<DevelopmentSystem> developmentSystem_;
    
    // Neuromodulation systems
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<PredictionError> predictionError_;
    std::unique_ptr<Novelty> novelty_;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem_;
    std::unique_ptr<STDP> stdp_;
    std::unique_ptr<Hebbian> hebbian_;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity_;
    
    // State
    SimulationState simulationState_;
    IntegrationState integrationState_;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager_;
    
    // Helper methods
    bool configureSTDP(float ltpWeight, float ltdWeight, float tau);
    bool configureStructuralPlasticity(float synaptogenesisRate, float pruningRate);
    void setupSpikeHandlers(Brain* brain);
    void setupCheckpointManager();
    void collectStatistics();
};

} // namespace nlm