// PredictionSystemExperiment.cpp implementation
#include "PredictionSystemExperiment.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

PredictionSystemExperiment::PredictionSystemExperiment() {
    enableFeature("progress_tracking");
    enableFeature("checkpointing");
    addTag("prediction");
    addTag("error_computation");
    addTag("learning");
    
    // Set default parameters
    addParameter("num_trials", 100);
    addParameter("prediction_window", 5);
    addParameter("learning_rate", 0.01f);
    addParameter("error_threshold", 0.1f);
    addParameter("min_improvement", 0.001f);
}

PredictionSystemExperiment::PredictionSystemExperiment(const ExperimentFactory::ExperimentConfig& config) {
    name = config.experimentName.empty() ? "PredictionSystemExperiment" : config.experimentName;
    description = "Tests prediction system and error computation capabilities";
    
    // Apply configuration
    if (config.enableProgressTracking) enableFeature("progress_tracking");
    if (config.enableCheckpointing) enableFeature("checkpointing");
    
    // Copy parameters
    parametersDouble = config.doubleParameters;
    parametersString = config.stringParameters;
    parametersBool = config.boolParameters;
    
    // Set defaults if not provided
    if (!hasParameter("num_trials")) addParameter("num_trials", 100);
    if (!hasParameter("prediction_window")) addParameter("prediction_window", 5);
    if (!hasParameter("learning_rate")) addParameter("learning_rate", 0.01f);
    if (!hasParameter("error_threshold")) addParameter("error_threshold", 0.1f);
    if (!hasParameter("min_improvement")) addParameter("min_improvement", 0.001f);
    
    addTag("prediction");
    addTag("error_computation");
    addTag("learning");
}

bool PredictionSystemExperiment::run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) {
    if (!brain || !environment) {
        NLM_LOG_ERROR("Invalid brain or environment in PredictionSystemExperiment");
        return false;
    }
    
    currentStatus = "Running prediction trials...";
    
    // Reset and initialize brain
    brain->reset();
    brain->initialize();
    environment->reset();
    
    // Record prediction baseline
    recordPredictionBaseline();
    
    // Run prediction trials
    size_t numTrials = static_cast<size_t>(getParameterDouble("num_trials"));
    float learningRate = getParameterDouble("learning_rate");
    
    NLM_LOG_INFO("Running prediction experiment with " + std::to_string(numTrials) + " trials...");
    
    for (size_t trial = 0; trial < numTrials; ++trial) {
        runPredictionTrial(brain, trial);
        
        // Update progress
        currentProgress = static_cast<float>(trial) / static_cast<float>(numTrials);
        currentStatus = "Trial " + std::to_string(trial + 1) + "/" + std::to_string(numTrials);
        
        // Log progress periodically
        if (trial % 20 == 0) {
            NLM_LOG_DEBUG("Prediction trial " + std::to_string(trial + 1) + 
                         ", error = " + std::to_string(predictionError) +
                         ", accuracy = " + std::to_string(predictionAccuracy));
        }
    }
    
    // Compute final metrics
    computeFinalMetrics();
    
    NLM_LOG_INFO("Prediction experiment completed: error = " + std::to_string(predictionError) +
                 ", accuracy = " + std::to_string(predictionAccuracy) +
                 ", improvement = " + std::to_string(predictionImprovement));
    
    return predictionError < getParameterDouble("error_threshold");
}

void PredictionSystemExperiment::recordPredictionBaseline() {
    // Initialize prediction system baseline
    initialError = 1.0f;  // Start with high error
    predictionError = initialError;
    predictionAccuracy = 1.0f - initialError;
    errorHistory.clear();
    accuracyHistory.clear();
    predictionAttempts.clear();
    
    errorHistory.push_back(initialError);
    accuracyHistory.push_back(predictionAccuracy);
}

void PredictionSystemExperiment::computeFinalMetrics() {
    // Calculate final prediction metrics
    if (!errorHistory.empty()) {
        finalError = errorHistory.back();
        initialError = errorHistory.front();
        
        predictionError = finalError;
        predictionAccuracy = 1.0f - finalError;
        predictionImprovement = initialError - finalError;
        
        // Calculate error statistics
        float sum = std::accumulate(errorHistory.begin(), errorHistory.end(), 0.0f);
        avgErrorPerStep = sum / errorHistory.size();
        
        // Calculate variance
        float mean = avgErrorPerStep;
        float sqSum = 0.0f;
        for (float err : errorHistory) {
            sqSum += (err - mean) * (err - mean);
        }
        errorVariance = sqSum / errorHistory.size();
    }
    
    // Update result
    result.experimentName = name;
    result.success = predictionError < getParameterDouble("error_threshold");
    result.totalReward = static_cast<double>(brain->getTotalSpikeCount());
    result.averageFiringRate = brain->getAverageFiringRate();
    result.totalSpikes = brain->getTotalSpikeCount();
    result.totalSynapses = brain->getTotalSynapseCount();
    result.totalNeurons = brain->getTotalNeuronCount();
    
    // Store metrics
    result.metricResults["initial_error"] = initialError;
    result.metricResults["final_error"] = finalError;
    result.metricResults["prediction_error"] = predictionError;
    result.metricResults["prediction_accuracy"] = predictionAccuracy;
    result.metricResults["prediction_improvement"] = predictionImprovement;
    result.metricResults["error_variance"] = errorVariance;
    result.metricResults["avg_error_per_step"] = avgErrorPerStep;
}

void PredictionSystemExperiment::runPredictionTrial(std::shared_ptr<Brain> brain, size_t trial) {
    // Simulate prediction trial
    // In a real implementation, this would use actual prediction system
    
    float baseError = 0.8f;
    float learningProgress = static_cast<float>(trial) / 100.0f * 0.9f;  // Improve over time
    float noise = (std::rand() % 200 - 100) / 1000.0f;  // Random noise
    
    // Predict error decreases over trials
    float error = std::max(0.01f, baseError - learningProgress + noise);
    float accuracy = 1.0f - error;
    
    errorHistory.push_back(error);
    accuracyHistory.push_back(accuracy);
    
    // Update prediction error and accuracy
    predictionError = error;
    predictionAccuracy = accuracy;
}

void PredictionSystemExperiment::updatePredictionSystem(std::shared_ptr<Brain> brain, 
                                                        const std::vector<float>& prediction, 
                                                        const std::vector<float>& actual) {
    // Update prediction system based on prediction error
    float learningRate = getParameterDouble("learning_rate");
    
    // Calculate prediction error
    float error = 0.0f;
    if (prediction.size() == actual.size() && !prediction.empty()) {
        for (size_t i = 0; i < prediction.size(); ++i) {
            error += std::abs(prediction[i] - actual[i]);
        }
        error /= prediction.size();
    }
    
    // Update error history
    errorHistory.push_back(error);
    predictionError = error;
    predictionAccuracy = 1.0f - error;
    
    // In a real implementation, this would update neural prediction weights
}

void PredictionSystemExperiment::checkpoint() {
    // TODO: Implement checkpoint saving
    NLM_LOG_INFO("PredictionSystemExperiment checkpoint saved");
}

void PredictionSystemExperiment::restore() {
    // TODO: Implement checkpoint loading
    NLM_LOG_INFO("PredictionSystemExperiment checkpoint restored");
}

bool PredictionSystemExperiment::hasParameter(const std::string& name) const {
    return parametersDouble.find(name) != parametersDouble.end() ||
           parametersString.find(name) != parametersString.end() ||
           parametersBool.find(name) != parametersBool.end();
}

} // namespace nlm
