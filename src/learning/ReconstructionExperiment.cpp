#include "ReconstructionExperiment.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <limits>

namespace nlm {

ReconstructionExperiment::ReconstructionExperiment(std::shared_ptr<Config> config, size_t duration)
    : config(config), duration(duration), randomGen(12345) {
    
    NLM_LOG_INFO("Reconstruction Experiment initialized");
    NLM_LOG_INFO("Duration: " + std::to_string(duration) + " steps");
    
    // Initialize statistics
    stats = Statistics();
    stats.bestError = std::numeric_limits<float>::max();
}

ReconstructionExperiment::~ReconstructionExperiment() = default;

void ReconstructionExperiment::initialize() {
    // Create brain with reconstruction learning config
    if (!config) {
        config = std::make_shared<Config>();
        
        // Configure for reconstruction learning
        config->set("random_seed", 12345LL, ConfigSource::Default);
        config->set("neuron_count", 2000, ConfigSource::Default);
        config->set("region_count", 2, ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Reconstruction-specific parameters
        config->set("reconstruction_learning_rate", 0.01f, ConfigSource::Default);
        config->set("reconstruction_error_threshold", 0.001f, ConfigSource::Default);
    }
    
    // Initialize brain
    brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for reconstruction experiment");
        return;
    }
    
    // Initialize reconstruction error system
    reconstructionError.initialize(brain.get(),
                                  config->getOr<float>("reconstruction_learning_rate", 0.01f),
                                  config->getOr<size_t>("reconstruction_buffer_size", 1000));
    
    startTime = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("Brain initialized for reconstruction experiment");
    NLM_LOG_INFO("Neuron count: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Region count: " + std::to_string(brain->getRegionCount()));
}

void ReconstructionExperiment::run() {
    NLM_LOG_INFO("=== Starting Reconstruction Experiment ===");
    
    // Initialize
    initialize();
    
    // Generate initial target
    generateNewTarget();
    
    // Run simulation steps
    for (size_t step = 0; step < duration; ++step) {
        runStep(step);
        updateStatistics();
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Error: " + std::to_string(stats.averageError) +
                        " | Progress: " + std::to_string(stats.learningProgress));
        }
        
        // Generate new target every 50 steps
        if (step % 50 == 0 && step > 0) {
            generateNewTarget();
        }
    }
    
    NLM_LOG_INFO("=== Reconstruction Experiment Complete ===");
    printResults();
}

void ReconstructionExperiment::runStep(size_t step) {
    // Get current brain state (reconstruction)
    std::vector<float> reconstruction = getCurrentReconstruction();
    
    // Compute reconstruction error
    reconstructionError.computeError(reconstruction);
    
    // Apply learning based on reconstruction error
    reconstructionError.applyLearning(randomGen);
    
    // Inject target as sensory input to help learning
    if (!targetHistory.empty()) {
        const auto& latestTarget = targetHistory.back();
        injectPattern(latestTarget);
    }
    
    // Step the brain
    brain->step(step, step * 0.001f);
}

std::vector<float> ReconstructionExperiment::generateTargetPattern(size_t size) {
    std::vector<float> pattern(size);
    
    // Generate random pattern with specific characteristics
    // Based on pattern type and configuration
    
    // Type 1: Simple random pattern
    for (size_t i = 0; i < size; ++i) {
        pattern[i] = randomGen.uniformReal(-1.0f, 1.0f);
    }
    
    return pattern;
}

void ReconstructionExperiment::injectPattern(const std::vector<float>& pattern) {
    // Convert pattern to sensory input
    // Use multiple sensory neurons to encode the pattern
    
    // Get sensory neurons from brain
    std::vector<Neuron*> sensoryNeurons = brain->getSensoryNeurons();
    
    if (sensoryNeurons.empty()) {
        return;
    }
    
    // Distribute pattern across sensory neurons
    size_t numNeurons = sensoryNeurons.size();
    size_t patternSize = pattern.size();
    
    for (size_t i = 0; i < numNeurons && i < patternSize; ++i) {
        // Normalize pattern value to appropriate range
        float normalizedValue = std::clamp(pattern[i], -1.0f, 1.0f);
        
        // Inject into sensory neuron
        // (This would require access to the brain's injectCurrent method)
        // For now, we'll just log the injection
        NLM_LOG_DEBUG("Injecting pattern value " + std::to_string(normalizedValue) +
                    " into sensory neuron " + std::to_string(i));
    }
}

std::vector<float> ReconstructionExperiment::getCurrentReconstruction() const {
    // This is a placeholder implementation
    // In a real implementation, this would extract the current brain state
    // as a reconstruction target
    
    // For now, return a dummy vector
    // This would ideally come from:
    // 1. Working memory contents
    // 2. Neural population activities
    // 3. Synaptic weight patterns
    // 4. Internal representation states
    
    std::vector<float> reconstruction(100, 0.0f);
    
    // Try to get reconstruction from brain if possible
    if (brain && brain->getWorkingMemory()) {
        // Access working memory traces
        // This depends on working memory API
    }
    
    return reconstruction;
}

void ReconstructionExperiment::updateStatistics() {
    // Update experiment statistics
    stats.stepsCompleted++;
    
    // Update target count
    stats.targetsGenerated = targetHistory.size();
    
    // Update average error (simplified)
    if (!stats.errorHistory.empty()) {
        float sum = std::accumulate(stats.errorHistory.begin(), stats.errorHistory.end(), 0.0f);
        stats.averageError = sum / stats.errorHistory.size();
        
        // Update best error
        stats.bestError = std::min(stats.bestError, stats.averageError);
    }
    
    // Update learning progress
    stats.learningProgress = reconstructionError.getLearningProgress();
    
    // Update error history
    if (!reconstructionError.getErrorHistory().empty()) {
        stats.errorHistory = reconstructionError.getErrorHistory();
    }
    
    // Update reconstruction history
    if (!reconstructionError.getReconstructionHistory().empty()) {
        for (const auto& recon : reconstructionError.getReconstructionHistory()) {
            stats.reconstructionHistory.insert(stats.reconstructionHistory.end(), 
                                             recon.begin(), recon.end());
        }
    }
}

void ReconstructionExperiment::generateNewTarget() {
    size_t targetSize = 100;  // Target pattern size
    auto target = generateTargetPattern(targetSize);
    
    // Set target in reconstruction error system
    reconstructionError.setTarget(target);
    
    // Add to history
    targetHistory.push_back(target);
    
    NLM_LOG_DEBUG("Generated new target pattern (size: " + std::to_string(targetSize) + ")");
}

void ReconstructionExperiment::printResults() const {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Reconstruction Experiment Results ===");
    NLM_LOG_INFO("")
    NLM_LOG_INFO("Duration: " + std::to_string(duration) + " steps");
    NLM_LOG_INFO("Targets generated: " + std::to_string(stats.targetsGenerated));
    NLM_LOG_INFO("Steps completed: " + std::to_string(stats.stepsCompleted));
    NLM_LOG_INFO("Average reconstruction error: " + std::to_string(stats.averageError));
    NLM_LOG_INFO("Best error achieved: " + std::to_string(stats.bestError));
    NLM_LOG_INFO("Learning progress: " + std::to_string(stats.learningProgress));
    NLM_LOG_INFO("Converged steps: " + std::to_string(reconstructionError.getConvergedSteps()));
    NLM_LOG_INFO("")
    
    // Determine experiment outcome
    if (stats.bestError < 0.01f) {
        NLM_LOG_INFO("✓ SUCCESS: Reconstruction learning achieved high accuracy");
        NLM_LOG_INFO("  The brain successfully learned to reconstruct patterns");
    } else if (stats.bestError < 0.1f) {
        NLM_LOG_INFO("✓ PARTIAL SUCCESS: Reconstruction learning showed moderate improvement");
        NLM_LOG_INFO("  The brain made some progress in pattern reconstruction");
    } else {
        NLM_LOG_INFO("✗ LIMITED SUCCESS: Reconstruction learning showed limited improvement");
        NLM_LOG_INFO("  The brain may need more training or different parameters");
    }
}

} // namespace nlm