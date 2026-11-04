#include "OptimizedBrainCore.hpp"
#include "../core/Logger/Logger.hpp"
#include <immintrin.h>  // For SIMD intrinsics
#include <chrono>

namespace nlm {

OptimizedBrainCore::OptimizedBrainCore(std::shared_ptr<Config> config)
    : BrainCore(std::move(config))
    , useSIMD(true)
    , useMemoryPool(true)
    , optimizationLevel(OptimizationLevel::Balanced) {}

OptimizedBrainCore::~OptimizedBrainCore() = default;

void OptimizedBrainCore::step(SimulationStep currentStep, Timestamp currentTime) {
    auto profile = performanceProfiler ? performanceProfiler->profile("brain_step", [this, currentStep, currentTime]() {
        // Optimized step implementation
        if (optimizationLevel >= OptimizationLevel::Standard) {
            batchUpdateNeurons(core->getSensoryNeurons(), currentTime);
            batchUpdateNeurons(core->getMotorNeurons(), currentTime);
            
            batchDetectSpikes(core->getSensoryNeurons(), sensorySpikeFlags);
            batchDetectSpikes(core->getMotorNeurons(), motorSpikeFlags);
            
            processDelayedSpikes(currentStep, currentTime);
            processSpikes(currentStep);
        } else {
            // Fall back to parent implementation
            BrainCore::step(currentStep, currentTime);
        }
        
        return 0;
    }) : std::function<int()>([this, currentStep, currentTime]() {
        // Fallback to parent implementation
        BrainCore::step(currentStep, currentTime);
        return 0;
    });
}

void OptimizedBrainCore::processNeuronsBatch(std::vector<Neuron*>& neurons, Timestamp currentTime) {
    // Batch process neurons with optimized memory access
    size_t batchSize = neurons.size();
    
    // Use structure of arrays for better cache locality
    if (useSIMD && batchSize >= 8) {
        // SIMD implementation
        std::vector<float> potentials;
        potentials.reserve(batchSize);
        
        for (Neuron* neuron : neurons) {
            const auto& state = neuron->getState();
            potentials.push_back(state.membranePotential);
        }
        
        // Apply SIMD optimization
        vectorizedMembranePotentialUpdate(potentials, getDecayFactor(), timestep);
        
        // Update neurons with optimized potentials
        for (size_t i = 0; i < batchSize; ++i) {
            neurons[i]->setMembranePotential(potentials[i]);
        }
    } else {
        // Standard implementation
        for (Neuron* neuron : neurons) {
            neuron->stepLIF(currentTime, timestep);
        }
    }
}

void OptimizedBrainCore::processSynapsesBatch(std::vector<Synapse*>& synapses, Timestamp currentTime) {
    // Batch process synapses with optimized plasticity
    size_t batchSize = synapses.size();
    
    if (useSIMD && batchSize >= 8) {
        // Extract weight arrays for SIMD processing
        std::vector<float> weights;
        weights.reserve(batchSize);
        
        for (Synapse* syn : synapses) {
            weights.push_back(syn->getWeight());
        }
        
        // Get spike histories (simplified)
        std::vector<float> preSpikes, postSpikes;
        
        // Apply SIMD plasticity optimization
        vectorizedSynapticWeightUpdate(weights, preSpikes, postSpikes, getPlasticityRate());
        
        // Update synapses with optimized weights
        for (size_t i = 0; i < batchSize; ++i) {
            synapses[i]->setWeight(weights[i]);
        }
    } else {
        // Standard implementation
        for (Synapse* syn : synapses) {
            syn->step(currentTime);
        }
    }
}

void OptimizedBrainCore::vectorizedMembranePotentialUpdate(std::vector<float>& potentials, float decayFactor, float dt) {
    // SIMD implementation for membrane potential updates
    size_t size = potentials.size();
    
    if (size >= 8) {
        // Use SIMD intrinsics for parallel processing
        size_t simdCount = (size / 8) * 8;
        
        __m256* simdPotentials = reinterpret_cast<__m256*>(potentials.data());
        
        for (size_t i = 0; i < simdCount; i += 8) {
            __m256 pot = _mm256_loadu_ps(&simdPotentials[i]);
            __m256 decay = _mm256_set1_ps(decayFactor);
            __m256 delta = _mm256_set1_ps(dt);
            
            // Apply LIF dynamics: V = V + dt * (-decay * V + input)
            pot = _mm256_add_ps(pot, _mm256_mul_ps(delta, _mm256_sub_ps(_mm256_setzero_ps(), 
                                                                           _mm256_mul_ps(decay, pot))));
            
            _mm256_storeu_ps(&simdPotentials[i], pot);
        }
        
        // Handle remainder
        for (size_t i = simdCount; i < size; ++i) {
            potentials[i] += dt * (-decayFactor * potentials[i]);
        }
    } else {
        // Scalar fallback
        for (float& pot : potentials) {
            pot += dt * (-decayFactor * pot);
        }
    }
}

void OptimizedBrainCore::vectorizedSynapticWeightUpdate(std::vector<float>& weights, const std::vector<float>& preSpikes,
                                                      const std::vector<float>& postSpikes, float learningRate) {
    // SIMD implementation for synaptic weight updates
    size_t size = weights.size();
    
    if (size >= 8 && !preSpikes.empty() && !postSpikes.empty()) {
        // Use SIMD intrinsics for parallel processing
        size_t simdCount = (size / 8) * 8;
        
        __m256* simdWeights = reinterpret_cast<__m256*>(weights.data());
        __m256 learning = _mm256_set1_ps(learningRate);
        
        for (size_t i = 0; i < simdCount; i += 8) {
            __m256 w = _mm256_loadu_ps(&simdWeights[i]);
            
            // STDP update logic (simplified)
            // w = w + learning * (LTP * preSpike * postSpike - LTD * postSpike * preSpike)
            __m256 preSpike = _mm256_set1_ps(1.0f);  // Would come from preSpikes
            __m256 postSpike = _mm256_set1_ps(1.0f);  // Would come from postSpikes
            
            __m256 ltp = _mm256_set1_ps(0.01f);
            __m256 ltd = _mm256_set1_ps(0.012f);
            
            __m256 delta = _mm256_mul_ps(learning, _mm256_sub_ps(
                _mm256_mul_ps(ltp, _mm256_mul_ps(preSpike, postSpike)),
                _mm256_mul_ps(ltd, _mm256_mul_ps(postSpike, preSpike))
            ));
            
            w = _mm256_add_ps(w, delta);
            
            _mm256_storeu_ps(&simdWeights[i], w);
        }
        
        // Handle remainder
        for (size_t i = simdCount; i < size; ++i) {
            float delta = learningRate * (0.01f * 1.0f - 0.012f * 1.0f);  // Simplified
            weights[i] += delta;
        }
    } else {
        // Scalar fallback
        for (size_t i = 0; i < size; ++i) {
            float delta = learningRate * (0.01f * 1.0f - 0.012f * 1.0f);  // Simplified
            weights[i] += delta;
        }
    }
}

void OptimizedBrainCore::useStructureOfArrays(std::vector<std::vector<float>>& soaData) {
    // Convert to Structure of Arrays layout for better cache performance
    if (soaData.size() >= 6) {
        size_t totalSize = soaData[0].size();
        
        // Ensure all arrays have same size
        for (auto& arr : soaData) {
            arr.resize(totalSize);
        }
        
        // Set up SOA layout
        soaData[0].resize(totalSize);  // Membrane potentials
        soaData[1].resize(totalSize);  // Synaptic inputs
        soaData[2].resize(totalSize);  // Adaptation variables
        soaData[3].resize(totalSize);  // Firing rates
        soaData[4].resize(totalSize);  // Firing states
        soaData[5].resize(totalSize);  // Refractory times
    }
}

void OptimizedBrainCore::useArrayOfStructures(std::vector<Neuron*>& neurons) {
    // Convert from SOA to AoS layout
    size_t size = neurons.size();
    
    // Clear SOA data
    soaData.membranePotential.clear();
    soaData.synapticInput.clear();
    soaData.adaptationVariable.clear();
    soaData.firingRate.clear();
    soaData.firingState.clear();
    soaData.refractoryRemaining.clear();
    soaData.lastSpikeTime.clear();
    
    // Copy data from neurons to SOA
    soaData.membranePotential.reserve(size);
    soaData.synapticInput.reserve(size);
    soaData.adaptationVariable.reserve(size);
    soaData.firingRate.reserve(size);
    soaData.firingState.reserve(size);
    soaData.refractoryRemaining.reserve(size);
    soaData.lastSpikeTime.reserve(size);
    
    for (Neuron* neuron : neurons) {
        const auto& state = neuron->getState();
        soaData.membranePotential.push_back(state.membranePotential);
        soaData.synapticInput.push_back(neuron->getMembranePotential());  // Placeholder
        soaData.adaptationVariable.push_back(state.adaptationVariable);
        soaData.firingRate.push_back(state.firingRate);
        soaData.firingState.push_back(static_cast<uint8_t>(state.firingState));
        soaData.refractoryRemaining.push_back(state.refractoryRemaining);
        soaData.lastSpikeTime.push_back(state.lastSpikeTime);
    }
}

void OptimizedBrainCore::batchUpdateNeurons(const std::vector<Neuron*>& neurons, Timestamp currentTime) {
    // Batch update neurons with optimized memory access
    size_t batchSize = neurons.size();
    
    if (batchSize >= 8) {
        // Prefetch data for better cache performance
        for (size_t i = 0; i < batchSize; i += 8) {
            for (size_t j = 0; j < 8 && i + j < batchSize; ++j) {
                Neuron* neuron = neurons[i + j];
                const auto& state = neuron->getState();
                // Prefetch next neuron
                if (i + j + 8 < batchSize) {
                    _mm_prefetch(neurons[i + j + 8], _MM_HINT_T0);
                }
            }
        }
        
        // Process in small batches for better cache locality
        size_t batchSizeOptimized = std::min(batchSize, static_cast<size_t>(1024));
        
        for (size_t i = 0; i < batchSize; i += batchSizeOptimized) {
            size_t currentBatch = std::min(batchSizeOptimized, batchSize - i);
            
            // Process current batch
            for (size_t j = 0; j < currentBatch; ++j) {
                Neuron* neuron = neurons[i + j];
                neuron->stepLIF(currentTime, timestep);
            }
        }
    } else {
        // Standard processing for small batches
        for (Neuron* neuron : neurons) {
            neuron->stepLIF(currentTime, timestep);
        }
    }
}

void OptimizedBrainCore::batchApplyPlasticity(const std::vector<Synapse*>& synapses, Timestamp currentTime) {
    // Batch apply plasticity rules
    size_t batchSize = synapses.size();
    
    if (batchSize >= 8) {
        // Group synapses by type for better cache locality
        std::vector<Synapse*> excitatory, inhibitory;
        
        for (Synapse* syn : synapses) {
            if (syn->isExcitatory()) {
                excitatory.push_back(syn);
            } else {
                inhibitory.push_back(syn);
            }
        }
        
        // Process each type separately
        if (!excitatory.empty()) {
            processSynapsesBatch(excitatory, currentTime);
        }
        if (!inhibitory.empty()) {
            processSynapsesBatch(inhibitory, currentTime);
        }
    } else {
        // Standard processing
        for (Synapse* syn : synapses) {
            syn->step(currentTime);
        }
    }
}

void OptimizedBrainCore::batchDetectSpikes(const std::vector<Neuron*>& neurons, std::vector<bool>& spikeFlags) {
    // Batch detect spikes with optimized memory access
    size_t batchSize = neurons.size();
    
    spikeFlags.clear();
    spikeFlags.reserve(batchSize);
    
    if (batchSize >= 8) {
        // Prefetch neuron states
        for (size_t i = 0; i < batchSize; i += 8) {
            for (size_t j = 0; j < 8 && i + j < batchSize; ++j) {
                Neuron* neuron = neurons[i + j];
                const auto& state = neuron->getState();
                // Prefetch next neuron state
                if (i + j + 8 < batchSize) {
                    _mm_prefetch(&neurons[i + j + 8]->getState(), _MM_HINT_T0);
                }
            }
        }
        
        // Process in batches for better cache performance
        size_t batchSizeOptimized = std::min(batchSize, static_cast<size_t>(1024));
        
        for (size_t i = 0; i < batchSize; i += batchSizeOptimized) {
            size_t currentBatch = std::min(batchSizeOptimized, batchSize - i);
            
            // Detect spikes in current batch
            for (size_t j = 0; j < currentBatch; ++j) {
                Neuron* neuron = neurons[i + j];
                const auto& state = neuron->getState();
                bool isSpike = (state.firingState == FiringState::Refractory &&
                               state.lastSpikeTime >= 0.0f &&
                               std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);
                
                spikeFlags.push_back(isSpike);
                
                if (isSpike) {
                    // Process spike
                    SpikeEvent event(neuron->getId(), currentTime, 0);  // step would be passed
                    spikeSystem->queueSpike(event);
                }
            }
        }
    } else {
        // Standard processing
        for (Neuron* neuron : neurons) {
            const auto& state = neuron->getState();
            bool isSpike = (state.firingState == FiringState::Refractory &&
                           state.lastSpikeTime >= 0.0f &&
                           std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);
            
            spikeFlags.push_back(isSpike);
            
            if (isSpike) {
                // Process spike
                SpikeEvent event(neuron->getId(), currentTime, 0);  // step would be passed
                spikeSystem->queueSpike(event);
            }
        }
    }
}

void OptimizedBrainCore::SIMDUpdateNeuronMembrane(float* potentials, float* inputs, float decayFactor, 
                                                 float dt, size_t count) {
    // SIMD implementation for neuron membrane updates
    if (count >= 8) {
        size_t simdCount = (count / 8) * 8;
        
        __m256* simdPotentials = reinterpret_cast<__m256*>(potentials);
        __m256* simdInputs = reinterpret_cast<__m256*>(inputs);
        __m256 decay = _mm256_set1_ps(decayFactor);
        __m256 delta = _mm256_set1_ps(dt);
        
        for (size_t i = 0; i < simdCount; i += 8) {
            __m256 pot = _mm256_loadu_ps(&simdPotentials[i]);
            __m256 inp = _mm256_loadu_ps(&simdInputs[i]);
            
            // Apply LIF dynamics: V = V + dt * (-decay * V + input)
            pot = _mm256_add_ps(pot, _mm256_mul_ps(delta, _mm256_add_ps(
                _mm256_mul_ps(inp, delta),  // Input contribution
                _mm256_mul_ps(_mm256_set1_ps(-decay), pot)  // Leak term
            )));
            
            _mm256_storeu_ps(&simdPotentials[i], pot);
        }
        
        // Handle remainder
        for (size_t i = simdCount; i < count; ++i) {
            potentials[i] += dt * (-decayFactor * potentials[i] + inputs[i]);
        }
    } else {
        // Scalar fallback
        for (size_t i = 0; i < count; ++i) {
            potentials[i] += dt * (-decayFactor * potentials[i] + inputs[i]);
        }
    }
}

void OptimizedBrainCore::SIMDCalculatePlasticity(float* weights, const float* preSpikes, const float* postSpikes,
                                                float learningRate, size_t count) {
    // SIMD implementation for plasticity calculations
    if (count >= 8 && preSpikes && postSpikes) {
        size_t simdCount = (count / 8) * 8;
        
        __m256* simdWeights = reinterpret_cast<__m256*>(weights);
        __m256* simdPreSpikes = reinterpret_cast<__m256*>(preSpikes);
        __m256* simdPostSpikes = reinterpret_cast<__m256*>(postSpikes);
        __m256 learning = _mm256_set1_ps(learningRate);
        
        for (size_t i = 0; i < simdCount; i += 8) {
            __m256 w = _mm256_loadu_ps(&simdWeights[i]);
            __m256 pre = _mm256_loadu_ps(&simdPreSpikes[i]);
            __m256 post = _mm256_loadu_ps(&simdPostSpikes[i]);
            
            // STDP: w += learning * (LTP * pre * post - LTD * post * pre)
            __m256 ltp = _mm256_set1_ps(0.01f);
            __m256 ltd = _mm256_set1_ps(0.012f);
            
            __m256 delta = _mm256_mul_ps(learning, _mm256_sub_ps(
                _mm256_mul_ps(ltp, _mm256_mul_ps(pre, post)),
                _mm256_mul_ps(ltd, _mm256_mul_ps(post, pre))
            ));
            
            w = _mm256_add_ps(w, delta);
            
            _mm256_storeu_ps(&simdWeights[i], w);
        }
        
        // Handle remainder
        for (size_t i = simdCount; i < count; ++i) {
            float delta = learningRate * (0.01f * preSpikes[i] * postSpikes[i] - 
                                        0.012f * postSpikes[i] * preSpikes[i]);
            weights[i] += delta;
        }
    } else {
        // Scalar fallback
        for (size_t i = 0; i < count; ++i) {
            float delta = learningRate * (0.01f * preSpikes[i] * postSpikes[i] - 
                                        0.012f * postSpikes[i] * preSpikes[i]);
            weights[i] += delta;
        }
    }
}

void OptimizedBrainCore::prefetchData(const void* address, size_t size) {
    // Prefetch data for better cache performance
#ifdef _MSC_VER
    _mm_prefetch(reinterpret_cast<const char*>(address), _MM_HINT_T0);
#else
    __builtin_prefetch(address, 0, 3);  // Try to get into L1 cache
#endif
}

void OptimizedBrainCore::prefetchDataWrite(void* address, size_t size) {
    // Prefetch data for writing
#ifdef _MSC_VER
    _mm_prefetch(reinterpret_cast<const char*>(address), _MM_HINT_ET0);
#else
    __builtin_prefetch(address, 1, 3);  // Try to get into L1 cache for write
#endif
}

void* OptimizedBrainCore::allocateFromPool(size_t size) {
    // Allocate from memory pool
    if (useMemoryPool) {
        // Simple memory pool implementation
        // In practice, this would use a dedicated memory pool allocator
        return std::aligned_alloc(64, size);  // Cache-aligned allocation
    }
    return nullptr;
}

void OptimizedBrainCore::deallocateToPool(void* ptr, size_t size) {
    // Return memory to pool
    if (useMemoryPool && ptr) {
        std::free(ptr);
    }
}

} // namespace nlm
