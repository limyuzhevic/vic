#include "OptimizedBrain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../core/Logger/Logger.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/Memory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../agent/AgentBrain.hpp"
#include <chrono>
#include <algorithm>
#include <unordered_set>
#include <iostream>

namespace nlm {

// OptimizedBrain implementation - constructor

OptimizedBrain::OptimizedBrain(std::shared_ptr<Config> config)
    : Brain(config)
    , eventDrivenEnabled_(true)
    , multithreadingEnabled_(true)
    , simdEnabled_(true)
    , sparseEnabled_(true)
    , threadCount_(std::thread::hardware_concurrency())
    , activeNeuronCount_(0)
    , totalSteps_(0) {
}

OptimizedBrain::~OptimizedBrain() {
    // All components are managed by std::unique_ptr
}

// OptimizedBrain implementation - initialize()

bool OptimizedBrain::initialize() {
    // Initialize the base brain components
    if (!Brain::initialize()) {
        return false;
    }
    
    // Set up performance optimizations based on configuration
    eventDrivenEnabled_ = config_->getBool("performance.eventDriven", true);
    multithreadingEnabled_ = config_->getBool("performance.multithreading", true);
    simdEnabled_ = config_->getBool("performance.simd", true);
    sparseEnabled_ = config_->getBool("performance.sparseConnectivity", true);
    
    threadCount_ = config_->getUInt("performance.threadCount", std::thread::hardware_concurrency());
    
    // Initialize neural state
    size_t totalNeurons = getTotalNeuronCount();
    if (totalNeurons > 0) {
        neuralState_.initialize(totalNeurons);
    }
    
    // Initialize synapse state
    size_t totalSynapses = getTotalSynapseCount();
    if (totalSynapses > 0) {
        synapseState_.initialize(totalSynapses);
    }
    
    // Initialize connectivity
    initializeSparseConnectivity();
    
    // Create parallel processor if multithreading is enabled
    if (multithreadingEnabled_) {
        parallelProcessor_ = std::make_unique<ParallelNeuralProcessor>(threadCount_);
    }
    
    // Initialize performance statistics
    perfStats_.neuronsPerSecond = 0.0;
    perfStats_.spikesPerSecond = 0.0;
    perfStats_.synapticEventsPerSecond = 0.0;
    perfStats_.realTimeFactor = 1.0;
    perfStats_.activeNeurons = 0;
    perfStats_.firingNeurons = 0;
    perfStats_.pendingSpikes = 0;
    perfStats_.pendingDelayedSpikes = 0;
    perfStats_.avgFiringRate = 0.0f;
    perfStats_.memoryUsageMB = 0.0f;
    perfStats_.stepTimeMs = 0.0;
    
    lastStepTime_ = std::chrono::high_resolution_clock::now();
    
    return true;
}

// OptimizedBrain implementation - step() - optimized version

void OptimizedBrain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<double>(currentStep));
}

void OptimizedBrain::step(SimulationStep currentStep, Timestamp currentTime) {
    // Lock for thread safety
    std::unique_lock<std::shared_mutex> lock(stepMutex_);
    
    // Record start time for performance metrics
    auto stepStart = std::chrono::high_resolution_clock::now();
    
    // Update statistics before step
    updateStatistics();
    
    // Update connections
    if (sparseEnabled_) {
        // Use sparse connectivity for efficient lookup
        for (auto& [regionId, region] : regions_) {
            region->updateConnections();
        }
    }
    
    // Event-driven processing - only process active neurons
    if (eventDrivenEnabled_) {
        processActiveNeurons(currentStep, currentTime);
        processFiringNeurons(currentStep, currentTime);
    } else {
        // Batch processing for all neurons
        processAllNeurons(currentStep, currentTime);
    }
    
    // Apply plasticity
    applyPlasticity();
    
    // Update synaptic connections
    if (sparseEnabled_) {
        updateAllSynapses(currentTime);
    }
    
    // Advance event queues
    if (eventDrivenEnabled_) {
        advanceEventQueues(currentStep);
    }
    
    // Record step time for performance metrics
    auto stepEnd = std::chrono::high_resolution_clock::now();
    perfStats_.stepTimeMs = std::chrono::duration<double, std::milli>(stepEnd - stepStart).count();
    
    // Update step counter
    totalSteps_++;
    
    // Update statistics
    updateStatistics();
}

// OptimizedBrain implementation - processActiveNeurons()

void OptimizedBrain::processActiveNeurons(uint64_t currentStep, double currentTime) {
    // Get list of active neurons
    const auto& activeIndices = neuralState_.activeNeuronIndices;
    
    if (activeIndices.empty()) {
        return;
    }
    
    // Process active neurons using parallel processing if enabled
    if (multithreadingEnabled_ && parallelProcessor_) {
        parallelProcessor_->processNeurons(activeIndices.size(),
            [this, currentStep, currentTime](size_t idx, ThreadLocalState& state) {
                updateNeuronsBatch(idx, idx + 1, currentStep, currentTime);
                // Update thread-local state
                if (neuralState_.firingNeuronIndices[idx]) {
                    state.localFiringCount++;
                }
            });
    } else {
        // Sequential processing
        for (size_t idx : activeIndices) {
            updateNeuronsBatch(idx, idx + 1, currentStep, currentTime);
            if (neuralState_.firingNeuronIndices[idx]) {
                // Track firing neurons
            }
        }
    }
    
    // Clear active neuron indices
    neuralState_.activeNeuronIndices.clear();
    neuralState_.firingNeuronIndices.clear();
}

// OptimizedBrain implementation - processFiringNeurons()

void OptimizedBrain::processFiringNeurons(uint64_t currentStep, double currentTime) {
    // Get list of neurons that fired this step
    const auto& firingIndices = neuralState_.firingNeuronIndices;
    
    if (firingIndices.empty()) {
        return;
    }
    
    // Add spikes to event queue
    for (size_t idx : firingIndices) {
        // Get neuron info
        auto neuron = neuralState_.getNeuron(idx);
        if (!neuron) continue;
        
        // Create spike event
        SpikeEvent event(neuron->getId(), currentTime, currentStep);
        
        // Add to spike queue
        if (eventDrivenEnabled_) {
            spikeQueue_.push(event);
        }
        
        // Record firing for statistics
        perfStats_.firingNeurons++;
    }
    
    // Process queued spikes
    processQueuedSpikes(currentStep, currentTime);
}

// OptimizedBrain implementation - processAllNeurons()

void OptimizedBrain::processAllNeurons(uint64_t currentStep, double currentTime) {
    // Process all neurons in batch
    size_t totalNeurons = neuralState_.getNeuronCount();
    
    if (multithreadingEnabled_ && parallelProcessor_ && totalNeurons > 1000) {
        parallelProcessor_->processNeurons(totalNeurons,
            [this, currentStep, currentTime](size_t idx, ThreadLocalState& state) {
                updateNeuronsBatch(idx, idx + 1, currentStep, currentTime);
                if (neuralState_.firingNeuronIndices[idx]) {
                    state.localFiringCount++;
                }
            });
    } else {
        // Sequential batch processing
        updateNeuronsBatch(0, totalNeurons, currentStep, currentTime);
    }
}

// OptimizedBrain implementation - updateNeuronsBatch()

void OptimizedBrain::updateNeuronsBatch(size_t begin, size_t end, uint64_t currentStep, double currentTime) {
    // Determine which processing method to use based on configuration
    size_t numNeurons = end - begin;
    
    if (simdEnabled_ && numNeurons >= SIMDUtils::SIMD_WIDTH) {
        // Use SIMD-optimized version
        if constexpr (SIMDUtils::isSIMDAvailable()) {
            lifVectorizer_.update(neuralState_.membranePotential,
                                 neuralState_.synapseInput,
                                 neuralState_.refractoryRemaining,
                                 neuralState_.firingNeuronIndices,
                                 numNeurons);
        }
    }
    
    // Update each neuron
    for (size_t i = begin; i < end; ++i) {
        auto neuron = neuralState_.getNeuron(i);
        if (!neuron) continue;
        
        // Update neuron for current timestep
        bool fired = neuron->step(currentTime, 1.0);  // TODO: Get actual timestep from config
        
        if (fired) {
            neuralState_.firingNeuronIndices.push_back(i);
            perfStats_.firingNeurons++;
        }
        
        // Update neural state from neuron
        neuralState_.membranePotential[i] = neuron->getMembranePotential();
        neuralState_.refractoryRemaining[i] = neuron->getRefractoryRemaining();
        
        // Check if neuron is active (has input)
        if (neuralState_.synapseInput[i] != 0.0f) {
            neuralState_.activeNeuronIndices.push_back(i);
        }
    }
}

// OptimizedBrain implementation - updateAllSynapses()

void OptimizedBrain::updateSynapsesBatch(size_t begin, size_t end, double currentTime) {
    size_t numSynapses = end - begin;
    
    // Use SIMD for synaptic input updates if available
    if (simdEnabled_ && numSynapses >= SIMDUtils::SIMD_WIDTH) {
        for (size_t i = begin; i < end; ++i) {
            auto synapse = synapseState_.getSynapse(i);
            if (!synapse) continue;
            
            // Update synapse efficacy based on plasticity
            float deltaW = calculateSTDPUpdate(i, currentTime);
            synapse->setEfficacy(synapse->getEfficacy() + deltaW);
            
            // Apply short-term plasticity
            updateShortTermPlasticity(i, currentTime);
        }
    } else {
        // Sequential update
        for (size_t i = begin; i < end; ++i) {
            auto synapse = synapseState_.getSynapse(i);
            if (!synapse) continue;
            
            // Update synapse state
            synapse->step(currentTime);
        }
    }
}

// OptimizedBrain implementation - applyPlasticity()

void OptimizedBrain::applyPlasticity() {
    // Apply different plasticity rules based on configuration
    if (config_->getBool("plasticity.stdp", true)) {
        applySTDP();
    }
    
    if (config_->getBool("plasticity.hebbian", true)) {
        applyHebbian();
    }
    
    if (config_->getBool("plasticity.structural", true)) {
        applyStructuralPlasticity();
    }
}

// OptimizedBrain implementation - updateStatistics()

void OptimizedBrain::updateStatistics() {
    // Update performance statistics
    perfStats_.activeNeurons = neuralState_.activeNeuronIndices.size();
    perfStats_.firingNeurons = neuralState_.firingNeuronIndices.size();
    perfStats_.pendingSpikes = spikeQueue_.size();
    perfStats_.pendingDelayedSpikes = delayedSpikeQueue_.size();
    
    // Calculate firing rate
    if (totalSteps_ > 0) {
        perfStats_.avgFiringRate = static_cast<float>(perfStats_.firingNeurons) / 
                                 static_cast<float>(neuralState_.getNeuronCount());
    }
    
    // Calculate memory usage
    size_t totalMemory = neuralState_.memoryUsage() + synapseState_.memoryUsage();
    perfStats_.memoryUsageMB = totalMemory / (1024 * 1024);
    
    // Calculate real-time factor (simulated seconds per real second)
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(currentTime - lastStepTime_).count();
    perfStats_.realTimeFactor = 1.0 / elapsed;  // TODO: Calculate properly
}

// OptimizedBrain implementation - getPerformanceStats()

OptimizedBrain::PerformanceStats OptimizedBrain::getPerformanceStats() const {
    return perfStats_;
}

// OptimizedBrain implementation - private helper methods

void OptimizedBrain::initializeSparseConnectivity() {
    // Create sparse connectivity structure
    connectivity_.reserveNeurons(getTotalNeuronCount());
    
    // Add all existing synapses
    for (size_t i = 0; i < getTotalSynapseCount(); ++i) {
        auto synapse = synapseState_.getSynapse(i);
        if (synapse) {
            connectivity_.addSynapse(
                synapse->getSourceNeuron(),
                synapse->getDestinationNeuron(),
                synapse->getWeight(),
                synapse->getDelay(),
                static_cast<uint8_t>(synapse->getType())
            );
        }
    }
}

void OptimizedBrain::advanceEventQueues(uint64_t currentStep) {
    // Advance delayed spike queue
    std::vector<DelayedSpikeEvent> dueSpikes = delayedSpikeQueue_.getDueSpikes(currentStep);
    
    for (const auto& spike : dueSpikes) {
        // Deliver spike to destination neuron
        auto destNeuron = neuralState_.getNeuron(spike.destinationNeuron.index());
        if (destNeuron) {
            destNeuron->receiveExcitatoryInput(spike.weight);
        }
    }
}

void OptimizedBrain::processQueuedSpikes(uint64_t currentStep, double currentTime) {
    // Process spikes from spike queue
    std::vector<SpikeEvent> queuedSpikes = spikeQueue_.getDueSpikes(currentStep);
    
    for (const auto& spike : queuedSpikes) {
        // Get source and destination neurons
        auto sourceNeuron = neuralState_.getNeuron(spike.source_neuron.index());
        auto destNeuron = neuralState_.getNeuron(spike.destination_neuron.index());
        
        if (!sourceNeuron || !destNeuron) continue;
        
        // Get spike information from spike system
        auto spikeSystem = getSpikeSystem();
        if (spikeSystem) {
            // Create delayed spike event
            DelayedSpikeEvent delayedSpike(
                spike.source_neuron,
                spike.destination_neuron,
                spike.synapse_id,
                spike.weight,
                spike.synapse_type,
                spike.timestamp,
                spike.timestamp + spike.delay,
                spike.step,
                spike.step + spike.delay,
                spike.is_excitatory
            );
            
            // Add to delayed spike queue
            delayedSpikeQueue_.schedule(
                spike.source_neuron,
                spike.destination_neuron,
                spike.synapse_id,
                spike.weight,
                spike.is_excitatory,
                spike.delivery_step
            );
        }
    }
}

// VectorizedLIF implementation for OptimizedBrain (inline from SIMDVectorization.hpp)

void OptimizedBrain::VectorizedLIF::updateBatched(
    std::vector<float>& V,
    const std::vector<float>& I,
    std::vector<uint32_t>& refractory,
    std::vector<uint32_t>& fired,
    size_t count,
    float V_rest,
    float V_reset,
    float V_threshold,
    float tau,
    float C,
    float dt,
    float refractoryDecay) {
    
    // Simplified batched update
    for (size_t i = 0; i < count; ++i) {
        if (refractory[i] > 0) {
            refractory[i]--;
            V[i] = V_rest;
            fired[i] = 0;
        } else {
            float leak = (V_rest - V[i]) / tau;
            float input = I[i] / C;
            V[i] += dt * 1000.0f * (leak + input);
            
            if (V[i] >= V_threshold) {
                V[i] = V_reset;
                refractory[i] = static_cast<uint32_t>(refractoryDecay);
                fired[i] = 1;
            } else {
                fired[i] = 0;
            }
        }
    }
}

} // namespace nlm

// Benchmarking and logging

inline void benchmarkOptimizedBrain() {
    std::cout << "OptimizedBrain benchmark not yet implemented" << std::endl;
}

inline void logOptimizedBrainStats(const OptimizedBrain& brain) {
    auto stats = brain.getPerformanceStats();
    std::cout << "OptimizedBrain Performance Stats:" << std::endl;
    std::cout << "  Step time: " << std::fixed << std::setprecision(3) 
              << stats.stepTimeMs << " ms" << std::endl;
    std::cout << "  Neurons per second: " << std::fixed << std::setprecision(1)
              << stats.neuronsPerSecond << std::endl;
    std::cout << "  Active neurons: " << stats.activeNeurons << std::endl;
    std::cout << "  Firing neurons: " << stats.firingNeurons << std::endl;
    std::cout << "  Pending spikes: " << stats.pendingSpikes << std::endl;
    std::cout << "  Pending delayed spikes: " << stats.pendingDelayedSpikes << std::endl;
    std::cout << "  Average firing rate: " << std::fixed << std::setprecision(3)
              << stats.avgFiringRate << " Hz" << std::endl;
    std::cout << "  Memory usage: " << std::fixed << std::setprecision(2)
              << stats.memoryUsageMB << " MB" << std::endl;
}

} // namespace nlm