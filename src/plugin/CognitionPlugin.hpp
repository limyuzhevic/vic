#pragma once

#include "Plugin.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include <string>
#include <memory>
#include <unordered_map>

namespace nlm {

// Abstract base class for cognition system plugins
class CognitionSystemPlugin : public virtual IPlugin, public virtual IConfigurable {
public:
    virtual ~CognitionSystemPlugin() = default;
    
    // Plugin category identification
    const char* getCategory() const override { return "Cognition"; }
    
    // Cognition plugin specific operations
    virtual std::unique_ptr<ConceptFormation> createConceptFormation(const std::string& name = "Default") = 0;
    virtual std::unique_ptr<NeuralPlanner> createNeuralPlanner(const std::string& name = "Default") = 0;
    virtual bool configureCognitiveSystem(class CognitiveSystem* system, const std::unordered_map<std::string, std::string>& config) = 0;
    virtual void updateCognitiveSystem(class CognitiveSystem* system, TimestepDuration dt) = 0;
    
    // Cognition-specific configuration parameters
    virtual std::vector<std::string> getCognitiveConfigParameters() const {
        return {"learningRate", "conceptThreshold", "planningHorizon", "attentionSpan", "memoryIntegration"};
    }
    
    virtual std::string getCognitiveParameterType(const std::string& paramName) const {
        if (paramName == "learningRate") return "float";
        if (paramName == "conceptThreshold") return "float";
        if (paramName == "planningHorizon") return "int";
        if (paramName == "attentionSpan") return "float";
        if (paramName == "memoryIntegration") return "float";
        return "string";
    }
    
    virtual std::string getCognitiveParameterDescription(const std::string& paramName) const {
        if (paramName == "learningRate") return "Learning rate for concept formation";
        if (paramName == "conceptThreshold") return "Threshold for concept activation";
        if (paramName == "planningHorizon") return "Time horizon for action planning";
        if (paramName == "attentionSpan") return "Duration of attention focus";
        if (paramName == "memoryIntegration") return "Strength of memory integration";
        return "";
    }
};

// Example cognition plugin: ConceptFormationPlugin
class ConceptFormationPlugin : public CognitionSystemPlugin {
public:
    ConceptFormationPlugin() = default;
    ~ConceptFormationPlugin() override = default;
    
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata = {
            "ConceptFormationPlugin", "1.0.0", "Concept formation system for pattern discovery",
            "NLM Team", {}, 65, "Cognition", {
                {"learningRate", "0.1"}, {"conceptThreshold", "0.5"}, {"attentionSpan", "10.0"}
            }
        };
        return metadata;
    }
    
    bool initialize(const std::unordered_map<std::string, std::string>& config) override {
        setState(PluginState::Initialized);
        return true;
    }
    
    void update(TimestepDuration dt) override {
        // Update concept formation dynamics
        if (cognitiveSystem_) {
            updateConceptFormation(cognitiveSystem_.get(), dt);
        }
    }
    
    void shutdown() override {
        if (cognitiveSystem_) {
            cognitiveSystem_.reset();
        }
        setState(PluginState::Unloaded);
    }
    
    PluginState getState() const override {
        return state_;
    }
    
    const char* getName() const override { return "ConceptFormationPlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    
    bool isValid() const override {
        return state_ == PluginState::Initialized || state_ == PluginState::Running;
    }
    
    bool configure(const std::unordered_map<std::string, std::string>& config) override {
        learningRate_ = getFloatFromConfig(config, "learningRate", 0.1f);
        conceptThreshold_ = getFloatFromConfig(config, "conceptThreshold", 0.5f);
        attentionSpan_ = getFloatFromConfig(config, "attentionSpan", 10.0f);
        return true;
    }
    
    const std::unordered_map<std::string, std::string>& getConfiguration() const override {
        return config_;
    }
    
    std::vector<std::string> getConfigParameters() const override {
        return getCognitiveConfigParameters();
    }
    
    std::string getParameterType(const std::string& paramName) const override {
        return getCognitiveParameterType(paramName);
    }
    
    std::string getParameterDescription(const std::string& paramName) const override {
        return getCognitiveParameterDescription(paramName);
    }
    
    std::unique_ptr<ConceptFormation> createConceptFormation(const std::string& name = "Default") override {
        auto conceptFormation = std::make_unique<ConceptFormation>();
        conceptFormation->setLearningRate(learningRate_);
        conceptFormation->setConceptThreshold(conceptThreshold_);
        conceptFormation->setAttentionSpan(attentionSpan_);
        conceptFormation->setName(name);
        return conceptFormation;
    }
    
    std::unique_ptr<NeuralPlanner> createNeuralPlanner(const std::string& name = "Default") override {
        auto neuralPlanner = std::make_unique<NeuralPlanner>();
        neuralPlanner->setPlanningHorizon(planningHorizon_);
        neuralPlanner->setName(name);
        return neuralPlanner;
    }
    
    bool configureCognitiveSystem(class CognitiveSystem* system, const std::unordered_map<std::string, std::string>& config) override {
        if (!system) return false;
        
        system->setLearningRate(learningRate_);
        system->setConceptThreshold(conceptThreshold_);
        system->setAttentionSpan(attentionSpan_);
        system->setPlanningHorizon(planningHorizon_);
        system->setMemoryIntegration(memoryIntegration_);
        
        return true;
    }
    
    void updateCognitiveSystem(class CognitiveSystem* system, TimestepDuration dt) override {
        // Update cognitive system dynamics
        if (system) {
            updateConceptFormation(system, dt);
            updateAttention(system, dt);
            integrateMemory(system, dt);
        }
    }
    
    void updateConceptFormation(class CognitiveSystem* system, TimestepDuration dt) {
        if (!system) return;
        
        // Update concept formation based on plugin parameters
        // This would involve updating concept activation levels
        // and learning based on sensory input
    }
    
    void updateAttention(class CognitiveSystem* system, TimestepDuration dt) {
        if (!system) return;
        
        // Update attention span and focus
        // This would involve updating attentional parameters
        // and selecting relevant concepts
    }
    
    void integrateMemory(class CognitiveSystem* system, TimestepDuration dt) {
        if (!system) return;
        
        // Integrate with memory systems
        // This would involve retrieving relevant memories
        // and incorporating them into cognitive processes
    }
    
    float getCpuUsage() const override { return learningRate_ * 0.3f; }
    size_t getMemoryUsage() const override { return sizeof(ConceptFormationPlugin); }
    size_t getActiveObjects() const override { return 1; }
    float getPerformanceScore() const override { return 1.0f / (1.0f + learningRate_); }
    void resetPerformanceMetrics() override {}
    
    bool saveState(const std::string& filepath) const override { return false; }
    bool loadState(const std::string& filepath) override { return false; }
    bool hasState() const override { return false; }
    void clearState() override {}
    std::string getStateSignature() const override { return "ConceptFormationPlugin_v1"; }
    
    void setCognitiveSystem(std::unique_ptr<class CognitiveSystem> system) {
        cognitiveSystem_ = std::move(system);
    }
    
    class CognitiveSystem* getCognitiveSystem() {
        return cognitiveSystem_.get();
    }
    
    const class CognitiveSystem* getCognitiveSystem() const {
        return cognitiveSystem_.get();
    }
    
private:
    PluginState state_ = PluginState::Unloaded;
    float learningRate_ = 0.1f;
    float conceptThreshold_ = 0.5f;
    float attentionSpan_ = 10.0f;
    int planningHorizon_ = 10;
    float memoryIntegration_ = 0.8f;
    std::unique_ptr<class CognitiveSystem> cognitiveSystem_;
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

// Example cognition plugin: AttentionPlugin
class AttentionPlugin : public CognitionSystemPlugin {
public:
    AttentionPlugin() = default;
    ~AttentionPlugin() override = default;
    
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata = {
            "AttentionPlugin", "1.0.0", "Attention selection and focus system",
            "NLM Team", {}, 60, "Cognition", {
                {"attentionSpan", "5.0"}, {"focusStrength", "0.8"}, {"switchCost", "0.5"}
            }
        };
        return metadata;
    }
    
    bool initialize(const std::unordered_map<std::string, std::string>& config) override {
        setState(PluginState::Initialized);
        return true;
    }
    
    void update(TimestepDuration dt) override {
        // Update attention dynamics
        if (cognitiveSystem_) {
            updateAttentionDynamics(cognitiveSystem_.get(), dt);
        }
    }
    
    void shutdown() override {
        if (cognitiveSystem_) {
            cognitiveSystem_.reset();
        }
        setState(PluginState::Unloaded);
    }
    
    PluginState getState() const override {
        return state_;
    }
    
    const char* getName() const override { return "AttentionPlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    
    bool isValid() const override {
        return state_ == PluginState::Initialized || state_ == PluginState::Running;
    }
    
    bool configure(const std::unordered_map<std::string, std::string>& config) override {
        attentionSpan_ = getFloatFromConfig(config, "attentionSpan", 5.0f);
        focusStrength_ = getFloatFromConfig(config, "focusStrength", 0.8f);
        switchCost_ = getFloatFromConfig(config, "switchCost", 0.5f);
        return true;
    }
    
    const std::unordered_map<std::string, std::string>& getConfiguration() const override {
        return config_;
    }
    
    std::vector<std::string> getConfigParameters() const override {
        return getCognitiveConfigParameters();
    }
    
    std::string getParameterType(const std::string& paramName) const override {
        return getCognitiveParameterType(paramName);
    }
    
    std::string getParameterDescription(const std::string& paramName) const override {
        return getCognitiveParameterDescription(paramName);
    }
    
    std::unique_ptr<ConceptFormation> createConceptFormation(const std::string& name = "Default") override {
        auto conceptFormation = std::make_unique<ConceptFormation>();
        conceptFormation->setAttentionSpan(attentionSpan_);
        conceptFormation->setName(name + "_Attentional");
        return conceptFormation;
    }
    
    std::unique_ptr<NeuralPlanner> createNeuralPlanner(const std::string& name = "Default") override {
        auto neuralPlanner = std::make_unique<NeuralPlanner>();
        neuralPlanner->setPlanningHorizon(planningHorizon_);
        neuralPlanner->setName(name + "_Attentional");
        return neuralPlanner;
    }
    
    bool configureCognitiveSystem(class CognitiveSystem* system, const std::unordered_map<std::string, std::string>& config) override {
        if (!system) return false;
        
        system->setAttentionSpan(attentionSpan_);
        system->setFocusStrength(focusStrength_);
        system->setSwitchCost(switchCost_);
        
        return true;
    }
    
    void updateCognitiveSystem(class CognitiveSystem* system, TimestepDuration dt) override {
        // Update attention system dynamics
        if (system) {
            updateAttentionDynamics(system, dt);
            updateSelection(system, dt);
        }
    }
    
    void updateAttentionDynamics(class CognitiveSystem* system, TimestepDuration dt) {
        if (!system) return;
        
        // Update attentional dynamics
        // This would involve updating attentional parameters
        // and managing attention switching between concepts
    }
    
    void updateSelection(class CognitiveSystem* system, TimestepDuration dt) {
        if (!system) return;
        
        // Update selection of focus
        // This would involve selecting the most relevant concepts
        // based on attentional parameters
    }
    
    float getCpuUsage() const override { return switchCost_ * 0.3f; }
    size_t getMemoryUsage() const override { return sizeof(AttentionPlugin); }
    size_t getActiveObjects() const override { return 1; }
    float getPerformanceScore() const override { return 1.0f / (1.0f + switchCost_); }
    void resetPerformanceMetrics() override {}
    
    bool saveState(const std::string& filepath) const override { return false; }
    bool loadState(const std::string& filepath) override { return false; }
    bool hasState() const override { return false; }
    void clearState() override {}
    std::string getStateSignature() const override { return "AttentionPlugin_v1"; }
    
private:
    PluginState state_ = PluginState::Unloaded;
    float attentionSpan_ = 5.0f;
    float focusStrength_ = 0.8f;
    float switchCost_ = 0.5f;
    std::unique_ptr<class CognitiveSystem> cognitiveSystem_;
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