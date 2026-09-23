#include "BrainIntegration.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// ===========================================
// BRAIN CONSTRUCTION AND LIFECYCLE
// ===========================================

void BrainIntegration::constructBrain(Brain* brain, std::shared_ptr<Config> config) {
    if (!brain || !config) return;

    new (brain) Brain(config);
}

void BrainIntegration::destructBrain(Brain* brain) {
    if (brain) {
        brain->~Brain();
    }
}

void BrainIntegration::initializeBrain(Brain* brain) {
    if (!brain) return;

    brain->initialize();
}

void BrainIntegration::resetBrain(Brain* brain) {
    if (!brain) return;

    brain->reset();
}

// ===========================================
// SENSORY INPUT AND MOTOR OUTPUT
// ===========================================

void BrainIntegration::receiveSensoryInput(Brain* brain, const SensoryInput& input) {
    if (!brain) return;

    brain->receiveSensoryInput(input);
}

void BrainIntegration::injectCurrent(Brain* brain, NeuronId neuron, MembranePotential current) {
    if (!brain) return;

    brain->injectCurrent(neuron, current);
}

void BrainIntegration::injectCurrentToNeurons(Brain* brain, NeuronType type, MembranePotential current) {
    if (!brain) return;

    brain->injectCurrentToNeurons(type, current);
}

void BrainIntegration::processSensoryInput(Brain* brain, const SensoryPercept& percept) {
    if (!brain) return;

    // This would be called by AgentBrain to process sensory percepts
    // Brain would extract sensory input from the percept
    auto sensoryInput = percept.toSensoryInput();
    brain->receiveSensoryInput(sensoryInput);
}

// ===========================================
// ACCESSORS AND STATISTICS
// ===========================================

void BrainIntegration::logBrainStatus(Brain* brain) {
    if (!brain) return;

    brain->logStatus();
}

// ===========================================
// NEURAL SYSTEM ACCESSORS
// ===========================================

SpikeSystem* BrainIntegration::getSpikeSystem(Brain* brain) {
    if (!brain) return nullptr;
    return brain->getSpikeSystem();
}

STDP* BrainIntegration::getSTDP(Brain* brain) {
    if (!brain) return nullptr;
    return brain->getSTDP();
}

Hebbian* BrainIntegration::getHebbian(Brain* brain) {
    if (!brain) return nullptr;
    return brain->getHebbian();
}

StructuralPlasticity* BrainIntegration::getStructuralPlasticity(Brain* brain) {
    if (!brain) return nullptr;
    return brain->getStructuralPlasticity();
}

// ===========================================
// MEMORY SYSTEM ACCESSORS
// ===========================================

NeuralWorkingMemory* BrainIntegration::getWorkingMemory(Brain* brain) {
    if (!brain) return nullptr;
    return brain->getWorkingMemory();
}

NeuralEpisodicMemory* BrainIntegration::getEpisodicMemory(Brain* brain) {
    if (!brain) return nullptr;
    return brain->getEpisodicMemory();
}

NeuralAssociativeMemory* BrainIntegration::getAssociativeMemory(Brain* brain) {
    if (!brain) return nullptr;
    return brain->getAssociativeMemory();
}

// ===========================================
// PREDICTION SYSTEM ACCESSOR
// ===========================================

PredictionSystem* BrainIntegration::getPredictionSystem(Brain* brain) {
    if (!brain) return nullptr;
    return brain->getPredictionSystem();
}

// ===========================================
// COGNITION SYSTEM ACCESSORS
// ===========================================

NeuralPlanner* BrainIntegration::getPlanner(Brain* brain) {
    if (!brain) return nullptr;
    return brain->getPlanner();
}

ConceptFormation* BrainIntegration::getConceptFormation(Brain* brain) {
    if (!brain) return nullptr;
    return brain->getConceptFormation();
}

AttentionalSelection* BrainIntegration::getAttention(Brain* brain) {
    if (!brain) return nullptr;
    return brain->getAttention();
}

// ===========================================
// DEVELOPMENT SYSTEM ACCESSORS
// ===========================================

DevelopmentSystem* BrainIntegration::getDevelopmentSystem(Brain* brain) {
    if (!brain) return nullptr;
    return brain->getDevelopmentSystem();
}

DevelopmentalStage BrainIntegration::getDevelopmentalStage(Brain* brain) {
    if (!brain) return DevelopmentalStage::Initial;
    return brain->getDevelopmentalStage();
}

// ===========================================
// NEUROMODULATION SYSTEM ACCESSORS
// ===========================================

Dopamine* BrainIntegration::getDopamine(Brain* brain) {
    if (!brain) return nullptr;
    return brain->getDopamine();
}

Curiosity* BrainIntegration::getCuriosity(Brain* brain) {
    if (!brain) return nullptr;
    return brain->getCuriosity();
}

Novelty* BrainIntegration::getNovelty(Brain* brain) {
    if (!brain) return nullptr;
    return brain->getNovelty();
}

PredictionError* BrainIntegration::getPredictionError(Brain* brain) {
    if (!brain) return nullptr;
    return brain->getPredictionErrorSignal();
}

// ===========================================
// CONFIGURATION AND UTILITIES
// ===========================================

std::shared_ptr<const Config> BrainIntegration::getConfig(Brain* brain) {
    if (!brain) return nullptr;
    return brain->getConfig();
}

RandomGenerator* BrainIntegration::getRandomGenerator(Brain* brain) {
    if (!brain) return nullptr;
    return brain->getRandomGenerator();
}

// ===========================================
// ACTION PRODUCTION AND MOTOR DECODING
// ===========================================

std::unique_ptr<Action> BrainIntegration::produceAction(Brain* brain) {
    if (!brain) return nullptr;
    return brain->produceAction();
}

void BrainIntegration::applyNeuromodulation(Brain* brain, const Neuromodulator& signal) {
    if (!brain) return;

    brain->applyNeuromodulation(signal);
}

void BrainIntegration::updatePlasticity(Brain* brain) {
    if (!brain) return;

    brain->updatePlasticity();
}

void BrainIntegration::develop(Brain* brain) {
    if (!brain) return;

    brain->develop();
}

// ===========================================
// STATE PERSISTENCE
// ===========================================

bool BrainIntegration::saveBrainState(Brain* brain, const std::string& filepath) {
    if (!brain) return false;
    return brain->save(filepath);
}

bool BrainIntegration::loadBrainState(Brain* brain, const std::string& filepath) {
    if (!brain) return false;
    return brain->load(filepath);
}

// ===========================================
// NEURAL NETWORK STATISTICS
// ===========================================

size_t BrainIntegration::getTotalNeuronCount(Brain* brain) {
    if (!brain) return 0;
    return brain->getTotalNeuronCount();
}

size_t BrainIntegration::getTotalSynapseCount(Brain* brain) {
    if (!brain) return 0;
    return brain->getTotalSynapseCount();
}

size_t BrainIntegration::getActiveNeuronCount(Brain* brain) {
    if (!brain) return 0;
    return brain->getActiveNeuronCount();
}

size_t BrainIntegration::getFiringNeuronCount(Brain* brain) {
    if (!brain) return 0;
    return brain->getFiringNeuronCount();
}

float BrainIntegration::getAverageFiringRate(Brain* brain) {
    if (!brain) return 0.0f;
    return brain->getAverageFiringRate();
}

float BrainIntegration::getExcitationInhibitionRatio(Brain* brain) {
    if (!brain) return 0.0f;
    return brain->getExcitationInhibitionRatio();
}

size_t BrainIntegration::getTotalSpikeCount(Brain* brain) {
    if (!brain) return 0;
    return brain->getTotalSpikeCount();
}

size_t BrainIntegration::getPendingSpikeEventCount(Brain* brain) {
    if (!brain) return 0;
    return brain->getPendingSpikeEventCount();
}

// ===========================================
// REGION MANAGEMENT
// ===========================================

RegionId BrainIntegration::addRegion(Brain* brain, const std::string& name) {
    if (!brain) return RegionId();
    return brain->addRegion(name);
}

NeuralRegion* BrainIntegration::getRegion(Brain* brain, RegionId id) {
    if (!brain) return nullptr;
    return brain->getRegion(id);
}

size_t BrainIntegration::getRegionCount(Brain* brain) {
    if (!brain) return 0;
    return brain->getRegionCount();
}

std::vector<RegionId> BrainIntegration::getRegionIds(Brain* brain) {
    if (!brain) return {};
    return brain->getRegionIds();
}

const std::vector<std::unique_ptr<NeuralRegion>>& BrainIntegration::getRegions(Brain* brain) {
    static std::vector<std::unique_ptr<NeuralRegion>> empty;
    if (!brain) return empty;
    return brain->getRegions();
}

} // namespace nlm
