#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>

namespace nlm {

struct BrainBuilder::Impl {
    // Configuration parameters
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    bool developmentEnabled;
    DevelopmentalStage developmentalStage;
    bool memoryEnabled;
    size_t memoryCapacity;
    bool predictionEnabled;
    bool cognitionEnabled;
    bool neuromodulationEnabled;
    PlasticityRule::Type plasticityRuleType;
    std::unordered_map<std::string, float> plasticityParameters;
    std::vector<std::shared_ptr<BrainObserver>> observers;
    
    Impl() : neuronCount(1000), regionCount(1), connectionProbability(0.1f),
             developmentEnabled(true), developmentalStage(DevelopmentalStage::Initial),
             memoryEnabled(true), memoryCapacity(10000), predictionEnabled(true),
             cognitionEnabled(true), neuromodulationEnabled(true),
             plasticityRuleType(PlasticityRule::Type::STDP) {}
};

BrainBuilder::BrainBuilder() : pImpl(new Impl()) {}

BrainBuilder::~BrainBuilder() {
    delete pImpl;
}

BrainBuilder& BrainBuilder::withNeuronCount(size_t count) {
    pImpl->neuronCount = count;
    return *this;
}

BrainBuilder& BrainBuilder::withRegionCount(size_t count) {
    pImpl->regionCount = count;
    return *this;
}

BrainBuilder& BrainBuilder::withConnectionProbability(float probability) {
    pImpl->connectionProbability = std::max(0.0f, std::min(1.0f, probability));
    return *this;
}

BrainBuilder& BrainBuilder::withDevelopment(bool enable) {
    pImpl->developmentEnabled = enable;
    return *this;
}

BrainBuilder& BrainBuilder::withDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
    return *this;
}

BrainBuilder& BrainBuilder::withMemorySystems(bool enable) {
    pImpl->memoryEnabled = enable;
    return *this;
}

BrainBuilder& BrainBuilder::withMemoryCapacity(size_t capacity) {
    pImpl->memoryCapacity = capacity;
    return *this;
}

BrainBuilder& BrainBuilder::withPredictionSystem(bool enable) {
    pImpl->predictionEnabled = enable;
    return *this;
}

BrainBuilder& BrainBuilder::withCognitionSystems(bool enable) {
    pImpl->cognitionEnabled = enable;
    return *this;
}

BrainBuilder& BrainBuilder::withNeuromodulationSystems(bool enable) {
    pImpl->neuromodulationEnabled = enable;
    return *this;
}

BrainBuilder& BrainBuilder::withPlasticityRule(PlasticityRule::Type ruleType,
                                              const std::unordered_map<std::string, float>& parameters) {
    pImpl->plasticityRuleType = ruleType;
    pImpl->plasticityParameters = parameters;
    return *this;
}

BrainBuilder& BrainBuilder::withObserver(std::shared_ptr<BrainObserver> observer) {
    if (observer) {
        pImpl->observers.push_back(observer);
    }
    return *this;
}

std::shared_ptr<Brain> BrainBuilder::build() {
    // Create configuration from builder parameters
    auto config = std::make_shared<Config>();
    
    // Add configuration parameters
    config->set("neuron_count", pImpl->neuronCount);
    config->set("region_count", pImpl->regionCount);
    config->set("connection_probability", pImpl->connectionProbability);
    config->set("simulation_timestep", 0.001);
    config->set("replay_interval", 100);
    config->set("consolidation_interval", 1000);
    
    if (pImpl->developmentEnabled) {
        config->set("developmental_stage", static_cast<int>(pImpl->developmentalStage));
    }
    
    if (pImpl->memoryEnabled) {
        config->set("memory_capacity", pImpl->memoryCapacity);
    }
    
    // Create brain with configuration
    auto brain = std::shared_ptr<Brain>(new Brain(config));
    
    // Store observers in brain for later use
    for (const auto& observer : pImpl->observers) {
        brain->addObserver(observer);
    }
    
    return brain;
}

std::shared_ptr<Brain> BrainBuilder::buildAndInitialize() {
    auto brain = build();
    if (brain) {
        brain->initialize(brain->getConfig());
    }
    return brain;
}

std::shared_ptr<Brain> Brain::createDefaultBrain() {
    BrainBuilder builder = createBrainBuilder();
    return builder.buildAndInitialize();
}

std::shared_ptr<Brain> Brain::createBrain(std::shared_ptr<Config> config) {
    if (!config) {
        throw std::invalid_argument("Configuration cannot be null");
    }
    
    auto brain = std::shared_ptr<Brain>(new Brain(config));
    if (brain) {
        brain->initialize(config);
    }
    return brain;
}

std::shared_ptr<Brain> Brain::createBrain(const std::string& configFilePath) {
    if (configFilePath.empty()) {
        throw std::invalid_argument("Configuration file path cannot be empty");
    }
    
    // Load configuration from file
    auto config = std::make_shared<Config>();
    std::ifstream file(configFilePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + configFilePath);
        return nullptr;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Simple parsing: key=value
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            config->set(key, value);
        }
    }
    
    return createBrain(config);
}

BrainBuilder Brain::createBrainBuilder() {
    return BrainBuilder();
}

} // namespace nlm