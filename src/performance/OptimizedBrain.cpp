#include "OptimizedBrain.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <numeric>

namespace nlm {

OptimizedBrain::OptimizedBrain(std::shared_ptr<Config> config)
    : Brain(std::move(config))
    , eventDrivenEnabled_(false)
    , multithreadingEnabled_(false)
    , simdEnabled_(false)
    , sparseEnabled_(false)
    , threadCount_(1)
    , activeNeuronCount_(0)
    , totalSteps_(0)
{
}

OptimizedBrain::~OptimizedBrain() = default;

bool OptimizedBrain::initialize() {
    // Call base initialization first
    if (!Brain::initialize()) {
        return false;
    }
    
    // Get configuration
    auto config = getConfig();
    size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = config->getOr<size_t>("region_count", 1);
    
    // Initialize neural state
    neuralState_.initialize(neuronCount);
    neuronIndices_.reserve(neuronCount);
    activeNeurons_.reserve(neuronCount);
    
    // Initialize synapse state
    synapseState_.initialize(regionCount * 100); // Approximate synapse count
    
    // Initialize connectivity
    initializeSparseConnectivity();
    
    // Initialize parallel processor
    parallelProcessor_ = std::make_unique<ParallelNeuralProcessor>(threadCount_);
    
    NLM_LOG_INFO("OptimizedBrain initialized (neuronCount=" + std::to_string(neuronCount) +
                 ", regions=" + std::to_string(regionCount) + ")");
    
    return true;
}

void OptimizedBrain::initializeSparseConnectivity() {
    size_t neuronCount = neuralState_.membranePotential.size();
    
    for (size_t i = 0; i < neuronCount; ++i) {
        // Create sparse connectivity for each neuron
        // Simplified implementation - would use adjacency lists
        uint32_t numConnections = std::min(static_cast<uint32_t>(neuronCount) / 10, 10u);
        
        for (uint32_t j = 0; j < numConnections; ++j) {
            // Random connectivity
            uint32_t target = (i + j + 1) % neuronCount;
            float weight = ((i ^ j) & 1) ? 0.5f : -0.5f;
            
            connectivity_.addConnection(i, target, weight);
        }
    }
}

void OptimizedBrain::step(SimulationStep currentStep) {
    std::shared_lock<std::shared_mutex> lock(stepMutex_);
    
    double currentTime = static_cast<double>(currentStep) * timestep;
    
    if (eventDrivenEnabled_) {
        processActiveNeurons(currentStep, currentTime);
    } else {
        // Standard step processing
        for (auto& region : getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(currentTime, timestep);
                }
            }
        }
    }
    
    updateStatistics();
    totalSteps_++;
}

void OptimizedBrain::step(SimulationStep currentStep, Timestamp currentTime) {
    std::shared_lock<std::shared_mutex> lock(stepMutex_);
    
    if (eventDrivenEnabled_) {
        processActiveNeurons(currentStep, currentTime);
    } else {
        // Standard step processing
        for (auto& region : getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(currentTime, timestep);
                }
            }
        }
    }
    
    updateStatistics();
    totalSteps_++;
}

void OptimizedBrain::processActiveNeurons(uint64_t currentStep, double currentTime) {
    // Get active neurons based on synaptic input
    neuralState_.firingNeurons.clear();
    neuralState_.activeNeurons.clear();
    
    for (size_t i = 0; i < neuralState_.membranePotential.size(); ++i) {
        if (neuralState_.synapticInput[i] > 0.1f) {
            neuralState_.activeNeurons.push_back(i);
        }
    }
    
    // Process active neurons
    if (multithreadingEnabled_) {
        parallelProcessor_->processNeurons(
            neuralState_.activeNeurons.size(),
            [this, currentStep, currentTime](size_t idx, auto& state) {
                size_t neuronIdx = neuralState_.activeNeurons[idx];
                processActiveNeuron(neuronIdx, currentStep, currentTime, state);
            },
            nullptr
        );
    } else {
        for (size_t idx : neuralState_.activeNeurons) {
            ThreadLocalState dummyState;
            processActiveNeuron(idx, currentStep, currentTime, dummyState);
        }
    }
    
    // Update active neuron count
    activeNeuronCount_ = neuralState_.activeNeurons.size();
}

void OptimizedBrain::processActiveNeuron(size_t neuronIdx, uint64_t currentStep, double currentTime, ThreadLocalState& state) {
    // Process a single active neuron
    float& V = neuralState_.membranePotential[neuronIdx];
    float& refractory = neuralState_.refractoryRemaining[neuronIdx];
    
    if (refractory > 0) {
        refractory--;
        V = neuralState_.restingPotential[neuronIdx];
    } else {
        float leak = (neuralState_.restingPotential[neuronIdx] - V) / lifVectorizer_.tau;
        float input = neuralState_.synapticInput[neuronIdx] / lifVectorizer_.C;
        V += static_cast<float>(timestep) * 1000.0f * (leak + input);
        
        if (V >= neuralState_.threshold[neuronIdx]) {
            V = neuralState_.resetPotential[neuronIdx];
            refractory = static_cast<uint32_t>(lifVectorizer_.refractoryDecay);
            neuralState_.firingNeurons.push_back(neuronIdx);
            state.localSpikeCount++;
            state.localFiringCount++;
        }
    }
}

void OptimizedBrain::processFiringNeurons(uint64_t currentStep, double currentTime) {
    // Process neurons that fired this step
    for (size_t idx : neuralState_.firingNeurons) {
        // Update synaptic connections
        connectivity_.processSpike(idx, currentStep, currentTime);
    }
}

void OptimizedBrain::updateNeuronsBatch(size_t begin, size_t end, uint64_t currentStep, double currentTime) {
    // Vectorized neuron update for batch processing
    size_t count = end - begin;
    
    if (simdEnabled_ && count >= SIMDVectorization::SIMD_WIDTH) {
        // Vectorized processing
        lifVectorizer_.update(
            neuralState_.membranePotential,
            neuralState_.synapticInput,
            neuralState_.refractoryRemaining,
            neuralState_.firingState,
            end - begin
        );
    } else {
        // Scalar processing for small batches
        for (size_t i = begin; i < end; ++i) {
            // Update neuron state
            float& V = neuralState_.membranePotential[i];
            float& refractory = neuralState_.refractoryRemaining[i];
            
            if (refractory > 0) {
                refractory--;
                V = neuralState_.restingPotential[i];
            } else {
                float leak = (neuralState_.restingPotential[i] - V) / lifVectorizer_.tau;
                float input = neuralState_.synapticInput[i] / lifVectorizer_.C;
                V += static_cast<float>(timestep) * 1000.0f * (leak + input);
                
                if (V >= neuralState_.threshold[i]) {
                    V = neuralState_.resetPotential[i];
                    refractory = static_cast<uint32_t>(lifVectorizer_.refractoryDecay);
                    neuralState_.firingState[i] = 1;
                }
            }
        }
    }
}

void OptimizedBrain::updateSynapsesBatch(size_t begin, size_t end, double currentTime) {
    // Batch synapse update
    for (size_t i = begin; i < end; ++i) {
        // Update synaptic states
        // Simplified implementation
    }
}

void OptimizedBrain::applyPlasticity() {
    // Apply STDP and Hebbian plasticity
    // This would integrate with the brain's plasticity systems
}

void OptimizedBrain::updateStatistics() {
    // Update performance statistics
    perfStats_.neuronsPerSecond = 
        static_cast<double>(getTotalNeuronCount()) / perfStats_.stepTimeMs * 1000.0;
    
    perfStats_.spikesPerSecond = 
        static_cast<double>(getTotalSpikeCount()) / perfStats_.stepTimeMs * 1000.0;
    
    perfStats_.activeNeurons = activeNeuronCount_;
    perfStats_.firingNeurons = neuralState_.firingNeurons.size();
    perfStats_.pendingSpikes = getPendingSpikeEventCount();
    perfStats_.pendingDelayedSpikes = getPendingSpikeEventCount(); // Approximation
    
    perfStats_.avgFiringRate = getAverageFiringRate();
    perfStats_.memoryUsageMB = getMemoryUsage() / (1024 * 1024);
    perfStats_.stepTimeMs = perfStats_.stepTimeMs; // Would track this
}

OptimizedBrain::PerformanceStats OptimizedBrain::getPerformanceStats() const {
    return perfStats_;
}

void OptimizedBrain::develop() {
    // Development system updates structural plasticity
    if (getStructuralPlasticity()) {
        getStructuralPlasticity()->update(this, *getRandomGenerator());
    }
}

OptimizedBrain::NeuralStateSoA::NeuralStateSoA() {}

OptimizedBrain::SynapseStateSoA::SynapseStateSoA() {}

} // namespace nlm
