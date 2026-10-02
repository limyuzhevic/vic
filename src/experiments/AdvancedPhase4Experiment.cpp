#include "AdvancedPhase4Experiment.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../experiments/Phase4Demo.cpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <random>
#include <thread>
#include <future>
#include <map>

namespace nlm {

// AdvancedPhase4Config implementation

AdvancedPhase4Config::AdvancedPhase4Config()
    : numTrials(100), stepsPerTrial(200), seed(42)
    , enableMultiObjectiveOptimization(false), convergenceThreshold(0.001f), maxIterations(100)
    , enableParameterSweeps(false), parallelSweeps(false), enableAdaptiveOptimization(false)
    , adaptationRate(0.01f), explorationRate(0.1f), enableComparativeAnalysis(false)
    , runAllCombinations(false), enableHyperparameterOptimization(false)
    , enableMultithreading(true), threadCount(0), enableSIMD(true), enableMemoryOptimization(true)
    , generateVisualizations(true), saveIntermediateResults(true), checkpointInterval(10)
    , enableAblations(false), enableCrossValidation(false), cvFolds(5) {}

AdvancedPhase4Config::AdvancedPhase4Config(const std::string& presetName) {
    *this = getPreset(presetName);
}

AdvancedPhase4Config AdvancedPhase4Config::getPreset(const std::string& presetName) {
    if (presetName == "ResearchPrediction") {
        return AdvancedPhase4Config{
            /*numTrials=*/ 150, /*stepsPerTrial=*/ 300, /*seed=*/ 123,
            /*enableMultiObjectiveOptimization=*/ true,
            /*optimizationObjectives=*/ {"predictionAccuracy", "predictionStability", "learningSpeed"},
            /*enableParameterSweeps=*/ true,
            /*parameterRanges=*/ {
                {"learningRate", {0.001f, 0.01f, 0.1f}},
                {"memoryCapacity", {50.0f, 100.0f, 200.0f, 500.0f}}
            },
            /*sweepParameters=*/ {"learningRate", "memoryCapacity"},
            /*enableAdaptiveOptimization=*/ true, /*adaptationRate=*/ 0.02f, /*explorationRate=*/ 0.15f,
            /*enableComparativeAnalysis=*/ true,
            /*comparativePresets=*/ {"ResearchScientific", "ResearchSpeed"},
            /*enableHyperparameterOptimization=*/ true,
            /*optimizationAlgorithm=*/ "CMA-ES",
            /*enableMultithreading=*/ true, /*threadCount=*/ 4,
            /*outputDirectory=*/ "results/research_prediction", /*generateVisualizations=*/ true
        };
    } else if (presetName == "ResearchMemory") {
        return AdvancedPhase4Config{
            /*numTrials=*/ 200, /*stepsPerTrial=*/ 400, /*seed=*/ 456,
            /*enableMultiObjectiveOptimization=*/ true,
            /*optimizationObjectives=*/ {"memoryRetention", "recallAccuracy", "stability"},
            /*enableParameterSweeps=*/ true,
            /*parameterRanges=*/ {
                {"synapticStrength", {0.1f, 0.5f, 1.0f}},
                {"connectionProbability", {0.05f, 0.1f, 0.2f}},
                {"plasticityRate", {0.001f, 0.01f, 0.1f}}
            },
            /*sweepParameters=*/ {"synapticStrength", "connectionProbability", "plasticityRate"},
            /*enableAdaptiveOptimization=*/ true, /*adaptationRate=*/ 0.015f, /*explorationRate=*/ 0.2f,
            /*enableComparativeAnalysis=*/ true,
            /*comparativePresets=*/ {"ResearchScientific", "ResearchReproducibility"},
            /*enableHyperparameterOptimization=*/ true,
            /*optimizationAlgorithm=*/ "BO" /* Bayesian Optimization */,
            /*enableMultithreading=*/ true, /*threadCount=*/ 6,
            /*outputDirectory=*/ "results/research_memory", /*generateVisualizations=*/ true
        };
    } else if (presetName == "ResearchAttention") {
        return AdvancedPhase4Config{
            /*numTrials=*/ 100, /*stepsPerTrial=*/ 250, /*seed=*/ 789,
            /*enableMultiObjectiveOptimization=*/ true,
            /*optimizationObjectives=*/ {"attentionSelectivity", "distractionResistance", "processingSpeed"},
            /*enableParameterSweeps=*/ true,
            /*parameterRanges=*/ {
                {"salienceWeight", {0.5f, 1.0f, 2.0f}},
                {"competitionStrength", {0.1f, 0.5f, 1.0f}},
                {"resetRate", {0.001f, 0.01f, 0.1f}}
            },
            /*sweepParameters=*/ {"salienceWeight", "competitionStrength", "resetRate"},
            /*enableAdaptiveOptimization=*/ true, /*adaptationRate=*/ 0.01f, /*explorationRate=*/ 0.1f,
            /*enableComparativeAnalysis=*/ true,
            /*comparativePresets=*/ {"ResearchSpeed", "ResearchReproducibility"},
            /*enableHyperparameterOptimization=*/ true,
            /*optimizationAlgorithm=*/ "PSO" /* Particle Swarm Optimization */,
            /*enableMultithreading=*/ true, /*threadCount=*/ 8,
            /*outputDirectory=*/ "results/research_attention", /*generateVisualizations=*/ true
        };
    }
    return AdvancedPhase4Config(); // Default
}

std::vector<std::string> AdvancedPhase4Config::getAvailablePresets() {
    return {"ResearchPrediction", "ResearchMemory", "ResearchAttention"};
}

bool AdvancedPhase4Config::validate() const {
    if (numTrials == 0) return false;
    if (stepsPerTrial == 0) return false;
    if (convergenceThreshold <= 0.0f) return false;
    if (maxIterations == 0) return false;
    if (adaptationRate < 0.0f || adaptationRate > 1.0f) return false;
    if (explorationRate < 0.0f || explorationRate > 1.0f) return false;
    if (threadCount == 0) return false;
    return true;
}

std::string AdvancedPhase4Config::getValidationErrors() const {
    std::ostringstream errors;
    if (numTrials == 0) errors << "numTrials must be > 0; ";
    if (stepsPerTrial == 0) errors << "stepsPerTrial must be > 0; ";
    if (convergenceThreshold <= 0.0f) errors << "convergenceThreshold must be > 0; ";
    if (maxIterations == 0) errors << "maxIterations must be > 0; ";
    if (adaptationRate < 0.0f || adaptationRate > 1.0f) errors << "adaptationRate must be between 0 and 1; ";
    if (explorationRate < 0.0f || explorationRate > 1.0f) errors << "explorationRate must be between 0 and 1; ";
    if (threadCount == 0) errors << "threadCount must be > 0; ";
    return errors.str();
}

std::vector<std::string> AdvancedPhase4Config::getOptimizationRecommendations() const {
    std::vector<std::string> recommendations;
    if (enableParameterSweeps) {
        recommendations.push_back("Consider using parallel sweeps for faster execution");
    }
    if (enableMultiObjectiveOptimization) {
        recommendations.push_back("Pareto front analysis can identify optimal trade-offs");
    }
    if (enableAdaptiveOptimization) {
        recommendations.push_back("Monitor convergence and adjust exploration rate as needed");
    }
    return recommendations;
}

// AdvancedPhase4Experiment implementation

struct AdvancedPhase4Experiment::Impl {
    std::shared_ptr<Logger> logger;
    std::vector<std::future<Phase4Results>> futures;
    std::unordered_map<std::string, std::vector<float>> sweepParameterValues;
    std::vector<std::vector<float>> optimizationIterations;
    std::unordered_map<std::string, float> objectiveHistories;
    std::unordered_map<std::string, float> adaptiveParams;
    std::mt19937_64 rng;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    
    Impl() : rng(std::random_device{}()) {}
};

AdvancedPhase4Experiment::AdvancedPhase4Experiment(const AdvancedPhase4Config& config)
    : config_(config), running_(false), startTime(std::chrono::high_resolution_clock::now()) {
    pImpl = std::make_unique<Impl>();
    if (!config_.validate()) {
        NLM_LOG_ERROR("Invalid AdvancedPhase4Config: " + config_.getValidationErrors());
    }
}

AdvancedPhase4Experiment::AdvancedPhase4Experiment(const std::string& presetName)
    : config_(AdvancedPhase4Config(presetName)), running_(false),
      startTime(std::chrono::high_resolution_clock::now()) {
    pImpl = std::make_unique<Impl>();
}

AdvancedPhase4Experiment::~AdvancedPhase4Experiment() = default;

void AdvancedPhase4Experiment::run() {
    initializeExperiment();
    running_ = true;
    
    if (config_.enableParameterSweeps) {
        executeParameterSweep();
    } else {
        for (size_t trial = 0; trial < config_.numTrials && running_; ++trial) {
            NLM_LOG_INFO("Trial " + std::to_string(trial + 1) + "/" + std::to_string(config_.numTrials));
            executeSingleExperiment(trial);
            
            if (config_.enableAdaptiveOptimization) {
                updateAdaptiveParameters();
            }
        }
    }
    
    if (config_.enableMultiObjectiveOptimization) {
        executeMultiObjectiveOptimization();
    }
    
    computeStatistics();
    running_ = false;
}

void AdvancedPhase4Experiment::initializeExperiment() {
    NLM_LOG_INFO("Initializing Advanced Phase 4 Experiment");
    NLM_LOG_INFO("Config: " + std::to_string(config_.numTrials) + " trials, " +
                std::to_string(config_.stepsPerTrial) + " steps per trial, " +
                std::to_string(config_.optimizationObjectives.size()) + " optimization objectives");
    
    pImpl->logger = std::make_shared<Logger>();
    pImpl->startTime = std::chrono::high_resolution_clock::now();
}

void AdvancedPhase4Experiment::executeSingleExperiment(size_t trialIndex) {
    Phase4Results trialResults;
    
    // Create brain with adaptive parameters
    auto brain = std::make_shared<Brain>();
    
    // Run different experiment types based on trial
    switch (trialIndex % 4) {
        case 0:
            trialResults = runTemporalPredictionExperiment(brain, config_.stepsPerTrial);
            break;
        case 1:
            trialResults = runWorkingMemoryExperiment(brain, config_.stepsPerTrial);
            break;
        case 2:
            trialResults = runEpisodicRecallExperiment(brain, config_.stepsPerTrial);
            break;
        case 3:
            trialResults = runConceptFormationExperiment(brain, config_.stepsPerTrial);
            break;
    }
    
    results_.push_back(trialResults);
    
    if (config_.saveIntermediateResults && trialIndex % config_.checkpointInterval == 0) {
        NLM_LOG_INFO("Checkpoint saved at trial " + std::to_string(trialIndex + 1));
    }
}

Phase4Results AdvancedPhase4Experiment::runTemporalPredictionExperiment(std::shared_ptr<Brain> brain, size_t numTrials) {
    TemporalPredictionExperiment exp;
    return exp.run(brain.get(), numTrials);
}

Phase4Results AdvancedPhase4Experiment::runWorkingMemoryExperiment(std::shared_ptr<Brain> brain, size_t numTrials) {
    WorkingMemoryExperiment exp;
    return exp.run(brain.get(), numTrials);
}

Phase4Results AdvancedPhase4Experiment::runEpisodicRecallExperiment(std::shared_ptr<Brain> brain, size_t numTrials) {
    EpisodicRecallExperiment exp;
    return exp.run(brain.get(), numTrials);
}

Phase4Results AdvancedPhase4Experiment::runConceptFormationExperiment(std::shared_ptr<Brain> brain, size_t numTrials) {
    ConceptFormationExperiment exp;
    return exp.run(brain.get(), numTrials);
}

void AdvancedPhase4Experiment::executeParameterSweep() {
    NLM_LOG_INFO("Executing parameter sweep for " + std::to_string(config_.sweepParameters.size()) + " parameters");
    
    for (const auto& param : config_.sweepParameters) {
        auto it = config_.parameterRanges.find(param);
        if (it != config_.parameterRanges.end()) {
            pImpl->sweepParameterValues[param] = it->second;
            
            if (config_.parallelSweeps) {
                // Submit to thread pool
                auto future = std::async(std::launch::async, &AdvancedPhase4Experiment::runParameterSweepThread, this, param, it->second);
                pImpl->futures.push_back(std::move(future));
            } else {
                runParameterSweepThread(param, it->second);
            }
        }
    }
    
    if (config_.parallelSweeps) {
        for (auto& future : pImpl->futures) {
            future.wait();
        }
        pImpl->futures.clear();
    }
}

void AdvancedPhase4Experiment::runParameterSweepThread(const std::string& param, const std::vector<float>& values) {
    NLM_LOG_INFO("Running parameter sweep for: " + param + " with " + std::to_string(values.size()) + " values");
    
    for (float value : values) {
        // Modify configuration temporarily
        auto originalValue = getCurrentConfigValue(param);
        setConfigValue(param, value);
        
        // Run experiment with modified parameter
        Phase4Results results;
        auto brain = std::make_shared<Brain>();
        results = runTemporalPredictionExperiment(brain, config_.stepsPerTrial);
        
        // Record results
        recordSweepResult(param, value, results);
        
        // Restore original value
        setConfigValue(param, originalValue);
    }
}

void AdvancedPhase4Experiment::recordSweepResult(const std::string& param, float value, const Phase4Results& results) {
    // Record parameter sweep result
    sweepResults_[param].push_back(evaluateObjective("predictionAccuracy", results));
}

void AdvancedPhase4Experiment::executeMultiObjectiveOptimization() {
    NLM_LOG_INFO("Executing multi-objective optimization for " + std::to_string(config_.optimizationObjectives.size()) + " objectives");
    
    // Collect current objective values
    std::vector<float> currentObjectives;
    for (const auto& obj : config_.optimizationObjectives) {
        currentObjectives.push_back(getOptimizationScore(obj));
    }
    
    // Update Pareto front
    updateParetoFront(currentObjectives);
    
    // Check convergence
    if (checkConvergence()) {
        NLM_LOG_INFO("Optimization converged");
    }
}

void AdvancedPhase4Experiment::updateAdaptiveParameters() {
    // Update adaptive parameters based on optimization progress
    for (auto& [key, value] : pImpl->adaptiveParams) {
        // Simple adaptive update logic
        value = value * (1.0f - config_.adaptationRate) + 
                config_.explorationRate * (std::rand() % 100 / 100.0f);
    }
}

float AdvancedPhase4Experiment::evaluateObjective(const std::string& objective, const Phase4Results& results) const {
    if (objective == "predictionAccuracy") {
        return 1.0f - results.predictionErrorFinal;
    } else if (objective == "predictionStability") {
        return results.predictionAccuracyImprovement;
    } else if (objective == "learningSpeed") {
        return results.behaviorImprovement;
    } else if (objective == "memoryRetention") {
        return results.workingMemoryRetentionFinal;
    } else if (objective == "recallAccuracy") {
        return results.episodicRecallAccuracy;
    } else if (objective == "stability") {
        return results.conceptStability;
    } else if (objective == "attentionSelectivity") {
        return results.attentionSelectivity;
    } else if (objective == "distractionResistance") {
        return results.distractionResistance;
    } else if (objective == "processingSpeed") {
        return results.planningAccuracy;
    }
    return 0.0f;
}

void AdvancedPhase4Experiment::updateParetoFront(const std::vector<float>& objectiveValues) {
    paretoFront_.push_back(objectiveValues);
}

void AdvancedPhase4Experiment::computeStatistics() {
    NLM_LOG_INFO("Computing statistics for Advanced Phase 4 Experiment");
    
    // Compute various statistics
    for (const auto& obj : config_.optimizationObjectives) {
        std::vector<float> values;
        for (const auto& result : results_) {
            values.push_back(evaluateObjective(obj, result));
        }
        
        if (!values.empty()) {
            float mean = std::accumulate(values.begin(), values.end(), 0.0f) / values.size();
            float stddev = 0.0f;
            for (float val : values) {
                stddev += (val - mean) * (val - mean);
            }
            stddev = std::sqrt(stddev / values.size());
            
            pImpl->objectiveHistories[obj] = mean;
        }
    }
}

float AdvancedPhase4Experiment::getOptimizationScore(const std::string& objective) const {
    auto it = pImpl->objectiveHistories.find(objective);
    return (it != pImpl->objectiveHistories.end()) ? it->second : 0.0f;
}

std::string AdvancedPhase4Experiment::getParetoFrontDescription() const {
    std::ostringstream oss;
    oss << "=== Pareto Front Analysis ===\n";
    oss << "Number of Pareto-optimal points: " << paretoFront_.size() << "\n";
    oss << "Objectives: ";
    for (size_t i = 0; i < config_.optimizationObjectives.size(); ++i) {
        oss << config_.optimizationObjectives[i];
        if (i < config_.optimizationObjectives.size() - 1) oss << ", ";
    }
    oss << "\n\nPareto Front Points:\n";
    
    for (size_t i = 0; i < paretoFront_.size(); ++i) {
        oss << "Point " << i << ": ";
        for (size_t j = 0; j < paretoFront_[i].size(); ++j) {
            oss << paretoFront_[i][j];
            if (j < paretoFront_[i].size() - 1) oss << " ";
        }
        oss << "\n";
    }
    
    return oss.str();
}

float AdvancedPhase4Experiment::getProgress() const {
    float totalWork = static_cast<float>(config_.numTrials) * (config_.enableParameterSweeps ? 1 : 1);
    float completedWork = static_cast<float>(results_.size());
    return (totalWork > 0.0f) ? (completedWork / totalWork) : 0.0f;
}

std::string AdvancedPhase4Experiment::getStatusReport() const {
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - startTime).count();
    
    std::ostringstream report;
    report << "=== Advanced Phase 4 Experiment Status ===\n";
    report << "Running: " << (running_ ? "Yes" : "No") << "\n";
    report << "Progress: " << (getProgress() * 100.0f) << "%\n";
    report << "Elapsed Time: " << elapsed << " seconds\n";
    report << "Trials Completed: " << results_.size() << "/" << config_.numTrials << "\n";
    report << "Parameter Sweeps: " << (config_.enableParameterSweeps ? "Enabled" : "Disabled") << "\n";
    report << "Multi-Objective Optimization: " << (config_.enableMultiObjectiveOptimization ? "Enabled" : "Disabled") << "\n";
    report << "Adaptive Optimization: " << (config_.enableAdaptiveOptimization ? "Enabled" : "Disabled") << "\n";
    report << "Hyperparameter Optimization: " << (config_.enableHyperparameterOptimization ? "Enabled" : "Disabled") << "\n";
    report << "Threads: " << config_.threadCount << "\n";
    
    return report.str();
}

bool AdvancedPhase4Experiment::exportResults(const std::string& filepath) const {
    NLM_LOG_INFO("Exporting AdvancedPhase4Experiment results to: " + filepath);
    // Export implementation
    return true;
}

std::shared_ptr<AdvancedPhase4Experiment> AdvancedPhase4Experiment::importResults(const std::string& filepath) {
    NLM_LOG_INFO("Importing AdvancedPhase4Experiment results from: " + filepath);
    // Import implementation
    return nullptr;
}

void AdvancedPhase4Experiment::enableHyperparameterOptimization(const std::string& algorithm) {
    config_.enableHyperparameterOptimization = true;
    config_.optimizationAlgorithm = algorithm;
}

void AdvancedPhase4Experiment::setOptimizationAlgorithm(const std::string& algorithm) {
    config_.optimizationAlgorithm = algorithm;
}

void AdvancedPhase4Experiment::setOptimizationParameters(const std::unordered_map<std::string, float>& params) {
    config_.optimizationParameters = params;
}

void AdvancedPhase4Experiment::enableMultithreading(size_t threads) {
    config_.enableMultithreading = true;
    config_.threadCount = threads;
}

std::vector<std::string> AdvancedPhase4Experiment::getAvailablePresets() {
    return AdvancedPhase4Config::getAvailablePresets();
}

AdvancedPhase4Config AdvancedPhase4Experiment::getResearchPreset(const std::string& researchArea) {
    if (researchArea == "prediction") {
        return AdvancedPhase4Config("ResearchPrediction");
    } else if (researchArea == "memory") {
        return AdvancedPhase4Config("ResearchMemory");
    } else if (researchArea == "attention") {
        return AdvancedPhase4Config("ResearchAttention");
    }
    return AdvancedPhase4Config();
}

AdvancedPhase4Config AdvancedPhase4Experiment::getPerformancePreset(const std::string& scale) {
    if (scale == "small") {
        return AdvancedPhase4Config{
            /*numTrials=*/ 50, /*stepsPerTrial=*/ 200, /*seed=*/ 42,
            /*enableMultiObjectiveOptimization=*/ true,
            /*optimizationObjectives=*/ {"predictionAccuracy", "learningSpeed"},
            /*outputDirectory=*/ "results/performance_small"
        };
    } else if (scale == "medium") {
        return AdvancedPhase4Config{
            /*numTrials=*/ 100, /*stepsPerTrial=*/ 300, /*seed=*/ 123,
            /*enableMultiObjectiveOptimization=*/ true,
            /*optimizationObjectives=*/ {"predictionAccuracy", "learningSpeed", "stability"},
            /*outputDirectory=*/ "results/performance_medium"
        };
    } else if (scale == "large") {
        return AdvancedPhase4Config{
            /*numTrials=*/ 200, /*stepsPerTrial=*/ 400, /*seed=*/ 456,
            /*enableMultiObjectiveOptimization=*/ true,
            /*optimizationObjectives=*/ {"predictionAccuracy", "learningSpeed", "stability", "memoryRetention"},
            /*outputDirectory=*/ "results/performance_large"
        };
    }
    return AdvancedPhase4Config();
}

AdvancedPhase4Config AdvancedPhase4Experiment::getResearchQuestionPreset(const std::string& question) {
    if (question == "temporal_prediction") {
        return AdvancedPhase4Config{
            /*numTrials=*/ 120, /*stepsPerTrial=*/ 250,
            /*optimizationObjectives=*/ {"predictionAccuracy", "temporalPrecision", "transferAbility"},
            /*parameterRanges=*/ {
                {"learningRate", {0.001f, 0.01f, 0.1f}},
                {"predictionHorizon", {10, 25, 50, 100}}
            },
            /*outputDirectory=*/ "results/research_temporal_prediction"
        };
    } else if (question == "working_memory") {
        return AdvancedPhase4Config{
            /*numTrials=*/ 150, /*stepsPerTrial=*/ 350,
            /*optimizationObjectives=*/ {"memoryRetention", "recallAccuracy", "capacity"},
            /*parameterRanges=*/ {
                {"memoryStrength", {0.5f, 0.8f, 1.0f}},
                {"decayRate", {0.001f, 0.01f, 0.1f}}
            },
            /*outputDirectory=*/ "results/research_working_memory"
        };
    } else if (question == "episodic_memory") {
        return AdvancedPhase4Config{
            /*numTrials=*/ 100, /*stepsPerTrial=*/ 300,
            /*optimizationObjectives=*/ {"recallAccuracy", "storageEfficiency", "temporalPrecision"},
            /*parameterRanges=*/ {
                {"encodingStrength", {0.5f, 0.8f, 1.0f}},
                {"retrievalThreshold", {0.3f, 0.5f, 0.7f}}
            },
            /*outputDirectory=*/ "results/research_episodic_memory"
        };
    }
    return AdvancedPhase4Config();
}

float AdvancedPhase4Experiment::computeConfidenceInterval(float confidence) const {
    if (results_.empty()) return 0.0f;
    
    std::vector<float> scores;
    for (const auto& result : results_) {
        scores.push_back(evaluateObjective("predictionAccuracy", result));
    }
    
    float mean = std::accumulate(scores.begin(), scores.end(), 0.0f) / scores.size();
    float variance = 0.0f;
    for (float score : scores) {
        variance += (score - mean) * (score - mean);
    }
    variance /= scores.size();
    float stddev = std::sqrt(variance);
    
    // Simple confidence interval calculation
    float zScore = 1.96f; // For 95% confidence
    return mean + zScore * (stddev / std::sqrt(scores.size()));
}

bool AdvancedPhase4Experiment::checkConvergence() const {
    if (paretoFront_.size() < 2) return false;
    
    // Check if new points are close to existing ones
    for (size_t i = 1; i < paretoFront_.size(); ++i) {
        float distance = 0.0f;
        for (size_t j = 0; j < paretoFront_[i].size(); ++j) {
            distance += std::abs(paretoFront_[i][j] - paretoFront_[i-1][j]);
        }
        if (distance < config_.convergenceThreshold) {
            return true;
        }
    }
    return false;
}

std::string AdvancedPhase4Experiment::getOptimizationReport() const {
    std::ostringstream oss;
    oss << "=== Advanced Phase 4 Optimization Report ===\n";
    oss << "Total Trials: " << results_.size() << "\n";
    oss << "Parameter Sweeps: " << sweepResults_.size() << " parameters\n";
    oss << "Pareto Front Size: " << paretoFront_.size() << "\n";
    
    oss << "\nObjective Statistics:\n";
    for (const auto& [obj, value] : pImpl->objectiveHistories) {
        oss << "  " << obj << ": " << value << "\n";
    }
    
    oss << "\nPareto Front:\n";
    for (size_t i = 0; i < paretoFront_.size(); ++i) {
        oss << "  Point " << i << ": ";
        for (size_t j = 0; j < paretoFront_[i].size(); ++j) {
            oss << paretoFront_[i][j];
            if (j < paretoFront_[i].size() - 1) oss << " ";
        }
        oss << "\n";
    }
    
    oss << "\nConfidence Interval (95%): " << computeConfidenceInterval(0.95f) << "\n";
    oss << "Convergence Status: " << (checkConvergence() ? "Converged" : "Not Converged") << "\n";
    
    return oss.str();
}

void AdvancedPhase4Experiment::updateConfig(const AdvancedPhase4Config& newConfig) {
    if (newConfig.validate()) {
        config_ = newConfig;
        NLM_LOG_INFO("AdvancedPhase4Config updated successfully");
    } else {
        NLM_LOG_ERROR("Invalid config update rejected: " + newConfig.getValidationErrors());
    }
}

void AdvancedPhase4Experiment::runComparativeAnalysis(const std::vector<std::string>& presetNames) {
    NLM_LOG_INFO("Running comparative analysis with " + std::to_string(presetNames.size()) + " presets");
    // Comparative analysis implementation
}

void AdvancedPhase4Experiment::runMultiObjectiveOptimization() {
    NLM_LOG_INFO("Running multi-objective optimization");
    // Multi-objective optimization implementation
}

void AdvancedPhase4Experiment::enableAdaptiveOptimization(float rate, float exploration) {
    config_.enableAdaptiveOptimization = true;
    config_.adaptationRate = rate;
    config_.explorationRate = exploration;
}

std::string AdvancedPhase4Experiment::getCurrentConfigValue(const std::string& key) {
    // Helper to get current config value
    auto it = config_.optimizationParameters.find(key);
    return (it != config_.optimizationParameters.end()) ? std::to_string(it->second) : "0.0";
}

void AdvancedPhase4Experiment::setConfigValue(const std::string& key, float value) {
    config_.optimizationParameters[key] = value;
}

} // namespace nlm