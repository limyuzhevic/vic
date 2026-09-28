#include "OptimizedBrainIntegration.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>
#include <atomic>

namespace nlm {

OptimizedBrainIntegration::OptimizedBrainIntegration(std::shared_ptr<Config> config)
    : eventDrivenEnabled_(true)
    , simdEnabled_(true)
    , multithreadingEnabled_(true)
    , cachingEnabled_(true)
    , threadCount_(std::thread::hardware_concurrency())
    , lastStep_(0)
    , lastTime_(0)
    , totalSimulationTime_(0.0)
    , stepCount_(0)
{
    // Create optimized subsystems with configuration
    agentBrain_ = std::make_unique<OptimizedAgentBrain>(std::make_shared<Brain>(config));
    workingMemory_ = std::make_unique<OptimizedNeuralWorkingMemory>();
    neuromodulator_ = std::make_unique<OptimizedNeuromodulator>();
    predictionSystem_ = std::make_unique<OptimizedNeuralPrediction>();
    
    integrateWithPerformanceFramework();
    
    NLM_LOG_INFO("OptimizedBrainIntegration initialized with all optimized subsystems");
}

OptimizedBrainIntegration::~OptimizedBrainIntegration() = default;

bool OptimizedBrainIntegration::initialize() {
    try {
        // Initialize optimized subsystems
        if (agentBrain_) {
            agentBrain_->initialize(); // Note: This needs to be modified to accept world parameter
        }
        
        if (workingMemory_) {
            workingMemory_->initialize(agentBrain_->getBrain().get());
        }
        
        if (predictionSystem_) {
            predictionSystem_->initialize(agentBrain_->getBrain().get());
        }
        
        // Setup performance framework
        setupEventDrivenProcessing();
        setupParallelProcessing();
        setupMemoryPools();
        
        // Validate integration
        if (!validateIntegration()) {
            NLM_LOG_ERROR("Integration validation failed");
            return false;
        }
        
        NLM_LOG_INFO("OptimizedBrainIntegration initialization completed successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Integration initialization failed: " + std::string(e.what()));
        return false;
    }
}

void OptimizedBrainIntegration::step(SimulationStep currentStep, Timestamp currentTime) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Update performance stats
    if (lastStep_ > 0) {
        double stepDuration = currentTime - lastTime_;
        perfStats_.simulationSpeed = 1000.0 / stepDuration; // Steps per second
    }
    
    // Batch process events for better performance
    batchProcessEvents();
    
    // Process active neurons with optimized methods
    if (eventDrivenEnabled_) {
        processSensoryInputOptimized(currentStep);
    }
    
    // Apply neuromodulation with smooth transitions
    applyNeuromodulationOptimized();
    
    // Update plasticity with optimized algorithms
    updatePlasticityOptimized();
    
    // Apply development updates
    developOptimized();
    
    // Generate optimized actions
    auto action = produceActionOptimized();
    if (action) {
        // Record action effects
        recordActionEffects(*action);
    }
    
    // Update performance metrics
    updatePerformanceMetrics();
    
    // Record timing
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    perfStats_.stepTimeMs = duration;
    
    // Update state
    lastStep_ = currentStep;
    lastTime_ = currentTime;
    stepCount_++;
}

void OptimizedBrainIntegration::processSensoryInputOptimized(const SensoryInput& input) {
    if (!agentBrain_) return;
    
    // Use optimized agent brain processing
    auto percept = input.toPercept();
    agentBrain_->processSensoryInput(percpt);
    
    // Update working memory with prediction integration
    if (workingMemory_) {
        auto prediction = predictionSystem_->generatePrediction(lastStep_);
        workingMemory_->integratePrediction(prediction);
    }
}

std::unique_ptr<Action> OptimizedBrainIntegration::produceActionOptimized() {
    if (!agentBrain_) return nullptr;
    
    // Use optimized agent brain to produce action
    return agentBrain_->produceAction();
}

void OptimizedBrainIntegration::applyNeuromodulationOptimized(const Neuromodulator& signal) {
    if (!agentBrain_ || !neuromodulator_) return;
    
    // Apply neuromodulation using optimized methods
    agentBrain_->applyNeuromodulation(signal);
    neuromodulator_->update(1.0f); // Assuming dt = 1.0
    
    // Update prediction system with neuromodulator
    predictionSystem_->modulatePredictionSynapses(neuromodulator_->getLevel(), 1.0f - neuromodulator_->getLevel());
}

void OptimizedBrainIntegration::updatePlasticityOptimized() {
    if (!agentBrain_) return;
    
    // Apply optimized plasticity updates
    agentBrain_->updatePlasticity();
}

void OptimizedBrainIntegration::developOptimized() {
    if (!agentBrain_) return;
    
    // Apply optimized development updates
    agentBrain_->updateDevelopment(1.0); // Assuming dt = 1.0
}

bool OptimizedBrainIntegration::saveOptimized(const std::string& filepath) const {
    // Optimized checkpointing with performance metrics
    try {
        // Save optimized brain state
        if (agentBrain_) {
            if (!agentBrain_->save(filepath)) {
                return false;
            }
        }
        
        // Save performance metrics
        std::string metricsFile = filepath + ".metrics";
        // Implementation would save performance statistics
        
        NLM_LOG_INFO("Optimized brain state saved to " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save optimized brain state: " + std::string(e.what()));
        return false;
    }
}

bool OptimizedBrainIntegration::loadOptimized(const std::string& filepath) {
    // Optimized checkpoint loading with performance restoration
    try {
        // Load optimized brain state
        if (agentBrain_) {
            if (!agentBrain_->load(filepath)) {
                return false;
            }
        }
        
        NLM_LOG_INFO("Optimized brain state loaded from " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load optimized brain state: " + std::string(e.what()));
        return false;
    }
}

OptimizedBrainIntegration::IntegrationPerformanceStats OptimizedBrainIntegration::getPerformanceStats() const {
    // Calculate comprehensive performance statistics
    IntegrationPerformanceStats stats = perfStats_;
    
    // Update dynamic metrics
    stats.activeNeurons = agentBrain_ ? agentBrain_->getBrain()->getActiveNeuronCount() : 0;
    stats.totalSpikes = agentBrain_ ? agentBrain_->getBrain()->getTotalSpikeCount() : 0;
    
    if (stepCount_ > 0) {
        stats.simulationSpeed = static_cast<double>(stepCount_) / (totalSimulationTime_ / 1000.0);
    }
    
    // Calculate memory efficiency
    size_t totalNeurons = agentBrain_ ? agentBrain_->getBrain()->getTotalNeuronCount() : 0;
    if (totalNeurons > 0) {
        stats.memoryEfficiency = static_cast<double>(getMemoryUsage()) / totalNeurons;
    }
    
    // Calculate cache efficiency
    size_t totalCacheAccesses = stats.cacheHits + stats.cacheMisses;
    if (totalCacheAccesses > 0) {
        stats.cacheHits = (stats.cacheHits * 100) / totalCacheAccesses;
    }
    
    // Calculate neuromodulator stability
    if (neuromodulator_) {
        auto neuromodStats = neuromodulator_->getStats();
        stats.neuromodulatorStability = 1.0f - neuromodStats.currentLevel * 0.1f;
    }
    
    return stats;
}

void OptimizedBrainIntegration::integrateWithPerformanceFramework() {
    // Setup event-driven processing
    setupEventDrivenProcessing();
    
    // Setup parallel processing
    setupParallelProcessing();
    
    // Setup memory pools
    setupMemoryPools();
}

void OptimizedBrainIntegration::setupEventDrivenProcessing() {
    if (!eventDrivenEnabled_) return;
    
    // Create event queue for spike processing
    spikeQueue_ = std::make_unique<SpikeRingBuffer>(16384); // Optimized capacity
    
    // Setup optimized neuron processing
    if (agentBrain_) {
        agentBrain_->enableEventDriven(eventDrivenEnabled_);
    }
}

void OptimizedBrainIntegration::setupParallelProcessing() {
    if (!multithreadingEnabled_) return;
    
    // Create parallel processor
    parallelProcessor_ = std::make_unique<ParallelNeuralProcessor>(threadCount_);
    
    // Configure parallel processing options
    parallelProcessor_->enableSIMD(simdEnabled_);
    parallelProcessor_->setWorkStealing(true);
}

void OptimizedBrainIntegration::setupMemoryPools() {
    // Create memory pools for neurons and synapses
    if (agentBrain_) {
        size_t neuronCount = agentBrain_->getBrain()->getTotalNeuronCount();
        size_t synapseCount = agentBrain_->getBrain()->getTotalSynapseCount();
        
        reserveMemoryPools(neuronCount, synapseCount);
    }
}

void OptimizedBrainIntegration::batchProcessEvents() {
    // Batch process spike events
    if (spikeQueue_ && !spikeQueue_->empty()) {
        // Process all pending spikes
        while (!spikeQueue_->empty()) {
            auto spike = spikeQueue_->pop();
            if (spike) {
                // Process spike event
                processSpikeEvent(*spike);
            }
        }
    }
}

void OptimizedBrainIntegration::batchProcessNeurons() {
    // Batch process neuron updates for better cache locality
    if (agentBrain_) {
        // Use optimized batch processing
        agentBrain_->batchProcessNeurons();
    }
}

void OptimizedBrainIntegration::batchProcessSynapses() {
    // Batch process synapse updates
    if (agentBrain_) {
        agentBrain_->batchProcessSynapses();
    }
}

void OptimizedBrainIntegration::updateCacheEfficiency() {
    // Update cache efficiency metrics
    if (predictionSystem_) {
        auto predStats = predictionSystem_->getPerformanceStats();
        perfStats_.cacheHits = predStats.cacheHits;
        perfStats_.cacheMisses = predStats.cacheMisses;
    }
}

void OptimizedBrainIntegration::optimizeMemoryLayout() {
    // Optimize memory layout for better performance
    if (workingMemory_) {
        workingMemory_->optimizeMemoryLayout();
    }
    
    if (agentBrain_) {
        agentBrain_->optimizeMemoryLayout();
    }
}

void OptimizedBrainIntegration::updatePerformanceMetrics() {
    // Update comprehensive performance metrics
    updateCacheEfficiency();
    optimizeMemoryLayout();
    
    // Calculate efficiency scores
    perfStats_.cpuUtilization = std::min(100.0, perfStats_.stepTimeMs * 10.0); // Approximate
    perfStats_.memoryUsageMB = static_cast<float>(getMemoryUsage()) / (1024 * 1024);
    
    // Update simulation time
    totalSimulationTime_ += perfStats_.stepTimeMs;
}

void OptimizedBrainIntegration::logPerformanceStats() const {
    // Log comprehensive performance statistics
    const auto& stats = getPerformanceStats();
    
    NLM_LOG_INFO("=== Optimized Brain Performance Statistics ===");
    NLM_LOG_INFO("Simulation Speed: " + std::to_string(stats.simulationSpeed) + " steps/sec");
    NLM_LOG_INFO("Memory Efficiency: " + std::to_string(stats.memoryEfficiency) + " bytes/neuron");
    NLM_LOG_INFO("CPU Utilization: " + std::to_string(stats.cpuUtilization) + "%");
    NLM_LOG_INFO("Cache Hit Rate: " + std::to_string((stats.cacheHits * 100) / std::max(1, stats.cacheHits + stats.cacheMisses)) + "%");
    NLM_LOG_INFO("Active Neurons: " + std::to_string(stats.activeNeurons));
    NLM_LOG_INFO("Total Spikes: " + std::to_string(stats.totalSpikes));
    NLM_LOG_INFO("Memory Usage: " + std::to_string(stats.memoryUsageMB) + " MB");
    NLM_LOG_INFO("Average Step Time: " + std::to_string(stats.stepTimeMs) + " ms");
    NLM_LOG_INFO("Neuromodulator Stability: " + std::to_string(stats.neuromulatorStability));
    NLM_LOG_INFO("Prediction Error: " + std::to_string(stats.predictionError));
    NLM_LOG_INFO("========================================================");
}

bool OptimizedBrainIntegration::validateIntegration() const {
    // Validate that all subsystems are properly integrated
    bool valid = true;
    
    if (!agentBrain_) {
        NLM_LOG_ERROR("AgentBrain not initialized");
        valid = false;
    }
    
    if (!workingMemory_) {
        NLM_LOG_ERROR("WorkingMemory not initialized");
        valid = false;
    }
    
    if (!neuromodulator_) {
        NLM_LOG_ERROR("Neuromodulator not initialized");
        valid = false;
    }
    
    if (!predictionSystem_) {
        NLM_LOG_ERROR("PredictionSystem not initialized");
        valid = false;
    }
    
    // Validate performance framework integration
    if (!spikeQueue_) {
        NLM_LOG_WARNING("SpikeQueue not initialized");
    }
    
    if (!parallelProcessor_) {
        NLM_LOG_WARNING("ParallelProcessor not initialized");
    }
    
    if (!neuronPool_) {
        NLM_LOG_WARNING("NeuronPool not initialized");
    }
    
    return valid;
}

bool OptimizedBrainIntegration::benchmarkPerformance() const {
    // Run performance benchmarks
    bool passed = true;
    
    const auto& stats = getPerformanceStats();
    
    // Check minimum performance requirements
    if (stats.simulationSpeed < 100.0) {  // At least 100 steps per second
        NLM_LOG_WARNING("Simulation speed below minimum: " + std::to_string(stats.simulationSpeed) + " steps/sec");
        passed = false;
    }
    
    if (stats.memoryUsageMB > 512.0) {  // Limit memory usage
        NLM_LOG_WARNING("Memory usage above limit: " + std::to_string(stats.memoryUsageMB) + " MB");
        passed = false;
    }
    
    if (stats.stepTimeMs > 10.0) {  // Limit step time
        NLM_LOG_WARNING("Step time above limit: " + std::to_string(stats.stepTimeMs) + " ms");
        passed = false;
    }
    
    return passed;
}

void OptimizedBrainIntegration::reserveMemoryPools(size_t neurons, size_t synapses) {
    // Reserve memory pools for better allocation performance
    neuronPool_ = std::make_unique<MemoryPool<NeuronActivation>>(neurons / 10, 4);
    activationPool_ = std::make_unique<MemoryPool<float>>(synapses / 10, 4);
}

size_t OptimizedBrainIntegration::getMemoryUsage() const {
    size_t total = 0;
    
    if (neuronPool_) total += neuronPool_->memoryUsage();
    if (activationPool_) total += activationPool_->memoryUsage();
    
    if (spikeQueue_) total += spikeQueue_->capacity() * sizeof(QueuedSpikeEvent);
    
    if (parallelProcessor_) total += parallelProcessor_->getMemoryUsage();
    
    // Add memory for optimized subsystems
    if (agentBrain_) total += agentBrain_->getMemoryUsage();
    if (workingMemory_) total += workingMemory_->getMemoryUsage();
    if (neuromodulator_) total += neuromodulator_->getMemoryUsage();
    if (predictionSystem_) total += predictionSystem_->getMemoryUsage();
    
    return total;
}

void OptimizedBrainIntegration::processSpikeEvent(const QueuedSpikeEvent& spike) {
    // Process spike event with optimized handling
    if (agentBrain_) {
        agentBrain_->processSpikeEvent(spike);
    }
}

} // namespace nlm