#pragma once

#include "abstraction/AbstractComponents.hpp"
#include "development/DevelopmentSystem.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.hpp"
#include "core/Config/Config.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

class DevelopmentSystemFactory {
public:
    // Factory type enum for configuration
    enum class DevelopmentSystemType {
        Synaptogenesis,
        Pruning,
        Maturation,
        FullDevelopmentSystem,
        Custom
    };
    
    DevelopmentSystemFactory();
    ~DevelopmentSystemFactory();
    
    // Create development system based on type
    std::unique_ptr<AbstractDevelopmentSystem> createDevelopmentSystem(
        DevelopmentSystemType type,
        std::shared_ptr<Config> config = nullptr
    );
    
    std::unique_ptr<AbstractDevelopmentSystem> createDevelopmentSystem(
        const std::string& developmentSystemName,
        std::shared_ptr<Config> config = nullptr
    );
    
    // Create specific development system types
    std::unique_ptr<Synaptogenesis> createSynaptogenesis(
        std::shared_ptr<Config> config = nullptr
    ) {
        auto synaptogenesis = std::make_unique<Synaptogenesis>();
        if (config) {
            configureFromConfig(synaptogenesis.get(), config);
        }
        return synaptogenesis;
    }
    
    std::unique_ptr<Pruning> createPruning(
        std::shared_ptr<Config> config = nullptr
    ) {
        auto pruning = std::make_unique<Pruning>();
        if (config) {
            configureFromConfig(pruning.get(), config);
        }
        return pruning;
    }
    
    std::unique_ptr<Maturation> createMaturation(
        std::shared_ptr<Config> config = nullptr
    ) {
        auto maturation = std::make_unique<Maturation>();
        if (config) {
            configureFromConfig(maturation.get(), config);
        }
        return maturation;
    }
    
    // Configuration-based factory creation
    std::unique_ptr<AbstractDevelopmentSystem> createFromConfig(
        const Config& config
    );
    
    // Plugin-based factory creation
    std::unique_ptr<AbstractDevelopmentSystem> createFromPlugin(
        const std::string& pluginName,
        std::shared_ptr<Config> config = nullptr
    );
    
    // List available development system types
    static std::vector<DevelopmentSystemType> getAvailableTypes() {
        return {
            DevelopmentSystemType::Synaptogenesis,
            DevelopmentSystemType::Pruning,
            DevelopmentSystemType::Maturation,
            DevelopmentSystemType::FullDevelopmentSystem
        };
    }
    
    // Get type name
    static const char* getTypeName(DevelopmentSystemType type) {
        switch (type) {
            case DevelopmentSystemType::Synaptogenesis: return "Synaptogenesis";
            case DevelopmentSystemType::Pruning: return "Pruning";
            case DevelopmentSystemType::Maturation: return "Maturation";
            case DevelopmentSystemType::FullDevelopmentSystem: return "DevelopmentSystem";
            default: return "Unknown";
        }
    }
    
    // Configuration helpers
    void configureFromConfig(AbstractDevelopmentSystem* developmentSystem, const Config& config);
    void configureFromConfig(Synaptogenesis* synaptogenesis, const Config& config);
    void configureFromConfig(Pruning* pruning, const Config& config);
    void configureFromConfig(Maturation* maturation, const Config& config);
    
    // Plugin system
    bool registerPlugin(
        const std::string& name,
        std::function<std::unique_ptr<AbstractDevelopmentSystem>(std::shared_ptr<Config>)> factory
    );
    std::unique_ptr<AbstractDevelopmentSystem> createFromRegisteredPlugin(
        const std::string& name,
        std::shared_ptr<Config> config = nullptr
    );
    
    // Get registered plugin names
    std::vector<std::string> getRegisteredPlugins() const;
    
    // Statistics and monitoring
    size_t getCreationCount() const { return creationCount_; }
    size_t getPluginCount() const { return plugins_.size(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    mutable size_t creationCount_;
    std::unordered_map<std::string, std::function<std::unique_ptr<AbstractDevelopmentSystem>(std::shared_ptr<Config>)>> plugins_;
};

} // namespace nlm
