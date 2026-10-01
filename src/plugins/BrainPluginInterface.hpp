#pragma once

// Brain Plugin Interface for NLM (Neural Learning Machine)
// Defines the interface for brain-specific plugins

#include "PluginCore.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Brain Plugin Categories
enum class BrainPluginCategory {
    Cognitive,        // Cognitive functions and decision making
    Memory,          // Memory systems and storage
    Plasticity,      // Synaptic plasticity and learning
    Neuromodulation, // Neuromodulatory systems
    Sensory,         // Sensory processing and input
    Motor,           // Motor output and action
    Development,     // Developmental processes
    Integration,     // System integration
    Optimization,    // Performance optimization
    Monitoring       // Brain state monitoring
};

// Brain Plugin Capabilities
struct BrainPluginCapabilities {
    std::vector<std::string> cognitiveFunctions;
    std::vector<std::string> memoryTypes;
    std::vector<std::string> plasticityRules;
    std::vector<std::string> neuromodulators;
    std::vector<std::string> sensoryModes;
    std::vector<std::string> motorActions;
    std::vector<std::string> developmentalStages;
    std::vector<std::string> integrationPoints;
    std::vector<std::string> optimizationTargets;
    std::vector<std::string> monitoringMetrics;
    
    bool supportsCognitiveFunction(const std::string& function) const {
        return std::find(cognitiveFunctions.begin(), cognitiveFunctions.end(), function) != cognitiveFunctions.end();
    }
    
    bool managesMemoryType(const std::string& type) const {
        return std::find(memoryTypes.begin(), memoryTypes.end(), type) != memoryTypes.end();
    }
    
    bool implementsPlasticityRule(const std::string& rule) const {
        return std::find(plasticityRules.begin(), plasticityRules.end(), rule) != plasticityRules.end();
    }
    
    bool providesNeuromodulator(const std::string& neuromodulator) const {
        return std::find(neuromodulators.begin(), neuromodulators.end(), neuromodulator) != neuromodulators.end();
    }
};

// Brain Plugin Event Types
enum class BrainPluginEventType {
    SpikeProcessed,
    MemoryStored,
    MemoryRetrieved,
    PlasticityUpdated,
    NeuromodulationApplied,
    CognitiveProcessStarted,
    CognitiveProcessCompleted,
    ActionProduced,
    SensoryInputReceived,
    MotorOutputGenerated,
    DevelopmentStageChanged,
    LearningEvent,
    MemoryConsolidation,
    Synaptogenesis,
    Pruning,
    RewardSignal,
    PredictionError,
    NoveltyDetection
};

// Brain Plugin Event
struct BrainPluginEvent {
    BrainPluginEventType type;
    std::string pluginName;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> data;
    std::shared_ptr<Brain> brainContext;
};

// Brain Plugin Event Callback
using BrainPluginEventCallback = std::function<void(const BrainPluginEvent&)>;

// Brain Plugin Interface
class BrainPluginInterface : public Plugin {
public:
    virtual ~BrainPluginInterface() = default;
    
    // Brain system access
    virtual std::shared_ptr<Brain> getBrain() const = 0;
    virtual bool setBrain(std::shared_ptr<Brain> brain) = 0;
    
    // Plugin category and capabilities
    virtual BrainPluginCategory getCategory() const = 0;
    virtual BrainPluginCapabilities getCapabilities() const = 0;
    
    // Integration with brain systems
    virtual bool integrateWithBrain(std::shared_ptr<Brain> brain) = 0;
    virtual bool removeIntegrationWithBrain(std::shared_ptr<Brain> brain) = 0;
    virtual bool isIntegratedWithBrain(std::shared_ptr<Brain> brain) const = 0;
    
    // Brain-specific operations
    virtual bool processSpike(const class Neuron& neuron, double time) = 0;
    virtual bool storeMemory(const std::string& key, const std::vector<float>& data) = 0;
    virtual std::vector<float> retrieveMemory(const std::string& key) = 0;
    virtual bool updatePlasticity(const class Synapse& synapse, double time) = 0;
    virtual bool applyNeuromodulation(const class Neuromodulator& signal) = 0;
    virtual bool processSensoryInput(const class SensoryInput& input) = 0;
    virtual std::unique_ptr<class Action> processMotorOutput() = 0;
    virtual bool updateDevelopment(const class DevelopmentalStage& stage) = 0;
    
    // Cognitive functions
    virtual std::vector<float> processCognition(const std::vector<float>& input) = 0;
    virtual bool initiateCognitiveProcess(const std::string& processName) = 0;
    virtual std::map<std::string, std::vector<float>> getCognitiveResults() = 0;
    
    // Learning and adaptation
    virtual bool learnFromExperience(const std::vector<float>& experience) = 0;
    virtual bool adaptToEnvironment(const std::map<std::string, float>& environmentState) = 0;
    virtual bool consolidateMemory() = 0;
    
    // Performance and optimization
    virtual bool optimizePerformance(const std::map<std::string, float>& metrics) = 0;
    virtual std::map<std::string, float> getPerformanceMetrics() const = 0;
    
    // Monitoring and statistics
    virtual bool updateMetrics(const std::map<std::string, float>& metrics) = 0;
    virtual std::map<std::string, float> getCurrentMetrics() const = 0;
    virtual std::map<std::string, float> getHistoricalMetrics() const = 0;
    
    // Event handling
    virtual void addBrainEventCallback(BrainPluginEventCallback callback) = 0;
    virtual void removeBrainEventCallback(BrainPluginEventCallback callback) = 0;
    virtual void broadcastBrainEvent(const BrainPluginEvent& event) = 0;
    
    // Configuration management
    virtual bool loadConfiguration(const std::string& filepath) = 0;
    virtual bool saveConfiguration(const std::string& filepath) const = 0;
    virtual bool resetConfiguration() = 0;
    
    // Plugin commands
    virtual bool executeBrainCommand(const std::string& command, 
                                    const std::map<std::string, std::string>& args = {}) = 0;
    
    // Plugin help
    virtual std::string getBrainHelp() const = 0;
    virtual std::string getPluginUsage() const = 0;
    
    // Plugin statistics
    virtual std::map<std::string, float> getPluginStatistics() const = 0;
    virtual std::map<std::string, float> getPluginPerformance() const = 0;
    
    // Brain plugin specific methods
    virtual bool initializeBrainIntegration() = 0;
    virtual bool shutdownBrainIntegration() = 0;
    virtual bool validateBrainConfiguration() = 0;
    virtual bool updateBrainParameters(const std::map<std::string, float>& parameters) = 0;
    virtual std::map<std::string, float> getBrainParameters() const = 0;
    
    // Event processing
    virtual bool onBrainEvent(const BrainPluginEvent& event) = 0;
    virtual bool onBrainStep(SimulationStep step, Timestamp time) = 0;
    virtual bool onBrainReset() = 0;
    
    // Plugin lifecycle for brain integration
    virtual bool prepareForBrainIntegration() = 0;
    virtual bool finalizeBrainIntegration() = 0;
    virtual bool cleanupBrainIntegration() = 0;
    
    // Brain plugin comparison
    virtual bool operator==(const BrainPluginInterface& other) const = 0;
    virtual bool operator<(const BrainPluginInterface& other) const = 0;
    
    // Plugin identification
    virtual std::string getPluginName() const override = 0;
    virtual std::string getVersion() const override = 0;
    virtual std::string getDescription() const override = 0;
    virtual std::string getAuthor() const override = 0;
    
    // Plugin lifecycle
    virtual PluginLoadStatus getLoadStatus() const override = 0;
    virtual bool initialize(const std::map<std::string, PluginConfigValue>& config) override = 0;
    virtual bool update(double deltaTime) override = 0;
    virtual bool shutdown() override = 0;
    virtual bool configure(const std::map<std::string, PluginConfigValue>& config) override = 0;
    
    // Plugin capabilities
    virtual PluginCapabilities getCapabilities() const override = 0;
    virtual bool supportsFeature(const std::string& feature) const override = 0;
    
    // Plugin state management
    virtual std::map<std::string, PluginConfigValue> getState() const override = 0;
    virtual bool setState(const std::map<std::string, PluginConfigValue>& state) override = 0;
    virtual bool saveState(const std::string& filepath) const override = 0;
    virtual bool loadState(const std::string& filepath) override = 0;
    
    // Plugin information
    virtual std::vector<std::string> getDependencies() const override = 0;
    virtual PluginSecurityLevel getSecurityLevel() const override = 0;
    virtual std::vector<std::string> getTags() const override = 0;
    virtual std::string getCategory() const override = 0;
    
    // Plugin statistics
    virtual PluginStatistics getStatistics() const override = 0;
    
    // Plugin event handling
    virtual void addEventCallback(PluginEventCallback callback) override = 0;
    virtual void removeEventCallback(PluginEventCallback callback) override = 0;
    
    // Plugin system integration
    virtual bool integrateWith(std::shared_ptr<Plugin> otherPlugin) override = 0;
    virtual bool removeIntegration(std::shared_ptr<Plugin> otherPlugin) override = 0;
    
    // Plugin validation
    virtual bool validate() const override = 0;
    virtual std::vector<std::string> getValidationErrors() const override = 0;
    
    // Plugin commands
    virtual bool executeCommand(const std::string& command, 
                              const std::map<std::string, std::string>& args = {}) override = 0;
    
    // Plugin help
    virtual std::string getHelp() const override = 0;
};

// Brain Plugin Factory Interface
class BrainPluginFactory : public PluginFactory {
public:
    virtual ~BrainPluginFactory() = default;
    
    // Create brain-specific plugin
    virtual std::unique_ptr<BrainPluginInterface> createBrainPlugin(const std::string& name) = 0;
    
    // Get plugin type
    virtual std::string getPluginType() const override = 0;
    
    // Get metadata
    virtual PluginMetadata getMetadata() const override = 0;
    
    // Check if can create
    virtual bool canCreate(const std::string& name) const override = 0;
    
    // Create regular plugin
    virtual std::unique_ptr<Plugin> createPlugin(const std::string& name) override = 0;
};

// Brain Plugin Base Class
class BrainPluginBase : public BrainPluginInterface {
public:
    BrainPluginBase(const std::string& name, const std::string& version);
    virtual ~BrainPluginBase() = default;
    
    // Plugin identification
    virtual std::string getName() const override;
    virtual std::string getVersion() const override;
    virtual std::string getDescription() const override;
    virtual std::string getAuthor() const override;
    
    // Plugin metadata
    virtual const PluginMetadata& getMetadata() const override;
    
    // Brain system access
    virtual std::shared_ptr<Brain> getBrain() const override;
    virtual bool setBrain(std::shared_ptr<Brain> brain) override;
    
    // Plugin category
    virtual BrainPluginCategory getCategory() const override = 0;
    
    // Integration management
    virtual bool integrateWithBrain(std::shared_ptr<Brain> brain) override;
    virtual bool removeIntegrationWithBrain(std::shared_ptr<Brain> brain) override;
    virtual bool isIntegratedWithBrain(std::shared_ptr<Brain> brain) const override;
    
    // Brain-specific operations (default implementations)
    virtual bool processSpike(const class Neuron& neuron, double time) override;
    virtual bool storeMemory(const std::string& key, const std::vector<float>& data) override;
    virtual std::vector<float> retrieveMemory(const std::string& key) override;
    virtual bool updatePlasticity(const class Synapse& synapse, double time) override;
    virtual bool applyNeuromodulation(const class Neuromodulator& signal) override;
    virtual bool processSensoryInput(const class SensoryInput& input) override;
    virtual std::unique_ptr<class Action> processMotorOutput() override;
    virtual bool updateDevelopment(const class DevelopmentalStage& stage) override;
    
    // Cognitive functions (default implementations)
    virtual std::vector<float> processCognition(const std::vector<float>& input) override;
    virtual bool initiateCognitiveProcess(const std::string& processName) override;
    virtual std::map<std::string, std::vector<float>> getCognitiveResults() override;
    
    // Learning and adaptation (default implementations)
    virtual bool learnFromExperience(const std::vector<float>& experience) override;
    virtual bool adaptToEnvironment(const std::map<std::string, float>& environmentState) override;
    virtual bool consolidateMemory() override;
    
    // Plugin lifecycle
    virtual bool prepareForBrainIntegration() override;
    virtual bool finalizeBrainIntegration() override;
    virtual bool cleanupBrainIntegration() override;
    
    // Configuration management
    virtual bool loadConfiguration(const std::string& filepath) override;
    virtual bool saveConfiguration(const std::string& filepath) const override;
    virtual bool resetConfiguration() override;
    
    // Event handling
    virtual void addBrainEventCallback(BrainPluginEventCallback callback) override;
    virtual void removeBrainEventCallback(BrainPluginEventCallback callback) override;
    virtual void broadcastBrainEvent(const BrainPluginEvent& event) override;
    
    // Brain plugin specific methods
    virtual bool initializeBrainIntegration() override;
    virtual bool shutdownBrainIntegration() override;
    virtual bool validateBrainConfiguration() override;
    virtual bool updateBrainParameters(const std::map<std::string, float>& parameters) override;
    virtual std::map<std::string, float> getBrainParameters() const override;
    
    // Event processing
    virtual bool onBrainEvent(const BrainPluginEvent& event) override;
    virtual bool onBrainStep(SimulationStep step, Timestamp time) override;
    virtual bool onBrainReset() override;
    
    // Plugin lifecycle callbacks
    virtual void onPluginLoadStarted(const std::string& name) override;
    virtual void onPluginLoadCompleted(const std::string& name, bool success) override;
    virtual void onPluginLoadFailed(const std::string& name, const std::string& error) override;
    
    virtual void onPluginStartRequested(const std::string& name) override;
    virtual void onPluginStartCompleted(const std::string& name, bool success) override;
    virtual void onPluginStartFailed(const std::string& name, const std::string& error) override;
    
    virtual void onPluginStopRequested(const std::string& name) override;
    virtual void onPluginStopCompleted(const std::string& name, bool success) override;
    virtual void onPluginStopFailed(const std::string& name, const std::string& error) override;
    
    virtual void onPluginUnloadRequested(const std::string& name) override;
    virtual void onPluginUnloadCompleted(const std::string& name, bool success) override;
    virtual void onPluginUnloadFailed(const std::string& name, const std::string& error) override;
    
    virtual void onPluginUpdated(const std::string& name, double deltaTime) override;
    virtual void onPluginRecovered(const std::string& name) override;
    
    // System lifecycle callbacks
    virtual void onSystemInitialized() override;
    virtual void onSystemShutdown() override;
    virtual void onSystemError(const std::string& error) override;
    virtual void onSystemWarning(const std::string& warning) override;
    
protected:
    std::string name;
    std::string version;
    PluginMetadata metadata;
    std::shared_ptr<Brain> brain;
    
    std::vector<PluginEventCallback> eventCallbacks;
    std::vector<BrainPluginEventCallback> brainEventCallbacks;
    
    bool brainIntegrated;
    PluginLoadStatus loadStatus;
    
    std::map<std::string, PluginConfigValue> config;
    
    std::map<std::string, std::vector<float>> memoryStore;
    std::map<std::string, std::vector<float>> cognitiveResults;
    
    PluginStatistics statistics;
    
    std::vector<PluginLifecycleEvent> lifecycleHistory;
};

// Brain Plugin Registry Interface
class BrainPluginRegistry {
public:
    virtual ~BrainPluginRegistry() = default;
    
    // Register brain plugin
    virtual bool registerBrainPlugin(std::unique_ptr<BrainPluginInterface> plugin) = 0;
    
    // Unregister brain plugin
    virtual bool unregisterBrainPlugin(const std::string& name) = 0;
    
    // Get brain plugin
    virtual BrainPluginInterface* getBrainPlugin(const std::string& name) = 0;
    virtual const BrainPluginInterface* getBrainPlugin(const std::string& name) const = 0;
    
    // Get plugins by category
    virtual std::vector<BrainPluginInterface*> getBrainPluginsByCategory(BrainPluginCategory category) = 0;
    virtual std::vector<BrainPluginInterface*> getBrainPluginsByCapability(const std::string& capability) = 0;
    
    // Get all plugins
    virtual std::vector<BrainPluginInterface*> getAllBrainPlugins() = 0;
    
    // Check if plugin is registered
    virtual bool isBrainPluginRegistered(const std::string& name) const = 0;
    
    // Plugin information
    virtual BrainPluginCategory getPluginCategory(const std::string& name) const = 0;
    virtual BrainPluginCapabilities getPluginCapabilities(const std::string& name) const = 0;
    
    // Plugin validation
    virtual bool validateBrainPlugin(const BrainPluginInterface& plugin) const = 0;
    virtual std::vector<std::string> getBrainPluginValidationErrors(const std::string& name) const = 0;
    
    // Plugin integration
    virtual bool integrateBrainPlugins() = 0;
    virtual bool removeBrainPluginIntegration(const std::string& name1, const std::string& name2) = 0;
    virtual std::vector<std::pair<std::string, std::string>> getBrainPluginIntegrations() const = 0;
    
    // Plugin statistics
    virtual std::map<std::string, float> getPluginStatistics(const std::string& name) const = 0;
    virtual std::map<std::string, float> getPluginPerformance(const std::string& name) const = 0;
    
    // Plugin commands
    virtual bool executeBrainPluginCommand(const std::string& name, 
                                          const std::string& command,
                                          const std::map<std::string, std::string>& args = {}) = 0;
};

} // namespace nlm