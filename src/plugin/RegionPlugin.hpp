#pragma once

#include "Plugin.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/NeuralRegion.hpp"
#include <string>
#include <memory>

namespace nlm {

// Abstract base class for brain region plugins
class RegionPlugin : public virtual IPlugin, public virtual IConfigurable {
public:
    virtual ~RegionPlugin() = default;
    
    // Plugin category identification
    const char* getCategory() const override { return "BrainRegion"; }
    
    // Region plugin specific operations
    virtual std::unique_ptr<NeuralRegion> createRegion(RegionId id, const std::string& name = "") = 0;
    virtual bool configureRegion(NeuralRegion* region, const std::unordered_map<std::string, std::string>& config) = 0;
    virtual void updateRegion(NeuralRegion* region, TimestepDuration dt) = 0;
    
    // Region-specific configuration parameters
    virtual std::vector<std::string> getRegionConfigParameters() const {
        return {"regionSize", "neuronDensity", "connectionProbability", "initialWeightMean", "initialWeightStd"};
    }
    
    virtual std::string getRegionParameterType(const std::string& paramName) const {
        if (paramName == "regionSize") return "int";
        if (paramName == "neuronDensity") return "float";
        if (paramName == "connectionProbability") return "float";
        if (paramName == "initialWeightMean") return "float";
        if (paramName == "initialWeightStd") return "float";
        return "string";
    }
    
    virtual std::string getRegionParameterDescription(const std::string& paramName) const {
        if (paramName == "regionSize") return "Number of neurons in the region";
        if (paramName == "neuronDensity") return "Neurons per unit volume";
        if (paramName == "connectionProbability") return "Probability of synaptic connections";
        if (paramName == "initialWeightMean") return "Mean synaptic weight at initialization";
        if (paramName == "initialWeightStd") return "Standard deviation of initial weights";
        return "";
    }
};

// Example brain region plugin: DefaultRegionPlugin
class DefaultRegionPlugin : public RegionPlugin {
public:
    DefaultRegionPlugin() = default;
    ~DefaultRegionPlugin() override = default;
    
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata = {
            "DefaultRegionPlugin", "1.0.0", "Standard brain region implementation",
            "NLM Team", {}, 50, "BrainRegion", {{"regionSize", "100"}, {"neuronDensity", "0.01"}}
        };
        return metadata;
    }
    
    bool initialize(const std::unordered_map<std::string, std::string>& config) override {
        setState(PluginState::Initialized);
        return true;
    }
    
    void update(TimestepDuration dt) override {
        // Default implementation: no custom updates
    }
    
    void shutdown() override {
        setState(PluginState::Unloaded);
    }
    
    PluginState getState() const override {
        return state_;
    }
    
    const char* getName() const override { return "DefaultRegionPlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    
    bool isValid() const override {
        return state_ == PluginState::Initialized || state_ == PluginState::Running;
    }
    
    bool configure(const std::unordered_map<std::string, std::string>& config) override {
        regionSize_ = getIntFromConfig(config, "regionSize", 100);
        neuronDensity_ = getFloatFromConfig(config, "neuronDensity", 0.01f);
        connectionProb_ = getFloatFromConfig(config, "connectionProbability", 0.1f);
        initialWeightMean_ = getFloatFromConfig(config, "initialWeightMean", 0.5f);
        initialWeightStd_ = getFloatFromConfig(config, "initialWeightStd", 0.1f);
        return true;
    }
    
    const std::unordered_map<std::string, std::string>& getConfiguration() const override {
        return config_;
    }
    
    std::vector<std::string> getConfigParameters() const override {
        return getRegionConfigParameters();
    }
    
    std::string getParameterType(const std::string& paramName) const override {
        return getRegionParameterType(paramName);
    }
    
    std::string getParameterDescription(const std::string& paramName) const override {
        return getRegionParameterDescription(paramName);
    }
    
    std::unique_ptr<NeuralRegion> createRegion(RegionId id, const std::string& name = "") override {
        auto region = std::make_unique<NeuralRegion>(id, name);
        configureRegion(region.get(), config_);
        return region;
    }
    
    bool configureRegion(NeuralRegion* region, const std::unordered_map<std::string, std::string>& config) override {
        if (!region) return false;
        // Configure region based on plugin parameters
        region->setName(config.find("regionName") != config.end() ? config.at("regionName") : "");
        return true;
    }
    
    void updateRegion(NeuralRegion* region, TimestepDuration dt) override {
        // Update region with custom dynamics based on plugin configuration
        if (region) {
            // Apply custom region properties
        }
    }
    
    float getCpuUsage() const override { return 0.1f; }
    size_t getMemoryUsage() const override { return sizeof(DefaultRegionPlugin); }
    size_t getActiveObjects() const override { return 1; }
    float getPerformanceScore() const override { return 1.0f; }
    void resetPerformanceMetrics() override {}
    
    bool saveState(const std::string& filepath) const override { return false; }
    bool loadState(const std::string& filepath) override { return false; }
    bool hasState() const override { return false; }
    void clearState() override {}
    std::string getStateSignature() const override { return "DefaultRegionPlugin_v1"; }
    
private:
    PluginState state_ = PluginState::Unloaded;
    int regionSize_ = 100;
    float neuronDensity_ = 0.01f;
    float connectionProb_ = 0.1f;
    float initialWeightMean_ = 0.5f;
    float initialWeightStd_ = 0.1f;
    std::unordered_map<std::string, std::string> config_;
    
    int getIntFromConfig(const std::unordered_map<std::string, std::string>& config, 
                        const std::string& key, int defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? std::stoi(it->second) : defaultValue;
    }
    
    float getFloatFromConfig(const std::unordered_map<std::string, std::string>& config,
                            const std::string& key, float defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? std::stof(it->second) : defaultValue;
    }
    
    void setState(PluginState state) {
        state_ = state;
    }
};

// Example custom region plugin: ActiveRegionPlugin
class ActiveRegionPlugin : public RegionPlugin {
public:
    ActiveRegionPlugin() = default;
    ~ActiveRegionPlugin() override = default;
    
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata = {
            "ActiveRegionPlugin", "1.0.0", "Dynamically active region with custom learning",
            "NLM Team", {}, 75, "BrainRegion", {{}, {}}
        };
        return metadata;
    }
    
    bool initialize(const std::unordered_map<std::string, std::string>& config) override {
        learningRate_ = getFloatFromConfig(config, "learningRate", 0.01f);
        maxWeightChange_ = getFloatFromConfig(config, "maxWeightChange", 1.0f);
        activityThreshold_ = getFloatFromConfig(config, "activityThreshold", 0.5f);
        setState(PluginState::Initialized);
        return true;
    }
    
    void update(TimestepDuration dt) override {
        // Custom update logic for active regions
    }
    
    void shutdown() override {
        setState(PluginState::Unloaded);
    }
    
    PluginState getState() const override {
        return state_;
    }
    
    const char* getName() const override { return "ActiveRegionPlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    
    bool isValid() const override {
        return state_ == PluginState::Initialized || state_ == PluginState::Running;
    }
    
    bool configure(const std::unordered_map<std::string, std::string>& config) override {
        learningRate_ = getFloatFromConfig(config, "learningRate", 0.01f);
        maxWeightChange_ = getFloatFromConfig(config, "maxWeightChange", 1.0f);
        activityThreshold_ = getFloatFromConfig(config, "activityThreshold", 0.5f);
        return true;
    }
    
    const std::unordered_map<std::string, std::string>& getConfiguration() const override {
        return config_;
    }
    
    std::vector<std::string> getConfigParameters() const override {
        return {"learningRate", "maxWeightChange", "activityThreshold"};
    }
    
    std::string getParameterType(const std::string& paramName) const override {
        return "float";
    }
    
    std::string getParameterDescription(const std::string& paramName) const override {
        if (paramName == "learningRate") return "Learning rate for synaptic updates";
        if (paramName == "maxWeightChange") return "Maximum weight change per update";
        if (paramName == "activityThreshold") return "Firing threshold for plasticity";
        return "";
    }
    
    std::unique_ptr<NeuralRegion> createRegion(RegionId id, const std::string& name = "") override {
        auto region = std::make_unique<NeuralRegion>(id, name);
        configureRegion(region.get(), config_);
        return region;
    }
    
    bool configureRegion(NeuralRegion* region, const std::unordered_map<std::string, std::string>& config) override {
        if (!region) return false;
        // Configure region with active learning properties
        region->setName(config.find("regionName") != config.end() ? config.at("regionName") : "");
        return true;
    }
    
    void updateRegion(NeuralRegion* region, TimestepDuration dt) override {
        // Custom region dynamics with learning
        if (region && isValid()) {
            // Apply custom learning rules based on region activity
        }
    }
    
    float getCpuUsage() const override { return learningRate_ * 0.5f; }
    size_t getMemoryUsage() const override { return sizeof(ActiveRegionPlugin); }
    size_t getActiveObjects() const override { return 1; }
    float getPerformanceScore() const override { return 1.0f / (1.0f + learningRate_); }
    void resetPerformanceMetrics() override {}
    
    bool saveState(const std::string& filepath) const override { return false; }
    bool loadState(const std::string& filepath) override { return false; }
    bool hasState() const override { return false; }
    void clearState() override {}
    std::string getStateSignature() const override { return "ActiveRegionPlugin_v1"; }
    
private:
    PluginState state_ = PluginState::Unloaded;
    float learningRate_ = 0.01f;
    float maxWeightChange_ = 1.0f;
    float activityThreshold_ = 0.5f;
    std::unordered_map<std::string, std::string> config_;
    
    float getFloatFromConfig(const std::unordered_map<std::string, std::string>& config,
                            const std::string& key, float defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? std::stof(it->second) : defaultValue;
    }
    
    void setState(PluginState state) {
        state_ = state;
    }
};

} // namespace nlm