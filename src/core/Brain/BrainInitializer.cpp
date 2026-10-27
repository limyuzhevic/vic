#include "BrainInitializer.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>

using namespace nlm;

std::shared_ptr<Brain> BrainInitializer::createBrain(std::shared_ptr<ConfigurationManager> config) {
    if (!config) {
        NLM_LOG_ERROR("Cannot create brain: Configuration is null");
        return nullptr;
    }
    
    NLM_LOG_INFO("Creating NLM brain with configuration...");
    
    // Check configuration is complete
    if (!config->isComplete()) {
        NLM_LOG_ERROR("Configuration is incomplete");
        auto errors = config->validateConfiguration();
        for (const auto& error : errors) {
            NLM_LOG_ERROR("  Validation error: " + error);
        }
        return nullptr;
    }
    
    // Create brain with validated configuration
    auto brain = std::make_shared<Brain>(config->getConfigFilePath().empty() ? 
                                          nullptr : config->getConfigFilePath());
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return nullptr;
    }
    
    initializationCount_++;
    totalBrainSize_ += brain->getTotalNeuronCount();
    
    NLM_LOG_INFO("Brain created successfully with " + 
                std::to_string(brain->getTotalNeuronCount()) + 
                " neurons and " + std::to_string(brain->getTotalSynapseCount()) + 
                " synapses");
    
    return brain;
}

bool BrainInitializer::resetBrain(std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot reset brain: Brain is null");
        return false;
    }
    
    NLM_LOG_INFO("Resetting brain to initial state...");
    
    if (!brain->reset()) {
        NLM_LOG_ERROR("Failed to reset brain");
        return false;
    }
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to reinitialize brain after reset");
        return false;
    }
    
    NLM_LOG_INFO("Brain reset and reinitialized successfully");
    return true;
}

void BrainInitializer::logBrainStatus(std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot log status: Brain is null");
        return;
    }
    
    NLM_LOG_INFO("=== Brain Status ===");
    brain->logStatus();
    
    // Additional statistics
    NLM_LOG_INFO("Initialization stats: " + getInitializationStats());
}

bool BrainInitializer::validateBrainState(std::shared_ptr<Brain> brain) const {
    if (!brain) {
        return false;
    }
    
    // Check basic brain properties
    if (brain->getTotalNeuronCount() == 0) {
        NLM_LOG_WARNING("Brain has no neurons");
        return false;
    }
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_WARNING("Brain has no regions");
        return false;
    }
    
    if (region->getAllNeurons().empty()) {
        NLM_LOG_WARNING("Brain region has no neurons");
        return false;
    }
    
    NLM_LOG_INFO("Brain state validation passed");
    return true;
}

std::string BrainInitializer::getInitializationStats() const {
    std::ostringstream oss;
    oss << "Initializations: " << initializationCount_ << ", "
        << "Average brain size: " << (initializationCount_ > 0 ? 
            totalBrainSize_ / initializationCount_ : 0) << " neurons";
    return oss.str();
}

void BrainInitializer::setupNetworkTopology(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!brain || !config) return;
    
    // Configure network topology based on configuration
    int64_t neuronCount = config->getOr<int64_t>("neuron_count", 500);
    int64_t regionCount = config->getOr<int64_t>("region_count", 1);
    float connectionProbability = config->getOr<float>("connection_probability", 0.15f);
    
    NLM_LOG_INFO("Setting up network topology: " + std::to_string(neuronCount) + 
                " neurons, " + std::to_string(regionCount) + " regions, " +
                std::to_string(connectionProbability * 100) + "% connection probability");
}

void BrainInitializer::setupSynapticConnections(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!brain || !config) return;
    
    NLM_LOG_INFO("Setting up synaptic connections");
}

void BrainInitializer::enablePlasticity(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!brain || !config) return;
    
    bool enableHebbian = config->getOr<bool>("enable_hebbian", true);
    bool enableStdp = config->getOr<bool>("enable_stdp", true);
    bool enableStructural = config->getOr<bool>("enable_structural", true);
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(enableHebbian, enableStdp, enableStructural);
        }
    }
    
    NLM_LOG_INFO("Plasticity enabled - Hebbian: " + std::to_string(enableHebbian) +
                ", STDP: " + std::to_string(enableStdp) +
                ", Structural: " + std::to_string(enableStructural));
}