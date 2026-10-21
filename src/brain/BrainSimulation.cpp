#include "BrainSimulation.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

BrainSimulation::BrainSimulation(std::shared_ptr<BrainCore> brainCore)
    : brainCore_(brainCore)
    , timestep_(0.001)
    , currentStep_(0)
    , currentTime_(0.0)
{
}

void BrainSimulation::step(SimulationStep currentStep) {
    step(currentStep, currentTime_);
}

void BrainSimulation::step(SimulationStep currentStep, Timestamp currentTime) {
    // Store current step and time
    currentStep_ = currentStep;
    currentTime_ = currentTime;
    
    // Update core simulation
    brainCore_->processSimulationStep();
    
    // Update timestep based on config
    timestep_ = brainCore_->getConfig()->getOr<double>("simulation_timestep", 0.001);
}

void BrainSimulation::stepWithWorld(SimulationStep currentStep, Timestamp currentTime, 
                                   const SimpleWorld& world, AgentBrain* agent) {
    // Process world update
    // This would integrate the world state into the brain simulation
    
    // Process agent sensory input if available
    if (agent) {
        // Get sensory percept from world
        const SensoryPercept& percept = world.getSensoryPercept();
        
        // Process sensory input through agent brain
        agent->processSensoryInput(percept);
        
        // Get motor command from agent
        MotorCommand action = agent->decodeMotorCommand();
        
        // Apply action to world
        world.applyMotorCommand(action, currentTime);
    }
    
    // Perform core brain simulation step
    step(currentStep, currentTime);
}

void BrainSimulation::receiveSensoryInput(const class SensoryInput& input) {
    // Find sensory neurons and inject appropriate currents
    for (Neuron* neuron : brainCore_->getSensoryNeurons()) {
        // Inject current based on input pattern
        // This is a simplified implementation
    }
}

void BrainSimulation::injectCurrent(NeuronId neuron, MembranePotential current) {
    // Find and inject current into specific neuron
    for (const auto& region : brainCore_->getRegions()) {
        auto* foundNeuron = region->findNeuron(neuron);
        if (foundNeuron) {
            foundNeuron->injectCurrent(current);
            break;
        }
    }
}

void BrainSimulation::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    // Inject current into all neurons of specified type
    for (const auto& region : brainCore_->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            if (population->getNeuronType() == type) {
                for (Neuron* neuron : population->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

float BrainSimulation::getExcitationInhibitionRatio() const {
    size_t excitatoryCount = 0;
    size_t inhibitoryCount = 0;
    
    for (const auto& region : brainCore_->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            NeuronType type = population->getNeuronType();
            size_t count = population->getNeuronCount();
            
            if (type == NeuronType::Excitatory) {
                excitatoryCount += count;
            } else if (type == NeuronType::Inhibitory) {
                inhibitoryCount += count;
            }
        }
    }
    
    if (inhibitoryCount == 0) return 1.0f;
    return static_cast<float>(excitatoryCount) / inhibitoryCount;
}

size_t BrainSimulation::getTotalSpikeCount() const {
    return brainCore_->getTotalSpikeCount();
}

size_t BrainSimulation::getPendingSpikeEventCount() const {
    return spikeSystem_ ? spikeSystem_->getPendingEventCount() : 0;
}

std::unique_ptr<Action> BrainSimulation::produceAction() {
    // Find motor neurons and produce action based on their activity
    // This is a simplified implementation
    auto action = std::make_unique<Action>();
    action->setType(ActionType::Wait); // Default action
    return action;
}

void BrainSimulation::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulatory signal to brain
    // This would affect neural excitability and plasticity
}

void BrainSimulation::updatePlasticity() {
    brainCore_->updatePlasticitySystems();
}

void BrainSimulation::develop() {
    brainCore_->develop();
}

void BrainSimulation::reset() {
    // Reset all brain components
    brainCore_->reset();
    currentStep_ = 0;
    currentTime_ = 0.0;
}

bool BrainSimulation::save(const std::string& filepath) const {
    // Save brain state to file
    return brainCore_->save(filepath);
}

bool BrainSimulation::load(const std::string& filepath) {
    // Load brain state from file
    return brainCore_->load(filepath);
}

RegionId BrainSimulation::addRegion(const std::string& name) {
    return brainCore_->addRegion(name);
}

NeuralRegion* BrainSimulation::getRegion(RegionId id) {
    return brainCore_->getRegion(id);
}

const NeuralRegion* BrainSimulation::getRegion(RegionId id) const {
    return brainCore_->getRegion(id);
}

size_t BrainSimulation::getRegionCount() const {
    return brainCore_->getRegionCount();
}

std::vector<RegionId> BrainSimulation::getRegionIds() const {
    return brainCore_->getRegionIds();
}

size_t BrainSimulation::getTotalNeuronCount() const {
    return brainCore_->getTotalNeuronCount();
}

size_t BrainSimulation::getTotalSynapseCount() const {
    return brainCore_->getTotalSynapseCount();
}

size_t BrainSimulation::getActiveNeuronCount() const {
    return brainCore_->getActiveNeuronCount();
}

size_t BrainSimulation::getFiringNeuronCount() const {
    return brainCore_->getFiringNeuronCount();
}

float BrainSimulation::getAverageFiringRate() const {
    size_t totalNeurons = getTotalNeuronCount();
    if (totalNeurons == 0) return 0.0f;
    
    float totalFiring = 0.0f;
    for (const auto& region : brainCore_->getRegions()) {
        totalFiring += region->getAverageFiringRate();
    }
    
    return totalFiring / totalNeurons;
}

// Memory system implementations (Phase 6)
NeuralWorkingMemory* BrainSimulation::getWorkingMemory() {
    return workingMemory_.get();
}

NeuralEpisodicMemory* BrainSimulation::getEpisodicMemory() {
    return episodicMemory_.get();
}

NeuralAssociativeMemory* BrainSimulation::getAssociativeMemory() {
    return associativeMemory_.get();
}

// Prediction system implementation (Phase 4)
PredictionSystem* BrainSimulation::getPredictionSystem() {
    return predictionSystem_.get();
}

// Cognition system implementations (Phase 4)
NeuralPlanner* BrainSimulation::getPlanner() {
    return planner_.get();
}

ConceptFormation* BrainSimulation::getConceptFormation() {
    return conceptFormation_.get();
}

AttentionalSelection* BrainSimulation::getAttention() {
    return attention_.get();
}

// Development system implementation
DevelopmentSystem* BrainSimulation::getDevelopmentSystem() {
    return developmentSystem_.get();
}

DevelopmentalStage BrainSimulation::getDevelopmentalStage() const {
    return brainCore_->getDevelopmentalStage();
}

void BrainSimulation::setDevelopmentalStage(DevelopmentalStage stage) {
    brainCore_->setDevelopmentalStage(stage);
}

// Neuromodulation system implementations
Dopamine* BrainSimulation::getDopamine() {
    return dopamine_.get();
}

Curiosity* BrainSimulation::getCuriosity() {
    return curiosity_.get();
}

Novelty* BrainSimulation::getNovelty() {
    return novelty_.get();
}

PredictionError* BrainSimulation::getPredictionErrorSignal() {
    return predictionError_.get();
}

// Configuration and random access
std::shared_ptr<const Config> BrainSimulation::getConfig() const {
    return brainCore_->getConfig();
}

RandomGenerator* BrainSimulation::getRandomGenerator() {
    return brainCore_->getRandomGenerator();
}

// Logging
void BrainSimulation::logStatus() const {
    brainCore_->logStatus();
    NLM_LOG_INFO("=== BrainSimulation Status ===");
    NLM_LOG_INFO("Core timestep: " + std::to_string(timestep_) + "s");
    NLM_LOG_INFO("Simulation step: " + std::to_string(currentStep_));
    NLM_LOG_INFO("Simulation time: " + std::to_string(currentTime_) + "s");
}

// Development update for agents
void BrainSimulation::updateDevelopment(double timestep) {
    if (developmentSystem_) {
        developmentSystem_->update(timestep);
    }
}

// Neuromodulation level getters
float BrainSimulation::getNeuromodulationLevel() const {
    // Return combined neuromodulation level
    return (curiosity_ ? curiosity_->getLevel() : 0.0f) +
           (dopamine_ ? dopamine_->getLevel() : 0.0f);
}

float BrainSimulation::getCuriosityLevel() const {
    return curiosity_ ? curiosity_->getLevel() : 0.0f;
}

float BrainSimulation::getNoveltyLevel() const {
    return novelty_ ? novelty_->getLevel() : 0.0f;
}

float BrainSimulation::getPredictionError() const {
    return predictionError_ ? predictionError_->getError() : 0.0f;
}

}} // namespace nlm