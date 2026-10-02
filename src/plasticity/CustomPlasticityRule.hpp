// CustomPlasticityRule.hpp - Base class for custom plasticity rules
// Allows users to implement custom synaptic learning rules beyond standard STDP and Hebbian

#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

class CustomPlasticityRule {
public:
    virtual ~CustomPlasticityRule() = default;
    
    // Core plasticity rule interface
    virtual void update(std::shared_ptr<class Synapse> synapse,
                       const std::vector<Timestamp>& preSpikeHistory,
                       const std::vector<Timestamp>& postSpikeHistory,
                       TimestepDuration dt,
                       const class Neuromodulator& neuromodulation) = 0;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Configuration
    virtual void configure(const std::map<std::string, double>& parameters) {}
    virtual std::map<std::string, double> getParameters() const { return parameters_; }
    
    // Metadata
    virtual bool isEnabled() const { return enabled_; }
    virtual void enable(bool enable) { enabled_ = enable; }
    
    // Type information
    virtual std::string getRuleType() const = 0;
    
protected:
    std::map<std::string, double> parameters_;
    bool enabled_ = true;
};

// Convenience typedef for custom rule factory function
typedef std::function<std::unique_ptr<CustomPlasticityRule>()> CustomRuleFactory;

// Registry for custom plasticity rules
class CustomPlasticityRuleRegistry {
public:
    static CustomPlasticityRuleRegistry& getInstance();
    
    void registerRule(const std::string& name,
                     const std::string& description,
                     const std::string& ruleType,
                     CustomRuleFactory factory);
    
    std::unique_ptr<CustomPlasticityRule> createRule(const std::string& name);
    
    std::vector<std::string> listAvailableRules() const;
    
    void enableRule(const std::string& name, bool enable);
    bool isRuleEnabled(const std::string& name) const;
    
private:
    CustomPlasticityRuleRegistry() = default;
    std::map<std::string, std::pair<std::string, std::pair<std::string, CustomRuleFactory>>> rules_;
    std::map<std::string, bool> enabledRules_;
};

} // namespace nlm