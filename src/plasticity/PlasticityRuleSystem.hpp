// Custom Plasticity Rule System
// Advanced plasticity rule engine for NLM

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <mutex>
#include <any>
#include <optional>

namespace nlm {
namespace plasticity {

// Plasticity rule types
enum class PlasticityRuleType {
    Hebbian,
    STDP,
    RewardModulated,
    Custom,
    Adaptive,
    SynapticTaggingAndCapture,
    Homeostatic
};

// Plasticity rule targets
enum class PlasticityTarget {
    Weight,
    Efficacy,
    Structure,
    Sensitivity,
    Threshold,
    Conductance
};

// Plasticity rule effects
struct PlasticityEffect {
    PlasticityTarget target;
    float delta;
    float multiplier;  // Applied to current value
    float minValue;
    float maxValue;
    float decayRate;
    bool instantaneous;  // Whether effect is applied immediately
    float eligibilityTrace;  // For reward-modulated learning
    float threshold;  // Minimum change required to trigger
};

// Plasticity rule conditions
struct PlasticityCondition {
    enum class ConditionType {
        SpikeTiming,
        Reward,
        TimeElapsed,
        WeightRange,
        ActivityLevel,
        ExternalSignal,
        Custom
    };
    
    ConditionType type;
    std::string name;
    bool enabled;
    std::unordered_map<std::string, std::any> parameters;
    std::function<bool(const std::unordered_map<std::string, std::any>&)> conditionFunc;
};

// Plasticity rule events
struct PlasticityEvent {
    enum class EventType {
        PreSpike,
        PostSpike,
        RewardReceived,
        TimeStep,
        WeightChange,
        EligibilityTraceUpdate,
        Custom
    };
    
    EventType type;
    double timestamp;
    std::string source;
    std::unordered_map<std::string, std::any> data;
    float weight;
    float predictionError;
    std::vector<float> spikeHistory;
};

// Base plasticity rule interface
class IPlasticityRule {
public:
    virtual ~IPlasticityRule() = default;
    
    // Rule metadata
    virtual std::string getName() const = 0;
    virtual PlasticityRuleType getType() const = 0;
    virtual std::string getDescription() const = 0;
    virtual float getLearningRate() const = 0;
    virtual void setLearningRate(float rate) = 0;
    
    // Rule configuration
    virtual bool configure(const std::unordered_map<std::string, std::any>& config) = 0;
    virtual std::unordered_map<std::string, std::any> getConfiguration() const = 0;
    
    // Event handling
    virtual void onEvent(const PlasticityEvent& event) = 0;
    virtual std::vector<PlasticityCondition> getConditions() const = 0;
    virtual std::vector<PlasticityEffect> getEffects() const = 0;
    
    // Plasticity computation
    virtual float computeChange(const std::unordered_map<std::string, std::any>& context) = 0;
    virtual float computeEligibilityTrace(const PlasticityEvent& event) = 0;
    virtual void updateTraces(float timestep) = 0;
    
    // Rule lifecycle
    virtual bool initialize() = 0;
    virtual bool activate() = 0;
    virtual bool deactivate() = 0;
    virtual bool shutdown() = 0;
    
    // Rule statistics
    virtual size_t getSpikeCount() const = 0;
    virtual float getTotalWeightChange() const = 0;
    virtual float getAverageLearningRate() const = 0;
    virtual std::unordered_map<std::string, std::any> getStatistics() const = 0;
};

// Typed plasticity rule template
template<typename T>
class PlasticityRuleBase : public IPlasticityRule {
public:
    PlasticityRuleBase(const std::string& name, PlasticityRuleType type, const std::string& description)
        : name_(name), type_(type), description_(description) {}
    
    std::string getName() const override { return name_; }
    PlasticityRuleType getType() const override { return type_; }
    std::string getDescription() const override { return description_; }
    float getLearningRate() const override { return learningRate_; }
    void setLearningRate(float rate) override { learningRate_ = rate; }
    
    bool configure(const std::unordered_map<std::string, std::any>& config) override {
        // Default configuration - can be overridden
        for (const auto& pair : config) {
            if (pair.first == "learning_rate") {
                setLearningRate(std::any_cast<float>(pair.second));
            } else {
                customConfig_[pair.first] = pair.second;
            }
        }
        return true;
    }
    
    std::unordered_map<std::string, std::any> getConfiguration() const override {
        std::unordered_map<std::string, std::any> config;
        config["learning_rate"] = getLearningRate();
        config.insert(customConfig_.begin(), customConfig_.end());
        return config;
    }
    
    virtual bool initialize() override { return true; }
    virtual bool activate() override { return true; }
    virtual bool deactivate() override { return true; }
    virtual bool shutdown() override { return true; }
    
protected:
    std::string name_;
    PlasticityRuleType type_;
    std::string description_;
    float learningRate_ = 0.01f;
    std::unordered_map<std::string, std::any> customConfig_;
    mutable std::mutex mutex_;
};

// Advanced plasticity rule engine
class PlasticityRuleEngine {
public:
    PlasticityRuleEngine();
    ~PlasticityRuleEngine();
    
    // Rule management
    bool registerRule(std::unique_ptr<IPlasticityRule> rule);
    bool unregisterRule(const std::string& name);
    std::shared_ptr<IPlasticityRule> getRule(const std::string& name) const;
    std::vector<std::shared_ptr<IPlasticityRule>> getRules() const;
    std::vector<std::shared_ptr<IPlasticityRule>> getRulesByType(PlasticityRuleType type) const;
    
    // Rule lifecycle
    bool activateRule(const std::string& name);
    bool deactivateRule(const std::string& name);
    bool configureRule(const std::string& name, const std::unordered_map<std::string, std::any>& config);
    
    // Event processing
    void onEvent(const PlasticityEvent& event);
    void processEvents(const std::vector<PlasticityEvent>& events);
    
    // Rule execution
    float computeTotalPlasticityChange(const std::unordered_map<std::string, std::any>& context);
    float computeRuleSpecificChange(const std::string& ruleName, const std::unordered_map<std::string, std::any>& context);
    
    // Configuration management
    void setGlobalLearningRate(float rate);
    float getGlobalLearningRate() const;
    void setGlobalConfig(const std::unordered_map<std::string, std::any>& config);
    std::unordered_map<std::string, std::any> getGlobalConfig() const;
    
    // Statistics and monitoring
    std::unordered_map<std::string, std::any> getEngineStatistics() const;
    std::unordered_map<std::string, std::any> getRuleStatistics(const std::string& ruleName) const;
    
    // Rule discovery and creation
    bool createCustomRule(const std::string& name, PlasticityRuleType type,
                          const std::string& description,
                          const std::vector<PlasticityCondition>& conditions,
                          const std::vector<PlasticityEffect>& effects,
                          const std::unordered_map<std::string, std::any>& config = {});
    
    // Rule backup and restoration
    bool saveRule(const std::string& name, const std::string& filename) const;
    bool loadRule(const std::string& filename);
    
    // Rule serialization
    std::string serializeRule(const std::string& name) const;
    bool deserializeRule(const std::string& serialized, const std::string& name);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Prevent copying
    PlasticityRuleEngine(const PlasticityRuleEngine&) = delete;
    PlasticityRuleEngine& operator=(const PlasticityRuleEngine&) = delete;
};

// Predefined plasticity rule implementations

// STDP (Spike-Timing-Dependent Plasticity) Rule
class STDPPlasticityRule : public PlasticityRuleBase<STDPPlasticityRule> {
public:
    STDPPlasticityRule();
    
    float computeChange(const std::unordered_map<std::string, std::any>& context) override;
    float computeEligibilityTrace(const PlasticityEvent& event) override;
    void updateTraces(float timestep) override;
    
    std::vector<PlasticityCondition> getConditions() const override;
    std::vector<PlasticityEffect> getEffects() const override;
    
    std::unordered_map<std::string, std::any> getStatistics() const override;
    
private:
    struct Trace {
        float preTrace;
        float postTrace;
        double lastPreSpikeTime;
        double lastPostSpikeTime;
        float tauPre;
        float tauPost;
    };
    
    std::unordered_map<std::string, Trace> traces_;
    float maxWeightChange_;
    float stdpLambda_;
    float stdpMu_;
    float stdpAlpha_;
};

// Reward-Modulated STDP Rule
class RewardModulatedSTDP : public PlasticityRuleBase<RewardModulatedSTDP> {
public:
    RewardModulatedSTDP();
    
    float computeChange(const std::unordered_map<std::string, std::any>& context) override;
    float computeEligibilityTrace(const PlasticityEvent& event) override;
    void updateTraces(float timestep) override;
    
    std::vector<PlasticityCondition> getConditions() const override;
    std::vector<PlasticityEffect> getEffects() const override;
    
    std::unordered_map<std::string, std::any> getStatistics() const override;
    
private:
    float predictionErrorThreshold_;
    float rewardUpdateRate_;
    float traceDecayRate_;
    float dopamineInfluence_;
};

// Adaptive Plasticity Rule
class AdaptivePlasticityRule : public PlasticityRuleBase<AdaptivePlasticityRule> {
public:
    AdaptivePlasticityRule();
    
    float computeChange(const std::unordered_map<std::string, std::any>& context) override;
    float computeEligibilityTrace(const PlasticityEvent& event) override;
    void updateTraces(float timestep) override;
    
    std::vector<PlasticityCondition> getConditions() const override;
    std::vector<PlasticityEffect> getEffects() const override;
    
    std::unordered_map<std::string, std::any> getStatistics() const override;
    
private:
    float adaptationRate_;
    float homeostaticTarget_;
    float stabilityMargin_;
    float plasticityCeiling_;
    float activityLevel_;
};

// Synaptic Tagging and Capture (STC) Rule
class SynapticTaggingRule : public PlasticityRuleBase<SynapticTaggingRule> {
public:
    SynapticTaggingRule();
    
    float computeChange(const std::unordered_map<std::string, std::any>& context) override;
    float computeEligibilityTrace(const PlasticityEvent& event) override;
    void updateTraces(float timestep) override;
    
    std::vector<PlasticityCondition> getConditions() const override;
    std::vector<PlasticityEffect> getEffects() const override;
    
    std::unordered_map<std::string, std::any> getStatistics() const override;
    
private:
    float tagDecayRate_;
    float captureThreshold_;
    float captureDuration_;
    std::unordered_map<std::string, float> synapticTags_;
    std::unordered_map<std::string, double> tagTimestamps_;
};

// Homeostatic Plasticity Rule
class HomeostaticPlasticityRule : public PlasticityRuleBase<HomeostaticPlasticityRule> {
public:
    HomeostaticPlasticityRule();
    
    float computeChange(const std::unordered_map<std::string, std::any>& context) override;
    float computeEligibilityTrace(const PlasticityEvent& event) override;
    void updateTraces(float timestep) override;
    
    std::vector<PlasticityCondition> getConditions() const override;
    std::vector<PlasticityEffect> getEffects() const override;
    
    std::unordered_map<std::string, std::any> getStatistics() const override;
    
private:
    float targetFiringRate_;
    float homeostaticCoefficient_;
    float gainControl_;
    float activityHistory_[100];  // Rolling window
    size_t historyIndex_;
    size_t historySize_;
};

// Custom User-Defined Plasticity Rule
class CustomPlasticityRule : public PlasticityRuleBase<CustomPlasticityRule> {
public:
    CustomPlasticityRule(const std::string& name = "CustomRule");
    
    void setCondition(const PlasticityCondition& condition);
    void addCondition(const PlasticityCondition& condition);
    void setEffect(const PlasticityEffect& effect);
    void addEffect(const PlasticityEffect& effect);
    
    float computeChange(const std::unordered_map<std::string, std::any>& context) override;
    float computeEligibilityTrace(const PlasticityEvent& event) override;
    void updateTraces(float timestep) override;
    
    std::vector<PlasticityCondition> getConditions() const override;
    std::vector<PlasticityEffect> getEffects() const override;
    
    std::unordered_map<std::string, std::any> getConfiguration() const override;
    void updateConfiguration(const std::unordered_map<std::string, std::any>& config) override;
    
    std::unordered_map<std::string, std::any> getStatistics() const override;
    
private:
    std::vector<PlasticityCondition> customConditions_;
    std::vector<PlasticityEffect> customEffects_;
    std::function<float(const std::unordered_map<std::string, std::any>&)> customComputeFunc_;
    std::string customFunctionSource_;
};

// Plasticity rule registry
class PlasticityRuleRegistry {
public:
    static PlasticityRuleRegistry& getInstance();
    
    // Rule registration
    bool registerRule(const std::string& name, std::unique_ptr<IPlasticityRule> rule);
    bool unregisterRule(const std::string& name);
    
    // Rule queries
    std::shared_ptr<IPlasticityRule> getRule(const std::string& name) const;
    std::vector<std::string> getRuleNames() const;
    std::vector<std::string> getRuleNamesByType(PlasticityRuleType type) const;
    
    // Rule discovery
    std::vector<std::string> discoverAvailableRules() const;
    bool loadRuleFromFile(const std::string& filename);
    
    // Registry management
    void clear();
    size_t getRuleCount() const;
    
private:
    PlasticityRuleRegistry() = default;
    ~PlasticityRuleRegistry() = default;
    
    // Prevent copying
    PlasticityRuleRegistry(const PlasticityRuleRegistry&) = delete;
    PlasticityRuleRegistry& operator=(const PlasticityRuleRegistry&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plasticity rule utilities
namespace utils {
    // Rule validation
    bool validatePlasticityCondition(const PlasticityCondition& condition);
    bool validatePlasticityEffect(const PlasticityEffect& effect);
    bool validatePlasticityRule(const IPlasticityRule& rule);
    
    // Rule serialization
    std::string serializeCondition(const PlasticityCondition& condition);
    PlasticityCondition deserializeCondition(const std::string& serialized);
    
    std::string serializeEffect(const PlasticityEffect& effect);
    PlasticityEffect deserializeEffect(const std::string& serialized);
    
    // Rule math utilities
    float calculateSTDPChange(float preSpikeTime, float postSpikeTime, float lambda, float mu);
    float calculateHomeostaticChange(float currentActivity, float targetActivity, float coefficient);
    
    // Spike history utilities
    void updateSpikeHistory(std::vector<float>& history, float spikeTime, size_t maxSize = 100);
    float calculateAverageSpikeInterval(const std::vector<float>& spikeTimes);
    
    // Thread-safe utilities
    template<typename Func>
    auto threadSafePlasticityOperation(Func&& func) -> decltype(func()) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        return func();
    }
}

} // namespace plasticity
} // namespace nlm

// Plasticity rule macros for easy registration
#define REGISTER_PLASTICITY_RULE(RuleClass, RuleName) \
    class RuleName##_registrar { \
    public: \
        RuleName##_registrar() { \
            nlm::plasticity::PlasticityRuleRegistry::getInstance().registerRule(#RuleName, std::make_unique<RuleClass>()); \
        } \
    }; \
    static RuleName##_registrar RuleName##_instance;

#define PLASTICITY_CONDITION(Name, Type, Parameters) \
    nlm::plasticity::PlasticityCondition {Name, Type, true, Parameters}

#define PLASTICITY_EFFECT(Target, Delta, Multiplier, Min, Max) \
    nlm::plasticity::PlasticityEffect {Target, Delta, Multiplier, Min, Max, 0.99f, true, 0.01f}