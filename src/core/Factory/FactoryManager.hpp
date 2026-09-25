#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <functional>
#include <any>
#include <typeindex>
#include <spdlog/spdlog.h>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class Logger;
class Brain;
class World;
class Experiment;

// Object creation parameters
struct ObjectParams {
    std::unordered_map<std::string, std::any> params;
    std::string name;
    bool enableObjectPool = false;
    size_t poolSize = 100;
    bool threadSafe = true;
    
    template<typename T>
    T getParam(const std::string& key, const T& defaultValue = T{}) const {
        auto it = params.find(key);
        if (it != params.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (const std::bad_any_cast&) {
                spdlog::error("Parameter cast failed for key: {}", key);
            }
        }
        return defaultValue;
    }
    
    void setParam(const std::string& key, const std::any& value) {
        params[key] = value;
    }
};

// Base factory interface
class ObjectFactory {
public:
    virtual ~ObjectFactory() = default;
    
    // Pure virtual creation methods
    virtual std::unique_ptr<Config> createConfig(const ObjectParams& params = {}) = 0;
    virtual std::unique_ptr<RandomGenerator> createRandomGenerator(const ObjectParams& params = {}) = 0;
    virtual std::unique_ptr<Logger> createLogger(const ObjectParams& params = {}) = 0;
    virtual std::unique_ptr<Brain> createBrain(const ObjectParams& params = {}) = 0;
    virtual std::unique_ptr<World> createWorld(const ObjectParams& params = {}) = 0;
    virtual std::unique_ptr<Experiment> createExperiment(const ObjectParams& params = {}) = 0;
    
    // Factory metadata
    virtual std::string getFactoryName() const = 0;
    virtual std::string getSupportedType() const = 0;
    virtual void configure(const std::unordered_map<std::string, std::any>& config) = 0;
    
    // Factory lifecycle
    virtual void initialize() {}
    virtual void shutdown() {}
    virtual bool isInitialized() const = 0;
    
    // Factory discovery registration
    virtual void registerFactory() = 0;
    
    // Factory cloning
    virtual std::unique_ptr<ObjectFactory> clone() const = 0;
};

// Factory registry entry
struct FactoryEntry {
    std::string factoryId;
    std::string typeName;
    std::unique_ptr<ObjectFactory> factory;
    bool isDefault = false;
    std::unordered_map<std::string, std::any> config;
    
    FactoryEntry(std::string id, std::string type, std::unique_ptr<ObjectFactory> fact)
        : factoryId(std::move(id)), typeName(std::move(type)), factory(std::move(fact)) {}
};

// Global factory manager for registry and lifecycle management
class FactoryManager {
public:
    static FactoryManager& getInstance();
    
    // Factory registry
    template<typename FactoryType>
    void registerFactory(const std::string& factoryId, const std::string& typeName, bool isDefault = false);
    
    template<typename FactoryType>
    std::unique_ptr<FactoryType> getFactory(const std::string& factoryId);
    
    std::unique_ptr<ObjectFactory> getFactory(const std::string& factoryId, const std::string& typeName);
    
    // Factory creation with dependency injection
    std::unique_ptr<Config> createConfig(const std::string& factoryId = "", const ObjectParams& params = {});
    std::unique_ptr<RandomGenerator> createRandomGenerator(const std::string& factoryId = "", const ObjectParams& params = {});
    std::unique_ptr<Logger> createLogger(const std::string& factoryId = "", const ObjectParams& params = {});
    std::unique_ptr<Brain> createBrain(const std::string& factoryId = "", const ObjectParams& params = {});
    std::unique_ptr<World> createWorld(const std::string& factoryId = "", const ObjectParams& params = {});
    std::unique_ptr<Experiment> createExperiment(const std::string& factoryId = "", const ObjectParams& params = {});
    
    // Object pooling for performance
    template<typename T>
    std::unique_ptr<T> acquireFromPool(const std::string& factoryId);
    
    template<typename T>
    void releaseToPool(const std::string& factoryId, std::unique_ptr<T> object);
    
    // Factory lifecycle management
    void initializeAllFactories();
    void shutdownAllFactories();
    bool isInitialized(const std::string& factoryId) const;
    
    // Factory discovery and automatic registration
    void discoverFactories();
    void autoRegisterFactories();
    
    // Configuration-based factory creation
    std::unique_ptr<Config> createConfigFromConfig(const std::string& configKey);
    std::unique_ptr<RandomGenerator> createRandomGeneratorFromConfig(const std::string& configKey);
    std::unique_ptr<Logger> createLoggerFromConfig(const std::string& configKey);
    std::unique_ptr<Brain> createBrainFromConfig(const std::string& configKey);
    std::unique_ptr<World> createWorldFromConfig(const std::string& configKey);
    std::unique_ptr<Experiment> createExperimentFromConfig(const std::string& configKey);
    
    // Factory metadata and status
    std::vector<std::string> getRegisteredFactoryIds(const std::string& typeName = "") const;
    std::vector<FactoryEntry> getAllFactories() const;
    std::unordered_map<std::string, std::any> getFactoryConfig(const std::string& factoryId) const;
    
    // Thread safety
    void lockFactories() const;
    void unlockFactories() const;
    
    // Configuration management
    void registerConfig(const std::string& key, const std::unordered_map<std::string, std::any>& config);
    std::unordered_map<std::string, std::any> getConfig(const std::string& key) const;
    
    // Factory testing
    bool testFactory(const std::string& factoryId, const std::string& typeName) const;
    std::vector<std::string> getFactoryTestsResults() const;
    
    // Private constructor for singleton
    FactoryManager();
    FactoryManager(const FactoryManager&) = delete;
    FactoryManager& operator=(const FactoryManager&) = delete;
    
private:
    static std::unique_ptr<FactoryManager> instance_;
    static std::mutex instanceMutex_;
    
    mutable std::mutex factoriesMutex_;
    std::unordered_map<std::string, FactoryEntry> factories_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::any>> factoryConfigs_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::any>> objectPools_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::any>> configs_;
    std::vector<std::string> testResults_;
    bool initialized_ = false;
    
    void initializeDefaultFactories();
    void cleanupPools();
};

// Macro for factory registration
#define REGISTER_FACTORY(FactoryClass, factoryId, typeName, isDefault) \
    namespace { \
        struct FactoryRegistrar##__LINE__ { \
            FactoryRegistrar##__LINE__() { \
                FactoryManager::getInstance().template registerFactory<FactoryClass>(factoryId, typeName, isDefault); \
            } \
        }; \
        FactoryRegistrar##__LINE__ factoryRegistrarInstance##__LINE__; \
    }

// Template implementations

inline FactoryManager& FactoryManager::getInstance() {
    std::lock_guard<std::mutex> lock(instanceMutex_);
    if (!instance_) {
        instance_ = std::unique_ptr<FactoryManager>(new FactoryManager());
    }
    return *instance_;
}

template<typename FactoryType>
inline void FactoryManager::registerFactory(const std::string& factoryId, const std::string& typeName, bool isDefault) {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    factories_.emplace(factoryId, FactoryEntry(factoryId, typeName, std::make_unique<FactoryType>()));
    factories_[factoryId].isDefault = isDefault;
}

template<typename FactoryType>
inline std::unique_ptr<FactoryType> FactoryManager::getFactory(const std::string& factoryId) {
    std::lock_guard<std::mutex> lock(factoriesMutex_);
    auto it = factories_.find(factoryId);
    if (it != factories_.end()) {
        return std::unique_ptr<FactoryType>(static_cast<FactoryType*>(it->second.factory.release()));
    }
    return nullptr;
}

// Macro for factory registration
#define REGISTER_FACTORY(FactoryClass, factoryId, typeName, isDefault) \
    namespace { \
        struct FactoryRegistrar##__LINE__ { \
            FactoryRegistrar##__LINE__() { \
                FactoryManager::getInstance().template registerFactory<FactoryClass>(factoryId, typeName, isDefault); \
            } \
        }; \
        FactoryRegistrar##__LINE__ factoryRegistrarInstance##__LINE__; \
    }

} // namespace nlm

