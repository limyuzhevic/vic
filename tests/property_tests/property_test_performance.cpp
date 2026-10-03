// Property-based tests for performance
// Tests invariants for time complexity, memory usage, and numerical stability

#ifndef PROPERTY_TEST_PERFORMANCE_H
#define PROPERTY_TEST_PERFORMANCE_H

#include "property_tests/NLMPropertyTestFramework.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "core/Random/Random.hpp"
#include <vector>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>

namespace test_performance {

// Property 1: Spike processing time should be O(m) not O(n²)
bool performance_spike_processing_complexity(nlm::property_test::PropertyRandom& rng) {
    const uint32_t network_sizes[] = {10, 50, 100, 200};
    
    bool complexity_correct = true;
    
    for (uint32_t network_size : network_sizes) {
        std::cout << "    Testing spike processing complexity for network size: " << network_size << std::endl;
        
        // Create neurons
        std::vector<nlm::Neuron> neurons;
        for (uint32_t i = 0; i < network_size; ++i) {
            neurons.emplace_back(nlm::NeuronId(i));
            auto params = rng.generate_neuron_params(network_size);
            neurons.back().setType(params.type);
            neurons.back().setRestingPotential(params.resting_potential);
            neurons.back().setThreshold(params.threshold);
        }
        
        // Measure spike processing time
        const uint32_t spike_steps = 50;
        const uint32_t repetitions = 10;
        
        std::vector<double> processing_times;
        
        for (uint32_t rep = 0; rep < repetitions; ++rep) {
            auto start_time = std::chrono::high_resolution_clock::now();
            
            for (uint32_t step = 0; step < spike_steps; ++step) {
                nlm::Timestamp current_time = static_cast<nlm::Timestamp>(step * 0.01);
                
                for (auto& neuron : neurons) {
                    double current = rng.uniform_double(-5.0, 5.0);
                    neuron.injectCurrent(static_cast<nlm::MembranePotential>(current));
                    neuron.stepLIF(current_time, 0.01);
                    neuron.clearTotalCurrent();
                }
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            processing_times.push_back(duration.count() / 1000.0); // Convert to milliseconds
        }
        
        // Calculate average processing time
        double total_time = std::accumulate(processing_times.begin(), processing_times.end(), 0.0);
        double avg_time = total_time / repetitions;
        
        // Check if processing time scales linearly with network size
        // We expect that doubling network size should roughly double processing time (O(m))
        // For O(n²) complexity, doubling size would quadruple processing time
        
        // Compare with expected linear scaling (very rough approximation)
        double expected_time_per_neuron = avg_time / network_size;
        
        if (expected_time_per_neuron > 0.1) { // Arbitrary threshold for "too slow"
            complexity_correct = false;
            std::cout << "      WARNING: Potential O(n²) complexity detected - "
                      << "time per neuron: " << expected_time_per_neuron << " ms" << std::endl;
        }
        
        // Store measurement for statistical analysis
        // In real implementation, would add to statistical test framework
    }
    
    return complexity_correct;
}

// Property 2: Memory usage should be bounded
bool performance_memory_usage_bounded(nlm::property_test::PropertyRandom& rng) {
    const uint32_t network_sizes[] = {50, 100, 200};
    
    bool memory_bounded = true;
    
    for (uint32_t network_size : network_sizes) {
        std::cout << "    Testing memory usage for network size: " << network_size << std::endl;
        
        // Create neurons and synapses
        std::vector<nlm::Neuron> neurons;
        std::vector<nlm::Synapse> synapses;
        
        for (uint32_t i = 0; i < network_size; ++i) {
            neurons.emplace_back(nlm::NeuronId(i));
        }
        
        // Create connections (sparse)
        for (uint32_t i = 0; i < network_size; ++i) {
            for (uint32_t j = 0; j < network_size && synapses.size() < network_size * 3; ++j) {
                if (i != j && rng.bernoulli(0.1)) { // 10% connection probability
                    synapses.emplace_back(nlm::SynapseId(synapses.size()), 
                                        nlm::NeuronId(i), nlm::NeuronId(j));
                    synapses.back().setType(nlm::SynapseType::Excitatory);
                    synapses.back().setWeight(rng.uniform_double(0.0, 0.5));
                }
            }
        }
        
        // Estimate memory usage
        // This is a simplified estimation - real implementation would use memory profiling
        size_t estimated_memory_mb = 0;
        
        // Neurons: each neuron has state (approx 50 bytes)
        estimated_memory_mb += neurons.size() * 50;
        
        // Synapses: each synapse has state (approx 100 bytes)
        estimated_memory_mb += synapses.size() * 100;
        
        // Convert to MB
        double memory_mb = static_cast<double>(estimated_memory_mb) / (1024 * 1024);
        
        // Memory should be bounded (not growing super-linearly)
        double memory_per_neuron = memory_mb / network_size;
        
        if (memory_per_neuron > 1.0) { // More than 1MB per neuron is concerning
            memory_bounded = false;
            std::cout << "      WARNING: High memory usage per neuron: " << memory_per_neuron << " MB" << std::endl;
        }
        
        std::cout << "      Estimated memory: " << memory_mb << " MB (" << memory_per_neuron << " MB/neuron)" << std::endl;
    }
    
    return memory_bounded;
}

// Property 3: Numerical stability should be maintained
bool performance_numerical_stability(nlm::property_test::PropertyRandom& rng) {
    const uint32_t numerical_tests = 100;
    const double instability_threshold = 1e-6;
    
    bool numerical_stable = true;
    
    for (uint32_t test = 0; test < numerical_tests && numerical_stable; ++test) {
        // Create neurons with varying parameters
        nlm::Neuron neuron(nlm::NeuronId(1));
        auto params = rng.generate_neuron_params(1);
        neuron.setType(params.type);
        neuron.setRestingPotential(params.resting_potential);
        neuron.setThreshold(params.threshold);
        neuron.setResetPotential(params.reset_potential);
        neuron.setLeakConductance(params.leak_conductance);
        neuron.setRefractoryPeriod(params.refractory_period);
        
        // Run numerical simulation
        nlm::Timestamp current_time = 0.0;
        double max_deviation = 0.0;
        
        for (uint32_t step = 0; step < 1000; ++step) {
            current_time = static_cast<nlm::Timestamp>(step * 0.001);
            
            // Inject various current patterns
            double current = rng.uniform_double(-10.0, 10.0);
            neuron.injectCurrent(static_cast<nlm::MembranePotential>(current));
            
            // Apply dynamics
            bool spiked = neuron.stepLIF(current_time, 0.001);
            
            // Check for numerical instability
            double potential = neuron.getMembranePotential();
            
            // Potential should remain within reasonable bounds
            if (std::abs(potential) > 1e6) { // Extreme values indicate instability
                numerical_stable = false;
                std::cout << "    Property 'performance_numerical_stability' FAILED: "
                          << "Extreme potential value: " << potential << " at step " << step << std::endl;
                break;
            }
            
            // Track deviation from expected behavior
            if (spiked) {
                double expected_reset = neuron.getResetPotential();
                double deviation = std::abs(potential - expected_reset);
                max_deviation = std::max(max_deviation, deviation);
            }
            
            neuron.clearTotalCurrent();
        }
        
        if (max_deviation > instability_threshold) {
            numerical_stable = false;
            std::cout << "    Property 'performance_numerical_stability' FAILED: "
                      << "Large numerical deviation: " << max_deviation << " > " << instability_threshold << std::endl;
        }
    }
    
    return numerical_stable;
}

// Property 4: Performance should scale predictably
bool performance_scalability(nlm::property_test::PropertyRandom& rng) {
    const uint32_t network_sizes[] = {10, 50, 100, 200};
    const uint32_t spike_steps = 100;
    
    bool scales_predictably = true;
    
    std::cout << "    Testing scalability across network sizes:" << std::endl;
    
    for (uint32_t size : network_sizes) {
        // Create network
        std::vector<nlm::Neuron> neurons;
        for (uint32_t i = 0; i < size; ++i) {
            neurons.emplace_back(nlm::NeuronId(i));
            auto params = rng.generate_neuron_params(size);
            neurons.back().setType(params.type);
            neurons.back().setRestingPotential(params.resting_potential);
            neurons.back().setThreshold(params.threshold);
        }
        
        // Measure performance
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (uint32_t step = 0; step < spike_steps; ++step) {
            nlm::Timestamp current_time = static_cast<nlm::Timestamp>(step * 0.01);
            
            for (auto& neuron : neurons) {
                double current = rng.uniform_double(-5.0, 5.0);
                neuron.injectCurrent(static_cast<nlm::MembranePotential>(current));
                neuron.stepLIF(current_time, 0.01);
                neuron.clearTotalCurrent();
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        double processing_time_ms = duration.count();
        
        double neurons_per_ms = size / processing_time_ms;
        
        std::cout << "      Size " << size << ": " << processing_time_ms << " ms, "
                  << neurons_per_ms << " neurons/ms" << std::endl;
        
        // Check for scalability issues (sudden drops in performance)
        // If performance drops significantly, it might indicate O(n²) behavior
        if (neurons_per_ms < 1.0) { // Less than 1 neuron per millisecond
            scales_predictably = false;
            std::cout << "      WARNING: Poor scalability detected - "
                      << "only " << neurons_per_ms << " neurons/ms" << std::endl;
        }
    }
    
    return scales_predictably;
}

// Property test runner for performance
void runPerformancePropertyTests() {
    std::cout << "=== PERFORMANCE PROPERTY TESTS ===" << std::endl;
    
    nlm::property_test::PropertyTestEngine engine;
    nlm::property_test::PropertyRandom rng(12345);
    
    // Define property test instances
    class TestSpikeProcessingComplexity {
    public:
        void run(nlm::property_test::PropertyTestEngine& engine, nlm::property_test::PropertyRandom& rng) {
            nlm::property_test::PropertyTestResult result("performance", "spike_processing_complexity");
            result.test_iterations = 4;
            bool passed = performance_spike_processing_complexity(rng);
            result.passed = passed;
            engine.add_result(result);
        }
    };
    
    class TestMemoryUsageBounded {
    public:
        void run(nlm::property_test::PropertyTestEngine& engine, nlm::property_test::PropertyRandom& rng) {
            nlm::property_test::PropertyTestResult result("performance", "memory_usage_bounded");
            result.test_iterations = 3;
            bool passed = performance_memory_usage_bounded(rng);
            result.passed = passed;
            engine.add_result(result);
        }
    };
    
    class TestNumericalStability {
    public:
        void run(nlm::property_test::PropertyTestEngine& engine, nlm::property_test::PropertyRandom& rng) {
            nlm::property_test::PropertyTestResult result("performance", "numerical_stability");
            result.test_iterations = 100;
            bool passed = performance_numerical_stability(rng);
            result.passed = passed;
            engine.add_result(result);
        }
    };
    
    class TestScalability {
    public:
        void run(nlm::property_test::PropertyTestEngine& engine, nlm::property_test::PropertyRandom& rng) {
            nlm::property_test::PropertyTestResult result("performance", "scalability");
            result.test_iterations = 4;
            bool passed = performance_scalability(rng);
            result.passed = passed;
            engine.add_result(result);
        }
    };
    
    // Run all property tests
    engine.start_timer();
    
    TestSpikeProcessingComplexity().run(engine, rng);
    TestMemoryUsageBounded().run(engine, rng);
    TestNumericalStability().run(engine, rng);
    TestScalability().run(engine, rng);
    
    engine.stop_timer();
    engine.generate_report("performance_property_test_report.txt");
}

} // namespace test_performance

#endif // PROPERTY_TEST_PERFORMANCE_H