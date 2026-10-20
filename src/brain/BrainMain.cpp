#include "Brain.hpp"

namespace nlm {

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    // Development updates structural plasticity
    pImpl->structuralPlasticity->update(this, *pImpl->rng);
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->spikeSystem->reset();
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    return pImpl->brainSerialization->save(filepath);
}

bool Brain::load(const std::string& filepath) {
    return pImpl->brainSerialization->load(filepath);
}

void Brain::logStatus() const {
    pImpl->brainStatistics->collectAndLogStatistics();
}

} // namespace nlm

#include "BrainInitialization.hpp"

namespace nlm {

void Brain::addRegion(const std::string& name) {
    pImpl->brainInitialization->addRegion(name);
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return pImpl->brainInitialization->getRegion(id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return pImpl->brainInitialization->getRegion(id);
}

size_t Brain::getRegionCount() const {
    return pImpl->brainInitialization->getRegionCount();
}

std::vector<RegionId> Brain::getRegionIds() const {
    return pImpl->brainInitialization->getRegionIds();
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->brainInitialization->getRegions();
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    pImpl->brainInitialization->addInterRegionConnection(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->brainInitialization->removeInterRegionConnection(source, target);
}

} // namespace nlm

#include "BrainMemoryManagement.hpp"

namespace nlm {

void Brain::updateWorkingMemory() {
    pImpl->brainMemoryManagement->updateWorkingMemory();
}

void Brain::updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->brainMemoryManagement->updateEpisodicMemory(currentStep, currentTime);
}

void Brain::consolidateMemories(SimulationStep currentStep) {
    pImpl->brainMemoryManagement->consolidateMemories(currentStep);
}

void Brain::replayMemories(SimulationStep currentStep) {
    pImpl->brainMemoryManagement->replayMemories(currentStep);
}

} // namespace nlm

#include "BrainNeuromodulation.hpp"

namespace nlm {

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    pImpl->brainNeuromodulation->applyNeuromodulationEffects(signal.getLevel());
}

} // namespace nlm

#include "BrainPredictionSystem.hpp"

namespace nlm {

void Brain::updatePredictionSystem() {
    pImpl->brainPredictionSystem->updatePredictionSystem();
}

} // namespace nlm

#include "BrainCognitionSystems.hpp"

namespace nlm {

void Brain::updateAttentionSystem(Timestamp currentTime) {
    pImpl->brainCognitionSystems->updateAttentionSystem(currentTime);
}

void Brain::updateConceptFormation() {
    pImpl->brainCognitionSystems->updateConceptFormation();
}

} // namespace nlm

#include "BrainDevelopment.hpp"

namespace nlm {

void Brain::applyDevelopmentEffects(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->brainDevelopment->applyDevelopmentEffects(currentStep, currentTime);
}

} // namespace nlm

#include "BrainStatistics.hpp"

namespace nlm {

void Brain::updateCheckpointManager(SimulationStep currentStep, Timestamp currentTime) {
    // Statistics are collected during the step
}

} // namespace nlm

#include "BrainPlasticity.hpp"

namespace nlm {

void Brain::applyStructuralPlasticity(SimulationStep currentStep) {
    pImpl->brainPlasticity->applyStructuralPlasticity(currentStep);
}

} // namespace nlm