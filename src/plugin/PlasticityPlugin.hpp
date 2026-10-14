#pragma once

#include "Plugin.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include <string>
#include <memory>
#include <unordered_map>

namespace nlm {

// Abstract base class for plasticity rule plugins
class PlasticityPlugin : public virtual IPlugin, public virtual IConfigurable {
public:
    virtual ~PlasticityPlugin() = default;
    
    // Plugin category identification
    const char* getCategory() const override { return "PlasticityRule"; }
    
    // Plasticity plugin specific operations
    virtual std::unique_ptr<PlasticityRule> createRule(const std::string& ruleName = "Default") = 0;
    virtual bool configureRule(PlasticityRule* rule, const std::unordered_map<std::string, std::string>& config) = 0;
    virtual void updateRule(PlasticityRule* rule, TimestepDuration dt) = 0;
    
    // Plasticity-specific configuration parameters
    virtual std::vector<std::string> getRuleConfigParameters() const {
        return {"learningRate", "weightDecay", "minWeight", "maxWeight", "eligibilityWindow"};
    }
    
    virtual std::string getRuleParameterType(const std::string& paramName) const {
        if (paramName == "learningRate") return "float";
        if (paramName == "weightDecay") return "float";
        if (paramName == "minWeight") return "float";
        if (paramName == "maxWeight") return "float";
        if (paramName == "eligibilityWindow") return "int";
        return "string";
    }
    
    virtual std::string getRuleParameterDescription(const std::string& paramName) const {
        if (paramName == "learningRate") return "Learning rate for weight updates";
        if (paramName == "weightDecay") return "Exponential decay factor for weights";
        if (paramName == "minWeight") return "Minimum allowed synaptic weight";
        if (paramName == "maxWeight") return "Maximum allowed synaptic weight";
        if (paramName == "eligibilityWindow") return "Time window for eligibility traces";
        return "";
    }
};

// Example plasticity plugin: STDPPlasticityPlugin
class STDPPlasticityPlugin : public PlasticityPlugin {
public:
    STDPPlasticityPlugin() = default;
    ~STDPPlasticityPlugin() override = default;
    
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata = {
            "STDPPlasticityPlugin", "1.0.0", "Spike-Timing-Dependent Plasticity rule implementation",
            "NLM Team", {"BasicSTDP"}, 80, "PlasticityRule", {
                {"learningRate", "0.01"}, {"weightDecay", "0.0"}, {"minWeight", "-1.0"},
                {"maxWeight", "1.0"}, {"eligibilityWindow", "100"}
            }
        };
        return metadata;
    }
    
    bool initialize(const std::unordered_map<std::string, std::string>& config) override {
        // Initialize STDP-specific parameters
        setState(PluginState::Initialized);
        return true;
    }
    
    void update(TimestepDuration dt) override {
        // Update STDP parameters or global state
    }
    
    void shutdown() override {
        setState(PluginState::Unloaded);
    }
    
    PluginState getState() const override {
        return state_;
    }
    
    const char* getName() const override { return "STDPPlasticityPlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    
    bool isValid() const override {
        return state_ == PluginState::Initialized || state_ == PluginState::Running;
    }
    
    bool configure(const std::unordered_map<std::string, std::string>& config) override {
        learningRate_ = getFloatFromConfig(config, "learningRate", 0.01f);
        weightDecay_ = getFloatFromConfig(config, "weightDecay", 0.0f);
        minWeight_ = getFloatFromConfig(config, "minWeight", -1.0f);
        maxWeight_ = getFloatFromConfig(config, "maxWeight", 1.0f);
        eligibilityWindow_ = getIntFromConfig(config, "eligibilityWindow", 100);
        return true;
    }
    
    const std::unordered_map<std::string, std::string>& getConfiguration() const override {
        return config_;
    }
    
    std::vector<std::string> getConfigParameters() const override {
        return getRuleConfigParameters();
    }
    
    std::string getParameterType(const std::string& paramName) const override {
        return getRuleParameterType(paramName);
    }
    
    std::string getParameterDescription(const std::string& paramName) const override {
        return getRuleParameterDescription(paramName);
    }
    
    std::unique_ptr<PlasticityRule> createRule(const std::string& ruleName = "Default") override {
        auto rule = std::make_unique<STDPPlasticityRule>();
        configureRule(rule.get(), config_);
        return rule;
    }
    
    bool configureRule(PlasticityRule* rule, const std::unordered_map<std::string, std::string>& config) override {
        if (!rule) return false;
        
        if (auto* stdpRule = dynamic_cast<STDPPlasticityRule*>(rule)) {
            stdpRule->setLearningRate(learningRate_);
            stdpRule->setWeightDecay(weightDecay_);
            stdpRule->setMinWeight(minWeight_);
            stdpRule->setMaxWeight(maxWeight_);
        }
        return true;
    }
    
    void updateRule(PlasticityRule* rule, TimestepDuration dt) override {
        // Update rule parameters based on plugin configuration
        if (rule && isValid()) {
            // Apply custom STDP dynamics
        }
    }
    
    float getCpuUsage() const override { return learningRate_ * 0.3f; }
    size_t getMemoryUsage() const override { return sizeof(STDPPlasticityPlugin); }
    size_t getActiveObjects() const override { return 1; }
    float getPerformanceScore() const override { return 1.0f / (1.0f + learningRate_); }
    void resetPerformanceMetrics() override {}
    
    bool saveState(const std::string& filepath) const override { return false; }
    bool loadState(const std::string& filepath) override { return false; }
    bool hasState() const override { return false; }
    void clearState() override {}
    std::string getStateSignature() const override { return "STDPPlasticityPlugin_v1"; }
    
private:
    PluginState state_ = PluginState::Unloaded;
    float learningRate_ = 0.01f;
    float weightDecay_ = 0.0f;
    float minWeight_ = -1.0f;
    float maxWeight_ = 1.0f;
    int eligibilityWindow_ = 100;
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

// Note: This would require including the actual STDPPlasticityRule from the plasticity module
// For now, this is a placeholder implementation
class STDPPlasticityRule {
public:
    void setLearningRate(float rate) {}
    void setWeightDecay(float decay) {}
    void setMinWeight(float min) {}
    void setMaxWeight(float max) {}
};

// Example plasticity plugin: HebbianPlasticityPlugin
class HebbianPlasticityPlugin : public PlasticityPlugin {
public:
    HebbianPlasticityPlugin() = default;
    ~HebbianPlasticityPlugin() override = default;
    
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata = {
            "HebbianPlasticityPlugin", "1.0.0", "Classic Hebbian learning rule implementation",
            "NLM Team", {}, 60, "PlasticityRule", {
                {"learningRate", "0.1"}, {"weightDecay", "0.0"}, {"minWeight", "-1.0"},
                {"maxWeight", "1.0"}
            }
        };
        return metadata;
    }
    
    bool initialize(const std::unordered_map<std::string, std::string>& config) override {
        setState(PluginState::Initialized);
        return true;
    }
    
    void update(TimestepDuration dt) override {
        // Update Hebbian parameters or global state
    }
    
    void shutdown() override {
        setState(PluginState::Unloaded);
    }
    
    PluginState getState() const override {
        return state_;
    }
    
    const char* getName() const override { return "HebbianPlasticityPlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    
    bool isValid() const override {
        return state_ == PluginState::Initialized || state_ == PluginState::Running;
    }
    
    bool configure(const std::unordered_map<std::string, std::string>& config) override {
        learningRate_ = getFloatFromConfig(config, "learningRate", 0.1f);
        weightDecay_ = getFloatFromConfig(config, "weightDecay", 0.0f);
        minWeight_ = getFloatFromConfig(config, "minWeight", -1.0f);
        maxWeight_ = getFloatFromConfig(config, "maxWeight", 1.0f);
        return true;
    }
    
    const std::unordered_map<std::string, std::string>& getConfiguration() const override {
        return config_;
    }
    
    std::vector<std::string> getConfigParameters() const override {
        return getRuleConfigParameters();
    }
    
    std::string getParameterType(const std::string& paramName) const override {
        return getRuleParameterType(paramName);
    }
    
    std::string getParameterDescription(const std::string& paramName) const override {
        return getRuleParameterDescription(paramName);
    }
    
    std::unique_ptr<PlasticityRule> createRule(const std::string& ruleName = "Default") override {
        auto rule = std::make_unique<HebbianRule>();
        configureRule(rule.get(), config_);
        return rule;
    }
    
    bool configureRule(PlasticityRule* rule, const std::unordered_map<std::string, std::string>& config) override {
        if (!rule) return false;
        
        if (auto* hebbianRule = dynamic_cast<HebbianRule*>(rule)) {
            hebbianRule->setLearningRate(learningRate_);
            hebbianRule->setWeightDecay(weightDecay_);
        }
        return true;
    }
    
    void updateRule(PlasticityRule* rule, TimestepDuration dt) override {
        // Update rule parameters based on plugin configuration
        if (rule && isValid()) {
            // Apply custom Hebbian dynamics
        }
    }
    
    float getCpuUsage() const override { return learningRate_ * 0.2f; }
    size_t getMemoryUsage() const override { return sizeof(HebbianPlasticityPlugin); }
    size_t getActiveObjects() const override { return 1; }
    float getPerformanceScore() const override { return 1.0f / (1.0f + learningRate_); }
    void resetPerformanceMetrics() override {}
    
    bool saveState(const std::string& filepath) const override { return false; }
    bool loadState(const std::string& filepath) override { return false; }
    bool hasState() const override { return false; }
    void clearState() override {}
    std::string getStateSignature() const override { return "HebbianPlasticityPlugin_v1"; }
    
private:
    PluginState state_ = PluginState::Unloaded;
    float learningRate_ = 0.1f;
    float weightDecay_ = 0.0f;
    float minWeight_ = -1.0f;
    float maxWeight_ = 1.0f;
    std::unordered_map<std::string, std::string> config_;
    
    float getFloatFromConfig(const std::unordered_map<std::string, std::string>& config,
                            const std::string& key, float defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? std::stof(it->second) : defaultValue;
    }
    
    int getIntFromConfig(const std::unordered_map<std::string, std::string>& config,
                        const std::string& key, int defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? std::stoi(it->second) : defaultValue;
    }
    
    void setState(PluginState state) {
        state_ = state;
    }
};

} // namespace nlm