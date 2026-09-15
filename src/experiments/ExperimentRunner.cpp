#include "ExperimentRunner.hpp"

namespace nlm {

struct ExperimentRunner::Impl {
    std::vector<std::shared_ptr<Experiment>> experiments;
    uint64_t globalSeed;
    
    Impl() : globalSeed(42) {}
};

ExperimentRunner::ExperimentRunner() : pImpl(new Impl) {}

ExperimentRunner::~ExperimentRunner() = default;

std::shared_ptr<Experiment> ExperimentRunner::createExperiment(const std::string& name) {
    auto exp = std::make_shared<Experiment>(name);
    exp->setSeed(pImpl->globalSeed++);
    pImpl->experiments.push_back(exp);
    return exp;
}

bool ExperimentRunner::runExperiment(std::shared_ptr<Experiment> experiment,
                                     std::shared_ptr<Brain> brain,
                                     std::shared_ptr<Environment> environment,
                                     SimulationStep maxSteps) {
    if (!experiment || !brain || !environment) {
        return false;
    }
    
    experiment->setStartStep(0);
    
    // Reset environment and brain
    auto observation = environment->reset();
    brain->receiveSensoryInput(*observation);
    
    // Run simulation loop
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
        experiment->recordMetric("total_reward", 
            experiment->getMetric("total_reward") + reward);
    }
    
    experiment->setEndStep(maxSteps);
    return true;
}

std::vector<std::shared_ptr<Experiment>> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    
    std::vector<std::shared_ptr<Experiment>> results;
    
    for (auto& exp : experiments) {
        // Create fresh brain and environment for each experiment
        // PLACEHOLDER: In real implementation, would clone from template
        auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
        brain->initialize();
        
        // Would also create fresh environment here
        
        if (runExperiment(exp, brain, environmentTemplate, maxSteps)) {
            results.push_back(exp);
        }
    }
    
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
}

void ExperimentRunner::setGlobalSeed(uint64_t seed) {
    pImpl->globalSeed = seed;
}

// Experiment data structure for results
struct ExperimentResult {
    std::string experimentName;
    SimulationStep startStep;
    SimulationStep endStep;
    size_t neuronCount;
    size_t synapseCount;
    size_t firingNeuronCount;
    float averageFiringRate;
    float totalReward;
    std::unordered_map<std::string, float> metrics;
    time_t timestamp;
};

// Serialization helpers
namespace {
    std::string serializeFloat(float value) {
        return std::to_string(value);
    }
    
    float deserializeFloat(const std::string& str) {
        return std::stof(str);
    }
    
    std::string escapeString(const std::string& str) {
        std::string result;
        for (char c : str) {
            if (c == '\\' || c == '"' || c == '\'') {
                result += '\\';
            }
            result += c;
        }
        return result;
    }
    
    std::string formatTimestamp() {
        time_t now = time(0);
        tm* t = localtime(&now);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", t);
        return std::string(buffer);
    }
}

bool ExperimentRunner::saveResults(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Experiment Results" << std::endl;
    file << "# Saved on: " << formatTimestamp() << std::endl;
    file << "# Global seed: " << pImpl->globalSeed << std::endl;
    file << std::endl;
    
    bool allSaved = true;
    
    for (const auto& experiment : pImpl->experiments) {
        file << "=== Experiment: " << experiment->getName() << " ===" << std::endl;
        
        // Collect experiment data
        ExperimentResult result;
        result.experimentName = experiment->getName();
        result.startStep = experiment->getStartStep();
        result.endStep = experiment->getEndStep();
        
        // Try to get metrics from experiment
        result.neuronCount = 0;
        result.synapseCount = 0;
        result.firingNeuronCount = 0;
        result.averageFiringRate = 0.0f;
        result.totalReward = 0.0f;
        
        // Check if experiment has necessary methods
        if (experiment->getMetrics && !experiment->getMetrics().empty()) {
            const auto& metrics = experiment->getMetrics();
            result.neuronCount = static_cast<size_t>(metrics.count("neuron_count") ? 
                                                   metrics.at("neuron_count") : 0);
            result.synapseCount = static_cast<size_t>(metrics.count("synapse_count") ? 
                                                    metrics.at("synapse_count") : 0);
            result.firingNeuronCount = static_cast<size_t>(metrics.count("firing_neuron_count") ? 
                                                             metrics.at("firing_neuron_count") : 0);
            result.averageFiringRate = metrics.count("average_firing_rate") ? 
                                       metrics.at("average_firing_rate") : 0.0f;
        }
        
        // Get reward metrics
        result.totalReward = experiment->getMetric("total_reward");
        
        // Store other metrics
        result.metrics = experiment->getMetrics();
        
        result.timestamp = time(0);
        
        // Write experiment data
        file << "Name: " << escapeString(result.experimentName) << std::endl;
        file << "Start Step: " << result.startStep << std::endl;
        file << "End Step: " << result.endStep << std::endl;
        file << "Neuron Count: " << result.neuronCount << std::endl;
        file << "Synapse Count: " << result.synapseCount << std::endl;
        file << "Firing Neuron Count: " << result.firingNeuronCount << std::endl;
        file << "Average Firing Rate: " << serializeFloat(result.averageFiringRate) << std::endl;
        file << "Total Reward: " << serializeFloat(result.totalReward) << std::endl;
        
        // Write all metrics
        for (const auto& metric : result.metrics) {
            if (metric.first != "neuron_count" && metric.first != "synapse_count" && 
                metric.first != "firing_neuron_count" && metric.first != "average_firing_rate") {
                file << metric.first << ": " << serializeFloat(metric.second) << std::endl;
            }
        }
        
        file << "Timestamp: " << result.timestamp << std::endl;
        file << std::endl;
        
        // Validate that critical data was written
        if (result.neuronCount == 0 || result.synapseCount == 0) {
            NLM_LOG_WARNING("Experiment " + result.experimentName + " has missing critical metrics");
            allSaved = false;
        }
    }
    
    // Write summary
    file << "=== Summary ===" << std::endl;
    file << "Total Experiments: " << pImpl->experiments.size() << std::endl;
    file << "Total Neurons (all): " << std::accumulate(pImpl->experiments.begin(), pImpl->experiments.end(), 
                                0, [](size_t sum, const auto& exp) {
            return sum + static_cast<size_t>(exp->getMetric("neuron_count", 0.0f));
        }) << std::endl;
    file << "Total Synapses (all): " << std::accumulate(pImpl->experiments.begin(), pImpl->experiments.end(), 
                                0, [](size_t sum, const auto& exp) {
            return sum + static_cast<size_t>(exp->getMetric("synapse_count", 0.0f));
        }) << std::endl;
    file << "Total Reward (all): " << std::accumulate(pImpl->experiments.begin(), pImpl->experiments.end(), 
                                0.0f, [](float sum, const auto& exp) {
            return sum + exp->getMetric("total_reward");
        }) << std::endl;
    
    file.close();
    
    if (allSaved) {
        NLM_LOG_INFO("Successfully saved experiment results to: " + filepath);
    } else {
        NLM_LOG_WARNING("Some experiments had incomplete data but results were saved");
    }
    
    return true;
}

} // namespace nlm
