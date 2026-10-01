// Factory Interface for NLM Component Creation
// Provides abstract factory pattern for creating brain simulation components

#ifndef NLM_FACTORY_HPP
#define NLM_FACTORY_HPP

#include <memory>
#include <string>
#include <map>

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"

namespace nlm {

class Factory {
public:
    virtual ~Factory() = default;
    
    virtual std::shared_ptr<Brain> createBrain(const std::shared_ptr<Config>& config) = 0;
    virtual std::shared_ptr<Config> createConfig() = 0;
    virtual std::shared_ptr<Config> createConfig(const std::string& configFile) = 0;
    
    static Factory& getInstance();
    static void setFactory(std::unique_ptr<Factory> factory);
};

class DefaultFactory : public Factory {
private:
    std::map<std::string, std::shared_ptr<Config>> configCache;
    
public:
    std::shared_ptr<Brain> createBrain(const std::shared_ptr<Config>& config) override {
        NLM_LOG_DEBUG("Creating Brain using DefaultFactory");
        return std::make_shared<Brain>(config);
    }
    
    std::shared_ptr<Config> createConfig() override {
        NLM_LOG_DEBUG("Creating default Config using DefaultFactory");
        return std::make_shared<Config>();
    }
    
    std::shared_ptr<Config> createConfig(const std::string& configFile) override {
        if (configCache.find(configFile) != configCache.end()) {
            NLM_LOG_DEBUG("Using cached config: " + configFile);
            return configCache[configFile];
        }
        
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
            configCache[configFile] = config;
        } else {
            NLM_LOG_INFO("Using default configuration (file not found: " + configFile + ")");
        }
        return config;
    }
};

// Singleton instance management
namespace FactoryImpl {
    extern std::unique_ptr<Factory> currentFactory;
}

inline Factory& Factory::getInstance() {
    if (!FactoryImpl::currentFactory) {
        FactoryImpl::currentFactory = std::make_unique<DefaultFactory>();
    }
    return *FactoryImpl::currentFactory;
}

inline void Factory::setFactory(std::unique_ptr<Factory> factory) {
    FactoryImpl::currentFactory = std::move(factory);
}

} // namespace nlm

#endif // NLM_FACTORY_HPP