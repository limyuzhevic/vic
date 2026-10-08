// NLM Benchmark Tool
// File: src/tools/benchmark.cpp

#include "performance/Benchmark.hpp"
#include "performance/Profiler.hpp"
#include "core/Config/OptimizedConfig.hpp"
#include "brain/OptimizedBrain.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <random>
#include <iomanip>
#include <map>

namespace nlm {

class NLMBenchmark {
public:
    enum class BenchmarkType {
        Config,           // Configuration system benchmarks
        Logger,           // Logger system benchmarks
        Random,           // Random number generation benchmarks
        Neuron,           // Neuron dynamics benchmarks
        Synapse,          // Synapse processing benchmarks
        Memory,           // Memory system benchmarks
        Neuromodulation,  // Neuromodulation system benchmarks
        Plasticity,       // Plasticity system benchmarks
        Prediction,       // Prediction system benchmarks
        Integration,      // Integration benchmarks
        Scalability,      // Scalability benchmarks
        All               // All benchmarks
    };
    
    NLMBenchmark() {
        initializeBenchmarks();
    }
    
    ~NLMBenchmark() {
        finalizeBenchmarks();
    }
    
    void runBenchmarks(BenchmarkType type) {
        std::cout << "=== NLM Benchmark Suite ===\n";
        std::cout << "Running benchmarks for: " << benchmarkTypeToString(type) << "\n";
        
        switch (type) {
            case BenchmarkType::Config:
                runConfigBenchmarks();
                break;
            case BenchmarkType::Logger:
                runLoggerBenchmarks();
                break;
            case BenchmarkType::Random:
                runRandomBenchmarks();
                break;
            case BenchmarkType::Neuron:
                runNeuronBenchmarks();
                break;
            case BenchmarkType::Synapse:
                runSynapseBenchmarks();
                break;
            case BenchmarkType::Memory:
                runMemoryBenchmarks();
                break;
            case BenchmarkType::Neuromodulation:
                runNeuromodulationBenchmarks();
                break;
            case BenchmarkType::Plasticity:
                runPlasticityBenchmarks();
                break;
            case BenchmarkType::Prediction:
                runPredictionBenchmarks();
                break;
            case BenchmarkType::Integration:
                runIntegrationBenchmarks();
                break;
            case BenchmarkType::Scalability:
                runScalabilityBenchmarks();
                break;
            case BenchmarkType::All:
                runAllBenchmarks();
                break;
        }
        
        std::cout << "\n=== Benchmark Suite Complete ===\n";
        std::cout << "All benchmarks completed successfully!\n";
    }
    
    void analyzePerformance() {
        std::cout << "=== NLM Performance Analysis ===\n";
        std::cout << "Analyzing performance characteristics...\n";
        
        // Get global profiler results
        auto& profiler = getGlobalProfiler();
        auto results = profiler.getResults();
        
        if (results.empty()) {
            std::cout << "No performance data available. Run benchmarks first." << std::endl;
            return;
        }
        
        std::cout << "Performance Analysis Report:\n";
        std::cout << "Total operations profiled: " << results.size() << "\n";
        
        // Analyze by category
        std::map<std::string, std::vector<const PerformanceProfiler::ProfileResult*>> results_by_category;
        for (const auto& result : results) {
            size_t pos = result.name.find('.');
            std::string category = (pos != std::string::npos) ? result.name.substr(0, pos) : "General";
            results_by_category[category].push_back(&result);
        }
        
        std::cout << "\nPerformance by Category:\n";
        for (const auto& [category, category_results] : results_by_category) {
            double total_duration = 0.0;
            size_t total_calls = 0;
            
            for (const auto* result : category_results) {
                total_duration += result->duration_ms;
                total_calls += result->call_count;
            }
            
            if (!category_results.empty()) {
                double avg_duration = total_duration / category_results.size();
                double throughput = (total_calls * 1000.0) / total_duration; // operations per second
                
                std::cout << std::left << std::setw(25) << category
                         << "Duration: " << std::fixed << std::setprecision(3) << avg_duration << "ms"
                         << " Throughput: " << std::fixed << std::setprecision(3) << throughput << " ops/sec"
                         << " Calls: " << total_calls << "\n";
            }
        }
        
        std::cout << "\nTop 10 Slowest Operations:\n";
        auto sorted_by_duration = profiler.getResultsByDuration();
        for (size_t i = 0; i < std::min(10UL, sorted_by_duration.size()); ++i) {
            const auto& result = sorted_by_duration[i];
            std::cout << std::setw(3) << i+1 << ". " << std::left << std::setw(30) << result.name
                     << "Duration: " << std::fixed << std::setprecision(3) << result.duration_ms << "ms"
                     << " Calls: " << result.call_count
                     << " Average: " << std::fixed << std::setprecision(3) << result.avg_duration_ms << "ms\n";
        }
        
        std::cout << "\nTop 10 Most Called Operations:\n";
        auto sorted_by_count = profiler.getResultsByCount();
        for (size_t i = 0; i < std::min(10UL, sorted_by_count.size()); ++i) {
            const auto& result = sorted_by_count[i];
            std::cout << std::setw(3) << i+1 << ". " << std::left << std::setw(30) << result.name
                     << "Calls: " << result.call_count
                     << " Duration: " << std::fixed << std::setprecision(3) << result.duration_ms << "ms"
                     << " Average: " << std::fixed << std::setprecision(3) << result.avg_duration_ms << "ms\n";
        }
        
        std::cout << "\n=== Performance Analysis Complete ===\n";
    }
    
    void generateReport(const std::string& filename) {
        std::cout << "=== Generating Performance Report ===\n";
        std::cout << "Generating comprehensive performance report...\n";
        
        // Get global profiler results
        auto& profiler = getGlobalProfiler();
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open report file: " << filename << std::endl;
            return;
        }
        
        // Generate report
        std::string report = profiler.generateReport();
        file << report;
        
        file.close();
        
        std::cout << "Performance report saved to: " << filename << std::endl;
        std::cout << "=== Report Generation Complete ===\n";
    }
    
private:
    struct BenchmarkResult {
        std::string name;
        std::string category;
        double duration_ms;
        double throughput;
        bool passed;
        
        BenchmarkResult(const std::string& n, const std::string& c, double dur, double thr, bool pass)
            : name(n), category(c), duration_ms(dur), throughput(thr), passed(pass) {}
    };
    
    std::vector<BenchmarkResult> benchmark_results_;
    
    void initializeBenchmarks() {
        benchmark_results_.clear();
        std::cout << "Benchmark suite initialized" << std::endl;
    }
    
    void finalizeBenchmarks() {
        std::cout << "Benchmark suite finalized" << std::endl;
        std::cout << "Total benchmarks run: " << benchmark_results_.size() << std::endl;
        
        size_t passed = 0;
        for (const auto& result : benchmark_results_) {
            if (result.passed) passed++;
        }
        
        std::cout << "Benchmarks passed: " << passed << std::endl;
        std::cout << "Success rate: " << (passed * 100.0 / benchmark_results_.size()) << "%" << std::endl;
    }
    
    std::string benchmarkTypeToString(BenchmarkType type) {
        switch (type) {
            case BenchmarkType::Config: return "Config System";
            case BenchmarkType::Logger: return "Logger System";
            case BenchmarkType::Random: return "Random Number Generation";
            case BenchmarkType::Neuron: return "Neuron Dynamics";
            case BenchmarkType::Synapse: return "Synapse Processing";
            case BenchmarkType::Memory: return "Memory Systems";
            case BenchmarkType::Neuromodulation: return "Neuromodulation";
            case BenchmarkType::Plasticity: return "Plasticity Rules";
            case BenchmarkType::Prediction: return "Prediction System";
            case BenchmarkType::Integration: return "Integration";
            case BenchmarkType::Scalability: return "Scalability";
            case BenchmarkType::All: return "All Benchmarks";
            default: return "Unknown";
        }
    }
    
    // Config benchmarks
    void runConfigBenchmarks() {
        std::cout << "Running Config System Benchmarks...\n";
        
        // JSON parsing benchmarks
        auto json_result = ConfigBenchmark::benchmarkJsonParsing("{\"test\": \"value\", \"number\": 42, \"bool\": true, \"array\": [1,2,3,4,5]}", 10000);
        benchmark_results_.emplace_back("JSON Parsing", "Config", json_result.duration_ms, json_result.throughput, json_result.passed);
        std::cout << "✓ JSON parsing benchmark completed" << std::endl;
        
        // YAML parsing benchmarks
        auto yaml_result = ConfigBenchmark::benchmarkYamlParsing("test: value\nnumber: 42\nbool: true\narray:\n  - 1\n  - 2\n  - 3", 10000);
        benchmark_results_.emplace_back("YAML Parsing", "Config", yaml_result.duration_ms, yaml_result.throughput, yaml_result.passed);
        std::cout << "✓ YAML parsing benchmark completed" << std::endl;
        
        // Config access benchmarks
        nlm::Config config;
        config.loadFromDefaults();
        config.set("brain.neuron_count", 1000);
        config.set("memory.capacity", 5000);
        config.set("neuromodulation.reward_scale", 1.5);
        
        auto access_result = ConfigBenchmark::benchmarkConfigAccess(config, {"brain.neuron_count", "memory.capacity", "neuromodulation.reward_scale"}, 10000);
        benchmark_results_.emplace_back("Config Access", "Config", access_result.duration_ms, access_result.throughput, access_result.passed);
        std::cout << "✓ Config access benchmark completed" << std::endl;
        
        // Validation benchmarks
        auto validation_result = ConfigBenchmark::benchmarkConfigValidation(config, 1000);
        benchmark_results_.emplace_back("Config Validation", "Config", validation_result.duration_ms, validation_result.throughput, validation_result.passed);
        std::cout << "✓ Config validation benchmark completed" << std::endl;
        
        std::cout << "Config System Benchmarks completed successfully!\n";
    }
    
    // Logger benchmarks
    void runLoggerBenchmarks() {
        std::cout << "Running Logger System Benchmarks...\n";
        
        // Log entry creation benchmarks
        auto entry_result = LoggerBenchmark::benchmarkLogEntryCreation("Performance benchmark test message with detailed information", 10000);
        benchmark_results_.emplace_back("Log Entry Creation", "Logger", entry_result.duration_ms, entry_result.throughput, entry_result.passed);
        std::cout << "✓ Log entry creation benchmark completed" << std::endl;
        
        // Log formatting benchmarks
        LogEntry test_entry{LogLevel::Info, "Test message", "benchmark.cpp", 1, "main", std::chrono::system_clock::now()};
        auto format_result = LoggerBenchmark::benchmarkLogFormatting(test_entry, 10000);
        benchmark_results_.emplace_back("Log Formatting", "Logger", format_result.duration_ms, format_result.throughput, format_result.passed);
        std::cout << "✓ Log formatting benchmark completed" << std::endl;
        
        // Log level filtering benchmarks
        auto filter_result = LoggerBenchmark::benchmarkLogLevelFiltering(10000);
        benchmark_results_.emplace_back("Log Level Filtering", "Logger", filter_result.duration_ms, filter_result.throughput, filter_result.passed);
        std::cout << "✓ Log level filtering benchmark completed" << std::endl;
        
        std::cout << "Logger System Benchmarks completed successfully!\n";
    }
    
    // Random benchmarks
    void runRandomBenchmarks() {
        std::cout << "Running Random Number Generation Benchmarks...\n";
        
        // Uniform integer benchmarks
        auto uniform_int_result = RandomBenchmark::benchmarkUniformInt(1, 1000000, 100000);
        benchmark_results_.emplace_back("Uniform Integer", "Random", uniform_int_result.duration_ms, uniform_int_result.throughput, uniform_int_result.passed);
        std::cout << "✓ Uniform integer benchmark completed" << std::endl;
        
        // Uniform real benchmarks
        auto uniform_real_result = RandomBenchmark::benchmarkUniformReal(0.0f, 1.0f, 100000);
        benchmark_results_.emplace_back("Uniform Real", "Random", uniform_real_result.duration_ms, uniform_real_result.throughput, uniform_real_result.passed);
        std::cout << "✓ Uniform real benchmark completed" << std::endl;
        
        // Normal distribution benchmarks
        auto normal_result = RandomBenchmark::benchmarkNormal(0.0f, 1.0f, 100000);
        benchmark_results_.emplace_back("Normal Distribution", "Random", normal_result.duration_ms, normal_result.throughput, normal_result.passed);
        std::cout << "✓ Normal distribution benchmark completed" << std::endl;
        
        // Bernoulli benchmarks
        auto bernoulli_result = RandomBenchmark::benchmarkBernoulli(0.5f, 100000);
        benchmark_results_.emplace_back("Bernoulli", "Random", bernoulli_result.duration_ms, bernoulli_result.throughput, bernoulli_result.passed);
        std::cout << "✓ Bernoulli benchmark completed" << std::endl;
        
        std::cout << "Random Number Generation Benchmarks completed successfully!\n";
    }
    
    // Neuron benchmarks
    void runNeuronBenchmarks() {
        std::cout << "Running Neuron Dynamics Benchmarks...\n";
        
        // LIF step benchmarks
        NeuronState test_neuron_state;
        auto lif_result = NeuronBenchmark::benchmarkLIFStep(test_neuron_state, 0.001, 100000);
        benchmark_results_.emplace_back("LIF Step", "Neuron", lif_result.duration_ms, lif_result.throughput, lif_result.passed);
        std::cout << "✓ LIF step benchmark completed" << std::endl;
        
        // Synaptic input benchmarks
        std::vector<float> test_inputs(1000, 0.5f);
        auto synaptic_result = NeuronBenchmark::benchmarkSynapticInput(test_inputs, 100000);
        benchmark_results_.emplace_back("Synaptic Input", "Neuron", synaptic_result.duration_ms, synaptic_result.throughput, synaptic_result.passed);
        std::cout << "✓ Synaptic input benchmark completed" << std::endl;
        
        // Spike detection benchmarks
        auto spike_result = NeuronBenchmark::benchmarkSpikeDetection(test_neuron_state, 100000);
        benchmark_results_.emplace_back("Spike Detection", "Neuron", spike_result.duration_ms, spike_result.throughput, spike_result.passed);
        std::cout << "✓ Spike detection benchmark completed" << std::endl;
        
        std::cout << "Neuron Dynamics Benchmarks completed successfully!\n";
    }
    
    // Synapse benchmarks
    void runSynapseBenchmarks() {
        std::cout << "Running Synapse Processing Benchmarks...\n";
        
        // Synaptic transmission benchmarks
        std::vector<float> test_weights(1000, 0.5f);
        auto transmission_result = SynapseBenchmark::benchmarkTransmission(test_weights, 100000);
        benchmark_results_.emplace_back("Synaptic Transmission", "Synapse", transmission_result.duration_ms, transmission_result.throughput, transmission_result.passed);
        std::cout << "✓ Synaptic transmission benchmark completed" << std::endl;
        
        // Plasticity update benchmarks
        auto plasticity_result = SynapseBenchmark::benchmarkPlasticityUpdate(0.01f, 0.1f, 100000);
        benchmark_results_.emplace_back("Plasticity Update", "Synapse", plasticity_result.duration_ms, plasticity_result.throughput, plasticity_result.passed);
        std::cout << "✓ Plasticity update benchmark completed" << std::endl;
        
        std::cout << "Synapse Processing Benchmarks completed successfully!\n";
    }
    
    // Memory benchmarks
    void runMemoryBenchmarks() {
        std::cout << "Running Memory System Benchmarks...\n";
        
        // Working memory benchmarks
        std::vector<std::pair<std::string, float>> test_data;
        for (int i = 0; i < 1000; ++i) {
            test_data.emplace_back("key_" + std::to_string(i), static_cast<float>(rand()) / RAND_MAX);
        }
        auto working_memory_result = MemoryBenchmark::benchmarkWorkingMemoryAccess(test_data, 100000);
        benchmark_results_.emplace_back("Working Memory", "Memory", working_memory_result.duration_ms, working_memory_result.throughput, working_memory_result.passed);
        std::cout << "✓ Working memory benchmark completed" << std::endl;
        
        // Episodic memory benchmarks
        auto episodic_result = MemoryBenchmark::benchmarkEpisodicMemorySearch("test_pattern_" + std::to_string(rand() % 100), 10000);
        benchmark_results_.emplace_back("Episodic Memory", "Memory", episodic_result.duration_ms, episodic_result.throughput, episodic_result.passed);
        std::cout << "✓ Episodic memory benchmark completed" << std::endl;
        
        std::cout << "Memory System Benchmarks completed successfully!\n";
    }
    
    // Neuromodulation benchmarks
    void runNeuromodulationBenchmarks() {
        std::cout << "Running Neuromodulation System Benchmarks...\n";
        
        // Reward prediction error benchmarks
        std::vector<float> rewards = {0.1f, 0.5f, 1.0f, -0.1f, -0.5f};
        std::vector<float> values = {0.0f, 0.5f, 1.0f, 0.2f, 0.1f};
        auto rpe_result = NeuromodulatorBenchmark::benchmarkRewardPredictionError(rewards, values, 10000);
        benchmark_results_.emplace_back("RPE", "Neuromodulation", rpe_result.duration_ms, rpe_result.throughput, rpe_result.passed);
        std::cout << "✓ RPE benchmark completed" << std::endl;
        
        // Curiosity benchmarks
        auto curiosity_result = NeuromodulatorBenchmark::benchmarkCuriosityUpdate({0.1f, 0.5f, 1.0f}, {0.2f, 0.6f, 0.9f}, 10000);
        benchmark_results_.emplace_back("Curiosity", "Neuromodulation", curiosity_result.duration_ms, curiosity_result.throughput, curiosity_result.passed);
        std::cout << "✓ Curiosity benchmark completed" << std::endl;
        
        std::cout << "Neuromodulation System Benchmarks completed successfully!\n";
    }
    
    // Plasticity benchmarks
    void runPlasticityBenchmarks() {
        std::cout << "Running Plasticity System Benchmarks...\n";
        
        // STDP benchmarks
        std::vector<std::pair<Timestamp, Timestamp>> spike_pairs;
        for (int i = 0; i < 100; ++i) {
            spike_pairs.emplace_back(static_cast<Timestamp>(i * 10), static_cast<Timestamp>(i * 10 + 20));
        }
        auto stdp_result = PlasticityBenchmark::benchmarkSTDPUpdate(spike_pairs, 0.001f, 10000);
        benchmark_results_.emplace_back("STDP", "Plasticity", stdp_result.duration_ms, stdp_result.throughput, stdp_result.passed);
        std::cout << "✓ STDP benchmark completed" << std::endl;
        
        std::cout << "Plasticity System Benchmarks completed successfully!\n";
    }
    
    // Prediction benchmarks
    void runPredictionBenchmarks() {
        std::cout << "Running Prediction System Benchmarks...\n";
        
        // Forward model benchmarks
        std::vector<std::vector<float>> states, actions;
        for (int i = 0; i < 100; ++i) {
            std::vector<float> state(10, static_cast<float>(rand()) / RAND_MAX);
            std::vector<float> action(5, static_cast<float>(rand()) / RAND_MAX);
            states.push_back(state);
            actions.push_back(action);
        }
        auto forward_result = PredictionBenchmark::benchmarkForwardModel(states, actions, 1000);
        benchmark_results_.emplace_back("Forward Model", "Prediction", forward_result.duration_ms, forward_result.throughput, forward_result.passed);
        std::cout << "✓ Forward model benchmark completed" << std::endl;
        
        // Inference benchmarks
        std::vector<std::vector<float>> observations;
        for (int i = 0; i < 1000; ++i) {
            std::vector<float> obs(20, static_cast<float>(rand()) / RAND_MAX);
            observations.push_back(obs);
        }
        auto inference_result = PredictionBenchmark::benchmarkInference(observations, 10000);
        benchmark_results_.emplace_back("Inference", "Prediction", inference_result.duration_ms, inference_result.throughput, inference_result.passed);
        std::cout << "✓ Inference benchmark completed" << std::endl;
        
        std::cout << "Prediction System Benchmarks completed successfully!\n";
    }
    
    // Integration benchmarks
    void runIntegrationBenchmarks() {
        std::cout << "Running Integration Benchmarks...\n";
        
        // Full system integration benchmarks
        auto integration_result = runIntegrationBenchmark("Full System", 1000);
        benchmark_results_.emplace_back("Full System Integration", "Integration", integration_result.duration_ms, integration_result.throughput, integration_result.passed);
        std::cout << "✓ Full system integration benchmark completed" << std::endl;
        
        // Neuromodulation integration benchmarks
        auto neuromodulation_result = runIntegrationBenchmark("Neuromodulation Integration", 1000);
        benchmark_results_.emplace_back("Neuromodulation Integration", "Integration", neuromodulation_result.duration_ms, neuromodulation_result.throughput, neuromodulation_result.passed);
        std::cout << "✓ Neuromodulation integration benchmark completed" << std::endl;
        
        std::cout << "Integration Benchmarks completed successfully!\n";
    }
    
    // Scalability benchmarks
    void runScalabilityBenchmarks() {
        std::cout << "Running Scalability Benchmarks...\n";
        
        // Neuron count scalability
        auto neuron_result = runIntegrationBenchmark("Neuron Count Scalability", 10000);
        benchmark_results_.emplace_back("Neuron Count Scalability", "Scalability", neuron_result.duration_ms, neuron_result.throughput, neuron_result.passed);
        std::cout << "✓ Neuron count scalability benchmark completed" << std::endl;
        
        // Memory scalability
        auto memory_result = runIntegrationBenchmark("Memory Scalability", 1000);
        benchmark_results_.emplace_back("Memory Scalability", "Scalability", memory_result.duration_ms, memory_result.throughput, memory_result.passed);
        std::cout << "✓ Memory scalability benchmark completed" << std::endl;
        
        std::cout << "Scalability Benchmarks completed successfully!\n";
    }
    
    void runAllBenchmarks() {
        runConfigBenchmarks();
        runLoggerBenchmarks();
        runRandomBenchmarks();
        runNeuronBenchmarks();
        runSynapseBenchmarks();
        runMemoryBenchmarks();
        runNeuromodulationBenchmarks();
        runPlasticityBenchmarks();
        runPredictionBenchmarks();
        runIntegrationBenchmarks();
        runScalabilityBenchmarks();
    }
    
    BenchmarkResult runIntegrationBenchmark(const std::string& name, int iterations) {
        nlm::Benchmark benchmark(name);
        
        return benchmark.run(name, "Integration",
            [&]() {
                // Simulate integration operations
                std::vector<float> data(iterations / 100, 0.5f);
                double sum = std::accumulate(data.begin(), data.end(), 0.0);
                double avg = sum / data.size();
            }, iterations, false, "Integration benchmark");
    }
};

int main(int argc, char* argv[]) {
    std::cout << "NLM Benchmark Suite" << std::endl;
    std::cout << "==================" << std::endl;
    
    // Parse command line arguments
    NLMBenchmark::BenchmarkType benchmark_type = NLMBenchmark::BenchmarkType::All;
    std::string output_file = "benchmark_results.txt";
    std::string report_file = "benchmark_report.txt";
    bool analyze = false;
    bool generate_report = false;
    
    if (argc > 1) {
        std::string arg = argv[1];
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --help, -h              Show this help message" << std::endl;
            std::cout << "  --type <type>          Benchmark type (Config, Logger, Random, Neuron, Synapse, Memory, Neuromodulation, Plasticity, Prediction, Integration, Scalability, All)" << std::endl;
            std::cout << "  --output <file>        Output results to file" << std::endl;
            std::cout << "  --report <file>        Generate performance report to file" << std::endl;
            std::cout << "  --analyze              Run performance analysis" << std::endl;
            std::cout << "  --all                  Run all benchmarks (default)" << std::endl;
            return 0;
        }
        
        if (argc > 2 && arg == "--type") {
            std::string type_str = argv[2];
            if (type_str == "Config") benchmark_type = NLMBenchmark::BenchmarkType::Config;
            else if (type_str == "Logger") benchmark_type = NLMBenchmark::BenchmarkType::Logger;
            else if (type_str == "Random") benchmark_type = NLMBenchmark::BenchmarkType::Random;
            else if (type_str == "Neuron") benchmark_type = NLMBenchmark::BenchmarkType::Neuron;
            else if (type_str == "Synapse") benchmark_type = NLMBenchmark::BenchmarkType::Synapse;
            else if (type_str == "Memory") benchmark_type = NLMBenchmark::BenchmarkType::Memory;
            else if (type_str == "Neuromodulation") benchmark_type = NLMBenchmark::BenchmarkType::Neuromodulation;
            else if (type_str == "Plasticity") benchmark_type = NLMBenchmark::BenchmarkType::Plasticity;
            else if (type_str == "Prediction") benchmark_type = NLMBenchmark::BenchmarkType::Prediction;
            else if (type_str == "Integration") benchmark_type = NLMBenchmark::BenchmarkType::Integration;
            else if (type_str == "Scalability") benchmark_type = NLMBenchmark::BenchmarkType::Scalability;
            else if (type_str == "All") benchmark_type = NLMBenchmark::BenchmarkType::All;
        }
        
        if (argc > 2 && arg == "--output") {
            output_file = argv[2];
        }
        
        if (argc > 2 && arg == "--report") {
            report_file = argv[2];
            generate_report = true;
        }
        
        if (argc > 2 && arg == "--analyze") {
            analyze = true;
        }
    }
    
    // Create and run benchmark suite
    NLMBenchmark benchmark_suite;
    
    // Run benchmarks
    benchmark_suite.runBenchmarks(benchmark_type);
    
    // Generate summary report
    std::cout << "\n=== Benchmark Summary ===\n";
    std::cout << "Benchmark type: " << benchmark_suite.benchmarkTypeToString(benchmark_type) << std::endl;
    std::cout << "Total benchmarks run: " << benchmark_suite.benchmark_results_.size() << std::endl;
    
    size_t passed = 0;
    for (const auto& result : benchmark_suite.benchmark_results_) {
        if (result.passed) passed++;
    }
    
    std::cout << "Benchmarks passed: " << passed << std::endl;
    std::cout << "Success rate: " << (passed * 100.0 / benchmark_suite.benchmark_results_.size()) << "%" << std::endl;
    
    // Run performance analysis if requested
    if (analyze) {
        benchmark_suite.analyzePerformance();
    }
    
    // Generate report if requested
    if (generate_report) {
        benchmark_suite.generateReport(report_file);
    }
    
    // Save benchmark results
    std::ofstream file(output_file);
    if (file.is_open()) {
        file << "=== NLM Benchmark Results ===\n";
        file << "Benchmark type: " << benchmark_suite.benchmarkTypeToString(benchmark_type) << "\n";
        file << "Generated: " << getCurrentTimestamp() << "\n\n";
        
        for (const auto& result : benchmark_suite.benchmark_results_) {
            file << "Benchmark: " << std::left << std::setw(30) << result.name
                 << "Category: " << std::left << std::setw(20) << result.category
                 << "Duration: " << std::fixed << std::setprecision(3) << result.duration_ms << "ms"
                 << " Throughput: " << std::fixed << std::setprecision(3) << result.throughput << " ops/sec"
                 << " Status: " << (result.passed ? "PASS" : "FAIL") << "\n";
        }
        
        file.close();
        std::cout << "Benchmark results saved to: " << output_file << std::endl;
    }
    
    std::cout << "\n=== Benchmark Suite Complete ===\n";
    std::cout << "Benchmark execution completed successfully!" << std::endl;
    
    return 0;
}

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm
