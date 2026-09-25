#include "EnhancedFramework.hpp"
#include "EnhancedExperiment.hpp"
#include "EnhancedMetrics.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct EnhancedExperimentRunner::Impl {
    std::vector<std::shared_ptr<EnhancedExperiment>> experiments;
    std::shared_ptr<ExperimentValidation> validationFramework;
    std::shared_ptr<MultiAgentRunner> multiAgentRunner;
    std::shared_ptr<ParameterTuner> parameterTuner;
};

EnhancedExperimentRunner::EnhancedExperimentRunner() : pImpl(new Impl) {}

EnhancedExperimentRunner::~EnhancedExperimentRunner() = default;

std::shared_ptr<EnhancedExperiment> EnhancedExperimentRunner::createExperiment(const std::string& name, const std::string& templateName) {
    auto experiment = std::make_shared<EnhancedExperiment>(name);
    experiment->setTemplate(templateName);
    pImpl->experiments.push_back(experiment);
    return experiment;
}

bool EnhancedExperimentRunner::configureExperiment(std::shared_ptr<EnhancedExperiment> experiment,
                                                   const std::unordered_map<std::string, std::string>& parameters) {
    if (!experiment) return false;
    
    // Apply parameters
    for (const auto& pair : parameters) {
        experiment->setParameter(pair.first, pair.second);
    }
    
    return true;
}

bool EnhancedExperimentRunner::runExperiment(std::shared_ptr<EnhancedExperiment> experiment) {
    if (!experiment) return false;
    
    NLM_LOG_INFO("Running enhanced experiment: " + experiment->getName());
    
    experiment->initialize();
    experiment->start();
    
    // Run experiment steps (simplified)
    for (size_t step = 0; step < 100; ++step) {
        if (!experiment->isRunning()) {
            break;
        }
        
        // Simulate experiment execution
        EnhancedExperiment::EnhancedStepData stepData;
        stepData.step = step;
        stepData.neuronCount = 100;
        stepData.synapseCount = 500;
        stepData.firingCount = 45;
        stepData.avgFiringRate = 1.2;
        stepData.reward = 0.1 * (step % 10 + 1);
        
        experiment->recordStep(stepData);
        
        // Record some metrics
        experiment->recordMetric("performance", 0.5 + (step / 100.0) * 0.5, "performance");
        experiment->recordMetric("accuracy", 0.6 + (step / 100.0) * 0.4, "accuracy");
        experiment->recordMetric("efficiency", 0.7 + (step / 100.0) * 0.3, "efficiency");
        
        // Record template-specific metrics
        if (experiment->getTemplate() == "memory_integration") {
            experiment->recordMetric("memory_retention", 0.8 + (step / 100.0) * 0.2, "memory");
        } else if (experiment->getTemplate() == "prediction_system") {
            experiment->recordMetric("prediction_error", 0.4 - (step / 100.0) * 0.3, "prediction");
        } else if (experiment->getTemplate() == "cognitive_benchmark") {
            experiment->recordMetric("cognition_score", 0.6 + (step / 100.0) * 0.4, "cognition");
        }
    }
    
    experiment->complete();
    
    NLM_LOG_INFO("Enhanced experiment completed: " + experiment->getName());
    return true;
}

std::vector<std::shared_ptr<EnhancedExperiment>> EnhancedExperimentRunner::runBatch(const std::vector<std::shared_ptr<EnhancedExperiment>>& experiments) {
    std::vector<std::shared_ptr<EnhancedExperiment>> results;
    
    for (const auto& experiment : experiments) {
        if (experiment) {
            if (runExperiment(experiment)) {
                results.push_back(experiment);
            }
        }
    }
    
    return results;
}

std::shared_ptr<EnhancedExperiment> EnhancedExperimentRunner::runMultiAgentExperiment(const std::vector<std::string>& templateNames,
                                                                                     const std::unordered_map<std::string, std::string>& globalParams) {
    if (templateNames.empty()) return nullptr;
    
    // Create agents
    std::vector<std::shared_ptr<EnhancedExperiment>> agents;
    for (const auto& templateName : templateNames) {
        auto experiment = createExperiment("Agent_" + templateName, templateName);
        if (experiment) {
            configureExperiment(experiment, globalParams);
            agents.push_back(experiment);
        }
    }
    
    if (agents.empty()) return nullptr;
    
    // Run multi-agent experiment
    if (pImpl->multiAgentRunner) {
        auto result = pImpl->multiAgentRunner->runMultiAgent(agents, globalParams);
        if (!result.empty()) {
            return result[0]; // Return first agent as main result
        }
    }
    
    // Fallback to running each agent separately
    runBatch(agents);
    return agents.empty() ? nullptr : agents[0];
}

bool EnhancedExperimentRunner::saveResults(const std::string& filepath,
                                           const std::vector<std::shared_ptr<EnhancedExperiment>>& experiments) {
    try {
        json j;
        
        json experimentsJson = json::array();
        for (const auto& experiment : experiments) {
            if (experiment) {
                json expJson;
                expJson["name"] = experiment->getName();
                expJson["description"] = experiment->getDescription();
                expJson["state"] = static_cast<int>(experiment->getState());
                expJson["duration"] = experiment->getDuration();
                expJson["template"] = experiment->getTemplate();
                
                // Save metrics
                json metricsJson = json::array();
                const auto& metrics = experiment->getMetrics();
                for (const auto& pair : metrics) {
                    json metricJson;
                    metricJson["name"] = pair.first;
                    metricJson["value"] = pair.second;
                    metricsJson.push_back(metricJson);
                }
                expJson["metrics"] = metricsJson;
                
                // Save step data
                json stepsJson = json::array();
                const auto& steps = experiment->getStepData();
                for (const auto& step : steps) {
                    json stepJson;
                    stepJson["step"] = step.step;
                    stepJson["neuronCount"] = step.neuronCount;
                    stepJson["synapseCount"] = step.synapseCount;
                    stepJson["firingCount"] = step.firingCount;
                    stepJson["avgFiringRate"] = step.avgFiringRate;
                    stepJson["reward"] = step.reward;
                    stepsJson.push_back(stepJson);
                }
                expJson["steps"] = stepsJson;
                
                experimentsJson.push_back(expJson);
            }
        }
        
        j["experiments"] = experimentsJson;
        
        // Write to file
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << j.dump(2);
            file.close();
            NLM_LOG_INFO("Experiment results saved to " + filepath);
            return true;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save experiment results: " + std::string(e.what()));
    }
    
    return false;
}

std::vector<std::shared_ptr<EnhancedExperiment>> EnhancedExperimentRunner::loadResults(const std::string& filepath) {
    std::vector<std::shared_ptr<EnhancedExperiment>> experiments;
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open experiment results file: " + filepath);
            return experiments;
        }
        
        json j;
        file >> j;
        
        for (const auto& expJson : j["experiments"]) {
            auto experiment = std::make_shared<EnhancedExperiment>(expJson.value("name", ""));
            experiment->setDescription(expJson.value("description", ""));
            
            // Load metrics
            if (expJson.contains("metrics")) {
                for (const auto& metricJson : expJson["metrics"]) {
                    std::string name = metricJson.value("name", "");
                    double value = metricJson.value("value", 0.0);
                    experiment->recordMetric(name, value);
                }
            }
            
            experiments.push_back(experiment);
        }
        
        file.close();
        NLM_LOG_INFO("Experiment results loaded from " + filepath);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load experiment results: " + std::string(e.what()));
    }
    
    return experiments;
}

std::string EnhancedExperimentRunner::generateComprehensiveReport(const std::vector<std::shared_ptr<EnhancedExperiment>>& experiments) const {
    std::stringstream ss;
    ss << "Comprehensive Experiment Report" << std::endl;
    ss << "==============================" << std::endl << std::endl;
    
    ss << "Total Experiments: " << experiments.size() << std::endl;
    ss << "Experiment Types:" << std::endl;
    
    std::unordered_map<std::string, int> templateCounts;
    for (const auto& experiment : experiments) {
        if (experiment) {
            templateCounts[experiment->getTemplate()]++;
        }
    }
    
    for (const auto& pair : templateCounts) {
        ss << "  " << pair.first << ": " << pair.second << std::endl;
    }
    
    ss << std::endl << "Experiment Details:" << std::endl;
    for (size_t i = 0; i < experiments.size(); ++i) {
        const auto& experiment = experiments[i];
        if (experiment) {
            ss << "Experiment " << (i + 1) << ": " << experiment->getName() << std::endl;
            ss << "  Description: " << experiment->getDescription() << std::endl;
            ss << "  Template: " << experiment->getTemplate() << std::endl;
            ss << "  State: " << static_cast<int>(experiment->getState()) << std::endl;
            ss << "  Duration: " << experiment->getDuration() << " seconds" << std::endl;
            ss << "  Metrics: " << experiment->getMetrics().size() << std::endl;
            ss << "  Steps: " << experiment->getStepData().size() << std::endl << std::endl;
        }
    }
    
    return ss.str();
}

std::string EnhancedExperimentRunner::generateMetaAnalysis(const std::vector<std::shared_ptr<EnhancedExperiment>>& experiments) const {
    std::stringstream ss;
    ss << "Meta-Analysis of Experiments" << std::endl;
    ss << "===========================" << std::endl << std::endl;
    
    if (experiments.empty()) {
        ss << "No experiments to analyze." << std::endl;
        return ss.str();
    }
    
    // Calculate overall statistics
    double totalDuration = 0.0;
    int totalMetrics = 0;
    int totalSteps = 0;
    
    for (const auto& experiment : experiments) {
        if (experiment) {
            totalDuration += experiment->getDuration();
            totalMetrics += experiment->getMetrics().size();
            totalSteps += experiment->getStepData().size();
        }
    }
    
    ss << "Overall Statistics:" << std::endl;
    ss << "  Total Duration: " << totalDuration << " seconds" << std::endl;
    ss << "  Total Metrics: " << totalMetrics << std::endl;
    ss << "  Total Steps: " << totalSteps << std::endl;
    ss << "  Average Duration: " << (totalDuration / experiments.size()) << " seconds" << std::endl;
    ss << "  Average Metrics per Experiment: " << (totalMetrics / experiments.size()) << std::endl;
    ss << "  Average Steps per Experiment: " << (totalSteps / experiments.size()) << std::endl << std::endl;
    
    // Template performance analysis
    ss << "Template Performance:" << std::endl;
    std::unordered_map<std::string, std::vector<double>> templateMetrics;
    
    for (const auto& experiment : experiments) {
        if (experiment) {
            const auto& metrics = experiment->getMetrics();
            for (const auto& pair : metrics) {
                templateMetrics[experiment->getTemplate()].push_back(pair.second);
            }
        }
    }
    
    for (const auto& pair : templateMetrics) {
        double avg = std::accumulate(pair.second.begin(), pair.second.end(), 0.0) / pair.second.size();
        double min = *std::min_element(pair.second.begin(), pair.second.end());
        double max = *std::max_element(pair.second.begin(), pair.second.end());
        
        ss << "  " << pair.first << ": avg=" << avg << ", min=" << min << ", max=" << max << std::endl;
    }
    
    return ss.str();
}

bool EnhancedExperimentRunner::validateExperiment(const std::shared_ptr<EnhancedExperiment>& experiment) {
    if (!pImpl->validationFramework || !experiment) return false;
    
    return pImpl->validationFramework->validateExperiment(experiment);
}

std::unordered_map<std::string, std::string> EnhancedExperimentRunner::tuneParameters(const std::shared_ptr<EnhancedExperiment>& experiment,
                                                                                      const std::string& tuningMethod) {
    if (!pImpl->parameterTuner || !experiment) return {};
    
    std::unordered_map<std::string, std::string> currentParams;
    // Get current parameters from experiment
    auto paramKeys = experiment->getParameterKeys();
    for (const auto& key : paramKeys) {
        currentParams[key] = experiment->getParameter(key);
    }
    
    if (tuningMethod == "bayesian") {
        return pImpl->parameterTuner->bayesianOptimization(experiment, currentParams);
    } else if (tuningMethod == "evolutionary") {
        return pImpl->parameterTuner->evolutionaryOptimization(experiment, currentParams);
    } else if (tuningMethod == "grid") {
        // For grid search, need parameter grid
        std::unordered_map<std::string, std::vector<std::string>> grid;
        for (const auto& pair : currentParams) {
            grid[pair.first] = {pair.second};
        }
        return pImpl->parameterTuner->gridSearch(experiment, grid);
    } else {
        return currentParams; // No tuning
    }
}

void EnhancedExperimentRunner::clearExperiments() {
    pImpl->experiments.clear();
}

size_t EnhancedExperimentRunner::getExperimentCount() const {
    return pImpl->experiments.size();
}

std::vector<std::string> EnhancedExperimentRunner::getExperimentNames() const {
    std::vector<std::string> names;
    for (const auto& experiment : pImpl->experiments) {
        if (experiment) {
            names.push_back(experiment->getName());
        }
    }
    return names;
}

void EnhancedExperimentRunner::setValidationFramework(const std::shared_ptr<ExperimentValidation>& validation) {
    pImpl->validationFramework = validation;
}

void EnhancedExperimentRunner::setMultiAgentRunner(const std::shared_ptr<MultiAgentRunner>& multiAgentRunner) {
    pImpl->multiAgentRunner = multiAgentRunner;
}

void EnhancedExperimentRunner::setParameterTuner(const std::shared_ptr<ParameterTuner>& parameterTuner) {
    pImpl->parameterTuner = parameterTuner;
}

} // namespace nlm
