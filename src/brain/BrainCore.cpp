#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <cstring>
#include <sstream>
#include <vector>
#include <memory>
#include <numeric>

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
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<PredictionError> predictionError;
    
    // ========== SPiking and Plasticity ==========
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // ========== Core Brain State ==========
    SimulationStep currentStep;
    Timestamp currentTime;
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    float learnedPreference;
    DevelopmentalStage developmentalStage;
    
    // Homeostatic parameters
    float homeostaticTargetFiringRate;
    float homeostaticAdaptationRate;
    float homeostaticTimeConstant;
    
    // Local competition
    bool localCompetitionEnabled;
    float localCompetitionStrength;
    std::vector<float> competitionBias;
    
    // Checkpoint/replay
    bool checkpointRequested;
    std::string checkpointPath;
    size_t replayStep;
    
    // ===== CONSTRUCTION AND DESTRUCTION =====
    
    Impl() : config(nullptr), rng(nullptr), regions(), interRegionConnections()
        , workingMemory(nullptr), episodicMemory(nullptr), associativeMemory(nullptr)
        , predictionSystem(nullptr), planner(nullptr), conceptFormation(nullptr), attention(nullptr)
        , developmentSystem(nullptr), dopamine(nullptr), curiosity(nullptr), novelty(nullptr), predictionError(nullptr)
        , spikeSystem(nullptr), stdp(nullptr), hebbian(nullptr), structuralPlasticity(nullptr)
        , currentStep(0), currentTime(0.0), totalSpikesThisStep(0), totalSpikesTotal(0), isResting(false)
        , stepsSinceLastEpisode(0), replayInterval(100), consolidationInterval(1000)
        , learnedPreference(0.0f), developmentalStage(DevelopmentalStage::Early)
        , homeostaticTargetFiringRate(20.0f), homeostaticAdaptationRate(0.001f), homeostaticTimeConstant(1000.0f)
        , localCompetitionEnabled(true), localCompetitionStrength(0.1f), competitionBias(),
        checkpointRequested(false), replayStep(0) {}
    
    ~Impl() = default;
};

// ===== BRAIN CORE IMPLEMENTATION =====

Brain::Brain(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Brain::initialize() {
    // TODO PHASE 3: Implement full brain initialization
    // This is a simplified version for demonstration
    
    // Initialize random generator with seed from config
    uint64_t seed = 42;  // Default seed
    if (auto seedOpt = pImpl->config->get<int64_t>("random_seed")) {
        seed = *seedOpt;
    }
    pImpl->rng = std::make_unique<RandomGenerator>(seed);
    
    // Initialize plasticity systems
    pImpl->spikeSystem = std::make_unique<SpikeSystem>();
    pImpl->stdp = std::make_unique<STDP>();
    pImpl->hebbian = std::make_unique<Hebbian>();
    pImpl->structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    // Configure STDP parameters
    float ltpWeight = pImpl->config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = pImpl->config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = pImpl->config->getOr<float>("stdp_tau", 20.0f);
    pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
    
    // Initialize regions based on config
    int64_t regionCount = pImpl->config->getOr<int64_t>("region_count", 2);
    int64_t neuronCount = pImpl->config->getOr<int64_t>("neuron_count", 1000);
    
    float neuronsPerRegion = static_cast<float>(neuronCount) / static_cast<float>(regionCount);
    
    for (int64_t i = 0; i < regionCount; ++i) {
        RegionId regionId(i);
        size_t neuronsInRegion = static_cast<size_t>(neuronsPerRegion);
        
        if (i == regionCount - 1) {
            neuronsInRegion = neuronCount - (regionCount - 1) * neuronsInRegion;
        }
        
        auto region = std::make_unique<NeuralRegion>(regionId, neuronsInRegion, pImpl->rng.get());
        pImpl->regions.push_back(std::move(region));
    }
    
    // Initialize memory systems
    pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    // Initialize prediction system
    pImpl->predictionSystem = std::make_unique<PredictionSystem>();
    
    // Initialize cognition systems
    pImpl->planner = std::make_unique<NeuralPlanner>();
    pImpl->conceptFormation = std::make_unique<ConceptFormation>();
    pImpl->attention = std::make_unique<AttentionalSelection>();
    
    // Initialize development system
    pImpl->developmentSystem = std::make_unique<DevelopmentSystem>();
    
    // Initialize neuromodulation systems
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->predictionError = std::make_unique<PredictionError>();
    pImpl->novelty = std::make_unique<Novelty>();
    
    // Set development stage
    pImpl->developmentSystem->setStage(pImpl->developmentalStage);
    
    return true;
}

void Brain::step(SimulationStep step, Timestamp time) {
    currentStep = step;
    currentTime = time;
    
    // Reset spike counters for this step
    pImpl->totalSpikesThisStep = 0;
    
    // Apply homeostatic scaling
    applyHomeostaticScaling();
    
    // Apply local competition
    applyLocalCompetition();
    
    // Process all regions
    for (auto& region : pImpl->regions) {
        region->step(step, time, pImpl->spikeSystem.get());
    }
    
    // Aggregate spikes
    aggregateSpikes();
    
    // Apply plasticity
    updatePlasticity();
    
    // Update neuromodulation
    updateNeuromodulation();
    
    // Update development
    pImpl->developmentSystem->update(time, step);
    
    // Apply development (synaptogenesis, pruning, etc.)
    develop();
    
    // Update memory systems
    pImpl->workingMemory->update(time, step);
    pImpl->episodicMemory->update(time, step);
    
    // Update prediction system
    pImpl->predictionSystem->update(time, step);
    
    // Update cognition systems
    pImpl->planner->update(time, step);
    pImpl->conceptFormation->update(time, step);
    pImpl->attention->update(time, step);
    
    // Apply checkpoint if requested
    if (pImpl->checkpointRequested) {
        save(pImpl->checkpointPath);
        pImpl->checkpointRequested = false;
    }
}

void Brain::step(SimulationStep step) {
    step(step, step * pImpl->config->getOr<double>("simulation_timestep", 0.001));
}

void Brain::receiveSensoryInput(const SensoryInput& input) {
    // Inject current into sensory neurons based on input pattern
    for (const auto& [neuronId, current] : input.getNeuronCurrents()) {
        injectCurrent(neuronId, current);
    }
}

void Brain::injectCurrent(NeuronId neuronId, MembranePotential current) {
    // Find the neuron in regions and inject current
    for (auto& region : pImpl->regions) {
        auto* neuron = region->getNeuron(neuronId);
        if (neuron) {
            neuron->injectCurrent(current);
            break;
        }
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    // Inject current into all neurons of a specific type
    for (auto& region : pImpl->regions) {
        region->injectCurrentToNeurons(type, current);
    }
}

void Brain::aggregateSpikes() {
    // Count spikes from spike system
    pImpl->totalSpikesThisStep = pImpl->spikeSystem->getSpikeCount();
    pImpl->totalSpikesTotal = pImpl->spikeSystem->getTotalSpikeCount();
    
    // Update region spike counts
    for (auto& region : pImpl->regions) {
        region->updateStepSpikeCount(pImpl->spikeSystem.get());
    }
}

void Brain::applyHomeostaticScaling() {
    // Simple homeostatic scaling to maintain firing rate target
    float currentFiringRate = getAverageFiringRate();
    float error = pImpl->homeostaticTargetFiringRate - currentFiringRate;
    
    if (std::abs(error) > 0.01f) {
        float scalingFactor = 1.0f + pImpl->homeostaticAdaptationRate * error;
        scalingFactor = std::max(0.1f, std::min(scalingFactor, 10.0f));
        
        for (auto& region : pImpl->regions) {
            region->applyHomeostaticScaling(scalingFactor);
        }
    }
}

void Brain::applyLocalCompetition() {
    if (!pImpl->localCompetitionEnabled) return;
    
    // Apply lateral inhibition between neurons in each region
    for (auto& region : pImpl->regions) {
        region->applyLocalCompetition(pImpl->localCompetitionStrength);
    }
}

void Brain::updatePlasticity() {
    // Apply STDP plasticity based on spike timing
    pImpl->stdp->applyPlasticity(pImpl->spikeSystem.get(), pImpl->rng.get());
    
    // Apply Hebbian plasticity
    pImpl->hebbian->applyPlasticity(pImpl->spikeSystem.get(), pImpl->rng.get());
    
    // Apply structural plasticity (synaptogenesis/pruning)
    pImpl->structuralPlasticity->updateDevelopment(pImpl->developmentalStage);
    
    // Update development (synaptogenesis, pruning, maturation)
    develop();
}

void Brain::updateNeuromodulation() {
    // Apply neuromodulation based on current brain state
    Dopamine::Signal dopamineSignal(
        pImpl->dopamine->getLevel(),
        pImpl->curiosity->getLevel(),
        pImpl->predictionError->getError(),
        pImpl->novelty->getLevel()
    );
    
    pImpl->dopamine->update(pImpl->spikeSystem.get());
    pImpl->curiosity->update(pImpl->spikeSystem.get(), pImpl->predictionSystem.get());
    pImpl->predictionError->update(pImpl->predictionSystem.get());
    pImpl->novelty->update(pImpl->predictionSystem.get());
    
    pImpl->dopamine->applyToSystem(pImpl->spikeSystem.get());
}

void Brain::develop() {
    // Apply developmental processes
    pImpl->developmentSystem->develop(pImpl->spikeSystem.get(), pImpl->structuralPlasticity.get());
    
    // Apply synaptogenesis
    pImpl->developmentSystem->applySynaptogenesis(pImpl->spikeSystem.get(), pImpl->structuralPlasticity.get());
    
    // Apply pruning
    pImpl->developmentSystem->applyPruning(pImpl->spikeSystem.get(), pImpl->structuralPlasticity.get());
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
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage));
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

void Brain::setHomeostaticParameters(float targetFiringRate, float adaptationRate, float timeConstant) {
    pImpl->homeostaticTargetFiringRate = targetFiringRate;
    pImpl->homeostaticAdaptationRate = adaptationRate;
    pImpl->homeostaticTimeConstant = timeConstant;
    
    NLM_LOG_INFO("Homeostatic parameters set: target=" + std::to_string(targetFiringRate) + 
                " Hz, adaptation=" + std::to_string(adaptationRate) + ", timeConstant=" + std::to_string(timeConstant) + "s");
}

void Brain::setLocalCompetition(bool enabled, float strength) {
    pImpl->localCompetitionEnabled = enabled;
    pImpl->localCompetitionStrength = strength;
    pImpl->competitionBias.assign(pImpl->regions.size(), 0.0f);
    
    NLM_LOG_INFO("Local competition " + std::string(enabled ? "enabled" : "disabled") + 
                " with strength " + std::to_string(strength));
}

void Brain::setDevelopmentStage(DevelopmentalStage stage) {
    if (pImpl->developmentalStage != stage) {
        NLM_LOG_INFO("Development stage transitioning from " + 
                    std::to_string(static_cast<int>(pImpl->developmentalStage)) + 
                    " to " + std::to_string(static_cast<int>(stage)));
        
        pImpl->developmentalStage = stage;
        pImpl->developmentSystem->setStage(stage);
        
        // Trigger development responses
        develop();
    }
}

void Brain::enableCheckpointing(const std::string& path, size_t interval) {
    pImpl->checkpointPath = path;
    pImpl->checkpointRequested = true;
    pImpl->consolidationInterval = interval;
    
    NLM_LOG_INFO("Checkpointing enabled: path=" + path + ", interval=" + std::to_string(interval));
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    pImpl->interRegionConnections.push_back(InterRegionConnection(source, target, weight, delay));
    NLM_LOG_INFO("Added inter-region connection: " + std::to_string(source.index()) + 
                " -> " + std::to_string(target.index()) + " (weight=" + std::to_string(weight) + ")");
}

std::vector<RegionId> Brain::getConnectedRegions() const {
    std::vector<RegionId> connected;
    for (const auto& conn : pImpl->interRegionConnections) {
        if (std::find(connected.begin(), connected.end(), conn.targetRegion) == connected.end()) {
            connected.push_back(conn.targetRegion);
        }
    }
    return connected;
}

float Brain::getNetworkExcitability() const {
    float totalWeight = 0.0f;
    for (const auto& conn : pImpl->interRegionConnections) {
        totalWeight += std::abs(conn.weight);
    }
    return totalWeight / static_cast<float>(pImpl->interRegionConnections.size() + 1);
}

bool Brain::hasRegion(RegionId regionId) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == regionId) {
            return true;
        }
    }
    return false;
}

RegionId Brain::getRegionByName(const std::string& name) const {
    for (const auto& region : pImpl->regions) {
        if (region->getName() == name) {
            return region->getId();
        }
    }
    return RegionId(); // Invalid ID
}

std::vector<float> Brain::getRegionalActivityLevels() const {
    std::vector<float> activityLevels;
    activityLevels.reserve(pImpl->regions.size());
    
    for (const auto& region : pImpl->regions) {
        activityLevels.push_back(region->getAverageFiringRate());
    }
    
    return activityLevels;
}

void Brain::exportNetworkTopology() const {
    NLM_LOG_INFO("=== NLM Brain Topology ===");
    NLM_LOG_INFO("Network structure:")
    NLM_LOG_INFO("  Total regions: " + std::to_string(pImpl->regions.size()));
    NLM_LOG_INFO("  Inter-region connections: " + std::to_string(pImpl->interRegionConnections.size()));\n    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        NLM_LOG_INFO("  Region " + std::to_string(i) + ": " +
                    std::to_string(pImpl->regions[i]->getTotalNeuronCount()) + " neurons,") +
                    std::to_string(pImpl->regions[i]->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(pImpl->regions[i]->getAverageSynapticWeight()));
    }
    
    NLM_LOG_INFO("Network excitability: " + std::to_string(getNetworkExcitability()));
    NLM_LOG_INFO("Development stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
}

}} // namespace nlm