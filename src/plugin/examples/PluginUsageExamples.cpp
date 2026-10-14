// Plugin usage examples for the NLM plugin architecture
// Demonstrates complete plugin loading and integration

#include "Plugin.hpp"
#include "PluginFactory.hpp"
#include <iostream>
#include <fstream>
#include <vector>

// ============================================================================
// Plugin Configuration Example
// ============================================================================

struct PluginConfiguration {
    std::string name;
    std::string category;
    std::unordered_map<std::string, std::string> parameters;
    int priority;
    bool enabled;
};

// ============================================================================
// Example 1: Basic Plugin Factory Usage
// ============================================================================

void example1_BasicFactoryUsage() {
    std::cout << "=== Example 1: Basic Plugin Factory Usage ===" << std::endl;
    
    // Create plugin configuration
    std::vector<PluginConfiguration> pluginConfigs = {
        {"DefaultRegionPlugin", "BrainRegion", {"regionSize": "100", "neuronType": "Excitatory"}, 50, true},
        {"STDPPlasticityPlugin", "PlasticityRule", {"learningRate": "0.01"}, 80, true},
        {"DopaminePlugin", "Neuromodulator", {"baselineLevel": "0.1", "decayRate": "0.5"}, 85, true},
        {"WorkingMemoryPlugin", "Memory", {"capacity": "500", "decayRate": "0.01"}, 70, true},
        {"DefaultEnvironmentPlugin", "Environment", {"worldWidth": "100", "worldHeight": "100"}, 65, true}
    };
    
    // Convert to NLM configuration format
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> nlmConfigs;
    for (const auto& config : pluginConfigs) {
        if (config.enabled) {
            nlmConfigs[config.name] = config.parameters;
        }
    }
    
    // Create all enabled plugins
    auto plugins = nlm::PluginFactory::getInstance().createAllEnabledPlugins(nlmConfigs);
    
    std::cout << "Created " << plugins.size() << " plugins:" << std::endl;
    for (const auto& plugin : plugins) {
        const auto& metadata = plugin->getMetadata();
        std::cout << "  - " << metadata.name << " (v" << metadata.version << ")" 
                  << " - " << metadata.description << std::endl;
    }
    
    std::cout << "Example 1 completed.\n" << std::endl;
}

// ============================================================================
// Example 2: Plugin with Configuration
// ============================================================================

void example2_PluginConfiguration() {
    std::cout << "=== Example 2: Plugin Configuration ===" << std::endl;
    
    // Configure plugins with specific parameters
    nlm::PluginFactory::getInstance().setPluginEnabled("DefaultRegionPlugin", true);
    nlm::PluginFactory::getInstance().setPluginEnabled("STDPPlasticityPlugin", true);
    nlm::PluginFactory::getInstance().setPluginEnabled("DopaminePlugin", false); // Disabled
    
    // Get plugin metadata
    auto metadata = nlm::PluginFactory::getInstance().getPluginMetadata("DefaultRegionPlugin");
    std::cout << "Plugin: " << metadata.name << std::endl;
    std::cout << "  Version: " << metadata.version << std::endl;
    std::cout << "  Description: " << metadata.description << std::endl;
    std::cout << "  Author: " << metadata.author << std::endl;
    std::cout << "  Priority: " << metadata.priority << std::endl;
    std::cout << "  Enabled: " << (metadata.enabled ? "Yes" : "No") << std::endl;
    
    // Create plugin with configuration
    std::unordered_map<std::string, std::string> regionConfig = {
        {"regionSize", "200"},
        {"neuronType", "Inhibitory"},
        {"connectionDensity", "0.5"}
    };
    
    auto plugin = nlm::PluginFactory::getInstance().createPlugin("DefaultRegionPlugin", regionConfig);
    if (plugin) {
        std::cout << "Successfully created and configured DefaultRegionPlugin" << std::endl;
        
        // Get configuration from plugin
        if (auto* configurable = dynamic_cast<nlm::IConfigurable*>(plugin.get())) {
            auto config = configurable->getConfiguration();
            std::cout << "Plugin configuration:" << std::endl;
            for (const auto& pair : config) {
                std::cout << "  - " << pair.first << ": " << pair.second << std::endl;
            }
        }
    }
    
    std::cout << "Example 2 completed.\n" << std::endl;
}

// ============================================================================
// Example 3: Plugin Lifecycle Management
// ============================================================================

void example3_PluginLifecycle() {
    std::cout << "=== Example 3: Plugin Lifecycle Management ===" << std::endl;
    
    // Create a plugin
    auto plugin = nlm::PluginFactory::getInstance().createPlugin("STDPPlasticityPlugin");
    if (!plugin) {
        std::cout << "Failed to create plugin" << std::endl;
        return;
    }
    
    std::cout << "Plugin created. State: " << static_cast<int>(plugin->getState()) << std::endl;
    
    // Configure and initialize
    std::unordered_map<std::string, std::string> config = {
        {"learningRate", "0.05"},
        {"weightDecay", "0.01"},
        {"minWeight", "-0.5"},
        {"maxWeight", "0.5"}
    };
    
    // Configure (IConfigurable interface)
    if (auto* configurable = dynamic_cast<nlm::IConfigurable*>(plugin.get())) {
        if (configurable->configure(config)) {
            std::cout << "Plugin configured successfully" << std::endl;
        } else {
            std::cout << "Plugin configuration failed" << std::endl;
        }
    }
    
    // Initialize
    if (plugin->initialize(config)) {
        std::cout << "Plugin initialized successfully" << std::endl;
    } else {
        std::cout << "Plugin initialization failed" << std::endl;
    }
    
    std::cout << "Plugin state after initialization: " << static_cast<int>(plugin->getState()) << std::endl;
    
    // Update plugin
    plugin->update(0.1f);
    std::cout << "Plugin updated with dt=0.1s" << std::endl;
    
    // Shutdown
    plugin->shutdown();
    std::cout << "Plugin shutdown. State: " << static_cast<int>(plugin->getState()) << std::endl;
    
    std::cout << "Example 3 completed.\n" << std::endl;
}

// ============================================================================
// Example 4: Plugin Validation and Safety Checks
// ============================================================================

void example4_PluginValidation() {
    std::cout << "=== Example 4: Plugin Validation and Safety Checks ===" << std::endl;
    
    // Check if plugin is registered
    bool isRegistered = nlm::PluginFactory::getInstance().isPluginRegistered("STDPPlasticityPlugin");
    std::cout << "STDPPlasticityPlugin registered: " << (isRegistered ? "Yes" : "No") << std::endl;
    
    // Get all registered plugins
    auto allPlugins = nlm::PluginFactory::getInstance().getRegisteredPluginNames();
    std::cout << "All registered plugins:" << std::endl;
    for (const auto& name : allPlugins) {
        std::cout << "  - " << name << std::endl;
    }
    
    // Get enabled plugins
    auto enabledPlugins = nlm::PluginFactory::getInstance().getEnabledPlugins();
    std::cout << "Enabled plugins:" << std::endl;
    for (const auto& name : enabledPlugins) {
        std::cout << "  - " << name << std::endl;
    }
    
    // Get plugins in loading order
    auto loadingOrder = nlm::PluginFactory::getInstance().getPluginsInLoadingOrder();
    std::cout << "Plugins in loading order:" << std::endl;
    for (int i = 0; i < loadingOrder.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << loadingOrder[i] << std::endl;
    }
    
    // Validate plugin creation
    auto plugin = nlm::PluginFactory::getInstance().createPlugin("NonExistentPlugin");
    if (plugin) {
        std::cout << "Created plugin: " << plugin->getName() << std::endl;
    } else {
        std::cout << "Failed to create plugin (expected for non-existent plugin)" << std::endl;
    }
    
    // Try to create disabled plugin
    nlm::PluginFactory::getInstance().setPluginEnabled("DopaminePlugin", false);
    auto disabledPlugin = nlm::PluginFactory::getInstance().createPlugin("DopaminePlugin");
    if (disabledPlugin) {
        std::cout << "Created disabled plugin (unexpected)" << std::endl;
    } else {
        std::cout << "Failed to create disabled plugin (expected)" << std::endl;
    }
    
    std::cout << "Example 4 completed.\n" << std::endl;
}

// ============================================================================
// Example 5: Plugin State Management
// ============================================================================

void example5_PluginStateManagement() {
    std::cout << "=== Example 5: Plugin State Management ===" << std::endl;
    
    // Create and initialize a plugin
    auto plugin = nlm::PluginFactory::getInstance().createPlugin("DefaultEnvironmentPlugin");
    if (!plugin) {
        std::cout << "Failed to create plugin" << std::endl;
        return;
    }
    
    // Configure and initialize
    std::unordered_map<std::string, std::string> config = {
        {"worldWidth", "150"},
        {"worldHeight", "150"},
        {"maxEntities", "2000"},
        {"timeOfDay", "8.0"}
    };
    
    if (plugin->initialize(config)) {
        std::cout << "Plugin initialized successfully" << std::endl;
        
        // Check state
        std::cout << "Initial state: " << static_cast<int>(plugin->getState()) << std::endl;
        
        // Update plugin
        for (int i = 0; i < 5; ++i) {
            plugin->update(1.0f);
            std::cout << "After update " << (i + 1) << ": " << static_cast<int>(plugin->getState()) << std::endl;
        }
        
        // Check computational metrics
        if (auto* computational = dynamic_cast<nlm::IComputational*>(plugin.get())) {
            std::cout << "Performance metrics:" << std::endl;
            std::cout << "  CPU Usage: " << computational->getCpuUsage() * 100 << "%" << std::endl;
            std::cout << "  Memory Usage: " << computational->getMemoryUsage() << " bytes" << std::endl;
            std::cout << "  Active Objects: " << computational->getActiveObjects() << std::endl;
            std::cout << "  Performance Score: " << computational->getPerformanceScore() << std::endl;
        }
        
        // Check state interface if available
        if (auto* state = dynamic_cast<nlm::IState*>(plugin.get())) {
            std::cout << "State management available:" << std::endl;
            std::cout << "  Has state: " << (state->hasState() ? "Yes" : "No") << std::endl;
            std::cout << "  State signature: " << state->getStateSignature() << std::endl;
            
            // Save and load state
            const std::string testFile = "test_plugin_state.dat";
            if (state->saveState(testFile)) {
                std::cout << "State saved to " << testFile << std::endl;
                
                // Clear state and reload
                state->clearState();
                std::cout << "State cleared" << std::endl;
                
                if (state->loadState(testFile)) {
                    std::cout << "State loaded from " << testFile << std::endl;
                } else {
                    std::cout << "Failed to load state" << std::endl;
                }
            } else {
                std::cout << "Failed to save state (I/O not available in demo)" << std::endl;
            }
        }
        
        // Shutdown
        plugin->shutdown();
        std::cout << "Plugin shutdown. State: " << static_cast<int>(plugin->getState()) << std::endl;
    }
    
    std::cout << "Example 5 completed.\n" << std::endl;
}

// ============================================================================
// Example 6: Plugin Factory Helper Functions
// ============================================================================

void example6_PluginFactoryHelpers() {
    std::cout << "=== Example 6: Plugin Factory Helper Functions ===" << std::endl;
    
    // Clear all plugins (for demonstration)
    nlm::PluginFactory::getInstance().clear();
    
    // Demonstrate plugin registration macro
    // Note: In a real implementation, you would use the REGISTER_PLUGIN macro
    
    std::cout << "Plugin factory cleared. Current plugins:" << std::endl;
    auto plugins = nlm::PluginFactory::getInstance().getRegisteredPluginNames();
    if (plugins.empty()) {
        std::cout << "  (No plugins registered)" << std::endl;
    } else {
        for (const auto& name : plugins) {
            std::cout << "  - " << name << std::endl;
        }
    }
    
    // Show how to use the plugin factory in a real application
    std::cout << "\nPlugin factory usage example:" << std::endl;
    std::cout << "  1. Configure plugins via configuration file or programmatic API" << std::endl;
    std::cout << "  2. Register plugin factories using PluginFactory::registerPlugin()" << std::endl;
    std::cout << "  3. Enable/disable plugins using PluginFactory::setPluginEnabled()" << std::endl;
    std::cout << "  4. Create plugins using PluginFactory::createPlugin()" << std::endl;
    std::cout << "  5. Use PluginFactory::createAllEnabledPlugins() for batch creation" << std::endl;
    std::cout << "  6. Manage plugin lifecycle (initialize, update, shutdown)" << std::endl;
    
    std::cout << "Example 6 completed.\n" << std::endl;
}

// ============================================================================
// Main function to run all examples
// ============================================================================

int main() {
    std::cout << "NLM Plugin Architecture Examples" << std::endl;
    std::cout << "=================================" << std::endl << std::endl;
    
    // Run all examples
    example1_BasicFactoryUsage();
    example2_PluginConfiguration();
    example3_PluginLifecycle();
    example4_PluginValidation();
    example5_PluginStateManagement();
    example6_PluginFactoryHelpers();
    
    std::cout << "All examples completed successfully!" << std::endl;
    return 0;
}