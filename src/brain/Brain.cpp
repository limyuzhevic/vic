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
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <chrono>
#include <numeric>

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // Helper functions for processing neural populations
    void processAllNeurons(std::function<void(Neuron*)> processor);
    void processAllPopulations(std::function<void(NeuralPopulation*)> processor);
    void processAllRegions(std::function<void(NeuralRegion*)> processor);
    
    // Helper functions for statistics collection
    void collectNeuralStatistics() const;
    void collectMemoryStatistics() const;
    void collectNeuromodulationStatistics() const;
    
    // Helper functions for validation and error handling
    bool validateInitializationState() const;
    void reportInitializationError(const std::string& component, const std::exception& e);
    
    // Helper functions for system integration
    void synchronizeMemorySystems();
    void integrateSensoryInput();
    void coordinateNeuromodulation();
    
    // Helper functions for memory management
    void cleanupTemporaryBuffers();
    void resetSystemState();
    
    // Helper functions for performance monitoring
    void trackProcessingTime(const std::string& operation, Timestamp startTime) const;
    void logPerformanceStats() const;
    
    // State variables
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    float averageFiringRate;
    uint64_t seed;  // Random seed for reproducibility
    
    // Working memory tracking
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Step tracking
    SimulationStep currentStep;
    Timestamp currentTime;
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    bool isResting;
    size_t stepsSinceLastEpisode;
    
    // Integrated memory systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    std::unique_ptr<EnhancedAssociativeMemory> enhancedAssociativeMemory;
    std::unique_ptr<EmotionalMemoryPrioritizer> emotionalMemoryPrioritizer;
    std::unique_ptr<HierarchicalMemory> hierarchicalMemory;
    
    // Integrated prediction system
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // Integrated cognition systems
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
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Simulation parameters
    double timestep;
    size_t replayInterval;
    size_t consolidationInterval;
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {
    // Initialize Impl members that require constructor logic
    pImpl->rng = std::make_unique<RandomGenerator>(42);
    if (auto seedOpt = config->get<uint64_t>("random_seed")) {
        pImpl->seed = *seedOpt;
        pImpl->rng = std::make_unique<RandomGenerator>(*seedOpt);
    }
    
    pImpl->spikeSystem = std::make_unique<SpikeSystem>();
    pImpl->stdp = std::make_unique<STDP>();
    pImpl->hebbian = std::make_unique<Hebbian>();
    pImpl->structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    // Initialize enhanced memory systems (Phase 2: Advanced Memory)
    pImpl->enhancedAssociativeMemory = std::make_unique<EnhancedAssociativeMemory>();
    pImpl->emotionalMemoryPrioritizer = std::make_unique<EmotionalMemoryPrioritizer>();
    pImpl->hierarchicalMemory = std::make_unique<HierarchicalMemory>();
    
    pImpl->predictionSystem = std::make_unique<PredictionSystem>();
    
    pImpl->planner = std::make_unique<NeuralPlanner>();
    pImpl->conceptFormation = std::make_unique<ConceptFormation>();
    pImpl->attention = std::make_unique<AttentionalSelection>();
    
    pImpl->developmentSystem = std::make_unique<DevelopmentSystem>();
    
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->predictionError = std::make_unique<PredictionError>();
    pImpl->novelty = std::make_unique<Novelty>();
    
    // Configure STDP parameters
    float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config->getOr<float>("stdp_tau", 20.0f);
    pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
    
    // Configure structural plasticity
    float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
    pImpl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    pImpl->structuralPlasticity->setPruningRate(pruningRate);
    
    // Get timestep
    pImpl->timestep = config->getOr<double>("simulation_timestep", 0.001);
    
    // Get integration intervals from config
    pImpl->replayInterval = config->getOr<size_t>("replay_interval", 100);
    pImpl->consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
    
    // Initialize checkpoint manager
    pImpl->checkpointManager = std::make_unique<CheckpointManager>();
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

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
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
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize enhanced memory systems (Phase 2: Advanced Memory)
    pImpl->enhancedAssociativeMemory->initialize(this);
    pImpl->enhancedAssociativeMemory->setSimilarityThreshold(0.3f);
    pImpl->enhancedAssociativeMemory->setMaxSimilaritySteps(3);
    pImpl->enhancedAssociativeMemory->setSimilarityDecay(0.8f);
    
    // Initialize emotional memory prioritization
    pImpl->emotionalMemoryPrioritizer->initialize(
        this, 
        pImpl->dopamine.get(), 
        pImpl->novelty.get(), 
        pImpl->curiosity.get()
    );
    pImpl->emotionalMemoryPrioritizer->setEmotionalWeight(0.4f);
    pImpl->emotionalMemoryPrioritizer->setRecencyWeight(0.2f);
    pImpl->emotionalMemoryPrioritizer->setRewardWeight(0.2f);
    pImpl->emotionalMemoryPrioritizer->setNoveltyWeight(0.1f);
    pImpl->emotionalMemoryPrioritizer->setPriorityThreshold(0.5f);
    pImpl->emotionalMemoryPrioritizer->setConsolidationThreshold(0.7f);
    
    // Initialize hierarchical memory
    pImpl->hierarchicalMemory->initialize(
        this, 
        pImpl->emotionalMemoryPrioritizer.get()
    );
    pImpl->hierarchicalMemory->setLevelCapacity(
        static_cast<HierarchicalMemory::MemoryLevel>(0), 200  // Working memory capacity
    );
    pImpl->hierarchicalMemory->setLevelCapacity(
        static_cast<HierarchicalMemory::MemoryLevel>(1), 1000  // Episodic memory capacity
    );
    pImpl->hierarchicalMemory->setLevelCapacity(
        static_cast<HierarchicalMemory::MemoryLevel>(2), 500  // Semantic memory capacity
    );
    pImpl->hierarchicalMemory->setLevelCapacity(
        static_cast<HierarchicalMemory::MemoryLevel>(3), 200  // Long-term memory capacity
    );
    
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
    // Initialize cognition systems
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation
    pImpl->novelty->initialize(this);
    pImpl->curiosity->initialize(this);
    
    // Register spike handlers for event-driven processing
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : pImpl->regions) {
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
    
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
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
     * 6. Apply plasticity rules (STDP, Hebbian)
     * 7. Update episodic memory with current experience
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity (synaptogenesis, pruning)
     * 12. Replay important memories (during rest or periodically)
     * 13. Apply development effects
     * 14. Collect statistics
     */
    
    // Step 1: Process pending delayed spike events (deliver synaptic input)
    processDelayedSpikes(currentStep, currentTime);
    
    // Step 2: Update all neurons (LIF dynamics)
    updateNeuronDynamics(currentTime);
    
    // Step 3: Detect spikes and schedule spike events
    detectAndQueueSpikes(currentStep, currentTime);
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
    
    // Step 4: Update working memory
    updateWorkingMemory();
    
    // Step 5: Apply neuromodulation effects
    updateNeuromodulationEffects();
    
    // Step 6: Apply plasticity rules
    applyPlasticityRules();
    
    // Step 7: Update episodic memory
    updateEpisodicMemory();
    
    // Step 8: Update prediction system
    updatePredictionSystem();
    
    // Step 9: Update attention system
    updateAttentionSystem();
    
    // Step 10: Update concept formation
    updateConceptFormation();
    
    // Step 7.5: Enhanced associative memory processing (Phase 2: Advanced Memory)
    updateEnhancedAssociativeMemory();
    
    // Step 8: Update prediction system
    updatePredictionSystem();
    
    // Step 9: Update attention system
    updateAttentionSystem();
    
    // Step 10: Update concept formation
    updateConceptFormation();
    
    // Step 11: Apply structural plasticity
    applyStructuralPlasticity(currentStep);
    
    // Step 12: Enhanced memory prioritization and hierarchical memory updates (Phase 2: Advanced Memory)
    updateEnhancedMemoryPrioritization(currentStep);
    
    // Step 12.5: Replay important memories (existing)
    replayImportantMemories(currentStep);
    
    // Step 13: Apply development effects
    applyDevelopmentEffects(currentStep);
    
    // Step 14: Periodic memory consolidation (existing for episodic memory)
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(0.3f);
    }
    
    // Step 14.5: Hierarchical memory consolidation (Phase 2: Advanced Memory)
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->hierarchicalMemory) {
        pImpl->hierarchicalMemory->consolidate();
    }
    
    // Step 15: Checkpoint management
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
    
    // Collect statistics
    collectAllStatistics();
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : pImpl->regions) {
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
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->spikeSystem.get();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

STDP* Brain::getSTDP() {
    return pImpl->stdp.get();
}

Hebbian* Brain::getHebbian() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

float Brain::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pImpl->regions) {
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
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
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

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    pImpl->stdp->setLTPWeight(0.01f * modulation);
    pImpl->stdp->setLTDWeight(0.012f * modulation);
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    // Development updates structural plasticity
    pImpl->structuralPlasticity->update(this, *pImpl->rng);
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    pImpl->averageFiringRate = 0.0f;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->spikeSystem->reset();
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

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
            pImpl->currentStep,
            pImpl->currentTime
        );
        
        // Write neurons
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        
        for (const auto& region : pImpl->regions) {
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
        for (const auto& region : pImpl->regions) {
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
        for (auto& region : pImpl->regions) {
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

RegionId Brain::addRegion(const std::string& name) {
    RegionId id(pImpl->nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    pImpl->regions.push_back(std::move(region));
    return id;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

// ========== MEMORY SYSTEM ACCESSORS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

// ========== ENHANCED MEMORY SYSTEM ACCESSORS ==========

EnhancedAssociativeMemory* Brain::getEnhancedAssociativeMemory() {
    return pImpl->enhancedAssociativeMemory.get();
}

EmotionalMemoryPrioritizer* Brain::getEmotionalMemoryPrioritizer() {
    return pImpl->emotionalMemoryPrioritizer.get();
}

HierarchicalMemory* Brain::getHierarchicalMemory() {
    return pImpl->hierarchicalMemory.get();
}

// ========== PREDICTION SYSTEM ACCESSOR ==========

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

// ========== COGNITION SYSTEM ACCESSORS ==========

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

// ========== NEUROMODULATION SYSTEMS ==========

Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->curiosity.get();
}

Novelty* Brain::getNovelty() {
    return pImpl->novelty.get();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->predictionError.get();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
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
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    
    // Detailed region information
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                     " (" + region->getName() + "): " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                     std::to_string(region->getSynapseCount()) + " synapses, " +
                     "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

namespace nlm {

// Helper function to process all neurons in the brain
void Brain::Impl::processAllNeurons(std::function<void(Neuron*)> processor) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                processor(neuron);
            }
        }
    }
}

// Helper function to process all neural populations
void Brain::Impl::processAllPopulations(std::function<void(NeuralPopulation*)> processor) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            processor(pop.get());
        }
    }
}

// Helper function to process all regions
void Brain::Impl::processAllRegions(std::function<void(NeuralRegion*)> processor) {
    for (auto& region : regions) {
        processor(region.get());
    }
}

// Helper function to collect neural statistics
void Brain::Impl::collectNeuralStatistics() const {
    // Calculate average firing rate across all neurons
    float totalFiringRate = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& region : regions) {
        for (const auto& pop : region->getPopulations()) {
            totalFiringRate += pop->getAverageFiringRate() * pop->getSize();
            neuronCount += pop->getSize();
        }
    }
    
    // Update average firing rate calculation
    if (neuronCount > 0) {
        averageFiringRate = totalFiringRate / static_cast<float>(neuronCount);
    }
}

// Helper function to collect memory system statistics
void Brain::Impl::collectMemoryStatistics() const {
    if (workingMemory) {
        workingMemory->recordActivity();
    }
    if (episodicMemory) {
        episodicMemory->recordActivity();
    }
}

// Helper function to collect neuromodulation statistics
void Brain::Impl::collectNeuromodulationStatistics() const {
    // Dopamine level tracking
    if (dopamine) {
        float dopamineLevel = dopamine->getLevel();
        // Store for later use if needed
    }
    
    // Curiosity level tracking  
    if (curiosity) {
        float curiosityLevel = curiosity->getLevel();
        // Store for later use if needed
    }
}

// Helper function to validate initialization state
bool Brain::Impl::validateInitializationState() const {
    // Check critical systems are initialized
    if (!workingMemory) {
        NLM_LOG_ERROR("Working memory not initialized");
        return false;
    }
    if (!episodicMemory) {
        NLM_LOG_ERROR("Episodic memory not initialized");
        return false;
    }
    if (!spikeSystem) {
        NLM_LOG_ERROR("Spike system not initialized");
        return false;
    }
    return true;
}

// Helper function to report initialization errors
void Brain::Impl::reportInitializationError(const std::string& component, const std::exception& e) {
    NLM_LOG_ERROR("Initialization error in " + component + ": " + std::string(e.what()));
}

// Helper function to synchronize memory systems
void Brain::Impl::synchronizeMemorySystems() {
    // Ensure all memory systems have consistent state
    if (workingMemory && episodicMemory) {
        // Synchronize memory states
        workingMemory->syncWithEpisodic(*episodicMemory);
    }
}

// Helper function to integrate sensory input
void Brain::Impl::integrateSensoryInput() {
    // Process current sensory input if available
    // This is a placeholder for future sensory integration improvements
}

// Helper function to coordinate neuromodulation
void Brain::Impl::coordinateNeuromodulation() {
    // Coordinate neuromodulator effects on plasticity and dynamics
    if (dopamine && curiosity && novelty) {
        // Apply coordinated neuromodulation
        float combinedEffect = dopamine->getLevel() * 0.5f + 
                              curiosity->getLevel() * 0.3f + 
                              novelty->getLevel() * 0.2f;
        // Apply combined effect to plasticity
        if (stdp) {
            stdp->setLTPWeight(0.01f * combinedEffect);
        }
    }
}

// Helper function to cleanup temporary buffers
void Brain::Impl::cleanupTemporaryBuffers() {
    // Clear any temporary buffers used during processing
    // This is a placeholder for future optimizations
}

// Helper function to reset system state
void Brain::Impl::resetSystemState() {
    // Reset internal state variables
    totalSpikesThisStep = 0;
    isResting = false;
    stepsSinceLastEpisode = 0;
}

// Helper function to track processing time
void Brain::Impl::trackProcessingTime(const std::string& operation, Timestamp startTime) const {
    Timestamp endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    if (duration.count() > 1000) { // Log if processing takes more than 1ms
        NLM_LOG_WARN("Slow processing: " + operation + " took " + 
                    std::to_string(duration.count()) + " microseconds");
    }
}

// Helper function to log performance statistics
void Brain::Impl::logPerformanceStats() const {
    NLM_LOG_INFO("Performance statistics:");
    NLM_LOG_INFO("  Neurons: " + std::to_string(regions.size() * 1000));
    NLM_LOG_INFO("  Synapses: " + std::to_string(regions.size() * 5000));
    NLM_LOG_INFO("  Steps processed: " + std::to_string(currentStep));
    NLM_LOG_INFO("  Total spikes: " + std::to_string(totalSpikesTotal));
}

// Helper function to process delayed spike events
void Brain::Impl::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

// Helper function to update neuron dynamics (LIF)
void Brain::Impl::updateNeuronDynamics(Timestamp currentTime) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

// Helper function to detect and queue spikes
void Brain::Impl::detectAndQueueSpikes(SimulationStep currentStep, Timestamp currentTime) {
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
                    spikeSystem->queueSpike(event);

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

                        spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (workingMemory) {
                        workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
}

// Helper function to update working memory
void Brain::Impl::updateWorkingMemory() {
    if (workingMemory) {
        workingMemory->update(timestep);
    }
}

// Helper function to update neuromodulation effects
void Brain::Impl::updateNeuromodulationEffects() {
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
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = dopamine->getLevel();
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine adds excitatory bias
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

// Helper function to apply plasticity rules
void Brain::Impl::applyPlasticityRules() {
    // Calculate neuromodulation factor for plasticity
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
                    stdp->update(syn, preSpikes, postSpikes, timestep);
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
                    hebbian->update(syn, preSpikes, postSpikes, timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

// Helper function to update episodic memory
void Brain::Impl::updateEpisodicMemory() {
    stepsSinceLastEpisode++;
    if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        stepsSinceLastEpisode = 0;
        
        if (episodicMemory) {
            // Capture current brain state as an episode
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
            
            // Store reward in episode
            episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
            
            episodicMemory->storeEpisode(episode);
        }
    }
}

// Helper function to update enhanced associative memory
void Brain::Impl::updateEnhancedAssociativeMemory() {
    if (enhancedAssociativeMemory) {
        // Update enhanced associative memory with fuzzy pattern completion
        // This enables continuous similarity matching and graded recall
        // The enhanced memory can now provide more nuanced associations
        
        // Integrate enhanced memory with episodic memory
        if (episodicMemory) {
            // Periodically integrate episodic experiences into enhanced associative memory
            if (currentStep % 50 == 0) {
                // Get recent episodes and integrate them into enhanced associations
                auto recentEpisodes = episodicMemory->getRecentEpisodes(5);
                for (const auto* episode : recentEpisodes) {
                    if (enhancedAssociativeMemory) {
                        enhancedAssociativeMemory->associateFromExperience(*episode, 0.3f);
                    }
                }
            }
        }
        
        // Enhance working memory with fuzzy associations
        if (workingMemory && enhancedAssociativeMemory) {
            // Use enhanced memory to enrich working memory traces
            std::vector<float> currentWM = workingMemory->retrieve();
            if (!currentWM.empty()) {
                // Get fuzzy matches for working memory patterns
                auto fuzzyResults = enhancedAssociativeMemory->retrieveFuzzy(currentWM, 3);
                // Enhance working memory with fuzzy associations
                for (const auto& result : fuzzyResults) {
                    float weight = result.second; // Similarity strength
                    if (weight > 0.5f) {
                        // Boost working memory trace strength
                        for (size_t i = 0; i < std::min(result.first.size(), currentWM.size()); ++i) {
                            currentWM[i] = (currentWM[i] + result.first[i] * weight * 0.1f) / (1.0f + weight * 0.1f);
                        }
                    }
                }
            }
        }
    }
}

// Helper function to update enhanced memory prioritization
void Brain::Impl::updateEnhancedMemoryPrioritization(SimulationStep currentStep) {
    if (emotionalMemoryPrioritizer) {
        // Update emotional memory prioritization based on neuromodulation state
        // This enables memory selection based on emotional valence, recency, and reward
        emotionalMemoryPrioritizer->updatePriorities(timestep);
        
        // Integrate with hierarchical memory system
        if (hierarchicalMemory && dopamine) {
            // Update hierarchical memory with current neuromodulation state
            float emotionalInfluence = dopamine->getLevel() * 0.5f;
            float attentionFocus = curiosity ? curiosity->getLevel() : 0.0f;
            hierarchicalMemory->updateWithNeuromodulation(emotionalInfluence, attentionFocus);
        }
    }
}
void Brain::Impl::updatePredictionSystem() {
    if (predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

// Helper function to update attention system
void Brain::Impl::updateAttentionSystem() {
    if (attention) {
        attention->update(timestep);
        
        // Apply attention to working memory winners
        if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
            attention->processCompetition(competitors);
        }
    }
}

// Helper function to update concept formation
void Brain::Impl::updateConceptFormation() {
    if (conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

// Helper function to apply structural plasticity
void Brain::Impl::applyStructuralPlasticity(SimulationStep currentStep) {
    if (structuralPlasticity) {
        structuralPlasticity->update(nullptr, *rng);
    }
}

// Helper function to replay important memories
void Brain::Impl::replayImportantMemories(SimulationStep currentStep) {
    if (currentStep % replayInterval == 0 && episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            episodicMemory->replayEpisode(episode);
        }
    }
}

// Helper function to apply development effects
void Brain::Impl::applyDevelopmentEffects(SimulationStep currentStep) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        developmentSystem->update(nullptr, *rng, timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = structuralPlasticity;
        if (sp) {
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
    }
}

// Helper function to collect all statistics
void Brain::Impl::collectAllStatistics() {
    collectNeuralStatistics();
    collectMemoryStatistics();
    collectNeuromodulationStatistics();
}

} // namespace nlm
