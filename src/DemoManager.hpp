#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <functional>
#include <algorithm>

namespace nlm {

// Demo configuration and management
class DemoManager {
public:
    DemoManager();
    ~DemoManager();
    
    // Disable copying
    DemoManager(const DemoManager&) = delete;
    DemoManager& operator=(const DemoManager&) = delete;
    
    // Initialize demo with configuration
    bool initialize(const std::map<std::string, std::string>& config);
    
    // Get demo information
    std::string getDemoName() const;
    std::string getDemoVersion() const;
    std::string getDemoDescription() const;
    
    // Get configuration
    std::map<std::string, std::string> getConfiguration() const;
    std::string getConfiguration(const std::string& key) const;
    
    // Set configuration
    void setConfiguration(const std::string& key, const std::string& value);
    void setConfiguration(const std::map<std::string, std::string>& config);
    
    // Validate configuration
    bool validateConfiguration() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Get demo parameters
    uint64_t getNeuronCount() const;
    uint64_t getRegionCount() const;
    double getConnectionProbability() const;
    double getTimestep() const;
    uint64_t getMaxSteps() const;
    
    // Demo setup
    bool setupDemoEnvironment();
    bool setupNeuralNetwork();
    bool setupPlasticity();
    bool setupMemorySystems();
    
    // Demo execution
    bool runDemo();
    bool runPhase1Demo();
    bool runPhase2Demo();
    bool runPhase3Demo();
    bool runPhase4Demo();
    bool runPhase5Demo();
    bool runPhase6Demo();
    
    // Demo status
    bool isDemoRunning() const;
    bool isDemoInitialized() const;
    bool isDemoCompleted() const;
    
    // Demo results
    std::map<std::string, double> getDemoResults() const;
    double getDemoResult(const std::string& metric) const;
    
    // Demo control
    void pauseDemo();
    void resumeDemo();
    void stopDemo();
    void resetDemo();
    
    // Demo statistics
    struct DemoStatistics {
        uint64_t totalSteps;
        double simulationTime;
        double realTimeElapsed;
        uint64_t neuronCount;
        uint64_t regionCount;
        double averageFiringRate;
        double averageSynapticWeight;
        size_t memoryEpisodesStored;
        float noveltyLevel;
        float curiosityLevel;
        float dopamineLevel;
        size_t totalSpikes;
        size_t learningEvents;
        
        DemoStatistics()
            : totalSteps(0), simulationTime(0.0), realTimeElapsed(0.0),
              neuronCount(0), regionCount(0), averageFiringRate(0.0),
              averageSynapticWeight(0.0), memoryEpisodesStored(0),
              noveltyLevel(0.0f), curiosityLevel(0.0f), dopamineLevel(0.0f),
              totalSpikes(0), learningEvents(0) {}
    };
    
    DemoStatistics getStatistics() const;
    
    // Demo configuration templates
    struct DemoConfigTemplate {
        std::string name;
        std::string description;
        std::map<std::string, std::string> defaultConfig;
        std::vector<std::string> requiredParameters;
        std::vector<std::string> optionalParameters;
        
        DemoConfigTemplate() = default;
        DemoConfigTemplate(const std::string& n, const std::string& desc,
                          const std::map<std::string, std::string>& defConfig)
            : name(n), description(desc), defaultConfig(defConfig) {}
    };
    
    // Get available demo templates
    std::vector<DemoConfigTemplate> getAvailableDemoTemplates() const;
    
    // Load demo configuration from file
    bool loadDemoConfiguration(const std::string& filepath);
    
    // Save demo configuration to file
    bool saveDemoConfiguration(const std::string& filepath) const;
    
    // Demo event handling
    using DemoEventCallback = std::function<void(const std::string& event, const std::map<std::string, std::string>& data)>;
    
    void addEventCallback(const DemoEventCallback& callback);
    void removeEventCallback(const DemoEventCallback& callback);
    
    // Demo error handling
    using DemoErrorHandler = std::function<void(const std::string& error, const std::map<std::string, std::string>& context)>;
    
    void setErrorHandler(const DemoErrorHandler& handler);
    
    // Demo logging
    using DemoLogger = std::function<void(const std::string& message, const std::map<std::string, std::string>& metadata)>;
    
    void setLogger(const DemoLogger& logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Demo initialization helpers
    bool initializeBasicComponents();
    bool initializeAdvancedComponents();
    bool initializeSupportSystems();
    
    // Demo execution helpers
    bool executePhase1Steps();
    bool executePhase2Steps();
    bool executePhase3Steps();
    bool executePhase4Steps();
    bool executePhase5Steps();
    bool executePhase6Steps();
    
    // Demo validation helpers
    bool validateBasicConfiguration() const;
    bool validateNeuralConfiguration() const;
    bool validatePlasticityConfiguration() const;
    bool validateMemoryConfiguration() const;
    
    // Demo result calculation
    void calculateBasicResults();
    void calculateAdvancedResults();
    void calculateIntegrationResults();
    
    // Demo statistics calculation
    void updateStatistics();
    
    // Configuration management
    void applyConfigurationDefaults();
    bool validateConfigurationAgainstSchema() const;
    
    // Demo lifecycle management
    void onDemoInitialized();
    void onDemoStarting();
    void onDemoRunning();
    void onDemoCompleted();
    void onDemoStopped();
    void onDemoReset();
    
    // Error handling helpers
    void handleDemoError(const std::string& error);
    
    // Logging helpers
    void logDemoMessage(const std::string& message, const std::map<std::string, std::string>& metadata = {});
}; // namespace nlm

// Global demo manager instance
DemoManager& getDemoManager();