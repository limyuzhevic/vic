#include "NeuralPopulation.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

struct NeuralPopulation::Impl {
    PopulationId id;
    size_t size;
    std::vector<Neuron*> neurons;
    std::vector<std::unique_ptr<Neuron>> ownedNeurons;
    NeuronType neuronType;
    std::vector<MembranePotential> membranePotentials;  // for statistics
    
    Impl(PopulationId id, size_t size) : id(id), size(size), neuronType(NeuronType::Internal) {}
};

NeuralPopulation::NeuralPopulation(PopulationId id, size_t size) : pImpl(new Impl(id, size)) {}

NeuralPopulation::~NeuralPopulation() = default;

NeuralPopulation::NeuralPopulation(NeuralPopulation&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

NeuralPopulation& NeuralPopulation::operator=(NeuralPopulation&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

PopulationId NeuralPopulation::getId() const {
    return pImpl->id;
}

size_t NeuralPopulation::getSize() const {
    return pImpl->size;
}

bool NeuralPopulation::isEmpty() const {
    return pImpl->neurons.empty();
}

Neuron* NeuralPopulation::getNeuron(NeuronIndex index) {
    if (index >= pImpl->neurons.size()) {
        return nullptr;
    }
    return pImpl->neurons[index];
}

const Neuron* NeuralPopulation::getNeuron(NeuronIndex index) const {
    if (index >= pImpl->neurons.size()) {
        return nullptr;
    }
    return pImpl->neurons[index];
}

void NeuralPopulation::createNeurons(NeuronId startId) {
    pImpl->ownedNeurons.clear();
    pImpl->neurons.clear();
    pImpl->neurons.reserve(pImpl->size);
    pImpl->ownedNeurons.reserve(pImpl->size);
    for (size_t i = 0; i < pImpl->size; ++i) {
        auto neuron = std::make_unique<Neuron>(NeuronId(startId.index() + i));
        neuron->setType(pImpl->neuronType);
        pImpl->neurons.push_back(neuron.get());
        pImpl->ownedNeurons.push_back(std::move(neuron));
    }
}

void NeuralPopulation::addNeuron(Neuron* neuron) {
    pImpl->neurons.push_back(neuron);
    if (pImpl->size < pImpl->neurons.size()) {
        pImpl->size = pImpl->neurons.size();
    }
}

const std::vector<Neuron*>& NeuralPopulation::getNeurons() const {
    return pImpl->neurons;
}

NeuronType NeuralPopulation::getNeuronType() const {
    return pImpl->neuronType;
}

void NeuralPopulation::setNeuronType(NeuronType type) {
    pImpl->neuronType = type;
    for (auto* neuron : pImpl->neurons) {
        neuron->setType(type);
    }
}

size_t NeuralPopulation::getActiveCount() const {
    size_t count = 0;
    for (const auto* neuron : pImpl->neurons) {
        if (neuron->isFiring() || neuron->getFiringRate() > 0.0f) {
            ++count;
        }
    }
    return count;
}

size_t NeuralPopulation::getFiringCount() const {
    size_t count = 0;
    for (const auto* neuron : pImpl->neurons) {
        if (neuron->isFiring()) {
            ++count;
        }
    }
    return count;
}

float NeuralPopulation::getAverageFiringRate() const {
    if (pImpl->neurons.empty()) {
        return 0.0f;
    }
    float sum = 0.0f;
    for (const auto* neuron : pImpl->neurons) {
        sum += neuron->getFiringRate();
    }
    return sum / static_cast<float>(pImpl->neurons.size());
}

float NeuralPopulation::getActivityLevel() const {
    if (pImpl->neurons.empty()) {
        return 0.0f;
    }
    return static_cast<float>(getActiveCount()) / static_cast<float>(pImpl->neurons.size());
}

void NeuralPopulation::initializeRandom(RandomGenerator& rng) {
    for (auto* neuron : pImpl->neurons) {
        neuron->initializeRandom(rng);
    }
}

void NeuralPopulation::step(Timestamp currentTime) {
    for (auto* neuron : pImpl->neurons) {
        neuron->step(currentTime);
    }
}

void NeuralPopulation::reset() {
    for (auto* neuron : pImpl->neurons) {
        neuron->reset();
    }
    pImpl->membranePotentials.clear();
}

std::vector<NeuronId> NeuralPopulation::getNeuronIds() const {
    std::vector<NeuronId> ids;
    ids.reserve(pImpl->neurons.size());
    for (const auto* neuron : pImpl->neurons) {
        ids.push_back(neuron->getId());
    }
    return ids;
}

void NeuralPopulation::recordActivity() {
    pImpl->membranePotentials.clear();
    pImpl->membranePotentials.reserve(pImpl->neurons.size());
    for (const auto* neuron : pImpl->neurons) {
        pImpl->membranePotentials.push_back(neuron->getMembranePotential());
    }
}

float NeuralPopulation::getMeanMembranePotential() const {
    if (pImpl->membranePotentials.empty()) {
        return 0.0f;
    }
    float sum = std::accumulate(pImpl->membranePotentials.begin(), 
                                 pImpl->membranePotentials.end(), 0.0f);
    return sum / static_cast<float>(pImpl->membranePotentials.size());
}

float NeuralPopulation::getVarianceMembranePotential() const {
    if (pImpl->membranePotentials.size() < 2) {
        return 0.0f;
    }
    float mean = getMeanMembranePotential();
    float sumSquares = 0.0f;
    for (float v : pImpl->membranePotentials) {
        float diff = v - mean;
        sumSquares += diff * diff;
    }
    return sumSquares / static_cast<float>(pImpl->membranePotentials.size() - 1);
}

void NeuralPopulation::updateNeurons(Timestamp currentTime, Timestamp timestep) {
    for (auto* neuron : pImpl->neurons) {
        neuron->stepLIF(currentTime, timestep);
    }
}

void NeuralPopulation::detectSpikes(SimulationStep currentStep, Timestamp currentTime,
                                    Timestamp timestep, SpikeSystem* spikeSystem) {
    for (auto* neuron : pImpl->neurons) {
        const auto& state = neuron->getState();
        bool justFired = (state.firingState == FiringState::Refractory &&
                         state.lastSpikeTime >= 0.0f &&
                         std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);

        if (justFired) {
            // Neuron fired this step - queue the spike
            SpikeEvent event(neuron->getId(), currentTime, currentStep);
            spikeSystem->queueSpike(event);
        }
    }
}

void NeuralPopulation::applyDopamineEffects(float level, Timestamp timestep) {
    float excitabilityMod = level * 0.5f;
    if (excitabilityMod > 0.0f) {
        for (auto* neuron : pImpl->neurons) {
            neuron->injectCurrent(excitabilityMod);
        }
    }
}

// Method to find neurons by type with better error handling
std::vector<Neuron*> NeuralPopulation::getNeuronsByType(NeuronType type) {
    std::vector<Neuron*> result;
    for (auto* neuron : pImpl->neurons) {
        if (neuron && neuron->getType() == type) {
            result.push_back(neuron);
        }
    }
    return result;
}

// Method to find neurons using a predicate function
std::vector<Neuron*> NeuralPopulation::findNeurons(std::function<bool(const Neuron*)> predicate) {
    std::vector<Neuron*> result;
    for (auto* neuron : pImpl->neurons) {
        if (neuron && predicate(neuron)) {
            result.push_back(neuron);
        }
    }
    return result;
}

// Method to clear the population
void NeuralPopulation::clear() {
    pImpl->neurons.clear();
    pImpl->ownedNeurons.clear();
    pImpl->size = 0;
}

// Method to inject current to neurons of a specific type
void NeuralPopulation::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto* neuron : pImpl->neurons) {
        if (neuron && neuron->getType() == type) {
            neuron->injectCurrent(current);
        }
    }
}

// Method to reset all neurons in the population
void NeuralPopulation::resetAll() {
    for (auto* neuron : pImpl->neurons) {
        if (neuron) {
            neuron->reset();
        }
    }
    pImpl->membranePotentials.clear();
}

// Method to apply dopamine effects to neurons in this population
void NeuralPopulation::applyDopamineEffects(float level, float timestep) {
    if (level <= 0.0f) return;  // Early return if no modulation
    
    float excitabilityMod = level * 0.5f;
    if (excitabilityMod > 0.0f) {
        for (auto* neuron : pImpl->neurons) {
            if (neuron) {
                neuron->injectCurrent(excitabilityMod);
            }
        }
    }
}

// Method to update neurons with proper error checking
void NeuralPopulation::updateNeurons(Timestamp currentTime, Timestamp timestep) {
    for (auto* neuron : pImpl->neurons) {
        if (neuron) {
            neuron->stepLIF(currentTime, timestep);
        }
    }
}

// Method to detect spikes with proper error handling
void NeuralPopulation::detectSpikes(SimulationStep currentStep, Timestamp currentTime,
                                    Timestamp timestep, SpikeSystem* spikeSystem) {
    if (!spikeSystem) return;
    
    for (auto* neuron : pImpl->neurons) {
        if (!neuron) continue;
        
        const auto& state = neuron->getState();
        bool justFired = (state.firingState == FiringState::Refractory &&
                         state.lastSpikeTime >= 0.0f &&
                         std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);

        if (justFired) {
            // Neuron fired this step - queue the spike
            SpikeEvent event(neuron->getId(), currentTime, currentStep);
            spikeSystem->queueSpike(event);
        }
    }
}

// Method to apply plasticity to neurons in this population
void NeuralPopulation::applyPlasticity(class STDP* stdp, class Hebbian* hebbian,
                                       float plasticityMod, Timestamp currentTime,
                                       Timestamp timestep) {
    // Note: Plasticity is typically applied to synapses at the region level
    // This is a placeholder for population-level plasticity
    // Real plasticity would involve:
    // 1. Getting synapses from the parent region
    // 2. Applying plasticity rules to those synapses
    // 3. Returning updated synapses back to the region
    
    for (auto* neuron : pImpl->neurons) {
        if (neuron) {
            // Store current state for potential plasticity
            // In practice, plasticity would modify synapses, not neurons directly
        }
    }
}

} // namespace nlm
