#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include <memory>
#include <string>
#include <optional>
#include <variant>
#include <type_traits>

namespace nlm {

// Modern error type for plasticity operations
enum class PlasticityError {
    InvalidParameters,
    WeightBoundsViolation,
    PlasticityDisabled,
    InsufficientData,
    InvalidSynapse,
    UpdateFailed
};

// Configuration for plasticity rules
struct PlasticityConfig {
    std::string name;
    float learningRate = 1.0f;
    float maxWeight = 1.0f;
    float minWeight = -1.0f;
    float timeConstant = 10.0f;
    bool enabled = true;
    std::unordered_map<std::string, std::string> parameters;
    
    PlasticityConfig() = default;
    explicit PlasticityConfig(const std::string& ruleName) : name(ruleName) {}
};

// Abstract base class for plasticity rules using Strategy pattern
class PlasticityRule {
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Rule of five for proper resource management
    PlasticityRule(const PlasticityRule&) = delete;
    PlasticityRule& operator=(const PlasticityRule&) = delete;
    PlasticityRule(PlasticityRule&&) noexcept = default;
    PlasticityRule& operator=(PlasticityRule&&) noexcept = default;
    
protected:
    PlasticityConfig config;
    
    explicit PlasticityRule(const PlasticityConfig& ruleConfig) : config(ruleConfig) {}
    
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    // Returns error status for robust error handling
    [[nodiscard]] virtual std::optional<PlasticityError> update(
        Synapse& synapse, 
        const std::vector<Timestamp>& preSpikes,
        const std::vector<Timestamp>& postSpikes,
        TimestepDuration dt) = 0;
    
    // Apply weight change with bounds checking
    [[nodiscard]] virtual std::optional<PlasticityError> applyWeightChange(
        Synapse& synapse, SynapticWeight delta) = 0;
    
    // Get rule information
    [[nodiscard]] virtual const std::string& getName() const noexcept { return config.name; }
    [[nodiscard]] virtual const PlasticityConfig& getConfig() const noexcept { return config; }
    
    // Configuration management
    virtual void setConfig(const PlasticityConfig& newConfig) {
        config = newConfig;
    }
    
    // Enable/disable rule
    [[nodiscard]] bool isEnabled() const noexcept { return config.enabled; }
    virtual void setEnabled(bool enabled) noexcept { config.enabled = enabled; }
    
    // Parameter management with type safety
    template<typename T>
    void setParameter(const std::string& key, T value) {
        config.parameters[key] = std::to_string(value);
    }
    
    template<typename T>
    std::expected<T, PlasticityError> getParameter(const std::string& key) const {
        auto it = config.parameters.find(key);
        if (it == config.parameters.end()) {
            return std::unexpected(PlasticityError::InvalidParameters);
        }
        try {
            // Simple conversion - in real implementation would use proper type handling
            return T(std::stoi(it->second));
        } catch (const std::exception&) {
            return std::unexpected(PlasticityError::InvalidParameters);
        }
    }
    
    // Factory method for creating rule instances
    static std::expected<std::unique_ptr<PlasticityRule>, PlasticityError> createRule(
        const std::string& ruleType, const PlasticityConfig& config);
    
    // Builder pattern for rule configuration
    class Builder {
        PlasticityConfig config;
        
    public:
        explicit Builder(const std::string& name) : config(name) {}
        
        Builder& withLearningRate(float rate) { 
            config.learningRate = rate; 
            return *this; 
        }
        
        Builder& withMaxWeight(float max) { 
            config.maxWeight = max; 
            return *this; 
        }
        
        Builder& withMinWeight(float min) { 
            config.minWeight = min; 
            return *this; 
        }
        
        Builder& withTimeConstant(float tau) { 
            config.timeConstant = tau; 
            return *this; 
        }
        
        Builder& disabled() { 
            config.enabled = false; 
            return *this; 
        }
        
        std::expected<std::unique_ptr<PlasticityRule>, PlasticityError> build() {
            return PlasticityRule::createRule(config.name, config);
        }
    };
    
    [[nodiscard]] static Builder createBuilder(const std::string& name) {
        return Builder(name);
    }
};

// Modern Hebbian plasticity rule: "neurons that fire together, wire together"
class HebbianRule : public PlasticityRule {
public:
    explicit HebbianRule(const PlasticityConfig& config = PlasticityConfig("Hebbian"));
    ~HebbianRule() override;
    
    [[nodiscard]] std::optional<PlasticityError> update(
        Synapse& synapse,
        const std::vector<Timestamp>& preSpikes,
        const std::vector<Timestamp>& postSpikes,
        TimestepDuration dt) override;
    
    [[nodiscard]] std::optional<PlasticityError> applyWeightChange(
        Synapse& synapse, SynapticWeight delta) override;
    
    [[nodiscard]] const char* getName() const override { return "Hebbian"; }
    
    // Hebbian-specific parameters
    void setLearningRate(float rate) noexcept { config.learningRate = rate; }
    [[nodiscard]] float getLearningRate() const noexcept { return config.learningRate; }
    
    void setMinCorrelation(float minCorr) noexcept { config.parameters["minCorrelation"] = std::to_string(minCorr); }
    [[nodiscard]] float getMinCorrelation() const {
        return getParameter<float>("minCorrelation").value_or(0.0f);
    }
};

// Anti-Hebbian rule: decrease weight when neurons fire together
class AntiHebbianRule : public PlasticityRule {
public:
    explicit AntiHebbianRule(const PlasticityConfig& config = PlasticityConfig("AntiHebbian"));
    
    [[nodiscard]] std::optional<PlasticityError> update(
        Synapse& synapse,
        const std::vector<Timestamp>& preSpikes,
        const std::vector<Timestamp>& postSpikes,
        TimestepDuration dt) override;
    
    [[nodiscard]] std::optional<PlasticityError> applyWeightChange(
        Synapse& synapse, SynapticWeight delta) override;
    
    [[nodiscard]] const char* getName() const override { return "AntiHebbian"; }
};

// Bienenstock-Cooper-Munro (BCM) rule
class BCMRule : public PlasticityRule {
public:
    explicit BCMRule(const PlasticityConfig& config = PlasticityConfig("BCM"));
    
    [[nodiscard]] std::optional<PlasticityError> update(
        Synapse& synapse,
        const std::vector<Timestamp>& preSpikes,
        const std::vector<Timestamp>& postSpikes,
        TimestepDuration dt) override;
    
    [[nodiscard]] std::optional<PlasticityError> applyWeightChange(
        Synapse& synapse, SynapticWeight delta) override;
    
    [[nodiscard]] const char* getName() const override { return "BCM"; }
    
    // BCM-specific parameters
    void setTheta(float theta) noexcept { config.parameters["theta"] = std::to_string(theta); }
    [[nodiscard]] float getTheta() const {
        return getParameter<float>("theta").value_or(25.0f);
    }
    
    void setPhi(float phi) noexcept { config.parameters["phi"] = std::to_string(phi); }
    [[nodiscard]] float getPhi() const {
        return getParameter<float>("phi").value_or(1.0f);
    }
};

// Spike-Timing-Dependent Plasticity (STDP) rule
class STDPRule : public PlasticityRule {
public:
    explicit STDPRule(const PlasticityConfig& config = PlasticityConfig("STDP"));
    
    [[nodiscard]] std::optional<PlasticityError> update(
        Synapse& synapse,
        const std::vector<Timestamp>& preSpikes,
        const std::vector<Timestamp>& postSpikes,
        TimestepDuration dt) override;
    
    [[nodiscard]] std::optional<PlasticityError> applyWeightChange(
        Synapse& synapse, SynapticWeight delta) override;
    
    [[nodiscard]] const char* getName() const override { return "STDP"; }
    
    // STDP-specific parameters
    void setLTPWeight(float weight) noexcept { config.parameters["ltpWeight"] = std::to_string(weight); }
    [[nodiscard]] float getLTPWeight() const {
        return getParameter<float>("ltpWeight").value_or(1.0f);
    }
    
    void setLTDWeight(float weight) noexcept { config.parameters["ltdWeight"] = std::to_string(weight); }
    [[nodiscard]] float getLTDWeight() const {
        return getParameter<float>("ltdWeight").value_or(1.0f);
    }
    
    void setTimeConstant(float tau) noexcept { config.timeConstant = tau; }
    [[nodiscard]] float getTimeConstant() const noexcept { return config.timeConstant; }
};

// Factory for creating plasticity rules
class PlasticityRuleFactory {
public:
    static std::expected<std::unique_ptr<PlasticityRule>, PlasticityError> createRule(
        const std::string& ruleType, const PlasticityConfig& config);
    
    static std::vector<std::string> getAvailableRuleTypes() {
        return {"Hebbian", "AntiHebbian", "BCM", "STDP"};
    }
};

} // namespace nlm