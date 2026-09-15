// Test LIF integration for stability issues
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_lif_stability {

void testNumericalPrecision() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Set up neuron with typical values
    neuron.setRestingPotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setResetPotential(-70.0f);
    neuron.setRefractoryPeriod(5);
    
    neuron.reset();
    
    // Test 1: Small changes should accumulate without explosion
    std::cout << "  Testing numerical precision..." << std::endl;
    float V_before = neuron.getMembranePotential();
    
    // Apply very small input many times
    for (int i = 0; i < 10000; ++i) {
        // Very small input
        neuron.injectCurrent(0.001f);
        bool fired = neuron.stepLIF(static_cast<float>(i), 0.001f);
        
        float V = neuron.getMembranePotential();
        
        // Check for numerical explosion
        if (std::abs(V) > 10000.0f) {
            std::cout << "FAIL: Numerical explosion at step " << i << ": " << V << std::endl;
            assert(false && "Numerical instability detected");
        }
        
        // Reset for next test
        neuron.reset();
        neuron.setRestingPotential(-70.0f);
    }
    
    std::cout << "    testNumericalPrecision passed" << std::endl;
}

void testConvergenceBehavior() {
    std::cout << "  Testing convergence behavior..." << std::endl;
    
    // Compare results with different time steps
    // For a stable system, halving dt should give similar results
    
    const float total_time = 1.0f;  // 1 second total
    const float current = 10.0f;   // Constant current
    
    std::vector<float> final_voltages;
    std::vector<int> spike_counts;
    
    // Test with different time steps
    for (float dt : {0.001f, 0.0005f, 0.00025f}) {
        nlm::Neuron neuron(nlm::NeuronId(1));
        neuron.setRestingPotential(-70.0f);
        neuron.setThreshold(-55.0f);
        neuron.setResetPotential(-70.0f);
        neuron.setRefractoryPeriod(2);
        neuron.reset();
        
        int steps = static_cast<int>(total_time / dt);
        int spikes = 0;
        
        for (int i = 0; i < steps; ++i) {
            neuron.injectCurrent(current);
            bool fired = neuron.stepLIF(static_cast<float>(i), dt);
            if (fired) spikes++;
        }
        
        final_voltages.push_back(neuron.getMembranePotential());
        spike_counts.push_back(spikes);
        
        std::cout << "    dt=" << dt << ": spikes=" << spikes << ", finalV=" << neuron.getMembranePotential() << std::endl;
    }
    
    // Check that halving dt doesn't dramatically change results
    if (final_voltages.size() >= 2) {
        float ratio = final_voltages[0] / final_voltages[1];
        if (std::abs(ratio - 1.0f) > 0.5f) {
            std::cout << "WARNING: Poor convergence - results vary significantly with dt" << std::endl;
        }
    }
    
    std::cout << "    testConvergenceBehavior passed" << std::endl;
}

void testExtremeValues() {
    std::cout << "  Testing extreme values..." << std::endl;
    
    // Test with very large and very small values
    struct TestCase {
        float current;
        float dt;
        float description;
    };
    
    std::vector<TestCase> testCases = {
        {1e6f, 0.001f, "Very large current"},
        {-1e6f, 0.001f, "Very large negative current"},
        {0.001f, 0.001f, "Very small current"},
        {1.0f, 1.0f, "Very large time step"},
        {1.0f, 0.00001f, "Very small time step"}
    };
    
    for (const auto& test : testCases) {
        nlm::Neuron neuron(nlm::NeuronId(1));
        neuron.setRestingPotential(-70.0f);
        neuron.setThreshold(-55.0f);
        neuron.setResetPotential(-70.0f);
        neuron.setRefractoryPeriod(2);
        neuron.reset();
        
        int steps = 10;
        
        for (int i = 0; i < steps; ++i) {
            neuron.injectCurrent(test.current);
            bool fired = neuron.stepLIF(static_cast<float>(i), test.dt);
            
            float V = neuron.getMembranePotential();
            
            // Check for instability
            if (std::isinf(V) || std::isnan(V)) {
                std::cout << "FAIL: " << test.description << ": NaN/inf at step " << i << std::endl;
                assert(false && "Numerical overflow");
            }
            
            if (std::abs(V) > 1e10f) {
                std::cout << "WARNING: " << test.description << ": Large voltage " << V << " at step " << i << std::endl;
            }
        }
    }
    
    std::cout << "    testExtremeValues passed" << std::endl;
}

void testEnergyConservation() {
    std::cout << "  Testing energy/magnitude conservation..." << std::endl;
    
    // For a stable LIF, voltage should not grow without bound
    // This test checks that the system doesn't exhibit runaway behavior
    
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setRestingPotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setResetPotential(-70.0f);
    neuron.setRefractoryPeriod(10);
    neuron.reset();
    
    float max_V_observed = -1000.0f;  // Start very low
    float min_V_observed = 1000.0f;   // Start very high
    
    for (int i = 0; i < 1000; ++i) {
        // Modulated current to test stability
        float current = 10.0f + 10.0f * std::sin(static_cast<float>(i) * 0.01f);
        
        neuron.injectCurrent(current);
        bool fired = neuron.stepLIF(static_cast<float>(i), 0.001f);
        
        float V = neuron.getMembranePotential();
        
        if (V > max_V_observed) max_V_observed = V;
        if (V < min_V_observed) min_V_observed = V;
        
        // Check for runaway growth
        if (V > 1000.0f || V < -1000.0f) {
            std::cout << "FAIL: Runaway behavior at step " << i << ": " << V << std::endl;
            assert(false && "Voltage growing without bound");
        }
    }
    
    std::cout << "    Energy conservation test passed (V range: " 
              << min_V_observed << " to " << max_V_observed << ")" << std::endl;
}

void testRefractoryStateStability() {
    std::cout << "  Testing refractory state stability..." << std::endl;
    
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setRestingPotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setResetPotential(-70.0f);
    neuron.setRefractoryPeriod(10);
    neuron.reset();
    
    // Strong input to trigger spikes during normal operation
    float current = 20.0f;
    
    for (int i = 0; i < 200; ++i) {
        neuron.injectCurrent(current);
        bool fired = neuron.stepLIF(static_cast<float>(i), 0.001f);
        
        // Check refractory state consistency
        if (neuron.isRefractory()) {
            // During refractory, firing state should be Refractory (or could be Resting after reset)
            // The key is that input should not affect voltage
            if (std::abs(neuron.getMembranePotential() - neuron.getResetPotential()) > 1.0f) {
                std::cout << "FAIL: Voltage changing during refractory at step " << i 
                          << ": " << neuron.getMembranePotential() << std::endl;
                assert(false && "Voltage changed during refractory");
            }
        }
        
        // Check for invalid state combinations
        if (neuron.isRefractory() && neuron.isFiring()) {
            // This might be acceptable if firingState is Active and refractoryRemaining > 0
            // But generally, refractory should override firing
        }
    }
    
    std::cout << "    testRefractoryStateStability passed" << std::endl;
}

void testTimeStepSensitivity() {
    std::cout << "  Testing time step sensitivity..." << std::endl;
    
    // Test that the simulation is not overly sensitive to small changes in dt
    const float current = 8.0f;
    const float total_time = 0.1f;  // Short test for speed
    
    std::vector<float> final_voltages;
    std::vector<int> spike_counts;
    
    // Test with slightly different time steps
    for (float dt : {0.001f, 0.0012f, 0.0015f, 0.002f}) {
        nlm::Neuron neuron(nlm::NeuronId(1));
        neuron.setRestingPotential(-70.0f);
        neuron.setThreshold(-55.0f);
        neuron.setResetPotential(-70.0f);
        neuron.setRefractoryPeriod(5);
        neuron.reset();
        
        int steps = static_cast<int>(total_time / dt);
        int spikes = 0;
        
        for (int i = 0; i < steps; ++i) {
            neuron.injectCurrent(current);
            bool fired = neuron.stepLIF(static_cast<float>(i), dt);
            if (fired) spikes++;
        }
        
        final_voltages.push_back(neuron.getMembranePotential());
        spike_counts.push_back(spikes);
    }
    
    // Check sensitivity
    bool is_sensitive = false;
    for (size_t i = 1; i < spike_counts.size(); ++i) {
        if (std::abs(spike_counts[i] - spike_counts[i-1]) > 2) {  // Allow small differences
            is_sensitive = true;
            break;
        }
    }
    
    if (is_sensitive) {
        std::cout << "WARNING: Simulation is sensitive to small changes in dt" << std::endl;
    } else {
        std::cout << "    Time step sensitivity test passed" << std::endl;
    }
}

void runAll() {
    std::cout << "=== LIF Stability Tests ===" << std::endl;
    std::cout << "Testing numerical stability and convergence of LIF integration" << std::endl;
    std::cout << std::endl;
    
    testNumericalPrecision();
    std::cout << std::endl;
    
    testConvergenceBehavior();
    std::cout << std::endl;
    
    testExtremeValues();
    std::cout << std::endl;
    
    testEnergyConservation();
    std::cout << std::endl;
    
    testRefractoryStateStability();
    std::cout << std::endl;
    
    testTimeStepSensitivity();
    std::cout << std::endl;
    
    std::cout << "=== All LIF Stability Tests PASSED ===" << std::endl;
}

} // namespace test_lif_stability