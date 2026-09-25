#include "Brain.hpp"
#include "BrainCore.cpp"
#include "BrainMemory.cpp"
#include "BrainPrediction.cpp"
#include "BrainCognition.cpp"
#include "BrainNeuromodulation.cpp"
#include "BrainPlasticity.cpp"
#include "BrainIO.cpp"
#include "BrainPersistence.cpp"

namespace nlm {

// Thin wrapper implementation that delegates to specialized subsystem files
// This allows maintaining API compatibility while improving code organization

// Constructor and destructor - delegate to BrainCore
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

// Initialize brain with configuration - delegate to BrainCore
void Brain::initialize() {
    return pImpl->BrainCore::initialize();
}

// Main simulation step - delegate to BrainCore
void Brain::step(SimulationStep currentStep) {
    return pImpl->BrainCore::step(currentStep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    return pImpl->BrainCore::step(currentStep, currentTime);
}

// I/O methods - delegate to BrainIO
void Brain::receiveSensoryInput(const class SensoryInput& input) {
    return pImpl->BrainIO::receiveSensoryInput(input);
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    return pImpl->BrainIO::injectCurrent(neuron, current);
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    return pImpl->BrainIO::injectCurrentToNeurons(type, current);
}

// Spike system access - delegate to BrainPlasticity
SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->BrainPlasticity::getSpikeSystem();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->BrainPlasticity::getSpikeSystem();
}

// Plasticity system access - delegate to BrainPlasticity
STDP* Brain::getSTDP() {
    return pImpl->BrainPlasticity::getSTDP();
}

Hebbian* Brain::getHebbian() {
    return pImpl->BrainPlasticity::getHebbian();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->BrainPlasticity::getStructuralPlasticity();
}

// Statistics - delegate to BrainCore
float Brain::getExcitationInhibitionRatio() const {
    return pImpl->BrainCore::getExcitationInhibitionRatio();
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->BrainCore::getTotalSpikeCount();
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->BrainCore::getPendingSpikeEventCount();
}

// Action production - delegate to BrainIO
std::unique_ptr<class Action> Brain::produceAction() {
    return pImpl->BrainIO::produceAction();
}

// Neuromodulation - delegate to BrainNeuromodulation
void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    return pImpl->BrainNeuromodulation::applyNeuromodulation(signal);
}

// Plasticity and development - delegate to BrainPlasticity
void Brain::updatePlasticity() {
    return pImpl->BrainPlasticity::updatePlasticity();
}

void Brain::develop() {
    return pImpl->BrainPlasticity::develop();
}

// Reset - delegate to BrainCore
void Brain::reset() {
    return pImpl->BrainCore::reset();
}

// Persistence - delegate to BrainPersistence
void Brain::save(const std::string& filepath) const {
    return pImpl->BrainPersistence::save(filepath);
}

void Brain::load(const std::string& filepath) {
    return pImpl->BrainPersistence::load(filepath);
}

// Region management - delegate to BrainCore
RegionId Brain::addRegion(const std::string& name) {
    return pImpl->BrainCore::addRegion(name);
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return pImpl->BrainCore::getRegion(id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return pImpl->BrainCore::getRegion(id);
}

size_t Brain::getRegionCount() const {
    return pImpl->BrainCore::getRegionCount();
}

std::vector<RegionId> Brain::getRegionIds() const {
    return pImpl->BrainCore::getRegionIds();
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->BrainCore::getRegions();
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                   float weight = 0.0f, Delay delay = 1) {
    return pImpl->BrainCore::addInterRegionConnection(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    return pImpl->BrainCore::removeInterRegionConnection(source, target);
}

// Global statistics - delegate to BrainCore
size_t Brain::getTotalNeuronCount() const {
    return pImpl->BrainCore::getTotalNeuronCount();
}

size_t Brain::getTotalSynapseCount() const {
    return pImpl->BrainCore::getTotalSynapseCount();
}

size_t Brain::getActiveNeuronCount() const {
    return pImpl->BrainCore::getActiveNeuronCount();
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->BrainCore::getFiringNeuronCount();
}

float Brain::getAverageFiringRate() const {
    return pImpl->BrainCore::getAverageFiringRate();
}

// Memory systems - delegate to BrainMemory
NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->BrainMemory::getWorkingMemory();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->BrainMemory::getEpisodicMemory();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->BrainMemory::getAssociativeMemory();
}

// Prediction system - delegate to BrainPrediction
PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->BrainPrediction::getPredictionSystem();
}

// Cognition systems - delegate to BrainCognition
NeuralPlanner* Brain::getPlanner() {
    return pImpl->BrainCognition::getPlanner();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->BrainCognition::getConceptFormation();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->BrainCognition::getAttention();
}

// Development system - delegate to BrainNeuromodulation
DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->BrainNeuromodulation::getDevelopmentSystem();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->BrainNeuromodulation::getDevelopmentalStage();
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    return pImpl->BrainNeuromodulation::setDevelopmentalStage(stage);
}

// Neuromodulation systems - delegate to BrainNeuromodulation
Dopamine* Brain::getDopamine() {
    return pImpl->BrainNeuromodulation::getDopamine();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->BrainNeuromodulation::getCuriosity();
}

Novelty* Brain::getNovelty() {
    return pImpl->BrainNeuromodulation::getNovelty();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->BrainNeuromodulation::getPredictionErrorSignal();
}

// Configuration and random generator - delegate to BrainCore
std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->BrainCore::getConfig();
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->BrainCore::getRandomGenerator();
}

// Logging - delegate to BrainCore
void Brain::logStatus() const {
    return pImpl->BrainCore::logStatus();
}

} // namespace nlm
