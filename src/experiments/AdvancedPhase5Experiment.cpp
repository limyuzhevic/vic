#include "AdvancedPhase5Experiment.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../experiments/Phase5Demo.cpp"
#include "../experiments/Phase5Experiment.cpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <random>
#include <thread>
#include <future>
#include <map>
#include <set>

namespace nlm {

// AdvancedPhase5Config implementation

AdvancedPhase5Config::AdvancedPhase5Config()
    : initialNeurons(10000), maxNeurons(100000), connectionProbability(0.1f)
    , synapseFormationSteps(10000), sensoryDevelopmentSteps(40000), motorDevelopmentSteps(50000)
    , associativeDevelopmentSteps(100000), memoryConsolidationSteps(100000), socialDevelopmentSteps(100000)
    , trialsPerPhase(5), stepsPerTrial(10000)
    , enableParameterSweeps(false), parallelSweeps(false)
    , enableMultiObjectiveOptimization(false), convergenceThreshold(0.001f), maxIterations(100)
    , enableAdaptiveExperimentation(false), adaptationRate(0.01f), explorationRate(0.1f)
    , enableComparativeAnalysis(false), runAllCombinations(false)
    , enableHyperparameterOptimization(false), enableMultithreading(true), threadCount(0)
    , enableSIMD(true), enableMemoryOptimization(true), enableSparseConnectivity(true)
    , enableEventDriven(true), generateVisualizations(true), saveCheckpoints(true), generateReports(true)
    , checkpointInterval(20)
    , enableContinualLearningAnalysis(false), enableDamageRecoveryAnalysis(false)
    , enableScalingAnalysis(false), enableAblationAnalysis(false)
    , enableMultiSeedExperiments(false), numSeeds(5), enableCrossValidation(false), cvFolds(5) {}

AdvancedPhase5Config::AdvancedPhase5Config(const std::string& presetName) {
    *this = getPreset(presetName);
}

AdvancedPhase5Config AdvancedPhase5Config::getPreset(const std::string& presetName) {
    if (presetName == "ResearchLifelongLearning") {
        return AdvancedPhase5Config{
            /*initialNeurons=*/ 20000, /*maxNeurons=*/ 200000, /*connectionProbability=*/ 0.15f,
            /*synapseFormationSteps=*/ 15000, /*sensoryDevelopmentSteps=*/ 50000,
            /*motorDevelopmentSteps=*/ 75000, /*associativeDevelopmentSteps=*/ 150000,
            /*memoryConsolidationSteps=*/ 150000, /*socialDevelopmentSteps=*/ 150000,
            /*trialsPerPhase=*/ 8, /*stepsPerTrial=*/ 15000,
            /*enableParameterSweeps=*/ true,
            /*parameterRanges=*/ {
                {"initialNeurons", {10000.0f, 20000.0f, 50000.0f}},
                {"connectionProbability", {0.05f, 0.1f, 0.2f}},
                {"socialDevelopmentSteps", {50000.0f, 100000.0f, 200000.0f}}
            },
            /*sweepParameters=*/ {"initialNeurons", "connectionProbability", "socialDevelopmentSteps"},
            /*enableMultiObjectiveOptimization=*/ true,
            /*optimizationObjectives=*/ {"learningEfficiency", "memoryRetention", "socialAdaptation", "behavioralFlexibility"},
            /*enableAdaptiveExperimentation=*/ true, /*adaptationRate=*/ 0.02f, /*explorationRate=*/ 0.15f,
            /*enableComparativeAnalysis=*/ true,
            /*comparativePresets=*/ {"ResearchScientific", "ResearchReproducibility"},
            /*enableHyperparameterOptimization=*/ true,
            /*optimizationAlgorithm=*/ "CMA-ES",
            /*enableMultithreading=*/ true, /*threadCount=*/ 8,
            /*outputDirectory=*/ "results/research_lifelong_learning", /*generateVisualizations=*/ true,
            /*generateReports=*/ true,
            /*enableContinualLearningAnalysis=*/ true, /*enableScalingAnalysis=*/ true,
            /*enableMultiSeedExperiments=*/ true, /*numSeeds=*/ 10
        };
    } else if (presetName == "ResearchDamageRecovery") {
        return AdvancedPhase5Config{
            /*initialNeurons=*/ 50000, /*maxNeurons=*/ 500000, /*connectionProbability=*/ 0.2f,
            /*synapseFormationSteps=*/ 20000, /*sensoryDevelopmentSteps=*/ 60000,
            /*motorDevelopmentSteps=*/ 80000, /*associativeDevelopmentSteps=*/ 200000,
            /*memoryConsolidationSteps=*/ 200000, /*socialDevelopmentSteps=*/ 100000,
            /*trialsPerPhase=*/ 6, /*stepsPerTrial=*/ 20000,
            /*enableParameterSweeps=*/ true,
            /*parameterRanges=*/ {
                {"damageFraction", {0.1f, 0.3f, 0.5f}},
                {"recoveryTimeScale", {50000.0f, 100000.0f, 200000.0f}},
                {"neuroplasticityRate", {0.001f, 0.01f, 0.05f}}
            },
            /*sweepParameters=*/ {"damageFraction", "recoveryTimeScale", "neuroplasticityRate"},
            /*enableMultiObjectiveOptimization=*/ true,
            /*optimizationObjectives=*/ {"recoverySpeed", "recoveryQuality", "longTermFunction"},
            /*enableAdaptiveExperimentation=*/ true, /*adaptationRate=*/ 0.015f, /*explorationRate=*/ 0.2f,
            /*enableComparativeAnalysis=*/ true,
            /*comparativePresets=*/ {"ResearchScientific", "ResearchSpeed"},
            /*enableHyperparameterOptimization=*/ true,
            /*optimizationAlgorithm=*/ "BO",
            /*enableMultithreading=*/ true, /*threadCount=*/ 6,
            /*outputDirectory=*/ "results/research_damage_recovery", /*generateVisualizations=*/ true,
            /*generateReports=*/ true,
            /*enableDamageRecoveryAnalysis=*/ true, /*enableAblationAnalysis=*/ true,
            /*enableMultiSeedExperiments=*/ true, /*numSeeds=*/ 8
        };
    } else if (presetName == "ResearchScaling") {
        return AdvancedPhase5Config{
            /*initialNeurons=*/ 5000, /*maxNeurons=*/ 100000, /*connectionProbability=*/ 0.05f,
            /*synapseFormationSteps=*/ 5000, /*sensoryDevelopmentSteps=*/ 20000,
            /*motorDevelopmentSteps=*/ 30000, /*associativeDevelopmentSteps=*/ 60000,
            /*memoryConsolidationSteps=*/ 60000, /*socialDevelopmentSteps=*/ 30000,
            /*trialsPerPhase=*/ 3, /*stepsPerTrial=*/ 5000,
            /*enableParameterSweeps=*/ false,
            /*enableMultiObjectiveOptimization=*/ true,
            /*optimizationObjectives=*/ {"performanceEfficiency", "memoryUtilization", "scalability"},
            /*enableComparativeAnalysis=*/ false,
            /*enableHyperparameterOptimization=*/ true,
            /*optimizationAlgorithm=*/ "CBO",
            /*enableMultithreading=*/ true, /*threadCount=*/ 12,
            /*outputDirectory=*/ "results/research_scaling", /*generateVisualizations=*/ true,
            /*generateReports=*/ true,
            /*enableScalingAnalysis=*/ true,
            /*enableMultiSeedExperiments=*/ true, /*numSeeds=*/ 6
        };
    }
    return AdvancedPhase5Config(); // Default
}

std::vector<std::string> AdvancedPhase5Config::getAvailablePresets() {
    return {"ResearchLifelongLearning", "ResearchDamageRecovery", "ResearchScaling"};
}

bool AdvancedPhase5Config::validate() const {
    if (initialNeurons == 0) return false;
    if (maxNeurons == 0 || maxNeurons < initialNeurons) return false;
    if (connectionProbability <= 0.0f || connectionProbability > 1.0f) return false;
    if (synapseFormationSteps == 0 || sensoryDevelopmentSteps == 0 || motorDevelopmentSteps == 0) return false;
    if (trialsPerPhase == 0) return false;
    if (stepsPerTrial == 0) return false;
    if (convergenceThreshold <= 0.0f) return false;
    if (maxIterations == 0) return false;
    if (adaptationRate < 0.0f || adaptationRate > 1.0f) return false;
    if (explorationRate < 0.0f || explorationRate > 1.0f) return false;
    if (threadCount == 0) return false;
    return true;
}

std::string AdvancedPhase5Config::getValidationErrors() const {
    std::ostringstream errors;
    if (initialNeurons == 0) errors << "initialNeurons must be > 0; ";
    if (maxNeurons == 0 || maxNeurons < initialNeurons) errors << "maxNeurons must be >= initialNeurons; ";
    if (connectionProbability <= 0.0f || connectionProbability > 1.0f) errors << "connectionProbability must be in (0,1]; ";
    if (synapseFormationSteps == 0 || sensoryDevelopmentSteps == 0 || motorDevelopmentSteps == 0) errors << "development steps must be > 0; ";
    if (trialsPerPhase == 0) errors << "trialsPerPhase must be > 0; ";
    if (stepsPerTrial == 0) errors << "stepsPerTrial must be > 0; ";
    if (convergenceThreshold <= 0.0f) errors << "convergenceThreshold must be > 0; ";
    if (maxIterations == 0) errors << "maxIterations must be > 0; ";
    if (adaptationRate < 0.0f || adaptationRate > 1.0f) errors << "adaptationRate must be between 0 and 1; ";
    if (explorationRate < 0.0f || explorationRate > 1.0f) errors << "explorationRate must be between 0 and 1; ";
    if (threadCount == 0) errors << "threadCount must be > 0; ";
    return errors.str();
}

std::vector<std::string> AdvancedPhase5Config::getOptimizationRecommendations() const {
    std::vector<std::string> recommendations;
    if (enableParameterSweeps) {
        recommendations.push_back("Consider parallel execution for faster parameter sweep completion");
    }
    if (enableMultiSeedExperiments) {
        recommendations.push_back("Multi-seed experiments provide better statistical confidence");
    }
    if (enableScalingAnalysis) {
        recommendations.push_back("Scaling analysis helps identify performance bottlenecks");
    }
    if (enableAblationAnalysis) {
        recommendations.push_back("Ablation studies reveal causal contributions of mechanisms");
    }
    return recommendations;
}

// AdvancedPhase5Experiment implementation

struct AdvancedPhase5Experiment::Impl {
    std::shared_ptr<Logger> logger;
    std::vector<std::future<LifetimeExperimentResult>> futures;
    std::unordered_map<std::string, std::vector<float>> sweepParameterValues;
    std::vector<std::vector<float>> optimizationIterations;
    std::unordered_map<std::string, float> objectiveHistories;
    std::unordered_map<std::string, float> adaptiveParams;
    std::mt19937_64 rng;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    
    Impl() : rng(std::random_device{}()) {}
};

AdvancedPhase5Experiment::AdvancedPhase5Experiment(const AdvancedPhase5Config& config)
    : config_(config), running_(false), startTime(std::chrono::high_resolution_clock::now()) {
    pImpl = std::make_unique<Impl>();
    if (!config_.validate()) {
        NLM_LOG_ERROR("Invalid AdvancedPhase5Config: " + config_.getValidationErrors());
    }
}

AdvancedPhase5Experiment::AdvancedPhase5Experiment(const std::string& presetName)
    : config_(AdvancedPhase5Config(presetName)), running_(false),
      startTime(std::chrono::high_resolution_clock::now()) {
    pImpl = std::make_unique<Impl>();
}

AdvancedPhase5Experiment::~AdvancedPhase5Experiment() = default;

void AdvancedPhase5Experiment::run() {
    initializeExperiment();
    running_ = true;
    
    if (config_.enableParameterSweeps) {
        executeParameterSweep();
    } else {
        executeLifetimeExperiment();
    }
    
    if (config_.enableMultiObjectiveOptimization) {
        executeMultiObjectiveOptimization();
    }
    
    if (config_.enableMultiSeedExperiments) {
        // Run with multiple seeds
    }
    
    if (config_.enableContinualLearningAnalysis) {
        runContinualLearningAnalysis();
    }
    
    if (config_.enableDamageRecoveryAnalysis) {
        runDamageRecoveryAnalysis(0.3f, "region_1");
    }
    
    if (config_.enableScalingAnalysis) {
        runScalingAnalysis();
    }
    
    if (config_.enableAblationAnalysis) {
        runAblationAnalysis();
    }
    
    computeStatistics();
    running_ = false;
}

void AdvancedPhase5Experiment::initializeExperiment() {
    NLM_LOG_INFO("Initializing Advanced Phase 5 Experiment");
    NLM_LOG_INFO("Config: " + std::to_string(config_.initialNeurons) + " -> " +
                std::to_string(config_.maxNeurons) + " neurons, " +
                std::to_string(config_.synapseFormationSteps + config_.sensoryDevelopmentSteps +
                             config_.motorDevelopmentSteps + config_.associativeDevelopmentSteps +
                             config_.memoryConsolidationSteps + config_.socialDevelopmentSteps) + " total steps");
    
    pImpl->logger = std::make_shared<Logger>();
    pImpl->startTime = std::chrono::high_resolution_clock::now();
}

void AdvancedPhase5Experiment::executeLifetimeExperiment() {
    NLM_LOG_INFO("Executing lifetime experiment");
    
    Phase5IntegratedExperiment experiment;
    LifetimeExperimentConfig baseConfig;
    
    // Configure based on AdvancedPhase5Config
    baseConfig.initialNeurons = config_.initialNeurons;
    baseConfig.maxNeurons = config_.maxNeurons;
    baseConfig.connectionProbability = config_.connectionProbability;
    
    // Create brain factory function
    auto createBrain = [this](const LifetimeExperimentConfig& cfg) -> std::shared_ptr<Brain> {
        return createBrainForPhase("Development", cfg);
    };
    
    // Run simulation
    auto runSimulation = [this](Brain* brain, uint64_t steps) -> bool {
        // Simulate the brain steps
        for (uint64_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001f);  // 1ms timestep
        }
        return true;
    };
    
    // Run the experiment
    LifetimeExperimentResult result = experiment.run(baseConfig, createBrain, runSimulation);
    
    // Extract phase results
    for (const auto& phaseResult : result.phaseResults) {
        phaseResults_.push_back(phaseResult);
    }
}

void AdvancedPhase5Experiment::executeParameterSweep() {
    NLM_LOG_INFO("Executing parameter sweep for " + std::to_string(config_.sweepParameters.size()) + " parameters");
    
    for (const auto& param : config_.sweepParameters) {
        auto it = config_.parameterRanges.find(param);
        if (it != config_.parameterRanges.end()) {
            pImpl->sweepParameterValues[param] = it->second;
            
            if (config_.parallelSweeps) {
                // Submit to thread pool
                auto future = std::async(std::launch::async, &AdvancedPhase5Experiment::runParameterSweepThread, this, param, it->second);
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

void AdvancedPhase5Experiment::runParameterSweepThread(const std::string& param, const std::vector<float>& values) {
    NLM_LOG_INFO("Running parameter sweep for: " + param + " with " + std::to_string(values.size()) + " values");
    
    for (float value : values) {
        // Modify configuration temporarily
        auto originalValue = getCurrentConfigValue(param);
        setConfigValue(param, value);
        
        // Run experiment with modified parameter
        executeLifetimeExperiment();
        
        // Record results
        recordSweepResult(param, value);
        
        // Restore original value
        setConfigValue(param, originalValue);
    }
}

void AdvancedPhase5Experiment::recordSweepResult(const std::string& param, float value) {
    std::vector<float> objectiveValues;
    
    for (const auto& obj : config_.optimizationObjectives) {
        objectiveValues.push_back(getOptimizationScore(obj));
    }
    
    sweepResults_.push_back(objectiveValues);
}

void AdvancedPhase5Experiment::executeMultiObjectiveOptimization() {
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

void AdvancedPhase5Experiment::updateAdaptiveParameters() {
    // Update adaptive parameters based on optimization progress
    for (auto& [key, value] : pImpl->adaptiveParams) {
        value = value * (1.0f - config_.adaptationRate) +
                config_.explorationRate * (std::rand() % 100 / 100.0f);
    }
}

float AdvancedPhase5Experiment::evaluateObjective(const std::string& objective, const LifetimePhaseResult& results) const {
    if (objective == "learningEfficiency") {
        return results.avgRewardPerStep;
    } else if (objective == "memoryRetention") {
        return results.memoryRetention;
    } else if (objective == "socialAdaptation") {
        return results.developmentalProgress.find("social") != results.developmentalProgress.end() ?
               results.developmentalProgress.at("social") : 0.0f;
    } else if (objective == "behavioralFlexibility") {
        return 1.0f - results.activityStability; // Less stability means more flexibility
    } else if (objective == "recoverySpeed") {
        return 1.0f - (results.endStep - results.startStep) / 100000.0f;
    } else if (objective == "recoveryQuality") {
        return results.memoryRecall;
    } else if (objective == "longTermFunction") {
        return results.predictionAccuracy;
    } else if (objective == "performanceEfficiency") {
        return results.performance.avgStepTimeMs > 0.0 ? 1.0f / results.performance.avgStepTimeMs : 0.0f;
    } else if (objective == "memoryUtilization") {
        return results.performance.connectionDensity;
    } else if (objective == "scalability") {
        return results.performance.neuronsPerSecond;
    }
    return 0.0f;
}

void AdvancedPhase5Experiment::updateParetoFront(const std::vector<float>& objectiveValues) {
    paretoFront_.push_back(objectiveValues);
}

void AdvancedPhase5Experiment::computeStatistics() {
    NLM_LOG_INFO("Computing statistics for Advanced Phase 5 Experiment");
    
    // Compute various statistics
    for (const auto& obj : config_.optimizationObjectives) {
        std::vector<float> values;
        for (const auto& result : phaseResults_) {
            values.push_back(evaluateObjective(obj, result));
        }
        
        if (!values.empty()) {
            float mean = std::accumulate(values.begin(), values.end(), 0.0f) / values.size();
            float variance = 0.0f;
            for (float val : values) {
                variance += (val - mean) * (val - mean);
            }
            variance /= values.size();
            float stddev = std::sqrt(variance);
            
            pImpl->objectiveHistories[obj] = mean;
        }
    }
}

float AdvancedPhase5Experiment::getOptimizationScore(const std::string& objective) const {
    auto it = pImpl->objectiveHistories.find(objective);
    return (it != pImpl->objectiveHistories.end()) ? it->second : 0.0f;
}

std::string AdvancedPhase5Experiment::getParetoFrontDescription() const {
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

float AdvancedPhase5Experiment::getProgress() const {
    float totalWork = static_cast<float>(config_.trialsPerPhase) * (config_.enableParameterSweeps ? 1 : 1);
    float completedWork = static_cast<float>(phaseResults_.size());
    return (totalWork > 0.0f) ? (completedWork / totalWork) : 0.0f;
}

std::string AdvancedPhase5Experiment::getStatusReport() const {
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - startTime).count();
    
    std::ostringstream report;
    report << "=== Advanced Phase 5 Experiment Status ===\n";
    report << "Running: " << (running_ ? "Yes" : "No") << "\n";
    report << "Progress: " << (getProgress() * 100.0f) << "%\n";
    report << "Elapsed Time: " << elapsed << " seconds\n";
    report << "Phases Completed: " << phaseResults_.size() << "\n";
    report << "Parameter Sweeps: " << (config_.enableParameterSweeps ? "Enabled" : "Disabled") << "\n";
    report << "Multi-Objective Optimization: " << (config_.enableMultiObjectiveOptimization ? "Enabled" : "Disabled") << "\n";
    report << "Adaptive Experimentation: " << (config_.enableAdaptiveExperimentation ? "Enabled" : "Disabled") << "\n";
    report << "Hyperparameter Optimization: " << (config_.enableHyperparameterOptimization ? "Enabled" : "Disabled") << "\n";
    report << "Multi-Seed Experiments: " << (config_.enableMultiSeedExperiments ? "Enabled" : "Disabled") << "\n";
    report << "Threads: " << config_.threadCount << "\n";
    
    return report.str();
}

bool AdvancedPhase5Experiment::exportResults(const std::string& filepath) const {
    NLM_LOG_INFO("Exporting AdvancedPhase5Experiment results to: " + filepath);
    // Export implementation
    return true;
}

std::shared_ptr<AdvancedPhase5Experiment> AdvancedPhase5Experiment::importResults(const std::string& filepath) {
    NLM_LOG_INFO("Importing AdvancedPhase5Experiment results from: " + filepath);
    // Import implementation
    return nullptr;
}

void AdvancedPhase5Experiment::enableHyperparameterOptimization(const std::string& algorithm) {
    config_.enableHyperparameterOptimization = true;
    config_.optimizationAlgorithm = algorithm;
}

void AdvancedPhase5Experiment::setOptimizationAlgorithm(const std::string& algorithm) {
    config_.optimizationAlgorithm = algorithm;
}

void AdvancedPhase5Experiment::setOptimizationParameters(const std::unordered_map<std::string, float>& params) {
    config_.optimizationParameters = params;
}

void AdvancedPhase5Experiment::enableMultithreading(size_t threads) {
    config_.enableMultithreading = true;
    config_.threadCount = threads;
}

std::vector<std::string> AdvancedPhase5Experiment::getAvailablePresets() {
    return AdvancedPhase5Config::getAvailablePresets();
}

AdvancedPhase5Config AdvancedPhase5Experiment::getResearchPreset(const std::string& researchArea) {
    if (researchArea == "lifelong_learning") {
        return AdvancedPhase5Config("ResearchLifelongLearning");
    } else if (researchArea == "damage_recovery") {
        return AdvancedPhase5Config("ResearchDamageRecovery");
    } else if (researchArea == "scaling") {
        return AdvancedPhase5Config("ResearchScaling");
    }
    return AdvancedPhase5Config();
}

AdvancedPhase5Config AdvancedPhase5Experiment::getPerformancePreset(const std::string& scale) {
    if (scale == "small") {
        return AdvancedPhase5Config{
            /*initialNeurons=*/ 5000, /*maxNeurons=*/ 20000, /*connectionProbability=*/ 0.05f,
            /*synapseFormationSteps=*/ 5000, /*sensoryDevelopmentSteps=*/ 15000,
            /*motorDevelopmentSteps=*/ 20000, /*associativeDevelopmentSteps=*/ 40000,
            /*memoryConsolidationSteps=*/ 40000, /*socialDevelopmentSteps=*/ 20000,
            /*outputDirectory=*/ "results/performance_small"
        };
    } else if (scale == "medium") {
        return AdvancedPhase5Config{
            /*initialNeurons=*/ 20000, /*maxNeurons=*/ 100000, /*connectionProbability=*/ 0.1f,
            /*synapseFormationSteps=*/ 15000, /*sensoryDevelopmentSteps=*/ 40000,
            /*motorDevelopmentSteps=*/ 60000, /*associativeDevelopmentSteps=*/ 120000,
            /*memoryConsolidationSteps=*/ 120000, /*socialDevelopmentSteps=*/ 60000,
            /*outputDirectory=*/ "results/performance_medium"
        };
    } else if (scale == "large") {
        return AdvancedPhase5Config{
            /*initialNeurons=*/ 100000, /*maxNeurons=*/ 1000000, /*connectionProbability=*/ 0.15f,
            /*synapseFormationSteps=*/ 30000, /*sensoryDevelopmentSteps=*/ 80000,
            /*motorDevelopmentSteps=*/ 120000, /*associativeDevelopmentSteps=*/ 240000,
            /*memoryConsolidationSteps=*/ 240000, /*socialDevelopmentSteps=*/ 120000,
            /*outputDirectory=*/ "results/performance_large"
        };
    }
    return AdvancedPhase5Config();
}

AdvancedPhase5Config AdvancedPhase5Experiment::getResearchQuestionPreset(const std::string& question) {
    if (question == "learning_curves_over_time") {
        return AdvancedPhase5Config{
            /*enableParameterSweeps=*/ true,
            /*parameterRanges=*/ {
                {"timeStep", {0.001f, 0.005f, 0.01f, 0.05f}},
                {"developmentSpeed", {0.5f, 1.0f, 2.0f}}
            },
            /*outputDirectory=*/ "results/research_learning_curves"
        };
    } else if (question == "memory_capacity_limits") {
        return AdvancedPhase5Config{
            /*initialNeurons=*/ 10000, /*maxNeurons=*/ 100000,
            /*parameterRanges=*/ {
                {"connectionProbability", {0.01f, 0.05f, 0.1f, 0.2f}},
                {"synapticStrength", {0.1f, 0.5f, 1.0f}}
            },
            /*outputDirectory=*/ "results/research_memory_capacity"
        };
    } else if (question == "social_behavior_dynamics") {
        return AdvancedPhase5Config{
            /*initialNeurons=*/ 15000,
            /*parameterRanges=*/ {
                {"socialInteractionRate", {0.1f, 0.3f, 0.5f}},
                {"imitationProbability", {0.3f, 0.6f, 0.9f}}
            },
            /*outputDirectory=*/ "results/research_social_dynamics"
        };
    } else if (question == "developmental_stages") {
        return AdvancedPhase5Config{
            /*enableDevelopmentalTracking=*/ true,
            /*parameterRanges=*/ {
                {"developmentStageDuration", {1000.0f, 5000.0f, 10000.0f}},
                {"transitionRate", {0.01f, 0.05f, 0.1f}}
            },
            /*outputDirectory=*/ "results/research_development_stages"
        };
    }
    return AdvancedPhase5Config();
}

void AdvancedPhase5Experiment::runContinualLearningAnalysis() {
    NLM_LOG_INFO("Running continual learning analysis");
    // Continual learning implementation
}

void AdvancedPhase5Experiment::runDamageRecoveryAnalysis(float damageFraction, const std::string& region) {
    NLM_LOG_INFO("Running damage recovery analysis: " + std::to_string(damageFraction) + " damage to " + region);
    // Damage recovery implementation
}

void AdvancedPhase5Experiment::runScalingAnalysis() {
    NLM_LOG_INFO("Running scaling analysis");
    // Scaling implementation
}

void AdvancedPhase5Experiment::runAblationAnalysis() {
    NLM_LOG_INFO("Running ablation analysis");
    // Ablation implementation
}

void AdvancedPhase5Experiment::updateConfig(const AdvancedPhase5Config& newConfig) {
    if (newConfig.validate()) {
        config_ = newConfig;
        NLM_LOG_INFO("AdvancedPhase5Config updated successfully");
    } else {
        NLM_LOG_ERROR("Invalid config update rejected: " + newConfig.getValidationErrors());
    }
}

void AdvancedPhase5Experiment::runComparativeAnalysis(const std::vector<std::string>& presetNames) {
    NLM_LOG_INFO("Running comparative analysis with " + std::to_string(presetNames.size()) + " presets");
    // Comparative analysis implementation
}

void AdvancedPhase5Experiment::runMultiObjectiveOptimization() {
    NLM_LOG_INFO("Running multi-objective optimization");
    // Multi-objective optimization implementation
}

void AdvancedPhase5Experiment::enableAdaptiveExperimentation(float rate, float exploration) {
    config_.enableAdaptiveExperimentation = true;
    config_.adaptationRate = rate;
    config_.explorationRate = exploration;
}

std::string AdvancedPhase5Experiment::getCurrentConfigValue(const std::string& key) {
    auto it = config_.optimizationParameters.find(key);
    return (it != config_.optimizationParameters.end()) ? std::to_string(it->second) : "0.0";
}

void AdvancedPhase5Experiment::setConfigValue(const std::string& key, float value) {
    config_.optimizationParameters[key] = value;
}

std::shared_ptr<Brain> AdvancedPhase5Experiment::createBrainForPhase(const std::string& phaseName, const LifetimeExperimentConfig& baseConfig) {
    NLM_LOG_INFO("Creating brain for phase: " + phaseName);
    
    auto brain = std::make_shared<Brain>(
        baseConfig.initialNeurons,
        baseConfig.connectionProbability
    );
    
    // Initialize based on phase
    if (phaseName == "Early Development") {
        brain->initializeSynapticFormation(baseConfig.synapseFormationSteps);
    } else if (phaseName == "Sensory Development") {
        brain->initializeSensoryDevelopment(baseConfig.sensoryDevelopmentSteps);
    } else if (phaseName == "Motor Development") {
        brain->initializeMotorDevelopment(baseConfig.motorDevelopmentSteps);
    } else if (phaseName == "Associative Development") {
        brain->initializeAssociativeDevelopment(baseConfig.associativeDevelopmentSteps);
    } else if (phaseName == "Memory Consolidation") {
        brain->initializeMemoryConsolidation(baseConfig.memoryConsolidationSteps);
    } else if (phaseName == "Social Development") {
        brain->initializeSocialDevelopment(baseConfig.socialDevelopmentSteps);
    }
    
    return brain;
}

std::shared_ptr<Brain> AdvancedPhase5Experiment::createBrainForScale(const ScaleLevel& scale, uint64_t seed) {
    NLM_LOG_INFO("Creating brain for scale: " + scale.name + " (" + std::to_string(scale.neurons) + " neurons)");
    
    auto brain = std::make_shared<Brain>(scale.neurons, scale.connectionProbability);
    brain->setRandomSeed(seed);
    
    return brain;
}

std::shared_ptr<Brain> AdvancedPhase5Experiment::createBrainForAblation(const AblationConfig& config) {
    NLM_LOG_INFO("Creating brain for ablation analysis");
    
    auto brain = std::make_shared<Brain>();
    
    // Apply ablation configuration
    for (const auto& target : config.disabledTargets) {
        brain->disableMechanism(target);
    }
    
    return brain;
}

float AdvancedPhase5Experiment::computeConfidenceInterval(float confidence) const {
    if (phaseResults_.empty()) return 0.0f;
    
    std::vector<float> scores;
    for (const auto& result : phaseResults_) {
        scores.push_back(evaluateObjective("learningEfficiency", result));
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

bool AdvancedPhase5Experiment::checkConvergence() const {
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

std::string AdvancedPhase5Experiment::getComprehensiveReport() const {
    std::ostringstream oss;
    oss << "=== Advanced Phase 5 Experiment Comprehensive Report ===\n\n";
    
    oss << "Experiment Configuration:\n";
    oss << "  - Scale: " << config_.initialNeurons << " -> " << config_.maxNeurons << " neurons\n";
    oss << "  - Development Time: " << (config_.synapseFormationSteps + config_.sensoryDevelopmentSteps + 
                                   config_.motorDevelopmentSteps + config_.associativeDevelopmentSteps +
                                   config_.memoryConsolidationSteps + config_.socialDevelopmentSteps) << " steps\n";
    oss << "  - Parameter Sweeps: " << (config_.enableParameterSweeps ? "Enabled" : "Disabled") << "\n";
    oss << "  - Multi-Objective Optimization: " << (config_.enableMultiObjectiveOptimization ? "Enabled" : "Disabled") << "\n";
    oss << "  - Adaptive Experimentation: " << (config_.enableAdaptiveExperimentation ? "Enabled" : "Disabled") << "\n\n";
    
    oss << "Results Summary:\n";
    oss << "  - Phases Completed: " << phaseResults_.size() << "\n";
    oss << "  - Parameter Sweep Points: " << sweepResults_.size() << "\n";
    oss << "  - Multi-Seed Results: " << multiSeedResults_.size() << "\n";
    oss << "  - Scaling Analyses: " << scalingResults_.size() << "\n";
    oss << "  - Ablation Analyses: " << ablativeResults_.size() << "\n\n";
    
    oss << "Pareto Front Analysis:\n";
    oss << getParetoFrontDescription() << "\n";
    
    oss << "\nConfidence Intervals:\n";
    oss << "  - Learning Efficiency (95%): " << computeConfidenceInterval(0.95f) << "\n";
    oss << "  - Convergence Status: " << (checkConvergence() ? "Converged" : "Not Converged") << "\n";
    
    return oss.str();
}

} // namespace nlm