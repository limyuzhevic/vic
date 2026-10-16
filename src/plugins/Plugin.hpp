#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include <typeinfo>

namespace nlm {

// Plugin type identification
enum class PluginType {
    SystemComponent,  // Core system component
    ProcessingModule,  // Processing module
    Visualization,     // Visualization component
    Extension,         // General extension
    Experimental      // Experimental feature
};

// Plugin interface base class
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Get plugin metadata
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual PluginType getType() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize(const std::map<std::string, std::string>& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    // Plugin capabilities
    virtual std::vector<std::string> getRequiredComponents() const { return {}; }
    virtual std::vector<std::string> getProvidedComponents() const { return {}; }
    
    // Extension points
    virtual void onBrainInitialized(std::shared_ptr<class Brain> brain) {}
    virtual void onExperimentStarted(const std::string& experimentName) {}
    virtual void onExperimentCompleted(const std::string& experimentName) {}
    virtual void onStepCompleted(size_t step) {}
    
    // Configuration access
    virtual void setPluginConfig(const std::map<std::string, std::string>& config) { pluginConfig_ = config; }
    virtual std::map<std::string, std::string> getPluginConfig() const { return pluginConfig_; }
    
protected:
    std::map<std::string, std::string> pluginConfig_;
};

// Template-based plugin implementation
class PluginBase : public IPlugin {
public:
    PluginBase(const std::string& name, const std::string& version, const std::string& description, PluginType type)
        : name_(name), version_(version), description_(description), type_(type), initialized_(false) {}
    
    std::string getName() const override { return name_; }
    std::string getVersion() const override { return version_; }
    std::string getDescription() const override { return description_; }
    PluginType getType() const override { return type_; }
    
    bool isInitialized() const override { return initialized_; }
    
    void setInitialized(bool initialized) { initialized_ = initialized; }
    
protected:
    std::string name_;
    std::string version_;
    std::string description_;
    PluginType type_;
    bool initialized_;
};

// Plugin factory function type
using PluginFactory = std::function<std::unique_ptr<IPlugin>()>;

// Plugin registry for automatic plugin discovery
class PluginRegistry {
public:
    static PluginRegistry& getInstance() {
        static PluginRegistry instance;
        return instance;
    }
    
    // Register a plugin factory
    void registerPlugin(const std::string& name, PluginFactory factory, PluginType type) {
        factories_[name] = {factory, type};
    }
    
    // Get plugin factory by name
    PluginFactory getPluginFactory(const std::string& name) const {
        auto it = factories_.find(name);
        if (it != factories_.end()) {
            return it->second.factory;
        }
        return nullptr;
    }
    
    // Get all registered plugin names
    std::vector<std::string> getPluginNames() const {
        std::vector<std::string> names;
        for (const auto& pair : factories_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get plugin type
    PluginType getPluginType(const std::string& name) const {
        auto it = factories_.find(name);
        if (it != factories_.end()) {
            return it->second.type;
        }
        return PluginType::Extension;
    }
    
private:
    struct PluginInfo {
        PluginFactory factory;
        PluginType type;
    };
    
    std::map<std::string, PluginInfo> factories_;
    
    PluginRegistry() = default;
    ~PluginRegistry() = default;
    PluginRegistry(const PluginRegistry&) = delete;
    PluginRegistry& operator=(const PluginRegistry&) = delete;
};

// Example plugin implementations

// Sensory Enhancement Plugin
class SensoryEnhancementPlugin : public PluginBase {
public:
    SensoryEnhancementPlugin() 
        : PluginBase("sensory_enhancement", "1.0.0", "Enhances sensory processing capabilities", PluginType::SystemComponent) {}
    
    bool initialize(const std::map<std::string, std::string>& config) override {
        // Configure sensory enhancement parameters
        sensitivity_ = std::stoi(config.at("sensitivity"));
        noise_reduction_ = std::stof(config.at("noise_reduction"));
        processing_channels_ = std::stoi(config.at("processing_channels"));
        
        // Apply enhancements
        applyEnhancements();
        setInitialized(true);
        return true;
    }
    
    void shutdown() override {
        // Reset sensory enhancements
        resetEnhancements();
        setInitialized(false);
    }
    
    std::vector<std::string> getRequiredComponents() const override {
        return {"SensorySystem", "NeuralDynamics"};
    }
    
    std::vector<std::string> getProvidedComponents() const override {
        return {"EnhancedSensoryOutput", "AdvancedPerception"};
    }
    
private:
    void applyEnhancements() {
        // Implementation of sensory enhancements
    }
    
    void resetEnhancements() {
        // Reset to normal sensory processing
    }
    
    int sensitivity_ = 100;
    float noise_reduction_ = 0.95f;
    int processing_channels_ = 8;
};

// Neuromodulation Plugin
class NeuromodulationPlugin : public PluginBase {
public:
    NeuromodulationPlugin()
        : PluginBase("neuromodulation", "1.0.0", "Advanced neuromodulation control and optimization", PluginType::ProcessingModule) {}
    
    bool initialize(const std::map<std::string, std::string>& config) override {
        // Configure neuromodulation parameters
        dopamine_sensitivity_ = std::stof(config.at("dopamine_sensitivity"));
        curiosity_strength_ = std::stof(config.at("curiosity_strength"));
        novelty_sensitivity_ = std::stof(config.at("novelty_sensitivity"));
        
        // Initialize neuromodulators
        initializeNeuromodulators();
        setInitialized(true);
        return true;
    }
    
    void shutdown() override {
        // Shutdown neuromodulators
        shutdownNeuromodulators();
        setInitialized(false);
    }
    
    void onBrainInitialized(std::shared_ptr<class Brain> brain) override {
        // Connect neuromodulation to brain
        connectToBrain(brain);
    }
    
    std::vector<std::string> getRequiredComponents() const override {
        return {"NeuromodulationSystem", "RewardSystem"};
    }
    
    std::vector<std::string> getProvidedComponents() const override {
        return {"OptimizedNeuromodulation", "AdaptiveLearning"};
    }
    
private:
    void initializeNeuromodulators() {
        // Initialize dopamine, curiosity, and novelty systems
    }
    
    void shutdownNeuromodulators() {
        // Shutdown neuromodulation systems
    }
    
    void connectToBrain(std::shared_ptr<class Brain> brain) {
        // Connect plugin to brain systems
    }
    
    float dopamine_sensitivity_ = 1.0f;
    float curiosity_strength_ = 1.0f;
    float novelty_sensitivity_ = 1.0f;
};

// Visualization Plugin
class AdvancedVisualizationPlugin : public PluginBase {
public:
    AdvancedVisualizationPlugin()
        : PluginBase("advanced_visualization", "1.0.0", "Provides advanced visualization and analysis capabilities", PluginType::Visualization) {}
    
    bool initialize(const std::map<std::string, std::string>& config) override {
        // Configure visualization parameters
        output_format_ = config.at("output_format");
        update_rate_ = std::stoi(config.at("update_rate"));
        enable_3d_rendering_ = config.at("enable_3d") == "true";
        enable_animations_ = config.at("enable_animations") == "true";
        
        // Initialize visualization components
        initializeVisualizationComponents();
        setInitialized(true);
        return true;
    }
    
    void shutdown() override {
        // Shutdown visualization components
        shutdownVisualizationComponents();
        setInitialized(false);
    }
    
    void onStepCompleted(size_t step) override {
        if (initialized_) {
            updateVisualization(step);
        }
    }
    
    std::vector<std::string> getRequiredComponents() const override {
        return {"VisualizationInterface", "DataAnalyzer"};
    }
    
    std::vector<std::string> getProvidedComponents() const override {
        return {"AdvancedVisualizer", "AnalysisTools"};
    }
    
private:
    void initializeVisualizationComponents() {
        // Initialize visualization components (3D renderer, animation engine, etc.)
    }
    
    void shutdownVisualizationComponents() {
        // Shutdown visualization components
    }
    
    void updateVisualization(size_t step) {
        // Update visualization based on current brain state
    }
    
    std::string output_format_ = "json";
    int update_rate_ = 30;
    bool enable_3d_rendering_ = false;
    bool enable_animations_ = true;
};

// Experimental Plugin
class ExperimentalPlugin : public PluginBase {
public:
    ExperimentalPlugin()
        : PluginBase("experimental", "1.0.0", "Provides experimental features and research capabilities", PluginType::Experimental) {}
    
    bool initialize(const std::map<std::string, std::string>& config) override {
        // Configure experimental features
        enable_experimental_features_ = config.at("enable_experimental") == "true";
        research_mode_ = config.at("research_mode") == "true";
        enable_hypothesis_testing_ = config.at("enable_hypothesis_testing") == "true";
        
        // Initialize experimental systems
        initializeExperimentalSystems();
        setInitialized(true);
        return true;
    }
    
    void shutdown() override {
        // Shutdown experimental systems
        shutdownExperimentalSystems();
        setInitialized(false);
    }
    
    void onExperimentCompleted(const std::string& experimentName) override {
        // Record experimental data
        recordExperimentalData(experimentName);
    }
    
    std::vector<std::string> getRequiredComponents() const override {
        return {"DataRecorder", "ExperimentFramework"};
    }
    
    std::vector<std::string> getProvidedComponents() const override {
        return {"ExperimentalData", "ResearchTools"};
    }
    
private:
    void initializeExperimentalSystems() {
        // Initialize experimental research systems
    }
    
    void shutdownExperimentalSystems() {
        // Shutdown experimental systems
    }
    
    void recordExperimentalData(const std::string& experimentName) {
        // Record data for research and analysis
    }
    
    bool enable_experimental_features_ = false;
    bool research_mode_ = false;
    bool enable_hypothesis_testing_ = false;
};

// Macro for easy plugin registration
#define REGISTER_PLUGIN(name, factory, type) \
    namespace { \
        struct PluginRegistrationHelper { \
            PluginRegistrationHelper() { \
                PluginRegistry::getInstance().registerPlugin(name, factory, type); \
            } \
        }; \
        static PluginRegistrationHelper pluginRegistrationHelper; \
    }

// Convenience macro for common plugin types
#define REGISTER_SIMPLE_PLUGIN(name, pluginClass, type) \
    namespace { \
        struct PluginRegistrationHelper { \
            PluginRegistrationHelper() { \
                PluginRegistry::getInstance().registerPlugin(name, []() { return std::make_unique<pluginClass>(); }, type); \
            } \
        }; \
        static PluginRegistrationHelper pluginRegistrationHelper; \
    }

// Example usage macros
// REGISTER_SIMPLE_PLUGIN("sensory_enhancement", SensoryEnhancementPlugin, PluginType::SystemComponent);
// REGISTER_SIMPLE_PLUGIN("neuromodulation", NeuromodulationPlugin, PluginType::ProcessingModule);
// REGISTER_SIMPLE_PLUGIN("advanced_visualization", AdvancedVisualizationPlugin, PluginType::Visualization);
// REGISTER_SIMPLE_PLUGIN("experimental", ExperimentalPlugin, PluginType::Experimental);

} // namespace nlm