#pragma once

#include <algorithm>
#include <execution>
#include <numeric>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <concurrent_vector>

namespace nlm {

// Performance optimization utilities for NLM
class PerformanceOptimizer {
public:
    // Performance measurement utilities
    struct PerformanceTimer {
        std::chrono::high_resolution_clock::time_point startTime;
        std::string operationName;
        
        PerformanceTimer(const std::string& name) : operationName(name) {
            startTime = std::chrono::high_resolution_clock::now();
        }
        
        ~PerformanceTimer() {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
            if (duration > 1000) { // Log operations longer than 1ms
                NLM_LOG_INFO("Performance: " + operationName + " took " + std::to_string(duration) + " μs");
            }
        }
    };
    
    // Optimized neural activation computation using SIMD-friendly patterns
    static void computeNeuralActivationsOptimized(const std::vector<Neuron*>& neurons, 
                                                  std::vector<float>& activations) {
        activations.clear();
        activations.reserve(neurons.size());
        
        // Use cache-friendly sequential access
        for (size_t i = 0; i < neurons.size(); ++i) {
            Neuron* neuron = neurons[i];
            if (neuron) {
                // Compute activation in optimized way
                const auto& state = neuron->getState();
                float activation = std::abs(state.membranePotential - state.restingPotential);
                activations.push_back(activation);
            }
        }
    }
    
    // Parallel spike processing using thread pools
    static void processSpikesParallel(const std::vector<std::unique_ptr<Neuron>>& neurons,
                                      std::vector<Neuron*>& outputSpikes) {
        const size_t numThreads = std::thread::hardware_concurrency();
        if (numThreads <= 1 || neurons.size() < 1000) {
            // Fall back to sequential for small datasets
            processSpikesSequential(neurons, outputSpikes);
            return;
        }
        
        outputSpikes.clear();
        outputSpikes.reserve(neurons.size());
        
        std::mutex outputMutex;
        std::atomic<size_t> processedCount(0);
        
        auto processChunk = [&](size_t start, size_t end) {
            std::vector<Neuron*> localSpikes;
            localSpikes.reserve(end - start);
            
            for (size_t i = start; i < end; ++i) {
                const auto& neuron = neurons[i];
                const auto& state = neuron->getState();
                bool isSpiking = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);
                
                if (isSpiking) {
                    localSpikes.push_back(neuron.get());
                }
            }
            
            // Thread-safe output update
            std::lock_guard<std::mutex> lock(outputMutex);
            outputSpikes.insert(outputSpikes.end(), localSpikes.begin(), localSpikes.end());
            
            processedCount += (end - start);
        };
        
        // Split work among threads
        size_t chunkSize = neurons.size() / numThreads;
        std::vector<std::thread> threads;
        size_t start = 0;
        
        for (size_t t = 0; t < numThreads; ++t) {
            size_t end = (t == numThreads - 1) ? neurons.size() : start + chunkSize;
            threads.emplace_back(processChunk, start, end);
            start = end;
        }
        
        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        NLM_LOG_INFO("Parallel spike processing: " + std::to_string(processedCount) + 
                    " spikes detected using " + std::to_string(numThreads) + " threads");
    }
    
    // Optimized synaptic connectivity initialization
    static void initializeSynapticConnectivityOptimized(std::vector<std::vector<Synapse*>>& adjacencyMatrix,
                                                        const std::vector<Neuron*>& neurons,
                                                        float connectionProbability,
                                                        std::mt19937& rng) {
        size_t numNeurons = neurons.size();
        adjacencyMatrix.resize(numNeurons);
        
        // Pre-allocate memory for typical connectivity patterns
        size_t expectedConnections = static_cast<size_t>(numNeurons * numNeurons * connectionProbability);
        adjacencyMatrix.reserve(expectedConnections);
        
        // Generate random connections with optimized random number generation
        for (size_t i = 0; i < numNeurons; ++i) {
            adjacencyMatrix[i].reserve(static_cast<size_t>(numNeurons * connectionProbability));
            
            for (size_t j = 0; j < numNeurons; ++j) {
                if (i == j) continue; // No self-connections
                
                std::uniform_real_distribution<float> dist(0.0f, 1.0f);
                if (dist(rng) < connectionProbability) {
                    // Create synapse with optimized weight initialization
                    std::uniform_real_distribution<float> weightDist(0.1f, 0.9f);
                    float weight = weightDist(rng);
                    
                    // Use local variable for efficiency
                    auto* synapse = new Synapse(i, j, weight);
                    adjacencyMatrix[i].push_back(synapse);
                }
            }
        }
        
        NLM_LOG_INFO("Optimized synaptic connectivity initialized: " + 
                    std::to_string(expectedConnections) + " expected connections");
    }
    
    // Vectorized neural state updates
    static void updateNeuralStatesVectorized(std::vector<NeuronState>& states, double timestep, double currentTime) {
        // Use SIMD-friendly operations by processing in chunks
        const size_t chunkSize = 16; // SIMD-friendly chunk size
        const size_t numStates = states.size();
        
        for (size_t i = 0; i < numStates; i += chunkSize) {
            size_t end = std::min(i + chunkSize, numStates);
            
            for (size_t j = i; j < end; ++j) {
                NeuronState& state = states[j];
                
                // Apply LIF dynamics in optimized way
                state.membranePotential += state.leakConductance * (-state.membranePotential + state.restingPotential) * timestep;
                
                // Check for spike threshold crossing
                if (state.membranePotential >= state.threshold) {
                    state.membranePotential = state.resetPotential;
                    state.firingState = FiringState::Refractory;
                    state.lastSpikeTime = static_cast<float>(currentTime);
                    state.refractoryRemaining = state.refractoryPeriod;
                }
                
                // Update refractory timer
                if (state.refractoryRemaining > 0.0f) {
                    state.refractoryRemaining -= static_cast<float>(timestep);
                    if (state.refractoryRemaining <= 0.0f) {
                        state.firingState = FiringState::Active;
                        state.refractoryRemaining = 0.0f;
                    }
                }
            }
        }
    }
    
    // Optimized plasticity rule application
    static void applyPlasticityOptimized(const std::vector<std::unique_ptr<Synapse>>& synapses,
                                         const std::vector<NeuronState>& preSpikeHistory,
                                         const std::vector<NeuronState>& postSpikeHistory,
                                         double timestep, float learningRate) {
        // Process synapses in optimized order for cache efficiency
        for (size_t i = 0; i < synapses.size(); ++i) {
            auto& synapse = synapses[i];
            
            // Pre-compute frequently used values
            const float weight = synapse->getWeight();
            const auto& plasticityFlags = synapse->getPlasticityFlags();
            
            // Apply STDP if enabled
            if (plasticityFlags.stdp) {
                // Compute weight change using optimized formula
                float weightChange = computeSTDPWeightChange(synapse, preSpikeHistory, postSpikeHistory, timestep, learningRate);
                synapse->addToWeight(weightChange);
            }
            
            // Apply Hebbian learning if enabled
            if (plasticityFlags.hebbian) {
                float weightChange = computeHebbianWeightChange(synapse, preSpikeHistory, postSpikeHistory, timestep, learningRate);
                synapse->addToWeight(weightChange);
            }
            
            // Update synapse state
            synapse->step(static_cast<float>(currentTime));
        }
    }
    
    // Memory pool for efficient object allocation/deallocation
    class NeuronMemoryPool {
    private:
        std::vector<std::unique_ptr<Neuron>> pool;
        std::vector<size_t> freeIndices;
        std::mutex poolMutex;
        
    public:
        NeuronMemoryPool(size_t initialSize) {
            pool.reserve(initialSize);
            freeIndices.reserve(initialSize);
            for (size_t i = 0; i < initialSize; ++i) {
                pool.emplace_back(std::make_unique<Neuron>(i));
                freeIndices.push_back(i);
            }
        }
        
        Neuron* allocate() {
            std::lock_guard<std::mutex> lock(poolMutex);
            if (freeIndices.empty()) {
                // Grow the pool if needed
                size_t currentSize = pool.size();
                pool.reserve(currentSize * 2);
                for (size_t i = currentSize; i < currentSize * 2; ++i) {
                    pool.emplace_back(std::make_unique<Neuron>(i));
                    freeIndices.push_back(i);
                }
            }
            
            size_t index = freeIndices.back();
            freeIndices.pop_back();
            return pool[index].get();
        }
        
        void deallocate(Neuron* neuron) {
            std::lock_guard<std::mutex> lock(poolMutex);
            // Find the neuron in the pool and mark it as free
            for (size_t i = 0; i < pool.size(); ++i) {
                if (pool[i].get() == neuron) {
                    freeIndices.push_back(i);
                    break;
                }
            }
        }
        
        ~NeuronMemoryPool() {
            // Clear all neurons
            pool.clear();
            freeIndices.clear();
        }
    };
    
    // Main simulation loop optimizations
    static void optimizedSimulationStep(Brain& brain, SimulationStep step, Timestamp time) {
        PerformanceTimer timer("Simulation step");
        
        // Get optimized neuron lists
        auto allNeurons = brain.getAllNeurons();
        auto sensoryNeurons = brain.getSensoryNeurons();
        auto motorNeurons = brain.getMotorNeurons();
        
        // Process spikes in parallel
        std::vector<Neuron*> spikes;
        processSpikesParallel(allNeurons, spikes);
        
        if (!spikes.empty()) {
            // Process synaptic connections efficiently
            for (Neuron* preNeuron : spikes) {
                // Get outgoing synapses using optimized adjacency structure
                auto outgoingSynapses = brain.getOutgoingSynapses(preNeuron->getId());
                for (Synapse* syn : outgoingSynapses) {
                    Neuron* postNeuron = brain.getNeuron(syn->getDestinationNeuron());
                    if (postNeuron) {
                        // Apply synaptic transmission
                        float synapticCurrent = syn->getWeight() * 10.0f;
                        if (syn->getType() == SynapseType::Excitatory) {
                            postNeuron->receiveExcitatoryInput(synapticCurrent);
                        } else {
                            postNeuron->receiveInhibitoryInput(synapticCurrent);
                        }
                    }
                }
            }
        }
        
        // Update all neuron states
        updateNeuralStatesVectorized(brain.getAllNeuronStates(), brain.getTimestep(), time);
        
        // Apply plasticity rules
        applyPlasticityOptimized(brain.getAllSynapses(), brain.getPreSpikeHistory(), 
                                brain.getPostSpikeHistory(), brain.getTimestep(), 0.01f);
        
        // Update memory systems
        if (brain.getWorkingMemory()) {
            brain.getWorkingMemory()->update(brain.getTimestep());
        }
    }
    
private:
    // Helper methods for optimized calculations
    static float computeSTDPWeightChange(Synapse* synapse, const std::vector<NeuronState>& preSpikeHistory,
                                        const std::vector<NeuronState>& postSpikeHistory, 
                                        double timestep, float learningRate) {
        // Optimized STDP weight change calculation
        float weightChange = 0.0f;
        
        // Find recent pre- and post-synaptic spikes
        float preTrace = 0.0f, postTrace = 0.0f;
        
        // Compute traces (simplified optimization)
        for (const auto& spike : postSpikeHistory) {
            postTrace = std::max(postTrace, spike.spikeTrace);
        }
        
        for (const auto& spike : preSpikeHistory) {
            preTrace = std::max(preTrace, spike.spikeTrace);
        }
        
        // Apply STDP rule
        if (preTrace > 0.0f && postTrace > 0.0f) {
            weightChange = learningRate * (postTrace - preTrace);
        }
        
        return weightChange;
    }
    
    static float computeHebbianWeightChange(Synapse* synapse, const std::vector<NeuronState>& preSpikeHistory,
                                           const std::vector<NeuronState>& postSpikeHistory,
                                           double timestep, float learningRate) {
        // Optimized Hebbian weight change calculation
        float weightChange = 0.0f;
        
        // Find correlated activity
        float preActivity = 0.0f, postActivity = 0.0f;
        
        for (const auto& spike : preSpikeHistory) {
            preActivity += spike.spikeTrace;
        }
        
        for (const auto& spike : postSpikeHistory) {
            postActivity += spike.spikeTrace;
        }
        
        // Apply Hebbian rule
        weightChange = learningRate * preActivity * postActivity;
        
        return weightChange;
    }
};

} // namespace nlm
