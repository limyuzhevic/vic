#include "OptimizedBrain.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

OptimizedBrain::OptimizedBrain(std::shared_ptr<Config> config)
    : originalBrain_(std::make_shared<Brain>(config)) {
    
    // Initialize optimized components
    initializeMemoryPool(*this, 100000);  // 100k neuron slots
    initializeEventQueue(*this, 50000);   // 50k event slots
    
    // Wrap original brain methods
    originalBrain_->setReceiveSensoryInputHandler([this](const SensoryInput& input) {
        onSensoryInput(input);
    });
    
    originalBrain_->setStepHandler([this](SimulationStep step, Timestamp time) {
        onStep(step, time);
    });
}

OptimizedBrain::~OptimizedBrain() = default;

// === OPTIMIZED NEURAL DYNAMICS ===

void OptimizedBrain::step(SimulationStep currentStep, Timestamp currentTime) {
    // Use event-driven execution for spike propagation
    processSpikeEvents(currentStep);
    
    // Batch process neurons using SIMD optimization
    batchProcessNeurons(currentStep, currentTime);
    
    // Process delayed spike events
    processDelayedSpikeEvents(currentStep, currentTime);
    
    // Apply plasticity using optimized batch operations
    batchProcessPlasticity(currentStep, currentTime);
    
    // Update neuromodulation
    updateNeuromodulation(currentStep, currentTime);
}

void OptimizedBrain::batchProcessNeurons(SimulationStep currentStep, Timestamp currentTime) {
    // Use SIMD vectorization for neuron updates
    for (auto& region : regions_) {
        // Process population groups in batches
        auto populations = region->getPopulations();
        for (auto& pop : populations) {
            // Use SIMD optimization for membrane potential updates
            processNeuronPopulation(pop, currentStep, currentTime);
        }
    }
}

void OptimizedBrain::processNeuronPopulation(NeuralPopulation* population, 
                                             SimulationStep currentStep, 
                                             Timestamp currentTime) {
    auto neurons = population->getNeurons();
    
    // Vectorized membrane potential update
    for (size_t i = 0; i + 4 <= neurons.size(); i += 4) {
        // Process 4 neurons at once using SIMD
        updateNeuronMembrane(neurons[i], neurons[i+1], neurons[i+2], neurons[i+3],
                           currentTime, pImpl->timestep);
    }
    
    // Process remaining neurons
    size_t remaining = neurons.size() % 4;
    for (size_t i = neurons.size() - remaining; i < neurons.size(); ++i) {
        neurons[i]->stepLIF(currentTime, pImpl->timestep);
    }
}

void OptimizedBrain::updateNeuronMembrane(Neuron* n1, Neuron* n2, Neuron* n3, Neuron* n4,
                                          Timestamp currentTime, TimestepDuration dt) {
    // TODO: Implement actual SIMD optimization
    // For now, just call individual updates
    n1->stepLIF(currentTime, dt);
    n2->stepLIF(currentTime, dt);
    n3->stepLIF(currentTime, dt);
    n4->stepLIF(currentTime, dt);
}

// === OPTIMIZED SPIKE HANDLING ===

void OptimizedBrain::processSpikeEvents(SimulationStep currentStep) {
    // Process all spikes scheduled for this step
    while (!spikeEvents_.empty() && spikeEvents_.front().step <= currentStep) {
        auto event = spikeEvents_.front();
        spikeEvents_.pop_front();
        
        // Find destination neuron and deliver synaptic input
        for (auto& region : regions_) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destinationNeuron) {
                    MembranePotential synapticCurrent = event.weight * 10.0f;
                    if (event.isExcitatory) {
                        neuron->receiveExcitatoryInput(synapticCurrent);
                    } else {
                        neuron->receiveInhibitoryInput(-synapticCurrent);
                    }
                    break;
                }
            }
        }
    }
}

void OptimizedBrain::processDelayedSpikeEvents(SimulationStep currentStep, Timestamp currentTime) {
    // Process delayed spike events
    while (!delayedSpikeEvents_.empty() && 
           delayedSpikeEvents_.front().deliveryStep <= currentStep) {
        
        auto event = delayedSpikeEvents_.front();
        delayedSpikeEvents_.pop_front();
        
        // Find destination neuron and deliver input
        for (auto& region : regions_) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destinationNeuron) {
                    MembranePotential synapticCurrent = event.weight * 10.0f;
                    if (event.isExcitatory) {
                        neuron->receiveExcitatoryInput(synapticCurrent);
                    } else {
                        neuron->receiveInhibitoryInput(-synapticCurrent);
                    }
                    break;
                }
            }
        }
    }
}

// === OPTIMIZED PLASTICITY ===

void OptimizedBrain::batchProcessPlasticity(SimulationStep currentStep, Timestamp currentTime) {
    // Process synaptic plasticity in batches
    for (auto& region : regions_) {
        auto synapses = region->getSynapses();
        
        // Process STDP in optimized batches
        processSTDPBatch(synapses, currentStep, currentTime);
        
        // Process Hebbian learning
        processHebbianBatch(synapses, currentStep, currentTime);
        
        // Process structural plasticity
        if (currentStep % 100 == 0) {
            processStructuralPlasticity(region.get(), currentStep);
        }
    }
}

void OptimizedBrain::processSTDPBatch(const std::vector<Synapse*>& synapses,
                                      SimulationStep currentStep,
                                      Timestamp currentTime) {
    // Collect spike histories for efficient STDP computation
    for (auto* syn : synapses) {
        if (syn->getPlasticityFlags().stdp) {
            const auto& preSpikes = syn->getPreSpikeHistory();
            const auto& postSpikes = syn->getPostSpikeHistory();
            
            if (!preSpikes.empty() && !postSpikes.empty()) {
                // Apply STDP
                float weight = syn->getWeight();
                weight += computeSTDPWeightChange(syn, preSpikes, postSpikes);
                syn->setWeight(weight);
            }
        }
    }
}

void OptimizedBrain::processHebbianBatch(const std::vector<Synapse*>& synapses,
                                         SimulationStep currentStep,
                                         Timestamp currentTime) {
    for (auto* syn : synapses) {
        if (syn->getPlasticityFlags().hebbian) {
            const auto& preSpikes = syn->getPreSpikeHistory();
            const auto& postSpikes = syn->getPostSpikeHistory();
            
            if (!preSpikes.empty() && !postSpikes.empty()) {
                // Apply Hebbian learning
                float weight = syn->getWeight();
                weight += computeHebbianWeightChange(syn, preSpikes, postSpikes);
                syn->setWeight(weight);
            }
        }
    }
}

// === OPTIMIZED NEUROMODULATION ===

void OptimizedBrain::updateNeuromodulation(SimulationStep currentStep, Timestamp currentTime) {
    // Update all neuromodulators efficiently
    if (dopamine_) {
        dopamine_->update(pImpl->timestep);
        applyDopamineEffects();
    }
    
    if (curiosity_) {
        curiosity_->update(pImpl->timestep);
    }
    
    if (novelty_) {
        novelty_->update(pImpl->timestep);
    }
    
    if (predictionError_) {
        predictionError_->update(pImpl->timestep);
    }
    
    // Apply ACh effects on attention and memory
    if (acetylcholine_) {
        acetylcholine_->update(pImpl->timestep);
        applyAChEffects();
    }
    
    // Apply NE effects on arousal
    if (norepinephrine_) {
        norepinephrine_->update(pImpl->timestep);
        applyNEEffects();
    }
    
    // Apply 5-HT effects on mood
    if (serotonin_) {
        serotonin_->update(pImpl->timestep);
        applySerotoninEffects();
    }
}

void OptimizedBrain::applyDopamineEffects() {
    // Batch apply dopamine effects to all neurons
    float dopamineLevel = dopamine_->getLevel();
    float excitabilityMod = dopamineLevel * 0.5f;
    
    if (excitabilityMod > 0.0f) {
        for (auto& region : regions_) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

void OptimizedBrain::applyAChEffects() {
    // ACh enhances attention and memory consolidation
    float achLevel = acetylcholine_->getLevel();
    
    if (achLevel > 0.1f) {
        // Boost memory trace strength
        for (auto& trace : memoryTraces_) {
            if (trace.activation > 0.1f) {
                trace.activation = std::min(1.0f, trace.activation + achLevel * 0.1f);
            }
        }
    }
}

void OptimizedBrain::applyNEEffects() {
    // NE enhances arousal and vigilance
    float neLevel = norepinephrine_->getLevel();
    
    if (neLevel > 0.3f) {
        // Increase neural excitability
        for (auto& region : regions_) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(neLevel * 0.3f);
                }
            }
        }
    }
}

void OptimizedBrain::applySerotoninEffects() {
    // 5-HT modulates learning rate
    float serotoninLevel = serotonin_->getLevel();
    
    if (serotoninLevel > 0.3f) {
        // Reduce plasticity (stabilize memories)
        for (auto& region : regions_) {
            for (auto& syn : region->getSynapses()) {
                // Reduce STDP influence
                auto flags = syn->getPlasticityFlags();
                flags.stdp = false;  // Temporarily inhibit STDP
                syn->setPlasticityFlags(flags);
            }
        }
    }
}

// === BRAIN INTEGRATION ===

void OptimizedBrain::initializeMemoryPool(Brain& brain, size_t capacity) {
    // Create memory pool for neuron states
    memoryPool_ = std::make_unique<MemoryPool<Neuron>>(capacity);
    
    // Initialize with brain's neurons
    for (auto& region : brain.getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                memoryPool_->allocate( neuron);  // Store neuron reference
            }
        }
    }
}

void OptimizedBrain::initializeEventQueue(Brain& brain, size_t capacity) {
    // Create event queue for spike events
    eventQueue_ = std::make_unique<EventQueue<SpikeEvent>>(capacity);
}

void OptimizedBrain::onSensoryInput(const SensoryInput& input) {
    // Handle sensory input through optimized pathway
    for (auto& region : regions_) {
        // Inject input into sensory neurons
        region->injectInputToSensoryNeurons(input);
    }
}

void OptimizedBrain::onStep(SimulationStep currentStep, Timestamp currentTime) {
    // Record performance metrics
    if (performanceMonitor_) {
        performanceMonitor_->recordStep(currentStep, 
                                        originalBrain_->getTotalNeuronCount(),
                                        originalBrain_->getTotalSynapseCount());
    }
    
    // Process memory traces
    updateMemoryTraces(currentStep, currentTime);
}

void OptimizedBrain::updateMemoryTraces(SimulationStep currentStep, Timestamp currentTime) {
    // Update working memory traces
    for (auto& trace : memoryTraces_) {
        if (trace.age < trace.maxAge) {
            trace.age++;
            trace.activation *= trace.decayRate;
        }
    }
}

// === UTILITY ===

float OptimizedBrain::computeSTDPWeightChange(Synapse* syn,
                                              const std::vector<float>& preSpikes,
                                              const std::vector<float>& postSpikes) {
    // Compute STDP weight change
    float weightChange = 0.0f;
    
    // LTP: pre before post
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            if (preSpikes[i] < postSpikes[j]) {
                weightChange += 0.01f;  // LTP
            } else {
                weightChange -= 0.012f; // LTD
            }
        }
    }
    
    return weightChange;
}

float OptimizedBrain::computeHebbianWeightChange(Synapse* syn,
                                                 const std::vector<float>& preSpikes,
                                                 const std::vector<float>& postSpikes) {
    // Simple Hebbian learning: co-activation strengthens synapses
    if (!preSpikes.empty() && !postSpikes.empty()) {
        return 0.001f * preSpikes.size() * postSpikes.size();
    }
    return 0.0f;
}

} // namespace nlm
