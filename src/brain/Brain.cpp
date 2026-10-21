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
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct Brain::Impl {
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
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)      // Replay every 100 steps
        , consolidationInterval(1000)  // Consolidate every 1000 steps
    {
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // ========== INITIALIZE INTEGRATED SYSTEMS ==========
        
        // Initialize memory systems
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        // Initialize prediction system
        predictionSystem = std::make_unique<PredictionSystem>();
        
        // Initialize cognition systems
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
        
        // Initialize development system
        developmentSystem = std::make_unique<DevelopmentSystem>();
        
        // Initialize neuromodulation systems
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        
        // Configure STDP parameters
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        // Get timestep
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

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
    
    // Initialize new architecture modules
    brainCore_ = std::make_unique<BrainCore>(pImpl->config);
    brainSimulation_ = std::make_unique<BrainSimulation>(brainCore_);
    
    // Legacy initialization (backward compatibility)
    pImpl->initializeLegacy();
    
    NLM_LOG_INFO("NLM Brain initialized successfully");
    return true;
}

void Brain::initializeLegacy() {
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
    
    // Initialize working memory
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(this);
    
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
    // Initialize cognition systems
    pImpl->planner->initialize(this);
    
    // Initialize development system
    pImpl->developmentSystem->initialize(this);
    
    // Initialize neuromodulation systems
    pImpl->dopamine->initialize();
    pImpl->curiosity->initialize();
    pImpl->predictionError->initialize();
    pImpl->novelty->initialize();
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, pImpl->currentTime);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // Update simulation state
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    
    // Use new architecture for simulation
    if (brainSimulation_) {
        brainSimulation_->step(currentStep, currentTime);
    }
    
    // Legacy simulation (for backward compatibility)
    pImpl->stepLegacy(currentStep, currentTime);
}

void Brain::stepLegacy(SimulationStep currentStep, Timestamp currentTime) {
    // ========== STEP 1: PROCESS NEURAL DYNAMICS ==========
    pImpl->timestep = pImpl->config->getOr<double>("simulation_timestep", 0.001);
    
    // Process neural dynamics
    for (const auto& region : pImpl->regions) {
        region->processStep(pImpl->timestep);
    }
    
    // Process inter-region connections
    pImpl->processInterRegionConnectionsLegacy();
    
    // ========== STEP 2: PROCESS SPIKE EVENTS ==========
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
    
    // Update spike statistics
    pImpl->totalSpikesThisStep = pImpl->spikeSystem->getTotalSpikesThisStep();
    pImpl->totalSpikesTotal += pImpl->totalSpikesThisStep;
    
    // ========== STEP 3: UPDATE PLASTICITY ==========
    // Update STDP with spike history
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            if (syn->getPlasticityFlags().stdp) {
                pImpl->stdp->update(syn, pImpl->timestep);
            }
        }
    }
    
    // Update Hebbian plasticity
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            if (syn->getPlasticityFlags().hebbian) {
                pImpl->hebbian->update(syn, pImpl->timestep);
            }
        }
    }
    
    // ========== STEP 4: Update working memory ==========
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
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
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
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
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
    
    // ========== STEP 7: Update episodic memory ==========
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : pImpl->regions) {
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
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // ========== STEP 8: Apply developmental changes ==========
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(pImpl->timestep);
    }
    
    // Update developmental stage
    pImpl->developmentalStage = pImpl->developmentSystem ? 
                                pImpl->developmentSystem->getDevelopmentalStage() : 
                                DevelopmentalStage::Adult;
    
    // ========== STEP 9: Update prediction system ==========
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->update(pImpl->timestep);
    }
    
    // ========== STEP 10: Update cognition systems ==========
    if (pImpl->planner) {
        pImpl->planner->update(pImpl->timestep);
    }
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->update(pImpl->timestep);
    }
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
    }
    
    // ========== STEP 11: Check for episode end ==========
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= pImpl->replayInterval) {
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->replayEpisodes();
        }
    }
    
    // ========== STEP 12: Update consolidation ==========
    if (pImpl->stepsSinceLastEpisode >= pImpl->consolidationInterval) {
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->consolidateImportantEpisodes();
        }
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Distribute input across sensory neurons
    size_t numSensory = std::min(pImpl->sensoryNeurons.size(), input.getDimensions());
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = input.getVision();
    for (size_t i = 0; i < numSensory && i < vision.size(); ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = vision[i] * 20.0f - 10.0f;
        
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
    
    // Find motor neuron with highest activity
    Neuron* mostActiveNeuron = nullptr;
    float maxActivity = -1.0f;
    
    for (auto* neuron : pImpl->motorNeurons) {
        float activity = neuron->getFiringRate();
        if (activity > maxActivity) {
            maxActivity = activity;
            mostActiveNeuron = neuron;
        }
    }
    
    // Convert neural activity to action
    ActionType actionType = ActionType::Wait;
    if (mostActiveNeuron) {
        // Simple mapping: highest activity neuron determines action
        // In a real implementation, this would be more sophisticated
        actionType = ActionType::Wait;
    }
    
    return std::make_unique<Action>(actionType);
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulatory signal to brain
    // This would affect neural excitability and plasticity
}

void Brain::updatePlasticity() {
    // Update all plasticity systems
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            syn->updatePlasticity();
        }
    }
}

void Brain::develop() {
    // Update development system
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(pImpl->timestep);
    }
}

void Brain::reset() {
    // Reset all brain components
    pImpl->reset();
    
    // Reset new architecture modules
    if (brainSimulation_) {
        brainSimulation_->reset();
    }
}

void Brain::resetLegacy() {
    // Reset legacy components
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->sensoryNeurons.clear();
    pImpl->motorNeurons.clear();
    pImpl->totalSpikesThisStep = 0;
    pImpl->stepsSinceLastEpisode = 0;
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    
    if (pImpl->workingMemory) {
        pImpl->workingMemory->reset();
    }
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->reset();
    }
    if (pImpl->dopamine) {
        pImpl->dopamine->reset();
    }
    if (pImpl->curiosity) {
        pImpl->curiosity->reset();
    }
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file for writing: " + filepath);
            return false;
        }
        
        // Write configuration
        CheckpointConfigData configData;
        configData.neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        configData.regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        configData.connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        configData.timestep = pImpl->config->getOr<double>("simulation_timestep", 0.001);
        configData.randomSeed = pImpl->config->getOr<uint64_t>("random_seed", 42);
        
        if (!writer.writeConfig(configData)) {
            NLM_LOG_ERROR("Failed to write configuration to checkpoint");
            return false;
        }
        
        // Write neurons
        NeuronCheckpointData neuronData;
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    neuronData.membranePotential.push_back(neuron->getMembranePotential());
                    neuronData.restingPotential.push_back(neuron->getRestingPotential());
                    neuronData.threshold.push_back(neuron->getThreshold());
                    neuronData.resetPotential.push_back(neuron->getResetPotential());
                    neuronData.leakConductance.push_back(neuron->getLeakConductance());
                    neuronData.firingState.push_back(static_cast<uint8_t>(neuron->getState().firingState));
                    neuronData.refractoryRemaining.push_back(neuron->getState().refractoryRemaining);
                    neuronData.refractoryPeriod.push_back(neuron->getState().refractoryPeriod);
                    neuronData.lastSpikeTime.push_back(neuron->getState().lastSpikeTime);
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
        
        // Apply neuron states - this is complex because we need to find matching synapses
        // For now, just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(neuronData.membranePotential.size()) + " neurons from checkpoint");
        
        // Read synapses
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.sourceNeuron.size()) + " synapses from checkpoint");
        
        // Read configuration
        CheckpointConfigData configData;
        if (!reader.readConfig(configData)) {
            NLM_LOG_ERROR("Failed to read configuration from checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Loaded configuration from checkpoint (" + 
                     std::to_string(configData.neuronCount) + " neurons, " +
                     std::to_string(configData.regionCount) + " regions)");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->addRegion(name);
    }
    
    // Legacy implementation
    RegionId newId = pImpl->nextRegionId++;
    pImpl->regions.push_back(std::make_unique<NeuralRegion>(newId, name));
    return newId;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getRegion(id);
    }
    
    // Legacy implementation
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getRegion(id);
    }
    
    // Legacy implementation
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getRegionCount();
    }
    
    // Legacy implementation
    return pImpl->regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getRegionIds();
    }
    
    // Legacy implementation
    std::vector<RegionId> regionIds;
    for (const auto& region : pImpl->regions) {
        regionIds.push_back(region->getId());
    }
    return regionIds;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getRegions();
    }
    
    // Legacy implementation
    static const std::vector<std::unique_ptr<NeuralRegion>> empty;
    return empty;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                     float weight, Delay delay) {
    // Use new architecture if available
    if (brainSimulation_) {
        brainSimulation_->addInterRegionConnection(source, target, weight, delay);
        return;
    }
    
    // Legacy implementation
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    // Use new architecture if available
    if (brainSimulation_) {
        brainSimulation_->removeInterRegionConnection(source, target);
        return;
    }
    
    // Legacy implementation
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(), pImpl->interRegionConnections.end(),
            [source, target](const InterRegionConnection& conn) {
                return conn.sourceRegion == source && conn.targetRegion == target;
            }),
        pImpl->interRegionConnections.end()
    );
}

void Brain::processInterRegionConnectionsLegacy() {
    for (const auto& connection : pImpl->interRegionConnections) {
        auto* sourceRegion = getRegion(connection.sourceRegion);
        auto* targetRegion = getRegion(connection.targetRegion);
        
        if (!sourceRegion || !targetRegion) continue;
        
        // Process connection with delay
        // Implementation would handle spike propagation with delays
    }
}

size_t Brain::getTotalNeuronCount() const {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getTotalNeuronCount();
    }
    
    // Legacy implementation
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getTotalSynapseCount();
    }
    
    // Legacy implementation
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalSynapseCount();
    }
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getActiveNeuronCount();
    }
    
    // Legacy implementation
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getFiringNeuronCount();
    }
    
    // Legacy implementation
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getFiringNeuronCount();
    }
    return total;
}

float Brain::getAverageFiringRate() const {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getAverageFiringRate();
    }
    
    // Legacy implementation
    size_t totalNeurons = getTotalNeuronCount();
    if (totalNeurons == 0) return 0.0f;
    
    float totalFiring = 0.0f;
    for (const auto& region : pImpl->regions) {
        totalFiring += region->getAverageFiringRate();
    }
    
    return totalFiring / totalNeurons;
}

// Memory system implementations
NeuralWorkingMemory* Brain::getWorkingMemory() {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getWorkingMemory();
    }
    
    // Legacy implementation
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getEpisodicMemory();
    }
    
    // Legacy implementation
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getAssociativeMemory();
    }
    
    // Legacy implementation
    return pImpl->associativeMemory.get();
}

// Prediction system implementation
PredictionSystem* Brain::getPredictionSystem() {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getPredictionSystem();
    }
    
    // Legacy implementation
    return pImpl->predictionSystem.get();
}

// Cognition system implementations
NeuralPlanner* Brain::getPlanner() {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getPlanner();
    }
    
    // Legacy implementation
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getConceptFormation();
    }
    
    // Legacy implementation
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getAttention();
    }
    
    // Legacy implementation
    return pImpl->attention.get();
}

// Development system implementation
DevelopmentSystem* Brain::getDevelopmentSystem() {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getDevelopmentSystem();
    }
    
    // Legacy implementation
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getDevelopmentalStage();
    }
    
    // Legacy implementation
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    // Use new architecture if available
    if (brainSimulation_) {
        brainSimulation_->setDevelopmentalStage(stage);
        return;
    }
    
    // Legacy implementation
    pImpl->developmentalStage = stage;
}

// Neuromodulation system implementations
Dopamine* Brain::getDopamine() {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getDopamine();
    }
    
    // Legacy implementation
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getCuriosity();
    }
    
    // Legacy implementation
    return pImpl->curiosity.get();
}

Novelty* Brain::getNovelty() {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getNovelty();
    }
    
    // Legacy implementation
    return pImpl->novelty.get();
}

PredictionError* Brain::getPredictionErrorSignal() {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getPredictionErrorSignal();
    }
    
    // Legacy implementation
    return pImpl->predictionError.get();
}

// Configuration and random access
std::shared_ptr<const Config> Brain::getConfig() const {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getConfig();
    }
    
    // Legacy implementation
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    // Use new architecture if available
    if (brainSimulation_) {
        return brainSimulation_->getRandomGenerator();
    }
    
    // Legacy implementation
    return pImpl->rng.get();
}

// Logging
void Brain::logStatus() const {
    NLM_LOG_INFO("=== Legacy Brain Status ===");
    NLM_LOG_INFO("Regions: " + std::to_string(pImpl->regions.size()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes this step: " + std::to_string(pImpl->totalSpikesThisStep));
    NLM_LOG_INFO("Total spikes total: " + std::to_string(pImpl->totalSpikesTotal));
    NLM_LOG_INFO("Simulation time: " + std::to_string(pImpl->currentTime) + "s");
    NLM_LOG_INFO("Current step: " + std::to_string(pImpl->currentStep));
}

// New architecture logging
void Brain::logStatusNew() const {
    if (brainSimulation_) {
        brainSimulation_->logStatus();
    }
    NLM_LOG_INFO("=== New Brain Architecture Status ===");
    NLM_LOG_INFO("BrainCore initialized: " + std::to_string(static_cast<bool>(brainCore_)));
    NLM_LOG_INFO("BrainSimulation initialized: " + std::to_string(static_cast<bool>(brainSimulation_)));
}

}} // namespace nlm