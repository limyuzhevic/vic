#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <optional>
#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

// Neuromodulator factory class with plugin architecture
class NeuromodulatorFactory {
public:
    // Factory function type for neuromodulator creation
    using NeuromodulatorCreator = std::function<std::unique_ptr<Neuromodulator>(std::shared_ptr<Config>, TimestepDuration)>;
    
    // Plugin registration function type
    using PluginRegistration = std::function<void(NeuromodulatorFactory&)>;
    
    // Neuromodulator type information
    struct NeuromodulatorInfo {
        std::string name;
        std::string description;
        std::unordered_map<std::string, std::string> configOptions;
        bool isBuiltIn;
        bool isStub;
    };
    
    // Create a neuromodulator instance based on type name and configuration
    static std::unique_ptr<Neuromodulator> create(const std::string& neuromodulatorType, 
                                                  std::shared_ptr<Config> config,
                                                  TimestepDuration timestep = 0.001);
    
    // Get all available neuromodulator types
    static std::vector<NeuromodulatorInfo> getAvailableTypes();
    
    // Check if a neuromodulator type is available
    static bool hasType(const std::string& neuromodulatorType);
    
    // Check if a neuromodulator type is a stub (placeholder)
    static bool isStub(const std::string& neuromodulatorType);
    
    // Register a built-in neuromodulator creator
    static void registerBuiltInCreator(const std::string& typeName, 
                                       NeuromodulatorCreator creator,
                                       const std::string& description,
                                       bool isStub = false);
    
    // Register a plugin
    static void registerPlugin(const std::string& pluginName, PluginRegistration registrationFunc);
    
    // Load all built-in neuromodulators
    static void loadBuiltInNeuromodulators();
    
    // Initialize plugin system
    static void initializePlugins();
    
    // Graceful fallback for missing implementations
    static std::unique_ptr<Neuromodulator> createWithFallback(const std::string& neuromodulatorType,
                                                              std::shared_ptr<Config> config,
                                                              TimestepDuration timestep);
    
private:
    static std::unordered_map<std::string, NeuromodulatorCreator> builtInCreators_;
    static std::vector<NeuromodulatorInfo> builtInTypes_;
    static std::mutex mutex_;
    static std::unordered_set<std::string> loadedPlugins_;
    
    // Thread-safe registration helper
    static void registerCreator(const std::string& typeName, 
                                NeuromodulatorCreator creator,
                                const std::string& description,
                                bool isStub = false);
    
    // Create default stub implementation
    static std::unique_ptr<Neuromodulator> createStub(const std::string& typeName, 
                                                      std::shared_ptr<Config> config);
};

} // namespace nlm