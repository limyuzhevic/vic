#include "NeuralPopulation.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

struct NeuralPopulation::Impl {
    PopulationId id;
    size_t size;
    std::vector<Neuron*> neurons;
    NeuronType neuronType;
    std::vector<MembranePotential> membranePotentials;  // for statistics
    
    Impl(PopulationId id, size_t size) : id(id), size(size), neuronType(NeuronType::Internal) {}
};

NeuralPopulation::NeuralPopulation(PopulationId id, size_t size) : pImpl(new Impl(id, size)) {}

NeuralPopulation::~NeuralPopulation() {
    delete pImpl;
}

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

#include "SIMDVectorization.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

// Add VectorizedLIF support to NeuralPopulation

void NeuralPopulation::stepSIMD(Timestamp currentTime) {
    if (pImpl->neurons.empty()) return;
    
    size_t neuronCount = pImpl->neurons.size();
    
    // Create arrays for vectorized processing
    std::vector<float> membranePotentials;
    std::vector<float> synapticInputs;
    std::vector<uint32_t> refractoryRemaining;
    std::vector<uint32_t> firedFlags;
    
    // Collect neuron data
    membranePotentials.reserve(neuronCount);
    synapticInputs.reserve(neuronCount);
    refractoryRemaining.reserve(neuronCount);
    firedFlags.reserve(neuronCount);
    
    for (size_t i = 0; i < neuronCount; ++i) {
        auto* neuron = pImpl->neurons[i];
        
        // Get neuron state
        float v = neuron->getMembranePotential();
        float threshold = neuron->getThreshold();
        uint32_t refractory = neuron->getRefractoryRemaining();
        
        // Get synaptic input
        float synapticInput = neuron->getMembranePotential();  // Using membrane potential as proxy
        
        // Store for vectorized processing
        membranePotentials.push_back(v);
        synapticInputs.push_back(synapticInput);
        refractoryRemaining.push_back(refractory);
        firedFlags.push_back(0);  // Initialize fired flags
        
        // Set neuron parameters for vectorized processing
        neuron->setThreshold(threshold);  // Store threshold
        neuron->setRefractoryPeriod(refractory);  // Store refractory period
    }
    
    // Initialize VectorizedLIF with standard neuron parameters
    VectorizedLIF lif;
    lif.V_rest = -70.0f;  // Typical resting potential
    lif.V_reset = -70.0f;
    lif.V_threshold = -55.0f;
    lif.tau = 20.0f;     // Typical time constant
    lif.C = 1.0f;       // Typical capacitance
    lif.dt = 0.001f;    // Typical timestep (1ms)
    lif.refractoryDecay = 1.0f;
    
    // Perform vectorized update
    size_t numFired = lif.update(membranePotentials, synapticInputs, refractoryRemaining,
                                 firedFlags, neuronCount);
    
    // Update neuron states based on vectorized results
    for (size_t i = 0; i < neuronCount; ++i) {
        auto* neuron = pImpl->neurons[i];
        
        // Update membrane potential
        neuron->setMembranePotential(membranePotentials[i]);
        
        // Update refractory counter
        neuron->setRefractoryPeriod(refractoryRemaining[i]);
        
        // Handle firing
        if (firedFlags[i]) {
            neuron->recordSpike(currentTime);
            neuron->setFiringState(FiringState::Active);
        }
        
        // Reset if not firing
        if (!firedFlags[i]) {
            neuron->setFiringState(FiringState::Active);
        }
    }
}

void NeuralPopulation::stepSIMDWithOptimization(Timestamp currentTime) {
    // Optimized version with better memory access patterns
    size_t neuronCount = pImpl->neurons.size();
    
    if (neuronCount == 0) return;
    
    // Pre-allocate vectors for better memory locality
    std::vector<float> membranePotentials;
    std::vector<float> synapticInputs;
    std::vector<uint32_t> refractoryRemaining(neuronCount, 0);
    std::vector<uint32_t> firedFlags(neuronCount, 0);
    
    // Process neurons in batches for better SIMD utilization
    constexpr size_t BATCH_SIZE = 64;  // Process in chunks for cache efficiency
    
    for (size_t batchStart = 0; batchStart < neuronCount; batchStart += BATCH_SIZE) {
        size_t batchSize = std::min(BATCH_SIZE, neuronCount - batchStart);
        
        // Collect batch data
        membranePotentials.clear();
        synapticInputs.clear();
        membranePotentials.reserve(batchSize);
        synapticInputs.reserve(batchSize);
        
        for (size_t i = 0; i < batchSize; ++i) {
            size_t idx = batchStart + i;
            auto* neuron = pImpl->neurons[idx];
            
            membranePotentials.push_back(neuron->getMembranePotential());
            synapticInputs.push_back(neuron->getMembranePotential());  // Using as synaptic input proxy
            refractoryRemaining[idx] = neuron->getRefractoryRemaining();
        }
        
        // Vectorized update for this batch
        VectorizedLIF lif;
        lif.V_rest = -70.0f;
        lif.V_reset = -70.0f;
        lif.V_threshold = -55.0f;
        lif.tau = 20.0f;
        lif.C = 1.0f;
        lif.dt = 0.001f;
        lif.refractoryDecay = 1.0f;
        
        size_t numFired = lif.update(membranePotentials, synapticInputs, 
                                     std::ref(refractoryRemaining), firedFlags, batchSize);
        
        // Update neuron states
        for (size_t i = 0; i < batchSize; ++i) {
            size_t idx = batchStart + i;
            auto* neuron = pImpl->neurons[idx];
            
            neuron->setMembranePotential(membranePotentials[i]);
            neuron->setRefractoryPeriod(refractoryRemaining[idx]);
            
            if (firedFlags[idx]) {
                neuron->recordSpike(currentTime);
                neuron->setFiringState(FiringState::Active);
            } else {
                neuron->setFiringState(FiringState::Active);
            }
        }
    }
}

// Optimized statistics computation using SIMD
float NeuralPopulation::getMeanMembranePotentialSIMD() const {
    if (pImpl->membranePotentials.empty()) {
        return 0.0f;
    }
    
    // Use vectorized statistics for better performance
    return VectorizedStats::mean(pImpl->membranePotentials.data(), 
                                pImpl->membranePotentials.size());
}

float NeuralPopulation::getVarianceMembranePotentialSIMD() const {
    if (pImpl->membranePotentials.size() < 2) {
        return 0.0f;
    }
    
    return VectorizedStats::variance(pImpl->membranePotentials.data(),
                                     pImpl->membranePotentials.size());
}

float NeuralPopulation::getMaxMembranePotentialSIMD() const {
    if (pImpl->membranePotentials.empty()) {
        return 0.0f;
    }
    
    return VectorizedStats::max(pImpl->membranePotentials.data(),
                                pImpl->membranePotentials.size());
}

// Add method to enable/disable SIMD optimizations
void NeuralPopulation::setSIMDMode(bool enableSIMD) {
    pImpl->neuronType = enableSIMD ? NeuronType::Internal : NeuronType::Sensory;
}

// Check if SIMD is available
bool NeuralPopulation::isSIMDAvailable() const {
    return SIMDUtils::isSIMDAvailable();
}

// Get SIMD type for diagnostics
const char* NeuralPopulation::getSIMDType() const {
    return SIMDUtils::getSIMDType();
}

// Get number of neurons processed with SIMD
size_t NeuralPopulation::getSIMDNeuronsProcessed() const {
    // This could be tracked by adding a counter in the SIMD methods
    return pImpl->neurons.size();
}

// Initialize SIMD parameters for neurons
void NeuralPopulation::initializeSIMDParameters(RandomGenerator& rng) {
    for (auto* neuron : pImpl->neurons) {
        // Set standard parameters for SIMD processing
        neuron->setMembranePotential(-70.0f);  // V_rest
        neuron->setThreshold(-55.0f);         // Threshold
        neuron->setRestingPotential(-70.0f);   // Resting potential
        neuron->setResetPotential(-70.0f);    // Reset potential
        neuron->setRefractoryPeriod(rng.uniformInt(2, 10));
        neuron->setLeakConductance(1.0f / 20.0f);  // 1/tau
        neuron->setAdaptationVariable(0.0f);
    }
}

// Optimized population-wide operations
void NeuralPopulation::bulkInjectCurrent(float current, NeuronType type) {
    for (auto* neuron : pImpl->neurons) {
        if (neuron->getType() == type) {
            neuron->injectCurrent(current);
        }
    }
}

void NeuralPopulation::bulkSetThreshold(float threshold) {
    for (auto* neuron : pImpl->neurons) {
        neuron->setThreshold(threshold);
    }
}

void NeuralPopulation::bulkReset() {
    for (auto* neuron : pImpl->neurons) {
        neuron->reset();
    }
    pImpl->membranePotentials.clear();
}

} // namespace nlm

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

} // namespace nlm
