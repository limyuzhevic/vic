// Performance Benchmarking Suite for NLM
// 
// This benchmark suite includes:
// 1. Configuration validation performance tests
// 2. Brain simulation performance tests
// 3. Memory usage benchmarks
// 4. Scalability tests
// 5. Configuration comparison benchmarks
// 6. Integration performance tests

#include <iostream>
#include <chrono>
#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <map>
#include <thread>

namespace benchmark {

// Performance measurement utilities
class PerformanceTimer {
public:
    PerformanceTimer() : start_time_(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed_seconds() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_);
        return duration.count() / 1000.0;
    }
    
    void reset() {
        start_time_ = std::chrono::high_resolution_clock::now();
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_time_;
};

// Memory usage measurement
class MemoryProfiler {
public:
    static size_t get_current_memory_usage() {
        #ifdef __linux__
        return get_linux_memory_usage();
        #elif _WIN32
        return get_windows_memory_usage();
        #else
        return 0; // Platform not supported
        #endif
    }
    
private:
    static size_t get_linux_memory_usage() {
        // This is a simplified implementation
        // A real implementation would parse /proc/self/status or similar
        return 0;
    }
    
    static size_t get_windows_memory_usage() {
        // This is a simplified implementation
        // A real implementation would use Windows API
        return 0;
    }
};

// Benchmark configuration
struct BenchmarkConfig {
    std::string name;
    int duration_seconds;
    int iterations;
    std::map<std::string, std::string> parameters;
    
    BenchmarkConfig(const std::string& n, int duration, int iters)
        : name(n), duration_seconds(duration), iterations(iters) {}
    
    BenchmarkConfig& add_parameter(const std::string& key, const std::string& value) {
        parameters[key] = value;
        return *this;
    }
};

// Benchmark results
struct BenchmarkResult {
    std::string benchmark_name;
    double total_time_seconds;
    double average_time_per_iteration;
    double iterations_per_second;
    size_t memory_usage_before;
    size_t memory_usage_after;
    std::map<std::string, double> metrics;
    std::string details;
    
    void print() const {
        std::cout << "=== " << benchmark_name << " ===" << std::endl;
        std::cout << "Total time: " << total_time_seconds << "s" << std::endl;
        std::cout << "Average per iteration: " << average_time_per_iteration << "s" << std::endl;
        std::cout << "Iterations per second: " << iterations_per_second << std::endl;
        if (!metrics.empty()) {
            std::cout << "Metrics:" << std::endl;
            for (const auto& metric : metrics) {
                std::cout << "  " << metric.first << ": " << metric.second << std::endl;
            }
        }
        if (!details.empty()) {
            std::cout << "Details:" << std::endl;
            std::cout << details << std::endl;
        }
        std::cout << std::endl;
    }
};

// Configuration validation benchmarks
namespace validation_benchmarks {
    
    BenchmarkResult run_configuration_validation_benchmark(const BenchmarkConfig& config) {
        std::cout << "Running configuration validation benchmark: " << config.name << std::endl;
        
        nlm::ConfigSchema schema = nlm::ConfigSchema::createStandardSchema();
        
        PerformanceTimer timer;
        size_t memory_before = MemoryProfiler::get_current_memory_usage();
        
        // Run validation multiple times
        int iterations = config.iterations;
        for (int i = 0; i < iterations; ++i) {
            nlm::Config test_config;
            // Set up test configuration
            test_config.set("brain.neuron_count", 1000);
            test_config.set("brain.synapse_density", 0.1);
            test_config.set("simulation.timestep", 0.001);
            
            // Perform validation
            bool valid = schema.isValid(test_config);
            (void)valid; // Suppress unused variable warning
        }
        
        double total_time = timer.elapsed_seconds();
        size_t memory_after = MemoryProfiler::get_current_memory_usage();
        
        BenchmarkResult result;
        result.benchmark_name = config.name + " - Configuration Validation";
        result.total_time_seconds = total_time;
        result.average_time_per_iteration = total_time / iterations;
        result.iterations_per_second = iterations / total_time;
        result.memory_usage_before = memory_before;
        result.memory_usage_after = memory_after;
        result.metrics["validations_per_second"] = iterations / total_time;
        
        std::stringstream details;
        details << "Iterations: " << iterations << std::endl;
        details << "Average time per validation: " << (total_time / iterations) * 1000 << "ms" << std::endl;
        details << "Memory usage before: " << memory_before << " bytes" << std::endl;
        details << "Memory usage after: " << memory_after << " bytes" << std::endl;
        
        result.details = details.str();
        
        return result;
    }
    
    BenchmarkResult run_enhanced_configuration_validation_benchmark(const BenchmarkConfig& config) {
        std::cout << "Running enhanced configuration validation benchmark: " << config.name << std::endl;
        
        PerformanceTimer timer;
        size_t memory_before = MemoryProfiler::get_current_memory_usage();
        
        int iterations = config.iterations;
        for (int i = 0; i < iterations; ++i) {
            nlm::ValidatedConfig test_config;
            // Set up test configuration with validation
            test_config.set("brain.neuron_count", 1000);
            test_config.set("brain.synapse_density", 0.1);
            test_config.set("simulation.timestep", 0.001);
            
            // Validate configuration
            test_config.isValid();
        }
        
        double total_time = timer.elapsed_seconds();
        size_t memory_after = MemoryProfiler::get_current_memory_usage();
        
        BenchmarkResult result;
        result.benchmark_name = config.name + " - Enhanced Configuration Validation";
        result.total_time_seconds = total_time;
        result.average_time_per_iteration = total_time / iterations;
        result.iterations_per_second = iterations / total_time;
        result.memory_usage_before = memory_before;
        result.memory_usage_after = memory_after;
        result.metrics["validations_per_second"] = iterations / total_time;
        
        std::stringstream details;
        details << "Iterations: " << iterations << std::endl;
        details << "Average time per validation: " << (total_time / iterations) * 1000 << "ms" << std::endl;
        details << "Memory overhead per validation: " << (memory_after - memory_before) / iterations << " bytes" << std::endl;
        
        result.details = details.str();
        
        return result;
    }
    
} // namespace validation_benchmarks

// Brain simulation benchmarks
namespace simulation_benchmarks {
    
    BenchmarkResult run_basic_simulation_benchmark(const BenchmarkConfig& config) {
        std::cout << "Running basic simulation benchmark: " << config.name << std::endl;
        
        nlm::ValidatedConfig test_config;
        test_config.set("brain.neuron_count", 1000);
        test_config.set("simulation.timestep", 0.001);
        
        nlm::EnhancedBrain brain(test_config);
        brain.initialize();
        
        PerformanceTimer timer;
        size_t memory_before = MemoryProfiler::get_current_memory_usage();
        
        int iterations = config.iterations;
        for (int i = 0; i < iterations; ++i) {
            // Reset brain for each iteration
            brain.reset();
            brain.initialize();
            
            // Run simulation
            brain.run_simulation(100, verbose: false);
        }
        
        double total_time = timer.elapsed_seconds();
        size_t memory_after = MemoryProfiler::get_current_memory_usage();
        
        BenchmarkResult result;
        result.benchmark_name = config.name + " - Basic Simulation";
        result.total_time_seconds = total_time;
        result.average_time_per_iteration = total_time / iterations;
        result.iterations_per_second = iterations / total_time;
        result.memory_usage_before = memory_before;
        result.memory_usage_after = memory_after;
        result.metrics["simulations_per_second"] = iterations / total_time;
        result.metrics["steps_per_second"] = 100 * iterations / total_time;
        
        std::stringstream details;
        details << "Iterations: " << iterations << std::endl;
        details << "Steps per iteration: 100" << std::endl;
        details << "Average time per simulation: " << (total_time / iterations) * 1000 << "ms" << std::endl;
        details << "Average time per step: " << (total_time / (iterations * 100)) * 1000 << "ms" << std::endl;
        
        result.details = details.str();
        
        return result;
    }
    
    BenchmarkResult run_large_brain_simulation_benchmark(const BenchmarkConfig& config) {
        std::cout << "Running large brain simulation benchmark: " << config.name << std::endl;
        
        nlm::ValidatedConfig test_config;
        test_config.set("brain.neuron_count", 10000);
        test_config.set("simulation.timestep", 0.001);
        
        nlm::EnhancedBrain brain(test_config);
        brain.initialize();
        
        PerformanceTimer timer;
        size_t memory_before = MemoryProfiler::get_current_memory_usage();
        
        int iterations = config.iterations;
        for (int i = 0; i < iterations; ++i) {
            brain.reset();
            brain.initialize();
            
            // Run larger simulation
            brain.run_simulation(500, verbose: false);
        }
        
        double total_time = timer.elapsed_seconds();
        size_t memory_after = MemoryProfiler::get_current_memory_usage();
        
        BenchmarkResult result;
        result.benchmark_name = config.name + " - Large Brain Simulation";
        result.total_time_seconds = total_time;
        result.average_time_per_iteration = total_time / iterations;
        result.iterations_per_second = iterations / total_time;
        result.memory_usage_before = memory_before;
        result.memory_usage_after = memory_after;
        result.metrics["simulations_per_second"] = iterations / total_time;
        result.metrics["steps_per_second"] = 500 * iterations / total_time;
        
        std::stringstream details;
        details << "Iterations: " << iterations << std::endl;
        details << "Steps per iteration: 500" << std::endl;
        details << "Average time per simulation: " << (total_time / iterations) * 1000 << "ms" << std::endl;
        details << "Memory overhead per simulation: " << (memory_after - memory_before) / iterations << " bytes" << std::endl;
        
        result.details = details.str();
        
        return result;
    }
    
} // namespace simulation_benchmarks

// Integration benchmarks
namespace integration_benchmarks {
    
    BenchmarkResult run_complete_integration_benchmark(const BenchmarkConfig& config) {
        std::cout << "Running complete integration benchmark: " << config.name << std::endl;
        
        nlm::ValidatedConfig test_config;
        test_config.set("brain.neuron_count", 1000);
        test_config.set("neuromod.dopamine.scale", 1.0);
        test_config.set("neuromod.curiosity.enable", true);
        test_config.set("prediction.enable", true);
        test_config.set("development.enable", true);
        test_config.set("memory.working.enable", true);
        test_config.set("memory.episodic.enable", true);
        
        nlm::EnhancedBrain brain(test_config);
        nlm::EnhancedWorld world(20, 20, 8, 8);
        world.add_resource(5, 5, 1.0);
        world.add_hazard(15, 15, -0.5);
        nlm::EnhancedAgent agent(brain, world);
        
        PerformanceTimer timer;
        size_t memory_before = MemoryProfiler::get_current_memory_usage();
        
        int iterations = config.iterations;
        for (int i = 0; i < iterations; ++i) {
            // Run complete integration episode
            float total_reward = world.run_episode(brain, agent, 50, verbose: false);
            (void)total_reward; // Suppress unused variable warning
        }
        
        double total_time = timer.elapsed_seconds();
        size_t memory_after = MemoryProfiler::get_current_memory_usage();
        
        BenchmarkResult result;
        result.benchmark_name = config.name + " - Complete Integration";
        result.total_time_seconds = total_time;
        result.average_time_per_iteration = total_time / iterations;
        result.iterations_per_second = iterations / total_time;
        result.memory_usage_before = memory_before;
        result.memory_usage_after = memory_after;
        result.metrics["episodes_per_second"] = iterations / total_time;
        result.metrics["rewards_per_second"] = 0.0; // Placeholder
        
        std::stringstream details;
        details << "Iterations: " << iterations << std::endl;
        details << "Steps per episode: 50" << std::endl;
        details << "Average time per episode: " << (total_time / iterations) * 1000 << "ms" << std::endl;
        details << "Memory overhead per episode: " << (memory_after - memory_before) / iterations << " bytes" << std::endl;
        
        result.details = details.str();
        
        return result;
    }
    
} // namespace integration_benchmarks

// Configuration comparison benchmarks
namespace comparison_benchmarks {
    
    std::vector<BenchmarkResult> run_configuration_comparison_benchmarks(const BenchmarkConfig& base_config) {
        std::cout << "Running configuration comparison benchmarks..." << std::endl;
        
        std::vector<BenchmarkResult> results;
        
        // Test different neuron counts
        for (int neuron_count : {100, 1000, 10000}) {
            BenchmarkConfig config = base_config;
            config.name = "Neuron Count " + std::to_string(neuron_count);
            config.parameters["brain.neuron_count"] = std::to_string(neuron_count);
            
            nlm::ValidatedConfig test_config;
            test_config.set("brain.neuron_count", neuron_count);
            test_config.set("simulation.timestep", 0.001);
            
            nlm::EnhancedBrain brain(test_config);
            brain.initialize();
            
            PerformanceTimer timer;
            int iterations = base_config.iterations / 3; // Distribute iterations
            
            for (int i = 0; i < iterations; ++i) {
                brain.reset();
                brain.initialize();
                brain.run_simulation(50, verbose: false);
            }
            
            double total_time = timer.elapsed_seconds();
            
            BenchmarkResult result;
            result.benchmark_name = "Configuration Comparison: " + std::to_string(neuron_count) + " neurons";
            result.total_time_seconds = total_time;
            result.average_time_per_iteration = total_time / iterations;
            result.iterations_per_second = iterations / total_time;
            result.metrics["simulations_per_second"] = iterations / total_time;
            result.details = "Neuron count: " + std::to_string(neuron_count) + "\n" +
                           "Iterations: " + std::to_string(iterations);
            
            results.push_back(result);
        }
        
        return results;
    }
    
    std::vector<BenchmarkResult> run_plasticity_comparison_benchmarks(const BenchmarkConfig& base_config) {
        std::cout << "Running plasticity comparison benchmarks..." << std::endl;
        
        std::vector<BenchmarkResult> results;
        
        // Test different plasticity settings
        struct PlasticitySetting {
            std::string name;
            std::string stdp_enabled;
            std::string hebbian_enabled;
            std::string neuromod_enabled;
        };
        
        PlasticitySetting settings[] = {
            {"No Plasticity", "false", "false", "false"},
            {"STDP Only", "true", "false", "false"},
            {"Hebbian Only", "false", "true", "false"},
            {"Neuromodulation Only", "false", "false", "true"},
            {"All Plasticity", "true", "true", "true"}
        };
        
        for (const auto& setting : settings) {
            BenchmarkConfig config = base_config;
            config.name = "Plasticity: " + setting.name;
            config.parameters["plasticity.stdp.enable"] = setting.stdp_enabled;
            config.parameters["plasticity.hebbian.enable"] = setting.hebbian_enabled;
            config.parameters["neuromod.dopamine.scale"] = setting.neurom_enabled;
            
            nlm::ValidatedConfig test_config;
            test_config.set("brain.neuron_count", 1000);
            test_config.set("plasticity.stdp.enable", (setting.stdp_enabled == "true"));
            test_config.set("plasticity.hebbian.enable", (setting.hebbian_enabled == "true"));
            
            nlm::EnhancedBrain brain(test_config);
            brain.initialize();
            
            PerformanceTimer timer;
            int iterations = base_config.iterations / 5; // Distribute iterations
            
            for (int i = 0; i < iterations; ++i) {
                brain.reset();
                brain.initialize();
                brain.run_simulation(50, verbose: false);
            }
            
            double total_time = timer.elapsed_seconds();
            
            BenchmarkResult result;
            result.benchmark_name = "Plasticity Comparison: " + setting.name;
            result.total_time_seconds = total_time;
            result.average_time_per_iteration = total_time / iterations;
            result.iterations_per_second = iterations / total_time;
            result.metrics["simulations_per_second"] = iterations / total_time;
            result.details = "STDP: " + setting.stdp_enabled + "\n" +
                           "Hebbian: " + setting.hebbian_enabled + "\n" +
                           "Neuromodulation: " + setting.neurom_enabled;
            
            results.push_back(result);
        }
        
        return results;
    }
    
} // namespace comparison_benchmarks

// Main benchmark runner
class BenchmarkRunner {
public:
    std::vector<BenchmarkResult> run_all_benchmarks() {
        std::vector<BenchmarkResult> results;
        
        std::cout << "=== Starting NLM Performance Benchmark Suite ===" << std::endl;
        
        // Configuration validation benchmarks
        BenchmarkConfig validation_config("Validation Tests", 30, 1000);
        results.push_back(validation_benchmarks::run_configuration_validation_benchmark(validation_config));
        results.push_back(validation_benchmarks::run_enhanced_configuration_validation_benchmark(validation_config));
        
        // Simulation benchmarks
        BenchmarkConfig simulation_config("Simulation Tests", 30, 100);
        results.push_back(simulation_benchmarks::run_basic_simulation_benchmark(simulation_config));
        results.push_back(simulation_benchmarks::run_large_brain_simulation_benchmark(simulation_config));
        
        // Integration benchmarks
        BenchmarkConfig integration_config("Integration Tests", 30, 50);
        results.push_back(integration_benchmarks::run_complete_integration_benchmark(integration_config));
        
        // Configuration comparison benchmarks
        BenchmarkConfig comparison_config("Comparison Tests", 20, 150);
        auto comparison_results = comparison_benchmarks::run_configuration_comparison_benchmarks(comparison_config);
        results.insert(results.end(), comparison_results.begin(), comparison_results.end());
        
        auto plasticity_results = comparison_benchmarks::run_plasticity_comparison_benchmarks(comparison_config);
        results.insert(results.end(), plasticity_results.begin(), plasticity_results.end());
        
        // Print summary
        std::cout << "\n=== Benchmark Summary ===" << std::endl;
        double total_time = 0.0;
        for (const auto& result : results) {
            std::cout << result.benchmark_name << ": " << result.total_time_seconds << "s" << std::endl;
            total_time += result.total_time_seconds;
        }
        std::cout << "Total benchmark time: " << total_time << "s" << std::endl;
        
        return results;
    }
    
    void save_results_to_file(const std::vector<BenchmarkResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
            return;
        }
        
        file << "NLM Performance Benchmark Results" << std::endl;
        file << "================================" << std::endl;
        file << std::endl;
        
        for (const auto& result : results) {
            file << "Benchmark: " << result.benchmark_name << std::endl;
            file << "Total Time: " << result.total_time_seconds << "s" << std::endl;
            file << "Average Time per Iteration: " << result.average_time_per_iteration << "s" << std::endl;
            file << "Iterations per Second: " << result.iterations_per_second << std::endl;
            file << "Memory Usage Before: " << result.memory_usage_before << " bytes" << std::endl;
            file << "Memory Usage After: " << result.memory_usage_after << " bytes" << std::endl;
            
            if (!result.metrics.empty()) {
                file << "Metrics:" << std::endl;
                for (const auto& metric : result.metrics) {
                    file << "  " << metric.first << ": " << metric.second << std::endl;
                }
            }
            
            if (!result.details.empty()) {
                file << "Details:" << std::endl;
                file << result.details << std::endl;
            }
            
            file << std::endl;
        }
        
        file.close();
        std::cout << "Results saved to: " << filename << std::endl;
    }
};

// Global benchmark runner
void run_all_benchmarks() {
    BenchmarkRunner runner;
    auto results = runner.run_all_benchmarks();
    runner.save_results_to_file(results, "benchmark_results.txt");
}

} // namespace benchmark

// Main function
int main() {
    std::cout << "NLM Performance Benchmarking Suite" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    try {
        benchmark::run_all_benchmarks();
        std::cout << "\nBenchmark suite completed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Benchmark suite failed with error: " << e.what() << std::endl;
        return 1;
    }
}