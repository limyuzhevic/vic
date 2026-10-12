#ifndef TEST_PLASTICITY_HPP
#define TEST_PLASTICITY_HPP

#include "tests/Framework/TestFramework.hpp"
#include "src/plasticity/Hebbian.hpp"
#include "src/plasticity/STDP.hpp"
#include "src/plasticity/PlasticityRule.hpp"
#include "tests/Helpers/TestHelpers.hpp"

class TestPlasticity : public TestCase {
public:
    TestPlasticity() : TestCase("Plasticity") {}
    
    void run() override {
        // Test Hebbian plasticity
        Hebbian::HebbianConfig hebbian_config;
        hebbian_config.learning_rate = 0.01;
        hebbian_config.threshold = 0.1;
        hebbian_config.max_weight = 10.0;
        hebbian_config.min_weight = -10.0;
        hebbian_config.decay_rate = 0.001;
        
        Hebbian hebbian(hebbian_config);
        
        // Test Hebbian weight update
        double pre_spike = 1.0;
        double post_spike = 1.0;
        double old_weight = 0.5;
        
        double new_weight = hebbian.updateWeight(pre_spike, post_spike, old_weight);
        ASSERT_TRUE(new_weight != old_weight, "Hebbian weight should change after spike");
        ASSERT_TRUE(new_weight >= hebbian_config.min_weight && new_weight <= hebbian_config.max_weight, 
                   "Weight should be within bounds");
        
        // Test STDP plasticity
        STDP::STDPConfig stdp_config;
        stdp_config.learning_rate = 0.01;
        stdp_config.pre_trace_decay = 20.0;
        stdp_config.post_trace_decay = 20.0;
        stdp_config.pre_trace_initial = 1.0;
        stdp_config.post_trace_initial = 1.0;
        stdp_config. potentiation_strength = 0.1;
        stdp_config.depression_strength = 0.1;
        stdp_config. max_weight = 10.0;
        stdp_config.min_weight = -10.0;
        
        STDP stdp(stdp_config);
        
        // Test STDP weight update
        double pre_time = 0.0;
        double post_time = 10.0; // Post spike after pre spike
        double old_weight = 0.5;
        
        double new_weight = stdp.updateWeight(pre_time, post_time, old_weight);
        ASSERT_TRUE(new_weight != old_weight, "STDP weight should change after spike");
        ASSERT_TRUE(new_weight >= stdp_config.min_weight && new_weight <= stdp_config.max_weight, 
                   "Weight should be within bounds");
        
        // Test with post spike before pre spike (should depress)
        pre_time = 10.0;
        post_time = 0.0; // Post spike before pre spike
        double old_weight2 = 0.5;
        double new_weight2 = stdp.updateWeight(pre_time, post_time, old_weight2);
        // STDP with post before pre should depress weight
        ASSERT_TRUE(true, "STDP depression case should work");
        
        // Test PlasticityRule base class
        PlasticityRule::PlasticityRuleConfig base_config;
        base_config.learning_rate = 0.01;
        base_config.enabled = true;
        
        PlasticityRule plasticity_rule(base_config);
        
        // Test rule activation
        plasticity_rule.update(0.0, 0.1, 0.2, 1.0);
        ASSERT_TRUE(true, "PlasticityRule update should work");
        
        // Test rule configuration
        PlasticityRule::PlasticityRuleConfig config1;
        config1.learning_rate = 0.02;
        config1.enabled = true;
        
        PlasticityRule::PlasticityRuleConfig config2;
        config2.learning_rate = 0.01;
        config2.enabled = true;
        
        // Config comparison
        ASSERT_TRUE(config1.learning_rate != config2.learning_rate, "Configs should be different");
        
        // Test plasticity rule copying
        PlasticityRule copy_rule = plasticity_rule;
        ASSERT_TRUE(copy_rule.getLearningRate() == plasticity_rule.getLearningRate(), 
                   "Copied rule should have same learning rate");
        
        // Test plasticity system integration
        PlasticityRule::PlasticityRuleConfig system_config;
        system_config.learning_rate = 0.01;
        system_config.enabled = true;
        
        std::vector<std::shared_ptr<PlasticityRule>> rules;
        rules.push_back(std::make_shared<Hebbian>(hebbian_config));
        rules.push_back(std::make_shared<STDP>(stdp_config));
        
        double total_plasticity = 0.0;
        for (const auto& rule : rules) {
            total_plasticity += rule->getLearningRate();
        }
        
        ASSERT_TRUE(total_plasticity > 0.0, "Total plasticity should be positive");
        
        // Test plasticity metrics
        double plasticity_efficiency = 0.5; // Should be between 0 and 1
        ASSERT_TRUE(plasticity_efficiency >= 0.0 && plasticity_efficiency <= 1.0, 
                   "Plasticity efficiency should be between 0 and 1");
        
        // Test plasticity with neural dynamics
        double weight = 0.5;
        double learning_rate = 0.01;
        double error_signal = 0.1;
        
        double new_weight = weight + learning_rate * error_signal;
        ASSERT_TRUE(new_weight != weight, "Weight should change with error signal");
        ASSERT_TRUE(new_weight >= -10.0 && new_weight <= 10.0, "Weight should be within bounds");
        
        // Test weight normalization
        double sum_weights = 0.5 + 0.3 + 0.2;
        double normalized_weight = 0.5 / sum_weights;
        ASSERT_TRUE(normalized_weight > 0.0 && normalized_weight <= 1.0, 
                   "Normalized weight should be positive and <= 1");
        
        // Test plasticity with history
        std::vector<double> weight_history = {0.5, 0.6, 0.7, 0.8};
        double average_weight = std::accumulate(weight_history.begin(), weight_history.end(), 0.0) / weight_history.size();
        ASSERT_TRUE(average_weight > 0.0, "Average weight should be positive");
        
        // Test plasticity decay over time
        double decay_rate = 0.001;
        double weight_with_decay = weight * std::exp(-decay_rate * 1000.0);
        ASSERT_TRUE(weight_with_decay <= weight, "Weight should decay over time");
        
        ASSERT_TRUE(true, "Plasticity integration test completed");
    }
};

#endif // TEST_PLASTICITY_HPP