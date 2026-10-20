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
    
    // Integration state
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Development state
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    
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
    
    // NEW: Brain coordinator for subsystem management
    std::unique_ptr<BrainCoordinator> brainCoordinator;
    
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
        , replayInterval(100)
        , consolidationInterval(1000) {
        
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
        
        // Create Brain coordinator
        brainCoordinator = std::make_unique<BrainCoordinator>(config);
    }
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
    
    // Initialize brain coordinator (which initializes all subsystems)
    if (!pImpl->brainCoordinator->initialize(this)) {
        NLM_LOG_ERROR("Failed to initialize BrainCoordinator");
        return false;
    }
    
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
    
    // Get timestep
    pImpl->timestep = pImpl->config->getOr<double>("simulation_timestep", 0.001);
    
    // Get integration intervals from config
    pImpl->replayInterval = pImpl->config->getOr<size_t>("replay_interval", 100);
    pImpl->consolidationInterval = pImpl->config->getOr<size_t>("consolidation_interval", 1000);
    
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
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Delegate to BrainCoordinator for integrated simulation
    pImpl->brainCoordinator->step(currentStep, currentTime);
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Delegate to environment interface in BrainCoordinator
    auto* envInterface = pImpl->brainCoordinator->getEnvironment();
    if (envInterface) {
        envInterface->processSensoryInput(input);
    }
    
    // Simple current injection for backward compatibility
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
        if (pImpl->brainCoordinator->getMemory() && normalizedValue > 0.5f) {
            auto* workingMemory = pImpl->brainCoordinator->getMemory()->getWorkingMemory();
            if (workingMemory) {
                workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
            }
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

// ========== SUBSYSTEM ACCESSORS (NEW) ==========

BrainCoordinator* Brain::getBrainCoordinator() {
    return pImpl->brainCoordinator.get();
}

const BrainCoordinator* Brain::getBrainCoordinator() const {
    return pImpl->brainCoordinator.get();
}

// ========== LEGACY SUBSYSTEM ACCESSORS (MAINTAINED FOR BACKWARD COMPATIBILITY) ==========

SpikeSystem* Brain::getSpikeSystem() {
    auto* coord = pImpl->brainCoordinator.get();
    if (coord) {
        auto* ncSystem = coord->getNeuralComputation();
        if (ncSystem) {
            return ncSystem->getSpikeSystem();
        }
    }
    return nullptr;
}

const SpikeSystem* Brain::getSpikeSystem() const {
    const auto* coord = pImpl->brainCoordinator.get();
    if (coord) {
        const auto* ncSystem = coord->getNeuralComputation();
        if (ncSystem) {
            return ncSystem->getSpikeSystem();
        }
    }
    return nullptr;
}

STDP* Brain::getSTDP() {
    auto* coord = pImpl->brainCoordinator.get();
    if (coord) {
        auto* plasticity = coord->getPlasticity();
        if (plasticity) {
            return plasticity->getSTDP();
        }
    }
    return nullptr;
}

Hebbian* Brain::getHebbian() {
    auto* coord = pImpl->brainCoordinator.get();
    if (coord) {
        auto* plasticity = coord->getPlasticity();
        if (plasticity) {
            return plasticity->getHebbian();
        }
    }
    return nullptr;
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    auto* coord = pImpl->brainCoordinator.get();
    if (coord) {
        auto* plasticity = coord->getPlasticity();
        if (plasticity) {
            return plasticity->getStructuralPlasticity();
        }
    }
    return nullptr;
}

float Brain::getExcitationInhibitionRatio() const {
    if (pImpl->brainCoordinator) {
        auto* ncSystem = pImpl->brainCoordinator->getNeuralComputation();
        if (ncSystem) {
            return ncSystem->getExcitationInhibitionRatio();
        }
    }
    return 0.0f;
}

size_t Brain::getTotalSpikeCount() const {
    if (pImpl->brainCoordinator) {
        auto* ncSystem = pImpl->brainCoordinator->getNeuralComputation();
        if (ncSystem) {
            return ncSystem->getTotalSpikeCount();
        }
    }
    return 0;
}

size_t Brain::getPendingSpikeEventCount() const {
    if (pImpl->brainCoordinator) {
        auto* ncSystem = pImpl->brainCoordinator->getNeuralComputation();
        if (ncSystem) {
            return ncSystem->getPendingSpikeEventCount();
        }
    }
    return 0;
}

std::unique_ptr<class Action> Brain::produceAction() {
    auto* envInterface = pImpl->brainCoordinator->getEnvironment();
    if (envInterface) {
        return envInterface->produceAction();
    }
    return std::make_unique<Action>(ActionType::Wait);
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    auto* neuromodulation = pImpl->brainCoordinator->getNeuromodulation();
    if (neuromodulation) {
        neuromodulation->applyNeuromodulationToBrain();
    }
}

void Brain::updatePlasticity() {
    auto* plasticity = pImpl->brainCoordinator->getPlasticity();
    if (plasticity) {
        plasticity->update(pImpl->timestep);
    }
}

void Brain::develop() {
    auto* development = pImpl->brainCoordinator->getDevelopment();
    if (development) {
        development->update(pImpl->timestep);
    }
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    // Reset all regions
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    // Reset brain coordinator
    if (pImpl->brainCoordinator) {
        pImpl->brainCoordinator->reset();
    }
    
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        // Save through BrainCoordinator for integrated state
        if (pImpl->brainCoordinator) {
            return pImpl->brainCoordinator->save(filepath);
        }
        
        // Fallback to old-style checkpointing
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
        // Load through BrainCoordinator if available
        if (pImpl->brainCoordinator) {
            return pImpl->brainCoordinator->load(filepath);
        }
        
        // Fallback to old-style checkpointing
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // For now, just log that we would load the checkpoint
        NLM_LOG_INFO("Brain checkpoint loading not yet implemented in refactored architecture");
        
        NLM_LOG_INFO("Brain state loading attempted");
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
    if (pImpl->brainCoordinator) {
        auto* memory = pImpl->brainCoordinator->getMemory();
        if (memory) {
            return memory->getWorkingMemory();
        }
    }
    return nullptr;
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    if (pImpl->brainCoordinator) {
        auto* memory = pImpl->brainCoordinator->getMemory();
        if (memory) {
            // This would need a getEpisodicMemory() method in MemorySystems
            // For now, return nullptr
            return nullptr;
        }
    }
    return nullptr;
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    if (pImpl->brainCoordinator) {
        auto* memory = pImpl->brainCoordinator->getMemory();
        if (memory) {
            // This would need a getAssociativeMemory() method in MemorySystems
            // For now, return nullptr
            return nullptr;
        }
    }
    return nullptr;
}

// ========== PREDICTION SYSTEM ACCESSOR ==========

PredictionSystem* Brain::getPredictionSystem() {
    if (pImpl->brainCoordinator) {
        auto* cognition = pImpl->brainCoordinator->getCognition();
        if (cognition) {
            // This would need a getPredictionSystem() method in CognitionSystem
            // For now, return nullptr
            return nullptr;
        }
    }
    return nullptr;
}

// ========== COGNITION SYSTEM ACCESSORS ==========

NeuralPlanner* Brain::getPlanner() {
    if (pImpl->brainCoordinator) {
        auto* cognition = pImpl->brainCoordinator->getCognition();
        if (cognition) {
            // This would need a getPlanner() method in CognitionSystem
            // For now, return nullptr
            return nullptr;
        }
    }
    return nullptr;
}

ConceptFormation* Brain::getConceptFormation() {
    if (pImpl->brainCoordinator) {
        auto* cognition = pImpl->brainCoordinator->getCognition();
        if (cognition) {
            // This would need a getConceptFormation() method in CognitionSystem
            // For now, return nullptr
            return nullptr;
        }
    }
    return nullptr;
}

AttentionalSelection* Brain::getAttention() {
    if (pImpl->brainCoordinator) {
        auto* cognition = pImpl->brainCoordinator->getCognition();
        if (cognition) {
            // This would need a getAttention() method in CognitionSystem
            // For now, return nullptr
            return nullptr;
        }
    }
    return nullptr;
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem* Brain::getDevelopmentSystem() {
    if (pImpl->brainCoordinator) {
        return pImpl->brainCoordinator->getDevelopment();
    }
    return nullptr;
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

// ========== NEUROMODULATION SYSTEMS ==========

Dopamine* Brain::getDopamine() {
    if (pImpl->brainCoordinator) {
        auto* neuromodulation = pImpl->brainCoordinator->getNeuromodulation();
        if (neuromodulation) {
            return neuromodulation->getDopamine();
        }
    }
    return nullptr;
}

Curiosity* Brain::getCuriosity() {
    if (pImpl->brainCoordinator) {
        auto* neuromodulation = pImpl->brainCoordinator->getNeuromodulation();
        if (neuromodulation) {
            return neuromodulation->getCuriosity();
        }
    }
    return nullptr;
}

Novelty* Brain::getNovelty() {
    if (pImpl->brainCoordinator) {
        auto* neuromodulation = pImpl->brainCoordinator->getNeuromodulation();
        if (neuromodulation) {
            return neuromodulation->getNovelty();
        }
    }
    return nullptr;
}

PredictionError* Brain::getPredictionErrorSignal() {
    if (pImpl->brainCoordinator) {
        auto* neuromodulation = pImpl->brainCoordinator->getNeuromodulation();
        if (neuromodulation) {
            return neuromodulation->getPredictionError();
        }
    }
    return nullptr;
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Refactored) ===");
    
    // Log through BrainCoordinator
    if (pImpl->brainCoordinator) {
        pImpl->brainCoordinator->logStatus();
    } else {
        NLM_LOG_INFO("BrainCoordinator not initialized");
    }
}

} // namespace nlm
