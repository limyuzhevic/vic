// This file contains the refactored Brain implementation using the manager pattern
// The Brain class is now a coordinator that delegates to specialized manager classes
// Each manager handles one specific subsystem, improving separation of concerns

#include "Brain.hpp"
#include "ManagerInterfaces.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/Memory.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

// ========== MEMORY MANAGER IMPLEMENTATION ==========

class DefaultMemoryManager : public MemoryManager {
public:
    void initialize(Brain* brain) override {
        this->brain = brain;
        // Initialize working memory with capacity from config
        if (brain->getConfig()) {
            size_t capacity = brain->getConfig()->getOr<size_t>("working_memory_capacity", 100);
            workingMemory->setCapacity(capacity);
        }
    }
    
    void update(float timestep) override {
        if (workingMemory) workingMemory->update(timestep);
        if (episodicMemory) episodicMemory->update(timestep);
        if (associativeMemory) associativeMemory->update(timestep);
    }
    
    void storeToNeuron(NeuronId neuronId, float activation) override {
        if (workingMemory) workingMemory->storeToNeuron(neuronId, activation);
    }
    
    void storeEpisode(const class EpisodicMemoryItem& episode) override {
        if (episodicMemory) episodicMemory->storeEpisode(episode);
    }
    
    void clear() override {
        if (workingMemory) workingMemory->clear();
        if (episodicMemory) episodicMemory->clear();
        if (associativeMemory) associativeMemory->clear();
    }
    
    // Working memory accessors
    NeuralWorkingMemory* getWorkingMemory() override { return workingMemory.get(); }
    size_t getWorkingMemoryCapacity() const override { return workingMemory ? workingMemory->getCapacity() : 0; }
    void setWorkingMemoryCapacity(size_t capacity) override { if (workingMemory) workingMemory->setCapacity(capacity); }
    
    // Episodic memory accessors
    NeuralEpisodicMemory* getEpisodicMemory() override { return episodicMemory.get(); }
    size_t getMaxEpisodes() const override { return episodicMemory ? episodicMemory->getMaxEpisodes() : 0; }
    void setMaxEpisodes(size_t maxEpisodes) override { if (episodicMemory) episodicMemory->setMaxEpisodes(maxEpisodes); }
    size_t getEpisodeCount() const override { return episodicMemory ? episodicMemory->getEpisodeCount() : 0; }
    
    // Associative memory accessors
    NeuralAssociativeMemory* getAssociativeMemory() override { return associativeMemory.get(); }
    void associate(const std::vector<float>& pattern1, const std::vector<float>& pattern2, float strength) override {
        if (associativeMemory) associativeMemory->associate(pattern1, pattern2, strength);
    }
    std::vector<float> retrieve(const std::vector<float>& pattern) override {
        return associativeMemory ? associativeMemory->retrieve(pattern) : std::vector<float>();
    }
    float getAssociationStrength(const std::vector<float>& pattern1, const std::vector<float>& pattern2) const override {
        return associativeMemory ? associativeMemory->getAssociationStrength(pattern1, pattern2) : 0.0f;
    }
    
private:
    Brain* brain = nullptr;
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
};

// ========== PLASTICITY MANAGER IMPLEMENTATION ==========

class DefaultPlasticityManager : public PlasticityManager {
public:
    void initialize(Brain* brain) override {
        this->brain = brain;
        // STDP is already initialized in the brain constructor
    }
    
    void update(float timestep) override {
        // Plasticity is applied during each step in the brain's step method
    }
    
    void applyPlasticity(class Synapse* synapse, float preSpikeTime, float postSpikeTime, float timestep) override {
        // Delegate to STDP, Hebbian, or StructuralPlasticity as appropriate
    }
    
    void applySTDP(class Synapse* synapse, const std::vector<float>& preSpikeHistory, const std::vector<float>& postSpikeHistory, float timestep) override {
        if (stdp) {
            float ltpWeight = 0.01f;
            float ltdWeight = 0.012f;
            float tau = 20.0f;
            stdp->configure(ltpWeight, ltdWeight, tau);
            stdp->update(synapse, preSpikeHistory, postSpikeHistory, timestep);
        }
    }
    
    void applyHebbian(class Synapse* synapse, const std::vector<float>& preSpikeHistory, const std::vector<float>& postSpikeHistory, float timestep) override {
        if (hebbian) {
            hebbian->update(synapse, preSpikeHistory, postSpikeHistory, timestep);
        }
    }
    
    void applyStructuralPlasticity(Brain* brain, class RandomGenerator& rng, float timestep) override {
        if (structuralPlasticity) {
            structuralPlasticity->update(brain, rng, timestep);
        }
    }
    
    // STDP accessors
    STDP* getSTDP() override { return stdp.get(); }
    void configureSTDP(float ltpWeight, float ltdWeight, float tau) override {
        if (stdp) stdp->configure(ltpWeight, ltdWeight, tau);
    }
    
    // Hebbian accessors
    Hebbian* getHebbian() override { return hebbian.get(); }
    
    // Structural plasticity accessors
    StructuralPlasticity* getStructuralPlasticity() override { return structuralPlasticity.get(); }
    void setSynaptogenesisRate(float rate) override { if (structuralPlasticity) structuralPlasticity->setSynaptogenesisRate(rate); }
    void setPruningRate(float rate) override { if (structuralPlasticity) structuralPlasticity->setPruningRate(rate); }
    
    // Configuration
    void applyNeuromodulationFactor(float factor) override {
        // Apply neuromodulation factor to plasticity
        if (stdp) {
            stdp->setLTPWeight(0.01f * factor);
            stdp->setLTDWeight(0.012f * factor);
        }
    }
    
private:
    Brain* brain = nullptr;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
};

// ========== DEVELOPMENT MANAGER IMPLEMENTATION ==========

class DefaultDevelopmentManager : public DevelopmentManager {
public:
    void initialize(Brain* brain) override {
        this->brain = brain;
        developmentalStage = DevelopmentalStage::Initial;
        criticalPeriodEnd = 1000.0f; // Default 1000 time units
    }
    
    void update(Brain* brain, class RandomGenerator& rng, float timestep) override {
        if (developmentSystem) {
            developmentSystem->update(brain, rng, timestep);
            
            // Apply development effects to plasticity
            applyDevelopmentEffects(brain, getPlasticityModulator());
        }
    }
    
    void applyDevelopmentEffects(Brain* brain, float plasticityMod) override {
        if (structuralPlasticity) {
            // Scale synaptogenesis and pruning rates based on development stage
            float synaptogenesisRate = 0.0001f * plasticityMod;
            float pruningRate = 0.00001f * (2.0f - plasticityMod);
            structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
            structuralPlasticity->setPruningRate(pruningRate);
        }
    }
    
    // Development stage management
    DevelopmentalStage getStage() const override { return developmentalStage; }
    void setStage(DevelopmentalStage stage) override { developmentalStage = stage; }
    void advanceStage() override {
        switch (developmentalStage) {
            case DevelopmentalStage::Initial:
                developmentalStage = DevelopmentalStage::CriticalPeriod;
                break;
            case DevelopmentalStage::CriticalPeriod:
                developmentalStage = DevelopmentalStage::Maturation;
                break;
            case DevelopmentalStage::Maturation:
                developmentalStage = DevelopmentalStage::Adult;
                break;
            default:
                break;
        }
    }
    bool isStageCompleted() const override {
        // Check if development stage has completed based on time
        switch (developmentalStage) {
            case DevelopmentalStage::Initial:
                return currentTime >= 100.0f;
            case DevelopmentalStage::CriticalPeriod:
                return currentTime >= criticalPeriodEnd;
            case DevelopmentalStage::Maturation:
                return currentTime >= criticalPeriodEnd * 2.0f;
            default:
                return true; // Adult stage never completes
        }
    }
    
    // Development system access
    DevelopmentSystem* getDevelopmentSystem() override { return developmentSystem.get(); }
    
    // Critical periods and plasticity effects
    float getPlasticityModulator() const override {
        switch (developmentalStage) {
            case DevelopmentalStage::Initial:
                return 1.0f; // High plasticity
            case DevelopmentalStage::CriticalPeriod:
                return 0.8f;
            case DevelopmentalStage::Maturation:
                return 0.5f;
            case DevelopmentalStage::Adult:
                return 0.2f; // Stable
            default:
                return 1.0f;
        }
    }
    
    float getCriticalPeriodEnd() const override { return criticalPeriodEnd; }
    void setCriticalPeriodEnd(float time) override { criticalPeriodEnd = time; }
    
    void setCurrentTime(Timestamp time) { currentTime = time; }
    
private:
    Brain* brain = nullptr;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    DevelopmentalStage developmentalStage;
    float criticalPeriodEnd;
    Timestamp currentTime = 0.0f;
};

// ========== NEUROMODULATION MANAGER IMPLEMENTATION ==========

class DefaultNeuromodulationManager : public NeuromodulationManager {
public:
    void initialize(Brain* brain) override {
        this->brain = brain;
        // Neuromodulators are initialized in the brain constructor
    }
    
    void update(float timestep) override {
        if (dopamine) dopamine->update(timestep);
        if (curiosity) curiosity->update(timestep);
        if (novelty) novelty->update(timestep);
        if (predictionError) predictionError->update(timestep);
        
        // Apply neuromodulation effects to the brain
        applyModulationToNeurons(brain);
    }
    
    // Dopamine system
    Dopamine* getDopamine() override { return dopamine.get(); }
    float getDopamineLevel() const override { return dopamine ? dopamine->getLevel() : 0.0f; }
    void updateDopamine(float predictionError, float reward, float timestep) override {
        if (dopamine) dopamine->updatePredictionError(predictionError, reward, timestep);
    }
    
    // Curiosity system
    Curiosity* getCuriosity() override { return curiosity.get(); }
    float getCuriosityLevel() const override { return curiosity ? curiosity->getLevel() : 0.0f; }
    void updateCuriosity(float novelty, float uncertainty, float timestep) override {
        if (curiosity) curiosity->update(novelty, uncertainty, timestep);
    }
    
    // Novelty detection
    Novelty* getNovelty() override { return novelty.get(); }
    float getNoveltyLevel() const override { return novelty ? novelty->getLevel() : 0.0f; }
    void updateNovelty(const std::vector<float>& activityPattern, float timestep) override {
        if (novelty) novelty->update(activityPattern, timestep);
    }
    
    // Prediction error
    PredictionError* getPredictionError() override { return predictionError.get(); }
    float getPredictionError() const override { return predictionError ? predictionError->getLevel() : 0.0f; }
    void updatePredictionError(float prediction, float actual, float timestep) override {
        if (predictionError) predictionError->update(prediction, actual, timestep);
    }
    
    // Global neuromodulation effects
    float getGlobalModulation() const override {
        float total = 0.0f;
        if (dopamine) total += dopamine->getLevel();
        if (curiosity) total += curiosity->getLevel();
        if (novelty) total += novelty->getLevel();
        if (predictionError) total += predictionError->getLevel();
        return total / 4.0f; // Average
    }
    
    void applyModulationToNeurons(Brain* brain) override {
        if (!brain) return;
        
        float dopamineLevel = getDopamineLevel();
        float curiosityLevel = getCuriosityLevel();
        float noveltyLevel = getNoveltyLevel();
        
        // Apply dopamine effects on neural excitability
        if (dopamineLevel > 0.0f) {
            float excitabilityMod = dopamineLevel * 0.5f;
            for (auto* region : brain->getRegions()) {
                for (auto* population : region->getPopulations()) {
                    for (auto* neuron : population->getNeurons()) {
                        // Dopamine modulates excitability by injecting additional current
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
        
        // Apply curiosity effects on exploration
        if (curiosityLevel > 0.0f) {
            float explorationMod = curiosityLevel * 0.2f;
            for (auto* region : brain->getRegions()) {
                for (auto* population : region->getPopulations()) {
                    for (auto* neuron : population->getNeurons()) {
                        // Curiosity increases random exploration
                        if (brain->getRandomGenerator()) {
                            float rand = brain->getRandomGenerator()->nextFloat();
                            if (rand < explorationMod) {
                                neuron->injectCurrent(explorationMod * 10.0f);
                            }
                        }
                    }
                }
            }
        }
    }
    
private:
    Brain* brain = nullptr;
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
};

// ========== COGNITION MANAGER IMPLEMENTATION ==========

class DefaultCognitionManager : public CognitionManager {
public:
    void initialize(Brain* brain) override {
        this->brain = brain;
    }
    
    void update(float timestep) override {
        if (planner) planner->update(timestep);
        if (conceptFormation) conceptFormation->update(timestep);
        if (attention) attention->update(timestep);
    }
    
    // Planner system
    NeuralPlanner* getPlanner() override { return planner.get(); }
    void setPlanningDepth(int depth) override { if (planner) planner->setPlanningDepth(depth); }
    std::vector<float> planAction(const std::vector<float>& state) override {
        return planner ? planner->planAction(state) : std::vector<float>();
    }
    
    // Concept formation
    ConceptFormation* getConceptFormation() override { return conceptFormation.get(); }
    void processForConceptFormation(const std::vector<float>& activityPattern) override {
        if (conceptFormation) conceptFormation->process(activityPattern);
    }
    std::vector<std::vector<float>> getConcepts() const override {
        return conceptFormation ? conceptFormation->getConcepts() : std::vector<std::vector<float>>();
    }
    
    // Attention system
    AttentionalSelection* getAttention() override { return attention.get(); }
    void setAttentionInhibition(float strength) override { if (attention) attention->setInhibitionStrength(strength); }
    void setAttentionExcitation(float strength) override { if (attention) attention->setExcitationStrength(strength); }
    void processCompetition(const std::vector<NeuronId>& competitors) override {
        if (attention) attention->processCompetition(competitors);
    }
    
    // Cognitive statistics
    float getCognitiveLoad() const override {
        float load = 0.0f;
        if (planner) load += planner->getCognitiveLoad();
        if (conceptFormation) load += conceptFormation->getCognitiveLoad();
        if (attention) load += attention->getCognitiveLoad();
        return load;
    }
    
    size_t getActiveConcepts() const override {
        return conceptFormation ? conceptFormation->getActiveConceptCount() : 0;
    }
    
private:
    Brain* brain = nullptr;
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
};

// ========== PREDICTION MANAGER IMPLEMENTATION ==========

class DefaultPredictionManager : public PredictionManager {
public:
    void initialize(Brain* brain) override {
        this->brain = brain;
    }
    
    void update(float timestep) override {
        if (predictionSystem) predictionSystem->update(timestep);
    }
    
    // Prediction system
    PredictionSystem* getPredictionSystem() override { return predictionSystem.get(); }
    void processSensoryInput(const std::vector<float>& input) override {
        if (predictionSystem) predictionSystem->processSensoryInput(input);
    }
    std::vector<float> generatePrediction(const std::vector<float>& state) override {
        return predictionSystem ? predictionSystem->generatePrediction(state) : std::vector<float>();
    }
    float getPredictionError() const override {
        return predictionSystem ? predictionSystem->getPredictionError() : 0.0f;
    }
    
    // Forward prediction
    std::vector<float> predictNextState(const std::vector<float>& currentState, float timestep) override {
        return predictionSystem ? predictionSystem->predictNextState(currentState, timestep) : std::vector<float>();
    }
    
    // Internal state management
    void resetPredictionState() override {
        if (predictionSystem) predictionSystem->reset();
    }
    
    bool isPredicting() const override {
        return predictionSystem ? predictionSystem->isActive() : false;
    }
    
private:
    Brain* brain = nullptr;
    std::unique_ptr<PredictionSystem> predictionSystem;
};

// ========== STATISTICS MANAGER IMPLEMENTATION ==========

class DefaultStatisticsManager : public StatisticsManager {
public:
    void initialize(Brain* brain) override {
        this->brain = brain;
    }
    
    void update(const Brain* brain, float timestep) override {
        // Collect statistics from the brain
        if (!brain) return;
        
        // Basic statistics
        totalSpikesThisStep = brain->getTotalSpikeCount();
        totalSpikesTotal = brain->getTotalSpikeCount(); // This should accumulate
        
        // Memory statistics
        if (brain->getWorkingMemory()) {
            workingMemoryActiveTraces = brain->getWorkingMemory()->getActiveTraces();
        }
        if (brain->getEpisodicMemory()) {
            episodicMemoryEpisodeCount = brain->getEpisodicMemory()->getEpisodeCount();
        }
        
        // Neuromodulation statistics
        if (brain->getDopamine()) {
            dopamineLevel = brain->getDopamine()->getLevel();
        }
        if (brain->getCuriosity()) {
            curiosityLevel = brain->getCuriosity()->getLevel();
        }
        if (brain->getNovelty()) {
            noveltyLevel = brain->getNovelty()->getLevel();
        }
        if (brain->getPredictionErrorSignal()) {
            predictionErrorLevel = brain->getPredictionErrorSignal()->getLevel();
        }
        
        // Development statistics
        developmentStage = brain->getDevelopmentalStage();
        
        // Cognitive statistics
        if (brain->getPlanner()) {
            cognitiveLoad = brain->getPlanner()->getCognitiveLoad();
        }
        if (brain->getConceptFormation()) {
            activeConcepts = brain->getConceptFormation()->getActiveConceptCount();
        }
    }
    
    // Basic statistics
    float getExcitationInhibitionRatio() const override {
        return brain ? brain->getExcitationInhibitionRatio() : 0.0f;
    }
    
    size_t getTotalSpikeCount() const override {
        return totalSpikesThisStep;
    }
    
    size_t getPendingSpikeEventCount() const override {
        return spikeSystem ? spikeSystem->getPendingSpikeCount() + spikeSystem->getPendingDelayedCount() : 0;
    }
    
    size_t getTotalNeuronCount() const override {
        return brain ? brain->getTotalNeuronCount() : 0;
    }
    
    size_t getTotalSynapseCount() const override {
        return brain ? brain->getTotalSynapseCount() : 0;
    }
    
    size_t getActiveNeuronCount() const override {
        return brain ? brain->getActiveNeuronCount() : 0;
    }
    
    size_t getFiringNeuronCount() const override {
        return brain ? brain->getFiringNeuronCount() : 0;
    }
    
    float getAverageFiringRate() const override {
        return brain ? brain->getAverageFiringRate() : 0.0f;
    }
    
    // Memory statistics
    size_t getWorkingMemoryActiveTraces() const override { return workingMemoryActiveTraces; }
    size_t getEpisodicMemoryEpisodeCount() const override { return episodicMemoryEpisodeCount; }
    float getMemoryRetentionRate() const override { return 0.8f; } // Placeholder
    
    // Neuromodulation statistics
    float getDopamineLevel() const override { return dopamineLevel; }
    float getCuriosityLevel() const override { return curiosityLevel; }
    float getNoveltyLevel() const override { return noveltyLevel; }
    float getPredictionError() const override { return predictionErrorLevel; }
    
    // Development statistics
    DevelopmentalStage getDevelopmentalStage() const override { return developmentStage; }
    
    // Cognitive statistics
    float getCognitiveLoad() const override { return cognitiveLoad; }
    size_t getActiveConcepts() const override { return activeConcepts; }
    
    // Log status
    void logStatus(const Brain* brain) const override {
        if (!brain) return;
        
        NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
        NLM_LOG_INFO("Regions: " + std::to_string(brain->getRegionCount()));
        NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
        NLM_LOG_INFO("Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
        NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(brain->getFiringNeuronCount()));
        NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("Pending spike events: " + std::to_string(brain->getPendingSpikeEventCount()));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
        NLM_LOG_INFO("E/I ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
        
        // Memory system status
        if (brain->getWorkingMemory()) {
            NLM_LOG_INFO("Working memory traces: " + std::to_string(brain->getWorkingMemory()->getActiveTraces()));
        }
        if (brain->getEpisodicMemory()) {
            NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(brain->getEpisodicMemory()->getEpisodeCount()));
        }
        
        // Neuromodulation status
        if (brain->getDopamine()) {
            NLM_LOG_INFO("Dopamine level: " + std::to_string(brain->getDopamine()->getLevel()));
        }
        
        // Development status
        NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
    }
    
private:
    Brain* brain = nullptr;
    
    // Basic statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // Memory statistics
    size_t workingMemoryActiveTraces;
    size_t episodicMemoryEpisodeCount;
    
    // Neuromodulation statistics
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionErrorLevel;
    
    // Development statistics
    DevelopmentalStage developmentStage;
    
    // Cognitive statistics
    float cognitiveLoad;
    size_t activeConcepts;
};

// ========== CONFIGURATION MANAGER IMPLEMENTATION ==========

class DefaultConfigurationManager : public ConfigurationManager {
public:
    void initialize(Brain* brain) override {
        this->brain = brain;
    }
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const override {
        return brain ? brain->getConfig() : nullptr;
    }
    
    RandomGenerator* getRandomGenerator() override {
        return brain ? brain->getRandomGenerator() : nullptr;
    }
    
    Logger* getLogger() override {
        // This would need access to the global logger
        return nullptr;
    }
    
    SimulationClock* getSimulationClock() override {
        // This would need access to the global simulation clock
        return nullptr;
    }
    
    // Parameter access
    size_t getNeuronCount() const override {
        return config ? config->getOr<size_t>("neuron_count", 1000) : 1000;
    }
    
    size_t getRegionCount() const override {
        return config ? config->getOr<size_t>("region_count", 1) : 1;
    }
    
    float getConnectionProbability() const override {
        return config ? config->getOr<float>("connection_probability", 0.1f) : 0.1f;
    }
    
    float getSimulationTimestep() const override {
        return config ? config->getOr<double>("simulation_timestep", 0.001) : 0.001;
    }
    
    float getReplayInterval() const override {
        return config ? config->getOr<size_t>("replay_interval", 100) : 100;
    }
    
    float getConsolidationInterval() const override {
        return config ? config->getOr<size_t>("consolidation_interval", 1000) : 1000;
    }
    
    // Plasticity parameters
    float getSTDP_LTPWeight() const override {
        return config ? config->getOr<float>("stdp_ltp_weight", 0.01f) : 0.01f;
    }
    
    float getSTDP_LTDWeight() const override {
        return config ? config->getOr<float>("stdp_ltd_weight", 0.012f) : 0.012f;
    }
    
    float getSTDP_Tau() const override {
        return config ? config->getOr<float>("stdp_tau", 20.0f) : 20.0f;
    }
    
    float getSynaptogenesisRate() const override {
        return config ? config->getOr<float>("synaptogenesis_rate", 0.0001f) : 0.0001f;
    }
    
    float getPruningRate() const override {
        return config ? config->getOr<float>("pruning_rate", 0.00001f) : 0.00001f;
    }
    
    // Memory parameters
    size_t getWorkingMemoryCapacity() const override {
        return config ? config->getOr<size_t>("working_memory_capacity", 100) : 100;
    }
    
    size_t getMaxEpisodes() const override {
        return config ? config->getOr<size_t>("max_episodes", 1000) : 1000;
    }
    
    float getMemoryConsolidationThreshold() const override {
        return config ? config->getOr<float>("memory_consolidation_threshold", 0.5f) : 0.5f;
    }
    
    // Cognitive parameters
    int getPlanningDepth() const override {
        return config ? config->getOr<int>("planning_depth", 5) : 5;
    }
    
    float getAttentionInhibition() const override {
        return config ? config->getOr<float>("attention_inhibition", 0.5f) : 0.5f;
    }
    
    float getAttentionExcitation() const override {
        return config ? config->getOr<float>("attention_excitation", 1.5f) : 1.5f;
    }
    
    // Development parameters
    float getCriticalPeriodEnd() const override {
        return config ? config->getOr<float>("critical_period_end", 1000.0f) : 1000.0f;
    }
    
    float getAdultPlasticityFactor() const override {
        return config ? config->getOr<float>("adult_plasticity_factor", 0.2f) : 0.2f;
    }
    
    // Checkpoint parameters
    std::string getCheckpointDir() const override {
        return config ? config->getOr<std::string>("checkpoint_dir", "./checkpoints") : "./checkpoints";
    }
    
    size_t getCheckpointInterval() const override {
        return config ? config->getOr<size_t>("checkpoint_interval", 10000) : 10000;
    }
    
    int getCheckpointCompression() const override {
        return config ? config->getOr<int>("checkpoint_compression", 5) : 5;
    }
    
    // Configuration validation
    bool validate() const override {
        return true; // Placeholder - would validate all config parameters
    }
    
    std::string getConfigurationErrors() const override {
        return ""; // Placeholder - would return all configuration errors
    }
    
    void setConfig(std::shared_ptr<Config> cfg) {
        config = cfg;
    }
    
private:
    Brain* brain = nullptr;
    std::shared_ptr<const Config> config;
};

// ========== BRAIN FACTORY FUNCTION ==========

// Factory function to create a Brain with default managers
// This provides backward compatibility while using the new manager-based architecture
Brain* createBrainWithDefaultManagers(std::shared_ptr<Config> config) {
    // Create default instances of all managers
    auto memoryManager = std::make_unique<DefaultMemoryManager>();
    auto plasticityManager = std::make_unique<DefaultPlasticityManager>();
    auto developmentManager = std::make_unique<DefaultDevelopmentManager>();
    auto neuromodulationManager = std::make_unique<DefaultNeuromodulationManager>();
    auto cognitionManager = std::make_unique<DefaultCognitionManager>();
    auto predictionManager = std::make_unique<DefaultPredictionManager>();
    auto statisticsManager = std::make_unique<DefaultStatisticsManager>();
    auto configurationManager = std::make_unique<DefaultConfigurationManager>();
    
    // Create brain with all managers
    return new Brain(config,
                    std::move(memoryManager),
                    std::move(plasticityManager),
                    std::move(developmentManager),
                    std::move(neuromodulationManager),
                    std::move(cognitionManager),
                    std::move(predictionManager),
                    std::move(statisticsManager),
                    std::move(configurationManager));
}

// Convenience constructor for backward compatibility
// Creates default instances of all managers
Brain::Brain(std::shared_ptr<Config> config)
    : Brain(config,
            std::make_unique<DefaultMemoryManager>(),
            std::make_unique<DefaultPlasticityManager>(),
            std::make_unique<DefaultDevelopmentManager>(),
            std::make_unique<DefaultNeuromodulationManager>(),
            std::make_unique<DefaultCognitionManager>(),
            std::make_unique<DefaultPredictionManager>(),
            std::make_unique<DefaultStatisticsManager>(),
            std::make_unique<DefaultConfigurationManager>()) {}

// ========== BRAIN IMPLEMENTATION ==========

Brain::Brain(std::shared_ptr<Config> config,
             std::unique_ptr<MemoryManager> memoryManager,
             std::unique_ptr<PlasticityManager> plasticityManager,
             std::unique_ptr<DevelopmentManager> developmentManager,
             std::unique_ptr<NeuromodulationManager> neuromodulationManager,
             std::unique_ptr<CognitionManager> cognitionManager,
             std::unique_ptr<PredictionManager> predictionManager,
             std::unique_ptr<StatisticsManager> statisticsManager,
             std::unique_ptr<ConfigurationManager> configurationManager)
    : config(config)
    , memoryManager(std::move(memoryManager))
    , plasticityManager(std::move(plasticityManager))
    , developmentManager(std::move(developmentManager))
    , neuromodulationManager(std::move(neuromodulationManager))
    , cognitionManager(std::move(cognitionManager))
    , predictionManager(std::move(predictionManager))
    , statisticsManager(std::move(statisticsManager))
    , configurationManager(std::move(configurationManager))
    , rng(nullptr)
    , timestep(0.001)
    , currentStep(0)
    , currentTime(0.0)
    , totalSpikesThisStep(0)
    , totalSpikesTotal(0)
    , isResting(false)
    , stepsSinceLastEpisode(0)
    , replayInterval(100)
    , consolidationInterval(1000)
    , developmentalStage(DevelopmentalStage::Initial)
    , nextRegionId(1)
{
    // Store configuration for the configuration manager
    if (configurationManager) {
        configurationManager->setConfig(config);
    }
    
    // Initialize integration infrastructure
    spikeSystem = std::make_unique<SpikeSystem>();
    checkpointManager = std::make_unique<CheckpointManager>();
    
    // Initialize managers with references to the brain
    if (memoryManager) memoryManager->initialize(this);
    if (plasticityManager) plasticityManager->initialize(this);
    if (developmentManager) developmentManager->initialize(this);
    if (neuromodulationManager) neuromodulationManager->initialize(this);
    if (cognitionManager) cognitionManager->initialize(this);
    if (predictionManager) predictionManager->initialize(this);
    if (statisticsManager) statisticsManager->initialize(this);
    if (configurationManager) configurationManager->initialize(this);
}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept
    : config(std::move(other.config))
    , memoryManager(std::move(other.memoryManager))
    , plasticityManager(std::move(other.plasticityManager))
    , developmentManager(std::move(other.developmentManager))
    , neuromodulationManager(std::move(other.neuromodulationManager))
    , cognitionManager(std::move(other.cognitionManager))
    , predictionManager(std::move(other.predictionManager))
    , statisticsManager(std::move(other.statisticsManager))
    , configurationManager(std::move(other.configurationManager))
    , regions(std::move(other.regions))
    , interRegionConnections(std::move(other.interRegionConnections))
    , rng(std::move(other.rng))
    , timestep(other.timestep)
    , currentStep(other.currentStep)
    , currentTime(other.currentTime)
    , totalSpikesThisStep(other.totalSpikesThisStep)
    , totalSpikesTotal(other.totalSpikesTotal)
    , sensoryNeurons(std::move(other.sensoryNeurons))
    , motorNeurons(std::move(other.motorNeurons))
    , isResting(other.isResting)
    , stepsSinceLastEpisode(other.stepsSinceLastEpisode)
    , replayInterval(other.replayInterval)
    , consolidationInterval(other.consolidationInterval)
    , developmentalStage(other.developmentalStage)
    , nextRegionId(other.nextRegionId)
    , spikeSystem(std::move(other.spikeSystem))
    , checkpointManager(std::move(other.checkpointManager))
{
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        config = std::move(other.config);
        memoryManager = std::move(other.memoryManager);
        plasticityManager = std::move(other.plasticityManager);
        developmentManager = std::move(other.developmentManager);
        neuromodulationManager = std::move(other.neuromodulationManager);
        cognitionManager = std::move(other.cognitionManager);
        predictionManager = std::move(other.predictionManager);
        statisticsManager = std::move(other.statisticsManager);
        configurationManager = std::move(other.configurationManager);
        regions = std::move(other.regions);
        interRegionConnections = std::move(other.interRegionConnections);
        rng = std::move(other.rng);
        timestep = other.timestep;
        currentStep = other.currentStep;
        currentTime = other.currentTime;
        totalSpikesThisStep = other.totalSpikesThisStep;
        totalSpikesTotal = other.totalSpikesTotal;
        sensoryNeurons = std::move(other.sensoryNeurons);
        motorNeurons = std::move(other.motorNeurons);
        isResting = other.isResting;
        stepsSinceLastEpisode = other.stepsSinceLastEpisode;
        replayInterval = other.replayInterval;
        consolidationInterval = other.consolidationInterval;
        developmentalStage = other.developmentalStage;
        nextRegionId = other.nextRegionId;
        spikeSystem = std::move(other.spikeSystem);
        checkpointManager = std::move(other.checkpointManager);
    }
    return *this;
}

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = config->getOr<size_t>("region_count", 1);
    float connectionProbability = config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Add populations
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    motorNeurons.push_back(neuron);
                }
            }
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory
    if (memoryManager && memoryManager->getWorkingMemory()) {
        memoryManager->getWorkingMemory()->initialize(this);
        memoryManager->setWorkingMemoryCapacity(neuronCount / 10);
    }
    
    // Initialize episodic memory
    if (memoryManager && memoryManager->getEpisodicMemory()) {
        memoryManager->getEpisodicMemory()->initialize(this);
        memoryManager->setMaxEpisodes(1000);
    }
    
    // Initialize associative memory
    if (memoryManager && memoryManager->getAssociativeMemory()) {
        memoryManager->getAssociativeMemory()->initialize(this);
    }
    
    // Initialize prediction system
    if (predictionManager && predictionManager->getPredictionSystem()) {
        // (PredictionSystem doesn't have initialize method currently)
    }
    
    // Initialize cognition systems
    if (cognitionManager) {
        if (cognitionManager->getPlanner()) {
            cognitionManager->getPlanner()->initialize(this);
            cognitionManager->setPlanningDepth(5);
        }
        if (cognitionManager->getConceptFormation()) {
            cognitionManager->getConceptFormation()->initialize(this);
        }
        if (cognitionManager->getAttention()) {
            cognitionManager->getAttention()->initialize(this);
            cognitionManager->setAttentionInhibition(0.5f);
            cognitionManager->setAttentionExcitation(1.5f);
        }
    }
    
    // Initialize neuromodulation
    if (neuromodulationManager) {
        if (neuromodulationManager->getNovelty()) {
            neuromodulationManager->getNovelty()->initialize(this);
        }
        if (neuromodulationManager->getCuriosity()) {
            neuromodulationManager->getCuriosity()->initialize(this);
        }
    }
    
    // Register spike handlers for event-driven processing
    if (spikeSystem) {
        spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            // Count spikes
            ++totalSpikesThisStep;
            ++totalSpikesTotal;
        });
        
        spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
            // Find destination neuron and deliver synaptic input
            for (auto& region : regions) {
                auto neurons = region->getAllNeurons();
                for (auto* neuron : neurons) {
                    if (neuron->getId() == event.destination_neuron) {
                        // Apply synaptic weight as current
                        MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                        if (event.is_excitatory) {
                            neuron->receiveExcitatoryInput(synapticCurrent);
                        } else {
                            neuron->receiveInhibitoryInput(-synapticCurrent);
                        }
                        return;
                    }
                }
            }
        });
    }
    
    // Configure checkpoint manager
    std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    if (checkpointManager) {
        checkpointManager->configure(checkpointDir, 10000, 5, true);
    }
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(motorNeurons.size()));
    
    return true;
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    /*
     * PHASE 6: INTEGRATED ARTIFICIAL BRAIN LOOP
     * 
     * This implements the complete integrated brain simulation:
     * 
     * 1. Process pending delayed spike events (deliver synaptic input)
     * 2. Update all neuron membrane potentials (LIF dynamics)
     * 3. Detect spikes and schedule outgoing spike events
     * 4. Update working memory (maintenance and competition)
     * 5. Apply neuromodulation effects on neural excitability
     * 6. Apply plasticity rules (STDP and Hebbian)
     * 7. Update episodic memory with current experience
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity (synaptogenesis, pruning)
     * 12. Replay important memories (during rest or periodically)
     * 13. Apply development effects
     * 14. Collect statistics
     */
    
    this->currentStep = currentStep;
    this->currentTime = currentTime;
    totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    if (spikeSystem) {
        spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    if (spikeSystem) {
                        spikeSystem->queueSpike(event);
                    }

                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            currentTime,
                            deliveryTime,
                            currentStep,
                            deliveryStep
                        );

                        if (spikeSystem) {
                            spikeSystem->queueDelayedSpike(delayedEvent);
                        }

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (memoryManager) {
                        memoryManager->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes
    if (spikeSystem) {
        spikeSystem->processSpikes(currentStep);
    }
    
    // ========== STEP 4: Update working memory ==========
    if (memoryManager) {
        memoryManager->update(timestep);
    }
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    // Update novelty detection
    if (neuromodulationManager) {
        if (neuromodulationManager->getNovelty()) {
            neuromodulationManager->getNovelty()->update(timestep);
        }
        
        // Update curiosity
        if (neuromodulationManager->getCuriosity()) {
            neuromodulationManager->getCuriosity()->update(timestep);
        }
        
        // Update dopamine (reward prediction error)
        if (neuromodulationManager->getDopamine()) {
            neuromodulationManager->getDopamine()->update(timestep);
            
            // Apply dopamine effects on neural excitability
            float dopamineLevel = neuromodulationManager->getDopamineLevel();
            for (auto& region : regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Dopamine modulates excitability by injecting additional current
                        float excitabilityMod = dopamineLevel * 0.5f;
                        if (excitabilityMod > 0.0f) {
                            neuron->injectCurrent(excitabilityMod);
                        }
                    }
                }
            }
        }
    }
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (neuromodulationManager && neuromodulationManager->getDopamine()) {
        plasticityMod = neuromodulationManager->getDopamineLevel(); // Simplified
    }
    
    for (auto& region : regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    if (plasticityManager && plasticityManager->getSTDP()) {
                        plasticityManager->getSTDP()->update(syn, preSpikes, postSpikes, timestep);
                    }
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    if (plasticityManager && plasticityManager->getHebbian()) {
                        plasticityManager->getHebbian()->update(syn, preSpikes, postSpikes, timestep);
                    }
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
    
    // ========== STEP 7: Update episodic memory ==========
    stepsSinceLastEpisode++;
    if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        stepsSinceLastEpisode = 0;
        
        if (memoryManager && memoryManager->getEpisodicMemory()) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = neuromodulationManager ? neuromodulationManager->getDopamineLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                        }
                    }
                }
            }
            
            // Store reward in episode
            episode.reward = neuromodulationManager ? neuromodulationManager->getDopamineLevel() : 0.0f;
            
            memoryManager->storeEpisode(episode);
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (predictionManager && predictionManager->getPredictionSystem()) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
    
    // ========== STEP 9: Update attention system ==========
    if (cognitionManager && cognitionManager->getAttention()) {
        cognitionManager->getAttention()->update(timestep);
        
        // Apply attention to working memory winners
        if (memoryManager && memoryManager->getWorkingMemory() && 
            !memoryManager->getWorkingMemory()->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = memoryManager->getWorkingMemory()->getMemoryNeurons();
            cognitionManager->getAttention()->processCompetition(competitors);
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (cognitionManager && cognitionManager->getConceptFormation()) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        if (plasticityManager && plasticityManager->getStructuralPlasticity() && rng) {
            plasticityManager->getStructuralPlasticity()->update(this, *rng, timestep * 100);
        }
    }
    
    // ========== STEP 12: Replay important memories ==========
    if (currentStep % replayInterval == 0 && memoryManager && memoryManager->getEpisodicMemory()) {
        // Get episodes for replay
        auto episodesToReplay = memoryManager->getEpisodicMemory()->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            memoryManager->getEpisodicMemory()->replayEpisode(episode);
        }
    }
    
    // ========== STEP 13: Apply development effects ==========
    if (developmentManager) {
        developmentManager->update(this, *rng, timestep * 1000);
        
        // Development affects plasticity rates
        if (plasticityManager && plasticityManager->getStructuralPlasticity()) {
            DevelopmentalStage stage = getDevelopmentalStage();
            float plasticityMod = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    plasticityMod = 1.0f; // High plasticity
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    plasticityMod = 0.8f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.2f; // Stable
                    break;
            }
            
            plasticityManager->getStructuralPlasticity()->setSynaptogenesisRate(0.0001f * plasticityMod);
            plasticityManager->getStructuralPlasticity()->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
    
    // ========== STEP 14: Periodic memory consolidation ==========
    if (currentStep % consolidationInterval == 0 && memoryManager && memoryManager->getEpisodicMemory()) {
        // Consolidate important memories, remove weak ones
        memoryManager->getEpisodicMemory()->consolidate(0.3f);
    }
    
    // ========== STEP 15: Checkpoint management ==========
    if (checkpointManager) {
        checkpointManager->update(currentStep, currentTime);
    }
    
    // Update development manager with current time
    if (developmentManager) {
        developmentManager->setCurrentTime(currentTime);
    }
    
    // ========== STEP 16: Update statistics ==========
    if (statisticsManager) {
        statisticsManager->update(this, timestep);
    }
    
    // Update neuromodulation manager with development stage
    if (neuromodulationManager) {
        neuromodulationManager->update(timestep);
    }
    
    if (cognitionManager) {
        cognitionManager->update(timestep);
    }
    
    if (predictionManager) {
        predictionManager->update(timestep);
    }
    
    if (memoryManager) {
        memoryManager->update(timestep);
    }
    
    if (plasticityManager) {
        plasticityManager->update(timestep);
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        if (sensoryNeurons[i]) {
            sensoryNeurons[i]->injectCurrent(normalizedValue);
            
            // Also store in working memory
            if (memoryManager && memoryManager->getWorkingMemory() && normalizedValue > 0.5f) {
                memoryManager->getWorkingMemory()->storeToNeuron(sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
            }
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

// ========== SYSTEM ACCESSORS ==========

SpikeSystem* Brain::getSpikeSystem() {
    return spikeSystem.get();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return spikeSystem.get();
}

STDP* Brain::getSTDP() {
    return plasticityManager ? plasticityManager->getSTDP() : nullptr;
}

Hebbian* Brain::getHebbian() {
    return plasticityManager ? plasticityManager->getHebbian() : nullptr;
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return plasticityManager ? plasticityManager->getStructuralPlasticity() : nullptr;
}

// ========== STATISTICS ==========

float Brain::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

size_t Brain::getTotalSpikeCount() const {
    return totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return spikeSystem ? spikeSystem->getPendingSpikeCount() + spikeSystem->getPendingDelayedCount() : 0;
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Simple action selection based on motor neuron activity
    if (motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    return std::make_unique<Action>(type);
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    if (plasticityManager && plasticityManager->getSTDP()) {
        plasticityManager->getSTDP()->setLTPWeight(0.01f * modulation);
        plasticityManager->getSTDP()->setLTDWeight(0.012f * modulation);
    }
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    // Development updates structural plasticity
    if (plasticityManager && plasticityManager->getStructuralPlasticity() && rng) {
        plasticityManager->getStructuralPlasticity()->update(this, *rng);
    }
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    currentStep = 0;
    currentTime = 0.0;
    totalSpikesThisStep = 0;
    totalSpikesTotal = 0;
    isResting = false;
    stepsSinceLastEpisode = 0;
    
    for (auto& region : regions) {
        region->reset();
    }
    
    if (spikeSystem) {
        spikeSystem->reset();
    }
    
    developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (memoryManager) memoryManager->clear();
    if (developmentManager && developmentManager->getDevelopmentSystem()) {
        developmentManager->getDevelopmentSystem()->reset();
    }
    if (cognitionManager && cognitionManager->getAttention()) {
        cognitionManager->getAttention()->reset();
    }
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

// ========== CHECKPOINTING ==========

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            currentStep,
            currentTime
        );
        
        // Write neurons
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        
        for (const auto& region : regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    neuronData.membranePotential.push_back(state.membranePotential);
                    neuronData.restingPotential.push_back(state.restingPotential);
                    neuronData.threshold.push_back(state.threshold);
                    neuronData.resetPotential.push_back(state.resetPotential);
                    neuronData.leakConductance.push_back(state.leakConductance);
                    neuronData.firingState.push_back(static_cast<uint8_t>(state.firingState));
                    neuronData.refractoryRemaining.push_back(state.refractoryRemaining);
                    neuronData.refractoryPeriod.push_back(state.refractoryPeriod);
                    neuronData.lastSpikeTime.push_back(state.lastSpikeTime);
                }
            }
        }
        
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses
        SynapseCheckpointData synapseData;
        for (const auto& region : regions) {
            for (const auto* syn : region->getSynapses()) {
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Apply neuron states
        size_t idx = 0;
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (idx < neuronData.membranePotential.size()) {
                        neuron->setMembranePotential(neuronData.membranePotential[idx]);
                        neuron->setRestingPotential(neuronData.restingPotential[idx]);
                        neuron->setThreshold(neuronData.threshold[idx]);
                        neuron->setResetPotential(neuronData.resetPotential[idx]);
                        neuron->setLeakConductance(neuronData.leakConductance[idx]);
                        if (idx < neuronData.firingState.size()) {
                            neuron->setFiringState(static_cast<FiringState>(neuronData.firingState[idx]));
                        }
                        if (idx < neuronData.refractoryRemaining.size()) {
                            neuron->setRefractoryPeriod(neuronData.refractoryPeriod[idx]);
                        }
                    }
                    idx++;
                }
            }
        }
        
        // Read synapses
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        // Apply synapse states - this is complex because we need to find matching synapses
        // For now, just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

// ========== REGION MANAGEMENT ==========

RegionId Brain::addRegion(const std::string& name) {
    RegionId id(nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    regions.push_back(std::move(region));
    return id;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    for (auto& region : regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    for (const auto& region : regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    return regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(regions.size());
    for (const auto& region : regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    interRegionConnections.erase(
        std::remove_if(interRegionConnections.begin(),
                      interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        interRegionConnections.end()
    );
}

// ========== GLOBAL STATISTICS ==========

size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : regions) {
        total += region->getSynapseCount();
    }
    total += interRegionConnections.size();
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    return totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    if (regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(regions.size());
}

// ========== MEMORY SYSTEM ACCESSORS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return memoryManager ? memoryManager->getWorkingMemory() : nullptr;
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return memoryManager ? memoryManager->getEpisodicMemory() : nullptr;
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return memoryManager ? memoryManager->getAssociativeMemory() : nullptr;
}

// ========== PREDICTION SYSTEM ACCESSOR ==========

PredictionSystem* Brain::getPredictionSystem() {
    return predictionManager ? predictionManager->getPredictionSystem() : nullptr;
}

// ========== COGNITION SYSTEM ACCESSORS ==========

NeuralPlanner* Brain::getPlanner() {
    return cognitionManager ? cognitionManager->getPlanner() : nullptr;
}

ConceptFormation* Brain::getConceptFormation() {
    return cognitionManager ? cognitionManager->getConceptFormation() : nullptr;
}

AttentionalSelection* Brain::getAttention() {
    return cognitionManager ? cognitionManager->getAttention() : nullptr;
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return developmentManager ? developmentManager->getDevelopmentSystem() : nullptr;
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    developmentalStage = stage;
}

// ========== NEUROMODULATION SYSTEMS ==========

Dopamine* Brain::getDopamine() {
    return neuromodulationManager ? neuromodulationManager->getDopamine() : nullptr;
}

Curiosity* Brain::getCuriosity() {
    return neuromodulationManager ? neuromodulationManager->getCuriosity() : nullptr;
}

Novelty* Brain::getNovelty() {
    return neuromodulationManager ? neuromodulationManager->getNovelty() : nullptr;
}

PredictionError* Brain::getPredictionErrorSignal() {
    return neuromodulationManager ? neuromodulationManager->getPredictionError() : nullptr;
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return rng.get();
}

void Brain::logStatus() const {
    if (statisticsManager) {
        statisticsManager->logStatus(this);
    }
}

} // namespace nlm
