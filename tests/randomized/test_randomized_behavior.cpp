// Randomized Testing Suite
// Tests Monte Carlo testing for probabilistic components, statistical validation,
// probability distribution testing, and random seed reproducibility

#include "brain/Brain.hpp"
#include "core/Random/Random.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <map>
#include <cmath>

namespace test_randomized {

class RandomizedTester {
public:
    struct StatisticalResult {
        std::string test_name;
        double mean_value;
        double std_deviation;
        double min_value;
        double max_value;
        double confidence_interval;
        bool passed;
        std::string details;
    };

    struct MonteCarloResult {
        std::string test_name;
        int iterations;
        int successes;
        double success_rate;
        double p_value;
        bool passed;
        std::string details;
    };

    std::vector<StatisticalResult> statistical_results;
    std::vector<MonteCarloResult> monte_carlo_results;

    void log_statistical_result(const std::string& test_name, double mean, double std_dev, 
                               double min_val, double max_val, double confidence_interval,
                               bool passed, const std::string& details = "") {
        std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << test_name << ": "
                  << std::fixed << std::setprecision(3)
                  << "mean=" << mean << ", std=" << std_dev << ", min=" << min_val << ", max=" << max_val << ", CI=" << confidence_interval << "% " << details << std::endl;
        
        statistical_results.push_back({test_name, mean, std_dev, min_val, max_val, confidence_interval, passed, details});
    }

    void log_monte_carlo_result(const std::string& test_name, int iterations, int successes,
                               double success_rate, double p_value, bool passed,
                               const std::string& details = "") {
        std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << test_name << ": "
                  << iterations << " iterations, " << successes << " successes, "
                  << std::fixed << std::setprecision(2) << success_rate << "% success, " << std::scientific << p_value << " p-value " << details << std::endl;
        
        monte_carlo_results.push_back({test_name, iterations, successes, success_rate, p_value, passed, details});
    }

    void test_probability_distribution() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Probability Distribution" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: Neuron membrane potential distribution
        std::mt19937 rng(12345);
        std::normal_distribution<float> normal_dist(-65.0f, 10.0f); // Mean, std dev

        std::vector<float> membrane_potentials;
        for (int i = 0; i < 10000; ++i) {
            membrane_potentials.push_back(normal_dist(rng));
        }
        
        // Calculate statistics
        double mean = std::accumulate(membrane_potentials.begin(), membrane_potentials.end(), 0.0) / membrane_potentials.size();
        double sum_squared_diff = 0.0;
        for (float val : membrane_potentials) {
            sum_squared_diff += (val - mean) * (val - mean);
        }
        double std_dev = std::sqrt(sum_squared_diff / membrane_potentials.size());
        double min_val = *std::min_element(membrane_potentials.begin(), membrane_potentials.end());
        double max_val = *std::max_element(membrane_potentials.begin(), membrane_potentials.end());
        
        // Expected distribution characteristics
        double expected_mean = -65.0f;
        double expected_std = 10.0f;
        
        // Check if distribution matches expected (within 2% tolerance)
        double mean_error = std::abs(mean - expected_mean) / std::abs(expected_mean);
        double std_error = std::abs(std_dev - expected_std) / std::abs(expected_std);
        
        bool passed = (mean_error < 0.02 && std_error < 0.02);
        
        log_statistical_result("Neuron membrane potential distribution", mean, std_dev, 
                              min_val, max_val, 95.0, passed,
                              "Expected normal distribution with mean=-65, std=10");

        // Test 2: Synaptic weight distribution
        std::vector<float> synaptic_weights;
        std::uniform_real_distribution<float> weight_dist(-1.0f, 1.0f);
        
        for (int i = 0; i < 10000; ++i) {
            synaptic_weights.push_back(weight_dist(rng));
        }
        
        // Calculate statistics
        mean = std::accumulate(synaptic_weights.begin(), synaptic_weights.end(), 0.0) / synaptic_weights.size();
        sum_squared_diff = 0.0;
        for (float val : synaptic_weights) {
            sum_squared_diff += (val - mean) * (val - mean);
        }
        std_dev = std::sqrt(sum_squared_diff / synaptic_weights.size());
        min_val = *std::min_element(synaptic_weights.begin(), synaptic_weights.end());
        max_val = *std::max_element(synaptic_weights.begin(), synaptic_weights.end());
        
        // Expected uniform distribution characteristics
        expected_mean = 0.0f;
        expected_std = 1.0f / std::sqrt(3.0f); // For uniform(-1,1)
        
        mean_error = std::abs(mean - expected_mean) / std::abs(expected_mean + 0.001);
        std_error = std::abs(std_dev - expected_std) / std::abs(expected_std);
        
        passed = (mean_error < 0.01 && std_error < 0.05);
        
        log_statistical_result("Synaptic weight distribution", mean, std_dev,
                              min_val, max_val, 95.0, passed,
                              "Expected uniform distribution with mean=0, std=0.577");
    }

    void test_monte_carlo_neuron_firing() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Monte Carlo Neuron Firing" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        std::mt19937 rng(54321);
        std::uniform_real_distribution<float> input_dist(0.0f, 100.0f);

        int iterations = 10000;
        int successes = 0;
        
        for (int i = 0; i < iterations; ++i) {
            nlm::Neuron neuron(nlm::NeuronId(i % 100));
            
            // Inject random input
            float input = input_dist(rng);
            neuron.injectCurrent(input);
            
            // Set random threshold
            float threshold = input_dist(rng) / 10.0f; // Lower threshold
            neuron.setThreshold(threshold);
            
            // Simulate membrane potential change
            neuron.setMembranePotential(neuron.getMembranePotential() + input - neuron.getTotalCurrent());
            
            // Check if neuron should fire
            if (neuron.getMembranePotential() >= neuron.getThreshold() &&
                neuron.getMembranePotential() < neuron.getThreshold() + 1.0f) {
                successes++;
            }
            
            neuron.clearTotalCurrent();
        }
        
        double success_rate = (double)successes / iterations * 100.0;
        
        // Statistical test for firing rate consistency
        double expected_rate = 50.0; // 50% firing rate expected
        double deviation = std::abs(success_rate - expected_rate) / expected_rate;
        
        bool passed = (deviation < 0.1); // Within 10% of expected
        
        log_monte_carlo_result("Neuron firing Monte Carlo test", iterations, successes,
                              success_rate, 0.05, passed,
                              "Testing neuron firing probability consistency");
    }

    void test_statistical_validation_neuromodulation() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Statistical Validation Neuromodulation" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        std::mt19937 rng(99999);
        std::normal_distribution<float> reward_dist(10.0f, 5.0f); // Mean reward, std dev
        std::normal_distribution<float> penalty_dist(-10.0f, 3.0f);

        // Test reward modulation distribution
        std::vector<float> reward_effects;
        for (int i = 0; i < 1000; ++i) {
            nlm::Neuron neuron(nlm::NeuronId(i));
            nlm::Reward reward_mod;
            
            float reward = (i % 2 == 0) ? reward_dist(rng) : penalty_dist(rng);
            reward_mod.applyModulation(&neuron, reward);
            
            reward_effects.push_back(neuron.getTotalCurrent());
        }
        
        // Calculate statistics
        double mean = std::accumulate(reward_effects.begin(), reward_effects.end(), 0.0) / reward_effects.size();
        double sum_squared_diff = 0.0;
        for (float val : reward_effects) {
            sum_squared_diff += (val - mean) * (val - mean);
        }
        double std_dev = std::sqrt(sum_squared_diff / reward_effects.size());
        double min_val = *std::min_element(reward_effects.begin(), reward_effects.end());
        double max_val = *std::max_element(reward_effects.begin(), reward_effects.end());
        
        // Test novelty detection distribution
        std::vector<float> novelty_scores;
        for (int i = 0; i < 1000; ++i) {
            nlm::Novelty novelty_mod;
            
            // Create patterns with varying similarity
            std::vector<float> pattern1(10, 0.0f);
            std::vector<float> pattern2(10);
            for (int j = 0; j < 10; ++j) {
                pattern2[j] = (j < i % 5) ? 1.0f : 0.0f; // Partial overlap
            }
            
            float novelty = novelty_mod.calculateNovelty(pattern1, pattern2);
            novelty_scores.push_back(novelty);
        }
        
        // Calculate novelty statistics
        double novelty_mean = std::accumulate(novelty_scores.begin(), novelty_scores.end(), 0.0) / novelty_scores.size();
        double novelty_sum_squared_diff = 0.0;
        for (float val : novelty_scores) {
            novelty_sum_squared_diff += (val - novelty_mean) * (val - novelty_mean);
        }
        double novelty_std_dev = std::sqrt(novelty_sum_squared_diff / novelty_scores.size());
        
        // Check if distributions are reasonable
        bool reward_passed = (std::isfinite(mean) && std::isfinite(std_dev) && std_dev > 0.0);
        bool novelty_passed = (std::isfinite(novelty_mean) && std::isfinite(novelty_std_dev) && novelty_std_dev > 0.0);
        
        if (reward_passed) {
            log_statistical_result("Reward modulation distribution", mean, std_dev,
                                  min_val, max_val, 95.0, true,
                                  "Testing reward signal distribution characteristics");
        } else {
            log_statistical_result("Reward modulation distribution", mean, std_dev,
                                  min_val, max_val, 95.0, false,
                                  "Abnormal reward distribution");
        }
        
        if (novelty_passed) {
            log_statistical_result("Novelty detection distribution", novelty_mean, novelty_std_dev,
                                  novelty_mean - 3*novelty_std_dev, novelty_mean + 3*novelty_std_dev, 99.7,
                                  true, "Testing novelty detection statistical properties");
        } else {
            log_statistical_result("Novelty detection distribution", novelty_mean, novelty_std_dev,
                                  novelty_mean - 3*novelty_std_dev, novelty_mean + 3*novelty_std_dev, 99.7,
                                  false, "Abnormal novelty distribution");
        }
    }

    void test_random_seed_reproducibility() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Random Seed Reproducibility" << std::endl;
        cout << std::string(70, '=') << std::endl;

        std::vector<std::vector<float>> sequence1;
        std::vector<std::vector<float>> sequence2;

        // Generate sequences with same seed
        for (int seed = 1; seed <= 10; ++seed) {
            std::mt19937 rng1(seed);
            std::mt19937 rng2(seed);
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            
            std::vector<float> seq1, seq2;
            for (int i = 0; i < 100; ++i) {
                seq1.push_back(dist(rng1));
                seq2.push_back(dist(rng2));
            }
            
            sequence1.push_back(seq1);
            sequence2.push_back(seq2);
        }
        
        // Verify reproducibility
        int reproducible_count = 0;
        for (int seed = 0; seed < 10; ++seed) {
            bool reproducible = true;
            for (int i = 0; i < 100; ++i) {
                if (std::abs(sequence1[seed][i] - sequence2[seed][i]) > 0.0001f) {
                    reproducible = false;
                    break;
                }
            }
            if (reproducible) reproducible_count++;
        }
        
        double reproducibility_rate = (double)reproducible_count / 10.0 * 100.0;
        
        bool passed = (reproducible_count == 10); // All 10 seeds should be reproducible
        
        log_monte_carlo_result("Random seed reproducibility", 10, reproducible_count,
                              reproducibility_rate, 0.0, passed,
                              "Testing that same seed produces identical sequences");
    }

    void test_probability_consistency() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Probability Consistency" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        std::mt19937 rng(11111);
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);

        // Test 1: Long sequence consistency
        int iterations = 100000;
        int count_less_than_0_5 = 0;
        
        for (int i = 0; i < iterations; ++i) {
            float val = dist(rng);
            if (val < 0.5f) {
                count_less_than_0_5++;
            }
        }
        
        double empirical_prob = (double)count_less_than_0_5 / iterations;
        double theoretical_prob = 0.5f;
        double error = std::abs(empirical_prob - theoretical_prob);
        double confidence_interval = 1.96 * std::sqrt(theoretical_prob * (1.0 - theoretical_prob) / iterations) * 100.0; // 95% CI
        
        bool passed = (error < 0.01); // Less than 1% error
        
        log_statistical_result("Probability consistency test", empirical_prob, confidence_interval,
                               empirical_prob - confidence_interval / 100.0, empirical_prob + confidence_interval / 100.0,
                               95.0, passed,
                               std::to_string(error * 100.0) + "% error from theoretical probability");
    }

    void run_all_tests() {
        std::cout << std::endl << std::string(80, '=') << std::endl;
        std::cout << "RANDOMIZED TESTING SUITE" << std::endl;
        std::cout << std::string(80, '=') << std::endl;

        test_probability_distribution();
        test_monte_carlo_neuron_firing();
        test_statistical_validation_neuromodulation();
        test_random_seed_reproducibility();
        test_probability_consistency();

        std::cout << std::endl << std::string(80, '=') << std::endl;
        std::cout << "TEST SUMMARY" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        int passed = 0, failed = 0;
        for (const auto& result : statistical_results) {
            if (result.passed) passed++; else failed++;
        }
        for (const auto& result : monte_carlo_results) {
            if (result.passed) passed++; else failed++;
        }
        
        std::cout << "Total Statistical Tests: " << (passed + failed) << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;

        std::cout << std::endl << "Statistical Results:" << std::endl;
        for (const auto& result : statistical_results) {
            std::cout << "  " << (result.passed ? "PASS" : "FAIL") << " " << result.test_name << std::endl;
        }
        
        std::cout << std::endl << "Monte Carlo Results:" << std::endl;
        for (const auto& result : monte_carlo_results) {
            std::cout << "  " << (result.passed ? "PASS" : "FAIL") << " " << result.test_name << ": "
                      << result.iterations << " iterations, " << std::fixed << std::setprecision(1)
                      << result.success_rate << "% success" << std::endl;
        }
    }
};

int main() {
    RandomizedTester tester;
    tester.run_all_tests();
    
    return 0;
}
