#include "PredictiveCodingExperiment.hpp"
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

PredictiveCodingExperiment::PredictiveCodingExperiment(std::shared_ptr<Config> config, size_t duration)
    : config(config), duration(duration), randomGen(12345) {
    
    NLM_LOG_INFO("Predictive Coding Experiment initialized");
    NLM_LOG_INFO("Duration: " + std::to_string(duration) + " steps");
    
    // Initialize statistics
    stats = Statistics();
    stats.bestPredictionError = std::numeric_limits<float>::max();
}

PredictiveCodingExperiment::~PredictiveCodingExperiment() = default;

void PredictiveCodingExperiment::initialize() {
    // Create brain with predictive coding config
    if (!config) {
        config = std::make_shared<Config>();
        
        // Configure for predictive coding
        config->set("random_seed", 12345LL, ConfigSource::Default);
        config->set("neuron_count", 2000, ConfigSource::Default);
        config->set("region_count", 2, ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Predictive coding-specific parameters
        config->set("prediction_gain", 0.1f, ConfigSource::Default);
        config->set("hierarchy_levels", 3, ConfigSource::Default);
        config->set("prediction_learning_rate", 0.01f, ConfigSource::Default);
    }
    
    // Initialize brain
    brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for predictive coding experiment");
        return;
    }
    
    // Initialize predictive coding system
    predictiveCoding.initialize(brain.get(),
                               config->getOr<float>("prediction_gain", 0.1f),
                               config->getOr<size_t>("hierarchy_levels", 3),
                               config->getOr<float>("prediction_learning_rate", 0.01f));
    
    startTime = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("Brain initialized for predictive coding experiment");
    NLM_LOG_INFO("Neuron count: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Region count: " + std::to_string(brain->getRegionCount()));
}

void PredictiveCodingExperiment::run() {
    NLM_LOG_INFO("=== Starting Predictive Coding Experiment ===");
    
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
                        " | Prediction error: " + std::to_string(stats.averagePredictionError) +
                        " | Progress: " + std::to_string(stats.learningProgress));
        }
        
        // Generate new state every 50 steps
        if (step % 50 == 0 && step > 0) {
            generateNewState();
        }
    }
    
    NLM_LOG_INFO("=== Predictive Coding Experiment Complete ===");
    printResults();
}

void PredictiveCodingExperiment::runStep(size_t step) {
    // Get current state
    std::vector<float> state = generateStatePattern(100); // State size
    
    // Set state in predictive coding system
    setState(state);
    
    // Compute predictions
    predictiveCoding.computePredictions(state);
    
    // Get prediction error
    float error = predictiveCoding.getPredictionError();
    
    // Update state based on prediction error
    predictiveCoding.updateState(std::vector<float>(100, error)); // Use error as prediction error
    
    // Train forward model
    auto predictions = predictiveCoding.getCurrentPredictions();
    predictiveCoding.trainForwardModel(state, predictions);
    
    // Step the brain
    brain->step(step, step * 0.001f);
}

std::vector<float> PredictiveCodingExperiment::generateStatePattern(size_t size) {
    std::vector<float> state(size);
    
    // Generate random state with specific characteristics
    // This represents the brain's internal state
    
    // Type 1: Simple random state
    for (size_t i = 0; i < size; ++i) {
        state[i] = randomGen.uniformReal(-1.0f, 1.0f);
    }
    
    return state;
}

void PredictiveCodingExperiment::setState(const std::vector<float>& state) {
    // Set state in predictive coding system
    predictiveCoding.setSensoryInput(state);
    
    // Store state in history
    stateHistory.push_back(state);
    
    NLM_LOG_DEBUG("Set state pattern (size: " + std::to_string(state.size()) + ")");
}

std::vector<float> PredictiveCodingExperiment::getCurrentPredictions() const {
    return predictiveCoding.getCurrentPredictions();
}

void PredictiveCodingExperiment::updateStatistics() {
    // Update experiment statistics
    stats.stepsCompleted++;
    
    // Update samples count
    stats.samplesProcessed = predictiveCoding.getTrainingSamplesProcessed();
    
    // Update average prediction error
    if (!stats.errorHistory.empty()) {
        float sum = std::accumulate(stats.errorHistory.begin(), stats.errorHistory.end(), 0.0f);
        stats.averagePredictionError = sum / stats.errorHistory.size();
        
        // Update best error
        stats.bestPredictionError = std::min(stats.bestPredictionError, stats.averagePredictionError);
    }
    
    // Update learning progress
    stats.learningProgress = predictiveCoding.getLearningProgress();
    
    // Update error history
    if (!predictiveCoding.getErrorHistory().empty()) {
        stats.errorHistory = predictiveCoding.getErrorHistory();
    }
    
    // Update prediction history
    if (!predictiveCoding.getPredictionHistory().empty()) {
        for (const auto& pred : predictiveCoding.getPredictionHistory()) {
            stats.predictionHistory.push_back(pred);
        }
    }
}

void PredictiveCodingExperiment::generateNewState() {
    size_t stateSize = 100;  // State size
    auto state = generateStatePattern(stateSize);
    
    // Set state in predictive coding system
    setState(state);
    
    NLM_LOG_DEBUG("Generated new state pattern (size: " + std::to_string(stateSize) + ")");
}

void PredictiveCodingExperiment::printResults() const {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Predictive Coding Experiment Results ===");
    NLM_LOG_INFO("")
    NLM_LOG_INFO("Duration: " + std::to_string(duration) + " steps");
    NLM_LOG_INFO("Samples processed: " + std::to_string(stats.samplesProcessed));
    NLM_LOG_INFO("Steps completed: " + std::to_string(stats.stepsCompleted));
    NLM_LOG_INFO("Average prediction error: " + std::to_string(stats.averagePredictionError));
    NLM_LOG_INFO("Best prediction error achieved: " + std::to_string(stats.bestPredictionError));
    NLM_LOG_INFO("Learning progress: " + std::to_string(stats.learningProgress));
    NLM_LOG_INFO("")
    
    // Determine experiment outcome
    if (stats.bestPredictionError < 0.01f) {
        NLM_LOG_INFO("✓ SUCCESS: Predictive coding achieved high accuracy");
        NLM_LOG_INFO("  The brain successfully generated accurate predictions");
    } else if (stats.bestPredictionError < 0.1f) {
        NLM_LOG_INFO("✓ PARTIAL SUCCESS: Predictive coding showed moderate improvement");
        NLM_LOG_INFO("  The brain made progress in prediction generation");
    } else {
        NLM_LOG_INFO("✗ LIMITED SUCCESS: Predictive coding showed limited improvement");
        NLM_LOG_INFO("  The brain may need more training or different parameters");
    }
}

} // namespace nlm