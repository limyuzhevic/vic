#include "BrainConstructor.cpp"
#include "BrainSystemSetup.cpp"
#include "BrainStepCore.cpp"
#include "BrainIntegration.cpp"
#include "BrainEpisodicMemory.cpp"
#include "BrainUtils.cpp"

namespace nlm {

// Brain constructor - now simple, delegates to BrainConstructor.cpp
Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

// Destructor
Brain::~Brain() = default;

// Move constructor
Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

// Move assignment operator
Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool Brain::initialize() {
    return initializeSystems();
}

void Brain::step(SimulationStep currentStep) {
    stepCore(currentStep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    stepIntegration(currentStep, currentTime);
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    receiveSensoryInputInternal(input);
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    injectCurrentInternal(neuron, current);
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    injectCurrentToNeuronsInternal(type, current);
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
    return getExcitationInhibitionRatioInternal();
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return getPendingSpikeEventCountInternal();
}

std::unique_ptr<class Action> Brain::produceAction() {
    return produceActionInternal();
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    applyNeuromodulationInternal(signal);
}

void Brain::updatePlasticity() {
    updatePlasticityInternal();
}

void Brain::develop() {
    developInternal();
}

void Brain::reset() {
    resetInternal();
}

bool Brain::save(const std::string& filepath) const {
    return saveInternal(filepath);
}

bool Brain::load(const std::string& filepath) {
    return loadInternal(filepath);
}

RegionId Brain::addRegion(const std::string& name) {
    return addRegionInternal(name);
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return getRegionInternal(id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return getRegionInternal(id);
}

size_t Brain::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    return getRegionIdsInternal();
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                      float weight, Delay delay) {
    addInterRegionConnectionInternal(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    removeInterRegionConnectionInternal(source, target);
}

size_t Brain::getTotalNeuronCount() const {
    return getTotalNeuronCountInternal();
}

size_t Brain::getTotalSynapseCount() const {
    return getTotalSynapseCountInternal();
}

size_t Brain::getActiveNeuronCount() const {
    return getActiveNeuronCountInternal();
}

size_t Brain::getFiringNeuronCount() const {
    return getFiringNeuronCountInternal();
}

float Brain::getAverageFiringRate() const {
    return getAverageFiringRateInternal();
}

// ========== MEMORY SYSTEMS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

// ========== PREDICTION SYSTEM ==========

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

// ========== COGNITION SYSTEMS ==========

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
    logStatusInternal();
}

} // namespace nlm