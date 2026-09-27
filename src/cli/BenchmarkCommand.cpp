#include "cli/BenchmarkCommand.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <numeric>
#include <algorithm>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"

using namespace nlm;
namespace cli {

BenchmarkCommand::BenchmarkCommand() 
    : CommandBase("benchmark", "Run performance benchmarks"),
      neurons_(1000),
      steps_(1000),
      output_format_("json"),
      benchmark_name_("default") {
}

bool BenchmarkCommand::execute(const std::map<std::string, std::string>& args) {
    std::cout << "Running benchmark: " << benchmark_name_ << std::endl;
    std::cout << "Configuration: " << neurons_ << " neurons, " 
              << steps_ << " steps" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neurons_), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Error: Failed to initialize brain for benchmark!" << std::endl;
        return false;
    }
    
    // Run benchmark
    auto benchmark_result = runBenchmark(brain, steps_);
    
    // Save benchmark results
    std::string output_file = "results/benchmark_" + benchmark_name_ + "_" + 
                             getTimestamp() + "." + output_format_;
    saveBenchmarkResults(benchmark_result, output_file, output_format_);
    
    std::cout << "Benchmark completed successfully!" << std::endl;
    std::cout << "Results saved to: " << output_file << std::endl;
    
    return true;
}

void BenchmarkCommand::printHelp() const {
    std::cout << "Usage: nlm-cli benchmark [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Run performance benchmarks." << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -n, --neurons <n>    Number of neurons for benchmark" << std::endl;
    std::cout << "  -s, --steps <n>      Number of simulation steps for benchmark" << std::endl;
    std::cout << "  -f, --format <fmt>   Output format (json, csv, text)" << std::endl;
    std::cout << "  -b, --benchmark <n>  Benchmark configuration name" << std::endl;
    std::cout << "  -q, --quiet          Suppress console output" << std::endl;
    std::cout << "  -v, --verbose        Enable verbose logging" << std::endl;
}

void BenchmarkCommand::printOptions() const {
    std::cout << "  [command options]" << std::endl;
}

json BenchmarkCommand::runBenchmark(std::shared_ptr<Brain> brain, int steps) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < steps; ++step) {
        // Inject some input
        for (int i = 0; i < std::min(10, brain->getTotalNeuronCount() / 100); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double total_time = std::chrono::duration<double>(end_time - start_time).count();
    
    json result;
    result["neurons"] = brain->getTotalNeuronCount();
    result["synapses"] = brain->getTotalSynapseCount();
    result["steps"] = steps;
    result["duration"] = total_time;
    result["speed"] = steps / total_time; // steps per second
    result["spikes_per_step"] = static_cast<double>(brain->getTotalSpikeCount()) / steps;
    result["synapses_per_neuron"] = static_cast<double>(brain->getTotalSynapseCount()) / 
                                   brain->getTotalNeuronCount();
    result["timestamp"] = getTimestamp();
    
    return result;
}

void BenchmarkCommand::saveBenchmarkResults(const json& benchmark_result, const std::string& output_file, 
                                          const std::string& format) {
    std::ofstream file(output_file);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open output file: " << output_file << std::endl;
        return;
    }
    
    if (format == "json") {
        file << benchmark_result.dump(2);
    } else if (format == "csv") {
        // Convert to CSV format
        file << "metric,value" << std::endl;
        for (const auto& item : benchmark_result.items()) {
            if (item.value().is_primitive()) {
                file << item.key() << "," << item.value().dump() << std::endl;
            }
        }
    } else if (format == "text") {
        file << "NLM Benchmark Results" << std::endl;
        file << "=======================" << std::endl;
        file << benchmark_result.dump() << std::endl;
    }
    
    file.close();
    std::cout << "Benchmark results saved to: " << output_file << std::endl;
}

} // namespace cli
} // namespace nlm
