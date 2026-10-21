#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

Brain::Brain(std::shared_ptr<Config> config) 
    : setupComponent(std::make_unique<BrainSetup>(config))
    , integrationComponent(std::make_unique<BrainIntegration>(config)) {
}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept
    : setupComponent(std::move(other.setupComponent))
    , integrationComponent(std::move(other.integrationComponent)) {
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        setupComponent = std::move(other.setupComponent);
        integrationComponent = std::move(other.integrationComponent);
    }
    return *this;
}

bool Brain::initialize() {
    // Initialize setup component
    bool setupSuccess = setupComponent->initialize();
    if (!setupSuccess) {
        NLM_LOG_ERROR("Brain setup component initialization failed");
        return false;
    }
    
    // Integration component is initialized as part of setup
    return true;
}

void Brain::step(SimulationStep currentStep) {
    integrationComponent->step(currentStep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    integrationComponent->step(currentStep, currentTime);
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    setupComponent->receiveSensoryInput(input);
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    setupComponent->injectCurrent(neuron, current);
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    setupComponent->injectCurrentToNeurons(type, current);
}

SpikeSystem* Brain::getSpikeSystem() {
    return integrationComponent->getSpikeSystem();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return integrationComponent->getSpikeSystem();
}

STDP* Brain::getSTDP() {
    return integrationComponent->getSTDP();
}

Hebbian* Brain::getHebbian() {
    return integrationComponent->getHebbian();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return integrationComponent->getStructuralPlasticity();
}

float Brain::getExcitationInhibitionRatio() const {
    return integrationComponent->getExcitationInhibitionRatio();
}

size_t Brain::getTotalSpikeCount() const {
    return integrationComponent->getTotalSpikeCount();
}

size_t Brain::getPendingSpikeEventCount() const {
    return integrationComponent->getPendingSpikeEventCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    return integrationComponent->produceAction();
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    integrationComponent->applyNeuromodulation(signal);
}

void Brain::updatePlasticity() {
    integrationComponent->updatePlasticity();
}

void Brain::develop() {
    integrationComponent->develop();
}

void Brain::reset() {
    setupComponent->reset();
}

bool Brain::save(const std::string& filepath) const {
    return integrationComponent->save(filepath);
}

bool Brain::load(const std::string& filepath) {
    bool success = integrationComponent->load(filepath);
    if (success) {
        // Re-initialize after load to ensure consistency
        setupComponent->initialize();
    }
    return success;
}

RegionId Brain::addRegion(const std::string& name) {
    return setupComponent->addRegion(name);
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return setupComponent->getRegion(id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return setupComponent->getRegion(id);
}

size_t Brain::getRegionCount() const {
    return setupComponent->getRegionCount();
}

std::vector<RegionId> Brain::getRegionIds() const {
    return setupComponent->getRegionIds();
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return setupComponent->getRegions();
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                     float weight, Delay delay) {
    setupComponent->addInterRegionConnection(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    setupComponent->removeInterRegionConnection(source, target);
}

size_t Brain::getTotalNeuronCount() const {
    return setupComponent->getTotalNeuronCount();
}

size_t Brain::getTotalSynapseCount() const {
    return setupComponent->getTotalSynapseCount();
}

size_t Brain::getActiveNeuronCount() const {
    return setupComponent->getActiveNeuronCount();
}

size_t Brain::getFiringNeuronCount() const {
    return integrationComponent->getPendingSpikeEventCount();
}

float Brain::getAverageFiringRate() const {
    return setupComponent->getAverageFiringRate();
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return integrationComponent->getWorkingMemory();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return integrationComponent->getEpisodicMemory();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return integrationComponent->getAssociativeMemory();
}

PredictionSystem* Brain::getPredictionSystem() {
    return integrationComponent->getPredictionSystem();
}

NeuralPlanner* Brain::getPlanner() {
    return integrationComponent->getPlanner();
}

ConceptFormation* Brain::getConceptFormation() {
    return integrationComponent->getConceptFormation();
}

AttentionalSelection* Brain::getAttention() {
    return integrationComponent->getAttention();
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return integrationComponent->getDevelopmentSystem();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return integrationComponent->getDevelopmentalStage();
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    integrationComponent->setDevelopmentalStage(stage);
}

Dopamine* Brain::getDopamine() {
    return integrationComponent->getDopamine();
}

Curiosity* Brain::getCuriosity() {
    return integrationComponent->getCuriosity();
}

Novelty* Brain::getNovelty() {
    return integrationComponent->getNovelty();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return integrationComponent->getPredictionErrorSignal();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return setupComponent->getConfig();
}

RandomGenerator* Brain::getRandomGenerator() {
    return setupComponent->getRandomGenerator();
}

void Brain::logStatus() const {
    integrationComponent->logStatus();
}

BrainIntegration* Brain::getIntegration() {
    return integrationComponent.get();
}

const BrainIntegration* Brain::getIntegration() const {
    return integrationComponent.get();
}

void Brain::integrateDevelopment() {
    develop();
}

void Brain::integrateNeuromodulation() {
    // Neuromodulation is integrated in the step method
}

void Brain::integrateMemorySystems() {
    // Memory systems are integrated through the BrainIntegration component
}

} // namespace nlm