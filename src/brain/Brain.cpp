#include "Brain.hpp"
#include "UnifiedMemorySystem.hpp"
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
    
    // ========== UNIFIED MEMORY SYSTEM ==========
    std::unique_ptr<MemorySystemManager> memorySystem;
    
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
        
        // Initialize memory system
        memorySystem = std::make_unique<MemorySystemManager>();
        
        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // ========== INITIALIZE INTEGRATED SYSTEMS ==========
        
        // Initialize unified memory system
        if (memorySystem) {
            memorySystem->initialize(this);
        }
        
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
        
        // Configure memory system
        if (memorySystem) {
            memorySystem->setReplayInterval(replayInterval);
            memorySystem->setConsolidationInterval(consolidationInterval);
        }
        
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
        }
    }
    
    NLM_LOG_INFO("Brain initialization complete. Created " + std::to_string(regionCount) + 
                 " regions with " + std::to_string(neuronCount) + " neurons total.");
    
    return true;
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, currentTime);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Update memory systems (if integrated)
    if (pImpl->memorySystem) {
        pImpl->memorySystem->updateAll(timestep);
        pImpl->memorySystem->consolidateAll();
    }
    
    // Update spike system (neural dynamics)
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->updateAll(timestep);
        pImpl->totalSpikesThisStep = pImpl->spikeSystem->getTotalSpikesThisStep();
    }
    
    // Update plasticity systems
    if (pImpl->stdp) pImpl->stdp->update(timestep);
    if (pImpl->hebbian) pImpl->hebbian->update(timestep);
    if (pImpl->structuralPlasticity) pImpl->structuralPlasticity->update(timestep);
    
    // Update neuromodulation systems
    if (pImpl->dopamine) pImpl->dopamine->update(timestep);
    if (pImpl->curiosity) pImpl->curiosity->update(timestep);
    if (pImpl->predictionError) pImpl->predictionError->update(timestep);
    if (pImpl->novelty) pImpl->novelty->update(timestep);
    
    // Apply neuromodulation effects
    if (pImpl->dopamine) {
        applyNeuromodulation(*pImpl->dopamine);
    }
    if (pImpl->curiosity) {
        applyNeuromodulation(*pImpl->curiosity);
    }
    if (pImpl->predictionError) {
        applyNeuromodulation(*pImpl->predictionError);
    }
    if (pImpl->novelty) {
        applyNeuromodulation(*pImpl->novelty);
    }
    
    // Update developmental processes
    develop();
    
    // Update memory system state
    if (pImpl->memorySystem) {
        pImpl->memorySystem->updateAll(timestep);
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input pattern
    // Simplified implementation - could be more sophisticated
    float inputCurrent = input.getCurrent();
    
    for (auto* neuron : pImpl->sensoryNeurons) {
        neuron->injectCurrent(inputCurrent);
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    // Find neuron and inject current
    for (auto* n : pImpl->sensoryNeurons) {
        if (n && n->getId() == neuron) {
            n->injectCurrent(current);
            break;
        }
    }
    
    for (auto* n : pImpl->motorNeurons) {
        if (n && n->getId() == neuron) {
            n->injectCurrent(current);
            break;
        }
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    std::vector<Neuron*> targetNeurons;
    
    if (type == NeuronType::Sensory) {
        targetNeurons = pImpl->sensoryNeurons;
    } else if (type == NeuronType::Motor) {
        targetNeurons = pImpl->motorNeurons;
    }
    
    for (auto* neuron : targetNeurons) {
        if (neuron) {
            neuron->injectCurrent(current);
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
    float totalExcitation = 0.0f;
    float totalInhibition = 0.0f;
    
    for (const auto& region : pImpl->regions) {
        totalExcitation += region->getExcitationCount();
        totalInhibition += region->getInhibitionCount();
    }
    
    if (totalInhibition == 0.0f) return totalExcitation;
    return totalExcitation / totalInhibition;
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem ? pImpl->spikeSystem->getPendingSpikeCount() : 0;
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Simplified action production based on motor neuron activity
    // Could be more sophisticated with actual action selection algorithms
    
    if (pImpl->motorNeurons.empty()) {
        return nullptr;
    }
    
    float maxActivation = 0.0f;
    NeuronId bestNeuron = 0;
    
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron && neuron->getActivation() > maxActivation) {
            maxActivation = neuron->getActivation();
            bestNeuron = neuron->getId();
        }
    }
    
    if (maxActivation > 0.5f) {
        return std::make_unique<Action>(ActionType::Move, bestNeuron, maxActivation);
    }
    
    return nullptr;
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulatory effects to plasticity and neural activity
    float level = signal.getLevel();
    
    if (signal.getName() == std::string("DA")) {
        // Dopamine effects
        // Modulate plasticity
        if (pImpl->stdp) {
            float plasticityFactor = signal.getPlasticityFactor();
            pImpl->stdp->setPlasticityFactor(plasticityFactor);
        }
        
        // Modulate firing rates
        for (auto* neuron : pImpl->sensoryNeurons) {
            if (neuron) {
                neuron->modulateFiringRate(level);
            }
        }
    } else if (signal.getName() == std::string("NE")) {
        // Norepinephrine effects
        // Enhance attention and arousal
        for (auto* neuron : pImpl->sensoryNeurons) {
            if (neuron) {
                neuron->modulateAttention(level);
            }
        }
    }
    
    // Update memory systems with neuromodulatory influence
    if (pImpl->memorySystem) {
        if (signal.getName() == std::string("DA")) {
            pImpl->memorySystem->getWorkingMemory()->integrateWithNeuromodulation(level);
        }
    }
}

void Brain::updatePlasticity() {
    // Update all plasticity systems
    if (pImpl->stdp) pImpl->stdp->update(timestep);
    if (pImpl->hebbian) pImpl->hebbian->update(timestep);
    if (pImpl->structuralPlasticity) pImpl->structuralPlasticity->update(timestep);
}

void Brain::develop() {
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(timestep);
    }
}

void Brain::reset() {
    // Reset all systems
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->reset();
    }
    
    if (pImpl->memorySystem) {
        pImpl->memorySystem->clearAll();
    }
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    NLM_LOG_INFO("Brain state reset completed");
}

bool Brain::save(const std::string& filepath) const {
    // Simplified checkpointing - would need more sophisticated implementation
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open file for saving brain state: " + filepath);
        return false;
    }
    
    // Serialize key brain state
    file << "Brain Checkpoint (Phase 6 - Integrated)" << std::endl;
    file << "Time Step: " << pImpl->currentStep << std::endl;
    file << "Total Spikes: " << pImpl->totalSpikesTotal << std::endl;
    file << "Regions: " << pImpl->regions.size() << std::endl;
    
    if (pImpl->memorySystem) {
        file << "Memory System: Active" << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Brain state saved to: " + filepath);
    return true;
}

bool Brain::load(const std::string& filepath) {
    // Simplified checkpoint loading - would need more sophisticated implementation
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open file for loading brain state: " + filepath);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("Time Step:") != std::string::npos) {
            // Parse time step
        } else if (line.find("Total Spikes:") != std::string::npos) {
            // Parse total spikes
        }
    }
    
    file.close();
    NLM_LOG_INFO("Brain state loaded from: " + filepath);
    return true;
}

RegionId Brain::addRegion(const std::string& name) {
    RegionId newId = pImpl->nextRegionId++;
    
    auto region = std::make_unique<NeuralRegion>(newId, name);
    NeuralRegion* regionPtr = region.get();
    pImpl->regions.push_back(std::move(region));
    
    return newId;
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
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                    float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    auto newEnd = std::remove_if(pImpl->interRegionConnections.begin(), 
                                pImpl->interRegionConnections.end(),
                                [source, target](const InterRegionConnection& conn) {
                                    return conn.sourceRegion == source && conn.targetRegion == target;
                                });
    pImpl->interRegionConnections.erase(newEnd, pImpl->interRegionConnections.end());
}

size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t active = 0;
    for (const auto& region : pImpl->regions) {
        active += region->getActiveNeuronCount();
    }
    return active;
}

size_t Brain::getFiringNeuronCount() const {
    size_t firing = 0;
    for (const auto& region : pImpl->regions) {
        firing += region->getFiringNeuronCount();
    }
    return firing;
}

float Brain::getAverageFiringRate() const {
    size_t totalSpikes = getTotalSpikeCount();
    size_t totalNeurons = getTotalNeuronCount();
    
    if (totalNeurons == 0) return 0.0f;
    return static_cast<float>(totalSpikes) / static_cast<float>(totalNeurons);
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->memorySystem ? pImpl->memorySystem->getWorkingMemory() : nullptr;
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->memorySystem ? pImpl->memorySystem->getEpisodicMemory() : nullptr;
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->memorySystem ? pImpl->memorySystem->getAssociativeMemory() : nullptr;
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

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
    if (pImpl->memorySystem) {
        pImpl->memorySystem->logStatus();
    }
    
    // Prediction system status
    if (pImpl->predictionSystem) {
        NLM_LOG_INFO("Prediction system: Active");
    }
    
    // Neuromodulation system status
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
}

} // namespace nlm
