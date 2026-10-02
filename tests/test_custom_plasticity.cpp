// test_custom_plasticity.cpp - Tests for CustomPlasticityRule
// Tests custom plasticity rule functionality and registry

#include <iostream>
#include <cassert>
#include <memory>
#include "plasticity/CustomPlasticityRule.hpp"
#include "brain/Synapse.hpp"

namespace test_custom_plasticity {

// Example custom plasticity rule for testing
class ExamplePlasticityRule : public nlm::CustomPlasticityRule {
public:
    std::string getName() const override {
        return "ExamplePlasticityRule";
    }
    
    std::string getDescription() const override {
        return "Example custom plasticity rule for testing";
    }
    
    void update(std::shared_ptr<nlm::Synapse> synapse,
               const std::vector<Timestamp>& preSpikeHistory,
               const std::vector<Timestamp>& postSpikeHistory,
               TimestepDuration dt,
               const nlm::Neuromodulator& neuromodulation) override {
        // Simple example: strengthen if pre-post > 0, weaken if post-pre < 0
        if (preSpikeHistory.empty() || postSpikeHistory.empty()) {
            return;
        }
        
        for (Timestamp preTime : preSpikeHistory) {
            for (Timestamp postTime : postSpikeHistory) {
                float dt = static_cast<float>(postTime - preTime);
                if (dt > 10.0f) {  // Potentiation
                    synapse->addToWeight(0.01f * neuromodulation.getLevel());
                } else if (dt < -10.0f) {  // Depression
                    synapse->addToWeight(-0.01f * neuromodulation.getLevel());
                }
            }
        }
    }
    
    std::string getRuleType() const override {
        return "Example";
    }
};

void testCustomPlasticityRuleBasic() {
    std::cout << "Testing CustomPlasticityRule basic functionality..." << std::endl;
    
    // Create example rule
    auto rule = std::make_unique<ExamplePlasticityRule>();
    
    // Test basic properties
    assert(rule->getName() == "ExamplePlasticityRule");
    assert(rule->getDescription() == "Example custom plasticity rule for testing");
    assert(rule->isEnabled());
    assert(rule->getRuleType() == "Example");
    
    // Test parameter management
    std::map<std::string, double> params;
    params["learning_rate"] = 0.1;
    rule->configure(params);
    
    assert(rule->getParameters().find("learning_rate") != rule->getParameters().end());
    
    // Test enable/disable
    rule->enable(false);
    assert(!rule->isEnabled());
    
    std::cout << "  ✓ CustomPlasticityRule basic functionality test passed" << std::endl;
}

void testCustomPlasticityRuleRegistry() {
    std::cout << "Testing CustomPlasticityRule registry..." << std::endl;
    
    // Access registry
    auto& registry = nlm::CustomPlasticityRuleRegistry::getInstance();
    
    // Register example rule
    auto exampleFactory = []() -> std::unique_ptr<nlm::CustomPlasticityRule> {
        return std::make_unique<ExamplePlasticityRule>();
    };
    
    registry.registerRule("Example", 
                         "Example custom plasticity rule",
                         "Example",
                         exampleFactory);
    
    // List available rules
    auto availableRules = registry.listAvailableRules();
    assert(!availableRules.empty());
    assert(std::find(availableRules.begin(), availableRules.end(), "Example") != availableRules.end());
    
    // Create rule from registry
    auto createdRule = registry.createRule("Example");
    assert(createdRule != nullptr);
    assert(createdRule->getName() == "ExamplePlasticityRule");
    
    // Test rule enabling
    registry.enableRule("Example", true);
    assert(registry.isRuleEnabled("Example"));
    
    std::cout << "  ✓ CustomPlasticityRule registry test passed" << std::endl;
}

void testCustomPlasticityRuleConfiguration() {
    std::cout << "Testing CustomPlasticityRule configuration..." << std::endl;
    
    auto rule = std::make_unique<ExamplePlasticityRule>();
    
    // Test configuration
    std::map<std::string, double> config;
    config["weight"] = 2.5;
    config["threshold"] = 0.5;
    config["learning_rate"] = 0.01;
    
    rule->configure(config);
    
    assert(rule->getParameters().size() == 3);
    assert(rule->getParameters().at("weight") == 2.5);
    assert(rule->getParameters().at("threshold") == 0.5);
    assert(rule->getParameters().at("learning_rate") == 0.01);
    
    std::cout << "  ✓ CustomPlasticityRule configuration test passed" << std::endl;
}

void runAll() {
    testCustomPlasticityRuleBasic();
    testCustomPlasticityRuleRegistry();
    testCustomPlasticityRuleConfiguration();
    
    std::cout << std::endl;
    std::cout << "=== All CustomPlasticityRule Tests Passed ===" << std::endl;
}

} // namespace test_custom_plasticity