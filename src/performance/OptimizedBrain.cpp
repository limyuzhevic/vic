#include "OptimizedBrain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <unordered_map>
#include <set>

namespace nlm {

OptimizedBrain::OptimizedBrain(std::shared_ptr<Config> config)
    : Brain(config)
    , eventDrivenEnabled_(true)
    , multithreadingEnabled_(true)
    , simdEnabled_(true)
    , sparseEnabled_(true)
    , threadCount_(std::thread::hardware_concurrency())
    , parallelProcessor_(nullptr)
    , totalSteps_(0) {
    
    if (multithreadingEnabled_) {
        parallelProcessor_ = std::make_unique<ParallelNeuralProcessor>(threadCount_);
    }
    
    // Create optimized neural state
    size_t totalNeurons = 1000; // Default from config
    neuralState_.initialize(totalNeurons);
    
    // Create optimized synapse state
    size_t totalSynapses = 100000; // Approximate from connection probability
    synapseState_.initialize(totalSynapses);
}

OptimizedBrain::~OptimizedBrain() = default;

bool OptimizedBrain::initialize() {
    NLM_LOG_INFO("Initializing Optimized NLM Brain...");
    
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Initialize neural state arrays
    neuralState_.initialize(neuronCount);
    synapseState_.initialize(neuronCount * 10); // Approximate synapses
    
    // Create regions using optimized approach
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Add populations with optimized neuron creation
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
        }
    }
    
    // Initialize connectivity with optimized sparse matrix
    initializeSparseConnectivity();
    
    // Initialize optimized neural state from brain neurons
    initializeNeuralStateFromBrain();
    
    // Initialize synapse state from brain synapses
    initializeSynapseStateFromBrain();
    
    NLM_LOG_INFO("Optimized NLM Brain initialization complete");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    
    return true;
}

void OptimizedBrain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void OptimizedBrain::step(SimulationStep currentStep, Timestamp currentTime) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    
    // Optimized step processing
    if (eventDrivenEnabled_) {
        processActiveNeurons(currentStep, currentTime);
    } else {
        processAllNeurons(currentStep, currentTime);
    }
    
    // Apply plasticity using optimized methods
    applyPlasticity();
    
    // Update statistics
    updateStatistics();
    
    // Record step time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    perfStats_.stepTimeMs = duration.count() / 1000.0f;
    lastStepTime_ = endTime;
    totalSteps_++;
}

void OptimizedBrain::processActiveNeurons(uint64_t currentStep, double currentTime) {
    // Process only active neurons using optimized SoA access
    neuralState_.clear();
    
    // Find active neurons (those with synaptic input)
    for (size_t i = 0; i < neuralState_.membranePotential.size(); ++i) {
        if (neuralState_.synapseInput[i] != 0.0f) {
            neuralState_.activeNeuronIndices.push_back(static_cast<uint32_t>(i));
        }
    }
    
    if (neuralState_.activeNeuronIndices.empty()) {
        return;
    }
    
    // Process active neurons in batches
    size_t batchSize = 1024;
    for (size_t i = 0; i < neuralState_.activeNeuronIndices.size(); i += batchSize) {
        size_t end = std::min(i + batchSize, neuralState_.activeNeuronIndices.size());
        if (multithreadingEnabled_) {
            // Parallel processing
            processNeuronBatchParallel(
                &neuralState_.activeNeuronIndices[i],
                end - i,
                currentStep,
                currentTime
            );
        } else {
            // Sequential processing
            for (size_t j = i; j < end; ++j) {
                size_t neuronIdx = neuralState_.activeNeuronIndices[j];
                processSingleNeuron(neuronIdx, currentStep, currentTime);
            }
        }
    }
    
    // Update firing neurons
    for (size_t i = 0; i < neuralState_.firingNeuronIndices.size(); ++i) {
        size_t neuronIdx = neuralState_.firingNeuronIndices[i];
        // Generate spike event
        recordSpikeEvent(neuronIdx, currentStep, currentTime);
    }
}

void OptimizedBrain::processFiringNeurons(uint64_t currentStep, double currentTime) {
    // Process neurons that fired using optimized methods
    for (size_t i = 0; i < neuralState_.firingNeuronIndices.size(); ++i) {
        size_t neuronIdx = neuralState_.firingNeuronIndices[i];
        recordSpikeEvent(neuronIdx, currentStep, currentTime);
    }
}

void OptimizedBrain::updateNeuronsBatch(size_t begin, size_t end, uint64_t currentStep, double currentTime) {
    // Optimized batch processing using SIMD when available
    if (simdEnabled_ && SIMDUtils::isSIMDAvailable()) {
        // Vectorized LIF update
        size_t firedCount = lifVectorizer_.update(
            neuralState_.membranePotential,
            neuralState_.synapseInput,
            neuralState_.refractoryRemaining,
            neuralState_.firingState,
            end - begin
        );
        
        // Process firing neurons
        neuralState_.firingNeuronIndices.reserve(neuralState_.firingNeuronIndices.size() + firedCount);
        for (size_t i = begin; i < end; ++i) {
            if (neuralState_.firingState[i]) {
                neuralState_.firingNeuronIndices.push_back(static_cast<uint32_t>(i));
            }
        }
    } else {
        // Scalar batch processing
        for (size_t i = begin; i < end; ++i) {
            processSingleNeuron(i, currentStep, currentTime);
        }
    }
}

void OptimizedBrain::updateSynapsesBatch(size_t begin, size_t end, double currentTime) {
    // Optimized synapse processing
    for (size_t i = begin; i < end; ++i) {
        // Update short-term plasticity using vectorized operations
        if (simdEnabled_ && SIMDUtils::isSIMDAvailable()) {
            // Vectorized synapse update
            float* depression = &synapseState_.shortTermDepression[i];
            float* facilitation = &synapseState_.shortTermFacilitation[i];
            const float* lastPre = &synapseState_.lastPreSpikeTime[i];
            const float* lastPost = &synapseState_.lastPostSpikeTime[i];
            
            // Batch update
            VectorizedSynapse syn;
            syn.updateSTD(depression, facilitation, lastPre, lastPost, currentTime, 1);
        } else {
            // Scalar update
            float& depression = synapseState_.shortTermDepression[i];
            float& facilitation = synapseState_.shortTermFacilitation[i];
            float lastPre = synapseState_.lastPreSpikeTime[i];
            float lastPost = synapseState_.lastPostSpikeTime[i];
            
            float dtPre = std::max(0.0f, currentTime - lastPre);
            float dtPost = std::max(0.0f, currentTime - lastPost);
            
            // Update facilitation
            facilitation *= std::exp(-dtPre / VectorizedSynapse().tau_facilitation);
            
            // Update depression
            depression += (1.0f - depression) * (1.0f - std::exp(-dtPost / VectorizedSynapse().tau_depression));
            
            // Clamp
            depression = std::clamp(depression, 0.0f, VectorizedSynapse().U_max);
        }
    }
}

void OptimizedBrain::applyPlasticity() {
    // Optimized plasticity application using parallel processing
    if (multithreadingEnabled_) {
        applyPlasticityParallel();
    } else {
        applyPlasticitySequential();
    }
}

void OptimizedBrain::updateStatistics() {
    // Update performance statistics using optimized computations
    size_t activeNeurons = 0;
    size_t firingNeurons = 0;
    
    for (size_t i = 0; i < neuralState_.membranePotential.size(); ++i) {
        if (neuralState_.synapseInput[i] != 0.0f) {
            ++activeNeurons;
        }
        if (neuralState_.firingState[i]) {
            ++firingNeurons;
        }
    }
    
    perfStats_.activeNeurons = activeNeurons;
    perfStats_.firingNeurons = firingNeurons;
    perfStats_.pendingSpikes = neuralState_.firingNeuronIndices.size();
    perfStats_.pendingDelayedSpikes = 0; // TODO: implement delayed spike queue
    perfStats_.avgFiringRate = calculateAverageFiringRate();
}

void OptimizedBrain::initializeSparseConnectivity() {
    // Initialize sparse connectivity matrix for efficient synapse access
    connectivity_.resize(neuralState_.membranePotential.size());
    
    // Create random connectivity (placeholder for actual implementation)
    for (size_t i = 0; i < neuralState_.membranePotential.size(); ++i) {
        // For now, create connections for internal neurons
        if (neuralState_.neuronType[i] == static_cast<uint8_t>(NeuronType::Internal)) {
            // Connect to random destination neurons
            size_t numConnections = 10; // Average connections per neuron
            std::set<size_t> destinations;
            while (destinations.size() < numConnections) {
                size_t dest = rand() % neuralState_.membranePotential.size();
                if (dest != i) {
                    destinations.insert(dest);
                }
            }
            
            for (size_t dest : destinations) {
                connectivity_.addConnection(i, dest);
            }
        }
    }
}

void OptimizedBrain::initializeNeuralStateFromBrain() {
    // Copy neural state from Brain's regions to SoA format
    size_t neuronIdx = 0;
    
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                
                neuralState_.membranePotential[neuronIdx] = state.membranePotential;
                neuralState_.restingPotential[neuronIdx] = state.restingPotential;
                neuralState_.threshold[neuronIdx] = state.threshold;
                neuralState_.resetPotential[neuronIdx] = state.resetPotential;
                neuralState_.leakConductance[neuronIdx] = state.leakConductance;
                neuralState_.neuronType[neuronIdx] = static_cast<uint64_t>(neuron->getType());
                
                // Initialize other fields
                neuralState_.synapseInput[neuronIdx] = 0.0f;
                neuralState_.refractoryRemaining[neuronIdx] = 0;
                neuralState_.firingState[neuronIdx] = 0;
                neuralState_.lastSpikeTime[neuronIdx] = state.lastSpikeTime;
                
                ++neuronIdx;
            }
        }
    }
}

void OptimizedBrain::initializeSynapseStateFromBrain() {
    // Copy synapse state from Brain's regions to SoA format
    size_t synapseIdx = 0;
    
    for (auto& region : pImpl->regions) {
        for (auto* synapse : region->getSynapses()) {
            synapseState_.sourceNeuron[synapseIdx] = synapse->getSourceNeuron().index();
            synapseState_.destinationNeuron[synapseIdx] = synapse->getDestinationNeuron().index();
            synapseState_.weight[synapseIdx] = synapse->getWeight();
            synapseState_.delay[synapseIdx] = synapse->getDelay();
            synapseState_.synapseType[synapseIdx] = static_cast<uint8_t>(synapse->getType());
            
            // Initialize other fields
            synapseState_.plasticityFlags[synapseIdx] = 0; // TODO: extract from synapse
            synapseState_.eligibilityTrace[synapseIdx] = 0.0f;
            synapseState_.efficacy[synapseIdx] = 1.0f;
            synapseState_.shortTermDepression[synapseIdx] = 1.0f;
            synapseState_.shortTermFacilitation[synapseIdx] = 0.0f;
            synapseState_.lastPreSpikeTime[synapseIdx] = -1.0f;
            synapseState_.lastPostSpikeTime[synapseIdx] = -1.0f;
            
            ++synapseIdx;
        }
    }
}

void OptimizedBrain::processAllNeurons(SimulationStep currentStep, Timestamp currentTime) {
    // Process all neurons (non-event-driven mode)
    if (multithreadingEnabled_) {
        processAllNeuronsParallel(currentStep, currentTime);
    } else {
        processAllNeuronsSequential(currentStep, currentTime);
    }
}

void OptimizedBrain::processSingleNeuron(size_t neuronIdx, uint64_t currentStep, double currentTime) {
    // Process single neuron using LIF dynamics
    float& V = neuralState_.membranePotential[neuronIdx];
    float& V_rest = neuralState_.restingPotential[neuronIdx];
    float threshold = neuralState_.threshold[neuronIdx];
    float V_reset = neuralState_.resetPotential[neuronIdx];
    float& I = neuralState_.synapseInput[neuronIdx];
    
    // LIF update
    float leak = (V_rest - V) / 20.0f; // Time constant = 20ms
    float input = I / 1.0f; // Capacitance = 1nF
    V += 0.001f * 1000.0f * (leak + input); // dt = 1ms
    
    // Check for spike
    if (V >= threshold) {
        // Spike recorded
        V = V_reset;
        neuralState_.refractoryRemaining[neuronIdx] = 5; // Refractory period
        neuralState_.firingState[neuronIdx] = 1;
        neuralState_.lastSpikeTime[neuronIdx] = static_cast<float>(currentTime);
        neuralState_.firingNeuronIndices.push_back(static_cast<uint32_t>(neuronIdx));
    } else {
        neuralState_.firingState[neuronIdx] = 0;
    }
    
    // Reset synaptic input for next step
    I = 0.0f;
}

void OptimizedBrain::recordSpikeEvent(size_t neuronIdx, uint64_t currentStep, double currentTime) {
    // Record spike event
    spikeQueue_.addSpike(SpikeEvent(
        NeuronId(static_cast<uint64_t>(neuronIdx)),
        currentTime,
        currentStep
    ));
}

void OptimizedBrain::processNeuronBatchParallel(const uint32_t* indices, size_t count, 
                                               uint64_t currentStep, double currentTime) {
    // Process neuron batch using parallel processing
    if (parallelProcessor_) {
        parallelProcessor_->processNeurons(count, 
            [this, indices, currentStep, currentTime](size_t i, ParallelNeuralProcessor::ThreadLocalState& state) {
                size_t neuronIdx = indices[i];
                processSingleNeuron(neuronIdx, currentStep, currentTime);
                // Update thread-local state
                if (neuralState_.firingState[neuronIdx]) {
                    ++state.localSpikeCount;
                    ++state.localFiringCount;
                }
            }
        );
    }
}

void OptimizedBrain::applyPlasticitySequential() {
    // Sequential plasticity application
    for (size_t i = 0; i < synapseState_.weight.size(); ++i) {
        // Update synapse based on spike history
        float weight = synapseState_.weight[i];
        
        // STDP update (simplified)
        float preTime = synapseState_.lastPreSpikeTime[i];
        float postTime = synapseState_.lastPostSpikeTime[i];
        
        if (preTime >= 0.0f && postTime >= 0.0f) {
            float dt = preTime - postTime;
            float deltaW = VectorizedSTDP().calculateDeltaW(dt);
            weight += deltaW;
            
            // Clamp weight
            weight = std::clamp(weight, -1.0f, 1.0f);
            synapseState_.weight[i] = weight;
        }
    }
}

void OptimizedBrain::applyPlasticityParallel() {
    // Parallel plasticity application
    if (parallelProcessor_) {
        parallelProcessor_->processNeurons(synapseState_.weight.size(),
            [this](size_t i, ParallelNeuralProcessor::ThreadLocalState& state) {
                // Update synapse plasticity
                float weight = synapseState_.weight[i];
                
                // STDP update
                float preTime = synapseState_.lastPreSpikeTime[i];
                float postTime = synapseState_.lastPostSpikeTime[i];
                
                if (preTime >= 0.0f && postTime >= 0.0f) {
                    float dt = preTime - postTime;
                    float deltaW = VectorizedSTDP().calculateDeltaW(dt);
                    weight += deltaW;
                    
                    // Clamp weight
                    weight = std::clamp(weight, -1.0f, 1.0f);
                    synapseState_.weight[i] = weight;
                }
            }
        );
    }
}

void OptimizedBrain::processAllNeuronsParallel(SimulationStep currentStep, Timestamp currentTime) {
    // Process all neurons in parallel
    if (parallelProcessor_) {
        parallelProcessor_->processNeurons(neuralState_.membranePotential.size(),
            [this, currentStep, currentTime](size_t i, ParallelNeuralProcessor::ThreadLocalState& state) {
                processSingleNeuron(i, currentStep, currentTime);
                // Update thread-local state
                if (neuralState_.firingState[i]) {
                    ++state.localSpikeCount;
                    ++state.localFiringCount;
                }
            }
        );
        
        // Collect statistics from parallel processor
        perfStats_.pendingSpikes = parallelProcessor_->getTotalSpikeCount();
        // Note: firing count not tracked in thread-local state, need to update elsewhere
    } else {
        processAllNeuronsSequential(currentStep, currentTime);
    }
}

void OptimizedBrain::processAllNeuronsSequential(SimulationStep currentStep, Timestamp currentTime) {
    // Process all neurons sequentially (fallback)
    for (size_t i = 0; i < neuralState_.membranePotential.size(); ++i) {
        processSingleNeuron(i, currentStep, currentTime);
    }
}

float OptimizedBrain::calculateAverageFiringRate() const {
    // Calculate average firing rate using optimized computation
    if (neuralState_.membranePotential.empty()) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    size_t count = 0;
    
    for (size_t i = 0; i < neuralState_.membranePotential.size(); ++i) {
        if (neuralState_.firingState[i]) {
            sum += 1.0f; // Each firing counts as 1 spike
            ++count;
        }
    }
    
    if (count == 0) {
        return 0.0f;
    }
    
    return sum / static_cast<float>(neuralState_.membranePotential.size());
}

OptimizedBrain::PerformanceStats OptimizedBrain::getPerformanceStats() const {
    return perfStats_;
}

void OptimizedBrain::enableEventDriven(bool enable) {
    eventDrivenEnabled_ = enable;
}

void OptimizedBrain::enableMultithreading(bool enable) {
    multithreadingEnabled_ = enable;
    if (!enable && parallelProcessor_) {
        parallelProcessor_.reset();
    }
}

void OptimizedBrain::enableSIMD(bool enable) {
    simdEnabled_ = enable;
}

void OptimizedBrain::enableSparseConnectivity(bool enable) {
    sparseEnabled_ = enable;
}

// Test optimized brain implementation
void testOptimizedBrain() {
    std::cout << "Optimized Brain Test" << std::endl;
    std::cout << "  Event-driven: enabled" << std::endl;
    std::cout << "  Multithreading: enabled" << std::endl;
    std::cout << "  SIMD: enabled" << std::endl;
    std::cout << "  Sparse connectivity: enabled" << std::endl;
    
    // Test neural state SoA
    OptimizedBrain::NeuralStateSoA neuralState;
    neuralState.initialize(1000);
    std::cout << "  Neural state memory: " << neuralState.memoryUsage() << " bytes" << std::endl;
    
    // Test synapse state SoA
    OptimizedBrain::SynapseStateSoA synapseState;
    synapseState.initialize(10000);
    std::cout << "  Synapse state memory: " << synapseState.memoryUsage() << " bytes" << std::endl;
    
    // Test performance stats
    OptimizedBrain::PerformanceStats stats;
    stats.activeNeurons = 100;
    stats.firingNeurons = 10;
    stats.pendingSpikes = 5;
    stats.avgFiringRate = 0.1f;
    std::cout << "  Performance stats captured successfully" << std::endl;
}

} // namespace nlm
