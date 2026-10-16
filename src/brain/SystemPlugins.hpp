#pragma once

#include "BrainSystem.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>

namespace nlm {

// Plugin system for adding new brain systems
class SystemPlugin {
public:
    virtual ~SystemPlugin() = default;
    
    // Get plugin name
    virtual const char* getName() const = 0;
    
    // Get plugin version
    virtual const char* getVersion() const { return "1.0"; }
    
    // Check if plugin can create the specified system
    virtual bool canCreateSystem(const std::string& systemName) const = 0;
    
    // Create a system instance
    virtual std::unique_ptr<BrainSystem> createSystem(const std::string& systemName, Brain* brain) = 0;
    
    // Get list of systems this plugin can create
    virtual std::vector<std::string> getSystemNames() const = 0;
};

// Default brain system plugins
class DefaultSystemPlugins {
public:
    static std::unique_ptr<SystemPlugin> createWorkingMemoryPlugin() {
        class WorkingMemoryPlugin : public SystemPlugin {
        public:
            const char* getName() const override { return "WorkingMemoryPlugin"; }
            bool canCreateSystem(const std::string& systemName) const override {
                return systemName == "WorkingMemory";
            }
            std::unique_ptr<BrainSystem> createSystem(const std::string& systemName, Brain* brain) override {
                // Create working memory system wrapper
                auto* workingMemory = brain->getWorkingMemory();
                if (workingMemory) {
                    return std::make_unique<WorkingMemorySystemWrapper>(workingMemory);
                }
                return nullptr;
            }
            std::vector<std::string> getSystemNames() const override {
                return {"WorkingMemory"};
            }
        };
        return std::make_unique<WorkingMemoryPlugin>();
    }
    
    static std::unique_ptr<SystemPlugin> createEpisodicMemoryPlugin() {
        class EpisodicMemoryPlugin : public SystemPlugin {
        public:
            const char* getName() const override { return "EpisodicMemoryPlugin"; }
            bool canCreateSystem(const std::string& systemName) const override {
                return systemName == "EpisodicMemory";
            }
            std::unique_ptr<BrainSystem> createSystem(const std::string& systemName, Brain* brain) override {
                auto* episodicMemory = brain->getEpisodicMemory();
                if (episodicMemory) {
                    return std::make_unique<EpisodicMemorySystemWrapper>(episodicMemory);
                }
                return nullptr;
            }
            std::vector<std::string> getSystemNames() const override {
                return {"EpisodicMemory"};
            }
        };
        return std::make_unique<EpisodicMemoryPlugin>();
    }
    
    static std::unique_ptr<SystemPlugin> createAssociativeMemoryPlugin() {
        class AssociativeMemoryPlugin : public SystemPlugin {
        public:
            const char* getName() const override { return "AssociativeMemoryPlugin"; }
            bool canCreateSystem(const std::string& systemName) const override {
                return systemName == "AssociativeMemory";
            }
            std::unique_ptr<BrainSystem> createSystem(const std::string& systemName, Brain* brain) override {
                auto* associativeMemory = brain->getAssociativeMemory();
                if (associativeMemory) {
                    return std::make_unique<AssociativeMemorySystemWrapper>(associativeMemory);
                }
                return nullptr;
            }
            std::vector<std::string> getSystemNames() const override {
                return {"AssociativeMemory"};
            }
        };
        return std::make_unique<AssociativeMemoryPlugin>();
    }
    
    static std::unique_ptr<SystemPlugin> createPredictionSystemPlugin() {
        class PredictionSystemPlugin : public SystemPlugin {
        public:
            const char* getName() const override { return "PredictionSystemPlugin"; }
            bool canCreateSystem(const std::string& systemName) const override {
                return systemName == "PredictionSystem";
            }
            std::unique_ptr<BrainSystem> createSystem(const std::string& systemName, Brain* brain) override {
                auto* predictionSystem = brain->getPredictionSystem();
                if (predictionSystem) {
                    return std::make_unique<PredictionSystemWrapper>(predictionSystem);
                }
                return nullptr;
            }
            std::vector<std::string> getSystemNames() const override {
                return {"PredictionSystem"};
            }
        };
        return std::make_unique<PredictionSystemPlugin>();
    }
    
    static std::unique_ptr<SystemPlugin> createDevelopmentSystemPlugin() {
        class DevelopmentSystemPlugin : public SystemPlugin {
        public:
            const char* getName() const override { return "DevelopmentSystemPlugin"; }
            bool canCreateSystem(const std::string& systemName) const override {
                return systemName == "DevelopmentSystem";
            }
            std::unique_ptr<BrainSystem> createSystem(const std::string& systemName, Brain* brain) override {
                auto* developmentSystem = brain->getDevelopmentSystem();
                if (developmentSystem) {
                    return std::make_unique<DevelopmentSystemWrapper>(developmentSystem);
                }
                return nullptr;
            }
            std::vector<std::string> getSystemNames() const override {
                return {"DevelopmentSystem"};
            }
        };
        return std::make_unique<DevelopmentSystemPlugin>();
    }
};

// Plugin manager for registering and managing plugins
class PluginManager {
public:
    // Get the singleton instance
    static PluginManager& getInstance() {
        static PluginManager instance;
        return instance;
    }
    
    // Register a plugin
    void registerPlugin(std::unique_ptr<SystemPlugin> plugin) {
        if (plugin) {
            plugins_.push_back(std::move(plugin));
        }
    }
    
    // Register default plugins
    void registerDefaultPlugins() {
        registerPlugin(DefaultSystemPlugins::createWorkingMemoryPlugin());
        registerPlugin(DefaultSystemPlugins::createEpisodicMemoryPlugin());
        registerPlugin(DefaultSystemPlugins::createAssociativeMemoryPlugin());
        registerPlugin(DefaultSystemPlugins::createPredictionSystemPlugin());
        registerPlugin(DefaultSystemPlugins::createDevelopmentSystemPlugin());
    }
    
    // Create a system using registered plugins
    std::unique_ptr<BrainSystem> createSystem(const std::string& systemName, Brain* brain) {
        // First, check registered plugins
        for (const auto& plugin : plugins_) {
            if (plugin->canCreateSystem(systemName)) {
                return plugin->createSystem(systemName, brain);
            }
        }
        
        // Fallback to registry factory if available
        auto& registry = SystemRegistry::getInstance();
        if (registry.isSystemRegistered(systemName)) {
            return registry.createSystem(systemName, brain);
        }
        
        return nullptr;
    }
    
    // Get all available system names
    std::vector<std::string> getAvailableSystemNames() const {
        std::vector<std::string> names;
        for (const auto& plugin : plugins_) {
            auto pluginNames = plugin->getSystemNames();
            names.insert(names.end(), pluginNames.begin(), pluginNames.end());
        }
        // Remove duplicates
        std::sort(names.begin(), names.end());
        names.erase(std::unique(names.begin(), names.end()), names.end());
        return names;
    }
    
    // Get all registered plugins
    const std::vector<std::unique_ptr<SystemPlugin>>& getPlugins() const {
        return plugins_;
    }
    
    // Clear all plugins
    void clear() {
        plugins_.clear();
    }
    
private:
    PluginManager() = default;
    ~PluginManager() = default;
    
    // Prevent copying
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    
    std::vector<std::unique_ptr<SystemPlugin>> plugins_;
};

} // namespace nlm
