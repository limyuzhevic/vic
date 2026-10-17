#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for plasticity rules
// Enhanced with plugin architecture and dependency injection support

#include <memory>
#include <functional>
#include <string>
#include <unordered_map>

namespace nlm {

// Plugin information for dynamic loading
struct PlasticityRuleInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::vector<std::string> dependencies;
    bool (*createFunc)();  // Factory function for plugin creation
    
    PlasticityRuleInfo() : createFunc(nullptr) {}
};

// Abstract base class for plasticity rules with enhanced features
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply weight change
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule name
    virtual const char* getName() const = 0;
    
    // Get rule description
    virtual const char* getDescription() const { return "Base plasticity rule"; }
    
    // Get rule version
    virtual const char* getVersion() const { return "1.0.0"; }
    
    // Check if rule is enabled
    bool isEnabled() const { return enabled_; }
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
    // Get rule parameters (for configuration)
    virtual std::unordered_map<std::string, std::string> getParameters() const { return {}; }
    
    // Set rule parameters (for configuration)
    virtual void setParameters(const std::unordered_map<std::string, std::string>& params) {}
    
    // Check if rule has parameter
    virtual bool hasParameter(const std::string& name) const { return false; }
    
    // Get parameter type/name
    virtual const char* getParameterName(size_t index) const { return nullptr; }
    
    // Clone rule for duplication
    virtual std::unique_ptr<PlasticityRule> clone() const = 0;
    
    // Get plugin info
    virtual const PlasticityRuleInfo& getPluginInfo() const { static PlasticityRuleInfo info; return info; }
    
    // Validate rule configuration
    virtual bool validate() const { return true; }
    
    // Initialize rule with brain context
    virtual void initialize(Brain* brain) {}
    
    // Cleanup rule
    virtual void shutdown() {}
    
protected:
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_;
};

// Plugin factory for dynamic loading
class PlasticityRuleFactory {
public:
    using CreateFunc = std::function<std::unique_ptr<PlasticityRule>()>;
    
    static PlasticityRuleFactory& getInstance() {
        static PlasticityRuleFactory instance;
        return instance;
    }
    
    // Register a plasticity rule factory
    void registerRule(const std::string& name, const CreateFunc& factory, 
                     const PlasticityRuleInfo& info = PlasticityRuleInfo()) {
        factories_[name] = factory;
        pluginInfo_[name] = info;
    }
    
    // Create a plasticity rule by name
    std::unique_ptr<PlasticityRule> createRule(const std::string& name) {
        auto it = factories_.find(name);
        if (it != factories_.end()) {
            return it->second();
        }
        return nullptr;
    }
    
    // Get all registered rule names
    std::vector<std::string> getRegisteredRuleNames() const {
        std::vector<std::string> names;
        for (const auto& pair : factories_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get plugin info for a rule
    const PlasticityRuleInfo& getPluginInfo(const std::string& name) const {
        auto it = pluginInfo_.find(name);
        static PlasticityRuleInfo emptyInfo;
        return (it != pluginInfo_.end()) ? it->second : emptyInfo;
    }
    
    // Check if rule is registered
    bool isRuleRegistered(const std::string& name) const {
        return factories_.find(name) != factories_.end();
    }
    
private:
    PlasticityRuleFactory() = default;
    std::unordered_map<std::string, CreateFunc> factories_;
    std::unordered_map<std::string, PlasticityRuleInfo> pluginInfo_;
};

// Macro for registering plasticity rules
#define REGISTER_PLASTICITY_RULE(name, ruleClass) \
    namespace { \
    bool _register_##name##_func() { \
        PlasticityRuleFactory::getInstance().registerRule( \
            #name, \
            []() -> std::unique_ptr<PlasticityRule> { \
                return std::make_unique<ruleClass>(); \
            } \
        ); \
        return true; \
    } \
    bool _dummy_##name##_registration = _register_##name##_func(); \
    } 

// Hebbian plasticity rule: "neurons that fire together, wire together"
// Enhanced with better parameter handling and validation
class HebbianRule : public PlasticityRule {
public:
    HebbianRule();
    ~HebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    const char* getDescription() const override;
    const char* getVersion() const override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    void setBaseline(float baseline);
    float getBaseline() const;
    
    void setUseVariance(bool useVariance);
    bool getUseVariance() const;
    
    // Enhanced parameter handling
    std::unordered_map<std::string, std::string> getParameters() const override;
    void setParameters(const std::unordered_map<std::string, std::string>& params) override;
    bool hasParameter(const std::string& name) const override;
    const char* getParameterName(size_t index) const override;
    
    // Validation
    bool validate() const override;
    
    // Cloning
    std::unique_ptr<PlasticityRule> clone() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
class AntiHebbianRule : public PlasticityRule {
public:
    AntiHebbianRule();
    ~AntiHebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    const char* getDescription() const override;
    const char* getVersion() const override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    std::unordered_map<std::string, std::string> getParameters() const override;
    void setParameters(const std::unordered_map<std::string, std::string>& params) override;
    bool hasParameter(const std::string& name) const override;
    const char* getParameterName(size_t index) const override;
    std::unique_ptr<PlasticityRule> clone() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Bienenstock-Cooper-Munro (BCM) rule
class BCMRule : public PlasticityRule {
public:
    BCMRule();
    ~BCMRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    const char* getDescription() const override;
    const char* getVersion() const override;
    
    // Parameters
    void setTheta(float theta);  // BCM threshold
    float getTheta() const;
    void setTau(float tau);      // Adaptation time constant
    float getTau() const;
    
    std::unordered_map<std::string, std::string> getParameters() const override;
    void setParameters(const std::unordered_map<std::string, std::string>& params) override;
    bool hasParameter(const std::string& name) const override;
    const char* getParameterName(size_t index) const override;
    std::unique_ptr<PlasticityRule> clone() const override;
    bool validate() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    const char* getDescription() const override;
    const char* getVersion() const override;
    
    // Parameters
    void setBaseSTDPWeight(float weight);
    float getBaseSTDPWeight() const;
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    std::unordered_map<std::string, std::string> getParameters() const override;
    void setParameters(const std::unordered_map<std::string, std::string>& params) override;
    bool hasParameter(const std::string& name) const override;
    const char* getParameterName(size_t index) const override;
    std::unique_ptr<PlasticityRule> clone() const override;
    void initialize(Brain* brain) override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

// Hebbian plasticity rule: "neurons that fire together, wire together"
// PLACEHOLDER - Phase 2 will implement real Hebbian learning
class HebbianRule : public PlasticityRule {
public:
    HebbianRule();
    ~HebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
// PLACEHOLDER - Phase 2
class AntiHebbianRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "AntiHebbian"; }
};

// Bienenstock-Cooper-Munro (BCM) rule
// PLACEHOLDER - Phase 2
class BCMRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "BCM"; }
};

} // namespace nlm
