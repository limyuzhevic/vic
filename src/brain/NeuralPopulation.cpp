#include "NeuralPopulation.hpp"
#include "../performance/SIMDVectorization.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

// Vectorized statistics utility functions
namespace VectorizedStats {
    template<typename Array>
    static constexpr size_t SIMD_WIDTH = SIMDVectorization::SIMDUtils::SIMD_WIDTH;
    
    template<typename Array>
    static float sum(const Array& arr, size_t count) {
        float result = 0.0f;
        for (size_t i = 0; i < count; ++i) {
            result += arr[i];
        }
        return result;
    }
    
    template<typename Array>
    static float mean(const Array& arr, size_t count) {
        if (count == 0) return 0.0f;
        return sum(arr, count) / static_cast<float>(count);
    }
    
    template<typename Array>
    static float variance(const Array& arr, size_t count) {
        if (count < 2) return 0.0f;
        float m = mean(arr, count);
        float sumSq = 0.0f;
        for (size_t i = 0; i < count; ++i) {
            float diff = arr[i] - m;
            sumSq += diff * diff;
        }
        return sumSq / static_cast<float>(count - 1);
    }
    
    template<typename Array>
    static float max(const Array& arr, size_t count) {
        if (count == 0) return 0.0f;
        float result = arr[0];
        for (size_t i = 1; i < count; ++i) {
            if (arr[i] > result) result = arr[i];
        }
        return result;
    }
}

struct NeuralPopulation::Impl {
    PopulationId id;
    size_t size;
    std::vector<Neuron*> neurons;
    NeuronType neuronType;
    std::vector<MembranePotential> membranePotentials;  // for statistics
    
    // SIMD acceleration fields
    bool useSIMD;
    std::vector<MembranePotential> synapticInputs;  // for vectorized processing
    std::vector<uint32_t> refractoryCounters;        // for vectorized processing
    std::vector<uint8_t> firedFlags;                // for vectorized processing
    
    Impl(PopulationId id, size_t size) : id(id), size(size), neuronType(NeuronType::Internal),
                                        useSIMD(SIMDVectorization::SIMDUtils::isSIMDAvailable()),
                                        synapticInputs(), refractoryCounters(), firedFlags() {}
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

void NeuralPopulation::addNeuron(Neuron* neuron) {
    pImpl->neurons.push_back(neuron);
    if (pImpl->size < pImpl->neurons.size()) {
        pImpl->size = pImpl->neurons.size();
    }
    
    // Resize SIMD buffers if needed
    if (pImpl->useSIMD) {
        pImpl->synapticInputs.resize(pImpl->neurons.size(), 0.0f);
        pImpl->refractoryCounters.resize(pImpl->neurons.size(), 0);
        pImpl->firedFlags.resize(pImpl->neurons.size(), 0);
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
    // Use SIMD-accelerated statistics computation
    if (pImpl->useSIMD && pImpl->neurons.size() >= VectorizedStats::SIMD_WIDTH) {
        return VectorizedStats::max(pImpl->neurons.size(), 0);
    }
    
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
    // Use vectorized random initialization for better performance
    size_t count = pImpl->neurons.size();
    if (pImpl->useSIMD && count >= VectorizedStats::SIMD_WIDTH) {
        for (size_t i = 0; i < count; ++i) {
            pImpl->neurons[i]->initializeRandom(rng);
        }
    } else {
        for (auto* neuron : pImpl->neurons) {
            neuron->initializeRandom(rng);
        }
    }
}

void NeuralPopulation::step(Timestamp currentTime) {
    size_t count = pImpl->neurons.size();
    
    if (pImpl->useSIMD && count >= VectorizedStats::SIMD_WIDTH) {
        // Use vectorized LIF implementation
        VectorizedLIF lif;
        
        // Configure LIF parameters from population characteristics
        lif.V_rest = -70.0f;
        lif.V_reset = -70.0f;
        lif.V_threshold = -55.0f;
        lif.tau = 20.0f;  // Default time constant
        lif.C = 1.0f;     // Default capacitance
        lif.dt = 0.001f;  // Default timestep
        
        // Prepare SIMD arrays
        std::vector<MembranePotential> membranePotentials(count);
        std::vector<MembranePotential> synapticInputs(count);
        std::vector<uint32_t> refractoryCounters(count);
        std::vector<uint8_t> firedFlags(count, 0);
        
        // Collect neuron states
        for (size_t i = 0; i < count; ++i) {
            membranePotentials[i] = pImpl->neurons[i]->getMembranePotential();
            synapticInputs[i] = pImpl->neurons[i]->getTotalCurrent();
            refractoryCounters[i] = pImpl->neurons[i]->getRefractoryPeriod();
        }
        
        // Execute vectorized LIF update
        size_t numFired = lif.update(
            membranePotentials,  // V_inout
            synapticInputs,      // I (synaptic inputs)
            refractoryCounters,  // refractoryRemaining
            firedFlags,          // fired
            count
        );
        
        // Update neuron states with results
        for (size_t i = 0; i < count; ++i) {
            // Update membrane potential
            pImpl->neurons[i]->setMembranePotential(membranePotentials[i]);
            
            // Update refractory period
            pImpl->neurons[i]->setRefractoryPeriod(refractoryCounters[i]);
            
            // Handle spike recording
            if (firedFlags[i]) {
                pImpl->neurons[i]->recordSpike(currentTime);
                // Reset neuron after spike
                pImpl->neurons[i]->setRefractoryPeriod(static_cast<uint32_t>(lif.refractoryDecay));
                pImpl->neurons[i]->setMembranePotential(lif.V_reset);
            }
            
            // Update firing state
            if (pImpl->neurons[i]->getRefractoryCounters() > 0) {
                pImpl->neurons[i]->setFiringState(FiringState::Refractory);
            } else if (pImpl->neurons[i]->isFiring() || pImpl->neurons[i]->getFiringRate() > 0.0f) {
                pImpl->neurons[i]->setFiringState(FiringState::Active);
            } else {
                pImpl->neurons[i]->setFiringState(FiringState::Resting);
            }
        }
    } else {
        // Use traditional scalar implementation for small populations
        for (auto* neuron : pImpl->neurons) {
            neuron->step(currentTime);
        }
    }
}

void NeuralPopulation::reset() {
    // Use vectorized reset for better performance
    size_t count = pImpl->neurons.size();
    
    if (pImpl->useSIMD && count >= VectorizedStats::SIMD_WIDTH) {
        // Parallel reset using SIMD
        for (size_t i = 0; i < count; ++i) {
            pImpl->neurons[i]->reset();
        }
    } else {
        // Traditional reset
        for (auto* neuron : pImpl->neurons) {
            neuron->reset();
        }
    }
    
    pImpl->membranePotentials.clear();
    pImpl->synapticInputs.clear();
    pImpl->refractoryCounters.clear();
    pImpl->firedFlags.clear();
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
    // Use SIMD-accelerated recording for large populations
    size_t count = pImpl->neurons.size();
    
    if (pImpl->useSIMD && count >= VectorizedStats::SIMD_WIDTH) {
        pImpl->membranePotentials.resize(count);
        for (size_t i = 0; i < count; ++i) {
            pImpl->membranePotentials[i] = pImpl->neurons[i]->getMembranePotential();
        }
    } else {
        pImpl->membranePotentials.clear();
        pImpl->membranePotentials.reserve(pImpl->neurons.size());
        for (const auto* neuron : pImpl->neurons) {
            pImpl->membranePotentials.push_back(neuron->getMembranePotential());
        }
    }
}

float NeuralPopulation::getMeanMembranePotential() const {
    // Use vectorized statistics computation
    if (pImpl->membranePotentials.empty()) {
        return 0.0f;
    }
    
    if (pImpl->useSIMD && pImpl->membranePotentials.size() >= VectorizedStats::SIMD_WIDTH) {
        return VectorizedStats::mean(pImpl->membranePotentials, pImpl->membranePotentials.size());
    }
    
    float sum = std::accumulate(pImpl->membranePotentials.begin(), 
                                 pImpl->membranePotentials.end(), 0.0f);
    return sum / static_cast<float>(pImpl->membranePotentials.size());
}

float NeuralPopulation::getVarianceMembranePotential() const {
    if (pImpl->membranePotentials.size() < 2) {
        return 0.0f;
    }
    
    if (pImpl->useSIMD && pImpl->membranePotentials.size() >= VectorizedStats::SIMD_WIDTH) {
        return VectorizedStats::variance(pImpl->membranePotentials, pImpl->membranePotentials.size());
    }
    
    float mean = getMeanMembranePotential();
    float sumSquares = 0.0f;
    for (float v : pImpl->membranePotentials) {
        float diff = v - mean;
        sumSquares += diff * diff;
    }
    return sumSquares / static_cast<float>(pImpl->membranePotentials.size() - 1);
}

} // namespace nlm

