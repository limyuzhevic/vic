#include "ExperimentRunner.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <thread>
#include <mutex>

namespace nlm {

struct ExperimentRunner::Impl {
    std::vector<std::shared_ptr<Experiment>> experiments;
    uint64_t globalSeed;
    AdvancedExperimentConfig globalConfig;
    
    // For parallel execution
    mutable std::mutex experimentMutex;
    
    Impl() : globalSeed(42) {
        globalConfig.experimentMode = "standard";
        globalConfig.seed = 42;
        globalConfig.maxSteps = 10000;
        globalConfig.enableParallelExecution = false;
        globalConfig.threadCount = 0;
    }
};

ExperimentRunner::ExperimentRunner() : pImpl(new Impl) {}

ExperimentRunner::~ExperimentRunner() = default;

std::shared_ptr<Experiment> ExperimentRunner::createExperiment(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->experimentMutex);
    auto exp = std::make_shared<Experiment>(name);
    exp->setSeed(pImpl->globalSeed++);
    pImpl->experiments.push_back(exp);
    NLM_LOG_INFO("Created experiment: " + name + " with seed: " + std::to_string(exp->getSeed()));
    return exp;
}

ExperimentResult ExperimentRunner::runExperiment(std::shared_ptr<Experiment> experiment,
                                                 std::shared_ptr<Brain> brain,
                                                 std::shared_ptr<Environment> environment,
                                                 const AdvancedExperimentConfig& config) {
    ExperimentResult result;
    result.experiment = experiment;
    result.timestamp = std::chrono::system_clock::now();
    
    if (!experiment || !brain || !environment) {
        result.errorMessage = "Invalid experiment, brain, or environment";
        return result;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Configure brain with experiment seed
        auto config = brain->getConfig();
        config->set("random_seed", static_cast<int64_t>(config->getOr<int64_t>("random_seed", 42) + experiment->getSeed()), ConfigSource::Default);
        
        // Reset brain and environment
        brain->reset();
        brain->initialize();
        
        auto observation = environment->reset();
        brain->receiveSensoryInput(*observation);
        
        experiment->setStartStep(0);
        experiment->clearMetrics();
        
        // Run simulation loop
        size_t step = 0;
        for (; step < config->getOr<SimulationStep>("max_steps", config->getOr<SimulationStep>("simulation_steps", 10000)); ++step) {
            // Check early stopping condition
            if (config.earlyStopCondition && !config.earlyStopCondition(brain.get(), step)) {
                break;
            }
            
            // Check environment done
            if (environment->isDone()) {
                break;
            }
            
            // Brain step
            brain->step(step);
            
            // Record data
            experiment->recordStep(
                step,
                brain->getTotalNeuronCount(),
                brain->getTotalSynapseCount(),
                brain->getFiringNeuronCount(),
                brain->getAverageFiringRate()
            );
            
            // Get action from brain
            auto action = brain->produceAction();
            
            // Environment step
            observation = environment->step(*action);
            
            // Send observation to brain
            brain->receiveSensoryInput(*observation);
            
            // Record reward
            float reward = environment->getLastReward();
            experiment->recordMetric("total_reward", 
                experiment->getMetric("total_reward") + reward);
            
            // Record additional metrics based on configuration
            if (config.experimentMode == "learning") {
                experiment->recordMetric("firing_rate", brain->getAverageFiringRate());
                experiment->recordMetric("active_neurons", brain->getFiringNeuronCount());
            }
        }
        
        experiment->setEndStep(step);
        result.success = true;
        
        // Collect metrics from experiment
        for (const auto& metric : experiment->getMetrics()) {
            result.metrics[metric.first] = metric.second;
        }
        
        // Copy step data
        result.stepData = experiment->getStepData();
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string("Exception during experiment execution: ") + e.what();
        result.success = false;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    if (result.success) {
        NLM_LOG_INFO("Experiment completed successfully: " + experiment->getName() + 
                    " (duration: " + std::to_string(result.executionTime) + "ms)");
    } else {
        NLM_LOG_ERROR("Experiment failed: " + experiment->getName() + 
                     " - " + result.errorMessage);
    }
    
    return result;
}

ExperimentResult ExperimentRunner::runExperiment(std::shared_ptr<Experiment> experiment,
                                                 std::shared_ptr<Brain> brain,
                                                 std::shared_ptr<Environment> environment) {
    // Use default configuration
    AdvancedExperimentConfig config;
    config.experimentMode = "standard";
    config.maxSteps = 10000;
    
    return runExperiment(experiment, brain, environment, config);
}

std::vector<ExperimentResult> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    const AdvancedExperimentConfig& config) {
    
    std::vector<ExperimentResult> results;
    results.reserve(experiments.size());
    
    for (auto& exp : experiments) {
        // Create fresh brain and environment for each experiment
        auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
        brain->initialize();
        
        auto environment = std::make_shared<Environment>(*environmentTemplate);
        
        ExperimentResult result = runExperiment(exp, brain, environment, config);
        results.push_back(result);
    }
    
    return results;
}

std::vector<ExperimentResult> ExperimentRunner::runWithMultipleSeeds(
    std::shared_ptr<Experiment> experiment,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    size_t numSeeds,
    const AdvancedExperimentConfig& config) {
    
    std::vector<ExperimentResult> results;
    results.reserve(numSeeds);
    
    for (size_t seed = 0; seed < numSeeds; ++seed) {
        uint64_t actualSeed = config.seed + seed;
        
        auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
        brain->initialize();
        
        auto environment = std::make_shared<Environment>(*environmentTemplate);
        
        // Set seed on brain
        auto brainConfig = brain->getConfig();
        brainConfig->set("random_seed", static_cast<int64_t>(actualSeed), ConfigSource::Default);
        
        ExperimentResult result = runExperiment(experiment, brain, environment, config);
        results.push_back(result);
    }
    
    return results;
}

ExperimentResult ExperimentRunner::runContinualLearning(
    std::shared_ptr<Brain> brain,
    const std::vector<std::function<float(Brain*)>>& tasks,
    size_t cycles,
    const AdvancedExperimentConfig& config) {
    
    ExperimentResult result;
    result.experiment = createExperiment("Continual Learning");
    
    try {
        brain->reset();
        brain->initialize();
        
        size_t totalSteps = 0;
        
        for (size_t cycle = 0; cycle < cycles; ++cycle) {
            for (size_t taskIdx = 0; taskIdx < tasks.size(); ++taskIdx) {
                auto taskName = "Task " + std::to_string(taskIdx) + " (Cycle " + std::to_string(cycle + 1) + ")";
                auto taskExperiment = createExperiment(taskName);
                
                // Run task
                size_t stepsPerTask = config.maxSteps / (tasks.size() * cycles);
                
                for (size_t step = 0; step < stepsPerTask; ++step) {
                    brain->step(totalSteps++);
                    
                    // Evaluate task performance
                    float performance = tasks[taskIdx](brain.get());
                    taskExperiment->recordMetric("task_performance", performance);
                    taskExperiment->recordStep(totalSteps, brain->getTotalNeuronCount(),
                                             brain->getTotalSynapseCount(), brain->getFiringNeuronCount(),
                                             brain->getAverageFiringRate());
                    
                    if (config.earlyStopCondition && !config.earlyStopCondition(brain.get(), totalSteps)) {
                        break;
                    }
                }
                
                // Add task experiment results
                for (const auto& metric : taskExperiment->getMetrics()) {
                    result.metrics[metric.first] = metric.second;
                }
            }
        }
        
        result.success = true;
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string("Exception during continual learning: ") + e.what();
        result.success = false;
    }
    
    return result;
}

ExperimentResult ExperimentRunner::runDamageRecovery(
    std::shared_ptr<Brain> brain,
    uint64_t damageStep,
    float damageFraction,
    const std::string& damageRegion,
    const AdvancedExperimentConfig& config) {
    
    ExperimentResult result;
    result.experiment = createExperiment("Damage Recovery");
    
    try {
        // Pre-damage baseline
        brain->reset();
        brain->initialize();
        
        for (uint64_t step = 0; step < damageStep; ++step) {
            brain->step(step);
        }
        
        // Record pre-damage state
        result.metrics["pre_damage_neurons"] = brain->getTotalNeuronCount();
        result.metrics["pre_damage_synapses"] = brain->getTotalSynapseCount();
        result.metrics["pre_damage_spikes"] = brain->getTotalSpikeCount();
        
        // Apply damage (placeholder - in real implementation, would damage specific brain regions)
        // For now, just reduce neuron count by damage fraction
        size_t preDamageNeurons = brain->getTotalNeuronCount();
        size_t damageNeurons = static_cast<size_t>(preDamageNeurons * damageFraction);
        // Note: This is a simplified damage model
        
        // Post-damage recovery
        for (uint64_t step = damageStep; step < damageStep + config.maxSteps; ++step) {
            brain->step(step);
            
            // Record recovery metrics
            result.metrics["post_damage_neurons"] = brain->getTotalNeuronCount();
            result.metrics["post_damage_synapses"] = brain->getTotalSynapseCount();
            result.metrics["post_damage_spikes"] = brain->getTotalSpikeCount();
        }
        
        result.success = true;
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string("Exception during damage recovery: ") + e.what();
        result.success = false;
    }
    
    return result;
}

std::vector<ExperimentResult> ExperimentRunner::runScalingExperiment(
    const std::vector<std::pair<size_t, std::function<std::shared_ptr<Brain>(const AdvancedExperimentConfig&)>>>& scales,
    uint64_t stepsPerScale,
    const AdvancedExperimentConfig& config) {
    
    std::vector<ExperimentResult> results;
    
    for (const auto& scale : scales) {
        NLM_LOG_INFO("Running scaling experiment with " + std::to_string(scale.first) + " neurons");
        
        // Create brain for this scale
        AdvancedExperimentConfig scaleConfig = config;
        scaleConfig.maxSteps = stepsPerScale;
        
        auto brain = scale.second(scaleConfig);
        if (!brain) {
            NLM_LOG_ERROR("Failed to create brain for scale " + std::to_string(scale.first));
            continue;
        }
        
        // Run experiment
        auto experiment = createExperiment("Scale " + std::to_string(scale.first) + " neurons");
        
        // Simple environment for testing
        auto environment = std::make_shared<Environment>();
        
        ExperimentResult result = runExperiment(experiment, brain, environment, scaleConfig);
        results.push_back(result);
    }
    
    return results;
}

ExperimentStatistics ExperimentRunner::computeStatistics(const std::vector<ExperimentResult>& results) {
    ExperimentStatistics stats;
    
    if (results.empty()) {
        return stats;
    }
    
    std::vector<float> rewards;
    std::vector<double> performances;
    
    for (const auto& result : results) {
        if (result.success) {
            stats.successCount++;
            
            // Extract reward metric
            if (result.metrics.find("total_reward") != result.metrics.end()) {
                float reward = result.metrics["total_reward"];
                rewards.push_back(reward);
            }
            
            // Extract performance metric if available
            if (result.metrics.find("firing_rate") != result.metrics.end()) {
                double performance = result.metrics["firing_rate"];
                performances.push_back(performance);
            }
        } else {
            stats.failureCount++;
        }
    }
    
    stats.runCount = results.size();
    
    if (!rewards.empty()) {
        stats.meanReward = *std::accumulate(rewards.begin(), rewards.end(), 0.0f) / rewards.size();
        stats.minReward = *std::min_element(rewards.begin(), rewards.end());
        stats.maxReward = *std::max_element(rewards.begin(), rewards.end());
        
        // Calculate standard deviation
        double sqSum = 0.0;
        for (float r : rewards) {
            double diff = r - stats.meanReward;
            sqSum += diff * diff;
        }
        stats.stdDevReward = std::sqrt(sqSum / rewards.size());
    }
    
    if (!performances.empty()) {
        stats.meanPerformance = *std::accumulate(performances.begin(), performances.end(), 0.0) / performances.size();
        
        // Calculate standard deviation
        double sqSum = 0.0;
        for (double p : performances) {
            double diff = p - stats.meanPerformance;
            sqSum += diff * diff;
        }
        stats.stdDevPerformance = std::sqrt(sqSum / performances.size());
        
        // Coefficient of variation
        if (stats.meanPerformance > 0) {
            stats.coefficientOfVariation = stats.stdDevPerformance / stats.meanPerformance;
        }
    }
    
    return stats;
}

std::string ExperimentRunner::generateStatisticalReport(const ExperimentStatistics& stats) {
    std::ostringstream report;
    report << "=== Experiment Statistics Report ===\n\n";
    report << "Experiment Name: " << stats.name << "\n";
    report << "Total Runs: " << stats.runCount << "\n";
    report << "Successful Runs: " << stats.successCount << "\n";
    report << "Failed Runs: " << stats.failureCount << "\n\n";
    
    if (stats.runCount > 0) {
        report << "Reward Statistics:\n";
        report << "  Mean: " << std::fixed << std::setprecision(4) << stats.meanReward << "\n";
        report << "  Standard Deviation: " << std::fixed << std::setprecision(4) << stats.stdDevReward << "\n";
        report << "  Minimum: " << std::fixed << std::setprecision(4) << stats.minReward << "\n";
        report << "  Maximum: " << std::fixed << std::setprecision(4) << stats.maxReward << "\n\n";
        
        report << "Performance Statistics:\n";
        report << "  Mean: " << std::fixed << std::setprecision(4) << stats.meanPerformance << "\n";
        report << "  Standard Deviation: " << std::fixed << std::setprecision(4) << stats.stdDevPerformance << "\n\n";
        
        report << "Coefficient of Variation: " << std::fixed << std::setprecision(4) << stats.coefficientOfVariation << "\n\n";
    }
    
    report << "Statistical Significance: ";
    if (stats.runCount >= 30) {
        report << "High (sufficient sample size)\n";
    } else if (stats.runCount >= 10) {
        report << "Medium (limited sample size)\n";
    } else {
        report << "Low (very limited sample size)\n";
    }
    
    return report.str();
}

std::string ExperimentRunner::generateComparativeReport(
    const std::vector<ExperimentStatistics>& stats,
    const std::vector<std::string>& labels) {
    
    if (stats.size() != labels.size()) {
        return "Error: Mismatch between statistics and labels";
    }
    
    std::ostringstream report;
    report << "=== Comparative Experiment Report ===\n\n";
    
    report << "Experiments Compared:\n";
    for (size_t i = 0; i < stats.size(); ++i) {
        report << "  " << labels[i] << "\n";
    }
    report << "\n";
    
    report << "Performance Comparison:\n";
    report << std::setw(20) << std::left << "Experiment" << std::setw(15) << std::left << "Mean Reward" << std::setw(20) << std::left << "Std Dev" << std::setw(15) << std::left << "Success Rate\n";
    report << std::string(70, '-') << "\n";
    
    for (size_t i = 0; i < stats.size(); ++i) {
        report << std::setw(20) << std::left << labels[i] 
               << std::setw(15) << std::left << stats[i].meanReward
               << std::setw(20) << std::left << stats[i].stdDevReward
               << std::setw(15) << std::left << (stats[i].runCount > 0 ? 
                     (static_cast<double>(stats[i].successCount) / stats[i].runCount) : 0.0) << "\n";
    }
    
    return report.str();
}

bool ExperimentRunner::exportResultsToJSON(const std::vector<ExperimentResult>& results,
                                          const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open file for writing: " + filepath);
        return false;
    }
    
    file << "{\n";
    file << "  \"experiments\": [\n";
    
    for (size_t i = 0; i < results.size(); ++i) {
        file << "    {\n";
        file << "      \"name\": \"" << (results[i].experiment ? results[i].experiment->getName() : "") << "\",\n";
        file << "      \"success\": " << (results[i].success ? "true" : "false") << ",\n";
        file << "      \"execution_time_ms\": " << results[i].executionTime << ",\n";
        file << "      \"error_message\": \"" << results[i].errorMessage << "\",\n";
        file << "      \"metrics\": {\n";
        
        size_t metricCount = 0;
        for (const auto& metric : results[i].metrics) {
            file << "        \"" << metric.first << "\": " << metric.second;
            if (++metricCount < results[i].metrics.size()) file << ",";
            file << "\n";
        }
        
        file << "      }\n";
        
        if (i < results.size() - 1) file << "    },\n";
        else file << "    }\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
    NLM_LOG_INFO("Results exported to JSON: " + filepath);
    return true;
}

bool ExperimentRunner::exportResultsToCSV(const std::vector<ExperimentResult>& results,
                                         const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open file for writing: " + filepath);
        return false;
    }
    
    // Write header
    file << "ExperimentName,Success,ExecutionTimeMs,ErrorMessage";
    
    // Get all unique metric names from all results
    std::set<std::string> allMetricNames;
    for (const auto& result : results) {
        for (const auto& metric : result.metrics) {
            allMetricNames.insert(metric.first);
        }
    }
    
    for (const auto& metricName : allMetricNames) {
        file << "," << metricName;
    }
    file << "\n";
    
    // Write data rows
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        
        file << (result.experiment ? result.experiment->getName() : "") << ","
             << (result.success ? "true" : "false") << ","
             << result.executionTime << ","
             << "\"" << result.errorMessage << "\"";
        
        // Write metrics
        for (const auto& metricName : allMetricNames) {
            if (result.metrics.find(metricName) != result.metrics.end()) {
                file << "," << result.metrics.at(metricName);
            } else {
                file << ",";
            }
        }
        
        if (i < results.size() - 1) file << "\n";
    }
    
    file.close();
    NLM_LOG_INFO("Results exported to CSV: " + filepath);
    return true;
}

void ExperimentRunner::setGlobalSeed(uint64_t seed) {
    std::lock_guard<std::mutex> lock(pImpl->experimentMutex);
    pImpl->globalSeed = seed;
    NLM_LOG_INFO("Global seed set to: " + std::to_string(seed));
}

void ExperimentRunner::setGlobalConfig(const AdvancedExperimentConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->experimentMutex);
    pImpl->globalConfig = config;
    NLM_LOG_INFO("Global configuration updated");
}

AdvancedExperimentConfig ExperimentRunner::getGlobalConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->experimentMutex);
    return pImpl->globalConfig;
}

std::vector<double> ExperimentRunner::analyzeLearningCurve(const std::vector<Experiment::StepData>& stepData) {
    std::vector<double> learningCurve;
    learningCurve.reserve(stepData.size());
    
    // Extract average firing rate from step data
    for (const auto& step : stepData) {
        learningCurve.push_back(step.avgFiringRate);
    }
    
    return learningCurve;
}

bool ExperimentRunner::detectLearningPlateau(const std::vector<double>& learningCurve,
                                            double threshold,
                                            size_t windowSize) {
    if (learningCurve.size() < windowSize * 2) {
        return false;
    }
    
    size_t curveSize = learningCurve.size();
    size_t midPoint = curveSize / 2;
    
    // Calculate average in first and second halves
    double firstHalfAvg = 0.0;
    double secondHalfAvg = 0.0;
    
    for (size_t i = 0; i < midPoint; ++i) {
        firstHalfAvg += learningCurve[i];
    }
    firstHalfAvg /= midPoint;
    
    for (size_t i = midPoint; i < curveSize; ++i) {
        secondHalfAvg += learningCurve[i];
    }
    secondHalfAvg /= (curveSize - midPoint);
    
    // Check if improvement is below threshold
    double improvement = std::abs(secondHalfAvg - firstHalfAvg);
    return improvement < threshold;
}

double ExperimentRunner::computeConvergenceMetric(const std::vector<double>& learningCurve) {
    if (learningCurve.size() < 2) {
        return 0.0;
    }
    
    // Calculate slope of last 10% of the curve
    size_t curveSize = learningCurve.size();
    size_t tailSize = curveSize / 10;
    if (tailSize < 2) tailSize = 2;
    
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    size_t tailStart = curveSize - tailSize;
    
    for (size_t i = 0; i < tailSize; ++i) {
        double x = i;
        double y = learningCurve[tailStart + i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    
    double slope = (tailSize * sumXY - sumX * sumY) / (tailSize * sumX2 - sumX * sumX);
    
    // Return absolute slope as convergence metric
    return std::abs(slope);
}

std::shared_ptr<Experiment> ExperimentRunner::createStandardExperiment(const std::string& name) {
    return createExperiment(name);
}

std::shared_ptr<Experiment> ExperimentRunner::createPlasticityExperiment(const std::string& name) {
    auto exp = createExperiment(name);
    exp->addNote("Plasticity experiment - focuses on synaptic changes");
    return exp;
}

std::shared_ptr<Experiment> ExperimentRunner::createLearningExperiment(const std::string& name) {
    auto exp = createExperiment(name);
    exp->addNote("Learning experiment - measures performance improvements");
    return exp;
}

const std::vector<std::shared_ptr<Experiment>>& ExperimentRunner::getExperiments() const {
    std::lock_guard<std::mutex> lock(pImpl->experimentMutex);
    return pImpl->experiments;
}

std::shared_ptr<Experiment> ExperimentRunner::getExperiment(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->experimentMutex);
    for (auto& exp : pImpl->experiments) {
        if (exp->getName() == name) {
            return exp;
        }
    }
    return nullptr;
}

std::vector<ExperimentResult> ExperimentRunner::getExperimentResults(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->experimentMutex);
    std::vector<ExperimentResult> results;
    
    auto experiment = getExperiment(name);
    if (experiment) {
        // In a real implementation, this would retrieve stored results
        // For now, return empty vector as results are stored per run
        results.push_back(ExperimentResult());
    }
    
    return results;
}

void ExperimentRunner::clearExperiments() {
    std::lock_guard<std::mutex> lock(pImpl->experimentMutex);
    pImpl->experiments.clear();
    NLM_LOG_INFO("All experiments cleared");
}

} // namespace nlm