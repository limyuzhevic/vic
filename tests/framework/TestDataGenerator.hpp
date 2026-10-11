// NLM Testing Framework
// Test Data Generator

#ifndef TEST_DATA_GENERATOR_HPP
#define TEST_DATA_GENERATOR_HPP

#include <vector>
#include <random>
#include <string>
#include <cmath>
#include <algorithm>
#include <chrono>

typedef long long Timestamp;

class TestDataGenerator {
private:
    std::mt19937 generator;
    std::uniform_real_distribution<double> double_dist;
    std::uniform_int_distribution<int> int_dist;
    
public:
    TestDataGenerator() : generator(std::chrono::system_clock::now().time_since_epoch().count()),
                          double_dist(-1.0, 1.0), int_dist(0, 100) {}
    
    // Generate random neural network weights
    std::vector<std::vector<double>> generateWeightMatrix(int rows, int cols) {
        std::vector<std::vector<double>> matrix(rows, std::vector<double>(cols));
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                matrix[i][j] = double_dist(generator);
            }
        }
        return matrix;
    }
    
    // Generate random neuron activities
    std::vector<double> generateNeuronActivities(int count, double correlation = 0.0) {
        std::vector<double> activities(count);
        for (int i = 0; i < count; ++i) {
            activities[i] = double_dist(generator);
        }
        
        if (correlation > 0.0) {
            // Add some correlation between neurons
            double avg = std::accumulate(activities.begin(), activities.end(), 0.0) / count;
            for (int i = 0; i < count; ++i) {
                activities[i] = activities[i] * (1 - correlation) + avg * correlation;
            }
        }
        
        return activities;
    }
    
    // Generate random synaptic connections
    std::vector<std::pair<int, int>> generateSynapticConnections(int pre_neuron_count, 
                                                                   int post_neuron_count,
                                                                   double connection_prob = 0.3) {
        std::vector<std::pair<int, int>> connections;
        std::bernoulli_distribution connect_dist(connection_prob);
        
        for (int pre = 0; pre < pre_neuron_count; ++pre) {
            for (int post = 0; post < post_neuron_count; ++post) {
                if (connect_dist(generator)) {
                    connections.push_back(std::make_pair(pre, post));
                }
            }
        }
        
        return connections;
    }
    
    // Generate random spike train
    std::vector<Timestamp> generateSpikeTrain(int neuron_count, Timestamp duration, 
                                             double firing_rate = 0.1) {
        std::vector<Timestamp> spike_times;
        std::poisson_distribution<int> poisson_dist(duration * firing_rate);
        
        for (int i = 0; i < neuron_count; ++i) {
            int spike_count = poisson_dist(generator);
            for (int j = 0; j < spike_count; ++j) {
                Timestamp spike_time = (rand() % duration) + 1;
                spike_times.push_back(spike_time);
            }
        }
        
        std::sort(spike_times.begin(), spike_times.end());
        return spike_times;
    }
    
    // Generate random brain regions
    std::vector<NeuralRegion> generateBrainRegions(int count) {
        std::vector<NeuralRegion> regions;
        for (int i = 0; i < count; ++i) {
            NeuralRegion region;
            region.id = i;
            region.type = static_cast<RegionType>(rand() % 4);
            region.x = double_dist(generator);
            region.y = double_dist(generator);
            region.z = double_dist(generator);
            region.volume = double_dist(generator) * 0.1 + 0.1; // Positive volume
            
            // Add some random properties
            region.neuron_density = double_dist(generator) * 0.5 + 0.5;
            region.blood_supply = double_dist(generator) * 0.3 + 0.7;
            region.metabolic_rate = double_dist(generator) * 0.5 + 0.5;
            
            regions.push_back(region);
        }
        return regions;
    }
    
    // Generate random memory patterns
    std::vector<std::vector<double>> generateMemoryPatterns(int pattern_count, int pattern_size) {
        std::vector<std::vector<double>> patterns(pattern_count, std::vector<double>(pattern_size));
        for (int i = 0; i < pattern_count; ++i) {
            for (int j = 0; j < pattern_size; ++j) {
                patterns[i][j] = double_dist(generator);
            }
        }
        return patterns;
    }
    
    // Generate random test scenarios
    std::vector<std::string> generateTestScenarios(int count) {
        std::vector<std::string> scenarios;
        std::vector<std::string> scenario_types = {
            "baseline_stability",
            "stress_response",
            "learning_curve",
            "adaptation",
            "recovery",
            "integration",
            "degradation",
            "regeneration"
        };
        
        for (int i = 0; i < count; ++i) {
            std::string scenario = scenario_types[rand() % scenario_types.size()];
            scenario += "_scenario_" + std::to_string(i + 1);
            scenarios.push_back(scenario);
        }
        
        return scenarios;
    }
    
    // Generate random performance test data
    std::vector<double> generatePerformanceData(int sample_count, double noise_level = 0.1) {
        std::vector<double> data(sample_count);
        for (int i = 0; i < sample_count; ++i) {
            // Generate realistic performance data with some pattern
            double trend = 0.001 * i; // Small upward trend
            double seasonality = 0.1 * std::sin(2 * M_PI * i / 100); // Daily pattern
            double noise = double_dist(generator) * noise_level; // Random noise
            
            data[i] = trend + seasonality + noise;
            if (data[i] < 0) data[i] = 0; // Ensure non-negative
        }
        return data;
    }
    
    // Generate random configuration parameters
    std::map<std::string, double> generateConfigParameters(int param_count) {
        std::map<std::string, double> params;
        std::vector<std::string> param_names = {
            "learning_rate",
            "neuron_count",
            "synaptic_connectivity",
            "memory_capacity",
            "adaptation_rate",
            "recovery_rate",
            "plasticity_strength",
            "inhibition_strength"
        };
        
        for (int i = 0; i < param_count && i < param_names.size(); ++i) {
            params[param_names[i]] = double_dist(generator) * 0.5 + 0.1; // Range [0.1, 0.6]
        }
        
        return params;
    }
    
    // Generate random test constraints
    struct TestConstraints {
        int max_neurons;
        double max_memory_usage;
        double max_cpu_time;
        int max_test_duration;
    };
    
    TestConstraints generateTestConstraints() {
        TestConstraints constraints;
        constraints.max_neurons = int_dist(generator) % 1000 + 100; // 100-1099
        constraints.max_memory_usage = double_dist(generator) * 1000 + 100; // 100-1100 MB
        constraints.max_cpu_time = double_dist(generator) * 300 + 30; // 30-330 seconds
        constraints.max_test_duration = int_dist(generator) % 3600 + 60; // 60-3660 seconds
        return constraints;
    }
};

#endif // TEST_DATA_GENERATOR_HPP