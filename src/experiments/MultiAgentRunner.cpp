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

struct ExperimentValidation::Impl {
    std::vector<ValidationRule> rules;
    bool strictMode;
    bool autoFix;
    double confidenceThreshold;
};

ExperimentValidation::ExperimentValidation() : pImpl(new Impl) {
    pImpl->strictMode = false;
    pImpl->autoFix = false;
    pImpl->confidenceThreshold = 0.7;
    
    // Initialize with default validation rules
    addValidationRule({
        "basic_validation",
        "completeness",
        [](const std::shared_ptr<EnhancedExperiment>& exp) {
            return !exp->getName().empty() && 
                   exp->getState() != ExperimentState::UNINITIALIZED;
        },
        {"Experiment must have a name", "Experiment must be initialized"},
        0.5
    });
    
    addValidationRule({
        "metric_validation",
        "performance",
        [](const std::shared_ptr<EnhancedExperiment>& exp) {
            return !exp->getMetrics().empty();
        },
        {"Experiment must record metrics"},
        0.8
    });
    
    addValidationRule({
        "step_validation",
        "integration",
        [](const std::shared_ptr<EnhancedExperiment>& exp) {
            return !exp->getStepData().empty();
        },
        {"Experiment must record step data"},
        0.6
    });
}

ExperimentValidation::~ExperimentValidation() = default;

bool ExperimentValidation::validateExperiment(const std::shared_ptr<EnhancedExperiment>& experiment) {
    if (!experiment) return false;
    
    auto result = validate(experiment);
    for (const auto& res : result) {
        if (res.category == "completeness" && !res.passed) {
            return false;
        }
    }
    return true;
}

bool ExperimentValidation::validateIntegration(const std::shared_ptr<EnhancedExperiment>& experiment) {
    if (!experiment) return false;
    
    // Check for key system integrations
    bool memoryIntegrated = experiment->hasMetric("working_memory_integrated") && 
                          experiment->getMetric("working_memory_integrated") > 0.5;
    bool predictionIntegrated = experiment->hasMetric("prediction_integrated") && 
                              experiment->getMetric("prediction_integrated") > 0.5;
    bool neuromodulationIntegrated = experiment->hasMetric("dopamine_level") && 
                                   experiment->getMetric("dopamine_level") > 0.0;
    
    return memoryIntegrated && predictionIntegrated && neuromodulationIntegrated;
}

bool ExperimentValidation::validatePerformance(const std::shared_ptr<EnhancedExperiment>& experiment) {
    if (!experiment) return false;
    
    // Check performance metrics
    auto metrics = experiment->getMetrics();
    if (metrics.empty()) return false;
    
    // Calculate average performance
    double totalScore = 0.0;
    int scoreCount = 0;
    
    for (const auto& pair : metrics) {
        if (pair.first.find("accuracy") != std::string::npos ||
            pair.first.find("success") != std::string::npos ||
            pair.first.find("efficiency") != std::string::npos) {
            totalScore += pair.second;
            scoreCount++;
        }
    }
    
    double avgScore = scoreCount > 0 ? totalScore / scoreCount : 0.0;
    return avgScore >= 0.5; // 50% or better performance
}

bool ExperimentValidation::validateResults(const std::shared_ptr<EnhancedExperiment>& experiment) {
    if (!experiment) return false;
    
    // Check result consistency and quality
    bool hasResults = !experiment->getMetrics().empty() && 
                     !experiment->getStepData().empty();
    
    bool hasReasonableValues = true;
    for (const auto& metric : experiment->getMetrics()) {
        if (std::isnan(metric.second) || std::isinf(metric.second)) {
            hasReasonableValues = false;
            break;
        }
    }
    
    return hasResults && hasReasonableValues;
}

std::vector<ExperimentValidation::ValidationResult> ExperimentValidation::validate(const std::shared_ptr<EnhancedExperiment>& experiment) {
    std::vector<ValidationResult> results;
    
    for (const auto& rule : pImpl->rules) {
        ValidationResult result;
        result.category = rule.category;
        result.passed = rule.check(experiment);
        result.confidenceScore = rule.weight;
        
        if (!result.passed && !rule.errorMessages.empty()) {
            result.errors = rule.errorMessages;
        }
        
        results.push_back(result);
    }
    
    return results;
}

void ExperimentValidation::addValidationRule(const ValidationRule& rule) {
    pImpl->rules.push_back(rule);
}

void ExperimentValidation::removeValidationRule(const std::string& name) {
    auto it = std::remove_if(pImpl->rules.begin(), pImpl->rules.end(),
                            [&name](const ValidationRule& rule) {
                                return rule.name == name;
                            });
    pImpl->rules.erase(it, pImpl->rules.end());
}

void ExperimentValidation::clearValidationRules() {
    pImpl->rules.clear();
}

void ExperimentValidation::setStrictMode(bool strict) {
    pImpl->strictMode = strict;
}

void ExperimentValidation::setAutoFix(bool autoFix) {
    pImpl->autoFix = autoFix;
}

void ExperimentValidation::setConfidenceThreshold(double threshold) {
    pImpl->confidenceThreshold = threshold;
}

std::string ExperimentValidation::generateValidationReport(const std::shared_ptr<EnhancedExperiment>& experiment) const {
    std::stringstream ss;
    ss << "Experiment Validation Report" << std::endl;
    ss << "==========================" << std::endl << std::endl;
    
    auto results = const_cast<ExperimentValidation*>(this)->validate(experiment);
    
    for (const auto& result : results) {
        ss << "Category: " << result.category << std::endl;
        ss << "  Status: " << (result.passed ? "PASS" : "FAIL") << std::endl;
        ss << "  Confidence: " << (result.confidenceScore * 100) << "%" << std::endl;
        if (!result.errors.empty()) {
            ss << "  Errors:" << std::endl;
            for (const auto& error : result.errors) {
                ss << "    - " << error << std::endl;
            }
        }
        ss << std::endl;
    }
    
    double overallScore = getOverallScore(experiment);
    ss << "Overall Score: " << (overallScore * 100) << "%" << std::endl;
    ss << "Recommendation: " << (overallScore >= pImpl->confidenceThreshold ? "EXPERIMENT READY" : "EXPERIMENT NEEDS IMPROVEMENT") << std::endl;
    
    return ss.str();
}

std::string ExperimentValidation::generateValidationJSON(const std::shared_ptr<EnhancedExperiment>& experiment) const {
    json j;
    
    auto results = const_cast<ExperimentValidation*>(this)->validate(experiment);
    
    json validationResults = json::array();
    for (const auto& result : results) {
        json resultJson;
        resultJson["category"] = result.category;
        resultJson["passed"] = result.passed;
        resultJson["confidenceScore"] = result.confidenceScore;
        
        if (!result.errors.empty()) {
            resultJson["errors"] = result.errors;
        }
        
        if (!result.warnings.empty()) {
            resultJson["warnings"] = result.warnings;
        }
        
        validationResults.push_back(resultJson);
    }
    
    j["validationResults"] = validationResults;
    j["overallScore"] = getOverallScore(experiment);
    j["experimentName"] = experiment->getName();
    
    return j.dump(2);
}

std::string ExperimentValidation::generateValidationCSV(const std::shared_ptr<EnhancedExperiment>& experiment) const {
    std::stringstream ss;
    ss << "Category,Passed,ConfidenceScore,Errors" << std::endl;
    
    auto results = const_cast<ExperimentValidation*>(this)->validate(experiment);
    
    for (const auto& result : results) {
        ss << result.category << "," 
           << (result.passed ? "true" : "false") << "," 
           << result.confidenceScore << ",";
        
        if (!result.errors.empty()) {
            ss << "\"";
            for (size_t i = 0; i < result.errors.size(); ++i) {
                if (i > 0) ss << ";";
                ss << result.errors[i];
            }
            ss << "\"";
        }
        
        ss << std::endl;
    }
    
    return ss.str();
}

double ExperimentValidation::getOverallScore(const std::shared_ptr<EnhancedExperiment>& experiment) const {
    if (!experiment) return 0.0;
    
    auto results = const_cast<ExperimentValidation*>(this)->validate(experiment);
    if (results.empty()) return 0.0;
    
    double totalScore = 0.0;
    for (const auto& result : results) {
        totalScore += result.passed ? result.confidenceScore : 0.0;
    }
    
    return totalScore / results.size();
}

std::vector<std::string> ExperimentValidation::getPassedCategories(const std::shared_ptr<EnhancedExperiment>& experiment) const {
    std::vector<std::string> categories;
    auto results = const_cast<ExperimentValidation*>(this)->validate(experiment);
    
    for (const auto& result : results) {
        if (result.passed) {
            categories.push_back(result.category);
        }
    }
    
    return categories;
}

std::vector<std::string> ExperimentValidation::getFailedCategories(const std::shared_ptr<EnhancedExperiment>& experiment) const {
    std::vector<std::string> categories;
    auto results = const_cast<ExperimentValidation*>(this)->validate(experiment);
    
    for (const auto& result : results) {
        if (!result.passed) {
            categories.push_back(result.category);
        }
    }
    
    return categories;
}

struct MultiAgentRunner::Impl {
    std::unordered_map<std::string, std::shared_ptr<EnhancedExperiment>> agents;
    std::string interactionModel;
    std::unordered_map<std::string, double> socialLearningRules;
    std::unordered_map<std::string, double> competitiveParameters;
};

MultiAgentRunner::MultiAgentRunner() : pImpl(new Impl) {
    pImpl->interactionModel = "social_learning";
}

MultiAgentRunner::~MultiAgentRunner() = default;

bool MultiAgentRunner::addAgent(const std::shared_ptr<EnhancedExperiment>& agent) {
    if (!agent) return false;
    
    pImpl->agents[agent->getName()] = agent;
    return true;
}

bool MultiAgentRunner::removeAgent(const std::string& agentName) {
    auto it = pImpl->agents.find(agentName);
    if (it != pImpl->agents.end()) {
        pImpl->agents.erase(it);
        return true;
    }
    return false;
}

bool MultiAgentRunner::clearAgents() {
    pImpl->agents.clear();
    return true;
}

std::vector<std::shared_ptr<EnhancedExperiment>> MultiAgentRunner::runMultiAgent(const std::vector<std::shared_ptr<EnhancedExperiment>>& agents,
                                                                              const std::unordered_map<std::string, std::string>& sharedParams) {
    std::vector<std::shared_ptr<EnhancedExperiment>> results;
    
    for (const auto& agent : agents) {
        if (agent) {
            if (configureAgentFromParams(agent, sharedParams)) {
                if (runAgent(agent)) {
                    results.push_back(agent);
                }
            }
        }
    }
    
    return results;
}

std::vector<std::shared_ptr<EnhancedExperiment>> MultiAgentRunner::runSocialLearning(const std::vector<std::shared_ptr<EnhancedExperiment>>& agents) {
    std::vector<std::shared_ptr<EnhancedExperiment>> results;
    
    if (agents.size() < 2) return results;
    
    // Run all agents
    for (const auto& agent : agents) {
        if (agent) {
            runAgent(agent);
        }
    }
    
    // Simulate social learning between agents
    simulateLearningBetweenAgents(agents);
    
    return agents;
}

std::vector<std::shared_ptr<EnhancedExperiment>> MultiAgentRunner::runCompetitive(const std::vector<std::shared_ptr<EnhancedExperiment>>& agents) {
    std::vector<std::shared_ptr<EnhancedExperiment>> results;
    
    if (agents.size() < 2) return results;
    
    // Run all agents in competition
    for (const auto& agent : agents) {
        if (agent) {
            runAgent(agent);
        }
    }
    
    // Determine winners and losers
    determineCompetitiveOutcomes(agents);
    
    return agents;
}

bool MultiAgentRunner::simulateAgentInteraction(const std::shared_ptr<EnhancedExperiment>& agent1,
                                               const std::shared_ptr<EnhancedExperiment>& agent2,
                                               const std::unordered_map<std::string, std::string>& interactionParams) {
    if (!agent1 || !agent2) return false;
    
    // Create interaction metrics
    double interactionQuality = 0.5;
    if (agent1->hasMetric("performance") && agent2->hasMetric("performance")) {
        interactionQuality = std::abs(agent1->getMetric("performance") - agent2->getMetric("performance"));
    }
    
    // Apply social learning if enabled
    if (pImpl->socialLearningRules.find("imitation_rate") != pImpl->socialLearningRules.end()) {
        double imitationRate = pImpl->socialLearningRules["imitation_rate"];
        
        if ((rand() % 100) < imitationRate * 100) {
            // Agent2 imitates Agent1's successful strategies
            double agent1Performance = agent1->hasMetric("performance") ? agent1->getMetric("performance") : 0.0;
            if (agent2->hasMetric("performance")) {
                double agent2Performance = agent2->getMetric("performance");
                if (agent1Performance > agent2Performance) {
                    // Update Agent2's parameters based on Agent1's success
                    updateAgentParameters(agent2, agent1, interactionParams);
                }
            }
        }
    }
    
    return true;
}

bool MultiAgentRunner::simulateSocialLearning(const std::shared_ptr<EnhancedExperiment>& observer,
                                             const std::shared_ptr<EnhancedExperiment>& model) {
    if (!observer || !model) return false;
    
    // Observer learns from model
    if (observer->hasMetric("learning_rate") && model->hasMetric("performance")) {
        double modelPerformance = model->getMetric("performance");
        double learningRate = observer->getMetric("learning_rate");
        
        // Observer improves based on model's performance
        double observerImprovement = learningRate * modelPerformance * 0.1;
        
        // Update observer's metrics
        std::string metricName = "social_learning_improvement";
        if (!observer->hasMetric(metricName)) {
            observer->recordMetric(metricName, observerImprovement, "social_learning");
        } else {
            double currentValue = observer->getMetric(metricName);
            observer->recordMetric(metricName, currentValue + observerImprovement, "social_learning");
        }
    }
    
    return true;
}

bool MultiAgentRunner::simulateCooperativeBehavior(const std::vector<std::shared_ptr<EnhancedExperiment>>& agents) {
    if (agents.size() < 2) return false;
    
    // Calculate cooperation level
    double totalCooperation = 0.0;
    int cooperationCount = 0;
    
    for (const auto& agent : agents) {
        if (agent && agent->hasMetric("cooperation_level")) {
            totalCooperation += agent->getMetric("cooperation_level");
            cooperationCount++;
        }
    }
    
    double avgCooperation = cooperationCount > 0 ? totalCooperation / cooperationCount : 0.0;
    
    // Update shared environment based on cooperation
    if (avgCooperation > 0.5) {
        // High cooperation benefits all agents
        for (const auto& agent : agents) {
            if (agent) {
                double performanceBoost = avgCooperation * 0.1; // 10% boost per cooperation point
                std::string metricName = "cooperative_bonus";
                if (!agent->hasMetric(metricName)) {
                    agent->recordMetric(metricName, performanceBoost, "cooperation");
                }
            }
        }
    }
    
    return true;
}

MultiAgentRunner::MultiAgentMetrics MultiAgentRunner::computeMultiAgentMetrics(const std::vector<std::shared_ptr<EnhancedExperiment>>& agents) {
    MultiAgentMetrics metrics;
    
    if (agents.empty()) return metrics;
    
    double totalPerformance = 0.0;
    double totalSocialLearning = 0.0;
    double totalCompetitivePressure = 0.0;
    double totalCooperation = 0.0;
    double totalCulturalSpread = 0.0;
    int count = 0;
    
    for (const auto& agent : agents) {
        if (agent) {
            totalPerformance += agent->hasMetric("performance") ? agent->getMetric("performance") : 0.0;
            totalSocialLearning += agent->hasMetric("social_learning_effect") ? agent->getMetric("social_learning_effect") : 0.0;
            totalCompetitivePressure += agent->hasMetric("competitive_pressure") ? agent->getMetric("competitive_pressure") : 0.0;
            totalCooperation += agent->hasMetric("cooperation_level") ? agent->getMetric("cooperation_level") : 0.0;
            totalCulturalSpread += agent->hasMetric("cultural_spread") ? agent->getMetric("cultural_spread") : 0.0;
            
            if (agent->hasMetric("performance") || agent->hasMetric("social_learning_effect") ||
                agent->hasMetric("competitive_pressure") || agent->hasMetric("cooperation_level") ||
                agent->hasMetric("cultural_spread")) {
                count++;
            }
        }
    }
    
    if (count > 0) {
        metrics.averagePerformance = totalPerformance / count;
        metrics.socialLearningEffect = totalSocialLearning / count;
        metrics.competitivePressure = totalCompetitivePressure / count;
        metrics.cooperationLevel = totalCooperation / count;
        metrics.culturalSpread = totalCulturalSpread / count;
    }
    
    return metrics;
}

std::vector<std::string> MultiAgentRunner::getAgentNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->agents) {
        names.push_back(pair.first);
    }
    return names;
}

std::shared_ptr<EnhancedExperiment> MultiAgentRunner::getAgent(const std::string& name) const {
    auto it = pImpl->agents.find(name);
    if (it != pImpl->agents.end()) {
        return it->second;
    }
    return nullptr;
}

bool MultiAgentRunner::hasAgent(const std::string& name) const {
    return pImpl->agents.find(name) != pImpl->agents.end();
}

void MultiAgentRunner::setAgentInteractionModel(const std::string& model) {
    pImpl->interactionModel = model;
}

void MultiAgentRunner::setSocialLearningRules(const std::unordered_map<std::string, double>& rules) {
    pImpl->socialLearningRules = rules;
}

void MultiAgentRunner::setCompetitiveParameters(const std::unordered_map<std::string, double>& params) {
    pImpl->competitiveParameters = params;
}

std::string MultiAgentRunner::generateMultiAgentReport(const std::vector<std::shared_ptr<EnhancedExperiment>>& agents) const {
    std::stringstream ss;
    ss << "Multi-Agent Experiment Report" << std::endl;
    ss << "===========================" << std::endl << std::endl;
    
    ss << "Number of Agents: " << agents.size() << std::endl;
    ss << "Interaction Model: " << pImpl->interactionModel << std::endl << std::endl;
    
    auto metrics = const_cast<MultiAgentRunner*>(this)->computeMultiAgentMetrics(agents);
    
    ss << "Multi-Agent Metrics:" << std::endl;
    ss << "  Average Performance: " << metrics.averagePerformance << std::endl;
    ss << "  Social Learning Effect: " << metrics.socialLearningEffect << std::endl;
    ss << "  Competitive Pressure: " << metrics.competitivePressure << std::endl;
    ss << "  Cooperation Level: " << metrics.cooperationLevel << std::endl;
    ss << "  Cultural Spread: " << metrics.culturalSpread << std::endl;
    ss << "  Knowledge Transfer: " << metrics.knowledgeTransfer << std::endl << std::endl;
    
    ss << "Agent Performance Summary:" << std::endl;
    for (const auto& agent : agents) {
        if (agent) {
            ss << "  " << agent->getName() << ": " << (agent->hasMetric("performance") ? agent->getMetric("performance") : 0.0) << std::endl;
        }
    }
    
    return ss.str();
}

std::string MultiAgentRunner::generateSocialLearningReport(const std::vector<std::shared_ptr<EnhancedExperiment>>& agents) const {
    std::stringstream ss;
    ss << "Social Learning Report" << std::endl;
    ss << "====================" << std::endl << std::endl;
    
    double totalImitation = 0.0;
    double totalObservation = 0.0;
    int imitationCount = 0;
    int observationCount = 0;
    
    for (const auto& agent : agents) {
        if (agent) {
            if (agent->hasMetric("imitation_success")) {
                totalImitation += agent->getMetric("imitation_success");
                imitationCount++;
            }
            if (agent->hasMetric("observation_success")) {
                totalObservation += agent->getMetric("observation_success");
                observationCount++;
            }
        }
    }
    
    double avgImitation = imitationCount > 0 ? totalImitation / imitationCount : 0.0;
    double avgObservation = observationCount > 0 ? totalObservation / observationCount : 0.0;
    
    ss << "Imitation Learning:" << std::endl;
    ss << "  Average Success Rate: " << avgImitation << std::endl;
    ss << "  Number of Agents: " << imitationCount << std::endl << std::endl;
    
    ss << "Observation Learning:" << std::endl;
    ss << "  Average Success Rate: " << avgObservation << std::endl;
    ss << "  Number of Agents: " << observationCount << std::endl << std::endl;
    
    ss << "Learning Effectiveness: " << (avgImitation + avgObservation) / 2.0 << std::endl;
    
    return ss.str();
}

std::string MultiAgentRunner::generateCompetitiveReport(const std::vector<std::shared_ptr<EnhancedExperiment>>& agents) const {
    std::stringstream ss;
    ss << "Competitive Experiment Report" << std::endl;
    ss << "==========================" << std::endl << std::endl;
    
    std::vector<std::pair<std::string, double>> performances;
    
    for (const auto& agent : agents) {
        if (agent) {
            double performance = agent->hasMetric("performance") ? agent->getMetric("performance") : 0.0;
            performances.emplace_back(agent->getName(), performance);
        }
    }
    
    // Sort by performance (descending)
    std::sort(performances.begin(), performances.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    ss << "Rankings:" << std::endl;
    for (size_t i = 0; i < performances.size(); ++i) {
        ss << "  " << (i + 1) << ". " << performances[i].first << ": " << performances[i].second << std::endl;
    }
    
    if (performances.size() >= 2) {
        double topPerformance = performances[0].second;
        double bottomPerformance = performances.back().second;
        double performanceGap = topPerformance - bottomPerformance;
        
        ss << "Performance Gap: " << performanceGap << std::endl;
        ss << "Competitive Pressure: " << (performanceGap / topPerformance) << std::endl;
    }
    
    return ss.str();
}

// Helper methods
bool MultiAgentRunner::configureAgentFromParams(const std::shared_ptr<EnhancedExperiment>& agent,
                                               const std::unordered_map<std::string, std::string>& params) {
    if (!agent) return false;
    
    for (const auto& pair : params) {
        agent->setParameter(pair.first, pair.second);
    }
    
    return true;
}

bool MultiAgentRunner::runAgent(const std::shared_ptr<EnhancedExperiment>& agent) {
    if (!agent) return false;
    
    agent->initialize();
    agent->start();
    
    // Simulate some experiment steps
    for (size_t step = 0; step < 10; ++step) {
        agent->recordStep(step, 100, 200, 50, 1.5, 0.1);
        agent->recordMetric("performance", 0.5 + (step / 10.0) * 0.5, "performance");
    }
    
    agent->complete();
    return true;
}

void MultiAgentRunner::simulateLearningBetweenAgents(const std::vector<std::shared_ptr<EnhancedExperiment>>& agents) {
    if (agents.size() < 2) return;
    
    for (size_t i = 0; i < agents.size(); ++i) {
        for (size_t j = i + 1; j < agents.size(); ++j) {
            if (agents[i] && agents[j]) {
                simulateAgentInteraction(agents[i], agents[j], {});
                simulateAgentInteraction(agents[j], agents[i], {});
            }
        }
    }
}

void MultiAgentRunner::determineCompetitiveOutcomes(const std::vector<std::shared_ptr<EnhancedExperiment>>& agents) {
    if (agents.size() < 2) return;
    
    std::vector<std::pair<std::string, double>> performances;
    
    for (const auto& agent : agents) {
        if (agent && agent->hasMetric("performance")) {
            performances.emplace_back(agent->getName(), agent->getMetric("performance"));
        }
    }
    
    if (performances.size() >= 2) {
        std::sort(performances.begin(), performances.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        // Determine winners and losers based on performance
        double topPerformance = performances[0].second;
        double threshold = topPerformance * 0.7; // 70% of top performance
        
        for (const auto& pair : performances) {
            if (pair.second >= threshold) {
                // Winner
                auto agent = getAgent(pair.first);
                if (agent) {
                    agent->recordMetric("winner", 1.0, "competitive");
                }
            } else {
                // Loser
                auto agent = getAgent(pair.first);
                if (agent) {
                    agent->recordMetric("winner", 0.0, "competitive");
                }
            }
        }
    }
}

void MultiAgentRunner::updateAgentParameters(const std::shared_ptr<EnhancedExperiment>& agent,
                                            const std::shared_ptr<EnhancedExperiment>& model,
                                            const std::unordered_map<std::string, std::string>& params) {
    if (!agent || !model) return;
    
    // Agent adopts successful parameters from model
    for (const auto& pair : params) {
        agent->setParameter(pair.first, pair.second);
    }
    
    // Update agent's metrics based on model's success
    if (model->hasMetric("performance")) {
        double modelPerformance = model->getMetric("performance");
        agent->recordMetric("imitated_performance", modelPerformance, "social_learning");
    }
}

} // namespace nlm
