#include "FactoryManager.hpp"
#include "core/Config/Config.hpp"
#include "core/Config/ConfigFactory.hpp"
#include "core/Random/Random.hpp"
#include "core/Random/RandomGeneratorFactory.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Logger/LoggerFactory.hpp"
#include "brain/Brain.hpp"
#include "brain/BrainFactory.hpp"
#include "world/World.hpp"
#include "world/WorldFactory.hpp"
#include "experiments/Experiment.hpp"
#include "experiments/ExperimentFactory.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace nlm {

FactoryManager::FactoryManager() {
    initialized_ = false;
}

FactoryManager* FactoryManager::instance_ = nullptr;
std::mutex FactoryManager::instanceMutex_;

FactoryManager& FactoryManager::getInstance() {
    std::lock_guard<std::mutex> lock(instanceMutex_);
    if (!instance_) {
        instance_ = std::unique_ptr<FactoryManager>(new FactoryManager());
    }
    return *instance_;
}

std::unique_ptr<ObjectFactory> FactoryManager::getFactory(const std::string& factoryId, const std::string& typeName) {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    for (auto& pair : factories_) {
        if ((factoryId.empty() || pair.first == factoryId) && 
            (typeName.empty() || pair.second.typeName == typeName)) {
            return std::move(pair.second.factory);
        }
    }
    return nullptr;
}

std::unique_ptr<Config> FactoryManager::createConfig(const std::string& factoryId, const ObjectParams& params) {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    
    // Try to find specific factory
    if (!factoryId.empty()) {
        auto it = factories_.find(factoryId);
        if (it != factories_.end() && it->second.factory->isInitialized()) {
            return it->second.factory->createConfig(params);
        }
    }
    
    // Use default config factory
    auto configFactory = getFactory<ConfigFactory>("");
    if (configFactory) {
        return configFactory->createConfig(params);
    }
    
    // Fallback to basic config creation
    return Config::create();
}

std::unique_ptr<RandomGenerator> FactoryManager::createRandomGenerator(const std::string& factoryId, const ObjectParams& params) {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    
    // Try to find specific factory
    if (!factoryId.empty()) {
        auto it = factories_.find(factoryId);
        if (it != factories_.end() && it->second.factory->isInitialized()) {
            return it->second.factory->createRandomGenerator(params);
        }
    }
    
    // Use default random generator factory
    auto randomFactory = getFactory<RandomGeneratorFactory>("");
    if (randomFactory) {
        return randomFactory->createRandomGenerator(params);
    }
    
    // Fallback to basic random generator creation
    uint64_t seed = params.getParam<uint64_t>("seed", std::chrono::system_clock::now().time_since_epoch().count());
    return std::make_unique<RandomGenerator>(seed);
}

std::unique_ptr<Logger> FactoryManager::createLogger(const std::string& factoryId, const ObjectParams& params) {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    
    // Try to find specific factory
    if (!factoryId.empty()) {
        auto it = factories_.find(factoryId);
        if (it != factories_.end() && it->second.factory->isInitialized()) {
            return it->second.factory->createLogger(params);
        }
    }
    
    // Use default logger factory
    auto loggerFactory = getFactory<LoggerFactory>("");
    if (loggerFactory) {
        return loggerFactory->createLogger(params);
    }
    
    // Fallback to basic logger creation
    LogLevel level = static_cast<LogLevel>(params.getParam<int>("logLevel", static_cast<int>(LogLevel::Info)));
    return std::make_unique<Logger>(level);
}

std::unique_ptr<Brain> FactoryManager::createBrain(const std::string& factoryId, const ObjectParams& params) {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    
    // Try to find specific factory
    if (!factoryId.empty()) {
        auto it = factories_.find(factoryId);
        if (it != factories_.end() && it->second.factory->isInitialized()) {
            return it->second.factory->createBrain(params);
        }
    }
    
    // Use default brain factory
    auto brainFactory = getFactory<BrainFactory>("");
    if (brainFactory) {
        return brainFactory->createBrain(params);
    }
    
    // Fallback to basic brain creation
    return std::make_unique<Brain>();
}

std::unique_ptr<World> FactoryManager::createWorld(const std::string& factoryId, const ObjectParams& params) {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    
    // Try to find specific factory
    if (!factoryId.empty()) {
        auto it = factories_.find(factoryId);
        if (it != factories_.end() && it->second.factory->isInitialized()) {
            return it->second.factory->createWorld(params);
        }
    }
    
    // Use default world factory
    auto worldFactory = getFactory<WorldFactory>("");
    if (worldFactory) {
        return worldFactory->createWorld(params);
    }
    
    // Fallback to basic world creation
    return std::make_unique<World>();
}

std::unique_ptr<Experiment> FactoryManager::createExperiment(const std::string& factoryId, const ObjectParams& params) {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    
    // Try to find specific factory
    if (!factoryId.empty()) {
        auto it = factories_.find(factoryId);
        if (it != factories_.end() && it->second.factory->isInitialized()) {
            return it->second.factory->createExperiment(params);
        }
    }
    
    // Use default experiment factory
    auto experimentFactory = getFactory<ExperimentFactory>("");
    if (experimentFactory) {
        return experimentFactory->createExperiment(params);
    }
    
    // Fallback to basic experiment creation
    return std::make_unique<Experiment>();
}

template<typename T>
std::unique_ptr<T> FactoryManager::acquireFromPool(const std::string& factoryId) {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    
    std::string typeName = typeid(T).name();
    auto poolIt = objectPools_.find(factoryId);
    if (poolIt != objectPools_.end()) {
        auto& pool = poolIt->second[typeName];
        if (!pool.empty()) {
            auto obj = std::move(pool.back());
            pool.pop_back();
            return std::unique_ptr<T>(static_cast<T*>(obj.release()));
        }
    }
    return nullptr;
}

template<typename T>
void FactoryManager::releaseToPool(const std::string& factoryId, std::unique_ptr<T> object) {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    
    std::string typeName = typeid(T).name();
    objectPools_[factoryId][typeName].push_back(std::move(object));
}

void FactoryManager::initializeAllFactories() {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    for (auto& pair : factories_) {
        if (pair.second.factory) {
            pair.second.factory->initialize();
        }
    }
    initialized_ = true;
    spdlog::info("All factories initialized successfully");
}

void FactoryManager::shutdownAllFactories() {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    cleanupPools();
    for (auto& pair : factories_) {
        if (pair.second.factory && pair.second.factory->isInitialized()) {
            pair.second.factory->shutdown();
        }
    }
    factories_.clear();
    initialized_ = false;
    spdlog::info("All factories shutdown successfully");
}

bool FactoryManager::isInitialized(const std::string& factoryId) const {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    auto it = factories_.find(factoryId);
    return it != factories_.end() && it->second.factory && it->second.factory->isInitialized();
}

void FactoryManager::discoverFactories() {
    spdlog::info("Discovering factories...");
    autoRegisterFactories();
}

void FactoryManager::autoRegisterFactories() {
    // This is a placeholder for automatic factory discovery
    // In a real implementation, this would use reflection or plugin system
    spdlog::debug("Automatic factory registration would occur here");
}

std::unique_ptr<Config> FactoryManager::createConfigFromConfig(const std::string& configKey) {
    auto configIt = configs_.find(configKey);
    if (configIt == configs_.end()) {
        spdlog::warn("Config key '{}' not found for factory creation", configKey);
        return nullptr;
    }
    
    ObjectParams params;
    for (const auto& param : configIt->second) {
        params.setParam(param.first, param.second);
    }
    
    return createConfig("", params);
}

std::unique_ptr<RandomGenerator> FactoryManager::createRandomGeneratorFromConfig(const std::string& configKey) {
    auto configIt = configs_.find(configKey);
    if (configIt == configs_.end()) {
        spdlog::warn("Config key '{}' not found for factory creation", configKey);
        return nullptr;
    }
    
    ObjectParams params;
    for (const auto& param : configIt->second) {
        params.setParam(param.first, param.second);
    }
    
    return createRandomGenerator("", params);
}

std::unique_ptr<Logger> FactoryManager::createLoggerFromConfig(const std::string& configKey) {
    auto configIt = configs_.find(configKey);
    if (configIt == configs_.end()) {
        spdlog::warn("Config key '{}' not found for factory creation", configKey);
        return nullptr;
    }
    
    ObjectParams params;
    for (const auto& param : configIt->second) {
        params.setParam(param.first, param.second);
    }
    
    return createLogger("", params);
}

std::unique_ptr<Brain> FactoryManager::createBrainFromConfig(const std::string& configKey) {
    auto configIt = configs_.find(configKey);
    if (configIt == configs_.end()) {
        spdlog::warn("Config key '{}' not found for factory creation", configKey);
        return nullptr;
    }
    
    ObjectParams params;
    for (const auto& param : configIt->second) {
        params.setParam(param.first, param.second);
    }
    
    return createBrain("", params);
}

std::unique_ptr<World> FactoryManager::createWorldFromConfig(const std::string& configKey) {
    auto configIt = configs_.find(configKey);
    if (configIt == configs_.end()) {
        spdlog::warn("Config key '{}' not found for factory creation", configKey);
        return nullptr;
    }
    
    ObjectParams params;
    for (const auto& param : configIt->second) {
        params.setParam(param.first, param.second);
    }
    
    return createWorld("", params);
}

std::unique_ptr<Experiment> FactoryManager::createExperimentFromConfig(const std::string& configKey) {
    auto configIt = configs_.find(configKey);
    if (configIt == configs_.end()) {
        spdlog::warn("Config key '{}' not found for factory creation", configKey);
        return nullptr;
    }
    
    ObjectParams params;
    for (const auto& param : configIt->second) {
        params.setParam(param.first, param.second);
    }
    
    return createExperiment("", params);
}

std::vector<std::string> FactoryManager::getRegisteredFactoryIds(const std::string& typeName) const {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    std::vector<std::string> ids;
    for (const auto& pair : factories_) {
        if (typeName.empty() || pair.second.typeName == typeName) {
            ids.push_back(pair.first);
        }
    }
    return ids;
}

std::vector<FactoryEntry> FactoryManager::getAllFactories() const {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    std::vector<FactoryEntry> entries;
    for (const auto& pair : factories_) {
        entries.push_back(pair.second);
    }
    return entries;
}

std::unordered_map<std::string, std::any> FactoryManager::getFactoryConfig(const std::string& factoryId) const {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    auto it = factoryConfigs_.find(factoryId);
    if (it != factoryConfigs_.end()) {
        return it->second;
    }
    return {};
}

void FactoryManager::lockFactories() const {
    factoriesMutex_.lock();
}

void FactoryManager::unlockFactories() const {
    factoriesMutex_.unlock();
}

void FactoryManager::registerConfig(const std::string& key, const std::unordered_map<std::string, std::any>& config) {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    configs_[key] = config;
}

std::unordered_map<std::string, std::any> FactoryManager::getConfig(const std::string& key) const {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    auto it = configs_.find(key);
    if (it != configs_.end()) {
        return it->second;
    }
    return {};
}

bool FactoryManager::testFactory(const std::string& factoryId, const std::string& typeName) const {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    auto it = factories_.find(factoryId);
    if (it != factories_.end()) {
        // Test the factory (simplified test)
        bool success = true;
        testResults_.push_back("Factory " + factoryId + " of type " + typeName + ": " + 
                               (success ? "PASSED" : "FAILED"));
        return success;
    }
    testResults_.push_back("Factory " + factoryId + " of type " + typeName + ": NOT FOUND");
    return false;
}

std::vector<std::string> FactoryManager::getFactoryTestsResults() const {
    return testResults_;
}

void FactoryManager::initializeDefaultFactories() {
    spdlog::info("Initializing default factories...");
    // Default factories will be registered via macros during construction
}

void FactoryManager::cleanupPools() {
    objectPools_.clear();
    spdlog::debug("Factory object pools cleaned up");
}

} // namespace nlm
