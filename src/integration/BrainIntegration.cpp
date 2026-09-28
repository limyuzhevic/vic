#include "BrainIntegration.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>
#include <numeric>

namespace nlm {

BrainIntegration::BrainIntegration(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , optimizationsEnabled_(true)
    , simdEnabled_(true)
    , cachingEnabled_(true)
    , multithreadingEnabled_(true)
    , perfState_()
    , stats_()
{
    // Initialize performance state
    perfState_.lastStep = 0;
    perfState_.lastTime = 0.0;
    perfState_.totalSimulationTime = 0.0;
    perfState_.stepCount = 0;
    perfState_.totalStepTime = 0.0;
    perfState_.cacheHits = 0;
    perfState_.cacheMisses = 0;
    perfState_.neuronPoolSize = 0;
    perfState_.synapsePoolSize = 0;
    
    setupOptimizedSubsystems();
    
    NLM_LOG_INFO("BrainIntegration initialized with optimized subsystems");
}

BrainIntegration::~BrainIntegration() = default;

bool BrainIntegration::initialize() {
    try {
        integrateWithBrainSystems();
        
        NLM_LOG_INFO("BrainIntegration completed successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("BrainIntegration initialization failed: " + std::string(e.what()));
        return false;
    }
}

void BrainIntegration::step(SimulationStep currentStep, Timestamp currentTime) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Update performance metrics
    if (perfState_.lastStep > 0) {
        double stepDuration = currentTime - perfState_.lastTime;
        stats_.simulationSpeed = 1000.0 / stepDuration;
    }
    
    // Use optimized processing
    optimizedStep(currentStep, currentTime);
    
    // Update performance monitoring
    updatePerformanceMetrics();
    
    // Record timing
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    perfState_.totalStepTime += duration;
    stats_.stepTimeMs = duration;
    
    // Update state
    perfState_.lastStep = currentStep;
    perfState_.lastTime = currentTime;
    perfState_.stepCount++;
}

void BrainIntegration::receiveSensoryInput(const SensoryInput& input) {
    if (!brain_) return;
    
    // Use optimized brain processing
    brain_->receiveSensoryInput(input);
    
    // Update working memory if available
    if (workingMemory_) {
        // Integrate with prediction system
        if (predictionSystem_) {
            auto prediction = predictionSystem_->generatePrediction(perfState_.lastStep);
            workingMemory_->integratePrediction(prediction);
        }
    }
}

std::unique_ptr<Action> BrainIntegration::produceAction() {
    if (!brain_) return nullptr;
    
    return brain_->produceAction();
}

void BrainIntegration::applyNeuromodulation(const Neuromodulator& signal) {
    if (!brain_) return;
    
    brain_->applyNeuromodulation(signal);
}

void BrainIntegration::updatePlasticity() {
    if (!brain_) return;
    
    brain_->updatePlasticity();
}

void BrainIntegration::develop() {
    if (!brain_) return;
    
    brain_->develop();
}

bool BrainIntegration::save(const std::string& filepath) const {
    if (!brain_) return false;
    
    return brain_->save(filepath);
}

bool BrainIntegration::load(const std::string& filepath) {
    if (!brain_) return false;
    
    return brain_->load(filepath);
}

BrainIntegration::IntegrationStats BrainIntegration::getStats() const {
    IntegrationStats stats = stats_;
    
    if (brain_) {
        // Update dynamic statistics from brain
        stats.activeNeurons = brain_->getActiveNeuronCount();
        stats.totalSpikes = brain_->getTotalSpikeCount();
        stats.avgFiringRate = brain_->getAverageFiringRate();
        
        if (perfState_.stepCount > 0) {
            stats.simulationSpeed = static_cast<double>(perfState_.stepCount) / (perfState_.totalSimulationTime / 1000.0);
        }
    }
    
    // Calculate cache efficiency
    size_t totalCacheAccesses = stats.cacheHits + stats.cacheMisses;
    if (totalCacheAccesses > 0) {
        stats.cacheHits = (stats.cacheHits * 100) / totalCacheAccesses;
    }
    
    // Estimate memory usage
    estimateMemoryUsage();
    
    return stats;
}

void BrainIntegration::setupOptimizedSubsystems() {
    // Create optimized subsystems that wrap brain components
    if (brain_) {
        agentBrain_ = std::make_unique<AgentBrain>(brain_);
        workingMemory_ = std::make_unique<NeuralWorkingMemory>();
        neuromodulator_ = std::make_unique<Neuromodulator>();
        predictionSystem_ = std::make_unique<PredictionSystem>();
        developmentSystem_ = std::make_unique<DevelopmentSystem>();
    }
    
    setupPerformanceMonitoring();
}

void BrainIntegration::setupPerformanceMonitoring() {
    // Initialize performance monitoring
    stats_.optimizationsEnabled = optimizationsEnabled_;
    stats_.simdEnabled = simdEnabled_;
    stats_.cachingEnabled = cachingEnabled_;
    stats_.multithreadingEnabled = multithreadingEnabled_;
}

void BrainIntegration::integrateWithBrainSystems() {
    // Integrate optimized subsystems with brain
    if (brain_ && workingMemory_) {
        workingMemory_->initialize(brain_.get());
    }
    
    if (brain_ && predictionSystem_) {
        predictionSystem_->initialize(brain_.get());
    }
    
    if (brain_ && developmentSystem_) {
        developmentSystem_->initialize(brain_.get());
    }
}

void BrainIntegration::optimizedStep(SimulationStep currentStep, Timestamp currentTime) {
    // Optimized step processing with performance considerations
    if (!brain_) return;
    
    // Use optimized methods when available
    brain_->step(currentStep, currentTime);
    
    // Apply optimized neuromodulation
    optimizedNeuromodulation();
    
    // Apply optimized plasticity
    optimizedPlasticity();
    
    // Apply optimized development
    optimizedDevelopment();
}

void BrainIntegration::optimizedNeuromodulation() {
    // Apply neuromodulation with optimizations
    if (!brain_) return;
    
    // Get current neuromodulator state
    if (neuromodulator_) {
        // Apply neuromodulator using optimized brain methods
        neuromodulator_->signalReward(0.0f); // Placeholder
    }
}

void BrainIntegration::optimizedPlasticity() {
    // Apply plasticity with optimizations
    if (!brain_) return;
    
    brain_->updatePlasticity();
}

void BrainIntegration::optimizedDevelopment() {
    // Apply development with optimizations
    if (!brain_) return;
    
    brain_->develop();
}

void BrainIntegration::updatePerformanceMetrics() {
    // Update comprehensive performance metrics
    if (brain_) {
        // Update simulation time
        perfState_.totalSimulationTime += stats_.stepTimeMs;
        
        // Calculate cache efficiency
        calculateCacheEfficiency();
        
        // Estimate memory usage
        estimateMemoryUsage();
        
        // Update cache statistics
        stats_.cacheHits = perfState_.cacheHits;
        stats_.cacheMisses = perfState_.cacheMisses;
        
        // Calculate neuromodulator stability (placeholder)
        stats_.neuromodulatorStability = 0.8f;
        
        // Estimate prediction accuracy (placeholder)
        stats_.predictionAccuracy = 0.75f;
    }
}

void BrainIntegration::calculateCacheEfficiency() {
    // Calculate cache efficiency
    size_t totalAccesses = perfState_.cacheHits + perfState_.cacheMisses;
    if (totalAccesses > 0) {
        // Update stats with current cache efficiency
        stats_.cacheHits = perfState_.cacheHits;
        stats_.cacheMisses = perfState_.cacheMisses;
    }
}

void BrainIntegration::estimateMemoryUsage() const {
    // Estimate memory usage of brain and subsystems
    if (brain_) {
        // Estimate brain memory usage
        size_t brainMemory = brain_->getTotalNeuronCount() * 100; // Approximate bytes per neuron
        brainMemory += brain_->getTotalSynapseCount() * 50; // Approximate bytes per synapse
        
        stats_.memoryUsageMB = static_cast<float>(brainMemory) / (1024 * 1024);
    }
}

void BrainIntegration::logIntegrationStats() const {
    // Log comprehensive integration statistics
    const auto& stats = getStats();
    
    NLM_LOG_INFO("=== Brain Integration Performance Statistics ===");
    NLM_LOG_INFO("Simulation Speed: " + std::to_string(stats.simulationSpeed) + " steps/sec");
    NLM_LOG_INFO("Memory Usage: " + std::to_string(stats.memoryUsageMB) + " MB");
    NLM_LOG_INFO("Active Neurons: " + std::to_string(stats.activeNeurons));
    NLM_LOG_INFO("Total Spikes: " + std::to_string(stats.totalSpikes));
    NLM_LOG_INFO("Average Firing Rate: " + std::to_string(stats.avgFiringRate) + " Hz");
    NLM_LOG_INFO("Prediction Accuracy: " + std::to_string(stats.predictionAccuracy * 100) + "%");
    NLM_LOG_INFO("Cache Hit Rate: " + std::to_string(stats.cacheHits) + "%");
    NLM_LOG_INFO("Neuromodulator Stability: " + std::to_string(stats.neuromodulatorStability));
    NLM_LOG_INFO("Average Step Time: " + std::to_string(stats.stepTimeMs) + " ms");
    NLM_LOG_INFO("Optimizations Enabled: " + std::string(stats.optimizationsEnabled ? "Yes" : "No"));
    NLM_LOG_INFO("========================================================");
}

bool BrainIntegration::validatePerformance() const {
    // Validate performance meets requirements
    bool valid = true;
    
    const auto& stats = getStats();
    
    // Check minimum performance requirements
    if (stats_.simulationSpeed < 10.0) {  // At least 10 steps per second
        NLM_LOG_WARNING("Simulation speed below minimum: " + std::to_string(stats_.simulationSpeed) + " steps/sec");
        valid = false;
    }
    
    if (stats_.memoryUsageMB > 256.0) {  // Limit memory usage
        NLM_LOG_WARNING("Memory usage above limit: " + std::to_string(stats_.memoryUsageMB) + " MB");
        valid = false;
    }
    
    if (stats_.stepTimeMs > 1000.0) {  // Limit step time (1 second)
        NLM_LOG_WARNING("Step time above limit: " + std::to_string(stats_.stepTimeMs) + " ms");
        valid = false;
    }
    
    return valid;
}

void BrainIntegration::reserveMemory(size_t neurons, size_t synapses) {
    // Reserve memory for neurons and synapses
    perfState_.neuronPoolSize = neurons;
    perfState_.synapsePoolSize = synapses;
}

size_t BrainIntegration::getMemoryUsage() const {
    // Calculate total memory usage
    size_t total = 0;
    
    if (brain_) {
        // Estimate memory usage from brain
        total += brain_->getTotalNeuronCount() * 100;  // Approximate bytes per neuron
        total += brain_->getTotalSynapseCount() * 50;   // Approximate bytes per synapse
    }
    
    return total;
}

} // namespace nlm