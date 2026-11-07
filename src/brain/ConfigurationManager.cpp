#include "ConfigurationManager.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

ConfigurationManager::ConfigurationManager(Brain* brain)
    : brain_(brain)
    , rng(nullptr)
    , logger(std::make_unique<Logger>()) {
}

ConfigurationManager::~ConfigurationManager() = default;

bool ConfigurationManager::initialize() {
    NLM_LOG_INFO("Initializing Configuration Manager...");
    
    if (!brain_) return false;
    
    // Initialize random generator
    uint64_t seed = 42;
    if (auto seedOpt = brain_->pImpl->config->get<int64_t>("random_seed")) {
        seed = *seedOpt;
    }
    rng = std::make_unique<RandomGenerator>(seed);
    
    // Initialize logger
    auto consoleLogger = std::make_unique<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger.get());
    
    NLM_LOG_INFO("Configuration Manager initialized successfully");
    return true;
}

void ConfigurationManager::reset() {
    // Configuration is constant
}

void ConfigurationManager::step(double timestep) {
    // Configuration doesn't change over time
}

float ConfigurationManager::getSimulationTimestep() const {
    if (brain_) {
        return brain_->pImpl->config->getOr<double>("simulation_timestep", 0.001);
    }
    return 0.001;
}

size_t ConfigurationManager::getNeuronCount() const {
    if (brain_) {
        return static_cast<size_t>(brain_->pImpl->config->getOr<int64_t>("neuron_count", 1000));
    }
    return 1000;
}

size_t ConfigurationManager::getRegionCount() const {
    if (brain_) {
        return static_cast<size_t>(brain_->pImpl->config->getOr<int64_t>("region_count", 1));
    }
    return 1;
}

float ConfigurationManager::getConnectionProbability() const {
    if (brain_) {
        return brain_->pImpl->config->getOr<float>("connection_probability", 0.1f);
    }
    return 0.1f;
}

} // namespace nlm
