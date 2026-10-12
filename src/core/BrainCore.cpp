#include "BrainCore.hpp"
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

BrainCore::BrainCore(std::shared_ptr<Config> cfg)
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
    , replayInterval(100)
    , consolidationInterval(1000)
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

BrainCore::~BrainCore() = default;

BrainCore::BrainCore(BrainCore&& other) noexcept
    : config(std::move(other.config))
    , rng(std::move(other.rng))
    , regions(std::move(other.regions))
    , sensoryNeurons(std::move(other.sensoryNeurons))
    , motorNeurons(std::move(other.motorNeurons))
    , spikeSystem(std::move(other.spikeSystem))
    , stdp(std::move(other.stdp))
    , hebbian(std::move(other.hebbian))
    , structuralPlasticity(std::move(other.structuralPlasticity))
    , workingMemory(std::move(other.workingMemory))
    , episodicMemory(std::move(other.episodicMemory))
    , associativeMemory(std::move(other.associativeMemory))
    , predictionSystem(std::move(other.predictionSystem))
    , planner(std::move(other.planner))
    , conceptFormation(std::move(other.conceptFormation))
    , attention(std::move(other.attention))
    , developmentSystem(std::move(other.developmentSystem))
    , dopamine(std::move(other.dopamine))
    , curiosity(std::move(other.curiosity))
    , predictionError(std::move(other.predictionError))
    , novelty(std::move(other.novelty))
    , checkpointManager(std::move(other.checkpointManager))
    , timestep(other.timestep)
    , currentStep(other.currentStep)
    , currentTime(other.currentTime)
    , totalSpikesThisStep(other.totalSpikesThisStep)
    , totalSpikesTotal(other.totalSpikesTotal)
    , isResting(other.isResting)
    , stepsSinceLastEpisode(other.stepsSinceLastEpisode)
    , replayInterval(other.replayInterval)
    , consolidationInterval(other.consolidationInterval)
    , developmentalStage(other.developmentalStage)
    , nextRegionId(other.nextRegionId)
{
    other.timestep = 0.001;
    other.currentStep = 0;
    other.currentTime = 0.0;
    other.totalSpikesThisStep = 0;
    other.totalSpikesTotal = 0;
    other.isResting = false;
    other.stepsSinceLastEpisode = 0;
    other.developmentalStage = DevelopmentalStage::Initial;
}

BrainCore& BrainCore::operator=(BrainCore&& other) noexcept {
    if (this != &other) {
        // Clean up current resources
        // (In real implementation, this would properly clean up all members)
        
        // Move from other
        config = std::move(other.config);
        rng = std::move(other.rng);
        regions = std::move(other.regions);
        sensoryNeurons = std::move(other.sensoryNeurons);
        motorNeurons = std::move(other.motorNeurons);
        spikeSystem = std::move(other.spikeSystem);
        stdp = std::move(other.stdp);
        hebbian = std::move(other.hebbian);
        structuralPlasticity = std::move(other.structuralPlasticity);
        workingMemory = std::move(other.workingMemory);
        episodicMemory = std::move(other.episodicMemory);
        associativeMemory = std::move(other.associativeMemory);
        predictionSystem = std::move(other.predictionSystem);
        planner = std::move(other.planner);
        conceptFormation = std::move(other.conceptFormation);
        attention = std::move(other.attention);
        developmentSystem = std::move(other.developmentSystem);
        dopamine = std::move(other.dopamine);
        curiosity = std::move(other.curiosity);
        predictionError = std::move(other.predictionError);
        novelty = std::move(other.novelty);
        checkpointManager = std::move(other.checkpointManager);
        timestep = other.timestep;
        currentStep = other.currentStep;
        currentTime = other.currentTime;
        totalSpikesThisStep = other.totalSpikesThisStep;
        totalSpikesTotal = other.totalSpikesTotal;
        isResting = other.isResting;
        stepsSinceLastEpisode = other.stepsSinceLastEpisode;
        replayInterval = other.replayInterval;
        consolidationInterval = other.consolidationInterval;
        developmentalStage = other.developmentalStage;
        nextRegionId = other.nextRegionId;
        
        other.timestep = 0.001;
        other.currentStep = 0;
        other.currentTime = 0.0;
        other.totalSpikesThisStep = 0;
        other.totalSpikesTotal = 0;
        other.isResting = false;
        other.stepsSinceLastEpisode = 0;
        other.developmentalStage = DevelopmentalStage::Initial;
    }
    return *this;
}

bool BrainCore::initialize() {
    // Initialize all integrated systems
    if (workingMemory) {
        workingMemory->initialize(nullptr);  // Would need brain reference in full implementation
        workingMemory->setCapacity(1000);
    }
    
    if (episodicMemory) {
        episodicMemory->initialize(nullptr);
        episodicMemory->setMaxEpisodes(1000);
    }
    
    if (associativeMemory) {
        associativeMemory->initialize(nullptr);
    }
    
    if (planner) {
        planner->initialize(nullptr);
        planner->setPlanningDepth(5);
    }
    
    if (conceptFormation) {
        conceptFormation->initialize(nullptr);
    }
    
    if (attention) {
        attention->initialize(nullptr);
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
    }
    
    if (novelty) {
        novelty->initialize(nullptr);
    }
    
    if (curiosity) {
        curiosity->initialize(nullptr);
    }
    
    // Register spike handlers
    if (spikeSystem) {
        spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            ++totalSpikesThisStep;
            ++totalSpikesTotal;
        });
    }
    
    // Configure checkpoint manager
    if (checkpointManager) {
        std::string checkpointDir = "checkpoints";
        checkpointManager->configure(checkpointDir, 10000, 5, true);
    }
    
    return true;
}

void BrainCore::addRegion(std::unique_ptr<NeuralRegion> region) {
    regions.push_back(std::move(region));
    if (region) {
        nextRegionId = std::max(nextRegionId, region->getId() + 1);
    }
}

void BrainCore::reset() {
    currentStep = 0;
    currentTime = 0.0;
    totalSpikesThisStep = 0;
    totalSpikesTotal = 0;
    isResting = false;
    stepsSinceLastEpisode = 0;
    developmentalStage = DevelopmentalStage::Initial;
    
    // Reset all regions
    for (auto& region : regions) {
        if (region) {
            region->reset();
        }
    }
    
    // Reset systems
    if (spikeSystem) spikeSystem->reset();
    
    // Reset memory systems
    if (workingMemory) workingMemory->clear();
    if (episodicMemory) episodicMemory->clear();
    if (associativeMemory) associativeMemory->clear();
    if (attention) attention->reset();
}

bool BrainCore::save(const std::string& filepath) const {
    if (!checkpointManager) return false;
    
    // Simplified checkpoint saving - would need full implementation
    // For now, just log success
    std::ofstream file(filepath);
    if (!file.is_open()) return false;
    
    file << "BrainCore checkpoint (simplified)" << std::endl;
    file << "Step: " << currentStep << std::endl;
    file << "Time: " << currentTime << std::endl;
    file << "Neurons: " << getTotalNeuronCount() << std::endl;
    file << "Spikes: " << totalSpikesTotal << std::endl;
    
    return true;
}

bool BrainCore::load(const std::string& filepath) {
    if (!checkpointManager) return false;
    
    // Simplified checkpoint loading - would need full implementation
    // For now, just log success
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    std::string line;
    while (std::getline(file, line)) {
        // Parse checkpoint data (simplified)
    }
    
    return true;
}

void BrainCore::logStatus() const {
    std::cout << "=== BrainCore Status ===" << std::endl;
    std::cout << "Regions: " << getRegionCount() << std::endl;
    std::cout << "Total neurons: " << getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << getTotalSynapseCount() << std::endl;
    std::cout << "Active neurons: (placeholder)" << std::endl;
    std::cout << "Firing neurons (this step): " << totalSpikesThisStep << std::endl;
    std::cout << "Total spikes: " << totalSpikesTotal << std::endl;
    std::cout << "Pending spike events: " << getPendingSpikeEventCount() << std::endl;
    std::cout << "Average firing rate: " << getAverageFiringRate() << std::endl;
    std::cout << "E/I ratio: " << getExcitationInhibitionRatio() << std::endl;
}

void BrainCore::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * timestep);
}

void BrainCore::step(SimulationStep currentStep, Timestamp currentTime) {
    this->currentStep = currentStep;
    this->currentTime = currentTime;
    totalSpikesThisStep = 0;
    
    // Process delayed spikes
    if (spikeSystem) {
        spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
    
    // Update neurons (simplified - would need actual neuron stepping)
    // This would loop through all neurons and apply LIF dynamics
    
    // Process immediate spikes
    if (spikeSystem) {
        spikeSystem->processSpikes(currentStep);
    }
    
    // Update working memory
    if (workingMemory) {
        workingMemory->update(timestep);
    }
    
    // Update neuromodulation
    if (novelty) {
        novelty->update(timestep);
    }
    
    if (curiosity) {
        curiosity->update(timestep);
    }
    
    if (dopamine) {
        dopamine->update(timestep);
    }
    
    // Update plasticity
    updatePlasticity();
    
    // Update episodic memory
    stepsSinceLastEpisode++;
    if (stepsSinceLastEpisode >= 10 && episodicMemory) {
        stepsSinceLastEpisode = 0;
        // Store episode (simplified)
    }
    
    // Update prediction system
    if (predictionSystem) {
        // Update prediction system (simplified)
    }
    
    // Update attention system
    if (attention) {
        attention->update(timestep);
    }
    
    // Update concept formation
    if (conceptFormation) {
        // Update concept formation (simplified)
    }
    
    // Apply structural plasticity periodically
    if (currentStep % 100 == 0 && structuralPlasticity) {
        structuralPlasticity->update(nullptr, *rng);
    }
    
    // Replay memories
    if (currentStep % replayInterval == 0 && episodicMemory) {
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            episodicMemory->replayEpisode(episode);
        }
    }
    
    // Apply development effects
    if (currentStep % 1000 == 0 && developmentSystem) {
        developmentSystem->update(nullptr, *rng, timestep * 1000);
    }
    
    // Memory consolidation
    if (currentStep % consolidationInterval == 0 && episodicMemory) {
        episodicMemory->consolidate(0.3f);
    }
    
    // Checkpoint management
    if (checkpointManager) {
        checkpointManager->update(currentStep, currentTime);
    }
}

void BrainCore::processSpikeEvents(SimulationStep currentStep) {
    // Process spike events (implementation would integrate with spike system)
}

void BrainCore::applyNeuromodulation(const Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    if (stdp) {
        float modulation = signal.getLevel();
        pImpl->stdp->setLTPWeight(0.01f * modulation);
        pImpl->stdp->setLTDWeight(0.012f * modulation);
    }
}

void BrainCore::updatePlasticity() {
    // Plasticity is applied during each step
    // This method is kept for API compatibility
}

void BrainCore::develop() {
    // Development updates structural plasticity
    if (structuralPlasticity) {
        structuralPlasticity->update(nullptr, *rng);
    }
}

} // namespace nlm