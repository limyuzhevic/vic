// Fuzz Testing Suite
// Tests random input fuzzing, configuration fuzzing, parameter fuzzing, and state fuzzing

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <unordered_set>
#include <sstream>
#include <algorithm>

namespace test_fuzz {

class FuzzTester {
public:
    struct FuzzTestResult {
        std::string test_name;
        int iterations;
        int crashes;
        int timeouts;
        double success_rate;
        std::string details;
    };

    std::vector<FuzzTestResult> results;

    void log_result(const std::string& test_name, int iterations, int crashes, int timeouts, double success_rate, const std::string& details = "") {
        std::cout << "[" << (success_rate > 90.0 ? "PASS" : "FAIL") << "] " << test_name << ": "
                  << iterations << " iterations, " << crashes << " crashes, " << timeouts << " timeouts, "
                  << std::fixed << std::setprecision(1) << success_rate << "% success rate " << details << std::endl;
        
        results.push_back({test_name, iterations, crashes, timeouts, success_rate, details});
    }

    void test_random_input_fuzzing() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Random Input Fuzzing" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        std::mt19937 rng(12345);
        std::uniform_real_distribution<float> float_dist(-10000.0f, 10000.0f);
        std::uniform_int_distribution<int> int_dist(0, 1000);
        std::uniform_int_distribution<int> step_dist(0, 10000);

        int iterations = 1000;
        int crashes = 0;
        int timeouts = 0;

        for (int i = 0; i < iterations; ++i) {
            try {
                // Random brain creation
                nlm::Brain brain;
                
                // Random neuron population
                int neuron_count = int_dist(rng) % 100 + 1; // 1-100 neurons
                for (int j = 0; j < neuron_count; ++j) {
                    nlm::Neuron neuron(nlm::NeuronId(j));
                    neuron.setMembranePotential(float_dist(rng));
                    neuron.setThreshold(float_dist(rng));
                    brain.addNeuron(neuron);
                }
                
                brain.initialize();
                
                // Random simulation steps
                int steps = step_dist(rng) % 1000; // 0-1000 steps
                auto start_time = std::chrono::high_resolution_clock::now();
                
                for (int step = 0; step < steps; ++step) {
                    brain.step();
                    
                    // Timeout check
                    auto current_time = std::chrono::high_resolution_clock::now();
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
                    if (elapsed > 1000) { // 1 second timeout
                        timeouts++;
                        break;
                    }
                }
                
            } catch (...) {
                crashes++;
            }
        }
        
        double success_rate = (1.0 - (double)(crashes + timeouts) / iterations) * 100.0;
        log_result("Random input fuzzing", iterations, crashes, timeouts, success_rate,
                 "Testing random brain creation and simulation");
    }

    void test_configuration_fuzzing() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Configuration Fuzzing" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        std::mt19937 rng(54321);
        std::uniform_real_distribution<float> float_dist(-1000.0f, 1000.0f);
        std::uniform_int_distribution<int> int_dist(0, 100);
        std::uniform_int_distribution<int> key_dist(0, 20);

        // Predefined valid keys for configuration fuzzing
        std::vector<std::string> config_keys = {
            "brain.neuron_count",
            "brain.region_count", 
            "brain.simulation_speed",
            "brain.plasticity_rate",
            "brain.noise_level",
            "brain.threshold_shift",
            "memory.working_memory_size",
            "memory.episodic_memory_capacity"
        };

        int iterations = 500;
        int crashes = 0;
        int invalid_configs = 0;

        for (int i = 0; i < iterations; ++i) {
            try {
                nlm::Config config;
                
                // Random configuration modifications
                int modifications = int_dist(rng) % 10; // 0-10 modifications
                
                for (int j = 0; j < modifications; ++j) {
                    std::string key = config_keys[key_dist(rng) % config_keys.size()];
                    
                    // Sometimes test invalid values
                    if (j % 5 == 0) {
                        // Test with potentially invalid values
                        float value = float_dist(rng);
                        if (value < -100.0f || value > 100.0f) {
                            invalid_configs++;
                            // Should handle gracefully
                        }
                        config.set(key, value);
                    } else {
                        int value = int_dist(rng);
                        config.set(key, value);
                    }
                }
                
                // Try to create brain from fuzzed config
                nlm::Brain brain(config);
                brain.initialize();
                
            } catch (...) {
                crashes++;
            }
        }
        
        double success_rate = (1.0 - (double)crashes / iterations) * 100.0;
        log_result("Configuration fuzzing", iterations, crashes, 0, success_rate,
                 "Testing random configuration mutations");
    }

    void test_parameter_fuzzing() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Parameter Fuzzing" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        std::mt19937 rng(99999);
        std::uniform_real_distribution<float> extreme_dist(-1e6f, 1e6f);
        std::uniform_real_distribution<float> small_dist(-0.001f, 0.001f);

        int iterations = 1000;
        int crashes = 0;
        int invalid_params = 0;

        for (int i = 0; i < iterations; ++i) {
            try {
                nlm::Brain brain;
                
                // Random parameter fuzzing
                int neuron_count = 10 + (rng() % 50); // 10-60 neurons
                
                for (int j = 0; j < neuron_count; ++j) {
                    nlm::Neuron neuron(nlm::NeuronId(j));
                    
                    // Fuzz neuron parameters with extreme values
                    if (j % 4 == 0) {
                        neuron.setMembranePotential(extreme_dist(rng)); // Extreme potential
                    } else if (j % 4 == 1) {
                        neuron.setThreshold(extreme_dist(rng)); // Extreme threshold
                    } else if (j % 4 == 2) {
                        neuron.injectCurrent(small_dist(rng)); // Tiny current
                    } else {
                        neuron.injectCurrent(extreme_dist(rng)); // Extreme current
                    }
                    
                    brain.addNeuron(neuron);
                }
                
                brain.initialize();
                
                // Fuzz simulation parameters
                int steps = rng() % 1000; // 0-999 steps
                for (int step = 0; step < steps; ++step) {
                    brain.step();
                }
                
            } catch (...) {
                crashes++;
            }
        }
        
        double success_rate = (1.0 - (double)crashes / iterations) * 100.0;
        log_result("Parameter fuzzing", iterations, crashes, 0, success_rate,
                 "Testing extreme parameter values");
    }

    void test_state_fuzzing() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing State Fuzzing" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        std::mt19937 rng(11111);
        std::uniform_real_distribution<float> state_dist(-1.0f, 1.0f);

        int iterations = 500;
        int crashes = 0;
        int invalid_states = 0;

        for (int i = 0; i < iterations; ++i) {
            try {
                nlm::Brain brain;
                
                // Create initial state
                for (int j = 0; j < 50; ++j) {
                    nlm::Neuron neuron(nlm::NeuronId(j));
                    neuron.setMembranePotential(state_dist(rng));
                    brain.addNeuron(neuron);
                }
                
                brain.initialize();
                
                // Run simulation to reach valid state
                for (int step = 0; step < 50; ++step) {
                    brain.step();
                }
                
                // Fuzz the state
                std::string state = brain.saveState();
                
                // Corrupt state with random modifications
                std::string corrupted_state = state;
                for (int j = 0; j < 10; ++j) {
                    int pos = rng() % corrupted_state.length();
                    corrupted_state[pos] = static_cast<char>(state_dist(rng) * 128.0f);
                }
                
                try {
                    // Try to load corrupted state
                    nlm::Brain corrupted_brain;
                    corrupted_brain.loadState(corrupted_state);
                    invalid_states++;
                } catch (...) {
                    // Expected behavior
                }
                
            } catch (...) {
                crashes++;
            }
        }
        
        double success_rate = (1.0 - (double)crashes / iterations) * 100.0;
        log_result("State fuzzing", iterations, crashes, 0, success_rate,
                 "Testing corrupted state handling");
    }

    void test_randomized_behavior() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Randomized Behavior" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        std::mt19937 rng(22222);
        std::uniform_real_distribution<float> rand_dist(0.0f, 1.0f);

        int iterations = 200;
        int crashes = 0;
        int inconsistencies = 0;
        
        std::vector<double> success_rates;

        for (int i = 0; i < iterations; ++i) {
            try {
                // Create multiple brains with same seed
                nlm::Config config;
                config.set("brain.neuron_count", 100);
                
                // Create first brain
                nlm::Brain brain1(config);
                
                // Set deterministic random seed
                nlm::RandomGenerator random1(42);
                nlm::RandomGenerator random2(42); // Same seed
                
                brain1.initializeRandom(&random1);
                
                // Create second brain with same seed
                nlm::Brain brain2(config);
                brain2.initializeRandom(&random2);
                
                // Both brains should have identical initial states
                bool deterministic = true;
                for (int j = 0; j < 100; ++j) {
                    nlm::Neuron* neuron1 = brain1.getNeuron(nlm::NeuronId(j));
                    nlm::Neuron* neuron2 = brain2.getNeuron(nlm::NeuronId(j));
                    
                    if (neuron1 && neuron2) {
                        if (std::abs(neuron1->getMembranePotential() - neuron2->getMembranePotential()) > 0.001f ||
                            std::abs(neuron1->getThreshold() - neuron2->getThreshold()) > 0.001f) {
                            deterministic = false;
                            break;
                        }
                    }
                }
                
                if (!deterministic) {
                    inconsistencies++;
                }
                
                // Run simulation
                for (int step = 0; step < 50; ++step) {
                    brain1.step();
                    brain2.step();
                }
                
                double success_rate = deterministic ? 100.0 : 0.0;
                success_rates.push_back(success_rate);
                
            } catch (...) {
                crashes++;
            }
        }
        
        double avg_success_rate = 0.0;
        if (!success_rates.empty()) {
            avg_success_rate = std::accumulate(success_rates.begin(), success_rates.end(), 0.0) / success_rates.size();
        }
        
        log_result("Randomized behavior testing", iterations, crashes, 0, avg_success_rate,
                 "Testing deterministic random number generation");
    }

    void run_all_tests() {
        std::cout << std::endl << std::string(80, '=') << std::endl;
        std::cout << "FUZZ TESTING SUITE" << std::endl;
        std::cout << std::string(80, '=') << std::endl;

        test_random_input_fuzzing();
        test_configuration_fuzzing();
        test_parameter_fuzzing();
        test_state_fuzzing();
        test_randomized_behavior();

        std::cout << std::endl << std::string(80, '=') << std::endl;
        std::cout << "TEST SUMMARY" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        int total_iterations = 0;
        int total_crashes = 0;
        int total_timeouts = 0;
        double total_success_rate = 0.0;
        
        for (const auto& result : results) {
            total_iterations += result.iterations;
            total_crashes += result.crashes;
            total_timeouts += result.timeouts;
            total_success_rate += result.success_rate;
        }
        
        if (!results.empty()) {
            total_success_rate /= results.size();
        }
        
        std::cout << "Total Iterations: " << total_iterations << std::endl;
        std::cout << "Total Crashes: " << total_crashes << std::endl;
        std::cout << "Total Timeouts: " << total_timeouts << std::endl;
        std::cout << "Overall Success Rate: " << total_success_rate << "%" << std::endl;

        std::cout << std::endl << "Detailed Results:" << std::endl;
        for (const auto& result : results) {
            std::cout << "  " << (result.success_rate > 90.0 ? "PASS" : "FAIL") << " " << result.test_name << ": "
                      << result.iterations << " iterations, " << result.crashes << " crashes, "
                      << result.timeouts << " timeouts, " << std::fixed << std::setprecision(1)
                      << result.success_rate << "% success" << std::endl;
        }
    }
};

int main() {
    FuzzTester tester;
    tester.run_all_tests();
    
    return 0;
}
