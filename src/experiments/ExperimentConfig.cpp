#include "ExperimentConfig.hpp"
#include "../core/Logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>

using json = nlohmann::json;

namespace nlm {

struct ExperimentConfig::Impl {
    std::string name;
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
    uint64_t seed;
    
    Impl() : maxSteps(10000), neuronCount(100), regionCount(1),
             connectionProbability(0.1f), enableCheckpointing(false),
             enableReplay(false), enableDevelopment(false), seed(42) {
        name = "DefaultExperiment";
        checkpointPath = "./experiment_checkpoint.bin";
    }
};

ExperimentConfig::ExperimentConfig() : pImpl(new Impl) {}

ExperimentConfig::ExperimentConfig(const std::string& name) : pImpl(new Impl) {
    pImpl->name = name;
}

ExperimentConfig::~ExperimentConfig() = default;

ExperimentConfig::ExperimentConfig(const ExperimentConfig& other) : pImpl(new Impl(*other.pImpl)) {}

ExperimentConfig& ExperimentConfig::operator=(const ExperimentConfig& other) {
    if (this != &other) {
        *pImpl = *other.pImpl;
    }
    return *this;
}

ExperimentConfig::ExperimentConfig(ExperimentConfig&& other) noexcept : pImpl(std::move(other.pImpl)) {}

ExperimentConfig& ExperimentConfig::operator=(ExperimentConfig&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

const std::string& ExperimentConfig::getName() const {
    return pImpl->name;
}

void ExperimentConfig::setName(const std::string& name) {
    if (name.empty()) {
        NLM_LOG_WARNING("Experiment name cannot be empty");
    }
    pImpl->name = name;
}

uint64_t ExperimentConfig::getMaxSteps() const {
    return pImpl->maxSteps;
}

void ExperimentConfig::setMaxSteps(uint64_t steps) {
    if (steps == 0) {
        throw NLMRuntimeError("Max steps cannot be zero");
    }
    pImpl->maxSteps = steps;
}

size_t ExperimentConfig::getNeuronCount() const {
    return pImpl->neuronCount;
}

void ExperimentConfig::setNeuronCount(size_t count) {
    if (count == 0) {
        throw NLMRuntimeError("Neuron count cannot be zero");
    }
    if (count > 1000000) {
        throw NLMRuntimeError("Neuron count cannot exceed 1,000,000");
    }
    pImpl->neuronCount = count;
}

size_t ExperimentConfig::getRegionCount() const {
    return pImpl->regionCount;
}

void ExperimentConfig::setRegionCount(size_t count) {
    if (count == 0) {
        throw NLMRuntimeError("Region count cannot be zero");
    }
    pImpl->regionCount = count;
}

float ExperimentConfig::getConnectionProbability() const {
    return pImpl->connectionProbability;
}

void ExperimentConfig::setConnectionProbability(float probability) {
    if (probability < 0.0f || probability > 1.0f) {
        throw NLMRuntimeError("Connection probability must be between 0.0 and 1.0");
    }
    pImpl->connectionProbability = probability;
}

bool ExperimentConfig::getEnableCheckpointing() const {
    return pImpl->enableCheckpointing;
}

void ExperimentConfig::setEnableCheckpointing(bool enable) {
    pImpl->enableCheckpointing = enable;
}

bool ExperimentConfig::getEnableReplay() const {
    return pImpl->enableReplay;
}

void ExperimentConfig::setEnableReplay(bool enable) {
    pImpl->enableReplay = enable;
}

bool ExperimentConfig::getEnableDevelopment() const {
    return pImpl->enableDevelopment;
}

void ExperimentConfig::setEnableDevelopment(bool enable) {
    pImpl->enableDevelopment = enable;
}

const std::string& ExperimentConfig::getCheckpointPath() const {
    return pImpl->checkpointPath;
}

void ExperimentConfig::setCheckpointPath(const std::string& path) {
    pImpl->checkpointPath = path;
}

uint64_t ExperimentConfig::getSeed() const {
    return pImpl->seed;
}

void ExperimentConfig::setSeed(uint64_t seed) {
    pImpl->seed = seed;
}

bool ExperimentConfig::validate() const {
    try {
        validateParameters();
        validateCrossParameterConstraints();
        return true;
    } catch (const NLMRuntimeError&) {
        return false;
    }
}

std::string ExperimentConfig::validateAndGetError() const {
    try {
        validateParameters();
        validateCrossParameterConstraints();
        return "Configuration is valid";
    } catch (const NLMRuntimeError& e) {
        return e.what();
    }
}

bool ExperimentConfig::saveToFile(const std::string& filepath) const {
    try {
        json j;
        
        j["name"] = pImpl->name;
        j["max_steps"] = pImpl->maxSteps;
        j["neuron_count"] = pImpl->neuronCount;
        j["region_count"] = pImpl->regionCount;
        j["connection_probability"] = pImpl->connectionProbability;
        j["enable_checkpointing"] = pImpl->enableCheckpointing;
        j["enable_replay"] = pImpl->enableReplay;
        j["enable_development"] = pImpl->enableDevelopment;
        j["checkpoint_path"] = pImpl->checkpointPath;
        j["seed"] = pImpl->seed;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw NLMRuntimeError("Cannot open file for writing: " + filepath);
        }
        
        file << j.dump(4);
        file.close();
        
        NLM_LOG_INFO("ExperimentConfig saved to " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save ExperimentConfig: " + std::string(e.what()));
        return false;
    }
}

bool ExperimentConfig::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw NLMRuntimeError("Cannot open file for reading: " + filepath);
        }
        
        json j;
        file >> j;
        file.close();
        
        pImpl->name = j.value("name", "DefaultExperiment");
        pImpl->maxSteps = j.value("max_steps", 10000);
        pImpl->neuronCount = j.value("neuron_count", 100);
        pImpl->regionCount = j.value("region_count", 1);
        pImpl->connectionProbability = j.value("connection_probability", 0.1f);
        pImpl->enableCheckpointing = j.value("enable_checkpointing", false);
        pImpl->enableReplay = j.value("enable_replay", false);
        pImpl->enableDevelopment = j.value("enable_development", false);
        pImpl->checkpointPath = j.value("checkpoint_path", "./experiment_checkpoint.bin");
        pImpl->seed = j.value("seed", 42);
        
        NLM_LOG_INFO("ExperimentConfig loaded from " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load ExperimentConfig: " + std::string(e.what()));
        return false;
    }
}

void ExperimentConfig::mergeWith(const ExperimentConfig& other) {
    if (!other.getName().empty()) {
        pImpl->name = other.getName();
    }
    pImpl->maxSteps = std::max(pImpl->maxSteps, other.getMaxSteps());
    pImpl->neuronCount = std::max(pImpl->neuronCount, other.getNeuronCount());
    pImpl->regionCount = std::max(pImpl->regionCount, other.getRegionCount());
    pImpl->connectionProbability = std::max(pImpl->connectionProbability, other.getConnectionProbability());
    
    // Only enable if both are enabled
    pImpl->enableCheckpointing = pImpl->enableCheckpointing && other.getEnableCheckpointing();
    pImpl->enableReplay = pImpl->enableReplay && other.getEnableReplay();
    pImpl->enableDevelopment = pImpl->enableDevelopment && other.getEnableDevelopment();
    
    if (!other.getCheckpointPath().empty()) {
        pImpl->checkpointPath = other.getCheckpointPath();
    }
    
    pImpl->seed = other.getSeed(); // Use other's seed for determinism
}

bool ExperimentConfig::equals(const ExperimentConfig& other) const {
    return pImpl->name == other.pImpl->name &&
           pImpl->maxSteps == other.pImpl->maxSteps &&
           pImpl->neuronCount == other.pImpl->neuronCount &&
           pImpl->regionCount == other.pImpl->regionCount &&
           pImpl->connectionProbability == other.pImpl->connectionProbability &&
           pImpl->enableCheckpointing == other.pImpl->enableCheckpointing &&
           pImpl->enableReplay == other.pImpl->enableReplay &&
           pImpl->enableDevelopment == other.pImpl->enableDevelopment &&
           pImpl->checkpointPath == other.pImpl->checkpointPath &&
           pImpl->seed == other.pImpl->seed;
}

ExperimentConfig ExperimentConfig::createQuickTestConfig() {
    ExperimentConfig config("QuickTest");
    config.setMaxSteps(100);
    config.setNeuronCount(50);
    config.setRegionCount(1);
    config.setConnectionProbability(0.05f);
    config.setEnableCheckpointing(false);
    config.setEnableReplay(false);
    config.setEnableDevelopment(false);
    return config;
}

ExperimentConfig ExperimentConfig::createTrainingConfig() {
    ExperimentConfig config("Training");
    config.setMaxSteps(10000);
    config.setNeuronCount(500);
    config.setRegionCount(2);
    config.setConnectionProbability(0.1f);
    config.setEnableCheckpointing(true);
    config.setEnableReplay(true);
    config.setEnableDevelopment(true);
    config.setCheckpointPath("./training_checkpoint.bin");
    return config;
}

ExperimentConfig ExperimentConfig::createBenchmarkConfig() {
    ExperimentConfig config("Benchmark");
    config.setMaxSteps(5000);
    config.setNeuronCount(1000);
    config.setRegionCount(3);
    config.setConnectionProbability(0.15f);
    config.setEnableCheckpointing(true);
    config.setEnableReplay(true);
    config.setEnableDevelopment(true);
    config.setCheckpointPath("./benchmark_checkpoint.bin");
    return config;
}

ExperimentConfig ExperimentConfig::createAblationConfig() {
    ExperimentConfig config("Ablation");
    config.setMaxSteps(2000);
    config.setNeuronCount(200);
    config.setRegionCount(1);
    config.setConnectionProbability(0.1f);
    config.setEnableCheckpointing(true);
    config.setEnableReplay(false);
    config.setEnableDevelopment(false);
    config.setCheckpointPath("./ablation_checkpoint.bin");
    return config;
}

void ExperimentConfig::validateParameters() const {
    if (pImpl->name.empty()) {
        throw NLMRuntimeError("Experiment name cannot be empty");
    }
    
    if (pImpl->maxSteps == 0) {
        throw NLMRuntimeError("Max steps cannot be zero");
    }
    
    if (pImpl->maxSteps > 1000000) {
        throw NLMRuntimeError("Max steps cannot exceed 1,000,000");
    }
    
    if (pImpl->neuronCount == 0) {
        throw NLMRuntimeError("Neuron count cannot be zero");
    }
    
    if (pImpl->neuronCount > 1000000) {
        throw NLMRuntimeError("Neuron count cannot exceed 1,000,000");
    }
    
    if (pImpl->regionCount == 0) {
        throw NLMRuntimeError("Region count cannot be zero");
    }
    
    if (pImpl->regionCount > 100) {
        throw NLMRuntimeError("Region count cannot exceed 100");
    }
    
    if (pImpl->connectionProbability < 0.0f || pImpl->connectionProbability > 1.0f) {
        throw NLMRuntimeError("Connection probability must be between 0.0 and 1.0");
    }
    
    if (pImpl->checkpointPath.empty() && pImpl->enableCheckpointing) {
        throw NLMRuntimeError("Checkpoint path cannot be empty when checkpointing is enabled");
    }
}

void ExperimentConfig::validateCrossParameterConstraints() const {
    // Ensure connection probability is appropriate for the neuron and region counts
    double expectedConnections = (double)pImpl->neuronCount * (double)pImpl->regionCount * pImpl->connectionProbability;
    if (expectedConnections > 1000000) {
        NLM_LOG_WARNING("High expected connection count may impact performance");
    }
}

} // namespace nlm
