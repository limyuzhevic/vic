#include "OptimizedNeuralDynamics.hpp"
#include "SIMDVectorization.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "<algorithm>"
#include "<numeric>
#include "<cmath>
#include "<chrono>

namespace nlm {

// OptimizedNeuralDynamics implementation
OptimizedNeuralDynamics::OptimizedNeuralDynamics(SimulationStep maxHistorySteps)
    : maxHistorySteps_(maxHistorySteps)
    , enableSIMD_(true)
    , enableParallel_(true)
    , enableMemPool_(true)
    , simulationFrequency_(1000.0f)  // 1kHz default
{
}

OptimizedNeuralDynamics::~OptimizedNeuralDynamics() = default;

void OptimizedNeuralDynamics::stepNeurons(const std::vector<Neuron*>& neurons,
                                          SimulationStep currentStep,
                                          Timestamp currentTime,
                                          const TimestepDuration timestep,
                                          const NeuralDynamicsCallbacks& callbacks) {
    if (neurons.empty()) return;
    
    // Update neuron states using optimized methods
    if (enableSIMD_ && SIMDUtils::isSIMDAvailable() && neurons.size() >= SIMDUtils::SIMD_WIDTH) {
        stepNeuronsSIMD(neurons, currentStep, currentTime, timestep, callbacks);
    } else {
        stepNeuronsScalar(neurons, currentStep, currentTime, timestep, callbacks);
    }
    
    // Record history for analysis
    if (maxHistorySteps_ > 0) {
        neuralHistory_.push_back(NeuronStepRecord(currentStep, currentTime, neurons));
        if (neuralHistory_.size() > maxHistorySteps_) {
            neuralHistory_.pop_front();
        }
    }
}

void OptimizedNeuralDynamics::stepSynapses(const std::vector<Synapse*>& synapses,
                                           const std::vector<Neuron*>& neurons,
                                           SimulationStep currentStep,
                                           Timestamp currentTime,
                                           const TimestepDuration timestep,
                                           const NeuralDynamicsCallbacks& callbacks) {
    if (synapses.empty()) return;
    
    // Build neuron lookup table for efficient access
    std::unordered_map<NeuronId, Neuron*> neuronLookup;
    for (auto* neuron : neurons) {
        neuronLookup[neuron->getId()] = neuron;
    }
    
    // Process synapses in optimized batches
    if (enableSIMD_ && SIMDUtils::isSIMDAvailable() && synapses.size() >= SIMDUtils::SIMD_WIDTH) {
        stepSynapsesSIMD(synapses, neuronLookup, currentStep, currentTime, timestep, callbacks);
    } else {
        stepSynapsesScalar(synapses, neuronLookup, currentStep, currentTime, timestep, callbacks);
    }
    
    // Record synapse history
    if (maxHistorySteps_ > 0) {
        synapseHistory_.push_back(SynapseStepRecord(currentStep, currentTime, synapses));
        if (synapseHistory_.size() > maxHistorySteps_) {
            synapseHistory_.pop_front();
        }
    }
}

void OptimizedNeuralDynamics::processSpikes(const std::vector<SpikeEvent>& spikes,
                                            const std::vector<Neuron*>& neurons,
                                            const NeuralDynamicsCallbacks& callbacks) {
    if (spikes.empty()) return;
    
    // Build neuron lookup for fast access
    std::unordered_map<NeuronId, Neuron*> neuronLookup;
    for (auto* neuron : neurons) {
        neuronLookup[neuron->getId()] = neuron;
    }
    
    // Process spikes in optimized manner
    if (enableSIMD_ && SIMDUtils::isSIMDAvailable() && spikes.size() >= SIMDUtils::SIMD_WIDTH) {
        processSpikesSIMD(spikes, neuronLookup, callbacks);
    } else {
        processSpikesScalar(spikes, neuronLookup, callbacks);
    }
    
    // Update firing statistics
    updateFiringStats(spikes, neurons);
}

void OptimizedNeuralDynamics::stepNeuronsSIMD(const std::vector<Neuron*>& neurons,
                                              SimulationStep currentStep,
                                              Timestamp currentTime,
                                              const TimestepDuration timestep,
                                              const NeuralDynamicsCallbacks& callbacks) {
    // Process neurons in SIMD batches
    size_t batchSize = SIMDUtils::SIMD_WIDTH;
    VectorizedLIF vectorizer;
    vectorizer.V_rest = -70.0f;
    vectorizer.V_reset = -70.0f;
    vectorizer.V_threshold = -55.0f;
    vectorizer.tau = 20.0f;
    vectorizer.C = 1.0f;
    vectorizer.dt = static_cast<float>(timestep);
    vectorizer.refractoryDecay = 5.0f;
    
    size_t i = 0;
    size_t vectorSize = neurons.size() - (neurons.size() % batchSize);
    
    // Allocate arrays for SIMD processing
    alignas(64) float membranePotentials[batchSize];
    alignas(64) float synapticInputs[batchSize];
    alignas(64) uint32_t refractoryRemaining[batchSize];
    alignas(64) uint8_t fired[batchSize];
    
    for (; i < vectorSize; i += batchSize) {
        // Load neuron data into SIMD arrays
        for (size_t j = 0; j < batchSize; ++j) {
            const auto& state = neurons[i + j]->getState();
            membranePotentials[j] = state.membranePotential;
            synapticInputs[j] = state.synapseConductance;
            refractoryRemaining[j] = state.refractoryRemaining;
            fired[j] = (state.firingState == FiringState::Refractory) ? 1 : 0;
        }
        
        // Apply vectorized LIF update
        vectorizer.update(membranePotentials, synapticInputs, refractoryRemaining, fired, batchSize);
        
        // Store results back to neurons
        for (size_t j = 0; j < batchSize; ++j) {
            neurons[i + j]->setMembranePotential(membranePotentials[j]);
            neurons[i + j]->getState().refractoryRemaining = refractoryRemaining[j];
            
            if (fired[j]) {
                neurons[i + j]->setFiringState(FiringState::Spiking);
                neurons[i + j]->getState().lastSpikeTime = currentTime;
                neurons[i + j]->getState().refractoryRemaining = static_cast<uint32_t>(vectorizer.refractoryDecay);
                
                // Call spike callback
                if (callbacks.onNeuronSpike) {
                    callbacks.onNeuronSpike(neurons[i + j]->getId(), currentTime, currentStep);
                }
            } else {
                neurons[i + j]->setFiringState(FiringState::Resting);
            }
        }
    }
    
    // Process remaining neurons (scalar)
    for (; i < neurons.size(); ++i) {
        auto* neuron = neurons[i];
        const auto& state = neuron->getState();
        
        // Simple LIF update (scalar)
        float leak = (state.restingPotential - state.membranePotential) / state.leakConductance;
        float input = state.synapseConductance / state.leakConductance;
        float newPotential = state.membranePotential + timestep * (leak + input);
        
        if (newPotential >= state.threshold) {
            newPotential = state.resetPotential;
            neuron->setFiringState(FiringState::Refractory);
            neuron->getState().lastSpikeTime = currentTime;
            neuron->getState().refractoryRemaining = state.refractoryPeriod;
            
            if (callbacks.onNeuronSpike) {
                callbacks.onNeuronSpike(neuron->getId(), currentTime, currentStep);
            }
        } else {
            neuron->setFiringState(FiringState::Resting);
        }
        
        neuron->setMembranePotential(newPotential);
    }
}

void OptimizedNeuralDynamics::stepNeuronsScalar(const std::vector<Neuron*>& neurons,
                                                SimulationStep currentStep,
                                                Timestamp currentTime,
                                                const TimestepDuration timestep,
                                                const NeuralDynamicsCallbacks& callbacks) {
    // Process neurons one by one (scalar fallback)
    for (auto* neuron : neurons) {
        const auto& state = neuron->getState();
        
        // LIF update
        float leak = (state.restingPotential - state.membranePotential) / state.leakConductance;
        float input = state.synapseConductance / state.leakConductance;
        float newPotential = state.membranePotential + timestep * (leak + input);
        
        if (newPotential >= state.threshold) {
            newPotential = state.resetPotential;
            neuron->setFiringState(FiringState::Refractory);
            neuron->getState().lastSpikeTime = currentTime;
            neuron->getState().refractoryRemaining = state.refractoryPeriod;
            
            if (callbacks.onNeuronSpike) {
                callbacks.onNeuronSpike(neuron->getId(), currentTime, currentStep);
            }
        } else {
            neuron->setFiringState(FiringState::Resting);
        }
        
        neuron->setMembranePotential(newPotential);
        
        // Update refractory counters
        if (state.refractoryRemaining > 0) {
            neuron->getState().refractoryRemaining--;
            if (neuron->getState().refractoryRemaining == 0) {
                neuron->setFiringState(FiringState::Resting);
            }
        }
    }
}

void OptimizedNeuralDynamics::stepSynapsesSIMD(const std::vector<Synapse*>& synapses,
                                              const std::unordered_map<NeuronId, Neuron*>& neuronLookup,
                                              SimulationStep currentStep,
                                              Timestamp currentTime,
                                              const TimestepDuration timestep,
                                              const NeuralDynamicsCallbacks& callbacks) {
    size_t batchSize = SIMDUtils::SIMD_WIDTH;
    size_t vectorSize = synapses.size() - (synapses.size() % batchSize);
    
    alignas(64) float weights[batchSize];
    alignas(64) float delays[batchSize];
    alignas(64) uint8_t types[batchSize];
    
    size_t i = 0;
    for (; i < vectorSize; i += batchSize) {
        // Load synapse data
        for (size_t j = 0; j < batchSize; ++j) {
            const auto& syn = *synapses[i + j];
            weights[j] = syn.getWeight();
            delays[j] = syn.getDelay();
            types[j] = static_cast<uint8_t>(syn.getType());
        }
        
        // Apply synaptic integration
        for (size_t j = 0; j < batchSize; ++j) {
            NeuronId destId = synapses[i + j]->getDestinationNeuron();
            auto it = neuronLookup.find(destId);
            if (it != neuronLookup.end()) {
                float synapticCurrent = weights[j] * 10.0f;  // Scale factor
                if (types[j] == static_cast<uint8_t>(SynapseType::Excitatory)) {
                    it->second->receiveExcitatoryInput(synapticCurrent);
                } else {
                    it->second->receiveInhibitoryInput(-synapticCurrent);
                }
                
                if (callbacks.onSynapticInput) {
                    callbacks.onSynapticInput(destId, weights[j], delays[j], currentStep);
                }
            }
        }
    }
    
    // Process remaining synapses (scalar)
    for (; i < synapses.size(); ++i) {
        const auto& syn = *synapses[i];
        NeuronId destId = syn.getDestinationNeuron();
        auto it = neuronLookup.find(destId);
        if (it != neuronLookup.end()) {
            float synapticCurrent = syn.getWeight() * 10.0f;
            if (syn.getType() == SynapseType::Excitatory) {
                it->second->receiveExcitatoryInput(synapticCurrent);
            } else {
                it->second->receiveInhibitoryInput(-synapticCurrent);
            }
            
            if (callbacks.onSynapticInput) {
                callbacks.onSynapticInput(destId, syn.getWeight(), syn.getDelay(), currentStep);
            }
        }
    }
}

void OptimizedNeuralDynamics::stepSynapsesScalar(const std::vector<Synapse*>& synapses,
                                                const std::unordered_map<NeuronId, Neuron*>& neuronLookup,
                                                SimulationStep currentStep,
                                                Timestamp currentTime,
                                                const TimestepDuration timestep,
                                                const NeuralDynamicsCallbacks& callbacks) {
    // Process synapses one by one (scalar)
    for (const auto& syn : synapses) {
        NeuronId destId = syn->getDestinationNeuron();
        auto it = neuronLookup.find(destId);
        if (it != neuronLookup.end()) {
            float synapticCurrent = syn->getWeight() * 10.0f;
            if (syn->getType() == SynapseType::Excitatory) {
                it->second->receiveExcitatoryInput(synapticCurrent);
            } else {
                it->second->receiveInhibitoryInput(-synapticCurrent);
            }
            
            if (callbacks.onSynapticInput) {
                callbacks.onSynapticInput(destId, syn->getWeight(), syn->getDelay(), currentStep);
            }
        }
    }
}

void OptimizedNeuralDynamics::processSpikesSIMD(const std::vector<SpikeEvent>& spikes,
                                                const std::unordered_map<NeuronId, Neuron*>& neuronLookup,
                                                const NeuralDynamicsCallbacks& callbacks) {
    size_t batchSize = SIMDUtils::SIMD_WIDTH;
    size_t vectorSize = spikes.size() - (spikes.size() % batchSize);
    
    alignas(64) NeuronId sourceNeurons[batchSize];
    alignas(64) NeuronId destNeurons[batchSize];
    alignas(64) float weights[batchSize];
    alignas(64) uint8_t types[batchSize];
    
    size_t i = 0;
    for (; i < vectorSize; i += batchSize) {
        // Load spike data
        for (size_t j = 0; j < batchSize; ++j) {
            const auto& spike = spikes[i + j];
            sourceNeurons[j] = spike.source_neuron;
            destNeurons[j] = spike.destination_neuron;
            weights[j] = spike.weight;
            types[j] = spike.is_excitatory ? 1 : 0;
        }
        
        // Deliver spikes to neurons
        for (size_t j = 0; j < batchSize; ++j) {
            auto it = neuronLookup.find(destNeurons[j]);
            if (it != neuronLookup.end()) {
                float synapticInput = weights[j] * 10.0f;
                if (types[j]) {
                    it->second->receiveExcitatoryInput(synapticInput);
                } else {
                    it->second->receiveInhibitoryInput(-synapticInput);
                }
                
                if (callbacks.onSpikeDelivery) {
                    callbacks.onSpikeDelivery(sourceNeurons[j], destNeurons[j], weights[j], currentStep);
                }
            }
        }
    }
    
    // Process remaining spikes (scalar)
    for (; i < spikes.size(); ++i) {
        const auto& spike = spikes[i];
        auto it = neuronLookup.find(spike.destination_neuron);
        if (it != neuronLookup.end()) {
            float synapticInput = spike.weight * 10.0f;
            if (spike.is_excitatory) {
                it->second->receiveExcitatoryInput(synapticInput);
            } else {
                it->second->receiveInhibitoryInput(-synapticInput);
            }
            
            if (callbacks.onSpikeDelivery) {
                callbacks.onSpikeDelivery(spike.source_neuron, spike.destination_neuron, spike.weight, currentStep);
            }
        }
    }
}

void OptimizedNeuralDynamics::processSpikesScalar(const std::vector<SpikeEvent>& spikes,
                                                  const std::unordered_map<NeuronId, Neuron*>& neuronLookup,
                                                  const NeuralDynamicsCallbacks& callbacks) {
    // Process spikes one by one (scalar)
    for (const auto& spike : spikes) {
        auto it = neuronLookup.find(spike.destination_neuron);
        if (it != neuronLookup.end()) {
            float synapticInput = spike.weight * 10.0f;
            if (spike.is_excitatory) {
                it->second->receiveExcitatoryInput(synapticInput);
            } else {
                it->second->receiveInhibitoryInput(-synapticInput);
            }
            
            if (callbacks.onSpikeDelivery) {
                callbacks.onSpikeDelivery(spike.source_neuron, spike.destination_neuron, spike.weight, currentStep);
            }
        }
    }
}

void OptimizedNeuralDynamics::updateFiringStats(const std::vector<SpikeEvent>& spikes,
                                               const std::vector<Neuron*>& neurons) {
    // Update firing statistics in optimized manner
    firingStats_.totalSpikes += spikes.size();
    firingStats_.lastStepSpikes = spikes.size();
    
    // Update neuron firing rates
    if (enableParallel_ && neurons.size() > SIMDUtils::SIMD_WIDTH) {
        // Parallel update of firing rates
        size_t chunkSize = neurons.size() / SIMDUtils::SIMD_WIDTH;
        std::vector<float> partialFiringRates(SIMDUtils::SIMD_WIDTH, 0.0f);
        
        #pragma omp parallel for
        for (size_t t = 0; t < SIMDUtils::SIMD_WIDTH; ++t) {
            size_t begin = t * chunkSize;
            size_t end = std::min(begin + chunkSize, neurons.size());
            
            for (size_t i = begin; i < end; ++i) {
                auto* neuron = neurons[i];
                if (neuron->isFiring()) {
                    partialFiringRates[t] += 1.0f;
                }
            }
        }
        
        for (float rate : partialFiringRates) {
            firingStats_.averageFiringRate += rate / neurons.size();
        }
    } else {
        // Scalar update
        for (auto* neuron : neurons) {
            if (neuron->isFiring()) {
                firingStats_.averageFiringRate += 1.0f / neurons.size();
            }
        }
    }
}

void OptimizedNeuralDynamics::getPerformanceMetrics(NeuralDynamicsMetrics& metrics) const {
    metrics.simulationFrequency = simulationFrequency_;
    metrics.enableSIMD = enableSIMD_;
    metrics.enableParallel = enableParallel_;
    metrics.enableMemPool = enableMemPool_;
    metrics.maxHistorySteps = maxHistorySteps_;
    metrics.averageFiringRate = firingStats_.averageFiringRate;
    metrics.totalSpikes = firingStats_.totalSpikes;
    metrics.lastStepSpikes = firingStats_.lastStepSpikes;
    metrics.neuralHistorySize = neuralHistory_.size();
    metrics.synapseHistorySize = synapseHistory_.size();
}

void OptimizedNeuralDynamics::enableFeature(OptimizationFeature feature, bool enabled) {
    switch (feature) {
        case OptimizationFeature::SIMD:
            enableSIMD_ = enabled;
            break;
        case OptimizationFeature::PARALLEL:
            enableParallel_ = enabled;
            break;
        case OptimizationFeature::MEMORY_POOL:
            enableMemPool_ = enabled;
            break;
        case OptimizationFeature::COMPRESSION:
            // Compression not yet implemented
            break;
        case OptimizationFeature::NUMA:
            // NUMA not yet implemented
            break;
        default:
            break;
    }
}

void OptimizedNeuralDynamics::setSimulationFrequency(float frequencyHz) {
    simulationFrequency_ = frequencyHz;
}

void OptimizedNeuralDynamics::getRecentHistory(std::deque<NeuronStepRecord>& neurons,
                                              std::deque<SynapseStepRecord>& synapses) {
    neurons = neuralHistory_;
    synapses = synapseHistory_;
}

void OptimizedNeuralDynamics::clearHistory() {
    neuralHistory_.clear();
    synapseHistory_.clear();
    firingStats_.reset();
}

} // namespace nlm
