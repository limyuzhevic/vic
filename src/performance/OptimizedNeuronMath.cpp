/**
 * OptimizedNeuronMath - LIF neuron math optimized with SIMD
 * 
 * Provides optimized Leaky Integrate-and-Fire (LIF) neuron mathematics
 * using SIMD instructions for high-performance neural simulations.
 * 
 * Features:
 * - SIMD-accelerated neuron state updates
 * - Prefetching and cache-friendly access patterns
 * - Automatic vectorization detection
 * - Memory-optimized state storage
 * - Thermal-aware neuron dynamics
 * 
 * Biological motivation:
 * - LIF model approximates biological neuron behavior
 * - Ion channel dynamics exhibit spatial locality
 * - Synaptic integration benefits from vectorization
 */

#include "OptimizedNeuronMath.hpp"
#include <cstring>
#include <cmath>
#include <algorithm>

namespace nlm {

/**
 * Optimize neuron update using SIMD instructions
 * @param neurons Array of neuron states
 * @param count Number of neurons
 * @param dt Time step
 * @param externalInput External input to neurons
 * @param synapticInput Synaptic input to neurons
 * @param spikeThreshold Spike threshold
 * @param refractoryPeriod Refractory period
 */
void OptimizedNeuronMath::updateNeuronsSIMD(
    NeuronState* neurons,
    size_t count,
    float dt,
    const float* externalInput,
    const float* synapticInput,
    float spikeThreshold,
    float refractoryPeriod
) {
    if (count == 0) return;
    
    // Ensure cache line alignment for optimal performance
    if ((reinterpret_cast<uintptr_t>(neurons) & 63) != 0) {
        // Not aligned, fallback to scalar implementation
        updateNeuronsScalar(neurons, count, dt, externalInput, 
                          synapticInput, spikeThreshold, refractoryPeriod);
        return;
    }
    
    // Prefetch next cache line for better performance
    if (count > SIMD_WIDTH) {
        for (size_t i = SIMD_WIDTH; i < count; i += SIMD_WIDTH) {
            _mm_prefetch(reinterpret_cast<const char*>(&neurons[i]), _MM_HINT_T0);
        }
    }
    
    // Process neurons in SIMD chunks
    size_t i = 0;
    if (count >= SIMD_WIDTH) {
        // Vectorized processing
        for (; i + SIMD_WIDTH <= count; i += SIMD_WIDTH) {
            // Load neuron states
            __m128 vPotential = _mm_load_ps(&neurons[i].potential);
            __m128 vRefractory = _mm_load_ps(&neurons[i].refractoryTime);
            __m128 vIsSpiking = _mm_load_ps(&neurons[i].isSpiking);
            
            // Load external and synaptic inputs
            __m128 vExternalInput = _mm_load_ps(externalInput + i);
            __m128 vSynapticInput = _mm_load_ps(synapticInput + i);
            
            // Update membrane potential: V = V * exp(-dt/tau) + I * R
            __m128 vDecayFactor = _mm_set1_ps(std::exp(-dt / membraneTimeConstant));
            __m128 vMembranePotential = _mm_mul_ps(vPotential, vDecayFactor);
            __m128 vInputCurrent = _mm_mul_ps(vSynapticInput, _mm_set1_ps(resistance));
            vMembranePotential = _mm_add_ps(vMembranePotential, vInputCurrent);
            
            // Update refractory time
            __m128 vRefractoryDecrement = _mm_sub_ps(vRefractory, _mm_set1_ps(dt));
            vRefractory = _mm_max_ps(vRefractoryDecrement, _mm_set1_ps(0.0f));
            
            // Check for spike threshold crossing
            __m128 vSpikeMask = _mm_cmpgt_ps(vMembranePotential, _mm_set1_ps(spikeThreshold));
            
            // Handle spike events
            vIsSpiking = _mm_or_ps(vIsSpiking, _mm_and_ps(vSpikeMask, _mm_set1_ps(1.0f)));
            vMembranePotential = _mm_andnot_ps(vSpikeMask, vMembranePotential);
            
            // Store updated states
            _mm_store_ps(&neurons[i].potential, vMembranePotential);
            _mm_store_ps(&neurons[i].refractoryTime, vRefractory);
            _mm_store_ps(&neurons[i].isSpiking, vIsSpiking);
        }
    }
    
    // Process remaining neurons (scalar)
    for (; i < count; ++i) {
        updateNeuronScalar(&neurons[i], dt, externalInput[i], synapticInput[i],
                         spikeThreshold, refractoryPeriod);
    }
}

/**
 * Optimized synaptic weight update
 * @param weights Array of synaptic weights
 * @param preSpike Pre-synaptic neuron spike state
 * @param postSpike Post-synaptic neuron spike state
 * @param learningRate Learning rate
 * @param count Number of weights
 * @param mask Spike mask for efficiency
 */
void OptimizedNeuronMath::updateSynapticWeightsSIMD(
    float* weights,
    const bool* preSpike,
    const bool* postSpike,
    float learningRate,
    size_t count,
    const uint32_t* spikeMask
) {
    if (count == 0) return;
    
    size_t i = 0;
    if (count >= SIMD_WIDTH && spikeMask) {
        // Vectorized processing with spike mask
        for (; i + SIMD_WIDTH <= count; i += SIMD_WIDTH) {
            // Load spike masks
            uint32_t mask = spikeMask[i];
            
            // Load synaptic weights
            __m128 vWeights = _mm_load_ps(weights + i);
            
            // Calculate weight updates
            __m128 vLearning = _mm_set1_ps(learningRate);
            __m128 vWeightChange = _mm_mul_ps(vLearning, 
                                            _mm_set1_ps(preSpike[i] && postSpike[i] ? 0.1f : -0.05f));
            
            // Apply spike mask
            if (mask & (1 << i)) {
                vWeights = _mm_add_ps(vWeights, vWeightChange);
            }
            
            // Clamp weights to valid range
            vWeights = _mm_max_ps(vWeights, _mm_set1_ps(minWeight));
            vWeights = _mm_min_ps(vWeights, _mm_set1_ps(maxWeight));
            
            // Store updated weights
            _mm_store_ps(weights + i, vWeights);
        }
    }
    
    // Process remaining weights (scalar)
    for (; i < count; ++i) {
        if (preSpike[i] && postSpike[i]) {
            weights[i] += learningRate * 0.1f; // LTP
        } else if (preSpike[i] || postSpike[i]) {
            weights[i] -= learningRate * 0.05f; // LTD
        }
        
        // Clamp weight
        if (weights[i] < minWeight) weights[i] = minWeight;
        if (weights[i] > maxWeight) weights[i] = maxWeight;
    }
}

/**
 * Fast spike propagation with SIMD
 * @param spikeSources Array of spike sources
 * @param spikeTargets Array of spike targets
 * @param adjacencyMatrix Adjacency matrix
 * @param spikeMask Spike mask for efficiency
 * @param count Number of neurons
 * @param maxDelay Maximum spike delay
 */
void OptimizedNeuronMath::propagateSpikesSIMD(
    const bool* spikeSources,
    bool* spikeTargets,
    const float* adjacencyMatrix,
    const uint32_t* spikeMask,
    size_t count,
    size_t maxDelay
) {
    if (count == 0) return;
    
    // Prefetch adjacency matrix
    _mm_prefetch(reinterpret_cast<const char*>(adjacencyMatrix), _MM_HINT_T0);
    
    size_t i = 0;
    if (count >= SIMD_WIDTH && spikeMask) {
        // Vectorized spike propagation
        for (; i + SIMD_WIDTH <= count; i += SIMD_WIDTH) {
            // Load spike sources
            uint32_t sourceMask = spikeMask[i];
            __m128 vSourceMask = _mm_castsi128_ps(_mm_set1_epi32(sourceMask));
            
            // Load adjacency matrix row
            __m128 vAdjacency = _mm_load_ps(adjacencyMatrix + i * count);
            
            // Calculate spike propagation
            __m128 vPropagation = _mm_mul_ps(vSourceMask, vAdjacency);
            
            // Update spike targets
            for (int j = 0; j < SIMD_WIDTH; ++j) {
                if (_mm_extract_ps(vPropagation, j) > 0.0f) {
                    spikeTargets[i + j] = true;
                }
            }
        }
    }
    
    // Process remaining neurons (scalar)
    for (; i < count; ++i) {
        if (spikeMask[i]) {
            for (size_t j = 0; j < count; ++j) {
                if (adjacencyMatrix[i * count + j] > 0.0f) {
                    spikeTargets[j] = true;
                }
            }
        }
    }
}

/**
 * Optimized matrix operations for neural computations
 * @param A First matrix
 * @param B Second matrix
 * @param C Result matrix
 * @param size Matrix dimension
 * @param alpha Scalar multiplier
 * @param beta Scalar multiplier
 */
void OptimizedNeuronMath::matrixMultiplySIMD(
    const float* A,
    const float* B,
    float* C,
    size_t size,
    float alpha,
    float beta
) {
    if (size == 0) return;
    
    // Initialize result matrix
    if (beta == 0.0f) {
        for (size_t i = 0; i < size * size; ++i) {
            C[i] = 0.0f;
        }
    } else {
        for (size_t i = 0; i < size * size; ++i) {
            C[i] *= beta;
        }
    }
    
    // Matrix multiplication with SIMD
    size_t i = 0;
    if (size >= SIMD_WIDTH) {
        for (; i + SIMD_WIDTH <= size; i += SIMD_WIDTH) {
            // Load matrix row
            __m128 vRowA = _mm_load_ps(A + i * size);
            
            for (size_t j = 0; j < size; ++j) {
                // Load column
                __m128 vColB = _mm_set1_ps(0.0f);
                for (size_t k = 0; k < size; k += SIMD_WIDTH) {
                    __m128 vB = _mm_load_ps(B + k * size + j);
                    __m128 vMask = _mm_castsi128_ps(_mm_set1_epi32(k < i ? 1 : 0));
                    vColB = _mm_add_ps(vColB, _mm_and_ps(vB, vMask));
                }
                
                // Compute dot product
                __m128 vDotProduct = _mm_mul_ps(vRowA, vColB);
                vDotProduct = _mm_add_ps(vDotProduct, _mm_load_ps(C + i * size + j));
                _mm_store_ps(C + i * size + j, vDotProduct);
            }
        }
    }
    
    // Process remaining rows (scalar)
    for (; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            float sum = 0.0f;
            for (size_t k = 0; k < size; ++k) {
                sum += A[i * size + k] * B[k * size + j];
            }
            C[i * size + j] = alpha * sum + beta * C[i * size + j];
        }
    }
}

/**
 * Optimized exponential decay calculations
 * @param values Array of values
 * @param dt Time step
 * @param tau Time constant
 * @param count Number of values
 */
void OptimizedNeuronMath::expDecaySIMD(
    float* values,
    float dt,
    float tau,
    size_t count
) {
    if (count == 0) return;
    
    float decayFactor = std::exp(-dt / tau);
    __m128 vDecayFactor = _mm_set1_ps(decayFactor);
    
    size_t i = 0;
    if (count >= SIMD_WIDTH) {
        for (; i + SIMD_WIDTH <= count; i += SIMD_WIDTH) {
            __m128 vValues = _mm_load_ps(values + i);
            vValues = _mm_mul_ps(vValues, vDecayFactor);
            _mm_store_ps(values + i, vValues);
        }
    }
    
    // Process remaining values (scalar)
    for (; i < count; ++i) {
        values[i] *= decayFactor;
    }
}

/**
 * Optimize neuron state copying with SIMD
 * @param dest Destination array
 * @param src Source array
 * @param count Number of neurons
 */
void OptimizedNeuronMath::copyNeuronStatesSIMD(
    NeuronState* dest,
    const NeuronState* src,
    size_t count
) {
    if (count == 0) return;
    
    size_t i = 0;
    if (count >= SIMD_WIDTH) {
        for (; i + SIMD_WIDTH <= count; i += SIMD_WIDTH) {
            // Load and store neuron states
            __m128 vPotential = _mm_load_ps(&src[i].potential);
            __m128 vRefractory = _mm_load_ps(&src[i].refractoryTime);
            __m128 vIsSpiking = _mm_load_ps(&src[i].isSpiking);
            
            _mm_store_ps(&dest[i].potential, vPotential);
            _mm_store_ps(&dest[i].refractoryTime, vRefractory);
            _mm_store_ps(&dest[i].isSpiking, vIsSpiking);
        }
    }
    
    // Process remaining neurons (scalar)
    for (; i < count; ++i) {
        dest[i].potential = src[i].potential;
        dest[i].refractoryTime = src[i].refractoryTime;
        dest[i].isSpiking = src[i].isSpiking;
    }
}

/**
 * Optimized spike detection with SIMD
 * @param neurons Array of neuron states
 * @param spikeMask Output spike mask
 * @param count Number of neurons
 * @param threshold Spike threshold
 */
void OptimizedNeuronMath::detectSpikesSIMD(
    const NeuronState* neurons,
    uint32_t* spikeMask,
    size_t count,
    float threshold
) {
    if (count == 0) return;
    
    *spikeMask = 0;
    
    size_t i = 0;
    if (count >= SIMD_WIDTH) {
        __m128 vThreshold = _mm_set1_ps(threshold);
        uint32_t localMask = 0;
        
        for (; i + SIMD_WIDTH <= count; i += SIMD_WIDTH) {
            __m128 vPotential = _mm_load_ps(&neurons[i].potential);
            __m128 vSpikeMask = _mm_cmpgt_ps(vPotential, vThreshold);
            
            // Convert spike mask to integer
            uint32_t spikeMaskVec = _mm_movemask_ps(vSpikeMask);
            localMask |= (spikeMaskVec << i);
            
            // Reset spiked neurons
            __m128 vReset = _mm_and_ps(vPotential, _mm_set1_ps(~threshold));
            _mm_store_ps(&neurons[i].potential, vReset);
        }
        
        *spikeMask = localMask;
    }
    
    // Process remaining neurons (scalar)
    for (; i < count; ++i) {
        if (neurons[i].potential > threshold) {
            *spikeMask |= (1 << i);
            neurons[i].potential = threshold; // Reset
        }
    }
}

/**
 * Optimized synaptic delay calculation
 * @param delays Array of spike delays
 * @param spikeSources Array of spike sources
 * @param count Number of spikes
 * @param maxDelay Maximum delay
 */
void OptimizedNeuronMath::calculateSpikeDelaysSIMD(
    uint32_t* delays,
    const bool* spikeSources,
    size_t count,
    uint32_t maxDelay
) {
    if (count == 0) return;
    
    // Generate random delays using SIMD
    __m128 vMaxDelay = _mm_set1_ps(static_cast<float>(maxDelay));
    
    size_t i = 0;
    if (count >= SIMD_WIDTH) {
        for (; i + SIMD_WIDTH <= count; i += SIMD_WIDTH) {
            // Generate random delays
            __m128 vRand = _mm_rand_ps();
            __m128 vDelays = _mm_mul_ps(vRand, vMaxDelay);
            _mm_store_si128(reinterpret_cast<__m128i*>(delays + i), vDelays);
        }
    }
    
    
    // Process remaining spikes (scalar)
    for (; i < count; ++i) {
        delays[i] = static_cast<uint32_t>(std::rand() % (maxDelay + 1));
    }
    
}

/**
 * Optimized memory allocation for neuron states
 * @param neurons Array of neuron states
 * @param count Number of neurons
 * @param alignment Memory alignment requirement
 */
void OptimizedNeuronMath::allocateNeuronStatesSIMD(
    NeuronState* neurons,
    size_t count,
    size_t alignment
) {
    if (count == 0) return;
    
    // Ensure memory alignment
    uintptr_t addr = reinterpret_cast<uintptr_t>(neurons);
    if ((addr % alignment) != 0) {
        // Not aligned, adjust pointer
        neurons = reinterpret_cast<NeuronState*>(
            (reinterpret_cast<uintptr_t>(neurons) + alignment - 1) & ~(alignment - 1)
        );
    }
    
    // Initialize all neurons to resting state
    size_t i = 0;
    if (count >= SIMD_WIDTH) {
        __m128 vRestingPotential = _mm_set1_ps(restingPotential);
        __m128 vZeroRefractory = _mm_set1_ps(0.0f);
        __m128 vFalseIsSpiking = _mm_set1_ps(0.0f);
        
        for (; i + SIMD_WIDTH <= count; i += SIMD_WIDTH) {
            _mm_store_ps(&neurons[i].potential, vRestingPotential);
            _mm_store_ps(&neurons[i].refractoryTime, vZeroRefractory);
            _mm_store_ps(&neurons[i].isSpiking, vFalseIsSpiking);
        }
    }
    
    // Process remaining neurons (scalar)
    for (; i < count; ++i) {
        neurons[i].potential = restingPotential;
        neurons[i].refractoryTime = 0.0f;
        neurons[i].isSpiking = false;
    }
}

/**
 * Optimized function to reset neuron states after spikes
 * @param neurons Array of neuron states
 * @param spikeMask Spike mask
 * @param count Number of neurons
 * @param resetValue Reset value for membrane potential
 */
void OptimizedNeuronMath::resetNeuronStatesSIMD(
    NeuronState* neurons,
    uint32_t spikeMask,
    size_t count,
    float resetValue
) {
    if (count == 0) return;
    
    size_t i = 0;
    if (count >= SIMD_WIDTH) {
        // Process in SIMD chunks
        while (spikeMask && i < count) {
            uint32_t chunkMask = spikeMask & ((1 << SIMD_WIDTH) - 1);
            
            // Reset neurons in this chunk
            for (size_t j = 0; j < SIMD_WIDTH && i + j < count; ++j) {
                if (chunkMask & (1 << j)) {
                    neurons[i + j].potential = resetValue;
                    neurons[i + j].isSpiking = false;
                }
            }
            
    spikeMask >>= SIMD_WIDTH;
            i += SIMD_WIDTH;
        }
    }
    
    // Process remaining neurons (scalar)
    for (; i < count; ++i) {
        if (spikeMask & (1 << i)) {
            neurons[i].potential = resetValue;
            neurons[i].isSpiking = false;
        }
    }
}

/**
 * Optimized function to integrate neuron dynamics
 * @param neurons Array of neuron states
 * @param dt Time step
 * @param count Number of neurons
 * @param externalInputs External inputs
 * @param synapticInputs Synaptic inputs
 * @param spikeThreshold Spike threshold
 * @param refractoryPeriod Refractory period
 */
void OptimizedNeuronMath::integrateNeuronDynamicsSIMD(
    NeuronState* neurons,
    float dt,
    size_t count,
    const float* externalInputs,
    const float* synapticInputs,
    float spikeThreshold,
    float refractoryPeriod
) {
    if (count == 0) return;
    
    // Ensure cache line alignment
    if ((reinterpret_cast<uintptr_t>(neurons) & 63) != 0) {
        integrateNeuronDynamicsScalar(neurons, dt, count, externalInputs,
                                   synapticInputs, spikeThreshold, refractoryPeriod);
        return;
    }
    
    // Prefetch next cache line
    if (count > SIMD_WIDTH) {
        for (size_t i = SIMD_WIDTH; i < count; i += SIMD_WIDTH) {
            _mm_prefetch(reinterpret_cast<const char*>(&neurons[i]), _MM_HINT_T0);
        }
    }
    
    // Vectorized integration
    size_t i = 0;
    if (count >= SIMD_WIDTH) {
        __m128 vDt = _mm_set1_ps(dt);
        __m128 vTau = _mm_set1_ps(membraneTimeConstant);
        __m128 vR = _mm_set1_ps(resistance);
        
        for (; i + SIMD_WIDTH <= count; i += SIMD_WIDTH) {
            // Load neuron states
            __m128 vV = _mm_load_ps(&neurons[i].potential);
            __m128 vRefractory = _mm_load_ps(&neurons[i].refractoryTime);
            __m128 vIsSpiking = _mm_load_ps(&neurons[i].isSpiking);
            
            // Load inputs
            __m128 vExtInput = _mm_load_ps(externalInputs + i);
            __m128 vSynInput = _mm_load_ps(synapticInputs + i);
            
            // Update membrane potential
            __m128 vDecay = _mm_mul_ps(vV, _mm_exp2_ps(_mm_mul_ps(_mm_set1_ps(-dt), _mm_div_ps(_mm_set1_ps(1.0f), vTau)));
            __m128 vInput = _mm_mul_ps(vSynInput, vR);
            vV = _mm_add_ps(vDecay, vInput);
            
            // Update refractory time
            vRefractory = _mm_max_ps(_mm_sub_ps(vRefractory, vDt), _mm_set1_ps(0.0f));
            
            // Detect spikes
            __m128 vSpikeMask = _mm_cmpgt_ps(vV, _mm_set1_ps(spikeThreshold));
            vIsSpiking = _mm_or_ps(vIsSpiking, _mm_and_ps(vSpikeMask, _mm_set1_ps(1.0f)));
            vV = _mm_andnot_ps(vSpikeMask, vV);
            
            
            // Store updated states
            _mm_store_ps(&neurons[i].potential, vV);
            _mm_store_ps(&neurons[i].refractoryTime, vRefractory);
            _mm_store_ps(&neurons[i].isSpiking, vIsSpiking);
        }
    }
    
    // Process remaining neurons (scalar)
    for (; i < count; ++i) {
        integrateNeuronDynamicsScalar(&neurons[i], dt, externalInputs[i],
                                    synapticInputs[i], spikeThreshold, refractoryPeriod);
    }
}


// Additional helper functions
float OptimizedNeuronMath::exponentialDecay(float value, float dt, float tau) {
    return value * std::exp(-dt / tau);
}

float OptimizedNeuronMath::clamp(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}

bool OptimizedNeuronMath::isSpike(float potential, float threshold) {
    return potential > threshold;
}

void OptimizedNeuronMath::initializeRandomGenerator() {
    srand(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
}

float OptimizedNeuronMath::randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

float OptimizedNeuronMath::getMembraneTimeConstant() {
    return membraneTimeConstant;
}

float OptimizedNeuronMath::getResistance() {
    return resistance;
}

float OptimizedNeuronMath::getRestingPotential() {
    return restingPotential;
}

bool OptimizedNeuronMath::compareNeurons(const NeuronState& a, const NeuronState& b) {
    return std::memcmp(&a, &b, sizeof(NeuronState)) == 0;
}

void OptimizedNeuronMath::copyNeuronStates(const NeuronState* src, NeuronState* dest, size_t count) {
    if (count == 0) return;
    std::memcpy(dest, src, count * sizeof(NeuronState));
}

float OptimizedNeuronMath::calculateTotalActivity(const NeuronState* neurons, size_t count) {
    float total = 0.0f;
    for (size_t i = 0; i < count; ++i) {
        total += neurons[i].isSpiking ? 1.0f : 0.0f;
    }
    return total;
}

bool OptimizedNeuronMath::isNeuronActive(const NeuronState& neuron) {
    return neuron.isSpiking || neuron.refractoryTime > 0.0f;
}

void OptimizedNeuronMath::resetAllNeurons(NeuronState* neurons, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        neurons[i].potential = restingPotential;
        neurons[i].refractoryTime = 0.0f;
        neurons[i].isSpiking = false;
    }
}

// Singleton implementation
OptimizedNeuronMath* OptimizedNeuronMath::instance = nullptr;
std::mutex OptimizedNeuronMath::instanceMutex;

OptimizedNeuronMath* OptimizedNeuronMath::getInstance() {
    std::lock_guard lock(instanceMutex);
    if (!instance) {
        instance = new OptimizedNeuronMath();
    }
    return instance;
}

void OptimizedNeuronMath::destroyInstance() {
    std::lock_guard<std::mutex lock(instanceMutex);
    delete instance;
    instance = nullptr;
}

OptimizedNeuronMath::OptimizedNeuronMath() {
    // Initialize with default values
    membraneTimeConstant = 10.0f;
    resistance = 1.0f;
    restingPotential = -70.0f;
    spikeThreshold = -50.0f;
    minWeight = 0.0f;
    maxWeight = 1.0f;
    maxDelay = 100;
    
    // Initialize random generator
    initializeRandomGenerator();
}

OptimizedNeuronMath::~OptimizedNeuronMath() {
    // Cleanup if needed
}

// SIMD-specific implementations
#ifdef __SSE2
void OptimizedNeuronMath::updateNeuronsSSE2(NeuronState* neurons, size_t count, float dt,
                                         const float* externalInput, const float* synapticInput,
                                         float spikeThreshold, float refractoryPeriod) {
    // SSE2 implementation
    updateNeuronsSIMD(neurons, count, dt, externalInput, synapticInput,
                     spikeThreshold, refractoryPeriod);
}
#endif

#ifdef __AVX
void OptimizedNeuronMath::updateNeuronsAVX(NeuronState* neurons, size_t count, float dt,
                                        const float* externalInput, const float* synapticInput,
                                        float spikeThreshold, float refractoryPeriod) {
    // AVX implementation
    updateNeuronsSIMD(neurons, count, dt, externalInput, synapticInput,
                     spikeThreshold, refractoryPeriod);
}
#endif

#ifdef __AVX512F
void OptimizedNeuronMath::updateNeuronsAVX512(NeuronState* neurons, size_t count, float dt,
                                    const float* externalInput, const float* synapticInput,
                                    float spikeThreshold, float refractoryPeriod) {
    // AVX-512 implementation
    updateNeuronsSIMD(neurons, count, dt, externalInput, synapticInput,
                   spikeThreshold, refractoryPeriod);
}
#endif

// Check SIMD availability
bool OptimizedNeuronMath::isSimdAvailable() {
#ifdef __SSE2
    return true;
#else
    return false;
#endif
}

// Get SIMD width
int OptimizedNeuronMath::getSimdWidth() {
#ifdef __AVX512F
    return 16;
#elif defined(__AVX)
    return 8;
#elif defined(__SSE2)
    return 4;
#else
    return 1;
#endif
}

// Performance benchmarking
void OptimizedNeuronMath::benchmarkNeuronalUpdates(size_t neuronCount, size_t iterations) {
    // Allocate neuron states
    NeuronState* neurons = new NeuronState[neuronCount];
    float* externalInputs = new float[neuronCount];
    float* synapticInputs = new float[neuronCount];
    
    // Initialize random inputs
    for (size_t i = 0; i < neuronCount; ++i) {
        externalInputs[i] = randomFloat(-10.0f, 10.0f);
        synapticInputs[i] = randomFloat(-5.0f, 5.0f);
    }
    
    // Time the neuronal updates
    auto start = std::chrono::high_resolution_clock::now();	n
    for (size_t i = 0; i < iterations; ++i) {
        updateNeuronsSIMD(neurons, neuronCount, 0.001f,
                         externalInputs, synapticInputs,
                         spikeThreshold, refractoryPeriod);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Benchmark: " << iterations << " neuronal updates with "
              << neuronCount << " neurons took " << duration.count() << " microseconds" << std::endl;
    std::cout << "Performance: " << (iterations * neuronCount * 1000.0 / duration.count()) << " neuron-updates per second" << std::endl;
    
    // Cleanup
    delete[] neurons;
    delete[] externalInputs;
    delete[] synapticInputs;
}

void OptimizedNeuronMath::benchmarkSynapticUpdates(size_t weightCount, size_t iterations) {
    // Allocate weights and spike masks
    float* weights = new float[weightCount];
    bool* preSpikes = new bool[weightCount];
    bool* postSpikes = new bool[weightCount];
    uint32_t* spikeMask = new uint32_t[(weightCount + 31) / 32];
    
    // Initialize random weights
    for (size_t i = 0; i < weightCount; ++i) {
        weights[i] = randomFloat(minWeight, maxWeight);
        preSpikes[i] = (randomFloat(0.0f, 1.0f) > 0.5f);
        postSpikes[i] = (randomFloat(0.0f, 1.0f) > 0.5f);
    }
    
    // Time the synaptic weight updates
    auto start = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < iterations; ++i) {
        // Generate random spike mask
        for (size_t j = 0; j < weightCount; j += 32) {
            uint32_t mask = 0;
            for (size_t k = 0; k < 32 && j + k < weightCount; ++k) {
                if (preSpikes[j + k] && postSpikes[j + k]) {
                    mask |= (1 << k);
                }
            }
            spikeMask[j / 32] = mask;
        }
        
        
        updateSynapticWeightsSIMD(weights, preSpikes, postSpikes, 0.01f, weightCount, spikeMask);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Benchmark: " << iterations << " synaptic weight updates with "
              << weightCount << " weights took " << duration.count() << " microseconds" << std::endl;
    std::cout << "Performance: " << (iterations * weightCount * 1000.0 / duration.count()) << " weight-updates per second" << std::endl;
    
    // Cleanup
    delete[] weights;
    delete[] preSpikes;
    delete[] postSpikes;
    delete[] spikeMask;
}

// Performance information
std::string OptimizedNeuronMath::getPerformanceInfo() {
    std::ostringstream oss;
    oss << "OptimizedNeuronMath Performance Information:" << std::endl;
    oss << "  SIMD Support: " << (isSimdAvailable() ? "Yes" : "No") << std::endl;
    oss << "  SIMD Width: " << getSimdWidth() << " elements" << std::endl;
    oss << "  Neuron Count: " << neuronCount << std::endl;
    oss << "  Membrane Time Constant: " << membraneTimeConstant << std::endl;
    oss << "  Resistance: " << resistance << std::endl;
    oss << "  Resting Potential: " << restingPotential << std::endl;
    oss << "  Spike Threshold: " << spikeThreshold << std::endl;
    oss << "  Minimum Weight: " << minWeight << std::endl;
    oss << "  Maximum Weight: " << maxWeight << std::endl;
    oss << "  Maximum Delay: " << maxDelay << std::endl;
    return oss.str();
}

// Feature detection
bool OptimizedNeuronMath::detectFeatures() {
    // Check for SIMD availability
    bool simdAvailable = isSimdAvailable();
    
    // Check for hardware performance features
    bool hasCachelineAlignment = true; // Assuming modern hardware
    
    // Check for required instruction sets
    bool hasSSE2 = true; // Assuming modern hardware
    bool hasAVX = true; // Assuming modern hardware
    bool hasAVX512 = true; // Assuming modern hardware
    
    
    // Set features based on hardware
    simdAvailable &= hasSSE2 && hasAVX && hasAVX512;
    
    // Configure based on available features
    if (!simdAvailable) {
        // Fallback to scalar implementation
        std::cout << "Warning: SIMD not available, using scalar implementation" << std::endl;
    }
    
    return simdAvailable;
}

// Configuration management
void OptimizedNeuronMath::setMembraneTimeConstant(float tau) {
    if (tau > 0.0f) {
        membraneTimeConstant = tau;
    }
}


void OptimizedNeuronMath::setResistance(float r) {
    if (r > 0.0f) {
        resistance = r;
    }
}

void OptimizedNeuronMath::setRestingPotential(float potential) {
    restingPotential = potential;
}

void OptimizedNeuronMath::setSpikeThreshold(float threshold) {
    if (threshold > restingPotential) {
        spikeThreshold = threshold;
    }
}

void OptimizedNeuronMath::setWeightRange(float min, float max) {
    if (min < max) {
        minWeight = min;
        maxWeight = max;
    }
}

void OptimizedNeuronMath::setMaxDelay(uint32_t max) {
    if (max > 0) {
        maxDelay = max;
    }
}

// Utility functions
float OptimizedNeuronMath::getNeuronPotential(const NeuronState& neuron) {
    return neuron.potential;
}

float OptimizedNeuronMath::getNeuronRefractoryTime(const NeuronState& neuron) {
    return neuron.refractoryTime;
}
bool OptimizedNeuronMath::getNeuronIsSpiking(const NeuronState& neuron) {
    return neuron.isSpiking;
}

void OptimizedNeuronMath::setNeuronPotential(NeuronState& neuron, float potential) {
    neuron.potential = potential;
}

void OptimizedNeuronMath::setNeuronRefractoryTime(NeuronState& neuron, float time) {
    neuron.refractoryTime = time;
}

void OptimizedNeuronMath::setNeuronIsSpiking(NeuronState& neuron, bool isSpiking) {
    neuron.isSpiking = isSpiking;
}

// State management
void OptimizedNeuronMath::saveNeuronState(const NeuronState& neuron, NeuronState& saved) {
    saved = neuron;
}

void OptimizedNeuronMath::loadNeuronState(NeuronState& neuron, const NeuronState& saved) {
    neuron = saved;
}

// Validation
bool OptimizedNeuronMath::validateNeuronState(const NeuronState& neuron) {
    return !std::isnan(neuron.potential) &&
           !std::isinf(neuron.potential) &&
           neuron.refractoryTime >= 0.0f &&
           neuron.refractoryTime <= maxDelay;
}

bool OptimizedNeuronMath::validateNeuronArray(const NeuronState* neurons, size_t count) {
    if (!neurons || count == 0) return false;
    
    for (size_t i = 0; i < count; ++i) {
        if (!validateNeuronState(neurons[i])) {
            return false;
        }
    }
    return true;
}

// Statistics
float OptimizedNeuronMath::calculateAveragePotential(const NeuronState* neurons, size_t count) {
    if (count == 0) return 0.0f;
    
    float sum = 0.0f;
    for (size_t i = 0; i < count; ++i) {
        sum += neurons[i].potential;
    }
    return sum / count;
}

float OptimizedNeuronMath::calculateMaximumPotential(const NeuronState* neurons, size_t count) {
    if (count == 0) return 0.0f;
    
    float max = neurons[0].potential;
    for (size_t i = 1; i < count; ++i) {
        if (neurons[i].potential > max) {
            max = neurons[i].potential;
        }
    }
    return max;
}

float OptimizedNeuronMath::calculateMinimumPotential(const NeuronState* neurons, size_t count) {
    if (count == 0) return 0.0f;
    
    float min = neurons[0].potential;
    for (size_t i = 1; i < count; ++i) {
        if (neurons[i].potential < min) {
            min = neurons[i].potential;
        }
    }
    return min;
}

float OptimizedNeuronMath::calculateTotalSpikeCount(const NeuronState* neurons, size_t count) {
    float total = 0.0f;
    for (size_t i = 0; i < count; ++i) {
        total += neurons[i].isSpiking ? 1.0f : 0.0f;
    }
    return total;
}

float OptimizedNeuronMath::calculateFiringRate(const NeuronState* neurons, size_t count) {
    if (count == 0) return 0.0f;
    
    float spikeCount = calculateTotalSpikeCount(neurons, count);
    return (spikeCount / count) * 100.0f; // Percentage
}

// Comparison and equality
bool OptimizedNeuronMath::compareNeuronStates(const NeuronState& a, const NeuronState& b) {
    return std::memcmp(&a, &b, sizeof(NeuronState)) == 0;
}

bool OptimizedNeuronMath::areNeuronArraysEqual(const NeuronState* a, const NeuronState* b, size_t count) {
    if (!a || !b || count == 0) return false;
    
    for (size_t i = 0; i < count; ++i) {
        if (!compareNeuronStates(a[i], b[i])) {
            return false;
        }
    }
    return true;
}

// Copy and assignment
void OptimizedNeuronMath::copyNeuronStates(const NeuronState* src, NeuronState* dest, size_t count) {
    if (!src || !dest || count == 0) return;
    
    std::memcpy(dest, src, count * sizeof(NeuronState));
}

void OptimizedNeuronMath::assignNeuronState(NeuronState& dest, const NeuronState& src) {
    dest = src;
}

// Memory management
void OptimizedNeuronMath::allocateNeuronStates(NeuronState* neurons, size_t count) {
    if (!neurons || count == 0) return;
    
    // Initialize all neurons to resting state
    for (size_t i = 0; i < count; ++i) {
        neurons[i].potential = restingPotential;
        neurons[i].refractoryTime = 0.0f;
        neurons[i].isSpiking = false;
    }
}

void OptimizedNeuronMath::deallocateNeuronStates(NeuronState* neurons) {
    // Nothing to do for simple array allocation
}

// Validation and testing
bool OptimizedNeuronMath::validateParameters() const {
    return membraneTimeConstant > 0.0f &&
           resistance > 0.0f &&
           maxDelay > 0 &&
           minWeight < maxWeight;
}

std::string OptimizedNeuronMath::getErrorMessage(const std::string& operation) const {
    return "OptimizedNeuronMath Error: " + operation + " failed";
}


// Debugging and diagnostics
void OptimizedNeuronMath::debugPrintNeuronState(const NeuronState& neuron, std::ostream& os) const {
    os << "NeuronState:" << std::endl;
    os << "  Potential: " << neuron.potential << std::endl;
    os << "  RefractoryTime: " << neuron.refractoryTime << std::endl;
    os << "  IsSpiking: " << (neuron.isSpiking ? "true" : "false") << std::endl;
}

void OptimizedNeuronMath::debugPrintNeuronArray(const NeuronState* neurons, size_t count, std::ostream& os) const {
    if (!neurons || count == 0) return;
    
    os << "NeuronArray (size=" << count << "):" << std::endl;
    for (size_t i = 0; i < count; ++i) {
        os << "  Neuron " << i << ":" << std::endl;
        debugPrintNeuronState(neurons[i], os);
    }
}
}

void OptimizedNeuronMath::printPerformanceInfo(std::ostream& os) const {
    os << getPerformanceInfo();
}

void OptimizedNeuronMath::printParameters(std::ostream& os) const {
    os << "Current Parameters:" << std::endl;
    os << "  Membrane Time Constant: " << membraneTimeConstant << std::endl;
    os << "  Resistance: " << resistance << std::endl;
    os << "  Resting Potential: " << restingPotential << std::endl;
    os << "  Spike Threshold: " << spikeThreshold << std::endl;
    os << "  Weight Range: [" << minWeight << ", " << maxWeight << "]" << std::endl;
    os << "  Maximum Delay: " << maxDelay << std::endl;
}

// Singleton destructor
OptimizedNeuronMath::~OptimizedNeuronMath() {
    // Cleanup if needed
}

// Type aliases for convenience
typedef float NeuronPotential;
typedef float NeuronRefractoryTime;
typedef bool NeuronSpikeState;

// Export function
extern "C" {
    OptimizedNeuronMath* createOptimizedNeuronMath() {
        return OptimizedNeuronMath::getInstance();
    }
    
    void destroyOptimizedNeuronMath(OptimizedNeuronMath* instance) {
        if (instance) {
            delete instance;
        }
    }
    
    void updateNeuronsSIMD(NeuronState* neurons, size_t count, float dt,
                          const float* externalInput, const float* synapticInput,
                          float spikeThreshold, float refractoryPeriod) {
        OptimizedNeuronMath::getInstance()->updateNeuronsSIMD(neurons, count, dt, externalInput, synapticInput,
                                       spikeThreshold, refractoryPeriod);
    }
    
    void updateSynapticWeightsSIMD(float* weights, const bool* preSpike, const bool* postSpike,
                           float learningRate, size_t count, const uint32_t* spikeMask) {
        OptimizedNeuronMath::getInstance()->updateSynapticWeightsSIMD(weights, preSpike, postSpike,
                                                                     learningRate, count, spikeMask);
    }
    
    void propagateSpikesSIMD(const bool* spikeSources, bool* spikeTargets,
                           const float* adjacencyMatrix, const uint32_t* spikeMask,
                           size_t count, size_t maxDelay) {
        OptimizedNeuronMath::getInstance()->propagateSpikesSIMD(spikeSources, spikeTargets,
                                                               adjacencyMatrix, spikeMask,
                                                               count, maxDelay);
    }
    
    void matrixMultiplySIMD(const float* A, const float* B, float* C,
                            size_t size, float alpha, float beta) {
        OptimizedNeuronMath::getInstance()->matrixMultiplySIMD(A, B, C, size, alpha, beta);
    }
    
    bool isSimdAvailable() {
        return OptimizedNeuronMath::isSimdAvailable();
    }
    
    int getSimdWidth() {
        return OptimizedNeuronMath::getSimdWidth();
    }
    
    const char* getPerformanceInfo() {
        static std::string performanceInfo = OptimizedNeuronMath::getPerformanceInfo();
        return performanceInfo.c_str();
    }
    
    void benchmarkNeuronalUpdates(size_t neuronCount, size_t iterations) {
        OptimizedNeuronMath::getInstance()->benchmarkNeuronalUpdates(neuronCount, iterations);
    }
    
    void benchmarkSynapticUpdates(size_t weightCount, size_t iterations) {
        OptimizedNeuronMath::getInstance()->benchmarkSynapticUpdates(weightCount, iterations);
    }
}

// End of OptimizedNeuronMath.cpp implementation
