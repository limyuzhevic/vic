// NLM Synapse Batch Processing Optimizations
// Optimized version of Brain.cpp for high-performance synapse iteration

#pragma once

#include "Brain.hpp"
#include "../performance/SIMDVectorization.hpp"
#include "../performance/ParallelProcessing.hpp"
#include <algorithm>
#include <execution>
#include <numeric>

namespace nlm {

// Batch synapse update structure for efficient processing
struct BatchSynapseUpdate {
    std::vector<Synapse*> synapses;
    std::vector<const Neuron*> sourceNeurons;
    std::vector<const Neuron*> destinationNeurons;
    std::vector<float> weights;
    std::vector<float> eligibilityTraces;
    std::vector<float> efficacies;
    
    BatchSynapseUpdate(size_t capacity) {
        synapses.reserve(capacity);
        sourceNeurons.reserve(capacity);
        destinationNeurons.reserve(capacity);
        weights.reserve(capacity);
        eligibilityTraces.reserve(capacity);
        efficacies.reserve(capacity);
    }
};

// Vectorized plasticity calculation for batch processing
class VectorizedPlasticity {
public:
    static void applySTDPBatch(const BatchSynapseUpdate& batch,
                               const VectorizedSTDP& stdpVectorizer,
                               float plasticityMod,
                               float currentTime) {
        // Process STDP in parallel using SIMD
        size_t numSynapses = batch.synapses.size();
        if (numSynapses == 0) return;
        
        // For each synapse, calculate weight changes using vectorized operations
        for (size_t i = 0; i < numSynapses; i += VectorizedSTDP::SIMD_WIDTH) {
            size_t chunkSize = std::min(VectorizedSTDP::SIMD_WIDTH, numSynapses - i);
            
            // Process chunk of synapses
            processSynapseChunk(batch, i, chunkSize, stdpVectorizer, plasticityMod, currentTime);
        }
    }
    
private:
    static void processSynapseChunk(const BatchSynapseUpdate& batch,
                                    size_t startIdx,
                                    size_t chunkSize,
                                    const VectorizedSTDP& stdpVectorizer,
                                    float plasticityMod,
                                    float currentTime) {
        // Extract data for this chunk
        std::vector<float> preSpikeCounts(chunkSize, 0.0f);
        std::vector<float> postSpikeCounts(chunkSize, 0.0f);
        std::vector<float> weightDeltas(chunkSize, 0.0f);
        
        // Get synapse histories and calculate deltas
        for (size_t j = 0; j < chunkSize; ++j) {
            size_t idx = startIdx + j;
            Synapse* syn = batch.synapses[idx];
            
            // Get spike histories (this could be optimized further with SoA)
            const auto& preSpikes = syn->getPreSpikeHistory();
            const auto& postSpikes = syn->getPostSpikeHistory();
            
            // Count spikes in history
            preSpikeCounts[j] = static_cast<float>(preSpikes.size());
            postSpikeCounts[j] = static_cast<float>(postSpikes.size());
            
            // Calculate STDP delta using vectorized approach
            if (!preSpikes.empty() && !postSpikes.empty()) {
                // Use vectorized STDP calculation
                for (const auto& preTime : preSpikes) {
                    for (const auto& postTime : postSpikes) {
                        float dt = static_cast<float>(postTime - preTime);
                        if (dt > 0.0f) {
                            // LTP: pre before post
                            float delta = stdpVectorizer.calculateDeltaW(dt);
                            weightDeltas[j] += delta;
                        } else if (dt < 0.0f) {
                            // LTD: post before pre
                            float delta = stdpVectorizer.calculateDeltaW(dt);
                            weightDeltas[j] += delta;
                        }
                    }
                }
            }
            
            // Apply neuromodulation factor
            weightDeltas[j] *= plasticityMod;
            
            // Apply weight change with bounds checking
            float newWeight = batch.weights[j] + weightDeltas[j];
            newWeight = std::clamp(newWeight, -1.0f, 1.0f);
            
            // Update eligibility trace
            float currentTrace = batch.eligibilityTraces[j];
            batch.eligibilityTraces[j] = currentTrace + weightDeltas[j];
            
            // Apply weight change to synapse
            syn->setWeight(newWeight);
        }
    }
};

// Optimized neuron step function using SIMD
class OptimizedNeuronStepper {
public:
    static size_t stepLIFBatch(VectorizedLIF& lifVectorizer,
                               float* membranePotentials,
                               const float* synapticInputs,
                               uint32_t* refractoryCounters,
                               uint8_t* firingStates,
                               size_t count) {
        return lifVectorizer.update(membranePotentials, synapticInputs,
                                    refractoryCounters, firingStates, count);
    }
};

// Synapse batch processor for efficient spike event handling
class SynapseBatchProcessor {
public:
    static void processSpikePropagation(const std::vector<Synapse*>& synapses,
                                        const std::vector<Neuron*>& neurons,
                                        float currentTime,
                                        size_t maxBatchSize = 256) {
        // Group synapses by destination neuron for batch processing
        std::unordered_map<NeuronId, std::vector<Synapse*>> synapsesByDestination;
        
        for (Synapse* syn : synapses) {
            neurons[syn->getDestinationNeuron().index()]->addIncomingSynapse(syn);
        }
        
        // Process in batches
        size_t totalSynapses = synapses.size();
        for (size_t i = 0; i < totalSynapses; i += maxBatchSize) {
            size_t batchSize = std::min(maxBatchSize, totalSynapses - i);
            
            // Process this batch of synapses
            for (size_t j = 0; j < batchSize; ++j) {
                size_t idx = i + j;
                Synapse* syn = synapses[idx];
                
                // Deliver spike to destination neuron
                neurons[syn->getDestinationNeuron().index()]->receiveExcitatoryInput(
                    syn->getWeight() * 10.0f
                );
            }
        }
    }
};

// Memory layout optimization for synapse data
class SynapseDataLayout {
public:
    // Structure of Arrays (SoA) layout for synapse data
    struct SynapseSoA {
        std::vector<uint64_t> sourceIds;
        std::vector<uint64_t> destIds;
        std::vector<float> weights;
        std::vector<float> efficacies;
        std::vector<uint8_t> types;
        std::vector<uint8_t> plasticityFlags;
        std::vector<float> eligibilityTraces;
        std::vector<size_t> preSpikeCounts;
        std::vector<size_t> postSpikeCounts;
        
        void clear() {
            sourceIds.clear();
            destIds.clear();
            weights.clear();
            efficacies.clear();
            types.clear();
            plasticityFlags.clear();
            eligibilityTraces.clear();
            preSpikeCounts.clear();
            postSpikeCounts.clear();
        }
    };
    
    static SynapseSoA convertToSoA(const std::vector<std::unique_ptr<Synapse>>& synapses) {
        SynapseSoA soa;
        size_t capacity = synapses.size();
        
        soa.sourceIds.reserve(capacity);
        soa.destIds.reserve(capacity);
        soa.weights.reserve(capacity);
        soa.efficacies.reserve(capacity);
        soa.types.reserve(capacity);
        soa.plasticityFlags.reserve(capacity);
        soa.eligibilityTraces.reserve(capacity);
        soa.preSpikeCounts.reserve(capacity);
        soa.postSpikeCounts.reserve(capacity);
        
        for (const auto& syn : synapses) {
            soa.sourceIds.push_back(syn->getSourceNeuron().index());
            soa.destIds.push_back(syn->getDestinationNeuron().index());
            soa.weights.push_back(syn->getWeight());
            soa.efficacies.push_back(syn->getEfficacy());
            soa.types.push_back(static_cast<uint8_t>(syn->getType()));
            soa.plasticityFlags.push_back(
                (syn->getPlasticityFlags().hebbian ? 1 : 0) |
                (syn->getPlasticityFlags().stdp ? 2 : 0) |
                (syn->getPlasticityFlags().reward_modulated ? 4 : 0)
            );
            soa.eligibilityTraces.push_back(syn->getEligibilityTrace());
            soa.preSpikeCounts.push_back(syn->getPreSpikeHistory().size());
            soa.postSpikeCounts.push_back(syn->getPostSpikeHistory().size());
        }
        
        return soa;
    }
};

// Batch neuron activation processor
class NeuronBatchProcessor {
public:
    static void processNeurons(const std::vector<Neuron*>& neurons,
                               float currentTime,
                               float timestep,
                               VectorizedLIF& lifVectorizer,
                               ParallelNeuralProcessor& parallelProcessor) {
        // Convert neurons to arrays for SIMD processing
        size_t neuronCount = neurons.size();
        if (neuronCount == 0) return;
        
        // Extract neuron states into arrays
        std::vector<float> membranePotentials(neuronCount);
        std::vector<float> synapticInputs(neuronCount);
        std::vector<uint32_t> refractoryCounters(neuronCount);
        std::vector<uint8_t> firingStates(neuronCount);
        
        for (size_t i = 0; i < neuronCount; ++i) {
            const auto& state = neurons[i]->getState();
            membranePotentials[i] = state.membranePotential;
            synapticInputs[i] = neurons[i]->getTotalCurrent();
            refractoryCounters[i] = state.refractoryRemaining;
            firingStates[i] = static_cast<uint8_t>(state.firingState);
        }
        
        // Process using parallel processor for large populations
        if (neuronCount > 100) {
            parallelProcessor.processNeurons(neuronCount,
                                             [&](size_t i, ParallelNeuralProcessor::ThreadLocalState& state) {
                                                 // Process individual neuron
                                                 if (refractoryCounters[i] > 0) {
                                                     refractoryCounters[i]--;
                                                     membranePotentials[i] = -70.0f;
                                                     state.localFiringCount++;
                                                 } else {
                                                     // Apply LIF dynamics
                                                     float leak = (-70.0f - membranePotentials[i]) / 20.0f;
                                                     float input = synapticInputs[i] / 1.0f;
                                                     membranePotentials[i] += timestep * 1000.0f * (leak + input);
                                                     
                                                     if (membranePotentials[i] >= -55.0f) {
                                                         membranePotentials[i] = -70.0f;
                                                         refractoryCounters[i] = 5;
                                                         state.localSpikeCount++;
                                                     }
                                                 }
                                             });
        } else {
            // Process directly for small populations
            size_t fired = OptimizedNeuronStepper::stepLIFBatch(
                lifVectorizer,
                membranePotentials.data(),
                synapticInputs.data(),
                refractoryCounters.data(),
                firingStates.data(),
                neuronCount
            );
            
            // Update neuron states
            for (size_t i = 0; i < neuronCount; ++i) {
                neurons[i]->setMembranePotential(membranePotentials[i]);
                neurons[i]->setRefractoryRemaining(refractoryCounters[i]);
                
                if (firingStates[i]) {
                    neurons[i]->recordSpike(currentTime);
                }
            }
        }
    }
};

// Optimized spike event processor
class OptimizedSpikeProcessor {
public:
    static void processSpikeEvents(Brain& brain,
                                   const std::vector<SpikeEvent>& spikeEvents,
                                   const std::vector<DelayedSpikeEvent>& delayedEvents,
                                   VectorizedLIF& lifVectorizer,
                                   ParallelNeuralProcessor& parallelProcessor) {
        // Process immediate spikes
        for (const auto& event : spikeEvents) {
            // Find neuron and apply spike effect
            for (auto& region : brain.getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->getId() == event.neuronId) {
                            neuron->recordSpike(event.timestamp);
                            break;
                        }
                    }
                }
            }
        }
        
        // Process delayed spikes in batches
        if (!delayedEvents.empty()) {
            // Group by destination neuron
            std::unordered_map<NeuronId, std::vector<const DelayedSpikeEvent*>> eventsByDestination;
            for (const auto& event : delayedEvents) {
                eventsByDestination[event.destination_neuron].push_back(&event);
            }
            
            // Process each group
            for (const auto& [neuronId, events] : eventsByDestination) {
                processDelayedSpikeBatch(events, brain, lifVectorizer, parallelProcessor);
            }
        }
    }
    
private:
    static void processDelayedSpikeBatch(const std::vector<const DelayedSpikeEvent*>& events,
                                         Brain& brain,
                                         VectorizedLIF& lifVectorizer,
                                         ParallelNeuralProcessor& parallelProcessor) {
        // Find neuron and apply delayed spike effects
        for (auto& region : brain.getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->getId() == events.front()->destination_neuron) {
                        // Batch apply all delayed inputs
                        std::vector<float> inputs;
                        for (const auto* event : events) {
                            inputs.push_back(event->weight * 10.0f);
                        }
                        
                        // Apply inputs to neuron
                        for (float input : inputs) {
                            neuron->receiveExcitatoryInput(input);
                        }
                        break;
                    }
                }
            }
        }
    }
};

} // namespace nlm
