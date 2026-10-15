// Benchmarking example
#include "src/bench/Benchmark.hpp"
#include <iostream>
#include <chrono>

int main() {
    std::cout << "Benchmarking example" << std::endl;
    std::cout << "======================" << std::endl;
    
    // Create benchmark configuration
    nlm::BenchmarkConfig benchmarkConfig;
    
    benchmarkConfig.name = "NeuralNetworkBenchmark";
    benchmarkConfig.neuron_count = 1000;
    benchmarkConfig.region_count = 2;
    benchmarkConfig.connection_probability = 0.15;
    benchmarkConfig.max_simulation_steps = 10000;
    
    // Create benchmark
    nlm::Benchmark benchmark(benchmarkConfig);
    
    // Run benchmark
    std::cout << "Running benchmark..." << std::endl;
    auto results = benchmark.run();
    
    // Print benchmark results
    std::cout << "Benchmark Results:" << std::endl;
    std::cout << "=================" << std::endl;
    std::cout << "Benchmark name: " << results.name << std::endl;
    std::cout << "Neuron count: " << results.neuron_count << std::endl;
    std::cout << "Region count: " << results.region_count << std::endl;
    std::cout << "Connection probability: " << results.connection_probability << std::endl;
    std::cout << "Max simulation steps: " << results.max_simulation_steps << std::endl;
    std::cout << "Total time: " << results.total_time << " seconds" << std::endl;
    std::cout << "Average firing rate: " << results.average_firing_rate << std::endl;
    std::cout << "Total spike count: " << results.total_spike_count << std::endl;
    std::cout << "Memory usage: " << results.memory_usage << " MB" << std::endl;
    std::cout << "CPU usage: " << results.cpu_usage << "%" << std::endl;
    
    // Run multiple benchmarks
    std::cout << std::endl;
    std::cout << "Running multiple benchmarks..." << std::endl;
    
    std::vector<nlm::BenchmarkResult> all_results;
    
    // Benchmark with different configurations
    nlm::BenchmarkConfig config1;
    config1.name = "SmallNetwork";
    config1.neuron_count = 100;
    config1.region_count = 1;
    config1.connection_probability = 0.1;
    config1.max_simulation_steps = 1000;
    
    nlm::Benchmark bench1(config1);
    all_results.push_back(bench1.run());
    
    nlm::BenchmarkConfig config2;
    config2.name = "MediumNetwork";
    config2.neuron_count = 1000;
    config2.region_count = 2;
    config2.connection_probability = 0.15;
    config2.max_simulation_steps = 10000;
    
    nlm::Benchmark bench2(config2);
    all_results.push_back(bench2.run());
    
    nlm::BenchmarkConfig config3;
    config3.name = "LargeNetwork";
    config3.neuron_count = 5000;
    config3.region_count = 3;
    config3.connection_probability = 0.2;
    config3.max_simulation_steps = 50000;
    
    nlm::Benchmark bench3(config3);
    all_results.push_back(bench3.run());
    
    // Print all benchmark results
    std::cout << std::endl;
    std::cout << "All Benchmark Results:" << std::endl;
    std::cout << "======================" << std::endl;
    
    for (const auto& result : all_results) {
        std::cout << "Benchmark name: " << result.name << std::endl;
        std::cout << "Neuron count: " << result.neuron_count << std::endl;
        std::cout << "Region count: " << result.region_count << std::endl;
        std::cout << "Connection probability: " << result.connection_probability << std::endl;
        std::cout << "Max simulation steps: " << result.max_simulation_steps << std::endl;
        std::cout << "Total time: " << result.total_time << " seconds" << std::endl;
        std::cout << "Average firing rate: " << result.average_firing_rate << std::endl;
        std::cout << "Total spike count: " << result.total_spike_count << std::endl;
        std::cout << "Memory usage: " << result.memory_usage << " MB" << std::endl;
        std::cout << "CPU usage: " << result.cpu_usage << "%" << std::endl;
        std::cout << std::endl;
    }
    
    std::cout << "Benchmarking example completed successfully!" << std::endl;
    return 0;
}