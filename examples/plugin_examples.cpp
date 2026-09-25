#pragma once

#include "plugin/PluginRegistry.hpp"
#include "brain/Brain.hpp"
#include "memory/WorkingMemory.hpp"
#include <iostream>

// Example plugin implementations

// 1. Example Plugin: Working Memory Enhancer
class WorkingMemoryEnhancerPlugin : public nlm::IPlugin {
public:
    const char* getName() const override { return "WorkingMemoryEnhancer"; }
    const char* getDescription() const override { 
        return "Enhances working memory capacity and consolidation"; 
    }
    nlm::PluginCategory getCategory() const override { 
        return nlm::PluginCategory::MemorySystem; 
    }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getAuthor() const override { return "NLM Core Team"; }
    nlm::PluginPriority getPriority() const override { return nlm::PluginPriority::High; }
    
    bool initialize(const std::any& config) override {
        std::cout << "WorkingMemoryEnhancerPlugin: Initializing with config..." << std::endl;
        // Initialize the plugin with configuration
        return true;
    }
    
    void shutdown() override {
        std::cout << "WorkingMemoryEnhancerPlugin: Shutting down..." << std::endl;
    }
    
    void update(double dt) override {
        // Update plugin logic
        std::cout << "WorkingMemoryEnhancerPlugin: Updating..." << std::endl;
    }
    
    std::string getStatus() const override {
        return "Enhanced working memory capacity and consolidation";
    }
};

// Register the plugin
REGISTER_PLUGIN(WorkingMemoryEnhancerPlugin, "WorkingMemoryEnhancer",
    "Enhances working memory capacity and consolidation",
    nlm::PluginCategory::MemorySystem, nlm::PluginPriority::High, {});

// 2. Example Plugin: Dopamine Amplifier
class DopamineAmplifierPlugin : public nlm::IPlugin {
public:
    const char* getName() const override { return "DopamineAmplifier"; }
    const char* getDescription() const override { 
        return "Amplifies dopamine effects for enhanced learning"; 
    }
    nlm::PluginCategory getCategory() const override { 
        return nlm::PluginCategory::Neuromodulator; 
    }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getAuthor() const override { return "NLM Core Team"; }
    nlm::PluginPriority getPriority() const override { return nlm::PluginPriority::Normal; }
    
    bool initialize(const std::any& config) override {
        std::cout << "DopamineAmplifierPlugin: Initializing..." << std::endl;
        return true;
    }
    
    void configure(const std::any& settings) override {
        std::cout << "DopamineAmplifierPlugin: Configuring..." << std::endl;
    }
    
    void update(double dt) override {
        // Implement dopamine amplification logic
        std::cout << "DopamineAmplifierPlugin: Amplifying dopamine effects..." << std::endl;
    }
    
    std::string getStatus() const override {
        return "Dopamine signal amplification for enhanced learning";
    }
};

// Register the plugin
REGISTER_PLUGIN(DopamineAmplifierPlugin, "DopamineAmplifier",
    "Amplifies dopamine effects for enhanced learning",
    nlm::PluginCategory::Neuromodulator, nlm::PluginPriority::Normal, {"WorkingMemoryEnhancer"});

// 3. Example Plugin: Prediction System Optimizer
class PredictionSystemOptimizerPlugin : public nlm::IPlugin {
public:
    const char* getName() const override { return "PredictionSystemOptimizer"; }
    const char* getDescription() const override { 
        return "Optimizes prediction system performance and accuracy"; 
    }
    nlm::PluginCategory getCategory() const override { 
        return nlm::PluginCategory::Prediction; 
    }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getAuthor() const override { return "NLM Core Team"; }
    nlm::PluginPriority getPriority() const override { return nlm::PluginPriority::High; }
    
    bool initialize(const std::any& config) override {
        std::cout << "PredictionSystemOptimizerPlugin: Initializing..." << std::endl;
        return true;
    }
    
    void configure(const std::any& settings) override {
        std::cout << "PredictionSystemOptimizerPlugin: Configuring..." << std::endl;
    }
    
    void update(double dt) override {
        // Optimize prediction system
        std::cout << "PredictionSystemOptimizerPlugin: Optimizing prediction system..." << std::endl;
    }
    
    std::string getStatus() const override {
        return "Prediction system optimization and accuracy enhancement";
    }
};

// Register the plugin
REGISTER_PLUGIN(PredictionSystemOptimizerPlugin, "PredictionSystemOptimizer",
    "Optimizes prediction system performance and accuracy",
    nlm::PluginCategory::Prediction, nlm::PluginPriority::High, {});

// 4. Example Plugin: Curiosity Driver
class CuriosityDriverPlugin : public nlm::IPlugin {
public:
    const char* getName() const override { return "CuriosityDriver"; }
    const char* getDescription() const override { 
        return "Drives curiosity-based exploration behavior"; 
    }
    nlm::PluginCategory getCategory() const override { 
        return nlm::PluginCategory::CognitiveModule; 
    }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getAuthor() const override { return "NLM Core Team"; }
    nlm::PluginPriority getPriority() const override { return nlm::PluginPriority::Normal; }
    
    bool initialize(const std::any& config) override {
        std::cout << "CuriosityDriverPlugin: Initializing..." << std::endl;
        return true;
    }
    
    void configure(const std::any& settings) override {
        std::cout << "CuriosityDriverPlugin: Configuring..." << std::endl;
    }
    
    void update(double dt) override {
        // Drive curiosity and exploration
        std::cout << "CuriosityDriverPlugin: Driving curiosity-based exploration..." << std::endl;
    }
    
    std::string getStatus() const override {
        return "Curiosity-driven exploration behavior";
    }
};

// Register the plugin
REGISTER_PLUGIN(CuriosityDriverPlugin, "CuriosityDriver",
    "Drives curiosity-based exploration behavior",
    nlm::PluginCategory::CognitiveModule, nlm::PluginPriority::Normal, {});

int main() {
    std::cout << "=== NLM Plugin Examples ===" << std::endl;
    std::cout << "Loading and demonstrating various plugins..." << std::endl;
    
    // Get the plugin registry
    auto& registry = nlm::PluginRegistry::getInstance();
    
    std::cout << "\n=== Available Plugins ===" << std::endl;
    auto plugins = registry.getAllPlugins();
    for (const auto& plugin : plugins) {
        std::cout << "Name: " << plugin.name << std::endl;
        std::cout << "Description: " << plugin.description << std::endl;
        std::cout << "Category: " << static_cast<int>(plugin.category) << std::endl;
        std::cout << "Priority: " << static_cast<int>(plugin.priority) << std::endl;
        std::cout << "Enabled: " << (plugin.enabled ? "Yes" : "No") << std::endl;
        std::cout << "Dependencies: ";
        for (size_t i = 0; i < plugin.dependencies.size(); ++i) {
            std::cout << plugin.dependencies[i];
            if (i < plugin.dependencies.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
        std::cout << "---" << std::endl;
    }
    
    std::cout << "\n=== Loading Plugins ===" << std::endl;
    
    // Try to initialize the plugin system
    if (!registry.initializePluginSystem()) {
        std::cerr << "Failed to initialize plugin system!" << std::endl;
        return 1;
    }
    
    std::cout << "\n=== Plugin System Status ===" << std::endl;
    std::cout << "System initialized: " << (registry.isPluginSystemInitialized() ? "Yes" : "No") << std::endl;
    
    std::cout << "\n=== Active Plugins ===" << std::endl;
    auto activePlugins = registry.getEnabledPlugins();
    for (const auto& plugin : activePlugins) {
        std::cout << "Plugin: " << plugin.name << std::endl;
        std::cout << "Status: " << (registry.isPluginEnabled(plugin.name) ? "Enabled" : "Disabled") << std::endl;
        std::cout << "Category: " << static_cast<int>(plugin.category) << std::endl;
        std::cout << "Priority: " << static_cast<int>(plugin.priority) << std::endl;
        std::cout << "---" << std::endl;
    }
    
    std::cout << "\n=== Plugin Report ===" << std::endl;
    std::cout << registry.getPluginReport() << std::endl;
    
    std::cout << "\n=== Plugin Statistics ===" << std::endl;
    auto stats = registry.getStatistics();
    std::cout << "Total Plugins: " << stats.totalPlugins << std::endl;
    std::cout << "Enabled Plugins: " << stats.enabledPlugins << std::endl;
    std::cout << "Disabled Plugins: " << stats.disabledPlugins << std::endl;
    std::cout << "Total Load Time: " << stats.totalLoadTime << "s" << std::endl;
    
    std::cout << "\n=== Plugin Categories ===" << std::endl;
    for (const auto& pair : stats.pluginsByCategory) {
        std::cout << static_cast<int>(pair.first) << ": " << pair.second << " plugins" << std::endl;
    }
    
    // Cleanup
    registry.shutdownPluginSystem();
    
    std::cout << "\n=== Plugin Examples Complete ===" << std::endl;
    return 0;
}
