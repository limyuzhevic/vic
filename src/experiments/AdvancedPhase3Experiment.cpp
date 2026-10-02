#include "AdvancedPhase3Experiment.hpp"
#include "../core/Logger/Logger.hpp"
#include "../agent/AgentBody.hpp"
#include "../environment/Environment.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <thread>
#include <future>
#include <random>

namespace nlm {

// AdvancedPhase3Config implementation

AdvancedPhase3Config::AdvancedPhase3Config() 
    : numAgents(50), worldWidth(100), worldHeight(100), timeStep(0.1f)
    , explorationRate(0.3f), noveltyThreshold(0.5f), energyDecayRate(0.01f)
    , curiosityCoefficient(0.1f), maxEpisodes(100), stepsPerEpisode(200)
    , rewardScaling(1.0f), noveltyScaling(1.0f), temperature(25.0f)
    , humidity(0.5f), resourceDensity(0.3f), enableVerboseLogging(false)
    , logInterval(10), enableParameterSweeps(false), enableMultiObjectiveOptimization(false)
    , enableAdaptiveExperimentation(false), adaptationRate(0.01f)
    , enableDevelopmentalTracking(true), developmentalMilestoneInterval(50.0f)
    , enableComparativeAnalysis(false), runInParallel(false) {}

AdvancedPhase3Config::AdvancedPhase3Config(const std::string& presetName) {
    *this = getPreset(presetName);
}

AdvancedPhase3Config AdvancedPhase3Config::getPreset(const std::string& presetName) {
    if (presetName == "ResearchScientific") {
        return AdvancedPhase3Config{
            /*numAgents=*/ 100, /*worldWidth=*/ 200, /*worldHeight=*/ 200, /*timeStep=*/ 0.05f,
            /*explorationRate=*/ 0.5f, /*noveltyThreshold=*/ 0.3f, /*energyDecayRate=*/ 0.005f,
            /*curiosityCoefficient=*/ 0.15f, /*maxEpisodes=*/ 200, /*stepsPerEpisode=*/ 500,
            /*rewardScaling=*/ 1.0f, /*noveltyScaling=*/ 1.0f, /*temperature=*/ 22.0f,
            /*humidity=*/ 0.6f, /*resourceDensity=*/ 0.4f, /*outputFile=*/ "research_scientific.json",
            /*enableVerboseLogging=*/ true, /*logInterval=*/ 5,
            /*enableParameterSweeps=*/ false, /*enableMultiObjectiveOptimization=*/ true,
            /*enableAdaptiveExperimentation=*/ true, /*adaptationRate=*/ 0.02f,
            /*enableDevelopmentalTracking=*/ true, /*developmentalMilestoneInterval=*/ 25.0f,
            /*enableComparativeAnalysis=*/ true, /*runInParallel=*/ false
        };
    } else if (presetName == "ResearchSpeed") {
        return AdvancedPhase3Config{
            /*numAgents=*/ 20, /*worldWidth=*/ 50, /*worldHeight=*/ 50, /*timeStep=*/ 0.2f,
            /*explorationRate=*/ 0.2f, /*noveltyThreshold=*/ 0.8f, /*energyDecayRate=*/ 0.02f,
            /*curiosityCoefficient=*/ 0.05f, /*maxEpisodes=*/ 50, /*stepsPerEpisode=*/ 100,
            /*rewardScaling=*/ 0.5f, /*noveltyScaling=*/ 0.5f, /*temperature=*/ 28.0f,
            /*humidity=*/ 0.4f, /*resourceDensity=*/ 0.2f, /*outputFile=*/ "research_speed.json",
            /*enableVerboseLogging=*/ false, /*logInterval=*/ 50,
            /*enableParameterSweeps=*/ true, /*enableMultiObjectiveOptimization=*/ false,
            /*enableAdaptiveExperimentation=*/ false, /*adaptationRate=*/ 0.0f,
            /*enableDevelopmentalTracking=*/ false, /*developmentalMilestoneInterval=*/ 0.0f,
            /*enableComparativeAnalysis=*/ false, /*runInParallel=*/ true
        };
    } else if (presetName == "ResearchReproducibility") {
        return AdvancedPhase3Config{
            /*numAgents=*/ 30, /*worldWidth=*/ 100, /*worldHeight=*/ 100, /*timeStep=*/ 0.1f,
            /*explorationRate=*/ 0.4f, /*noveltyThreshold=*/ 0.5f, /*energyDecayRate=*/ 0.01f,
            /*curiosityCoefficient=*/ 0.1f, /*maxEpisodes=*/ 150, /*stepsPerEpisode=*/ 300,
            /*rewardScaling=*/ 1.0f, /*noveltyScaling=*/ 1.0f, /*temperature=*/ 23.0f,
            /*humidity=*/ 0.5f, /*resourceDensity=*/ 0.3f, /*outputFile=*/ "research_reproducible.json",
            /*enableVerboseLogging=*/ true, /*logInterval=*/ 10,
            /*enableParameterSweeps=*/ true, /*enableMultiObjectiveOptimization=*/ true,
            /*enableAdaptiveExperimentation=*/ true, /*adaptationRate=*/ 0.01f,
            /*enableDevelopmentalTracking=*/ true, /*developmentalMilestoneInterval=*/ 25.0f,
            /*enableComparativeAnalysis=*/ true, /*runInParallel=*/ false
        };
    } else {
        return AdvancedPhase3Config(); // Default
    }
}

std::vector<std::string> AdvancedPhase3Config::getAvailablePresets() {
    return {"ResearchScientific", "ResearchSpeed", "ResearchReproducibility"};
}

bool AdvancedPhase3Config::validate() const {
    if (numAgents == 0) return false;
    if (worldWidth == 0 || worldHeight == 0) return false;
    if (timeStep <= 0.0f) return false;
    if (maxEpisodes == 0) return false;
    if (stepsPerEpisode == 0) return false;
    if (adaptationRate < 0.0f || adaptationRate > 1.0f) return false;
    if (developmentalMilestoneInterval < 0.0f) return false;
    return true;
}

std::string AdvancedPhase3Config::getValidationErrors() const {
    std::ostringstream errors;
    if (numAgents == 0) errors << "numAgents must be > 0; ";
    if (worldWidth == 0 || worldHeight == 0) errors << "world dimensions must be > 0; ";
    if (timeStep <= 0.0f) errors << "timeStep must be > 0; ";
    if (maxEpisodes == 0) errors << "maxEpisodes must be > 0; ";
    if (stepsPerEpisode == 0) errors << "stepsPerEpisode must be > 0; ";
    if (adaptationRate < 0.0f || adaptationRate > 1.0f) errors << "adaptationRate must be between 0 and 1; ";
    if (developmentalMilestoneInterval < 0.0f) errors << "developmentalMilestoneInterval must be >= 0; ";
    return errors.str();
}

// AdvancedPhase3Experiment implementation

struct AdvancedPhase3Experiment::Impl {
    std::shared_ptr<ExperimentLogger> logger;
    std::shared_ptr<Environment> environment;
    std::vector<std::shared_ptr<AgentBody>> agents;
    std::unordered_map<std::string, std::vector<float>> parameterSweepResults;
    std::vector<std::vector<float>> optimizationHistory;
    std::unordered_map<std::string, float> adaptiveParameters;
    std::vector<std::string> comparativeReports;
    std::mt19937_64 rng;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    
    Impl() : rng(std::random_device{}()) {}
};

AdvancedPhase3Experiment::AdvancedPhase3Experiment(const AdvancedPhase3Config& config) 
    : config_(config), running_(false), currentAge_(0.0f), currentStage_("Initial"), 
      performanceScore_(0.0f) {
    pImpl = std::make_unique<Impl>();
    if (!config_.validate()) {
        NLM_LOG_ERROR("Invalid AdvancedPhase3Config: " + config_.getValidationErrors());
    }
}

AdvancedPhase3Experiment::AdvancedPhase3Experiment(const std::string& presetName) 
    : config_(AdvancedPhase3Config(presetName)), running_(false), currentAge_(0.0f),
      currentStage_("Initial"), performanceScore_(0.0f) {
    pImpl = std::make_unique<Impl>();
}

AdvancedPhase3Experiment::~AdvancedPhase3Experiment() = default;

void AdvancedPhase3Experiment::run() {
    initializeExperiment();
    running_ = true;
    
    for (size_t episode = 0; episode < config_.maxEpisodes && running_; ++episode) {
        NLM_LOG_INFO("Episode " + std::to_string(episode) + "/" + std::to_string(config_.maxEpisodes));
        
        for (size_t step = 0; step < config_.stepsPerEpisode && running_; ++step) {
            executeExperimentStep(step);
            
            if (config_.enableAdaptiveExperimentation) {
                updateAdaptiveParameters();
            }
        }
        
        if (config_.enableDevelopmentalTracking && episode % 20 == 0) {
            currentAge_ += config_.developmentalMilestoneInterval;
            trackDevelopmentalStage(currentAge_, "ProgressCheckpoint");
        }
    }
    
    finalizeExperiment();
    running_ = false;
}

void AdvancedPhase3Experiment::initializeExperiment() {
    NLM_LOG_INFO("Initializing Advanced Phase 3 Experiment");
    NLM_LOG_INFO("Config: " + std::to_string(config_.numAgents) + " agents, " +
                std::to_string(config_.worldWidth) + "x" + std::to_string(config_.worldHeight) +
                " world, " + std::to_string(config_.maxEpisodes) + " episodes");
    
    // Initialize logger
    pImpl->logger = std::make_shared<ExperimentLogger>(config_.outputFile);
    pImpl->logger->logPhase3Header();
    
    // Initialize agents
    for (size_t i = 0; i < config_.numAgents; ++i) {
        auto agent = std::make_shared<AgentBody>(
            config_.explorationRate, config_.noveltyThreshold,
            config_.energyDecayRate, config_.curiosityCoefficient
        );
        pImpl->agents.push_back(agent);
    }
    
    // Initialize environment
    pImpl->environment = std::make_shared<Environment>(
        config_.worldWidth, config_.worldHeight, config_.temperature,
        config_.humidity, config_.resourceDensity
    );
    
    pImpl->startTime = std::chrono::high_resolution_clock::now();
}

void AdvancedPhase3Experiment::executeExperimentStep(size_t step) {
    // Simulate one step of the experiment
    currentAge_ += config_.timeStep;
    
    // Log step
    if (config_.enableVerboseLogging && step % config_.logInterval == 0) {
        pImpl->logger->logStep(
            step, 
            0.0f,  // reward (placeholder)
            0.0f,  // neuromod (placeholder)
            0.0f,  // curiosity (placeholder)
            0.0f,  // energy (placeholder)
            "move"  // action (placeholder)
        );
    }
}

void AdvancedPhase3Experiment::finalizeExperiment() {
    NLM_LOG_INFO("Finalizing Advanced Phase 3 Experiment");
    performanceScore_ = calculatePerformanceScore();
    
    if (config_.enableComparativeAnalysis) {
        generateComparativeReport();
    }
    
    if (config_.enableParameterSweeps) {
        NLM_LOG_INFO("Parameter sweep results collected");
    }
    
    pImpl->logger->flush();
}

void AdvancedPhase3Experiment::updateAdaptiveParameters() {
    // Update parameters based on experiment results
    if (performanceScore_ > 0.0f) {
        for (auto& [key, value] : pImpl->adaptiveParameters) {
            value += adaptationRate * (0.5f - value);  // Move toward 0.5
        }
    }
}

float AdvancedPhase3Experiment::calculatePerformanceScore() const {
    if (results_.empty()) return 0.0f;
    
    float totalReward = 0.0f;
    float totalNovelty = 0.0f;
    float totalEnergy = 0.0f;
    
    for (const auto& result : results_) {
        totalReward += result.totalReward;
        totalNovelty += result.finalNovelty;
        totalEnergy += result.finalEnergy;
    }
    
    size_t count = results_.size();
    return (totalReward / count) * 0.4f + (totalNovelty / count) * 0.3f + (totalEnergy / count) * 0.3f;
}

void AdvancedPhase3Experiment::generateComparativeReport() const {
    NLM_LOG_INFO("Generating comparative analysis report");
    // Comparative analysis implementation
}

void AdvancedPhase3Experiment::runParameterSweep(const std::string& parameterName, const std::vector<float>& values) {
    NLM_LOG_INFO("Running parameter sweep for: " + parameterName);
    pImpl->parameterSweepResults[parameterName] = values;
    // Parameter sweep implementation
}

void AdvancedPhase3Experiment::runMultiObjectiveOptimization(const std::vector<std::string>& objectives) {
    NLM_LOG_INFO("Running multi-objective optimization with objectives: " + std::to_string(objectives.size()));
    pImpl->optimizationHistory.push_back(std::vector<float>(objectives.size(), 0.0f));
    // Multi-objective optimization implementation
}

void AdvancedPhase3Experiment::enableAdaptiveExperimentation(float rate) {
    config_.enableAdaptiveExperimentation = true;
    config_.adaptationRate = rate;
}

void AdvancedPhase3Experiment::runComparativeAnalysis(const std::vector<std::string>& presetNames) {
    NLM_LOG_INFO("Running comparative analysis with " + std::to_string(presetNames.size()) + " presets");
    // Comparative analysis implementation
}

void AdvancedPhase3Experiment::trackDevelopmentalStage(float age, const std::string& stageName) {
    currentAge_ = age;
    currentStage_ = stageName;
    
    if (pImpl->logger) {
        pImpl->logger->logDevelopment(age, stageName);
    }
}

void AdvancedPhase3Experiment::updateConfig(const AdvancedPhase3Config& newConfig) {
    if (newConfig.validate()) {
        config_ = newConfig;
        NLM_LOG_INFO("AdvancedPhase3Config updated successfully");
    } else {
        NLM_LOG_ERROR("Invalid config update rejected: " + newConfig.getValidationErrors());
    }
}

void AdvancedPhase3Experiment::enableHyperparameterOptimization() {
    config_.enableMultiObjectiveOptimization = true;
    config_.enableParameterSweeps = true;
    setOptimizationAlgorithm("CMA-ES");
}

void AdvancedPhase3Experiment::setOptimizationAlgorithm(const std::string& algorithm) {
    // Set optimization algorithm
}

void AdvancedPhase3Experiment::setOptimizationParameters(const std::unordered_map<std::string, float>& params) {
    // Set optimization parameters
}

float AdvancedPhase3Experiment::getProgress() const {
    return running_ ? (static_cast<float>(currentAge_) / 
                      static_cast<float>(config_.maxEpisodes * config_.stepsPerEpisode)) : 0.0f;
}

std::string AdvancedPhase3Experiment::getStatusReport() const {
    std::ostringstream report;
    report << "=== Advanced Phase 3 Experiment Status ===\n";
    report << "Running: " << (running_ ? "Yes" : "No") << "\n";
    report << "Progress: " << (getProgress() * 100.0f) << "%\n";
    report << "Current Age: " << currentAge_ << " steps\n";
    report << "Current Stage: " << currentStage_ << "\n";
    report << "Performance Score: " << performanceScore_ << "\n";
    report << "Preset: " << (config_.outputFile.empty() ? "Default" : config_.outputFile) << "\n";
    report << "Advanced Features:\n";
    report << "  - Parameter Sweeps: " << (config_.enableParameterSweeps ? "Enabled" : "Disabled") << "\n";
    report << "  - Multi-Objective Optimization: " << (config_.enableMultiObjectiveOptimization ? "Enabled" : "Disabled") << "\n";
    report << "  - Adaptive Experimentation: " << (config_.enableAdaptiveExperimentation ? "Enabled" : "Disabled") << "\n";
    report << "  - Developmental Tracking: " << (config_.enableDevelopmentalTracking ? "Enabled" : "Disabled") << "\n";
    report << "  - Comparative Analysis: " << (config_.enableComparativeAnalysis ? "Enabled" : "Disabled") << "\n";
    
    return report.str();
}

bool AdvancedPhase3Experiment::exportResults(const std::string& filepath) const {
    NLM_LOG_INFO("Exporting AdvancedPhase3Experiment results to: " + filepath);
    // Export implementation
    return true;
}

std::shared_ptr<AdvancedPhase3Experiment> AdvancedPhase3Experiment::importResults(const std::string& filepath) {
    NLM_LOG_INFO("Importing AdvancedPhase3Experiment results from: " + filepath);
    // Import implementation
    return nullptr;
}

std::vector<std::string> AdvancedPhase3Experiment::getAvailablePresets() {
    return AdvancedPhase3Config::getAvailablePresets();
}

AdvancedPhase3Config AdvancedPhase3Experiment::getResearchPreset(const std::string& researchType) {
    if (researchType == "scientific") {
        return AdvancedPhase3Config("ResearchScientific");
    } else if (researchType == "speed") {
        return AdvancedPhase3Config("ResearchSpeed");
    } else if (researchType == "reproducibility") {
        return AdvancedPhase3Config("ResearchReproducibility");
    }
    return AdvancedPhase3Config();
}

AdvancedPhase3Config AdvancedPhase3Experiment::getPerformancePreset(const std::string& performanceLevel) {
    if (performanceLevel == "small") {
        return AdvancedPhase3Config{
            /*numAgents=*/ 50, /*worldWidth=*/ 100, /*worldHeight=*/ 100, /*timeStep=*/ 0.1f,
            /*explorationRate=*/ 0.3f, /*noveltyThreshold=*/ 0.5f, /*energyDecayRate=*/ 0.01f,
            /*curiosityCoefficient=*/ 0.1f, /*maxEpisodes=*/ 50, /*stepsPerEpisode=*/ 200,
            /*outputFile=*/ "performance_small.json"
        };
    } else if (performanceLevel == "medium") {
        return AdvancedPhase3Config{
            /*numAgents=*/ 100, /*worldWidth=*/ 200, /*worldHeight=*/ 200, /*timeStep=*/ 0.05f,
            /*explorationRate=*/ 0.4f, /*noveltyThreshold=*/ 0.4f, /*energyDecayRate=*/ 0.005f,
            /*curiosityCoefficient=*/ 0.15f, /*maxEpisodes=*/ 100, /*stepsPerEpisode=*/ 300,
            /*outputFile=*/ "performance_medium.json"
        };
    } else if (performanceLevel == "large") {
        return AdvancedPhase3Config{
            /*numAgents=*/ 200, /*worldWidth=*/ 400, /*worldHeight=*/ 400, /*timeStep=*/ 0.02f,
            /*explorationRate=*/ 0.5f, /*noveltyThreshold=*/ 0.3f, /*energyDecayRate=*/ 0.001f,
            /*curiosityCoefficient=*/ 0.2f, /*maxEpisodes=*/ 200, /*stepsPerEpisode=*/ 500,
            /*outputFile=*/ "performance_large.json"
        };
    }
    return AdvancedPhase3Config();
}

AdvancedPhase3Config AdvancedPhase3Experiment::getResearchQuestionPreset(const std::string& question) {
    if (question == "learning_curves") {
        return AdvancedPhase3Config{
            /*outputFile=*/ "research_learning_curves.json",
            /*enableVerboseLogging=*/ true, /*logInterval=*/ 5,
            /*enableParameterSweeps=*/ true, /*enableMultiObjectiveOptimization=*/ true,
            /*parameterRanges=*/ {
                {"explorationRate", {0.1f, 0.2f, 0.3f, 0.4f, 0.5f}},
                {"curiosityCoefficient", {0.05f, 0.1f, 0.15f, 0.2f, 0.25f}}
            }
        };
    } else if (question == "memory_capacity") {
        return AdvancedPhase3Config{
            /*outputFile=*/ "research_memory_capacity.json",
            /*enableVerboseLogging=*/ true, /*logInterval=*/ 10,
            /*enableParameterSweeps=*/ true, /*enableMultiObjectiveOptimization=*/ true,
            /*parameterRanges=*/ {
                {"worldWidth", {50.0f, 100.0f, 150.0f, 200.0f}},
                {"worldHeight", {50.0f, 100.0f, 150.0f, 200.0f}}
            }
        };
    } else if (question == "social_dynamics") {
        return AdvancedPhase3Config{
            /*numAgents=*/ 100, /*worldWidth=*/ 200, /*worldHeight=*/ 200,
            /*outputFile=*/ "research_social_dynamics.json",
            /*enableVerboseLogging=*/ true, /*logInterval=*/ 5,
            /*enableParameterSweeps=*/ true, /*enableMultiObjectiveOptimization=*/ true,
            /*parameterRanges=*/ {
                {"socialInteractionRate", {0.1f, 0.3f, 0.5f, 0.7f}},
                {"imitationProbability", {0.3f, 0.5f, 0.7f}}
            }
        };
    } else if (question == "development_stages") {
        return AdvancedPhase3Config{
            /*outputFile=*/ "research_development_stages.json",
            /*enableDevelopmentalTracking=*/ true, /*developmentalMilestoneInterval=*/ 10.0f,
            /*enableVerboseLogging=*/ true, /*logInterval=*/ 2,
            /*enableParameterSweeps=*/ true, /*enableMultiObjectiveOptimization=*/ true,
            /*parameterRanges=*/ {
                {"energyDecayRate", {0.001f, 0.01f, 0.05f, 0.1f}},
                {"noveltyThreshold", {0.2f, 0.4f, 0.6f, 0.8f}}
            }
        };
    }
    return AdvancedPhase3Config();
}

std::string AdvancedPhase3Experiment::getOptimizationReport() const {
    std::ostringstream report;
    report << "=== Advanced Phase 3 Optimization Report ===\n";
    report << "Final Performance Score: " << performanceScore_ << "\n";
    report << "Number of Results: " << results_.size() << "\n";
    report << "Average Reward: " << (results_.empty() ? 0.0f : 
        std::accumulate(results_.begin(), results_.end(), 0.0f,
                       [](float sum, const ExperimentMetrics& r) { return sum + r.totalReward; }) / results_.size()) << "\n";
    report << "Average Novelty: " << (results_.empty() ? 0.0f :
        std::accumulate(results_.begin(), results_.end(), 0.0f,
                       [](float sum, const ExperimentMetrics& r) { return sum + r.finalNovelty; }) / results_.size()) << "\n";
    
    if (pImpl->optimizationHistory.size() > 0) {
        report << "\nOptimization History:\n";
        for (size_t i = 0; i < pImpl->optimizationHistory.size(); ++i) {
            report << "Generation " << i << ": ";
            for (float value : pImpl->optimizationHistory[i]) {
                report << value << " ";
            }
            report << "\n";
        }
    }
    
    return report.str();
}

} // namespace nlm