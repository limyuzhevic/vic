#include "MetaLearningExperiment.hpp"
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

MetaLearningExperiment::MetaLearningExperiment(std::shared_ptr<Config> config, size_t duration)
    : config(config), duration(duration), randomGen(12345) {
    
    NLM_LOG_INFO("Meta-Learning Experiment initialized");
    NLM_LOG_INFO("Duration: " + std::to_string(duration) + " steps");
    
    // Initialize statistics
    stats = Statistics();
    stats.bestLearningPerformance = std::numeric_limits<float>::lowest();
}

MetaLearningExperiment::~MetaLearningExperiment() = default;

void MetaLearningExperiment::initialize() {
    // Create brain with meta-learning config
    if (!config) {
        config = std::make_shared<Config>();
        
        // Configure for meta-learning
        config->set("random_seed", 12345LL, ConfigSource::Default);
        config->set("neuron_count", 2000, ConfigSource::Default);
        config->set("region_count", 2, ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Meta-learning-specific parameters
        config->set("base_learning_rate", 0.01f, ConfigSource::Default);
        config->set("meta_memory_size", 1000, ConfigSource::Default);
    }
    
    // Initialize brain
    brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for meta-learning experiment");
        return;
    }
    
    // Initialize meta-learning system
    metaLearning.initialize(brain.get(),
                           config->getOr<float>("base_learning_rate", 0.01f),
                           config->getOr<size_t>("meta_memory_size", 1000));
    
    startTime = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("Brain initialized for meta-learning experiment");
    NLM_LOG_INFO("Neuron count: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Region count: " + std::to_string(brain->getRegionCount()));
}

void MetaLearningExperiment::run() {
    NLM_LOG_INFO("=== Starting Meta-Learning Experiment ===");
    
    // Initialize
    initialize();
    
    // Generate initial state
    generateNewState();
    
    // Run simulation steps
    for (size_t step = 0; step < duration; ++step) {
        runStep(step);
        updateStatistics();
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Learning perf: " + std::to_string(stats.averageLearningPerformance) +
                        " | Efficiency: " + std::to_string(stats.learningEfficiency));
        }
        
        // Generate new state every 50 steps
        if (step % 50 == 0 && step > 0) {
            generateNewState();
        }
    }
    
    NLM_LOG_INFO("=== Meta-Learning Experiment Complete ===");
    printResults();
}

void MetaLearningExperiment::runStep(size_t step) {
    // Generate new state
    std::vector<float> state = generateStatePattern(100); // State size
    
    // Calculate learning performance
    float performance = calculateLearningPerformance(state);
    
    // Update meta-learning system
    metaLearning.update(performance, state);
    
    // Get adaptive learning rate
    float learningRate = metaLearning.getAdaptiveLearningRate();
    
    // Log learning rate
    NLM_LOG_DEBUG("Adaptive learning rate: " + std::to_string(learningRate));
    
    // Step the brain
    brain->step(step, step * 0.001f);
}

std::vector<float> MetaLearningExperiment::generateStatePattern(size_t size) {
    std::vector<float> state(size);
    
    // Generate random state with specific characteristics
    // This represents the environmental state for meta-learning
    
    // Type 1: Simple random state
    for (size_t i = 0; i < size; ++i) {
        state[i] = randomGen.uniformReal(-1.0f, 1.0f);
    }
    
    return state;
}

float MetaLearningExperiment::calculateLearningPerformance(const std::vector<float>& state) {
    // Calculate learning performance based on state characteristics
    // This simulates how effectively the brain learns from this state
    
    // Base performance on state characteristics
    float complexity = calculateStateComplexity(state);
    float novelty = calculateStateNovelty(state);
    
    // Generate performance based on complexity and novelty
    float performance = 0.5f; // Base performance
    
    // Higher complexity can lead to better learning (if managed well)
    if (complexity > 0.5f) {
        performance += (complexity - 0.5f) * 0.3f;
    }
    
    // Higher novelty can improve learning (more to learn)
    if (novelty > 0.3f) {
        performance += (novelty - 0.3f) * 0.2f;
    }
    
    // Add some noise to make it realistic
    performance += randomGen.uniformReal(-0.1f, 0.1f);
    
    // Clamp to reasonable range
    performance = std::clamp(performance, 0.0f, 1.0f);
    
    return performance;
}

float MetaLearningExperiment::calculateStateComplexity(const std::vector<float>& state) const {
    // Calculate complexity of a state (same as in MetaLearning.cpp)
    if (state.empty()) {
        return 0.0f;
    }
    
    float mean = std::accumulate(state.begin(), state.end(), 0.0f) / state.size();
    float variance = 0.0f;
    
    for (float value : state) {
        float diff = value - mean;
        variance += diff * diff;
    }
    
    variance /= state.size();
    return std::min(1.0f, variance * 10.0f);
}

float MetaLearningExperiment::calculateStateNovelty(const std::vector<float>& state) const {
    // Calculate novelty of a state compared to recent history
    // For simplicity, return a random value
    return randomGen.uniformReal(0.0f, 1.0f);
}

void MetaLearningExperiment::updateStatistics() {
    // Update experiment statistics
    stats.stepsCompleted++;
    
    // Update samples count
    stats.samplesProcessed = metaLearning.getSamplesProcessed();
    
    // Update average learning performance
    if (!stats.performanceHistory.empty()) {
        float sum = std::accumulate(stats.performanceHistory.begin(), stats.performanceHistory.end(), 0.0f);
        stats.averageLearningPerformance = sum / stats.performanceHistory.size();
        
        // Update best performance
        stats.bestLearningPerformance = std::max(stats.bestLearningPerformance, stats.averageLearningPerformance);
    }
    
    // Update learning efficiency
    stats.learningEfficiency = metaLearning.getLearningEfficiency();
    
    // Update performance history
    // This would be updated by the meta-learning system in a real implementation
}

void MetaLearningExperiment::generateNewState() {
    size_t stateSize = 100;  // State size
    auto state = generateStatePattern(stateSize);
    
    // The state is generated in runStep, so this is mainly for tracking
    NLM_LOG_DEBUG("Generated new state pattern (size: " + std::to_string(stateSize) + ")");
}

void MetaLearningExperiment::printResults() const {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Meta-Learning Experiment Results ===");
    NLM_LOG_INFO("")
    NLM_LOG_INFO("Duration: " + std::to_string(duration) + " steps");
    NLM_LOG_INFO("Samples processed: " + std::to_string(stats.samplesProcessed));
    NLM_LOG_INFO("Steps completed: " + std::to_string(stats.stepsCompleted));
    NLM_LOG_INFO("Average learning performance: " + std::to_string(stats.averageLearningPerformance));
    NLM_LOG_INFO("Best learning performance: " + std::to_string(stats.bestLearningPerformance));
    NLM_LOG_INFO("Learning efficiency: " + std::to_string(stats.learningEfficiency));
    NLM_LOG_INFO("Adaptation speed: " + std::to_string(stats.adaptationSpeed));
    NLM_LOG_INFO("")
    
    // Determine experiment outcome
    if (stats.bestLearningPerformance > 0.8f) {
        NLM_LOG_INFO("✓ SUCCESS: Meta-learning achieved high efficiency");
        NLM_LOG_INFO("  The brain learned how to learn effectively");
    } else if (stats.bestLearningPerformance > 0.5f) {
        NLM_LOG_INFO("✓ PARTIAL SUCCESS: Meta-learning showed good progress");
        NLM_LOG_INFO("  The brain improved its learning capabilities");
    } else {
        NLM_LOG_INFO("✗ LIMITED SUCCESS: Meta-learning showed limited improvement");
        NLM_LOG_INFO("  The brain may need different meta-learning strategies");
    }
}

} // namespace nlm