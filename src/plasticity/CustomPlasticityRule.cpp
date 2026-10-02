// CustomPlasticityRule.cpp - Implementation of custom plasticity rule registry
// Provides registration and management of custom plasticity rules

#include "plasticity/CustomPlasticityRule.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// CustomPlasticityRuleRegistry static member definition
CustomPlasticityRuleRegistry& CustomPlasticityRuleRegistry::getInstance() {
    static CustomPlasticityRuleRegistry instance;
    return instance;
}

void CustomPlasticityRuleRegistry::registerRule(const std::string& name,
                                              const std::string& description,
                                              const std::string& ruleType,
                                              CustomRuleFactory factory) {
    // Check if rule already exists
    auto it = std::find_if(rules_.begin(), rules_.end(),
        [&name](const auto& entry) { return entry.first == name; });
    
    if (it != rules_.end()) {
        NLM_LOG_WARNING(std::string("Plasticity rule already registered: ") + name);
        rules_.erase(it);
    }
    
    // Register new rule
    rules_[name] = std::make_pair(
        std::make_pair(description, ruleType),
        factory
    );
    
    enabledRules_[name] = true;
    
    NLM_LOG_INFO(std::string("Registered custom plasticity rule: ") + name + 
                std::string(" (type: ") + ruleType + std::string(")"));
}

std::unique_ptr<CustomPlasticityRule> CustomPlasticityRuleRegistry::createRule(const std::string& name) {
    auto it = rules_.find(name);
    if (it == rules_.end()) {
        NLM_LOG_ERROR(std::string("Plasticity rule not found: ") + name);
        return nullptr;
    }
    
    // Create rule instance
    auto rule = it->second.second();
    rule->enable(enabledRules_[name]);
    
    NLM_LOG_INFO(std::string("Created plasticity rule instance: ") + name);
    return rule;
}

std::vector<std::string> CustomPlasticityRuleRegistry::listAvailableRules() const {
    std::vector<std::string> ruleNames;
    for (const auto& entry : rules_) {
        ruleNames.push_back(entry.first);
    }
    return ruleNames;
}

void CustomPlasticityRuleRegistry::enableRule(const std::string& name, bool enable) {
    auto it = enabledRules_.find(name);
    if (it != enabledRules_.end()) {
        it->second = enable;
        NLM_LOG_INFO(std::string("Enabled state changed for rule ") + name + 
                    std::string(": ") + std::string(enable ? "enabled" : "disabled"));
    }
}

bool CustomPlasticityRuleRegistry::isRuleEnabled(const std::string& name) const {
    auto it = enabledRules_.find(name);
    return it != enabledRules_.end() && it->second;
}

} // namespace nlm