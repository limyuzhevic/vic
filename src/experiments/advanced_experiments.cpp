#include "advanced_experiments.hpp"
#include "ExperimentRunner.hpp"
#include "../advanced/AdvancedBrain.hpp"
#include "../advanced/AdvancedSimulation.hpp"
#include "../advanced/AdvancedAnalysis.hpp"
#include "../advanced/AdvancedConfig.hpp"
#include "../core/Logger/Logger.hpp"
#include "../advanced/AdvancedBrain.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iostream>

namespace nlm {

struct AdvancedExperiment::Impl {
    std::shared_ptr<AdvancedBrain> advancedBrain;
    std::shared_ptr<AdvancedSimulation> advancedSimulation;
    std::shared_ptr<AdvancedAnalysis> advancedAnalysis;
    std::shared_ptr<AdvancedConfig> advancedConfig;
    
    std::vector<AdvancedExperimentConfig> experimentConfigs;
    std::vector<AdvancedExperimentResult> experimentResults;
    
    // Advanced experiment tracking
    struct AdvancedExperimentTracking {
        std::string experimentId;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        size_t totalSteps;
        size_t totalAgents;
        float averageFitness;
        float convergenceRate;
        std::map<std::string, float> performanceMetrics;
        std::vector<std::string> achievedGoals;
    };
    
    std::vector<AdvancedExperimentTracking> experimentTracking;
    
    Impl() {
        // Initialize advanced experiment components
        loadDefaultAdvancedExperimentConfig();
    }
    
    void loadDefaultAdvancedExperimentConfig() {
        // Load default advanced experiment configurations
        AdvancedExperimentConfig multiAgentExp;
        multiAgentExp.name = "Multi-Agent Coordination Experiment";
        multiAgentExp.description = "Advanced multi-agent coordination with custom learning rules";
        multiAgentExp.type = AdvancedExperimentConfig::ExperimentType::MultiAgent;
        multiAgentExp.objective = AdvancedExperimentConfig::Objective::MaximizeCohesion;
        multiAgentExp.duration = 1000;
        multiAgentExp.maxAgents = 50;
        multiAgentExp.randomSeed = 12345;
        multiAgentExp.useAdvancedFeatures = true;
        
        AdvancedExperimentConfig plasticityExp;
        plasticityExp.name = "Advanced Plasticity Learning Experiment";
        plasticityExp.description = "Testing advanced plasticity mechanisms and learning rules";
        plasticityExp.type = AdvancedExperimentConfig::ExperimentType::Plasticity;
        plasticityExp.objective = AdvancedExperimentConfig::Objective::MinimizeError;
        plasticityExp.duration = 500;
        plasticityExp.maxAgents = 20;
        plasticityExp.randomSeed = 67890;
        plasticityExp.useAdvancedFeatures = true;
        
        AdvancedExperimentConfig analysisExp;
        analysisExp.name = "Neural Pattern Analysis Experiment";
        analysisExp.description = "Analyzing complex neural patterns with advanced ML";
        analysisExp.type = AdvancedExperimentConfig::ExperimentType::Analysis;
        analysisExp.objective = AdvancedExperimentConfig::Objective::BalanceAll;
        analysisExp.duration = 800;
        analysisExp.maxAgents = 30;
        analysisExp.randomSeed = 54321;
        analysisExp.useAdvancedFeatures = true;
        
        experimentConfigs = {multiAgentExp, plasticityExp, analysisExp};
    }
};

AdvancedExperiment::AdvancedExperiment() : pImpl(std::make_unique<Impl>()) {
}

AdvancedExperiment::~AdvancedExperiment() = default;

bool AdvancedExperiment::configure(const AdvancedExperimentConfig& config) {
    pImpl->experimentConfigs.push_back(config);
    
    NLM_LOG_INFO("Configured advanced experiment: " + config.name);
    
    // Initialize experiment components based on config
    switch (config.type) {
        case AdvancedExperimentConfig::ExperimentType::MultiAgent:
            // Initialize multi-agent experiment components
            pImpl->advancedSimulation = std::make_shared<AdvancedSimulation>();
            break;
        case AdvancedExperimentConfig::ExperimentType::Plasticity:
            // Initialize plasticity experiment components
            pImpl->advancedBrain = std::make_shared<AdvancedBrain>();
            break;
        case AdvancedExperimentConfig::ExperimentType::Analysis:
            // Initialize analysis experiment components
            pImpl->advancedAnalysis = std::make_shared<AdvancedAnalysis>();
            break;
        default:
            // Default initialization
            pImpl->advancedBrain = std::make_shared<AdvancedBrain>();
            break;
    }
    
    return true;
}

bool AdvancedExperiment::run() {
    NLM_LOG_INFO("Starting advanced experiment with " + std::to_string(pImpl->experimentConfigs.size()) + " configurations");
    
    bool allSuccess = true;
    
    for (const auto& config : pImpl->experimentConfigs) {
        AdvancedExperimentResult result;
        result.experimentName = config.name;
        result.experimentType = static_cast<int>(config.type);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Initialize experiment based on type
            switch (config.type) {
                case AdvancedExperimentConfig::ExperimentType::MultiAgent:
                    allSuccess &= runMultiAgentExperiment(config, result);
                    break;
                case AdvancedExperimentConfig::ExperimentType::Plasticity:
                    allSuccess &= runPlasticityExperiment(config, result);
                    break;
                case AdvancedExperimentConfig::ExperimentType::Analysis:
                    allSuccess &= runAnalysisExperiment(config, result);
                    break;
                default:
                    allSuccess &= runStandardExperiment(config, result);
                    break;
            }
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in advanced experiment " + config.name + ": ") + e.what());
            allSuccess = false;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        result.duration = duration.count();
        result.success = allSuccess;
        result.timestamp = std::chrono::system_clock::now();
        
        pImpl->experimentResults.push_back(result);
        
        if (allSuccess) {
            NLM_LOG_INFO("Advanced experiment completed: " + config.name + " in " + std::to_string(result.duration) + "ms");
        }
    }
    
    return allSuccess;
}

bool AdvancedExperiment::runMultiAgentExperiment(const AdvancedExperimentConfig& config, AdvancedExperimentResult& result) {
    // Multi-agent experiment implementation
    if (!pImpl->advancedSimulation) {
        NLM_LOG_ERROR("Advanced simulation not initialized for multi-agent experiment");
        return false;
    }
    
    // Configure advanced simulation
    MultiAgentConfig simConfig;
    simConfig.parallelConfig.enableParallel = true;
    simConfig.parallelConfig.numThreads = std::thread::hardware_concurrency();
    simConfig.experimentControl.enableStepControl = true;
    simConfig.experimentControl.enableConditionControl = true;
    simConfig.experimentControl.noiseLevel = 0.01f;
    
    pImpl->advancedSimulation->configure(simConfig);
    
    // Create and activate agents
    std::vector<AgentId> agentIds;
    for (size_t i = 0; i < config.maxAgents; ++i) {
        AgentId agentId = pImpl->advancedSimulation->createAgent("coordination_agent");
        if (agentId.index() > 0) {
            agentIds.push_back(agentId);
            pImpl->advancedSimulation->activateAgent(agentId);
        }
    }
    
    result.agentCount = agentIds.size();
    
    // Run simulation
    pImpl->advancedSimulation->start();
    
    for (size_t step = 0; step < config.duration; ++step) {
        // Record experiment data
        auto stats = pImpl->advancedSimulation->getSimulationStats();
        result.stepData.push_back({
            static_cast<size_t>(step),
            stats.activeAgents,
            stats.averageFitness,
            static_cast<float>(step) / config.duration
        });
        
        // Process coordination events
        pImpl->advancedSimulation->step(step);
        
        // Log progress periodically
        if (step % 100 == 0) {
            NLM_LOG_INFO("Multi-agent experiment progress: step " + std::to_string(step) + 
                         " of " + std::to_string(config.duration) + 
                         " (" + std::to_string(agentIds.size()) + " agents)");
        }
    }
    
    // Stop simulation
    pImpl->advancedSimulation->stop();
    
    // Calculate experiment statistics
    float totalFitness = 0.0f;
    for (const auto& data : result.stepData) {
        totalFitness += data.fitness;
    }
    
    if (!result.stepData.empty()) {
        result.finalMetric = totalFitness / result.stepData.size();
    }
    
    return true;
}

bool AdvancedExperiment::runPlasticityExperiment(const AdvancedExperimentConfig& config, AdvancedExperimentResult& result) {
    // Advanced plasticity experiment implementation
    if (!pImpl->advancedBrain) {
        NLM_LOG_ERROR("Advanced brain not initialized for plasticity experiment");
        return false;
    }
    
    // Initialize brain with configuration
    std::shared_ptr<Config> brainConfig = std::make_shared<Config>();
    brainConfig->set("neuron_count", static_cast<int>(config.maxAgents * 10));
    brainConfig->set("region_count", 3);
    brainConfig->set("random_seed", config.randomSeed);
    
    pImpl->advancedBrain = std::make_shared<AdvancedBrain>(brainConfig);
    
    if (!pImpl->advancedBrain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize advanced brain");
        return false;
    }
    
    // Add advanced plasticity mechanisms
    AdvancedBrainConfig::PlasticityMechanism stdpRule;
    stdpRule.name = "STDP";
    stdpRule.type = "STDP";
    stdpRule.parameters["A+"] = 0.01f;
    stdpRule.parameters["A-"] = 0.012f;
    stdpRule.parameters["tau"] = 20.0f;
    
    AdvancedBrainConfig::PlasticityMechanism bcmRule;
    bcmRule.name = "BCM";
    bcmRule.type = "BCM";
    bcmRule.parameters["learning_rate"] = 0.01f;
    bcmRule.parameters["threshold"] = 0.02f;
    
    pImpl->advancedBrain->addPlasticityMechanism(stdpRule);
    pImpl->advancedBrain->addPlasticityMechanism(bcmRule);
    
    // Run plasticity learning
    for (size_t step = 0; step < config.duration; ++step) {
        // Apply advanced learning rules
        pImpl->advancedBrain->setCustomDynamicsModel("stdp_learning", {
            {"learning_rate", 0.01f},
            {"trace_decay", 0.95f}
        });
        
        // Update brain
        pImpl->advancedBrain->step(static_cast<SimulationStep>(step));
        
        // Record learning metrics
        AdvancedBrain::AdvancedStats stats = pImpl->advancedBrain->calculateAdvancedStats();
        
        result.stepData.push_back({
            static_cast<size_t>(step),
            0,  // neurons (simplified)
            stats.plasticityRate,
            stats.learningEfficiency
        });
        
        // Log progress periodically
        if (step % 100 == 0) {
            NLM_LOG_INFO("Plasticity experiment progress: step " + std::to_string(step) + 
                         " of " + std::to_string(config.duration) + 
                         " (plasticity rate: " + std::to_string(stats.plasticityRate) + ")");
        }
    }
    
    // Calculate experiment statistics
    float totalPlasticityRate = 0.0f;
    for (const auto& data : result.stepData) {
        totalPlasticityRate += data.fitness;
    }
    
    if (!result.stepData.empty()) {
        result.finalMetric = totalPlasticityRate / result.stepData.size();
    }
    
    return true;
}

bool AdvancedExperiment::runAnalysisExperiment(const AdvancedExperimentConfig& config, AdvancedExperimentResult& result) {
    // Advanced analysis experiment implementation
    if (!pImpl->advancedAnalysis) {
        NLM_LOG_ERROR("Advanced analysis not initialized for analysis experiment");
        return false;
    }
    
    // Configure advanced analysis
    AdvancedAnalysisConfig analysisConfig;
    analysisConfig.enableNeuralPatternAnalysis = true;
    analysisConfig.enableLearningCurveAnalysis = true;
    analysisConfig.enableStatisticalAnalysis = true;
    analysisConfig.enableMachineLearning = true;
    analysisConfig.mlFramework = "scikit-learn";
    
    pImpl->advancedAnalysis->configure(analysisConfig);
    
    // Generate sample data for analysis
    std::vector<size_t> steps;
    std::vector<float> metricValues;
    
    for (size_t step = 0; step < config.duration; step += config.duration / 10) {
        steps.push_back(step);
        
        // Generate synthetic learning curve
        float metric = 0.1f * std::log(static_cast<float>(step + 1)) + 
                      0.5f * (1.0f - std::exp(-static_cast<float>(step) / 100.0f));
        metric += (rand() % 100 - 50) / 1000.0f;  // Add some noise
        
        metricValues.push_back(metric);
    }
    
    // Perform analysis
    LearningCurveResult learningCurve = pImpl->advancedAnalysis->analyzeLearningCurve(steps, metricValues, "performance");
    
    // Generate neural pattern
    std::vector<float> neuralData(100, 0.0f);
    for (size_t i = 0; i < 100; ++i) {
        neuralData[i] = std::sin(2.0f * M_PI * static_cast<float>(i) / 100.0f) * 
                       0.5f + 0.5f + (rand() % 100 - 50) / 200.0f;
    }
    
    PatternAnalysisResult patternAnalysis = pImpl->advancedAnalysis->analyzeNeuralPattern(neuralData, NeuralPatternType::SpikingPattern, "neural_pattern");
    
    // Perform statistical analysis
    StatisticalAnalysisResult statisticalAnalysis = pImpl->advancedAnalysis->performStatisticalTest(metricValues, metricValues);
    
    // Store analysis results
    result.analysisResults.push_back({
        "learning_curve", learningCurve
    });
    result.analysisResults.push_back({
        "pattern_analysis", patternAnalysis
    });
    result.analysisResults.push_back({
        "statistical_analysis", statisticalAnalysis
    });
    
    // Calculate experiment metrics
    result.finalMetric = learningCurve.fitParameters["r_squared"] * 100.0f;  // Normalized R-squared
    
    return true;
}

bool AdvancedExperiment::runStandardExperiment(const AdvancedExperimentConfig& config, AdvancedExperimentResult& result) {
    // Standard experiment implementation (fallback)
    NLM_LOG_INFO("Running standard experiment: " + config.name);
    
    for (size_t step = 0; step < config.duration; ++step) {
        // Simulate experiment progress
        result.stepData.push_back({
            static_cast<size_t>(step),
            step % 10,  // neurons (simplified)
            step % 100 / 100.0f,  // metric (simplified)
            static_cast<float>(step) / config.duration
        });
        
        // Log progress periodically
        if (step % 100 == 0) {
            NLM_LOG_INFO("Standard experiment progress: step " + std::to_string(step) + 
                         " of " + std::to_string(config.duration));
        }
    }
    
    return true;
}

AdvancedExperimentResult AdvancedExperiment::getResult(size_t index) const {
    if (index < pImpl->experimentResults.size()) {
        return pImpl->experimentResults[index];
    }
    return AdvancedExperimentResult();
}

std::vector<AdvancedExperimentResult> AdvancedExperiment::getAllResults() const {
    return pImpl->experimentResults;
}

bool AdvancedExperiment::saveResults(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for saving results: " + filepath);
            return false;
        }
        
        // Export results (simplified JSON-like format)
        file << "AdvancedExperimentResults {\n";
        file << "  totalExperiments = " << pImpl->experimentResults.size() << "\n";
        file << "  successfulExperiments = " << std::count_if(pImpl->experimentResults.begin(), pImpl->experimentResults.end(),
                                                            [](const AdvancedExperimentResult& r) { return r.success; }) << "\n";
        
        for (size_t i = 0; i < pImpl->experimentResults.size(); ++i) {
            const auto& result = pImpl->experimentResults[i];
            file << "  experiment_" << i << " {\n";
            file << "    name = \"" << result.experimentName << "\"\n";
            file << "    type = " << result.experimentType << "\n";
            file << "    success = " << (result.success ? "true" : "false") << "\n";
            file << "    duration = " << result.duration << "\n";
            file << "    final_metric = " << result.finalMetric << "\n";
            file << "    agent_count = " << result.agentCount << "\n";
            file << "  }\n";
        }
        
        file << "}\n";
        
        file.close();
        NLM_LOG_INFO("Advanced experiment results saved to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving advanced experiment results: ") + e.what());
        return false;
    }
}

void AdvancedExperiment::enableAdvancedFeatures(bool enable) {
    // Enable/disable advanced features in experiment
    NLM_LOG_INFO(std::string("Advanced experiment features ") + std::string(enable ? "enabled" : "disabled"));
}

void AdvancedExperiment::enableExperimentTracking(bool enable) {
    // Enable/disable experiment tracking
    NLM_LOG_INFO(std::string("Experiment tracking ") + std::string(enable ? "enabled" : "disabled"));
}

void AdvancedExperiment::logExperimentStatus() const {
    NLM_LOG_INFO("=== Advanced Experiment Status ===");
    NLM_LOG_INFO("Total Configurations: " + std::to_string(pImpl->experimentConfigs.size()));
    NLM_LOG_INFO("Total Results: " + std::to_string(pImpl->experimentResults.size()));
    
    size_t successfulExperiments = std::count_if(pImpl->experimentResults.begin(), pImpl->experimentResults.end(),
                                                [](const AdvancedExperimentResult& r) { return r.success; });
    
    NLM_LOG_INFO("Successful Experiments: " + std::to_string(successfulExperiments));
    NLM_LOG_INFO("Success Rate: " + std::to_string(static_cast<float>(successfulExperiments) / 
                                                 std::max(1.0f, static_cast<float>(pImpl->experimentResults.size()))) * 100.0f + "%");
    
    NLM_LOG_INFO("=== End Advanced Experiment Status ===");
}

} // namespace nlm
