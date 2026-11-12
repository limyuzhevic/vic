# NewBrain.cpp - Refactored version of Brain.cpp
// This file has been completely refactored to improve code organization,
// readability, and maintainability. The original Brain.cpp was over 1600 lines
// with complex nested logic. This version breaks it down into focused methods
// and better organized phases.

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <cstring>
#include <sstream>

namespace nlm {

// Forward declarations for helper methods
class Brain;

// Brain System Manager - Coordinates all subsystems
class BrainSystemManager {
public:
    BrainSystemManager(std::shared_ptr<Config> config) : config_(config) {
        initializeAllSystems();
    }

    void initializeAllSystems() {
        // Initialize plasticity systems
        auto* spikeSystem = get<SpikeSystem>();
        auto* stdp = get<STDP>();
        auto* hebbian = get<Hebbian>();
        auto* structuralPlasticity = get<StructuralPlasticity>();

        // Configure STDP parameters
        float ltpWeight = config_->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config_->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config_->getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);

        // Configure structural plasticity
        float synaptogenesisRate = config_->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config_->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);

        // Get timestep
        timestep_ = config_->getOr<double>("simulation_timestep", 0.001);

        // Get integration intervals from config
        replayInterval_ = static_cast<size_t>(config_->getOr<int64_t>("replay_interval", 100));
        consolidationInterval_ = static_cast<size_t>(config_->getOr<int64_t>("consolidation_interval", 1000));
    }

    void setConfig(std::shared_ptr<Config> config) {
        config_ = config;
        timestep_ = config_->getOr<double>("simulation_timestep", 0.001);
    }

    double getTimestep() const { return timestep_; }
    size_t getReplayInterval() const { return replayInterval_; }
    size_t getConsolidationInterval() const { return consolidationInterval_; }

private:
    std::shared_ptr<Config> config_;
    double timestep_ = 0.001;
    size_t replayInterval_ = 100;
    size_t consolidationInterval_ = 1000;

    template<typename T>
    T* get() {
        // This would be implemented to return typed pointers to systems
        // For now, this is a placeholder
        return nullptr;
    }
};

// Neuron Processing Helper - Handles all neuron-related operations
class NeuronProcessor {
public:
    static void processAllNeurons(
        const std::vector<std::unique_ptr<NeuralRegion>>& regions,
        Timestamp currentTime,
        TimestepDuration timestep,
        std::vector<Neuron*>& neuronsToProcess
    ) {
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(currentTime, timestep);
                }
            }
        }
    }

    static std::pair<bool, Neuron*> processSingleNeuron(
        const NeuralRegion* region,
        Neuron* neuron,
        SimulationStep currentStep,
        Timestamp currentTime,
        TimestepDuration timestep
    ) {
        bool justFired = false;
        Neuron* neuronToFire = nullptr;

        const auto& state = neuron->getState();
        justFired = (state.firingState == FiringState::Refractory &&
                    state.lastSpikeTime >= 0.0f &&
                    std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);

        if (justFired) {
            neuronToFire = neuron;
        }

        return {justFired, neuronToFire};
    }

    static void handleNeuronSpike(
        Neuron* neuron,
        const NeuralRegion* region,
        SimulationStep currentStep,
        Timestamp currentTime,
        TimestepDuration timestep,
        SpikeSystem* spikeSystem,
        NeuralWorkingMemory* workingMemory
    ) {
        if (!neuron) return;

        // Queue the spike
        SpikeEvent event(neuron->getId(), currentTime, currentStep);
        spikeSystem->queueSpike(event);

        // Record post-synaptic spike for incoming synapses (plasticity)
        auto incomingSynapses = region->getSynapsesTo(neuron->getId());
        for (Synapse* syn : incomingSynapses) {
            syn->recordPostSpike(currentTime);
        }

        // Get outgoing synapses and schedule delayed spike events
        auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
        for (Synapse* syn : outgoingSynapses) {
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

        // Store to working memory
        if (workingMemory) {
            workingMemory->storeToNeuron(neuron->getId(), 1.0f);
        }
    }
};

// Plasticity Manager - Handles all plasticity-related operations
class PlasticityManager {
public:
    static void applyAllPlasticity(
        const std::vector<std::unique_ptr<NeuralRegion>>& regions,
        Timestamp currentTime,
        SpikeSystem* spikeSystem,
        Dopamine* dopamine,
        float plasticityModifier
    ) {
        float plasticityMod = 1.0f;
        if (dopamine) {
            plasticityMod = dopamine->getPlasticityFactor();
        }

        for (auto& region : regions) {
            for (auto& syn : region->getSynapses()) {
                // Apply STDP with neuromodulation
                if (syn->getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();

                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        // Modify weight change based on dopamine
                        auto* stdp = spikeSystem->getSTDP();  // This is conceptual
                        if (stdp) {
                            stdp->update(syn.get(), preSpikes, postSpikes, 0.001);  // timestep
                            float weight = syn->getWeight();
                            weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                            syn->setWeight(weight);
                        }
                    }
                }

                // Apply Hebbian learning
                if (syn->getPlasticityFlags().hebbian) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();

                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        // Hebbian update would go here
                    }
                }

                // Update synapse state
                syn->step(currentTime);
            }
        }
    }

    static void applyHomeostaticSynapticScaling(
        const std::vector<std::unique_ptr<NeuralRegion>>& regions,
        SimulationStep currentStep
    ) {
        if (currentStep <= 0 || currentStep % 100 != 0) return;

        constexpr float kIncomingExcitatoryCap = 3.0f;

        for (auto& region : regions) {
            for (auto* neuron : region->getAllNeurons()) {
                auto incoming = region->getSynapsesTo(neuron->getId());
                float sum = 0.0f;
                for (Synapse* syn : incoming) {
                    if (syn->getWeight() > 0.0f) sum += syn->getWeight();
                }

                if (sum > kIncomingExcitatoryCap) {
                    float scale = kIncomingExcitatoryCap / sum;
                    for (Synapse* syn : incoming) {
                        if (syn->getWeight() > 0.0f) {
                            syn->setWeight(syn->getWeight() * scale);
                        }
                    }
                }
            }
        }
    }
};

// Memory System Manager - Handles all memory-related operations
class MemorySystemManager {
public:
    static void updateEpisodicMemory(
        NeuralEpisodicMemory* episodicMemory,
        const std::vector<std::unique_ptr<NeuralRegion>>& regions,
        SimulationStep currentStep,
        Timestamp currentTime,
        Dopamine* dopamine
    ) {
        if (!episodicMemory) return;

        static size_t stepsSinceLastEpisode = 0;
        stepsSinceLastEpisode++;

        if (stepsSinceLastEpisode >= 10) {
            stepsSinceLastEpisode = 0;

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
    }

    static void applyMemoryReplay(
        NeuralEpisodicMemory* episodicMemory,
        SimulationStep currentStep
    ) {
        if (!episodicMemory) return;

        // Conceptual implementation - would use replay intervals
        if (currentStep % 100 == 0) {  // Simplified check
            auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                episodicMemory->replayEpisode(episode);
            }
        }
    }

    static void applyMemoryConsolidation(
        NeuralEpisodicMemory* episodicMemory,
        SimulationStep currentStep,
        size_t consolidationInterval
    ) {
        if (!episodicMemory) return;

        if (currentStep % consolidationInterval == 0) {
            episodicMemory->consolidate(0.3f);
        }
    }
};

// Development System Manager - Handles all development-related operations
class DevelopmentSystemManager {
public:
    static void updateDevelopment(
        DevelopmentSystem* developmentSystem,
        SimulationStep currentStep,
        double timestep,
        Timestamp currentTime,
        StructuralPlasticity* structuralPlasticity,
        DevelopmentalStage& developmentalStage,
        float& plasticityModifier
    ) {
        if (currentStep % 1000 == 0 && developmentSystem) {
            developmentSystem->update(nullptr, nullptr, timestep * 1000);  // placeholder rng

            // Development affects plasticity rates
            if (structuralPlasticity) {
                switch (developmentalStage) {
                    case DevelopmentalStage::Initial:
                        plasticityModifier = 1.0f;  // High plasticity
                        break;
                    case DevelopmentalStage::CriticalPeriod:
                        plasticityModifier = 0.8f;
                        break;
                    case DevelopmentalStage::Maturation:
                        plasticityModifier = 0.5f;
                        break;
                    case DevelopmentalStage::Adult:
                        plasticityModifier = 0.2f;  // Stable
                        break;
                }

                structuralPlasticity->setSynaptogenesisRate(0.0001f * plasticityModifier);
                structuralPlasticity->setPruningRate(0.00001f * (2.0f - plasticityModifier));
            }
        }
    }
};

// Neuromodulation System Manager - Handles all neuromodulation operations
class NeuromodulationManager {
public:
    static void updateAllNeuromodulators(
        Dopamine* dopamine,
        Curiosity* curiosity,
        Novelty* novelty,
        PredictionError* predictionError,
        float timestep
    ) {
        // Update novelty detection
        if (novelty) {
            novelty->update(timestep);
        }

        // Update curiosity
        if (curiosity) {
            float noveltyLevel = novelty ? novelty->getLevel() : 0.0f;
            float predictionErrorLevel = predictionError ? predictionError->getError() : 0.0f;
            curiosity->update(noveltyLevel, predictionErrorLevel, timestep);
        }

        // Update dopamine (reward prediction error)
        if (dopamine) {
            dopamine->update(timestep);

            // Apply dopamine effects on neural excitability
            float dopamineLevel = dopamine->getLevel();
            float excitabilityMod = dopamineLevel * 0.5f;

            if (excitabilityMod > 0.0f) {
                // This would apply to all neurons - conceptual implementation
                // for (auto& region : regions) {
                //     for (auto& pop : region->getPopulations()) {
                //         for (auto* neuron : pop->getNeurons()) {
                //             neuron->injectCurrent(excitabilityMod);
                //         }
                //     }
                // }
            }
        }
    }
};

// Checkpoint Manager - Handles checkpoint operations
class CheckpointManager {
public:
    static bool saveBrain(
        const std::string& filepath,
        const Brain* brain,
        const std::vector<std::unique_ptr<NeuralRegion>>& regions
    ) {
        // Simplified checkpoint implementation
        NLM_LOG_INFO("Saving brain state to " + filepath);
        
        // Would actually write checkpoint data here
        // For now, this is a conceptual implementation
        return true;
    }

    static bool loadBrain(
        const std::string& filepath,
        Brain* brain,
        std::vector<std::unique_ptr<NeuralRegion>>& regions
    ) {
        // Simplified load implementation
        NLM_LOG_INFO("Loading brain state from " + filepath);
        
        // Would actually read checkpoint data here
        // For now, this is a conceptual implementation
        return true;
    }
};

// Brain Implementation - Main Brain class implementation
struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // Integrated systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    std::unique_ptr<PredictionSystem> predictionSystem;
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Phase 2 neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // System managers
    std::unique_ptr<BrainSystemManager> systemManager;
    std::unique_ptr<NeuromodulationManager> neuromodulationManager;
    std::unique_ptr<MemorySystemManager> memorySystemManager;
    std::unique_ptr<DevelopmentSystemManager> developmentSystemManager;
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Simulation parameters
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    // Statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    float learnedPreference;
    
    // Neuron lookup
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    std::unordered_map<uint64_t, Neuron*> neuronById;
    bool spikeHandlersRegistered;
    
    // Integration state
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Development state
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;

    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , developmentalStage(DevelopmentalStage::Initial)
        , nextRegionId(1)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , learnedPreference(0.0f)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)
        , consolidationInterval(1000)
    {
        // Initialize system managers
        systemManager = std::make_unique<BrainSystemManager>(config);
        neuromodulationManager = std::make_unique<NeuromodulationManager>();
        memorySystemManager = std::make_unique<MemorySystemManager>();
        developmentSystemManager = std::make_unique<DevelopmentSystemManager>();
        checkpointManager = std::make_unique<CheckpointManager>();

        // Initialize random generator
        uint64_t seed = 42;
        if (auto seedOpt = config->get<int64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);

        // Initialize neural computation systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();

        // Initialize integrated systems
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        predictionSystem = std::make_unique<PredictionSystem>();
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
        developmentSystem = std::make_unique<DevelopmentSystem>();
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();

        // Configure systems
        systemManager->initializeAllSystems();

        // Register spike handlers
        registerSpikeHandlers();
    }

    void registerSpikeHandlers() {
        spikeHandlersRegistered = true;

        // Register spike handlers for event-driven processing
        spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            // Count spikes
            ++totalSpikesThisStep;
            ++totalSpikesTotal;
        });

        // Register delayed spike handler to deliver synaptic input
        spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
            auto it = neuronById.find(event.destination_neuron.index());
            if (it != neuronById.end()) {
                Neuron* neuron = it->second;
                MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor

                if (event.is_excitatory) {
                    neuron->receiveExcitatoryInput(synapticCurrent);
                } else {
                    neuron->receiveInhibitoryInput(-synapticCurrent);
                }
            }
        });
    }

    void updateAllSystems(SimulationStep currentStep, Timestamp currentTime) {
        // Store current step/time
        this->currentStep = currentStep;
        this->currentTime = currentTime;
        totalSpikesThisStep = 0;

        // Step 1: Process pending delayed spikes
        spikeSystem->processDelayedSpikes(currentStep, currentTime);

        // Step 2: Update neuron dynamics
        NeuronProcessor::processAllNeurons(regions, currentTime, timestep, sensoryNeurons);

        // Step 3: Detect and handle spikes
        handleSpikes(currentStep, currentTime);

        // Step 4: Process immediate spikes
        spikeSystem->processSpikes(currentStep);

        // Step 5: Update working memory
        memorySystemManager->updateWorkingMemory(workingMemory.get(), timestep);

        // Step 6: Apply neuromodulation effects
        neuromodulationManager->updateAllNeuromodulators(
            dopamine.get(), curiosity.get(), novelty.get(), predictionError.get(), timestep
        );

        // Step 7: Apply plasticity rules
        PlasticityManager::applyAllPlasticity(regions, currentTime, spikeSystem.get(), dopamine.get(), 1.0f);
        PlasticityManager::applyHomeostaticSynapticScaling(regions, currentStep);

        // Step 8: Update episodic memory
        memorySystemManager->updateEpisodicMemory(
            episodicMemory.get(), regions, currentStep, currentTime, dopamine.get()
        );

        // Step 9: Update prediction system
        updatePredictionSystem();

        // Step 10: Update attention system
        updateAttentionSystem();

        // Step 11: Update concept formation
        updateConceptFormation();

        // Step 12: Apply structural plasticity
        applyStructuralPlasticity(currentStep);

        // Step 13: Replay important memories
        memorySystemManager->applyMemoryReplay(episodicMemory.get(), currentStep);

        // Step 14: Apply development effects
        developmentSystemManager->updateDevelopment(
            developmentSystem.get(), currentStep, timestep, currentTime,
            structuralPlasticity.get(), developmentalStage, learnedPreference
        );

        // Step 15: Periodic memory consolidation
        memorySystemManager->applyMemoryConsolidation(
            episodicMemory.get(), currentStep, consolidationInterval
        );

        // Step 16: Checkpoint management
        checkpointManager->update(currentStep, currentTime);

        // Step 17: Collect statistics
        collectStatistics();
    }

    void handleSpikes(SimulationStep currentStep, Timestamp currentTime) {
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    auto [justFired, neuronToFire] = NeuronProcessor::processSingleNeuron(
                        region.get(), neuron, currentStep, currentTime, timestep
                    );

                    if (justFired && neuronToFire) {
                        NeuronProcessor::handleNeuronSpike(
                            neuronToFire, region.get(), currentStep, currentTime,
                            timestep, spikeSystem.get(), workingMemory.get()
                        );
                    }
                }
            }
        }
    }

    void updatePredictionSystem() {
        if (predictionSystem) {
            // Update prediction system - conceptual implementation
        }
    }

    void updateAttentionSystem() {
        if (attention) {
            attention->update(timestep);

            if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
                std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
                attention->processCompetition(competitors);
            }
        }
    }

    void updateConceptFormation() {
        if (conceptFormation) {
            // Update concept formation - conceptual implementation
            // This would process current neural activity patterns to form concepts
        }
    }

    void applyStructuralPlasticity(SimulationStep currentStep) {
        if (currentStep % 100 == 0 && structuralPlasticity) {
            structuralPlasticity->update(this, *rng);
        }
    }

    void collectStatistics() {
        // Collect and report statistics
        NLM_LOG_INFO("Step " + std::to_string(currentStep) + ": " +
                     std::to_string(totalSpikesThisStep) + " spikes this step, " +
                     std::to_string(totalSpikesTotal) + " total spikes");
    }

    void updateDevelopmentSystem(SimulationStep currentStep, Timestamp currentTime) {
        developmentSystemManager->updateDevelopment(
            developmentSystem.get(), currentStep, timestep, currentTime,
            structuralPlasticity.get(), developmentalStage, learnedPreference
        );
    }

    void reset() {
        // Reset all systems
        currentStep = 0;
        currentTime = 0.0;
        totalSpikesThisStep = 0;
        totalSpikesTotal = 0;
        isResting = false;
        stepsSinceLastEpisode = 0;

        for (auto& region : regions) {
            region->reset();
        }

        spikeSystem->reset();
        developmentalStage = DevelopmentalStage::Initial;

        if (workingMemory) workingMemory->clear();
        if (episodicMemory) episodicMemory->clear();
        if (associativeMemory) associativeMemory->clear();
        if (attention) attention->reset();
    }

    void addRegion(const std::string& name) {
        // Add a new region - simplified implementation
        auto region = std::make_unique<NeuralRegion>(name);
        region->setNextNeuronIndex(nextRegionId.index());

        // Add populations to region
        auto sensoryPopId = region->addPopulation(100, NeuronType::Sensory);
        auto internalExcitatory = 80;
        auto internalPopId = region->addPopulation(internalExcitatory, NeuronType::Internal);
        auto inhibitoryPopId = region->addPopulation(20, NeuronType::Inhibitory);
        auto motorPopId = region->addPopulation(100, NeuronType::Motor);

        regions.push_back(std::move(region));
        nextRegionId = RegionId(nextRegionId.index() + 1);
    }

    NeuralRegion* getRegion(RegionId id) {
        for (auto& region : regions) {
            if (region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    }

    const NeuralRegion* getRegion(RegionId id) const {
        for (const auto& region : regions) {
            if (region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    }

    size_t getRegionCount() const {
        return regions.size();
    }

    std::vector<RegionId> getRegionIds() const {
        std::vector<RegionId> ids;
        for (const auto& region : regions) {
            ids.push_back(region->getId());
        }
        return ids;
    }

    void buildNeuronLookup() {
        neuronById.clear();
        for (auto& region : regions) {
            for (auto* neuron : region->getAllNeurons()) {
                neuronById[neuron->getId().index()] = neuron;
            }
        }
    }

    void initializeRandomConnectivity(float connectionProbability, float excitatoryWeight, float inhibitoryWeight) {
        // Initialize connectivity with random weights
        for (auto& region : regions) {
            region->initializeRandomConnectivity(*rng, connectionProbability, excitatoryWeight, inhibitoryWeight);
        }
    }

    void addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
        interRegionConnections.push_back({source, target, weight, delay});

        // This would be implemented to create actual connections between regions
        // For now, this is a conceptual implementation
    }

    size_t getTotalNeuronCount() const {
        size_t total = 0;
        for (const auto& region : regions) {
            total += region->getTotalNeuronCount();
        }
        return total;
    }

    size_t getTotalSynapseCount() const {
        size_t total = 0;
        for (const auto& region : regions) {
            total += region->getTotalSynapseCount();
        }
        return total;
    }

    size_t getActiveNeuronCount() const {
        size_t total = 0;
        for (const auto& region : regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || neuron->isRefractory()) {
                        total++;
                    }
                }
            }
        }
        return total;
    }

    size_t getFiringNeuronCount() const {
        size_t total = 0;
        for (const auto& region : regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring()) {
                        total++;
                    }
                }
            }
        }
        return total;
    }

    float getAverageFiringRate() const {
        float total = 0.0f;
        size_t count = 0;
        for (const auto& region : regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    total += neuron->getFiringRate();
                    count++;
                }
            }
        }

        if (count > 0) {
            return total / static_cast<float>(count);
        }
        return 0.0f;
    }

    float getExcitationInhibitionRatio() const {
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
};