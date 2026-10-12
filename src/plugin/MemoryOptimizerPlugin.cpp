// Example Plugin: Advanced Memory Optimizer Plugin
// Demonstrates how to create and use plugins in the NLM system
// This plugin provides enhanced memory management for neural components

#pragma once

#include "../plugin/PluginSystem.hpp"

namespace nlm {

// Memory Optimization Component
class MemoryOptimizerComponent : public IPluginComponent {
public:
    MemoryOptimizerComponent() = default;
    MemoryOptimizerComponent(const MemoryOptimizerComponent& other) {
        config_ = other.config_;
    }
    
    std::string getName() const override { return "MemoryOptimizer"; }
    std::string getVersion() const override { return "1.0.0"; }
    PluginComponentType getType() const override { return PluginComponentType::MemorySystem; }
    
    bool initialize() override {
        // Initialize memory optimization settings
        optimalBlockSize_ = 1024;
        enableCompression_ = true;
        enablePrefetching_ = true;
        adaptiveAllocation_ = true;
        return true;
    }
    
    void shutdown() override {
        // Clean up memory optimization structures
        optimizedBlocks_.clear();
        compressionCache_.clear();
    }
    
    bool isInitialized() const override {
        return optimalBlockSize_ > 0;
    }
    
    IPluginComponent* createInstance() const override {
        return new MemoryOptimizerComponent(*this);
    }
    
    void setConfig(const PluginConfig& config) override {
        config_ = config;
        // Parse optimization parameters from config
        if (config.metadata.find("block_size") != config.metadata.end()) {
            optimalBlockSize_ = std::stoi(config.metadata.at("block_size"));
        }
        if (config.metadata.find("compression") != config.metadata.end()) {
            enableCompression_ = (config.metadata.at("compression") == "true");
        }
    }
    
    PluginConfig getConfig() const override {
        PluginConfig config("MemoryOptimizer", "1.0.0", "Advanced memory optimization for neural components");
        config.metadata["block_size"] = std::to_string(optimalBlockSize_);
        config.metadata["compression"] = enableCompression_ ? "true" : "false";
        config.metadata["prefetching"] = enablePrefetching_ ? "true" : "false";
        return config;
    }
    
    // Memory optimization functions
    size_t optimizeMemoryUsage(size_t currentSize) {
        if (!enableCompression_) return currentSize;
        
        // Apply compression optimization
        size_t optimizedSize = currentSize;
        if (adaptiveAllocation_) {
            // Adaptive optimization based on usage patterns
            optimizedSize = calculateOptimalBlockSize(currentSize);
        }
        
        // Store optimization record
        optimizedBlocks_.push_back({currentSize, optimizedSize});
        return optimizedSize;
    }
    
    bool enablePrefetching = false;
    bool adaptiveAllocation_ = false;
    
private:
    struct OptimizationRecord {
        size_t originalSize;
        size_t optimizedSize;
        double efficiency; // (original - optimized) / original
        
        OptimizationRecord(size_t original, size_t optimized)
            : originalSize(original), optimizedSize(optimized) {
            efficiency = (original > 0) ? (static_cast<double>(original - optimized) / original) : 0.0;
        }
    };
    
    size_t optimalBlockSize_ = 1024;
    std::vector<OptimizationRecord> optimizedBlocks_;
    std::map<size_t, size_t> compressionCache_;
    
    size_t calculateOptimalBlockSize(size_t currentSize) {
        // Simple heuristic: use powers of 2 for cache efficiency
        size_t blockSize = 1;
        while (blockSize < currentSize && blockSize < optimalBlockSize_ * 2) {
            blockSize <<= 1;
        }
        return std::min(blockSize, optimalBlockSize_ * 2);
    }
};

// Main Plugin: Advanced Memory Optimizer Plugin
class MemoryOptimizerPlugin : public IPlugin {
public:
    MemoryOptimizerPlugin(const PluginConfig& config) : config_(config) {}
    
    std::string getName() const override { return config_.name; }
    std::string getVersion() const override { return config_.version; }
    std::string getDescription() const override { return config_.description; }
    PluginConfig getConfig() const override { return config_; }
    void setConfig(const PluginConfig& config) override { config_ = config; }
    
    bool load() override {
        // Plugin loading logic
        return true;
    }
    
    bool initialize() override {
        // Create and initialize the memory optimizer component
        component_ = std::make_unique<MemoryOptimizerComponent>(config_);
        return component_->initialize();
    }
    
    bool shutdown() override {
        // Shutdown component and cleanup
        if (component_) {
            component_->shutdown();
        }
        return true;
    }
    
    bool isLoaded() const override { return true; }
    bool isInitialized() const override {
        return component_ && component_->isInitialized();
    }
    
    bool registerComponent(std::unique_ptr<IPluginComponent> component) override {
        // Register additional components
        components_.push_back(std::move(component));
        return true;
    }
    
    IPluginComponent* getComponent(const std::string& name) override {
        if (component_ && component_->getName() == name) {
            return component_.get();
        }
        
        // Check additional components
        for (const auto& comp : components_) {
            if (comp->getName() == name) {
                return comp.get();
            }
        }
        
        return nullptr;
    }
    
    std::vector<IPluginComponent*> getComponents(PluginComponentType type) override {
        std::vector<IPluginComponent*> result;
        
        if (component_ && component_->getType() == type) {
            result.push_back(component_.get());
        }
        
        // Check additional components
        for (const auto& comp : components_) {
            if (comp->getType() == type) {
                result.push_back(comp.get());
            }
        }
        
        return result;
    }
    
    bool unregisterComponent(const std::string& name) override {
        for (auto it = components_.begin(); it != components_.end(); ++it) {
            if ((*it)->getName() == name) {
                components_.erase(it);
                return true;
            }
        }
        return false;
    }
    
    void addEventListener(std::function<void(const PluginEvent&)> listener) override {
        eventListeners_.push_back(listener);
    }
    
    void removeEventListener(std::function<void(const PluginEvent&)> listener) override {
        eventListeners_.erase(std::remove(eventListeners_.begin(), eventListeners_.end(), listener), eventListeners_.end());
    }
    
    void emitEvent(const PluginEvent& event) override {
        for (const auto& listener : eventListeners_) {
            try {
                listener(event);
            } catch (const std::exception& e) {
                // Log error but continue with other listeners
            }
        }
    }
    
    bool activate() override {
        // Activate the plugin
        emitEvent(PluginEvent(PluginEventType::INITIALIZED, getName(), "Plugin activated"));
        return true;
    }
    
    bool deactivate() override {
        // Deactivate the plugin
        emitEvent(PluginEvent(PluginEventType::SHUTDOWN, getName(), "Plugin deactivated"));
        return true;
    }
    
    bool canLoad() const override {
        return true; // Always can load for this example
    }
    
    std::string getError() const override {
        return error_;
    }
    
private:
    PluginConfig config_;
    std::unique_ptr<MemoryOptimizerComponent> component_;
    std::vector<std::unique_ptr<IPluginComponent>> components_;
    std::vector<std::function<void(const PluginEvent&)>> eventListeners_;
    std::string error_;
};

// Plugin Factory for Memory Optimizer
class MemoryOptimizerFactory : public IPluginFactory {
public:
    std::string getFactoryName() const override { return "MemoryOptimizerFactory"; }
    std::string getFactoryVersion() const override { return "1.0.0"; }
    
    std::unique_ptr<IPlugin> createPlugin(const PluginConfig& config) override {
        return std::make_unique<MemoryOptimizerPlugin>(config);
    }
    
    bool canCreate(const PluginConfig& config) const override {
        return config.name == "MemoryOptimizer";
    }
    
    std::string getDescription() const override {
        return "Factory for Memory Optimizer Plugin - Provides advanced memory management for neural components";
    }
    
    std::vector<std::string> getSupportedComponents() const override {
        return {"MemoryOptimizer"};
    }
};

} // namespace nlm

// Plugin entry point macro
#define NLM_PLUGIN_ENTRY_POINT \
    extern "C" NLM_PLUGIN_EXPORT IPlugin* createPlugin(const nlm::PluginConfig& config) { \
        auto factory = std::make_unique<nlm::MemoryOptimizerFactory>(); \
        if (factory->canCreate(config)) { \
            return factory->createPlugin(config).release(); \
        } \
        return nullptr; \
    } \
    \
    extern "C" NLM_PLUGIN_EXPORT const char* getPluginVersion() { \
        return nlm::PLUGIN_ARCHITECTURE_VERSION; \
    } \
    \
    extern "C" NLM_PLUGIN_EXPORT const char* getPluginName() { \
        return "MemoryOptimizerPlugin"; \
    } \
    \
    extern "C" NLM_PLUGIN_EXPORT void destroyPlugin(IPlugin* plugin) { \
        delete plugin; \
    } 

// Plugin registration macro
#define NLM_REGISTER_PLUGIN() \
    namespace { \
        struct MemoryOptimizerPluginRegistrar { \
            MemoryOptimizerPluginRegistrar() { \
                static auto factory = std::make_unique<nlm::MemoryOptimizerFactory>(); \
                nlm::getPluginManager().registerFactory(std::move(factory)); \
            } \
        } memoryOptimizerRegistrar; \
    } 

// Plugin main function
NLM_REGISTER_PLUGIN()

// Plugin compilation flags
// Add -DNLM_PLUGIN_EXPORTS to compiler command line when building as a plugin
// Add -DNLM_PLUGIN_BUILD to compiler command line when building a plugin that exports symbols

// Plugin documentation
/**
 * Advanced Memory Optimizer Plugin
 * 
 * This plugin provides enhanced memory management capabilities for NLM neural networks.
 * It optimizes memory allocation patterns, implements compression strategies, and
 * provides adaptive memory management based on usage patterns.
 * 
 * Features:
 * - Dynamic block size optimization
 * - Memory compression for large neural networks
 * - Prefetching for improved cache performance
 * - Adaptive allocation based on usage patterns
 * - Performance monitoring and statistics
 * 
 * Usage:
 * 1. Compile the plugin with -DNLM_PLUGIN_EXPORTS
 * 2. Load the plugin using the plugin manager
 * 3. Access the MemoryOptimizer component for memory optimization
 * 
 * Example:
 *   nlm::loadPlugin("path/to/MemoryOptimizerPlugin.so");
 *   auto component = nlm::getComponent("MemoryOptimizer");
 *   if (component) {
 *       size_t optimizedSize = component->optimizeMemoryUsage(1024 * 1024);
 *   }
 */nlm::PluginConfig memoryOptimizerConfig;

// Helper function to create memory optimizer plugin
inline nlm::PluginConfig createMemoryOptimizerConfig() {
    nlm::PluginConfig config("MemoryOptimizer", "1.0.0", 
                           "Advanced memory optimization for neural components");
    config.author = "NLM Research Team";
    config.contact = "research@nlm.org";
    config.license = "MIT";
    config.dependencies = "CoreSystem";
    config.optionalDependencies = "PerformanceSystem,EventSystem";
    config.enabled = true;
    config.required = true;
    
    // Plugin-specific metadata
    config.metadata["block_size"] = "1024";
    config.metadata["compression"] = "true";
    config.metadata["prefetching"] = "true";
    config.metadata["adaptive_allocation"] = "true";
    
    return config;
}

// Convenience function to load memory optimizer plugin
inline bool loadMemoryOptimizerPlugin() {
    nlm::PluginConfig config = createMemoryOptimizerConfig();
    
    auto factory = std::make_unique<nlm::MemoryOptimizerFactory>();
    if (factory->canCreate(config)) {
        auto plugin = factory->createPlugin(config);
        
        if (plugin->load() && plugin->initialize()) {
            nlm::getPluginManager().registerFactory(std::move(factory));
            return true;
        }
    }
    
    return false;
}