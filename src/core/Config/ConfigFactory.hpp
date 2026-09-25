#pragma once

#include "core/Factory/FactoryManager.hpp"
#include "Config.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <any>
#include <mutex>
#include <spdlog/spdlog.h>

namespace nlm {

class ConfigFactory : public ObjectFactory {
public:
    ConfigFactory() = default;
    ~ConfigFactory() override = default;
    
    // ObjectFactory interface implementation
    std::unique_ptr<Config> createConfig(const ObjectParams& params = {}) override;
    std::unique_ptr<RandomGenerator> createRandomGenerator(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by ConfigFactory
    }
    std::unique_ptr<Logger> createLogger(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by ConfigFactory
    }
    std::unique_ptr<Brain> createBrain(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by ConfigFactory
    }
    std::unique_ptr<World> createWorld(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by ConfigFactory
    }
    std::unique_ptr<Experiment> createExperiment(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by ConfigFactory
    }
    
    // Factory metadata
    std::string getFactoryName() const override {
        return "ConfigFactory";
    }
    
    std::string getSupportedType() const override {
        return "Config";
    }
    
    void configure(const std::unordered_map<std::string, std::any>& config) override {
        config_ = config;
        spdlog::info("ConfigFactory configured with {} parameters", config.size());
    }
    
    // Factory lifecycle
    bool isInitialized() const override {
        return initialized_;
    }
    
    void initialize() override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!initialized_) {
            spdlog::info("ConfigFactory initializing");
            initialized_ = true;
        }
    }
    
    void shutdown() override {
        std::lock_guard<std::mutex> lock(mutex_);
        spdlog::info("ConfigFactory shutting down");
        initialized_ = false;
    }
    
    // Factory discovery registration
    void registerFactory() override {
        FactoryManager::getInstance().registerFactory<ConfigFactory>(getFactoryName(), getSupportedType(), true);
    }
    
    // Factory cloning
    std::unique_ptr<ObjectFactory> clone() const override {
        return std::make_unique<ConfigFactory>(*this);
    }
    
    // Factory registration macro
    REGISTER_FACTORY(ConfigFactory, "default_config", "Config", true)
    
    // Factory registration macro
    REGISTER_FACTORY(ConfigFactory, "default_config", "Config", true)
    
    // Config-specific factory methods
    std::unique_ptr<Config> createDefaultConfig();
    std::unique_ptr<Config> createConfigFromDefaults(const std::map<std::string, ConfigValue>& defaults);
    std::unique_ptr<Config> createConfigFromFile(const std::string& filepath, bool merge = false);
    std::unique_ptr<Config> createConfigFromArgs(int argc, char** argv, bool merge = false);
    
    // Profile management
    void registerProfile(const ConfigProfile& profile);
    std::optional<ConfigProfile> getProfile(const std::string& name);
    bool applyProfileToConfig(const std::string& profileName, Config& config);
    
    // Template management
    void registerTemplate(const std::string& name, const ConfigProfile& template_);
    std::optional<ConfigProfile> getTemplate(const std::string& name);
    std::unique_ptr<Config> createFromTemplate(const std::string& templateName);
    
    // Object pooling for Config objects
    std::unique_ptr<Config> acquireConfigFromPool();
    void releaseConfigToPool(std::unique_ptr<Config> config);
    
private:
    mutable std::mutex mutex_;
    bool initialized_ = false;
    std::unordered_map<std::string, std::any> config_;
    std::vector<std::unique_ptr<Config>> configPool_;
};

} // namespace nlm
