#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace nlm {

// Abstract base class for plasticity rules
class PlasticityRule;

// Factory class for creating plasticity rules
class PlasticityFactory {
public:
    PlasticityFactory();
    ~PlasticityFactory();
    
    // Disable copying
    PlasticityFactory(const PlasticityFactory&) = delete;
    PlasticityFactory& operator=(const PlasticityFactory&) = delete;
    
    // Register a new plasticity rule factory
    bool registerRule(const std::string& name,
                      std::function<std::unique_ptr<PlasticityRule>()> factory);
    
    // Create a plasticity rule by name
    std::unique_ptr<PlasticityRule> createRule(const std::string& name);
    
    // Check if a rule type exists
    bool hasRule(const std::string& name) const;
    
    // Get list of all available rule types
    std::vector<std::string> getAvailableRules() const;
    
    // Clear all registered rule factories
    void clear();
    
    // Get description of a rule type
    std::string getRuleDescription(const std::string& name) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm