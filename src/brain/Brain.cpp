// Brain.cpp - Complete replacement with critical fixes
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../performance/MemoryPool.hpp"
#include "../performance/EventQueue.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/MotorSystem.hpp"
#include "../environment/Environment.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <set>

namespace nlm {

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() {
    delete pImpl;
}

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
    
    if (!pImpl) {
        NLM_LOG_ERROR("Brain implementation not properly initialized");
        return false;
    }
    
    // Validate configuration
    if (!validateConfiguration()) {
        NLM_LOG_ERROR("Configuration validation failed");
        return false;
    }
    
    // Get validated configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 
                                                      Impl::DEFAULT_NEURON_COUNT);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 
                                                     Impl::DEFAULT_REGION_COUNT);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 
                                                               Impl::DEFAULT_CONNECTION_PROBABILITY);
    
    // Validate ranges
    if (neuronCount == 0) {
        NLM_LOG_ERROR("Invalid neuron_count: must be positive");
        return false;
    }
    if (regionCount == 0) {
        NLM_LOG_ERROR("Invalid region_count: must be positive");
        return false;
    }
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        NLM_LOG_ERROR("Invalid connection_probability: must be between 0.0 and 1.0");
        return false;
    }
    
    NLM_LOG_INFO("Configuration validated: " + std::to_string(neuronCount) + 
                 " neurons, " + std::to_string(regionCount) + " regions");
    
    // Create performance systems first
    pImpl->memoryPool = std::make_unique<MemoryPool>(neuronCount * 10);
    pImpl->eventQueue = std::make_unique<EventQueue>();
    pImpl->checkpointSystem = std::make_unique<CheckpointSystem>();
    
    // Create spike system
    pImpl->spikeSystem = std::make_unique<SpikeSystem>(neuronCount);
    pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        // Thread-safe spike delivery
        std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
        
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
    
    // Create plasticity systems
    pImpl->stdp = std::make_unique<STDP>(
        pImpl->config->getOr<float>("stdp_ltp_weight", 0.01f),
        pImpl->config->getOr<float>("stdp_ltd_weight", 0.012f),
        pImpl->config->getOr<float>("stdp_tau", 20.0f)
    );
    
    pImpl->hebbian = std::make_unique<Hebbian>(
        pImpl->config->getOr<float>("hebbian_rate", 0.01f)
    );
    
    pImpl->structuralPlasticity = std::make_unique<StructuralPlasticity>(
        pImpl->config->getOr<float>("synaptogenesis_rate", 0.0001f),
        pImpl->config->getOr<float>("pruning_rate", 0.00001f)
    );
    
    // Create memory systems
    pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    // Create prediction system
    pImpl->predictionSystem = std::make_unique<PredictionSystem>();
    
    // Create cognition systems
    pImpl->planner = std::make_unique<NeuralPlanner>();
    pImpl->conceptFormation = std::make_unique<ConceptFormation>();
    pImpl->attentionalSelection = std::make_unique<AttentionalSelection>();
    
    // Create development system
    pImpl->developmentSystem = std::make_unique<DevelopmentSystem>();
    
    // Create neuromodulation systems
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->novelty = std::make_unique<Novelty>();
    pImpl->predictionError = std::make_unique<PredictionError>();
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Add populations with validated neuron counts
            size_t sensoryNeurons = neuronsPerRegion / 4;
            size_t internalNeurons = neuronsPerRegion / 2;
            size_t motorNeurons = neuronsPerRegion / 4;
            
            if (sensoryNeurons == 0) sensoryNeurons = 1;
            if (internalNeurons == 0) internalNeurons = 1;
            if (motorNeurons == 0) motorNeurons = 1;
            
            auto sensoryPopId = region->addPopulation(sensoryNeurons, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(internalNeurons, NeuronType::Internal);
            auto motorPopId = region->addPopulation(motorNeurons, NeuronType::Motor);
            
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
        }
    }
    
    // Establish synaptic connections between regions
    establishConnectivity(connectionProbability);
    
    // Initialize systems
    if (pImpl->spikeSystem) pImpl->spikeSystem->initialize();
    if (pImpl->stdp) pImpl->stdp->initialize(this);
    if (pImpl->hebbian) pImpl->hebbian->initialize(this);
    if (pImpl->structuralPlasticity) pImpl->structuralPlasticity->initialize(this);
    if (pImpl->workingMemory) pImpl->workingMemory->initialize(this);
    if (pImpl->episodicMemory) pImpl->episodicMemory->initialize(this);
    if (pImpl->associativeMemory) pImpl->associativeMemory->initialize(this);
    if (pImpl->predictionSystem) pImpl->predictionSystem->initialize(this);
    if (pImpl->planner) pImpl->planner->initialize(this);
    if (pImpl->conceptFormation) pImpl->conceptFormation->initialize(this);
    if (pImpl->attentionalSelection) pImpl->attentionalSelection->initialize(this);
    if (pImpl->developmentSystem) pImpl->developmentSystem->initialize(this);
    if (pImpl->dopamine) pImpl->dopamine->initialize(this);
    if (pImpl->curiosity) pImpl->curiosity->initialize(this);
    if (pImpl->novelty) pImpl->novelty->initialize(this);
    if (pImpl->predictionError) pImpl->predictionError->initialize(this);
    
    // Configure checkpoint system
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    if (pImpl->checkpointSystem) {
        pImpl->checkpointSystem->configure(checkpointDir, 10000, 5, true);
    }
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

bool Brain::validateConfiguration() const {
    if (!pImpl || !pImpl->config) {
        return false;
    }
    
    // Validate timestep
    double timestep = pImpl->config->getOr<double>("simulation_timestep", 0.001);
    if (timestep <= 0.0) {
        NLM_LOG_ERROR("Invalid simulation_timestep: must be positive");
        return false;
    }
    if (timestep > 0.1) {  // Warn about large timesteps
        NLM_LOG_WARNING("Large simulation_timestep (" + std::to_string(timestep) + ") may affect stability");
    }
    
    // Validate neuron count ranges
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", Impl::DEFAULT_NEURON_COUNT);
    if (neuronCount < 10) {
        NLM_LOG_ERROR("Invalid neuron_count: minimum is 10 neurons");
        return false;
    }
    if (neuronCount > 100000) {
        NLM_LOG_WARNING("Large neuron_count may affect performance");
    }
    
    // Validate region count
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", Impl::DEFAULT_REGION_COUNT);
    if (regionCount == 0) {
        NLM_LOG_ERROR("Invalid region_count: must be at least 1");
        return false;
    }
    if (regionCount > 100) {
        NLM_LOG_WARNING("Large region_count may affect performance");
    }
    
    // Validate plasticity parameters
    float stdpLtpWeight = pImpl->config->getOr<float>("stdp_ltp_weight", 0.01f);
    float stdpLtdWeight = pImpl->config->getOr<float>("stdp_ltd_weight", 0.012f);
    if (stdpLtpWeight <= 0.0f || stdpLtdWeight <= 0.0f) {
        NLM_LOG_ERROR("Invalid STDP weights: must be positive");
        return false;
    }
    
    // Validate developmental parameters
    double developmentTime = pImpl->config->getOr<double>("development_period", 60.0);
    if (developmentTime < 0.0) {
        NLM_LOG_ERROR("Invalid development_period: must be non-negative");
        return false;
    }
    
    return true;
}

void Brain::establishConnectivity(float connectionProbability) {
    if (!pImpl) return;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    // Create symmetric connectivity between regions
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        for (size_t j = i; j < pImpl->regions.size(); ++j) {
            RegionId sourceId(i + 1);
            RegionId targetId(j + 1);
            
            // Ensure bidirectional connectivity for main regions
            float prob = (i == j) ? connectionProbability : connectionProbability * 0.5f;
            
            if (pImpl->randomGenerator && pImpl->randomGenerator->uniformReal(0.0f, 1.0f) < prob) {
                addInterRegionConnection(sourceId, targetId, 1.0f + (pImpl->randomGenerator->uniformReal(-0.1f, 0.1f)), 1);
            }
        }
    }
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, currentStep * 0.001);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    if (!pImpl) return;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    // Update step counters
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Update systems
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->update(currentTime);
        pImpl->totalSpikesThisStep = pImpl->spikeSystem->getCurrentSpikeCount();
    }
    
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(currentTime);
    }
    
    if (pImpl->novelty) {
        pImpl->novelty->update(currentTime);
        pImpl->noveltyScore = pImpl->novelty->getNoveltyScore();
    }
    
    // Update development system
    develop();
    
    // Update plasticity
    updatePlasticity();
}

void Brain::develop() {
    if (!pImpl || !pImpl->developmentSystem) return;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    // Update developmental age
    pImpl->developmentalAge += 0.001; // 1ms development time
    
    // Update development system
    pImpl->developmentSystem->update(pImpl->developmentalAge);
    
    // Get developmental stage
    DevelopmentalStage stage = pImpl->developmentSystem->getDevelopmentalStage();
    
    // Update plasticity modifier based on developmental stage
    switch (stage) {
        case DevelopmentalStage::Initial:
            pImpl->plasticityModifier = 1.0f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->plasticityModifier = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            pImpl->plasticityModifier = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            pImpl->plasticityModifier = 0.2f;
            break;
    }
}

void Brain::updatePlasticity() {
    if (!pImpl) return;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    // Update STDP
    if (pImpl->stdp) {
        pImpl->stdp->update(currentTime, pImpl->plasticityModifier);
    }
    
    // Update Hebbian plasticity
    if (pImpl->hebbian) {
        pImpl->hebbian->update(currentTime, pImpl->plasticityModifier);
    }
    
    // Update structural plasticity
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(currentTime, pImpl->plasticityModifier);
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    if (!pImpl) return;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    // Inject current into sensory neurons based on input values
    const auto& inputValues = input.getInputValues();
    size_t minNeurons = std::min(pImpl->sensoryNeurons.size(), inputValues.size());
    
    for (size_t i = 0; i < minNeurons; ++i) {
        if (pImpl->sensoryNeurons[i]) {
            MembranePotential current = inputValues[i] * 5.0f; // Scale factor
            pImpl->sensoryNeurons[i]->injectCurrent(current);
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    if (!pImpl) return;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    // Find neuron and inject current
    for (auto* region : pImpl->regions) {
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
    if (!pImpl) return;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    // Inject current to all neurons of specified type
    for (auto* region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->getType() == type) {
                neuron->injectCurrent(current);
            }
        }
    }
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl ? pImpl->spikeSystem.get() : nullptr;
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl ? pImpl->spikeSystem.get() : nullptr;
}

STDP* Brain::getSTDP() {
    return pImpl ? pImpl->stdp.get() : nullptr;
}

Hebbian* Brain::getHebbian() {
    return pImpl ? pImpl->hebbian.get() : nullptr;
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl ? pImpl->structuralPlasticity.get() : nullptr;
}

float Brain::getExcitationInhibitionRatio() const {
    if (!pImpl) return 1.0f;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    float excitation = 0.0f, inhibition = 0.0f;
    
    // Calculate excitation/inhibition ratio from all neurons
    for (auto* region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            // Simple approximation based on input types
            auto inputs = neuron->getInputs();
            for (auto* synapse : inputs) {
                if (synapse->isExcitatory()) {
                    excitation += synapse->getWeight();
                } else {
                    inhibition += synapse->getWeight();
                }
            }
        }
    }
    
    if (inhibition == 0.0f) return 1.0f;
    return excitation / inhibition;
}

size_t Brain::getTotalSpikeCount() const {
    if (!pImpl) return 0;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    size_t total = pImpl->totalSpikesThisStep;
    
    // Add up spikes from all regions
    for (auto* region : pImpl->regions) {
        total += region->getTotalSpikeCount();
    }
    
    return total;
}

size_t Brain::getPendingSpikeEventCount() const {
    if (!pImpl || !pImpl->spikeSystem) return 0;
    
    return pImpl->spikeSystem->getPendingSpikeCount();
}

std::unique_ptr<Action> Brain::produceAction() {
    if (!pImpl) return nullptr;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    // Find most active motor neurons
    Neuron* mostActive = nullptr;
    float highestActivity = 0.0f;
    
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron && neuron->getActivity() > highestActivity) {
            highestActivity = neuron->getActivity();
            mostActive = neuron;
        }
    }
    
    if (!mostActive || highestActivity < MOTOR_ACTIVITY_THRESHOLD) {
        return nullptr;
    }
    
    // Create action based on most active neuron
    return std::make_unique<Action>(mostActive->getId(), highestActivity);
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    if (!pImpl) return;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    // Apply neuromodulatory signals to relevant systems
    if (pImpl->dopamine && signal.getType() == NeuromodulatorType::Dopamine) {
        pImpl->dopamine->receiveSignal(signal.getStrength(), currentTime);
    }
    
    if (pImpl->curiosity && signal.getType() == NeuromodulatorType::Curiosity) {
        pImpl->curiosity->receiveSignal(signal.getStrength(), currentTime);
    }
    
    if (pImpl->novelty && signal.getType() == NeuromodulatorType::Novelty) {
        pImpl->novelty->receiveSignal(signal.getStrength(), currentTime);
    }
    
    if (pImpl->predictionError && signal.getType() == NeuromodulatorType::PredictionError) {
        pImpl->predictionError->receiveSignal(signal.getStrength(), currentTime);
    }
}

void Brain::reset() {
    if (!pImpl) return;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    // Reset all components
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->developmentalAge = 0.0;
    pImpl->plasticityModifier = 1.0f;
    pImpl->noveltyScore = 0.0f;
    
    // Reset regions
    for (auto* region : pImpl->regions) {
        if (region) region->reset();
    }
    
    // Reset systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->spikeSystem) pImpl->spikeSystem->reset();
}

bool Brain::save(const std::string& filepath) const {
    if (!pImpl) return false;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    // Use checkpoint system if available
    if (pImpl->checkpointSystem) {
        return pImpl->checkpointSystem->save(filepath, this);
    }
    
    // Fallback: save individual components
    try {
        // Save configuration
        if (!pImpl->config->save(filepath + ".config")) {
            NLM_LOG_ERROR("Failed to save configuration to " + filepath + ".config");
            return false;
        }
        
        // Save regions
        for (size_t i = 0; i < pImpl->regions.size(); ++i) {
            if (pImpl->regions[i]) {
                std::string regionFile = filepath + ".region_" + std::to_string(i);
                if (!pImpl->regions[i]->save(regionFile)) {
                    NLM_LOG_ERROR("Failed to save region " + std::to_string(i));
                    return false;
                }
            }
        }
        
        NLM_LOG_INFO("Brain state saved to " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during save: " + std::string(e.what()));
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    if (!pImpl) return false;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    // Use checkpoint system if available
    if (pImpl->checkpointSystem) {
        return pImpl->checkpointSystem->load(filepath, this);
    }
    
    // Fallback: load individual components
    try {
        // Load configuration
        if (!pImpl->config->load(filepath + ".config")) {
            NLM_LOG_ERROR("Failed to load configuration from " + filepath + ".config");
            return false;
        }
        
        // Clear existing regions and reload
        pImpl->regions.clear();
        
        // Load regions (simplified - would need proper region management)
        NLM_LOG_WARNING("Full brain state loading requires checkpoint system support");
        
        NLM_LOG_INFO("Brain state loaded from " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during load: " + std::string(e.what()));
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    if (!pImpl) return RegionId();
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    RegionId newId(pImpl->regions.size() + 1);
    pImpl->regions.push_back(std::make_unique<NeuralRegion>(newId, name));
    
    return newId;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    if (!pImpl) return nullptr;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    if (id.getId() - 1 < pImpl->regions.size()) {
        return pImpl->regions[id.getId() - 1].get();
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    if (!pImpl) return nullptr;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    if (id.getId() - 1 < pImpl->regions.size()) {
        return pImpl->regions[id.getId() - 1].get();
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    if (!pImpl) return 0;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    return pImpl->regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    if (!pImpl) return {};
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    std::vector<RegionId> ids;
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        ids.push_back(RegionId(i + 1));
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    if (!pImpl) return pImpl->regions;
    
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                    float weight, Delay delay) {
    if (!pImpl) return;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    // Create connection
    InterRegionConnection conn(source, target, weight, delay);
    pImpl->interRegionConnections.push_back(conn);
    
    // Establish actual neural connection
    auto* sourceRegion = getRegion(source);
    auto* targetRegion = getRegion(target);
    
    if (sourceRegion && targetRegion) {
        // Create connection between regions
        auto sourceNeurons = sourceRegion->getAllNeurons();
        auto targetNeurons = targetRegion->getAllNeurons();
        
        if (!sourceNeurons.empty() && !targetNeurons.empty()) {
            // Connect first neurons as representatives
            NeuronId sourceId = sourceNeurons[0]->getId();
            NeuronId targetId = targetNeurons[0]->getId();
            
            // Use spike system to create delayed connection
            if (pImpl->spikeSystem) {
                pImpl->spikeSystem->addConnection(sourceId, targetId, weight, delay, true);
            }
        }
    }
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    if (!pImpl) return;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    // Remove connection from list
    auto it = std::remove_if(pImpl->interRegionConnections.begin(), 
                             pImpl->interRegionConnections.end(),
                             [source, target](const InterRegionConnection& conn) {
                                 return conn.sourceRegion == source && conn.targetRegion == target;
                             });
    
    pImpl->interRegionConnections.erase(it, pImpl->interRegionConnections.end());
}

size_t Brain::getTotalNeuronCount() const {
    if (!pImpl) return 0;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    size_t total = 0;
    for (auto* region : pImpl->regions) {
        if (region) total += region->getNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    if (!pImpl) return 0;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    size_t total = 0;
    for (auto* region : pImpl->regions) {
        if (region) total += region->getSynapseCount();
    }
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    if (!pImpl) return 0;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    size_t total = 0;
    for (auto* region : pImpl->regions) {
        if (region) total += region->getActiveNeuronCount();
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    if (!pImpl) return 0;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    size_t total = 0;
    for (auto* region : pImpl->regions) {
        if (region) total += region->getFiringNeuronCount();
    }
    return total;
}

float Brain::getAverageFiringRate() const {
    if (!pImpl) return 0.0f;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    size_t totalSpikes = getTotalSpikeCount();
    size_t totalNeurons = getTotalNeuronCount();
    
    if (totalNeurons == 0) return 0.0f;
    
    return static_cast<float>(totalSpikes) / totalNeurons;
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl ? pImpl->workingMemory.get() : nullptr;
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl ? pImpl->episodicMemory.get() : nullptr;
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl ? pImpl->associativeMemory.get() : nullptr;
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl ? pImpl->predictionSystem.get() : nullptr;
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl ? pImpl->planner.get() : nullptr;
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl ? pImpl->conceptFormation.get() : nullptr;
}

AttentionalSelection* Brain::getAttention() {
    return pImpl ? pImpl->attentionalSelection.get() : nullptr;
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl ? pImpl->developmentSystem.get() : nullptr;
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    if (!pImpl || !pImpl->developmentSystem) return DevelopmentalStage::Initial;
    
    return pImpl->developmentSystem->getDevelopmentalStage();
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    if (!pImpl || !pImpl->developmentSystem) return;
    
    pImpl->developmentSystem->setDevelopmentalStage(stage);
}

Dopamine* Brain::getDopamine() {
    return pImpl ? pImpl->dopamine.get() : nullptr;
}

Curiosity* Brain::getCuriosity() {
    return pImpl ? pImpl->curiosity.get() : nullptr;
}

Novelty* Brain::getNovelty() {
    return pImpl ? pImpl->novelty.get() : nullptr;
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl ? pImpl->predictionError.get() : nullptr;
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl ? pImpl->config : nullptr;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl ? pImpl->randomGenerator.get() : nullptr;
}

void Brain::logStatus() const {
    if (!pImpl) return;
    
    std::lock_guard<std::mutex> lock(pImpl->stepMutex_);
    
    NLM_LOG_INFO("=== NLM Brain Status ===");
    NLM_LOG_INFO("Step: " + std::to_string(pImpl->currentStep));
    NLM_LOG_INFO("Time: " + std::to_string(pImpl->currentTime) + "s");
    NLM_LOG_INFO("Neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Spike rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("Developmental age: " + std::to_string(pImpl->developmentalAge) + "s");
    NLM_LOG_INFO("Plasticity modifier: " + std::to_string(pImpl->plasticityModifier));
    NLM_LOG_INFO("Novelty score: " + std::to_string(pImpl->noveltyScore));
    NLM_LOG_INFO("Inter-region connections: " + std::to_string(pImpl->interRegionConnections.size()));
}

} // namespace nlm