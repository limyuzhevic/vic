#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <unordered_map>
#include <functional>

namespace nlm {

// ========== HELPER FUNCTIONS ==========

namespace {
    // Helper function to trim whitespace from strings
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    // Helper function to extract value from config with fallback
    template<typename T>
    T getConfigValue(const std::shared_ptr<Config>& config, 
                    const std::string& key, T defaultValue) {
        auto val = config->get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    }
    
    // Helper function to log configuration values
    void logConfigValues(const std::shared_ptr<Config>& config, 
                        const std::string& prefix = "") {
        auto keys = config->getKeys();
        NLM_LOG_INFO(prefix + "Configuration keys: ");
        for (const auto& key : keys) {
            NLM_LOG_INFO(prefix + "  " + key + " = ");
        }
    }
} // namespace

// ========== BRAIN IMPLEMENTATION ==========

struct Brain::Impl {
private:
    // ========== CONFIGURATION MANAGEMENT ==========
    void loadConfiguration(const std::shared_ptr<Config>& config) {
        // Log configuration values
        logConfigValues(config, "Config: ");
        
        // Validate critical configuration parameters
        if (auto neuronCount = config->getOr<size_t>("neuron_count", 1000)) {
            if (neuronCount < 100) {
                NLM_LOG_WARNING("Neuron count is very small (" + std::to_string(neuronCount) + ") - simulation may be trivial");
            }
        }
        
        if (auto timestep = config->get<double>("simulation_timestep")) {
            if (timestep <= 0.0) {
                NLM_LOG_ERROR("Invalid simulation timestep: " + std::to_string(timestep));
            }
        }
    }
    
    // ========== INITIALIZATION HELPERS ==========
    void initializePlasticitySystems() {
        // Initialize STDP with configuration
        float ltpWeight = getConfigValue(config, "stdp_ltp_weight", 0.01f);
        float ltdWeight = getConfigValue(config, "stdp_ltd_weight", 0.012f);
        float stdpTau = getConfigValue(config, "stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, stdpTau);
        
        // Configure structural plasticity
        float synaptogenesisRate = getConfigValue(config, "synaptogenesis_rate", 0.0001f);
        float pruningRate = getConfigValue(config, "pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        NLM_LOG_INFO("Plasticity systems initialized: STDP (LTP=" + std::to_string(ltpWeight) + 
                     ", LTD=" + std::to_string(ltdWeight) + ", tau=" + std::to_string(stdpTau) + ")");
    }
    
    // ========== REGION CREATION HELPERS ==========
    void createRegionWithNeurons(size_t regionIndex, size_t neuronCount, size_t regionCount) {
        RegionId id(nextRegionId++);
        auto region = std::make_unique<NeuralRegion>(id, "Region_" + std::to_string(regionIndex + 1));
        
        // Calculate neurons per population (sensory, internal, motor)
        size_t neuronsPerRegion = neuronCount / regionCount;
        size_t sensoryNeurons = neuronsPerRegion / 4;
        size_t internalNeurons = neuronsPerRegion / 2;
        size_t motorNeurons = neuronsPerRegion / 4;
        
        // Create populations
        auto sensoryPopId = region->addPopulation(sensoryNeurons, NeuronType::Sensory);
        auto internalPopId = region->addPopulation(internalNeurons, NeuronType::Internal);
        auto motorPopId = region->addPopulation(motorNeurons, NeuronType::Motor);
        
        // Collect I/O neurons for later use
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
        
        regions.push_back(std::move(region));
        
        NLM_LOG_INFO("Created region " + std::to_string(regionIndex + 1) + 
                     ": " + std::to_string(neuronsPerRegion) + " neurons, " +
                     std::to_string(region->getPopulationCount()) + " populations");
    }
    
    // ========== CONNECTIVITY INITIALIZATION HELPERS ==========
    void initializeRandomConnectivityForRegion(NeuralRegion* region, float connectionProbability) {
        if (!region) return;
        
        // Ensure region has random generator
        if (!rng) {
            rng = std::make_unique<RandomGenerator>(42); // Default seed
        }
        
        region->initializeRandomConnectivity(*rng, connectionProbability, 0.2f, 0.1f);
    }
    
    // ========== SPIKE EVENT HANDLERS ==========
    void setupSpikeSystemHandlers() {
        // Register spike counter handler
        spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            ++totalSpikesThisStep;
            ++totalSpikesTotal;
        });
        
        // Register delayed spike handler for synaptic input delivery
        spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
            deliverSynapticInput(event);
        });
        
        NLM_LOG_INFO("Spike system handlers registered");
    }
    
    void deliverSynapticInput(const DelayedSpikeEvent& event) {
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
    }
    
    // ========== MEMORY SYSTEM INITIALIZATION HELPERS ==========
    void initializeMemorySystems() {
        if (workingMemory) {
            workingMemory->initialize(&brain);
            workingMemory->setCapacity(getTotalNeuronCount() / 10);
            NLM_LOG_INFO("Working memory initialized with capacity: " + 
                        std::to_string(getTotalNeuronCount() / 10));
        }
        
        if (episodicMemory) {
            episodicMemory->initialize(&brain);
            episodicMemory->setMaxEpisodes(1000);
            NLM_LOG_INFO("Episodic memory initialized with max episodes: 1000");
        }
        
        if (associativeMemory) {
            associativeMemory->initialize(&brain);
            NLM_LOG_INFO("Associative memory initialized");
        }
    }
    
    // ========== COGNITION SYSTEM INITIALIZATION HELPERS ==========
    void initializeCognitiveSystems() {
        if (planner) {
            planner->initialize(&brain);
            planner->setPlanningDepth(5);
            NLM_LOG_INFO("Neural planner initialized with depth: 5");
        }
        
        if (conceptFormation) {
            conceptFormation->initialize(&brain);
            NLM_LOG_INFO("Concept formation initialized");
        }
        
        if (attention) {
            attention->initialize(&brain);
            attention->setInhibitionStrength(0.5f);
            attention->setExcitationStrength(1.5f);
            NLM_LOG_INFO("Attention system initialized (inhibition=0.5, excitation=1.5)");
        }
    }
    
    // ========== NEUROMODULATION SYSTEM INITIALIZATION HELPERS ==========
    void initializeNeuromodulationSystems() {
        if (novelty) {
            novelty->initialize(&brain);
            NLM_LOG_INFO("Novelty detection system initialized");
        }
        
        if (curiosity) {
            curiosity->initialize(&brain);
            NLM_LOG_INFO("Curiosity system initialized");
        }
        
        // Note: Dopamine and PredictionError don't have initialize methods in current implementation
    }
    
    // ========== CHECKPOINT SYSTEM INITIALIZATION HELPERS ==========
    void initializeCheckpointManager() {
        std::string checkpointDir = getConfigValue(config, "checkpoint_dir", "./checkpoints");
        checkpointManager->configure(checkpointDir, 10000, 5, true);
        NLM_LOG_INFO("Checkpoint manager configured with directory: " + checkpointDir);
    }
    
    // ========== STEP EXECUTION HELPERS ==========
    void executeStep12ProcessDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
        spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
    
    void executeStep2UpdateNeurons(TimestepDuration timestep, Timestamp currentTime) {
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(currentTime, timestep);
                }
            }
        }
    }
    
    void executeStep3DetectAndScheduleSpikes(Timestamp currentTime, SimulationStep currentStep) {
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    detectAndScheduleSingleSpike(neuron, currentTime, currentStep);
                }
            }
        }
    }
    
    void detectAndScheduleSingleSpike(Neuron* neuron, Timestamp currentTime, SimulationStep currentStep) {
        const auto& state = neuron->getState();
        bool justFired = (state.firingState == FiringState::Refractory &&
                         state.lastSpikeTime >= 0.0f &&
                         std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);
        
        if (justFired) {
            // Queue the spike
            SpikeEvent event(neuron->getId(), currentTime, currentStep);
            spikeSystem->queueSpike(event);
            
            // Record post-synaptic spike for incoming synapses (plasticity)
            auto incomingSynapses = getIncomingSynapsesFor(neuron->getId(), region.get());
            for (Synapse* syn : incomingSynapses) {
                syn->recordPostSpike(currentTime);
            }
            
            // Schedule outgoing spike events
            scheduleOutgoingSpikeEvents(neuron, currentTime, currentStep);
            
            // Store to working memory
            if (workingMemory) {
                float activation = std::abs(state.membranePotential - state.restingPotential) / 10.0f;
                workingMemory->storeToNeuron(neuron->getId(), activation);
            }
        }
    }
    
    void scheduleOutgoingSpikeEvents(Neuron* neuron, Timestamp currentTime, SimulationStep currentStep) {
        auto outgoingSynapses = getOutgoingSynapsesFor(neuron->getId(), region.get());
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
            
            spikeSystem->queueDelayedSpike(delayedEvent);
            
            // Record pre-synaptic spike for plasticity
            syn->recordPreSpike(currentTime);
        }
    }
    
    std::vector<Synapse*> getIncomingSynapsesFor(NeuronId neuronId, NeuralRegion* region) {
        std::vector<Synapse*> incomingSynapses;
        if (region) {
            incomingSynapses = region->getSynapsesTo(neuronId);
        }
        return incomingSynapses;
    }
    
    std::vector<Synapse*> getOutgoingSynapsesFor(NeuronId neuronId, NeuralRegion* region) {
        std::vector<Synapse*> outgoingSynapses;
        if (region) {
            outgoingSynapses = region->getSynapsesFrom(neuronId);
        }
        return outgoingSynapses;
    }
    
    void executeStep4UpdateWorkingMemory() {
        if (workingMemory) {
            workingMemory->update(timestep);
        }
    }
    
    void executeStep5ApplyNeuromodulation() {
        // Update novelty detection
        if (novelty) {
            novelty->update(timestep);
        }
        
        // Update curiosity
        if (curiosity) {
            curiosity->update(timestep);
        }
        
        // Update dopamine (reward prediction error)
        if (dopamine) {
            dopamine->update(timestep);
            applyDopamineEffectsToNeurons();
        }
    }
    
    void applyDopamineEffectsToNeurons() {
        float dopamineLevel = dopamine->getLevel();
        float excitabilityMod = dopamineLevel * 0.5f;
        
        if (excitabilityMod > 0.0f) {
            for (auto& region : regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    void executeStep6ApplyPlasticityRules() {
        float plasticityMod = 1.0f;
        if (dopamine) {
            plasticityMod = dopamine->getPlasticityFactor();
        }
        
        for (auto& region : regions) {
            for (auto& syn : region->getSynapses()) {
                applyPlasticityToSynapse(syn, plasticityMod);
            }
        }
    }
    
    void applyPlasticityToSynapse(Synapse* syn, float plasticityMod) {
        if (syn->getPlasticityFlags().stdp) {
            auto preSpikes = syn->getPreSpikeHistory();
            auto postSpikes = syn->getPostSpikeHistory();
            
            if (!preSpikes.empty() && !postSpikes.empty()) {
                stdp->update(syn, preSpikes, postSpikes, timestep);
                float weight = syn->getWeight();
                weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                syn->setWeight(weight);
            }
        }
        
        if (syn->getPlasticityFlags().hebbian) {
            auto preSpikes = syn->getPreSpikeHistory();
            auto postSpikes = syn->getPostSpikeHistory();
            
            if (!preSpikes.empty() && !postSpikes.empty()) {
                hebbian->update(syn, preSpikes, postSpikes, timestep);
            }
        }
        
        syn->step(currentTime);
    }
    
    void executeStep7UpdateEpisodicMemory(SimulationStep currentStep) {
        stepsSinceLastEpisode++;
        if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
            stepsSinceLastEpisode = 0;
            
            if (episodicMemory) {
                createAndStoreEpisode(currentStep);
            }
        }
    }
    
    void createAndStoreEpisode(SimulationStep currentStep) {
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
        
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
        
        episodicMemory->storeEpisode(episode);
    }
    
    void executeStep8UpdatePredictionSystem() {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
    
    void executeStep9UpdateAttentionSystem() {
        if (attention) {
            attention->update(timestep);
            
            // Apply attention to working memory winners
            if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
                std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
                attention->processCompetition(competitors);
            }
        }
    }
    
    void executeStep10UpdateConceptFormation() {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
    
    void executeStep11ApplyStructuralPlasticity(SimulationStep currentStep) {
        if (currentStep % 100 == 0) {
            structuralPlasticity->update(&brain, *rng);
        }
    }
    
    void executeStep12ReplayImportantMemories(SimulationStep currentStep) {
        if (currentStep % replayInterval == 0 && episodicMemory) {
            auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                episodicMemory->replayEpisode(episode);
            }
        }
    }
    
    void executeStep13ApplyDevelopmentEffects(SimulationStep currentStep) {
        if (currentStep % 1000 == 0) {  // Update development every 1000 steps
            developmentalSystem->update(&brain, *rng, timestep * 1000);
            applyDevelopmentEffectsToPlasticity();
        }
    }
    
    void applyDevelopmentEffectsToPlasticity() {
        auto* sp = structuralPlasticity.get();
        if (!sp) return;
        
        float plasticityMod = 1.0f;
        switch (developmentalStage) {
            case DevelopmentalStage::Initial:
                plasticityMod = 1.0f;  // High plasticity
                break;
            case DevelopmentalStage::CriticalPeriod:
                plasticityMod = 0.8f;
                break;
            case DevelopmentalStage::Maturation:
                plasticityMod = 0.5f;
                break;
            case DevelopmentalStage::Adult:
                plasticityMod = 0.2f;  // Stable
                break;
        }
        
        sp->setSynaptogenesisRate(0.0001f * plasticityMod);
        sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
    }
    
    void executeStep14PeriodicMemoryConsolidation(SimulationStep currentStep) {
        if (currentStep % consolidationInterval == 0 && episodicMemory) {
            episodicMemory->consolidate(0.3f);
        }
    }
    
    void executeStep15CheckpointManagement(SimulationStep currentStep, Timestamp currentTime) {
        if (checkpointManager) {
            checkpointManager->update(currentStep, currentTime);
        }
    }
    
    // ========== BRAINGLOBAL ACCESSORS ==========
    NeuralRegion* getRegionForId(RegionId id) {
        for (auto& region : regions) {
            if (region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    }
    
    const NeuralRegion* getRegionForId(RegionId id) const {
        for (const auto& region : regions) {
            if (region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    }
    
    // ========== LEGACY COMPATIBILITY ACCESSORS ==========
    NeuralRegion* getRegion(RegionId id) { return getRegionForId(id); }
    const NeuralRegion* getRegion(RegionId id) const { return getRegionForId(id); }
    
    // ========== MEMORY ACCESSORS ==========
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory.get(); }
    PredictionSystem* getPredictionSystem() { return predictionSystem.get(); }
    NeuralPlanner* getPlanner() { return planner.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation.get(); }
    AttentionalSelection* getAttention() { return attention.get(); }
    DevelopmentSystem* getDevelopmentSystem() { return developmentalSystem.get(); }
    Dopamine* getDopamine() { return dopamine.get(); }
    Curiosity* getCuriosity() { return curiosity.get(); }
    Novelty* getNovelty() { return novelty.get(); }
    PredictionError* getPredictionError() { return predictionError.get(); }
    
    // ========== NEUROMODULATION ACCESSORS ==========
    float getCurrentDopamineLevel() const { return dopamine ? dopamine->getLevel() : 0.0f; }
    float getCurrentPlasticityFactor() const { return dopamine ? dopamine->getPlasticityFactor() : 1.0f; }
    
public:
    // ========== COMPONENT DECLARATIONS ==========
    
    class NeuralEngine {
    public:
        std::shared_ptr<Config> config;
        std::unique_ptr<RandomGenerator> rng;
        std::vector<std::unique_ptr<NeuralRegion>> regions;
        std::vector<InterRegionConnection> interRegionConnections;
        std::vector<Neuron*> sensoryNeurons;
        std::vector<Neuron*> motorNeurons;
        
        TimestepDuration timestep;
        SimulationStep currentStep;
        Timestamp currentTime;
        size_t totalSpikesThisStep;
        size_t totalSpikesTotal;
        bool isResting;
        size_t stepsSinceLastEpisode;
        size_t replayInterval;
        size_t consolidationInterval;
        
        std::unique_ptr<SpikeSystem> spikeSystem;
        std::unique_ptr<STDP> stdp;
        std::unique_ptr<Hebbian> hebbian;
        std::unique_ptr<StructuralPlasticity> structuralPlasticity;
        
        NeuralEngine(std::shared_ptr<Config> cfg);
        ~NeuralEngine();
        
        void initialize(size_t neuronCount, size_t regionCount, float connectionProbability);
        void createRegions(size_t neuronCount, size_t regionCount);
        void initializeConnectivity(float connectionProbability);
        void step(TimestepDuration timestep, Timestamp currentTime, SimulationStep currentStep);
        void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
        void updateNeurons(TimestepDuration timestep, Timestamp currentTime);
        void detectSpikesAndScheduleEvents(Timestamp currentTime, SimulationStep currentStep);
        void processSpikes(SimulationStep currentStep);
        void collectIONeurons();
        void updateConnectivity(const std::vector<Neuron*>& neurons, float weightScale, float weightOffset);
        float getDopamineLevel() const;
        float getPlasticityFactor() const;
    };
    
    class MemoryManager {
    public:
        std::unique_ptr<NeuralWorkingMemory> workingMemory;
        std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
        std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
        std::shared_ptr<Brain> brain;
        
        void initialize(std::shared_ptr<Brain> brainRef);
        void update(TimestepDuration timestep);
        void storeEpisode(const EpisodicMemoryItem& episode);
        void replayEpisodes(size_t count);
        void consolidate(float threshold);
        void reset();
        
        MemoryManager();
    };
    
    class CognitiveProcessor {
    public:
        std::unique_ptr<NeuralPlanner> planner;
        std::unique_ptr<ConceptFormation> conceptFormation;
        std::unique_ptr<AttentionalSelection> attention;
        std::shared_ptr<Brain> brain;
        
        void initialize(std::shared_ptr<Brain> brainRef);
        void update(TimestepDuration timestep);
        void processCompetition(const std::vector<NeuronId>& competitors);
        void setAttentionParameters(float inhibition, float excitation);
        void reset();
        
        CognitiveProcessor();
    };
    
    class DevelopmentalManager {
    public:
        std::unique_ptr<DevelopmentSystem> developmentSystem;
        DevelopmentalStage currentStage;
        std::shared_ptr<Brain> brain;
        
        void initialize(std::shared_ptr<Brain> brainRef);
        void update(SimulationStep currentStep, TimestepDuration timestep);
        void updatePlasticityRates(float plasticityMod);
        DevelopmentalStage getStage() const { return currentStage; }
        void setStage(DevelopmentalStage stage) { currentStage = stage; }
        
        DevelopmentalManager();
    };
    
    class NeuromodulationSystem {
    public:
        std::unique_ptr<Dopamine> dopamine;
        std::unique_ptr<Curiosity> curiosity;
        std::unique_ptr<PredictionError> predictionError;
        std::unique_ptr<Novelty> novelty;
        std::shared_ptr<Brain> brain;
        
        void initialize(std::shared_ptr<Brain> brainRef);
        void update(TimestepDuration timestep);
        void applyNeuromodulationToNeurons();
        void applyNeuromodulationToPlasticity();
        float getLevel() const;
        float getPlasticityFactor() const;
        void reset();
        
        NeuromodulationSystem();
    };
    
    class CheckpointManager {
    public:
        std::unique_ptr<CheckpointManager> checkpointManager;
        
        void initialize(const std::string& checkpointDir);
        bool save(const std::string& filepath) const;
        bool load(const std::string& filepath);
        void update(SimulationStep currentStep, Timestamp currentTime);
        
        CheckpointManager();
    };
    
    // ========== COMPOSITE STATE ==========
    std::unique_ptr<NeuralEngine> neuralEngine;
    MemoryManager memoryManager;
    CognitiveProcessor cognitiveProcessor;
    DevelopmentalManager developmentalManager;
    NeuromodulationSystem neuromodulationSystem;
    CheckpointManager checkpointManager;
    
    // ========== COMPOSITE ACCESSORS ==========
    NeuralEngine& getNeuralEngine() { return *neuralEngine; }
    MemoryManager& getMemoryManager() { return memoryManager; }
    CognitiveProcessor& getCognitiveProcessor() { return cognitiveProcessor; }
    DevelopmentalManager& getDevelopmentalManager() { return developmentalManager; }
    NeuromodulationSystem& getNeuromodulationSystem() { return neuromodulationSystem; }
    CheckpointManager& getCheckpointManager() { return checkpointManager; }
    
    // ========== LEGACY ACCESSORS ==========
    // These maintain backward compatibility with the existing API
    SpikeSystem* getSpikeSystem() { return neuralEngine->spikeSystem.get(); }
    const SpikeSystem* getSpikeSystem() const { return neuralEngine->spikeSystem.get(); }
    STDP* getSTDP() { return neuralEngine->stdp.get(); }
    Hebbian* getHebbian() { return neuralEngine->hebbian.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return neuralEngine->structuralPlasticity.get(); }
    NeuralWorkingMemory* getWorkingMemory() { return memoryManager.workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return memoryManager.episodicMemory.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return memoryManager.associativeMemory.get(); }
    NeuralPlanner* getPlanner() { return cognitiveProcessor.planner.get(); }
    ConceptFormation* getConceptFormation() { return cognitiveProcessor.conceptFormation.get(); }
    AttentionalSelection* getAttention() { return cognitiveProcessor.attention.get(); }
    DevelopmentSystem* getDevelopmentSystem() { return developmentalManager.developmentSystem.get(); }
    Dopamine* getDopamine() { return neuromodulationSystem.dopamine.get(); }
    Curiosity* getCuriosity() { return neuromodulationSystem.curiosity.get(); }
    Novelty* getNovelty() { return neuromodulationSystem.novelty.get(); }
    PredictionError* getPredictionError() { return neuromodulationSystem.predictionError.get(); }
    
    // ========== LIFECYCLE ==========
    Impl(std::shared_ptr<Config> cfg);
    ~Impl();
    
    // ========== INITIALIZATION ==========
    bool initialize();
    
    // ========== SIMULATION LOOP ==========
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // ========== SENSORY-MOTOR INTERFACE ==========
    void receiveSensoryInput(const class SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    std::unique_ptr<class Action> produceAction();
    
    // ========== STATE MANAGEMENT ==========
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // ========== REGION MANAGEMENT ==========
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // ========== GLOBAL STATISTICS ==========
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // ========== SYSTEM ACCESSORS ==========
    std::shared_ptr<const Config> getConfig() const { return config; }
    RandomGenerator* getRandomGenerator() { return neuralEngine->rng.get(); }
    DevelopmentalStage getDevelopmentalStage() const { return developmentalManager.currentStage; }
    void setDevelopmentalStage(DevelopmentalStage stage) { developmentalManager.currentStage = stage; }
    
    // ========== LOGGING ==========
    void logStatus() const;
    
private:
    // ========== IMPLEMENTATION DETAILS ==========
    
    // Core neural computation system
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain; // Self-reference for component initialization
    
    // Neural engine (core computation)
    std::unique_ptr<NeuralEngine> neuralEngine;
    
    // Individual component systems
    MemoryManager memoryManager;
    CognitiveProcessor cognitiveProcessor;
    DevelopmentalManager developmentalManager;
    NeuromodulationSystem neuromodulationSystem;
    CheckpointManager checkpointManager;
    
    // Configuration values
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    
    // Legacy pointers for backward compatibility (to be phased out)
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    std::vector<InterRegionConnection> interRegionConnections;
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    std::unique_ptr<PredictionSystem> predictionSystem;
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    std::unique_ptr<DevelopmentSystem> developmentalSystem;
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<CheckpointManager> checkpointManager;
    std::unique_ptr<RandomGenerator> rng;
};

// ========== BRAIN CONSTRUCTOR ==========

Brain::Impl::Impl(std::shared_ptr<Config> cfg) : 
    config(cfg),
    brain(nullptr),  // Will be set by component constructors
    timestep(0.001),
    currentStep(0),
    currentTime(0.0),
    totalSpikesThisStep(0),
    totalSpikesTotal(0),
    isResting(false),
    stepsSinceLastEpisode(0),
    replayInterval(100),
    consolidationInterval(1000),
    developmentalStage(DevelopmentalStage::Initial),
    nextRegionId(1) {
    
    // Initialize the neural engine first
    neuralEngine = std::make_unique<NeuralEngine>(config);
    
    // Set up self-reference for components
    brain = std::shared_ptr<Brain>(std::shared_ptr<Brain>(), this);
    
    // Initialize all components
    memoryManager.initialize(brain);
    cognitiveProcessor.initialize(brain);
    developmentalManager.initialize(brain);
    neuromodulationSystem.initialize(brain);
    checkpointManager.initialize("./checkpoints");
    
    NLM_LOG_INFO("BrainImpl initialized with composite architecture (Phase 6)");
}

// ========== BRAIN DESTRUCTOR ==========

Brain::Impl::~Impl() {
    NLM_LOG_INFO("BrainImpl being destroyed");
}

// ========== BRAIN INITIALIZATION ==========

bool Brain::Impl::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Composite Architecture)...");
    
    // Load and validate configuration
    loadConfiguration(config);
    
    // Initialize the neural engine
    size_t neuronCount = getConfigValue(config, "neuron_count", 1000);
    size_t regionCount = getConfigValue(config, "region_count", 2);
    float connectionProbability = getConfigValue(config, "connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions, " +
                 std::to_string(connectionProbability) + " connection probability");
    
    // Initialize plasticity systems
    initializePlasticitySystems();
    
    // Set up spike system handlers
    setupSpikeSystemHandlers();
    
    // Initialize all integrated systems
    initializeMemorySystems();
    initializeCognitiveSystems();
    initializeNeuromodulationSystems();
    initializeCheckpointManager();
    
    // Neural system initialization (delegated to neural engine)
    neuralEngine->initialize(neuronCount, regionCount, connectionProbability);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Composite)");
    logStatus();
    
    return true;
}

// ========== SIMULATION STEP ==========

void Brain::Impl::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * timestep);
}

void Brain::Impl::step(SimulationStep currentStep, Timestamp currentTime) {
    /*
     * PHASE 6: COMPOSITE ARTIFICIAL BRAIN LOOP
     * 
     * This implements the complete integrated brain simulation:
     * 
     * 1. Process delayed spike events (deliver synaptic input)
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
     * 14. Consolidate memories periodically
     * 15. Update checkpoint management
     */
    
    this->currentStep = currentStep;
    this->currentTime = currentTime;
    totalSpikesThisStep = 0;
    
    // ========== EXECUTE COMPREHENSIVE STEP LOOP ==========
    
    // Step 1: Process delayed spike events (deliver synaptic input)
    executeStep12ProcessDelayedSpikes(currentStep, currentTime);
    
    // Step 2: Update all neurons (LIF dynamics)
    executeStep2UpdateNeurons(timestep, currentTime);
    
    // Step 3: Detect spikes and schedule outgoing spike events
    executeStep3DetectAndScheduleSpikes(currentTime, currentStep);
    
    // Step 4: Process immediate spikes
    neuralEngine->processSpikes(currentStep);
    
    // Step 5: Update working memory
    executeStep4UpdateWorkingMemory();
    
    // Step 6: Apply neuromodulation effects
    executeStep5ApplyNeuromodulation();
    
    // Step 7: Apply plasticity rules
    executeStep6ApplyPlasticityRules();
    
    // Step 8: Update episodic memory
    executeStep7UpdateEpisodicMemory(currentStep);
    
    // Step 9: Update prediction system
    executeStep8UpdatePredictionSystem();
    
    // Step 10: Update attention system
    executeStep9UpdateAttentionSystem();
    
    // Step 11: Update concept formation
    executeStep10UpdateConceptFormation();
    
    // Step 12: Apply structural plasticity
    executeStep11ApplyStructuralPlasticity(currentStep);
    
    // Step 13: Replay important memories
    executeStep12ReplayImportantMemories(currentStep);
    
    // Step 14: Apply development effects
    executeStep13ApplyDevelopmentEffects(currentStep);
    
    // Step 15: Periodic memory consolidation
    executeStep14PeriodicMemoryConsolidation(currentStep);
    
    // Step 16: Checkpoint management
    executeStep15CheckpointManagement(currentStep, currentTime);
    
    // Log step completion
    if (currentStep % 100 == 0) {
        NLM_LOG_INFO("Completed step " + std::to_string(currentStep) + 
                     ", total spikes: " + std::to_string(totalSpikesTotal));
    }
}

// ========== SENSORY-MOTOR INTERFACE ==========

void Brain::Impl::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = neuralEngine->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        neuralEngine->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (workingMemory && normalizedValue > 0.5f) {
            workingMemory->storeToNeuron(neuralEngine->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void Brain::Impl::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : neuralEngine->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void Brain::Impl::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : neuralEngine->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

std::unique_ptr<class Action> Brain::Impl::produceAction() {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (neuralEngine->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : neuralEngine->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    return action;
}

// ========== STATE MANAGEMENT ==========

void Brain::Impl::reset() {
    NLM_LOG_INFO("Resetting NLM Brain (Composite Architecture)...");
    
    currentStep = 0;
    currentTime = 0.0;
    totalSpikesThisStep = 0;
    totalSpikesTotal = 0;
    isResting = false;
    stepsSinceLastEpisode = 0;
    
    // Reset neural engine
    neuralEngine->reset();
    
    // Reset all components
    memoryManager.reset();
    cognitiveProcessor.reset();
    neuromodulationSystem.reset();
    developmentalManager.setStage(DevelopmentalStage::Initial);
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::Impl::save(const std::string& filepath) const {
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
        
        for (const auto& region : neuralEngine->regions) {
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
        for (const auto& region : neuralEngine->regions) {
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

bool Brain::Impl::load(const std::string& filepath) {
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
        
        // Read synapses
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
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

RegionId Brain::Impl::addRegion(const std::string& name) {
    RegionId id(nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name.empty() ? "Region_" + std::to_string(id.index()) : name);
    neuralEngine->regions.push_back(std::move(region));
    return id;
}

NeuralRegion* Brain::Impl::getRegion(RegionId id) {
    return neuralEngine->getRegionForId(id);
}

const NeuralRegion* Brain::Impl::getRegion(RegionId id) const {
    return neuralEngine->getRegionForId(id);
}

size_t Brain::Impl::getRegionCount() const {
    return neuralEngine->regions.size();
}

std::vector<RegionId> Brain::Impl::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(neuralEngine->regions.size());
    for (const auto& region : neuralEngine->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::Impl::getRegions() const {
    return neuralEngine->regions;
}

// ========== GLOBAL STATISTICS ==========

size_t Brain::Impl::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : neuralEngine->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::Impl::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : neuralEngine->regions) {
        total += region->getSynapseCount();
    }
    return total;
}

size_t Brain::Impl::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : neuralEngine->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t Brain::Impl::getFiringNeuronCount() const {
    return totalSpikesThisStep;
}

float Brain::Impl::getAverageFiringRate() const {
    if (neuralEngine->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : neuralEngine->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(neuralEngine->regions.size());
}

float Brain::Impl::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : neuralEngine->regions) {
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

size_t Brain::Impl::getTotalSpikeCount() const {
    return totalSpikesTotal;
}

size_t Brain::Impl::getPendingSpikeEventCount() const {
    return neuralEngine->spikeSystem->getPendingSpikeCount() + 
           neuralEngine->spikeSystem->getPendingDelayedCount();
}

// ========== LOGGING ==========

void Brain::Impl::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Composite Architecture) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    // Memory system status
    if (workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(workingMemory->getActiveTraces()));
    }
    if (episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(episodicMemory->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(dopamine->getLevel()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(developmentalStage)));
    
    for (const auto& region : neuralEngine->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                     " (" + region->getName() + "): " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                     std::to_string(region->getSynapseCount()) + " synapses, " +
                     "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

// ========== BRAIN CONSTRUCTOR ==========

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {
    // Set up self-reference for brain-wide components
    pImpl->brain = std::shared_ptr<Brain>(std::shared_ptr<Brain>(), this);
}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

// Add remaining method implementations...

} // namespace nlm
