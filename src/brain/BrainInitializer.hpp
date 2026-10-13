#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

class NeuralRegion;
class Neuron;
class Synapse;

class BrainInitializer {
public:
    explicit BrainInitializer(std::shared_ptr<Config> config);
    ~BrainInitializer() = default;
    
    // Initialize plasticity systems
    void initializePlasticitySystems();
    
    // Initialize memory systems
    void initializeMemorySystems();
    
    // Initialize cognition systems
    void initializeCognitionSystems();
    
    // Initialize neuromodulation systems
    void initializeNeuromodulationSystems();
    
    // Initialize development system
    void initializeDevelopmentSystem();
    
    // Initialize all spike system components
    void initializeSpikeSystem();
    
    // Initialize checkpoint manager
    void initializeCheckpointManager();
    
    // Configure region connectivity and neurons
    void configureRegionsAndNeurons(size_t neuronCount, size_t regionCount, 
                                   float connectionProbability);
    
    // Setup spike handlers for event-driven processing
    void setupSpikeHandlers(class Brain* brain);
    
    // Get initialized components
    std::vector<std::unique_ptr<NeuralRegion>>& getRegions() { return regions; }
    std::vector<InterRegionConnection>& getInterRegionConnections() { return interRegionConnections; }
    std::unique_ptr<SpikeSystem>& getSpikeSystem() { return spikeSystem; }
    std::unique_ptr<STDP>& getSTDP() { return stddp; }
    std::unique_ptr<Hebbian>& getHebbian() { return hebbian; }
    std::unique_ptr<StructuralPlasticity>& getStructuralPlasticity() { return structuralPlasticity; }
    std::unique_ptr<NeuralWorkingMemory>& getWorkingMemory() { return workingMemory; }
    std::unique_ptr<NeuralEpisodicMemory>& getEpisodicMemory() { return episodicMemory; }
    std::unique_ptr<NeuralAssociativeMemory>& getAssociativeMemory() { return associativeMemory; }
    std::unique_ptr<PredictionSystem>& getPredictionSystem() { return predictionSystem; }
    std::unique_ptr<NeuralPlanner>& getPlanner() { return planner; }
    std::unique_ptr<ConceptFormation>& getConceptFormation() { return conceptFormation; }
    std::unique_ptr<AttentionalSelection>& getAttention() { return attention; }
    std::unique_ptr<DevelopmentSystem>& getDevelopmentSystem() { return developmentSystem; }
    std::unique_ptr<Dopamine>& getDopamine() { return dopamine; }
    std::unique_ptr<Curiosity>& getCuriosity() { return curiosity; }
    std::unique_ptr<PredictionError>& getPredictionError() { return predictionError; }
    std::unique_ptr<Novelty>& getNovelty() { return novelty; }
    std::unique_ptr<CheckpointManager>& getCheckpointManager() { return checkpointManager; }
    std::vector<Neuron*>& getSensoryNeurons() { return sensoryNeurons; }
    std::vector<Neuron*>& getMotorNeurons() { return motorNeurons; }
    
    // Configuration access
    std::shared_ptr<Config> getConfig() const { return config; }
    TimestepDuration getTimestep() const { return timestep; }
    SimulationStep getCurrentStep() const { return currentStep; }
    Timestamp getCurrentTime() const { return currentTime; }
    size_t getTotalSpikesThisStep() const { return totalSpikesThisStep; }
    size_t getTotalSpikesTotal() const { return totalSpikesTotal; }
    bool isResting() const { return isResting; }
    size_t getStepsSinceLastEpisode() const { return stepsSinceLastEpisode; }
    size_t getReplayInterval() const { return replayInterval; }
    size_t getConsolidationInterval() const { return consolidationInterval; }
    
    // Setters for step-related state
    void setCurrentStep(SimulationStep step) { currentStep = step; }
    void setCurrentTime(Timestamp time) { currentTime = time; }
    void setTotalSpikesThisStep(size_t spikes) { totalSpikesThisStep = spikes; }
    void incrementTotalSpikesThisStep() { totalSpikesThisStep++; }
    void incrementTotalSpikesTotal() { totalSpikesTotal++; }
    void setIsResting(bool resting) { isResting = resting; }
    void incrementStepsSinceLastEpisode() { stepsSinceLastEpisode++; }
    void resetStepsSinceLastEpisode() { stepsSinceLastEpisode = 0; }
    
    // Get statistics
    size_t getNeuronCount() const { return neuronCount; }
    size_t getRegionCount() const { return regionCount; }
    float getConnectionProbability() const { return connectionProbability; }

private:
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // Memory systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // Prediction system
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // Cognition systems
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // Development system
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // Neuromodulation systems
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Plasticity systems
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
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // I/O neurons
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Helper methods for initialization
    void initializePlasticityConfig();
    void initializeMemorySystemConfigs();
    void initializeCognitionSystemConfigs();
    void initializeNeuromodulationSystemConfigs();
    void initializeDevelopmentSystemConfig();
    void createRegionConnections();
    void collectSensoryMotorNeurons();
    void configureSpikeSystem();
    DevelopmentStage developmentalStage;
    RegionId nextRegionId;
};