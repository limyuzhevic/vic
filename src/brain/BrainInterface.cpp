// NLM Brain Interface Implementation - Core interface methods

#include "BrainImpl.hpp"
#include "brain/NeuralRegion.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

// Brain interface implementation
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

// Static global functions
const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

SpikeSystem* Brain::getSpikeSystem() {
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
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;

    for (const auto& region : pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }

    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

size_t Brain::getTotalNeuronCount() const {
    size_t count = 0;
    for (const auto& region : pImpl->regions) {
        count += region->getTotalNeuronCount();
    }
    return count;
}

size_t Brain::getTotalSynapseCount() const {
    size_t count = 0;
    for (const auto& region : pImpl->regions) {
        count += region->getSynapseCount();
    }
    return count;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action

    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }

    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }

    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }

    auto action = std::make_unique<Action>(type);

    return action;
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();

    // Scale STDP learning rates
    pImpl->stdp->setLTPWeight(0.01f * modulation);
    pImpl->stdp->setLTDWeight(0.012f * modulation);
}

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

// Brain statistics interface
void Brain::logStatus() {
    NLM_LOG_INFO("=== NLM Brain Status ===");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
}

// Memory system accessors
NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

const NeuralWorkingMemory* Brain::getWorkingMemory() const {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

const NeuralEpisodicMemory* Brain::getEpisodicMemory() const {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

const NeuralAssociativeMemory* Brain::getAssociativeMemory() const {
    return pImpl->associativeMemory.get();
}

Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
}

const Dopamine* Brain::getDopamine() const {
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->curiosity.get();
}

const Curiosity* Brain::getCuriosity() const {
    return pImpl->curiosity.get();
}

PredictionError* Brain::getPredictionError() {
    return pImpl->predictionError.get();
}

const PredictionError* Brain::getPredictionError() const {
    return pImpl->predictionError.get();
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

const DevelopmentSystem* Brain::getDevelopmentSystem() const {
    return pImpl->developmentSystem.get();
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

const NeuralPlanner* Brain::getPlanner() const {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

const ConceptFormation* Brain::getConceptFormation() const {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

const AttentionalSelection* Brain::getAttention() const {
    return pImpl->attention.get();
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

const PredictionSystem* Brain::getPredictionSystem() const {
    return pImpl->predictionSystem.get();
}

float Brain::getAverageFiringRate() const {
    size_t firingCount = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring()) {
                    firingCount++;
                }
            }
        }
    }
    size_t totalNeurons = getTotalNeuronCount();
    return totalNeurons > 0 ? (static_cast<float>(firingCount) / totalNeurons) * 1000.0f : 0.0f;
}

size_t Brain::getFiringNeuronCount() const {
    size_t firingCount = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring()) {
                    firingCount++;
                }
            }
        }
    }
    return firingCount;
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

float Brain::getNeuromodulationLevel() const {
    if (pImpl->dopamine) {
        return pImpl->dopamine->getLevel();
    }
    return 0.0f;
}

size_t Brain::getMemoryEpisodesStored() const {
    if (pImpl->episodicMemory) {
        return pImpl->episodicMemory->getEpisodeCount();
    }
    return 0;
}

float Brain::getNoveltyLevel() const {
    if (pImpl->novelty) {
        return pImpl->novelty->getLevel();
    }
    return 0.0f;
}

float Brain::getCuriosityLevel() const {
    if (pImpl->curiosity) {
        return pImpl->curiosity->getLevel();
    }
    return 0.0f;
}

float Brain::getPredictionError() const {
    if (pImpl->predictionError) {
        return pImpl->predictionError->getError();
    }
    return 0.0f;
}

} // namespace nlm
