#include "AdvancedExperiment.hpp"
#include <thread>
#include <future>
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>
#include <unordered_set>
#include <sstream>
#include <filesystem>

namespace nlm {

struct AdvancedExperiment::Impl {
    std::string name;
    ParameterMap parameters;
    std::vector<ParameterSpec> parameterSpecs;
    std::unordered_map<std::string, std::vector<MetricType>> metrics;
    std::vector<StepData> stepData;
    std::vector<std::string> notes;
    std::set<std::string> tags;
    SimulationStep startStep;
    SimulationStep endStep;
    uint64_t seed;
    AdvancedConfig config;
    std::shared_ptr<ExperimentMonitor> monitor;
    
    Impl() : startStep(0), endStep(0), seed(42) {}
    Impl(const ExperimentTemplate& experimentTemplate) {
        name = experimentTemplate.name;
        seed = 42;
        startStep = 0;
        endStep = experimentTemplate.maxSteps;
        
        // Set default parameters from template
        for (const auto& param : experimentTemplate.parameters) {
            parameters[param.name] = param.defaultValue;
            parameterSpecs.push_back(param);
        }
    }
};

AdvancedExperiment::AdvancedExperiment(const std::string& name) : pImpl(std::make_unique<Impl>()) {
    pImpl->name = name;
    pImpl->seed = std::chrono::system_clock::now().time_since_epoch().count();
}

AdvancedExperiment::AdvancedExperiment(const ExperimentTemplate& experimentTemplate) : pImpl(std::make_unique<Impl>(experimentTemplate)) {
}

AdvancedExperiment::~AdvancedExperiment() = default;

const std::string& AdvancedExperiment::getName() const {
    return pImpl->name;
}

void AdvancedExperiment::setName(const std::string& name) {
    pImpl->name = name;
}

void AdvancedExperiment::addParameter(const ParameterSpec& param) {
    pImpl->parameterSpecs.push_back(param);
    pImpl->parameters[param.name] = param.defaultValue;
}

bool AdvancedExperiment::hasParameter(const std::string& name) const {
    return pImpl->parameters.find(name) != pImpl->parameters.end();
}

ParameterSpec AdvancedExperiment::getParameter(const std::string& name) const {
    auto it = std::find_if(pImpl->parameterSpecs.begin(), pImpl->parameterSpecs.end(),
                          [&name](const ParameterSpec& p) { return p.name == name; });n    if (it != pImpl->parameterSpecs.end()) {
        return *it;
    }
    return ParameterSpec{};
}

const std::vector<ParameterSpec>& AdvancedExperiment::getParameters() const {
    return pImpl->parameterSpecs;
}

void AdvancedExperiment::setParameter(const std::string& name, const ParameterValue& value) {
    if (hasParameter(name)) {
        pImpl->parameters[name] = value;
    }
}

void AdvancedExperiment::setParameters(const ParameterMap& params) {
    for (const auto& pair : params) {
        setParameter(pair.first, pair.second);
    }
}

ParameterValue AdvancedExperiment::getParameter(const std::string& name) const {
    auto it = pImpl->parameters.find(name);
    if (it != pImpl->parameters.end()) {
        return it->second;
    }
    return ParameterValue{}; // Empty variant
}

const ParameterMap& AdvancedExperiment::getParameters() const {
    return pImpl->parameters;
}

bool AdvancedExperiment::run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment, 
                           std::shared_ptr<ExperimentMonitor> monitor) {
    return runWithConfiguration(pImpl->config, brain, environment, monitor);
}

bool AdvancedExperiment::runWithConfiguration(const AdvancedConfig& config, 
                                             std::shared_ptr<Brain> brain, 
                                             std::shared_ptr<Environment> environment,
                                             std::shared_ptr<ExperimentMonitor> monitor) {
    if (!brain || !environment) {
        return false;
    }
    
    pImpl->monitor = monitor;
    
    // Set up brain with configuration
    brain->setConfig(config);
    
    if (!brain->initialize()) {
        if (pImpl->monitor) {
            pImpl->monitor->activeErrors.push_back("Brain initialization failed");
        }
        return false;
    }
    
    // Reset environment
    auto observation = environment->reset();
    brain->receiveSensoryInput(*observation);
    
    // Update monitor
    if (pImpl->monitor) {
        pImpl->monitor->startTime = std::chrono::system_clock::now();
        pImpl->monitor->lastUpdate = pImpl->monitor->startTime;
        pImpl->monitor->currentStep = 0;
        pImpl->monitor->maxSteps = config.getOr<int64_t>("max_steps", 1000);
        updateMonitorProgress(pImpl->monitor, 0, pImpl->monitor->maxSteps);
    }
    
    // Run simulation loop
    for (SimulationStep step = 0; step < pImpl->monitor->maxSteps && !environment->isDone(); ++step) {
        pImpl->monitor->currentStep = step;
        
        // Brain step
        brain->step(step);
        
        // Record data
        recordStep(step,
                  brain->getTotalNeuronCount(),
                  brain->getTotalSynapseCount(),
                  brain->getFiringNeuronCount(),
                  brain->getAverageFiringRate());
        
        // Get action from brain
        auto action = brain->produceAction();
        
        // Environment step
        observation = environment->step(*action);
        
        // Send observation to brain
        brain->receiveSensoryInput(*observation);
        
        // Record reward
        float reward = environment->getLastReward();
        recordMetric("total_reward", static_cast<double>(reward));
        
        // Update monitor
        if (pImpl->monitor) {
            updateMonitorProgress(pImpl->monitor, step, pImpl->monitor->maxSteps);
            checkMonitorWarnings(pImpl->monitor);
        }
    }
    
    pImpl->endStep = pImpl->monitor->maxSteps;
    return true;
}

std::vector<std::shared_ptr<AdvancedExperiment>> AdvancedExperiment::runParameterSweep(
    const std::string& paramName,
    const std::vector<ParameterValue>& values,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    std::shared_ptr<ExperimentMonitor> monitor) {
    
    std::vector<std::shared_ptr<AdvancedExperiment>> results;
    
    // Get original parameter value
    ParameterValue originalValue = getParameter(paramName);
    
    for (const auto& value : values) {
        // Create a copy of this experiment
        auto exp = clone();
        
        // Set new parameter value
        exp->setParameter(paramName, value);
        
        // Run the experiment
        if (exp->run(brainTemplate, environmentTemplate, monitor)) {
            results.push_back(exp);
        }
    }
    
    // Restore original value
    setParameter(paramName, originalValue);
    
    return results;
}

ExperimentComparison AdvancedExperiment::compareExperiments(
    const std::shared_ptr<AdvancedExperiment>& otherExperiment,
    const std::string& metricName) const {
    ExperimentComparison comparison;
    comparison.metricName = metricName;
    
    // Extract metric values from both experiments
    auto it1 = pImpl->metrics.find(metricName);
    auto it2 = otherExperiment->pImpl->metrics.find(metricName);
    
    if (it1 != pImpl->metrics.end() && it2 != otherExperiment->pImpl->metrics.end()) {
        comparison.experiment1Values.clear();
        comparison.experiment2Values.clear();
        
        // Convert MetricType to double for comparison
        for (const auto& value : it1->second) {
            if (std::holds_alternative<double>(value)) {
                comparison.experiment1Values.push_back(std::get<double>(value));
            } else if (std::holds_alternative<int>(value)) {
                comparison.experiment1Values.push_back(static_cast<double>(std::get<int>(value)));
            } else if (std::holds_alternative<float>(value)) {
                comparison.experiment1Values.push_back(static_cast<double>(std::get<float>(value)));
            }
        }
        
        for (const auto& value : it2->second) {
            if (std::holds_alternative<double>(value)) {
                comparison.experiment2Values.push_back(std::get<double>(value));
            } else if (std::holds_alternative<int>(value)) {
                comparison.experiment2Values.push_back(static_cast<double>(std::get<int>(value)));
            } else if (std::holds_alternative<float>(value)) {
                comparison.experiment2Values.push_back(static_cast<double>(std::get<float>(value)));
            }
        }
        
        // Calculate statistics
        comparison.meanDifference = ExperimentUtils::calculateMean(comparison.experiment2Values) - 
                                   ExperimentUtils::calculateMean(comparison.experiment1Values);
        
        double stddev1 = ExperimentUtils::calculateStddev(comparison.experiment1Values,
                                                        ExperimentUtils::calculateMean(comparison.experiment1Values));
        double stddev2 = ExperimentUtils::calculateStddev(comparison.experiment2Values,
                                                        ExperimentUtils::calculateMean(comparison.experiment2Values));
        
        comparison.pooledStddev = std::sqrt((stddev1 * stddev1 + stddev2 * stddev2) / 2.0);
        
        if (comparison.pooledStddev > 0) {
            comparison.tStatistic = comparison.meanDifference / comparison.pooledStddev;
        }
        
        // Calculate p-value (simplified t-test)
        comparison.pValue = 2.0 * std::erfc(std::abs(comparison.tStatistic) / std::sqrt(2.0));
        
        // Calculate Cohen's d
        if (comparison.pooledStddev > 0) {
            comparison.cohensD = comparison.meanDifference / comparison.pooledStddev;
        }
        
        // Determine conclusion based on p-value and effect size
        if (comparison.pValue < 0.05) {
            if (comparison.meanDifference > 0) {
                comparison.conclusion = "Experiment 2 significantly outperforms Experiment 1";
            } else {
                comparison.conclusion = "Experiment 1 significantly outperforms Experiment 2";
            }
        } else {
            comparison.conclusion = "No significant difference detected";
        }
    }
    
    return comparison;
}

StatisticalAnalysis AdvancedExperiment::analyzeMetric(const std::string& metricName) const {
    StatisticalAnalysis analysis;
    analysis.metricName = metricName;
    
    auto it = pImpl->metrics.find(metricName);
    if (it == pImpl->metrics.end()) {
        return analysis;
    }
    
    // Extract numeric values
    for (const auto& value : it->second) {
        if (std::holds_alternative<double>(value)) {
            analysis.values.push_back(std::get<double>(value));
        } else if (std::holds_alternative<int>(value)) {
            analysis.values.push_back(static_cast<double>(std::get<int>(value)));
        } else if (std::holds_alternative<float>(value)) {
            analysis.values.push_back(static_cast<double>(std::get<float>(value)));
        }
    }
    
    if (analysis.values.empty()) {
        return analysis;
    }
    
    // Calculate statistics
    analysis.statistics.mean = ExperimentUtils::calculateMean(analysis.values);
    analysis.statistics.stddev = ExperimentUtils::calculateStddev(analysis.values, analysis.statistics.mean);
    analysis.statistics.min = *std::min_element(analysis.values.begin(), analysis.values.end());
    analysis.statistics.max = *std::max_element(analysis.values.begin(), analysis.values.end());
    analysis.statistics.count = analysis.values.size();
    analysis.statistics.median = ExperimentUtils::calculatePercentile(analysis.values, 50.0);
    analysis.statistics.p95 = ExperimentUtils::calculatePercentile(analysis.values, 95.0);
    analysis.statistics.p99 = ExperimentUtils::calculatePercentile(analysis.values, 99.0);
    
    // Determine distribution type (simplified)
    double skewness = 0.0;
    if (analysis.statistics.stddev > 0) {
        double mean = analysis.statistics.mean;
        double n = analysis.values.size();
        double m2 = 0.0, m3 = 0.0;
        
        for (double val : analysis.values) {
            double diff = val - mean;
            m2 += diff * diff;
            m3 += diff * diff * diff;
        }
        
        m2 /= n;
        m3 /= n;
        
        if (m2 > 0) {
            skewness = (std::pow(m3, 1.5)) / std::pow(m2, 2.5);
        }
    }
    
    if (std::abs(skewness) < 0.5) {
        analysis.distributionType = "normal";
    } else if (skewness > 0.5) {
        analysis.distributionType = "right-skewed";
    } else {
        analysis.distributionType = "left-skewed";
    }
    
    // Calculate significance (assuming comparison with zero mean difference)
    analysis.pValue = 2.0 * std::erfc(std::abs(analysis.statistics.mean) / 
                                     (analysis.statistics.stddev * std::sqrt(analysis.values.size())));
    analysis.isSignificant = analysis.pValue < 0.05;
    analysis.effectSize = analysis.statistics.mean / (analysis.statistics.stddev + 1e-10);
    
    // Generate interpretation
    if (analysis.isSignificant) {
        analysis.interpretation = "Statistically significant ";
        if (analysis.effectSize > 0.8) analysis.interpretation += "large effect";
        else if (analysis.effectSize > 0.5) analysis.interpretation += "medium effect";
        else analysis.interpretation += "small effect";
    } else {
        analysis.interpretation = "Not statistically significant";
    }
    
    return analysis;
}

std::vector<StatisticalAnalysis> AdvancedExperiment::analyzeAllMetrics() const {
    std::vector<StatisticalAnalysis> results;
    for (const auto& pair : pImpl->metrics) {
        results.push_back(analyzeMetric(pair.first));
    }
    return results;
}

ParameterMap AdvancedExperiment::optimizeParameters(std::shared_ptr<Brain> brainTemplate,
                                                    std::shared_ptr<Environment> environmentTemplate,
                                                    const std::vector<std::string>& objectiveMetrics,
                                                    const std::unordered_map<std::string, std::vector<ParameterValue>>& searchSpace,
                                                    std::shared_ptr<ExperimentMonitor> monitor) {
    ParameterMap bestParameters;
    double bestObjective = -std::numeric_limits<double>::max();
    
    // Simple random search for optimization
    uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);
    
    // Create parameter specifications for search space
    std::vector<ParameterSpec> paramSpecs;
    for (const auto& pair : searchSpace) {
        ParameterSpec spec;
        spec.name = pair.first;
        spec.defaultValue = pair.second.empty() ? 0 : pair.second[0];
        spec.type = "double"; // Simplified - assuming all parameters are double
        spec.includeInOutput = true;
        paramSpecs.push_back(spec);
    }
    
    // Generate random combinations
    size_t numIterations = std::min<size_t>(100, searchSpace.size() > 0 ? 
                                            searchSpace.begin()->second.size() * 2 : 100);
    
    for (size_t i = 0; i < numIterations; ++i) {
        ParameterMap currentParams;
        
        // Randomly select parameter values
        for (const auto& pair : searchSpace) {
            const auto& values = pair.second;
            if (!values.empty()) {
                size_t idx = rng() % values.size();
                currentParams[pair.first] = values[idx];
            }
        }
        
        // Create temporary experiment
        auto tempExp = clone();
        tempExp->setParameters(currentParams);
        
        // Run experiment
        if (tempExp->run(brainTemplate, environmentTemplate, monitor)) {
            // Calculate objective value
            double objective = 0.0;
            for (const auto& metricName : objectiveMetrics) {
                auto value = tempExp->getMetric(metricName);
                if (value.has_value() && std::holds_alternative<double>(value.value())) {
                    objective += std::get<double>(value.value());
                }
            }
            
            if (objective > bestObjective) {
                bestObjective = objective;
                bestParameters = currentParams;
            }
        }
        
        if (monitor) {
            updateMonitorProgress(monitor, i, numIterations);
        }
    }
    
    return bestParameters;
}

void AdvancedExperiment::recordMetric(const std::string& name, double value) {
    pImpl->metrics[name].push_back(value);
}

void AdvancedExperiment::recordMetric(const std::string& name, float value) {
    recordMetric(name, static_cast<double>(value));
}

void AdvancedExperiment::recordMetric(const std::string& name, int value) {
    recordMetric(name, static_cast<double>(value));
}

std::optional<MetricType> AdvancedExperiment::getMetric(const std::string& name) const {
    auto it = pImpl->metrics.find(name);
    if (it != pImpl->metrics.end() && !it->second.empty()) {
        return it->second.back();
    }
    return std::nullopt;
}

const std::unordered_map<std::string, std::vector<MetricType>>& AdvancedExperiment::getMetrics() const {
    return pImpl->metrics;
}

void AdvancedExperiment::recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                                   size_t firingCount, float avgFiringRate) {
    StepData data;
    data.step = step;
    data.neuronCount = neuronCount;
    data.synapseCount = synapseCount;
    data.firingCount = firingCount;
    data.avgFiringRate = avgFiringRate;
    pImpl->stepData.push_back(data);
}

const std::vector<AdvancedExperiment::StepData>& AdvancedExperiment::getStepData() const {
    return pImpl->stepData;
}

double AdvancedExperiment::getObjectiveValue(const std::string& metricName) const {
    auto it = pImpl->metrics.find(metricName);
    if (it != pImpl->metrics.end() && !it->second.empty()) {
        double sum = 0.0;
        for (const auto& value : it->second) {
            if (std::holds_alternative<double>(value)) {
                sum += std::get<double>(value);
            } else if (std::holds_alternative<int>(value)) {
                sum += static_cast<double>(std::get<int>(value));
            } else if (std::holds_alternative<float>(value)) {
                sum += static_cast<double>(std::get<float>(value));
            }
        }
        return sum / it->second.size();
    }
    return 0.0;
}

std::vector<double> AdvancedExperiment::getObjectiveVector(const std::vector<std::string>& metricNames) const {
    std::vector<double> objectives;
    for (const auto& metricName : metricNames) {
        objectives.push_back(getObjectiveValue(metricName));
    }
    return objectives;
}

std::string AdvancedExperiment::generateReport() const {
    std::ostringstream oss;
    oss << "Advanced Experiment Report" << std::endl;
    oss << "===========================" << std::endl;
    oss << "Experiment Name: " << pImpl->name << std::endl;
    oss << "Seed: " << pImpl->seed << std::endl;
    oss << "Start Step: " << pImpl->startStep << std::endl;
    oss << "End Step: " << pImpl->endStep << std::endl;
    oss << "Number of Steps: " << pImpl->stepData.size() << std::endl;
    oss << "Number of Parameters: " << pImpl->parameters.size() << std::endl;
    oss << std::endl;
    
    oss << "Parameters:" << std::endl;
    for (const auto& pair : pImpl->parameters) {
        oss << "  " << pair.first << " = " << parameterValueToString(pair.second) << std::endl;
    }
    
    oss << std::endl;
    oss << "Metrics:" << std::endl;
    for (const auto& pair : pImpl->metrics) {
        oss << "  " << pair.first << ": " << pair.second.size() << " values" << std::endl;
        
        // Calculate statistics for this metric
        std::vector<double> values;
        for (const auto& value : pair.second) {
            if (std::holds_alternative<double>(value)) {
                values.push_back(std::get<double>(value));
            } else if (std::holds_alternative<int>(value)) {
                values.push_back(static_cast<double>(std::get<int>(value)));
            } else if (std::holds_alternative<float>(value)) {
                values.push_back(static_cast<double>(std::get<float>(value)));
            }
        }
        
        if (!values.empty()) {
            double mean = ExperimentUtils::calculateMean(values);
            double stddev = ExperimentUtils::calculateStddev(values, mean);
            oss << "    Mean: " << mean << " ± " << stddev << std::endl;
            oss << "    Min: " << *std::min_element(values.begin(), values.end()) << std::endl;
            oss << "    Max: " << *std::max_element(values.begin(), values.end()) << std::endl;
        }
    }
    
    oss << std::endl;
    oss << "Notes:" << std::endl;
    for (const auto& note : pImpl->notes) {
        oss << "  - " << note << std::endl;
    }
    
    oss << std::endl;
    oss << "Tags: ";
    bool firstTag = true;
    for (const auto& tag : pImpl->tags) {
        if (!firstTag) oss << ", ";
        oss << tag;
        firstTag = false;
    }
    oss << std::endl;
    
    return oss.str();
}

bool AdvancedExperiment::exportResults(const std::string& filepath) const {
    return ExperimentUtils::saveExperimentResults(*this, filepath);
}

void AdvancedExperiment::setStartStep(SimulationStep step) {
    pImpl->startStep = step;
}

void AdvancedExperiment::setEndStep(SimulationStep step) {
    pImpl->endStep = step;
}

SimulationStep AdvancedExperiment::getStartStep() const {
    return pImpl->startStep;
}

SimulationStep AdvancedExperiment::getEndStep() const {
    return pImpl->endStep;
}

void AdvancedExperiment::addNote(const std::string& note) {
    pImpl->notes.push_back(note);
}

const std::vector<std::string>& AdvancedExperiment::getNotes() const {
    return pImpl->notes;
}

void AdvancedExperiment::addTag(const std::string& tag) {
    pImpl->tags.insert(tag);
}

const std::set<std::string>& AdvancedExperiment::getTags() const {
    return pImpl->tags;
}

uint64_t AdvancedExperiment::getSeed() const {
    return pImpl->seed;
}

void AdvancedExperiment::setSeed(uint64_t seed) {
    pImpl->seed = seed;
}

void AdvancedExperiment::setConfigValue(const std::string& key, const std::string& value) {
    pImpl->config.set(key, value);
}

std::string AdvancedExperiment::getConfigValue(const std::string& key) const {
    auto value = pImpl->config.get<std::string>(key);
    return value.has_value() ? value.value() : "";
}

void AdvancedExperiment::setMonitor(std::shared_ptr<ExperimentMonitor> monitor) {
    pImpl->monitor = monitor;
}

std::shared_ptr<ExperimentMonitor> AdvancedExperiment::getMonitor() const {
    return pImpl->monitor;
}

bool AdvancedExperiment::operator==(const AdvancedExperiment& other) const {
    return pImpl->name == other.pImpl->name &&
           pImpl->parameters == other.pImpl->parameters &&
           pImpl->seed == other.pImpl->seed;
}

bool AdvancedExperiment::operator!=(const AdvancedExperiment& other) const {
    return !(*this == other);
}

std::shared_ptr<AdvancedExperiment> AdvancedExperiment::clone() const {
    auto cloned = std::make_shared<AdvancedExperiment>(name);
    
    // Copy parameters
    cloned->pImpl->parameters = pImpl->parameters;
    cloned->pImpl->parameterSpecs = pImpl->parameterSpecs;
    
    // Copy metrics
    cloned->pImpl->metrics = pImpl->metrics;
    
    // Copy step data
    cloned->pImpl->stepData = pImpl->stepData;
    
    // Copy notes
    cloned->pImpl->notes = pImpl->notes;
    
    // Copy tags
    cloned->pImpl->tags = pImpl->tags;
    
    // Copy timing
    cloned->pImpl->startStep = pImpl->startStep;
    cloned->pImpl->endStep = pImpl->endStep;
    
    // Copy seed
    cloned->pImpl->seed = pImpl->seed;
    
    // Copy config
    cloned->pImpl->config = pImpl->config;
    
    return cloned;
}

void AdvancedExperiment::updateMonitorProgress(std::shared_ptr<ExperimentMonitor> monitor, 
                                               SimulationStep currentStep, 
                                               SimulationStep maxSteps) const {
    if (!monitor) return;
    
    monitor->currentStep = currentStep;
    monitor->lastUpdate = std::chrono::system_clock::now();
    
    double progress = (maxSteps > 0) ? (static_cast<double>(currentStep) / maxSteps) : 0.0;
    
    std::ostringstream oss;
    oss << "Progress: " << (progress * 100.0) << "% (Step " << currentStep << "/" << maxSteps << ")";
    
    if (monitor->progressCallback) {
        monitor->progressCallback(oss.str());
    }
}

void AdvancedExperiment::checkMonitorWarnings(std::shared_ptr<ExperimentMonitor> monitor) const {
    if (!monitor) return;
    
    // Check for common warnings
    if (pImpl->stepData.size() > 100) {
        size_t recentSteps = std::min<size_t>(pImpl->stepData.size(), 10);
        float totalFiringRate = 0.0;
        
        for (size_t i = pImpl->stepData.size() - recentSteps; i < pImpl->stepData.size(); ++i) {
            totalFiringRate += pImpl->stepData[i].avgFiringRate;
        }
        
        float avgFiringRate = totalFiringRate / recentSteps;
        
        if (avgFiringRate > 1.0) {
            monitor->activeWarnings.push_back("High average firing rate detected: " + 
                                            std::to_string(avgFiringRate));
        }
    }
}

std::vector<ParameterValue> AdvancedExperiment::generateParameterCombination(
    const std::vector<ParameterSpec>& parameters) const {
    std::vector<ParameterValue> combination;
    std::mt19937 rng(pImpl->seed);
    
    for (const auto& param : parameters) {
        // Randomly select a value based on parameter type
        if (param.type == "int" && param.minValue.index() == param.maxValue.index()) {
            std::uniform_int_distribution<int> dist(
                std::get<int>(param.minValue),
                std::get<int>(param.maxValue)
            );
            combination.push_back(dist(rng));
        } else if ((param.type == "double" || param.type == "float") && 
                   param.minValue.index() == param.maxValue.index()) {
            std::uniform_real_distribution<double> dist(
                std::get<double>(param.minValue),
                std::get<double>(param.maxValue)
            );
            combination.push_back(dist(rng));
        } else if (param.type == "bool") {
            std::bernoulli_distribution dist(0.5);
            combination.push_back(static_cast<int>(dist(rng)));
        } else if (!param.categoricalValues.empty()) {
            size_t idx = rng() % param.categoricalValues.size();
            combination.push_back(param.categoricalValues[idx]);
        } else {
            combination.push_back(param.defaultValue);
        }
    }
    
    return combination;
}

bool AdvancedExperiment::isValueInRange(const ParameterValue& value, 
                                       const ParameterValue& min, 
                                       const ParameterValue& max) {
    // Check if value is within min/max range
    return toDouble(value) >= toDouble(min) && toDouble(value) <= toDouble(max);
}

double AdvancedExperiment::toDouble(const ParameterValue& value) {
    if (std::holds_alternative<int>(value)) {
        return static_cast<double>(std::get<int>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<float>(value)) {
        return static_cast<double>(std::get<float>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? 1.0 : 0.0;
    } else if (std::holds_alternative<std::string>(value)) {
        // Try to convert string to double
        try {
            return std::stod(std::get<std::string>(value));
        } catch (...) {
            return 0.0;
        }
    }
    return 0.0;
}

std::string AdvancedExperiment::parameterValueToString(const ParameterValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<double>(value)) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(4) << std::get<double>(value);
        return oss.str();
    } else if (std::holds_alternative<float>(value)) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(4) << std::get<float>(value);
        return oss.str();
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    }
    return "unknown";
}

ParameterValue AdvancedExperiment::stringToParameterValue(const std::string& str, const std::string& type) {
    if (type == "int") {
        return std::stoi(str);
    } else if (type == "double") {
        return std::stod(str);
    } else if (type == "float") {
        return static_cast<float>(std::stod(str));
    } else if (type == "bool") {
        return (str == "true" || str == "1" || str == "yes");
    } else {
        return str;
    }
}

// AdvancedExperimentRunner implementation

struct AdvancedExperimentRunner::Impl {
    std::vector<std::shared_ptr<AdvancedExperiment>> experiments;
    uint64_t globalSeed;
    std::unordered_map<std::string, ExperimentTemplate> templates;
    std::vector<std::thread> threadPool;
    bool threadPoolInitialized;
    
    Impl() : globalSeed(42), threadPoolInitialized(false) {}
    ~Impl() {
        shutdownThreadPool();
    }
};

AdvancedExperimentRunner::AdvancedExperimentRunner() : pImpl(std::make_unique<Impl>()) {}

AdvancedExperimentRunner::~AdvancedExperimentRunner() = default;

std::shared_ptr<AdvancedExperiment> AdvancedExperimentRunner::createExperiment(const std::string& name) {
    auto exp = std::make_shared<AdvancedExperiment>(name);
    exp->setSeed(pImpl->globalSeed++);
    pImpl->experiments.push_back(exp);
    return exp;
}

std::shared_ptr<AdvancedExperiment> AdvancedExperimentRunner::createExperiment(const ExperimentTemplate& template) {
    auto exp = std::make_shared<AdvancedExperiment>(template);
    exp->setSeed(pImpl->globalSeed++);
    pImpl->experiments.push_back(exp);
    return exp;
}

std::vector<std::shared_ptr<AdvancedExperiment>> AdvancedExperimentRunner::createBatch(
    const std::vector<std::string>& names) {
    
    std::vector<std::shared_ptr<AdvancedExperiment>> batch;
    batch.reserve(names.size());
    
    for (const auto& name : names) {
        batch.push_back(createExperiment(name));
    }
    
    return batch;
}

std::vector<std::shared_ptr<AdvancedExperiment>> AdvancedExperimentRunner::createParameterSweep(
    const ExperimentTemplate& template,
    const std::string& paramName,
    const std::vector<ParameterValue>& values,
    size_t maxParallel) {
    
    std::vector<std::shared_ptr<AdvancedExperiment>> results;
    
    // Get original parameter value
    ParameterSpec originalParam = template.getParameter(paramName);
    ParameterValue originalValue = originalParam.defaultValue;
    
    // Create thread pool if needed
    if (!pImpl->threadPoolInitialized) {
        initializeThreadPool(maxParallel);
        pImpl->threadPoolInitialized = true;
    }
    
    // Generate parameter combinations
    std::vector<ParameterMap> paramCombinations;
    
    for (const auto& value : values) {
        ParameterMap params;
        for (const auto& param : template.parameters) {
            if (param.name == paramName) {
                params[param.name] = value;
            } else {
                params[param.name] = param.defaultValue;
            }
        }
        paramCombinations.push_back(params);
    }
    
    // Run experiments in parallel
    std::vector<std::future<std::shared_ptr<AdvancedExperiment>>> futures;
    futures.reserve(paramCombinations.size());
    
    for (const auto& params : paramCombinations) {
        futures.push_back(std::async(std::launch::async, [this, template, params]() {
            auto exp = std::make_shared<AdvancedExperiment>(template);
            exp->setParameters(params);
            return exp;
        }));
    }
    
    // Collect results
    for (auto& future : futures) {
        try {
            auto exp = future.get();
            results.push_back(exp);
        } catch (const std::exception& e) {
            // Handle error
        }
    }
    
    return results;
}

bool AdvancedExperimentRunner::runExperiment(std::shared_ptr<AdvancedExperiment> experiment,
                                             std::shared_ptr<Brain> brain,
                                             std::shared_ptr<Environment> environment,
                                             std::shared_ptr<ExperimentMonitor> monitor,
                                             bool useAsync) {
    if (!experiment || !brain || !environment) {
        return false;
    }
    
    if (useAsync) {
        // Run asynchronously
        auto future = std::async(std::launch::async, [this, experiment, brain, environment, monitor]() {
            return runExperiment(experiment, brain, environment, monitor, false);
        });
        return true; // Async execution started
    } else {
        // Run synchronously
        return experiment->run(brain, environment, monitor);
    }
}

std::vector<std::shared_ptr<AdvancedExperiment>> AdvancedExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    bool useAsync,
    size_t maxParallel) {
    
    std::vector<std::shared_ptr<AdvancedExperiment>> results;
    
    if (useAsync && maxParallel > 1) {
        // Parallel execution
        std::vector<std::future<std::shared_ptr<AdvancedExperiment>>> futures;
        futures.reserve(experiments.size());
        
        for (const auto& exp : experiments) {
            futures.push_back(std::async(std::launch::async, [this, exp, brainTemplate, environmentTemplate]() {
                return runExperiment(exp, brainTemplate, environmentTemplate, nullptr, false);
            }));
        }
        
        // Collect results
        for (auto& future : futures) {
            try {
                auto exp = future.get();
                if (exp) {
                    results.push_back(exp);
                }
            } catch (const std::exception& e) {
                // Handle error
            }
        }
    } else {
        // Sequential execution
        for (const auto& exp : experiments) {
            if (runExperiment(exp, brainTemplate, environmentTemplate, nullptr, false)) {
                results.push_back(exp);
            }
        }
    }
    
    return results;
}

AdvancedExperimentRunner::BatchResults AdvancedExperimentRunner::runBatchWithResults(
    const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    bool useAsync,
    size_t maxParallel) {
    
    BatchResults results;
    
    // Run experiments
    auto successful = runBatch(experiments, brainTemplate, environmentTemplate, useAsync, maxParallel);
    
    // Separate successful and failed experiments
    for (const auto& exp : experiments) {
        if (std::find(successful.begin(), successful.end(), exp) != successful.end()) {
            results.successful.push_back(exp);
        } else {
            results.failed.push_back(exp);
            results.errors[exp->getName()] = "Experiment failed to run";
        }
    }
    
    // Calculate aggregate statistics
    results.totalObjectiveValue = 0.0;
    results.totalStepsCompleted = 0;
    
    for (const auto& exp : results.successful) {
        // Calculate objective value based on metrics
        for (const auto& pair : exp->getMetrics()) {
            auto value = exp->getMetric(pair.first);
            if (value.has_value() && std::holds_alternative<double>(value.value())) {
                results.totalObjectiveValue += std::get<double>(value.value());
            }
        }
        
        results.totalStepsCompleted += exp->getEndStep();
    }
    
    if (!results.successful.empty()) {
        results.totalObjectiveValue /= results.successful.size();
    }
    
    return results;
}

std::vector<ExperimentComparison> AdvancedExperimentRunner::compareExperiments(
    const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments,
    const std::string& metricName) const {
    
    std::vector<ExperimentComparison> comparisons;
    
    for (size_t i = 0; i < experiments.size(); ++i) {
        for (size_t j = i + 1; j < experiments.size(); ++j) {
            ExperimentComparison comparison = experiments[i]->compareExperiments(experiments[j], metricName);
            comparisons.push_back(comparison);
        }
    }
    
    return comparisons;
}

std::vector<double> AdvancedExperimentRunner::aggregateMetric(
    const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments,
    const std::string& metricName,
    const std::string& aggregationMethod) const {
    
    std::vector<double> values;
    
    for (const auto& exp : experiments) {
        double value = exp->getObjectiveValue(metricName);
        if (value != 0.0) { // Only include non-zero values
            values.push_back(value);
        }
    }
    
    if (values.empty()) {
        return values;
    }
    
    // Apply aggregation method
    if (aggregationMethod == "mean") {
        double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
        return std::vector<double>(values.size(), mean);
    } else if (aggregationMethod == "median") {
        std::vector<double> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        double median = sorted[sorted.size() / 2];
        return std::vector<double>(values.size(), median);
    } else if (aggregationMethod == "sum") {
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        return std::vector<double>(values.size(), sum);
    }
    
    return values;
}

bool AdvancedExperimentRunner::saveResults(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write header
        file << "Advanced Experiment Results" << std::endl;
        file << "===========================" << std::endl;
        file << "Total Experiments: " << pImpl->experiments.size() << std::endl;
        file << std::endl;
        
        // Write each experiment
        for (const auto& exp : pImpl->experiments) {
            file << "Experiment: " << exp->getName() << std::endl;
            file << "  Seed: " << exp->getSeed() << std::endl;
            file << "  Parameters:" << std::endl;
            
            const auto& params = exp->getParameters();
            for (const auto& pair : params) {
                file << "    " << pair.first << " = " << 
                    AdvancedExperiment::parameterValueToString(pair.second) << std::endl;
            }
            
            file << "  Metrics:" << std::endl;
            const auto& metrics = exp->getMetrics();
            for (const auto& pair : metrics) {
                file << "    " << pair.first << ": " << pair.second.size() << " values" << std::endl;
            }
            
            file << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool AdvancedExperimentRunner::loadResults(const std::string& filepath) {
    pImpl->experiments.clear();
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        std::string currentExperiment;
        
        while (std::getline(file, line)) {
            if (line.find("Experiment: ") == 0) {
                currentExperiment = line.substr(13);
            } else if (line.find("  Parameters:") == 0) {
                // Would parse parameters here
            } else if (line.find("  Metrics:") == 0) {
                // Would parse metrics here
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void AdvancedExperimentRunner::setGlobalSeed(uint64_t seed) {
    pImpl->globalSeed = seed;
}

uint64_t AdvancedExperimentRunner::getGlobalSeed() const {
    return pImpl->globalSeed;
}

void AdvancedExperimentRunner::addTemplate(const ExperimentTemplate& template) {
    pImpl->templates[template.name] = template;
}

ExperimentTemplate AdvancedExperimentRunner::getTemplate(const std::string& name) const {
    auto it = pImpl->templates.find(name);
    if (it != pImpl->templates.end()) {
        return it->second;
    }
    return ExperimentTemplate{};
}

const std::unordered_map<std::string, ExperimentTemplate>& AdvancedExperimentRunner::getTemplates() const {
    return pImpl->templates;
}

AdvancedExperimentRunner::RunnerStats AdvancedExperimentRunner::getStats() const {
    RunnerStats stats;
    stats.totalExperimentsRun = pImpl->experiments.size();
    stats.successfulExperiments = pImpl->experiments.size(); // Simplified
    stats.failedExperiments = 0;
    stats.averageObjectiveValue = 0.0;
    stats.lastRunTime = std::chrono::system_clock::now();
    stats.totalRunTime = std::chrono::seconds(0);
    return stats;
}

void AdvancedExperimentRunner::initializeThreadPool(size_t threadCount) {
    if (threadCount > 0) {
        pImpl->threadPool.reserve(threadCount);
        for (size_t i = 0; i < threadCount; ++i) {
            pImpl->threadPool.emplace_back([]() {
                // Worker thread
                while (true) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            });
        }
    }
}

void AdvancedExperimentRunner::shutdownThreadPool() {
    for (auto& thread : pImpl->threadPool) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    pImpl->threadPool.clear();
    pImpl->threadPoolInitialized = false;
}

// ExperimentUtils implementation

bool ExperimentUtils::validateParameterRange(const ParameterSpec& param, const ParameterValue& value) {
    return AdvancedExperiment::isValueInRange(value, param.minValue, param.maxValue);
}

bool ExperimentUtils::validateParameterValue(const ParameterSpec& param, const ParameterValue& value) {
    // Check type
    if (param.type == "int" && !std::holds_alternative<int>(value)) {
        return false;
    } else if (param.type == "double" && !std::holds_alternative<double>(value)) {
        return false;
    } else if (param.type == "bool" && !std::holds_alternative<bool>(value)) {
        return false;
    } else if (param.type == "string" && !std::holds_alternative<std::string>(value)) {
        return false;
    }
    
    // Check range
    if (!validateParameterRange(param, value)) {
        return false;
    }
    
    // Check categorical values
    if (!param.categoricalValues.empty()) {
        bool isCategorical = false;
        if (std::holds_alternative<int>(value)) {
            for (const auto& catValue : param.categoricalValues) {
                if (std::get<int>(value) == std::get<int>(catValue)) {
                    isCategorical = true;
                    break;
                }
            }
        } else if (std::holds_alternative<double>(value)) {
            for (const auto& catValue : param.categoricalValues) {
                if (std::get<double>(value) == std::get<double>(catValue)) {
                    isCategorical = true;
                    break;
                }
            }
        } else if (std::holds_alternative<std::string>(value)) {
            for (const auto& catValue : param.categoricalValues) {
                if (std::get<std::string>(value) == std::get<std::string>(catValue)) {
                    isCategorical = true;
                    break;
                }
            }
        }
        
        if (!isCategorical) {
            return false;
        }
    }
    
    return true;
}

double ExperimentUtils::calculateMean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double ExperimentUtils::calculateStddev(const std::vector<double>& values, double mean) {
    if (values.size() < 2) return 0.0;
    
    double sum = 0.0;
    for (double val : values) {
        sum += (val - mean) * (val - mean);
    }
    
    return std::sqrt(sum / (values.size() - 1));
}

double ExperimentUtils::calculatePercentile(const std::vector<double>& values, double percentile) {
    if (values.empty()) return 0.0;
    
    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    
    double index = (percentile / 100.0) * (sorted.size() - 1);
    size_t lower = static_cast<size_t>(std::floor(index));
    size_t upper = static_cast<size_t>(std::ceil(index));
    
    if (lower == upper) {
        return sorted[lower];
    }
    
    double weight = index - lower;
    return sorted[lower] * (1.0 - weight) + sorted[upper] * weight;
}

double ExperimentUtils::calculateTTest(const std::vector<double>& sample1, const std::vector<double>& sample2) {
    if (sample1.size() < 2 || sample2.size() < 2) return 0.0;
    
    double mean1 = calculateMean(sample1);
    double mean2 = calculateMean(sample2);
    double stddev1 = calculateStddev(sample1, mean1);
    double stddev2 = calculateStddev(sample2, mean2);
    
    double pooledStddev = std::sqrt((stddev1 * stddev1 + stddev2 * stddev2) / 2.0);
    
    if (pooledStddev == 0.0) return 0.0;
    
    return (mean1 - mean2) / pooledStddev;
}

std::vector<ParameterMap> ExperimentUtils::generateGrid(const std::vector<ParameterSpec>& parameters) {
    std::vector<ParameterMap> combinations;
    
    if (parameters.empty()) return combinations;
    
    // This is a simplified implementation - real grid search would be more complex
    // For now, just generate a few combinations
    for (size_t i = 0; i < parameters.size() && i < 10; ++i) {
        ParameterMap params;
        for (const auto& param : parameters) {
            params[param.name] = param.defaultValue;
        }
        combinations.push_back(params);
    }
    
    return combinations;
}

std::vector<ParameterMap> ExperimentUtils::generateRandom(const std::vector<ParameterSpec>& parameters,
                                                          size_t numSamples,
                                                          uint64_t seed) {
    std::vector<ParameterMap> combinations;
    std::mt19937 rng(seed);
    
    for (size_t i = 0; i < numSamples; ++i) {
        ParameterMap params;
        
        for (const auto& param : parameters) {
            // Generate random value based on parameter type
            if (param.type == "int" && param.minValue.index() == param.maxValue.index()) {
                std::uniform_int_distribution<int> dist(
                    std::get<int>(param.minValue),
                    std::get<int>(param.maxValue)
                );
                params[param.name] = dist(rng);
            } else if ((param.type == "double" || param.type == "float") && 
                       param.minValue.index() == param.maxValue.index()) {
                std::uniform_real_distribution<double> dist(
                    std::get<double>(param.minValue),
                    std::get<double>(param.maxValue)
                );
                params[param.name] = dist(rng);
            } else if (param.type == "bool") {
                std::bernoulli_distribution dist(0.5);
                params[param.name] = static_cast<int>(dist(rng));
            } else {
                params[param.name] = param.defaultValue;
            }
        }
        
        combinations.push_back(params);
    }
    
    return combinations;
}

bool ExperimentUtils::saveExperimentResults(const AdvancedExperiment& experiment, const std::string& filepath) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write experiment report
        file << experiment.generateReport() << std::endl;
        
        // Write step data
        file << "\nStep Data:" << std::endl;
        for (const auto& step : experiment.getStepData()) {
            file << "  Step " << step.step << ": "
                 << "Neurons=" << step.neuronCount << ", "
                 << "Synapses=" << step.synapseCount << ", "
                 << "Firing=" << step.firingCount << ", "
                 << "Rate=" << step.avgFiringRate << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool ExperimentUtils::loadExperimentResults(AdvancedExperiment& experiment, const std::string& filepath) {
    // Simplified implementation - would need full parsing
    return false;
}

std::string ExperimentUtils::compareExperiments(const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments) {
    std::ostringstream oss;
    oss << "Experiment Comparison" << std::endl;
    oss << "=====================" << std::endl;
    
    if (experiments.empty()) {
        oss << "No experiments to compare." << std::endl;
        return oss.str();
    }
    
    // Compare metrics across experiments
    std::set<std::string> allMetrics;
    for (const auto& exp : experiments) {
        const auto& metrics = exp->getMetrics();
        for (const auto& pair : metrics) {
            allMetrics.insert(pair.first);
        }
    }
    
    for (const auto& metricName : allMetrics) {
        oss << "\nMetric: " << metricName << std::endl;
        
        std::vector<double> values;
        for (const auto& exp : experiments) {
            double value = exp->getObjectiveValue(metricName);
            if (value != 0.0) {
                values.push_back(value);
                oss << "  " << exp->getName() << ": " << value << std::endl;
            }
        }
        
        if (!values.empty()) {
            double mean = ExperimentUtils::calculateMean(values);
            double stddev = ExperimentUtils::calculateStddev(values, mean);
            oss << "  Mean: " << mean << " ± " << stddev << std::endl;
        }
    }
    
    return oss.str();
}

} // namespace nlm
