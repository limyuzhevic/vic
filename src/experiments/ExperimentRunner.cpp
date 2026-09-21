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

bool ExperimentRunner::saveResults(const std::string& filepath) const {
    // Implement comprehensive experiment results saving with multiple formats:
    // 1. JSON format for easy parsing and integration
    // 2. Binary format for efficient storage
    // 3. Summary format for quick overview
    
    try {
        // Create output directory if it doesn't exist
        std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        
        // Save as JSON (primary format)
        std::string jsonPath = filepath;
        if (jsonPath.substr(jsonPath.find_last_of(".")) != ".json") {
            jsonPath += ".json";
        }
        
        saveResultsJSON(jsonPath);
        
        // Save as binary (compressed, efficient)
        std::string binPath = filepath;
        if (binPath.substr(binPath.find_last_of(".")) != ".bin") {
            binPath += ".bin";
        }
        
        saveResultsBinary(binPath);
        
        // Save as human-readable summary
        std::string summaryPath = filepath;
        if (summaryPath.substr(summaryPath.find_last_of(".")) != ".summary") {
            summaryPath += ".summary";
        }
        
        saveResultsSummary(summaryPath);
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Helper function to save results as JSON
void ExperimentRunner::saveResultsJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    // Create JSON structure
    nlohmann::json results;
    
    // Metadata
    results["metadata"]["version"] = "1.0.0";
    results["metadata"]["timestamp"] = std::chrono::system_clock::now();
    results["metadata"]["experiment_count"] = pImpl->experiments.size();
    
    // Experiments data
    nlohmann::json experimentsArray = nlohmann::json::array();
    
    for (const auto& experiment : pImpl->experiments) {
        nlohmann::json expData;
        
        expData["name"] = experiment->getName();
        expData["seed"] = experiment->getSeed();
        expData["start_step"] = experiment->getStartStep();
        expData["end_step"] = experiment->getEndStep();
        
        // Collect all metrics
        nlohmann::json metricsArray = nlohmann::json::array();
        for (const auto& metric : experiment->getMetrics()) {
            nlohmann::json metricData;
            metricData["name"] = metric.first;
            metricData["value"] = metric.second;
            metricsArray.push_back(metricData);
        }
        expData["metrics"] = metricsArray;
        
        // Collect per-step data
        nlohmann::json stepsArray = nlohmann::json::array();
        for (const auto& stepData : experiment->getStepData()) {
            nlohmann::json stepJson;
            stepJson["step"] = stepData.step;
            stepJson["neuron_count"] = stepData.neuronCount;
            stepJson["synapse_count"] = stepData.synapseCount;
            stepJson["firing_neuron_count"] = stepData.firingNeuronCount;
            stepJson["average_firing_rate"] = stepData.averageFiringRate;
            stepsArray.push_back(stepJson);
        }
        expData["steps"] = stepsArray;
        
        experimentsArray.push_back(expData);
    }
    
    results["experiments"] = experimentsArray;
    
    // Write JSON with pretty formatting
    file << results.dump(2);
}

// Helper function to save results as binary
void ExperimentRunner::saveResultsBinary(const std::string& filepath) const {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return;
    }
    
    // Binary format: header + experiment data
    struct BinaryHeader {
        uint32_t magic;  // File signature
        uint32_t version;
        uint64_t experimentCount;
        uint64_t timestamp;
    };
    
    BinaryHeader header;
    header.magic = 0x4E4C4D01;  // "NLM" in hex
    header.version = 1;
    header.experimentCount = pImpl->experiments.size();
    header.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    
    file.write(reinterpret_cast<const char*>(&header), sizeof(BinaryHeader));
    
    // Write experiment data
    for (const auto& experiment : pImpl->experiments) {
        // Write experiment metadata
        writeStringToBinary(file, experiment->getName());
        writeUint64ToBinary(file, experiment->getSeed());
        writeUint64ToBinary(file, experiment->getStartStep());
        writeUint64ToBinary(file, experiment->getEndStep());
        
        // Write metrics
        const auto& metrics = experiment->getMetrics();
        writeUint64ToBinary(file, metrics.size());
        for (const auto& metric : metrics) {
            writeStringToBinary(file, metric.first);
            writeDoubleToBinary(file, metric.second);
        }
        
        // Write step data
        const auto& stepData = experiment->getStepData();
        writeUint64ToBinary(file, stepData.size());
        for (const auto& step : stepData) {
            writeUint64ToBinary(file, step.step);
            writeUint64ToBinary(file, step.neuronCount);
            writeUint64ToBinary(file, step.synapseCount);
            writeUint64ToBinary(file, step.firingNeuronCount);
            writeDoubleToBinary(file, step.averageFiringRate);
        }
    }
}

// Helper function to save results as summary
void ExperimentRunner::saveResultsSummary(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    file << "=== NLM Experiment Results Summary ===\n\n";
    file << "Generated: " << std::chrono::system_clock::now() << "\n";
    file << "Total Experiments: " << pImpl->experiments.size() << "\n\n";
    
    for (const auto& experiment : pImpl->experiments) {
        file << "Experiment: " << experiment->getName() << "\n";
        file << "  Seed: " << experiment->getSeed() << "\n";
        file << "  Duration: " << experiment->getEndStep() - experiment->getStartStep() << " steps\n";
        
        // Summary statistics
        float totalReward = 0.0f;
        size_t totalSteps = 0;
        
        const auto& metrics = experiment->getMetrics();
        for (const auto& metric : metrics) {
            if (metric.first == "total_reward") {
                totalReward = static_cast<float>(metric.second);
            }
        }
        
        const auto& stepData = experiment->getStepData();
        totalSteps = stepData.size();
        
        file << "  Total Reward: " << totalReward << "\n";
        file << "  Average Reward per Step: " << (totalSteps > 0 ? totalReward / totalSteps : 0.0f) << "\n";
        file << "  Final Neuron Count: " << (stepData.empty() ? 0 : stepData.back().neuronCount) << "\n";
        file << "  Final Synapse Count: " << (stepData.empty() ? 0 : stepData.back().synapseCount) << "\n";
        file << "  Peak Firing Rate: " << (stepData.empty() ? 0.0f : getMaxFiringRate(stepData)) << " Hz\n";
        file << "\n";
    }
}

// Helper function to write string to binary file
void ExperimentRunner::writeStringToBinary(std::ofstream& file, const std::string& str) const {
    uint64_t length = str.length();
    file.write(reinterpret_cast<const char*>(&length), sizeof(uint64_t));
    if (length > 0) {
        file.write(str.c_str(), length);
    }
}

// Helper function to write uint64 to binary file
void ExperimentRunner::writeUint64ToBinary(std::ofstream& file, uint64_t value) const {
    file.write(reinterpret_cast<const char*>(&value), sizeof(uint64_t));
}

// Helper function to write double to binary file
void ExperimentRunner::writeDoubleToBinary(std::ofstream& file, double value) const {
    file.write(reinterpret_cast<const char*>(&value), sizeof(double));
}

// Helper function to get maximum firing rate from step data
float ExperimentRunner::getMaxFiringRate(const std::vector<Experiment::StepData>& stepData) const {
    float maxRate = 0.0f;
    for (const auto& step : stepData) {
        if (step.averageFiringRate > maxRate) {
            maxRate = step.averageFiringRate;
        }
    }
    return maxRate;
}

} // namespace nlm
