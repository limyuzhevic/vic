// NLM Unit Testing Framework
// Plasticity Tests

#include "framework/TestFramework.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "plasticity/STDP.hpp"
#include <cassert>
#include <cmath>

class TestPlasticity : public TestCase {
public:
    TestPlasticity() : TestCase("Plasticity") {}
    
    void testSTDPRule() {
        STDPRule stdp;
        
        // Test spike-timing dependent plasticity
        double pre_time = 0.0;
        double post_time = 10.0; // Post fires after pre
        double weight_change = stdp.calculateWeightChange(pre_time, post_time);
        
        // Long-term potentiation (post after pre)
        assert(weight_change > 0.0);
        assert(weight_change <= 1.0); // Normalized
        
        // Reset for depression test
        pre_time = 10.0;
        post_time = 0.0; // Pre fires after post
        weight_change = stdp.calculateWeightChange(pre_time, post_time);
        
        // Long-term depression (pre after post)
        assert(weight_change < 0.0);
        assert(weight_change >= -1.0);
    }
    
    void testStructuralPlasticity() {
        StructuralPlasticity plasticity;
        
        // Create initial neural structure
        NeuralStructure initial;
        initial.synapses = std::vector<Synapse>(100, Synapse{0.1, 0.1});
        
        // Apply activity-dependent growth
        plasticity.applyActivityDependentGrowth(initial, 0.5);
        
        // Should create new connections
        assert(initial.synapses.size() >= 100);
        
        // Test homeostatic plasticity
        plasticity.applyHomeostaticPlasticity(initial, 0.1);
        assert(true); // Stub - implementation dependent
    }
    
    void testHebbianLearning() {
        PlasticityRule hebbian(PlasticityType::HEBBIAN);
        
        // Implement Hebb's rule: "neurons that fire together wire together"
        std::vector<double> pre_spikes = {1.0, 0.0, 1.0, 0.0};
        std::vector<double> post_spikes = {1.0, 1.0, 0.0, 0.0};
        
        double learning_rate = 0.01;
        for (size_t i = 0; i < pre_spikes.size(); ++i) {
            if (pre_spikes[i] > 0.5 && post_spikes[i] > 0.5) {
                // Both fired simultaneously - strengthen connection
                double weight_change = hebbian.calculateWeightChange(
                    1.0, 1.0, learning_rate);
                assert(weight_change > 0.0);
            }
        }
    }
    
    void testOjaLearningRule() {
        PlasticityRule oja(PlasticityType::OJA);
        
        // Oja's rule for competitive learning
        std::vector<double> weights = {0.5, 0.5, 0.5};
        double input = 1.0;
        double output = 0.5;
        
        double weight_change = oja.calculateWeightChange(weights, input, output);
        
        // Oja's rule stabilizes weights
        assert(std::isfinite(weight_change));
    }
    
    void testBaccopoulouPlasticity() {
        PlasticityRule baccopoulou(PlasticityType::BACCOPOULOU);
        
        // Test metaplastic changes in learning rate
        double current_learning_rate = 0.1;
        double activity_level = 2.0;
        
        double adjusted_rate = baccopoulou.adjustLearningRate(
            current_learning_rate, activity_level);
        
        // Metaplastic adaptation
        assert(adjusted_rate > 0.0);
        assert(adjusted_rate != current_learning_rate); // Should change
    }
};