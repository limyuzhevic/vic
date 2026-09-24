#pragma once

#include <string>
#include <map>
#include <vector>

namespace nlm {

// Advanced configuration interface for AgentBrain
// Allows fine-grained control over agent behavior and neuromodulation systems
class AgentConfig {
public:
    // Basic neuromodulation control
    bool enableRewardModulation = true;           // Enable/disable reward prediction error
    bool enableStructuralPlasticity = true;       // Enable/disable structural synapse changes
    bool enableDevelopment = true;                // Enable/disable developmental changes
    bool enableCuriosity = true;                  // Enable/disable curiosity-driven exploration
    
    // Neuromodulation parameters
    float rewardModulationStrength = 1.0f;        // Dopamine modulation strength (-1 to 1)
    float curiosityThreshold = 0.3f;             // Minimum curiosity for exploration
    float noveltySensitivity = 2.0f;              // How strongly novelty affects curiosity
    float predictionErrorWeight = 0.5f;           // How prediction error affects behavior
    
    // Developmental parameters
    double developmentalSpeed = 1.0f;             // How fast development progresses
    float plasticityRate = 1.0f;                  // Base learning rate
    float structuralPlasticityRate = 0.01f;       // Synaptogenesis rate
    float pruningRate = 0.0001f;                 // Synapse pruning rate
    
    // Sensory processing parameters
    float visionSmoothing = 0.9f;                // Vision signal smoothing (0-1)
    float touchThreshold = 0.1f;                 // Minimum touch signal to register collision
    float internalSignalScale = 2.0f;            // Scale for internal state signals
    
    // Motor control parameters
    float motorActivityThreshold = 0.5f;          // Minimum neural activity for action
    float explorationProbability = 0.1f;          // Base exploration rate
    bool enableLookActions = true;                // Enable LookLeft/LookRight actions
    
    // Memory and consolidation parameters
    bool enableMemoryReplay = true;               // Enable episodic memory replay
    float memoryConsolidationThreshold = 0.8f;    // Confidence level for memory consolidation
    bool enableWorkingMemory = true;              // Enable working memory system
    
    // Performance tuning
    size_t maxMemoryBufferSize = 1024;            // Maximum memory buffer size
    float performanceMonitorInterval = 1.0f;      // Performance monitoring interval
    bool enablePerformanceMetrics = true;         // Track performance statistics
    
    // Debug and monitoring
    bool enableLogging = true;                    // Enable detailed logging
    bool enableDebugVisualization = false;         // Enable debug visualization
    bool enableProfileMode = false;               // Enable performance profiling
    
    // Configuration presets
    enum class Preset {
        Minimal,          // Minimal functionality, fastest performance
        Balanced,         // Balanced between performance and features
        Advanced,         // Full functionality with all features enabled
        Custom           // User-defined configuration
    };
    
    Preset currentPreset = Preset::Balanced;
    
    // Configuration validation
    bool validate() const;
    
    // Get configuration parameter value
    template<typename T>
    T getParameter(const std::string& name, const T& defaultValue = T{}) const;
    
    // Set configuration parameter value
    template<typename T>
    void setParameter(const std::string& name, const T& value);
    
    // Apply preset configuration
    void applyPreset(Preset preset);
    
    // Get configuration summary for debugging
    std::string getConfigSummary() const;
    
    // Configuration changed callback (for extensibility)
    using ConfigChangedCallback = std::function<void(const std::string& parameter, const std::string& oldValue, const std::string& newValue)>;
    
    // Register configuration change callback
    void addConfigChangedCallback(const ConfigChangedCallback& callback);
    
private:
    // Internal configuration storage
    std::map<std::string, std::string> configMap_;
    
    // Callback list
    std::vector<ConfigChangedCallback> callbacks_;
    
    // Parameter validation helper
    template<typename T>
    bool validateParameter(const std::string& name, const T& value) const;
};

// Global configuration instance manager
class ConfigManager {
public:
    static ConfigManager& getInstance();
    
    // Get the global configuration
    AgentConfig& getConfig();
    
    // Set the global configuration
    void setConfig(const AgentConfig& config);
    
    // Reset to default configuration
    void resetToDefaults();
    
    // Save configuration to file
    bool saveToFile(const std::string& filename) const;
    
    // Load configuration from file
    bool loadFromFile(const std::string& filename);
    
    // Get configuration parameter
    template<typename T>
    static T getParameter(const std::string& name, const T& defaultValue = T{});
    
    // Set configuration parameter
    template<typename T>
    static void setParameter(const std::string& name, const T& value);
    
private:
    AgentConfig config_;
    ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
};

// Helper function to easily access global configuration
inline AgentConfig& getAgentConfig() {
    return ConfigManager::getInstance().getConfig();
}

} // namespace nlm