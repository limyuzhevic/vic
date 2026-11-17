#include "ExperimentRunner.hpp"
#include "ExperimentRunnerUtils.hpp"  // Include utility functions for experiment processing
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <cmath>

namespace nlm {

struct ExperimentRunner::Impl {
    std::vector<std::shared_ptr<Experiment>> experiments;
    uint64_t globalSeed;
    bool verboseOutput;
    
    Impl() : globalSeed(42), verboseOutput(false) {}
};

// Helper functions for experiment processing
namespace ExperimentRunnerUtils {

// Convert Experiment to JSON
nlohmann::json experimentToJSON(const std::shared_ptr<const Experiment>& experiment) {
    nlohmann::json j;
    
    j["name"] = experiment->getName();
    j["seed"] = experiment->getSeed();
    j["start_step"] = experiment->getStartStep();
    j["end_step"] = experiment->getEndStep();
    
    // Convert step data
    nlohmann::json steps = nlohmann::json::array();
    for (const auto& stepData : experiment->getStepData()) {
        nlohmann::json step;
        step["step"] = stepData.step;
        step["neuron_count"] = stepData.neuronCount;
        step["synapse_count"] = stepData.synapseCount;
        step["firing_count"] = stepData.firingCount;
        step["avg_firing_rate"] = stepData.avgFiringRate;
        steps.push_back(step);
    }
    j["steps"] = steps;
    
    // Convert metrics
    nlohmann::json metrics = nlohmann::json::array();
    for (const auto& metricPair : experiment->getMetrics()) {
        nlohmann::json metric;
        metric["name"] = metricPair.first;
        metric["value"] = metricPair.second;
        metrics.push_back(metric);
    }
    j["metrics"] = metrics;
    
    // Convert notes
    nlohmann::json notes = nlohmann::json::array();
    for (const auto& note : experiment->getNotes()) {
        notes.push_back(note);
    }
    j["notes"] = notes;
    
    return j;
}

// Convert YAML Node from Experiment
YAML::Node experimentToYAML(const std::shared_ptr<const Experiment>& experiment) {
    YAML::Node j;
    
    j["name"] = experiment->getName();
    j["seed"] = experiment->getSeed();
    j["start_step"] = experiment->getStartStep();
    j["end_step"] = experiment->getEndStep();
    
    // Convert step data
    YAML::Node steps = YAML::Node(YAML::NodeType::Sequence);
    for (const auto& stepData : experiment->getStepData()) {
        YAML::Node step;
        step["step"] = stepData.step;
        step["neuron_count"] = stepData.neuronCount;
        step["synapse_count"] = stepData.synapseCount;
        step["firing_count"] = stepData.firingCount;
        step["avg_firing_rate"] = stepData.avgFiringRate;
        steps.push_back(step);
    }
    j["steps"] = steps;
    
    // Convert metrics
    YAML::Node metrics = YAML::Node(YAML::NodeType::Sequence);
    for (const auto& metricPair : experiment->getMetrics()) {
        YAML::Node metric;
        metric["name"] = metricPair.first;
        metric["value"] = metricPair.second;
        metrics.push_back(metric);
    }
    j["metrics"] = metrics;
    
    // Convert notes
    YAML::Node notes = YAML::Node(YAML::NodeType::Sequence);
    for (const auto& note : experiment->getNotes()) {
        notes.push_back(note);
    }
    j["notes"] = notes;
    
    return j;
}

// Statistical analysis of experimental results
ExperimentResults analyzeExperimentResults(const std::shared_ptr<const Experiment>& experiment) {
    ExperimentResults results;
    results.experiment = experiment;
    
    // Calculate basic statistics from step data
    const auto& stepData = experiment->getStepData();
    if (!stepData.empty()) {
        size_t totalNeurons = 0;
        size_t totalSynapses = 0;
        size_t totalFiring = 0;
        float totalFiringRate = 0.0f;
        
        for (const auto& step : stepData) {
            totalNeurons += step.neuronCount;
            totalSynapses += step.synapseCount;
            totalFiring += step.firingCount;
            totalFiringRate += step.avgFiringRate;
        }
        
        results.avgNeurons = totalNeurons / stepData.size();
        results.avgSynapses = totalSynapses / stepData.size();
        results.avgFiringCount = totalFiring / stepData.size();
        results.avgFiringRate = totalFiringRate / stepData.size();
        
        // Calculate variability
        float neuronVar = 0.0f, synapseVar = 0.0f, firingVar = 0.0f, rateVar = 0.0f;
        
        for (const auto& step : stepData) {
            neuronVar += std::pow(static_cast<float>(step.neuronCount) - results.avgNeurons, 2);
            synapseVar += std::pow(static_cast<float>(step.synapseCount) - results.avgSynapses, 2);
            firingVar += std::pow(static_cast<float>(step.firingCount) - results.avgFiringCount, 2);
            rateVar += std::pow(step.avgFiringRate - results.avgFiringRate, 2);
        }
        
        results.neuronStdDev = std::sqrt(neuronVar / stepData.size());
        results.synapseStdDev = std::sqrt(synapseVar / stepData.size());
        results.firingStdDev = std::sqrt(firingVar / stepData.size());
        results.rateStdDev = std::sqrt(rateVar / stepData.size());
        
        // Coefficient of variation
        results.neuronCV = (results.avgNeurons > 0) ? (results.neuronStdDev / results.avgNeurons) : 0.0f;
        results.synapseCV = (results.avgSynapses > 0) ? (results.synapseStdDev / results.avgSynapses) : 0.0f;
        results.firingCV = (results.avgFiringCount > 0) ? (results.firingStdDev / results.avgFiringCount) : 0.0f;
        results.rateCV = (results.avgFiringRate > 0) ? (results.rateStdDev / results.avgFiringRate) : 0.0f;
    }
    
    // Calculate metrics statistics
    const auto& metrics = experiment->getMetrics();
    std::unordered_map<std::string, double> metricValues;
    for (const auto& metricPair : metrics) {
        metricValues[metricPair.first] = metricPair.second;
    }
    
    results.metricStats.clear();
    for (const auto& [name, value] : metricValues) {
        StatisticalResult stat;
        stat.name = name;
        stat.value = value;
        
        // For now, stats only contain the recorded value
        // In a full implementation, you'd track all values over time
        stat.mean = value;
        stat.min = value;
        stat.max = value;
        stat.stdDev = 0.0f;
        
        results.metricStats.push_back(stat);
    }
    
    return results;
}

// Generate comprehensive report
std::string generateComprehensiveReport(const std::vector<std::shared_ptr<const Experiment>>& experiments) {
    std::ostringstream oss;
    oss << "=== Comprehensive Experiment Report ===\n\n";
    
    oss << "Total Experiments: " << experiments.size() << "\n\n";
    
    for (size_t i = 0; i < experiments.size(); ++i) {
        const auto& exp = experiments[i];
        oss << "Experiment " << (i + 1) << ": " << exp->getName() << "\n";
        oss << "  Seed: " << exp->getSeed() << "\n";
        oss << "  Duration: " << (exp->getEndStep() - exp->getStartStep()) << " steps\n";
        
        const auto& stepData = exp->getStepData();
        if (!stepData.empty()) {
            oss << "  Neurons: " << stepData.back().neuronCount << " (avg: " 
                << (stepData.size() > 0 ? (stepData[0].neuronCount + stepData.back().neuronCount) / 2 : 0) << ")\n";
            oss << "  Synapses: " << stepData.back().synapseCount << " (avg: " 
                << (stepData.size() > 0 ? (stepData[0].synapseCount + stepData.back().synapseCount) / 2 : 0) << ")\n";
            oss << "  Firing events: " << stepData.back().firingCount << "\n";
            oss << "  Firing rate: " << stepData.back().avgFiringRate << " Hz\n";
        }
        
        oss << "\n";
    }
    
    oss << "=== End Report ===";
    return oss.str();
}

} // namespace ExperimentRunnerUtils

ExperimentRunner::ExperimentRunner() : pImpl(new Impl) {}

ExperimentRunner::~ExperimentRunner() = default;

std::shared_ptr<Experiment> ExperimentRunner::createExperiment(const std::string& name) {
    auto exp = std::make_shared<Experiment>(name);
    exp->setSeed(pImpl->globalSeed++);
    pImpl->experiments.push_back(exp);
    
    if (pImpl->verboseOutput) {
        NLM_LOG_INFO("Created experiment: " + name + " (seed: " + std::to_string(exp->getSeed()) + ")");
    }
    
    return exp;
}

bool ExperimentRunner::runExperiment(std::shared_ptr<Experiment> experiment,
                                    std::shared_ptr<Brain> brain,
                                    std::shared_ptr<Environment> environment,
                                    SimulationStep maxSteps) {
    if (!experiment || !brain || !environment) {
        NLM_LOG_ERROR("Invalid experiment, brain, or environment provided");
        return false;
    }
    
    experiment->setStartStep(0);
    
    // Reset environment and brain
    auto observation = environment->reset();
    brain->receiveSensoryInput(*observation);
    
    // Record initial metrics
    experiment->recordMetric("total_steps", 0);
    experiment->recordMetric("total_rewards", 0.0f);
    
    // Run simulation loop with progress tracking
    for (SimulationStep step = 0; step < maxSteps && !environment->isDone(); ++step) {
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
        double currentTotalReward = experiment->getMetric("total_rewards");
        experiment->recordMetric("total_rewards", currentTotalReward + reward);
        
        // Progress reporting for long experiments
        if (pImpl->verboseOutput && step % 100 == 0) {
            NLM_LOG_INFO("  Progress: Step " + std::to_string(step) + "/" + 
                        std::to_string(maxSteps) + ", Reward: " + std::to_string(reward) +
                        ", Total Rewards: " + std::to_string(currentTotalReward + reward));
        }
    }
    
    experiment->setEndStep(maxSteps);
    
    // Record final metrics
    experiment->recordMetric("total_steps", static_cast<double>(maxSteps));
    
    if (pImpl->verboseOutput) {
        NLM_LOG_INFO("Experiment " + experiment->getName() + " completed successfully");
        NLM_LOG_INFO("  Total steps: " + std::to_string(maxSteps));
        NLM_LOG_INFO("  Final total rewards: " + std::to_string(experiment->getMetric("total_rewards")));
    }
    
    return true;
}

std::vector<std::shared_ptr<Experiment>> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    
    std::vector<std::shared_ptr<Experiment>> results;
    
    NLM_LOG_INFO("Running batch of " + std::to_string(experiments.size()) + " experiments...");
    
    for (size_t i = 0; i < experiments.size(); ++i) {
        auto& exp = experiments[i];
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Running experiment " + std::to_string(i + 1) + "/" + 
                    std::to_string(experiments.size()) + ": " + exp->getName());
        
        // Create fresh brain and environment for each experiment
        auto brain = std::make_shared<Brain>(std::const_pointer_cast<Config>(brainTemplate->getConfig()));
        brain->initialize();
        
        // Create fresh environment
        auto environment = std::make_shared<Environment>(*environmentTemplate.get());
        
        if (runExperiment(exp, brain, environment, maxSteps)) {
            results.push_back(exp);
            NLM_LOG_INFO("✓ Experiment " + exp->getName() + " completed successfully");
        } else {
            NLM_LOG_ERROR("✗ Experiment " + exp->getName() + " failed");
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Batch completed: " + std::to_string(results.size()) + "/" + 
                std::to_string(experiments.size()) + " experiments successful");
    
    return results;
}

const std::vector<std::shared_ptr<Experiment>>& ExperimentRunner::getExperiments() const {
    return pImpl->experiments;
}

std::shared_ptr<Experiment> ExperimentRunner::getExperiment(const std::string& name) const {
    for (auto& exp : pImpl->experiments) {
        if (exp->getName() == name) {
            return exp;
        }
    }
    return nullptr;
}

void ExperimentRunner::clearExperiments() {
    pImpl->experiments.clear();
    NLM_LOG_INFO("Cleared all experiments");
}

void ExperimentRunner::setGlobalSeed(uint64_t seed) {
    pImpl->globalSeed = seed;
    NLM_LOG_INFO("Set global seed to: " + std::to_string(seed));
}

bool ExperimentRunner::saveResults(const std::string& filepath) const {
    // Detect file format based on extension
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    
    if (ext == ".json" || ext == ".JSON") {
        return saveResultsToJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml" || ext == ".YAML" || ext == ".YML") {
        return saveResultsToYAML(filepath);
    } else {
        // Default to simple format
        return saveResultsToSimple(filepath);
    }
}

bool ExperimentRunner::saveResultsToJSON(const std::string& filepath) const {
    try {
        nlohmann::json j;
        
        // Convert all experiments to JSON
        nlohmann::json experiments = nlohmann::json::array();
        for (const auto& exp : pImpl->experiments) {
            experiments.push_back(ExperimentRunnerUtils::experimentToJSON(exp));
        }
        
        j["experiments"] = experiments;
        j["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
        j["version"] = "2.0";
        
        // Add comprehensive analysis
        j["analysis"] = generateAnalysisJSON();
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open JSON file for writing: " + filepath);
            return false;
        }
        
        file << std::setw(2) << j << std::endl;
        NLM_LOG_INFO("Saved experiment results to JSON file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving results to JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool ExperimentRunner::saveResultsToYAML(const std::string& filepath) const {
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        // Add metadata
        emitter << YAML::FlowKey("version") << YAML::Value << "2.0";
        emitter << YAML::FlowKey("timestamp") << YAML::Value 
                << std::chrono::system_clock::now().time_since_epoch().count();
        
        // Add experiments
        emitter << YAML::FlowKey("experiments") << YAML::Value << YAML::BeginSeq;
        
        for (const auto& exp : pImpl->experiments) {
            YAML::Node expNode = ExperimentRunnerUtils::experimentToYAML(exp);
            
            // Convert YAML Node to emitter
            emitter << YAML::BeginMap;
            for (const auto& field : expNode) {
                if (field.IsScalar()) {
                    std::string key = field.as<std::string>();
                    YAML::Node value = expNode[field];
                    
                    if (value.IsScalar()) {
                        std::string strValue = value.as<std::string>();
                        emitter << YAML::Key << key << YAML::Value << strValue;
                    } else if (value.IsSequence()) {
                        emitter << YAML::Key << key << YAML::Value << YAML::BeginSeq;
                        for (const auto& item : value) {
                            if (item.IsScalar()) {
                                emitter << item.as<std::string>();
                            }
                        }
                        emitter << YAML::EndSeq;
                    }
                }
            }
            emitter << YAML::EndMap;
        }
        
        emitter << YAML::EndSeq;
        
        // Add analysis
        emitter << YAML::FlowKey("analysis") << YAML::Value << generateAnalysisYAML();
        
        emitter << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open YAML file for writing: " + filepath);
            return false;
        }
        
        file << emitter.c_str();
        NLM_LOG_INFO("Saved experiment results to YAML file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving results to YAML file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool ExperimentRunner::saveResultsToSimple(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open results file for writing: " + filepath);
        return false;
    }
    
    file << "=== NLM Experiment Results ===\n\n";
    file << "Generated: " << std::chrono::system_clock::now().time_since_epoch().count() << "\n";
    file << "Total Experiments: " << pImpl->experiments.size() << "\n\n";
    
    for (size_t i = 0; i < pImpl->experiments.size(); ++i) {
        const auto& exp = pImpl->experiments[i];
        file << "Experiment " << (i + 1) << ": " << exp->getName() << "\n";
        file << "  Seed: " << exp->getSeed() << "\n";
        file << "  Duration: " << (exp->getEndStep() - exp->getStartStep()) << " steps\n";
        
        const auto& stepData = exp->getStepData();
        if (!stepData.empty()) {
            file << "  Final neurons: " << stepData.back().neuronCount << "\n";
            file << "  Final synapses: " << stepData.back().synapseCount << "\n";
            file << "  Final firing events: " << stepData.back().firingCount << "\n";
            file << "  Final firing rate: " << stepData.back().avgFiringRate << " Hz\n";
        }
        
        // Add all metrics
        const auto& metrics = exp->getMetrics();
        for (const auto& metricPair : metrics) {
            file << "  " << metricPair.first << ": " << metricPair.second << "\n";
        }
        
        file << "\n";
    }
    
    file << "=== End Results ===";
    NLM_LOG_INFO("Saved experiment results to simple text file: " + filepath);
    return true;
}

nlohmann::json ExperimentRunner::generateAnalysisJSON() const {
    nlohmann::json analysis;
    analysis["type"] = "comprehensive";
    analysis["experiment_count"] = pImpl->experiments.size();
    
    // Calculate overall statistics
    std::vector<double> allStepCounts;
    std::vector<double> allNeuronCounts;
    std::vector<double> allFiringCounts;
    std::vector<double> allFiringRates;
    
    for (const auto& exp : pImpl->experiments) {
        const auto& stepData = exp->getStepData();
        if (!stepData.empty()) {
            allStepCounts.push_back(exp->getEndStep() - exp->getStartStep());
            allStepCounts.push_back(stepData.back().neuronCount);
            allStepCounts.push_back(stepData.back().firingCount);
            allStepCounts.push_back(stepData.back().avgFiringRate);
        }
    }
    
    if (!allStepCounts.empty()) {
        analysis["avg_duration"] = calculateMean(allStepCounts);
        analysis["std_duration"] = calculateStdDev(allStepCounts, analysis["avg_duration"]);
        
        // Remove first element (placeholder) and recalculate
        allStepCounts.erase(allStepCounts.begin());
        analysis["avg_neuron_count"] = calculateMean(allStepCounts);
        analysis["std_neuron_count"] = calculateStdDev(allStepCounts, analysis["avg_neuron_count"]);
        
        allStepCounts.erase(allStepCounts.begin());
        analysis["avg_firing_count"] = calculateMean(allStepCounts);
        analysis["std_firing_count"] = calculateStdDev(allStepCounts, analysis["avg_firing_count"]);
        
        allStepCounts.erase(allStepCounts.begin());
        analysis["avg_firing_rate"] = calculateMean(allStepCounts);
        analysis["std_firing_rate"] = calculateStdDev(allStepCounts, analysis["avg_firing_rate"]);
    }
    
    return analysis;
}

YAML::Node ExperimentRunner::generateAnalysisYAML() const {
    YAML::Node analysis;
    analysis["type"] = "comprehensive";
    analysis["experiment_count"] = pImpl->experiments.size();
    
    // Calculate overall statistics
    std::vector<double> allStepCounts;
    std::vector<double> allNeuronCounts;
    std::vector<double> allFiringCounts;
    std::vector<double> allFiringRates;
    
    for (const auto& exp : pImpl->experiments) {
        const auto& stepData = exp->getStepData();
        if (!stepData.empty()) {
            allStepCounts.push_back(exp->getEndStep() - exp->getStartStep());
            allStepCounts.push_back(stepData.back().neuronCount);
            allStepCounts.push_back(stepData.back().firingCount);
            allStepCounts.push_back(stepData.back().avgFiringRate);
        }
    }
    
    if (!allStepCounts.empty()) {
        analysis["avg_duration"] = calculateMean(allStepCounts);
        analysis["std_duration"] = calculateStdDev(allStepCounts, analysis["avg_duration"]);
        
        // Remove first element (placeholder) and recalculate
        allStepCounts.erase(allStepCounts.begin());
        analysis["avg_neuron_count"] = calculateMean(allStepCounts);
        analysis["std_neuron_count"] = calculateStdDev(allStepCounts, analysis["avg_neuron_count"]);
        
        allStepCounts.erase(allStepCounts.begin());
        analysis["avg_firing_count"] = calculateMean(allStepCounts);
        analysis["std_firing_count"] = calculateStdDev(allStepCounts, analysis["avg_firing_count"]);
        
        allStepCounts.erase(allStepCounts.begin());
        analysis["avg_firing_rate"] = calculateMean(allStepCounts);
        analysis["std_firing_rate"] = calculateStdDev(allStepCounts, analysis["avg_firing_rate"]);
    }
    
    return analysis;
}

double ExperimentRunner::calculateMean(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double ExperimentRunner::calculateStdDev(const std::vector<double>& values, double mean) const {
    if (values.empty()) return 0.0;
    double sumSquares = 0.0;
    for (double value : values) {
        sumSquares += std::pow(value - mean, 2);
    }
    return std::sqrt(sumSquares / values.size());
}

} // namespace nlm
