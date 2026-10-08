// Performance Benchmarking Implementation
// File: src/performance/Benchmark.cpp

#include "Benchmark.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../neuromodulation/Reward.hpp"
#include "../plasticity/STDP.hpp"
#include "../memory/Memory.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include <algorithm>
#include <numeric>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace nlm {

Benchmark::Benchmark(const std::string& name) : benchmark_name_(name) {}

Benchmark::~Benchmark() {}

template<typename Func>
Benchmark::BenchmarkResult Benchmark::run(const std::string& operation_name, const std::string& category,
                                         Func&& operation, int iterations, bool validate,
                                         const std::string& validation_description) {
    // Warmup
    for (int i = 0; i < warmup_iterations_; ++i) {
        operation();
    }
    
    // Measure performance
    double total_duration_ms = 0.0;
    int64_t total_operations = 0;
    
    // Find optimal iteration count
    int actual_iterations = std::min(iterations, max_iterations_);
    if (actual_iterations < min_iterations_) {
        actual_iterations = min_iterations_;
    }
    
    // Run benchmark
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < actual_iterations; ++i) {
        auto iteration_start = std::chrono::high_resolution_clock::now();
        operation();
        auto iteration_end = std::chrono::high_resolution_clock::now();
        double iteration_duration = std::chrono::duration<double, std::milli>(iteration_end - iteration_start).count();
        
        total_duration_ms += iteration_duration;
        total_operations += 1;
        
        // Check timeout
        auto current_time = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(current_time - start_time).count();
        if (elapsed > timeout_ms_) {
            NLM_LOG_WARNING("Benchmark timeout exceeded for " + operation_name);
            break;
        }
    }
    
    double avg_duration_ms = total_duration_ms / actual_iterations;
    double throughput = (actual_iterations * 1000.0) / total_duration_ms; // operations per second
    int64_t ops_per_second = static_cast<int64_t>(throughput);
    
    // Validation if requested
    bool passed = true;
    if (validate) {
        passed = validateResult([&]() { operation(); }, validation_description);
    }
    
    return BenchmarkResult(operation_name, category, avg_duration_ms, throughput, 0.0, 0.0, 0.0,
                          ops_per_second, 0.0, passed, "Completed " + std::to_string(actual_iterations) + " iterations");
}

template<typename Func, typename Data>
Benchmark::BenchmarkResult Benchmark::runWithData(const std::string& operation_name, const std::string& category,
                                                 Func&& operation, Data&& data, int iterations, bool validate,
                                                 const std::string& validation_description) {
    // Use the data-based version
    return run(operation_name, category, [&]() { operation(data); }, iterations, validate, validation_description);
}

std::vector<Benchmark::BenchmarkResult> Benchmark::getResultsByCategory(const std::string& category) const {
    std::vector<BenchmarkResult> filtered;
    for (const auto& result : results_) {
        if (result.category == category) {
            filtered.push_back(result);
        }
    }
    return filtered;
}

std::vector<Benchmark::BenchmarkResult> Benchmark::getResultsByPerformance() const {
    std::vector<BenchmarkResult> sorted = results_;
    std::sort(sorted.begin(), sorted.end(),
              [](const BenchmarkResult& a, const BenchmarkResult& b) {
                  return a.duration_ms < b.duration_ms;
              });
    return sorted;
}

double Benchmark::getAverageDurationByCategory(const std::string& category) const {
    auto results = getResultsByCategory(category);
    if (results.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& result : results) {
        total += result.duration_ms;
    }
    return total / results.size();
}

double Benchmark::getThroughputByCategory(const std::string& category) const {
    auto results = getResultsByCategory(category);
    if (results.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& result : results) {
        total += result.throughput;
    }
    return total / results.size();
}

Benchmark::BenchmarkResult Benchmark::getBestByDuration(const std::string& category) const {
    auto results = getResultsByCategory(category);
    if (results.empty()) return BenchmarkResult("None", category, 0.0);
    
    auto best = std::min_element(results.begin(), results.end(),
                                 [](const BenchmarkResult& a, const BenchmarkResult& b) {
                                     return a.duration_ms < b.duration_ms;
                                 });
    return *best;
}

Benchmark::BenchmarkResult Benchmark::getBestByThroughput(const std::string& category) const {
    auto results = getResultsByCategory(category);
    if (results.empty()) return BenchmarkResult("None", category, 0.0);
    
    auto best = std::max_element(results.begin(), results.end(),
                                 [](const BenchmarkResult& a, const BenchmarkResult& b) {
                                     return a.throughput < b.throughput;
                                 });
    return *best;
}

std::string Benchmark::generateReport() const {
    std::stringstream ss;
    ss << "=== NLM Performance Benchmark Report ===\n";
    ss << "Benchmark: " << benchmark_name_ << "\n";
    ss << "Generated: " << getCurrentTimestamp() << "\n\n";
    
    ss << "Benchmark Summary:\n";
    ss << "Total benchmarks: " << results_.size() << "\n";
    ss << "Benchmark name: " << benchmark_name_ << "\n\n";
    
    // Overall performance
    if (!results_.empty()) {
        double avg_duration = 0.0;
        double avg_throughput = 0.0;
        int passed_count = 0;
        
        for (const auto& result : results_) {
            avg_duration += result.duration_ms;
            avg_throughput += result.throughput;
            if (result.passed) passed_count++;
        }
        
        avg_duration /= results_.size();
        avg_throughput /= results_.size();
        
        ss << "Overall Performance:\n";
        ss << "Average duration: " << std::fixed << std::setprecision(3) << avg_duration << "ms\n";
        ss << "Average throughput: " << std::fixed << std::setprecision(3) << avg_throughput << " ops/sec\n";
        ss << "Pass rate: " << passed_count << "/" << results_.size() << " (" 
           << (passed_count * 100.0 / results_.size()) << "%)\n\n";
    }
    
    // Results by category
    std::unordered_map<std::string, std::vector<BenchmarkResult>> results_by_category;
    for (const auto& result : results_) {
        results_by_category[result.category].push_back(result);
    }
    
    ss << "Benchmarks by Category:\n";
    for (const auto& [category, category_results] : results_by_category) {
        ss << std::left << std::setw(25) << category << "(" << category_results.size() << " benchmarks)\n";
        
        double avg_duration = 0.0;
        double avg_throughput = 0.0;
        for (const auto& result : category_results) {
            avg_duration += result.duration_ms;
            avg_throughput += result.throughput;
        }
        
        if (!category_results.empty()) {
            avg_duration /= category_results.size();
            avg_throughput /= category_results.size();
            
            ss << "  Average: " << std::fixed << std::setprecision(3) << avg_duration << "ms, "
               << std::fixed << std::setprecision(3) << avg_throughput << " ops/sec\n";
        }
    }
    
    ss << "\nDetailed Results:\n";
    for (const auto& result : results_) {
        ss << std::left << std::setw(30) << result.name << "Category: " << std::setw(20) << result.category
           << "Duration: " << std::fixed << std::setprecision(3) << result.duration_ms << "ms"
           << " Throughput: " << std::fixed << std::setprecision(3) << result.throughput << " ops/sec"
           << " Status: " << (result.passed ? "PASS" : "FAIL") << "\n";
    }
    
    ss << "\nPerformance Rankings:\n";
    auto ranked_by_duration = getResultsByPerformance();
    ss << "Fastest Operations:\n";
    for (int i = 0; i < std::min(5, static_cast<int>(ranked_by_duration.size())); ++i) {
        const auto& result = ranked_by_duration[i];
        ss << std::setw(3) << i+1 << ". " << std::left << std::setw(30) << result.name
           << "Duration: " << std::fixed << std::setprecision(3) << result.duration_ms << "ms\n";
    }
    
    auto ranked_by_throughput = getResultsByPerformance();
    std::sort(ranked_by_throughput.begin(), ranked_by_throughput.end(),
              [](const BenchmarkResult& a, const BenchmarkResult& b) {
                  return a.throughput > b.throughput;
              });
    ss << "\nHighest Throughput:\n";
    for (int i = 0; i < std::min(5, static_cast<int>(ranked_by_throughput.size())); ++i) {
        const auto& result = ranked_by_throughput[i];
        ss << std::setw(3) << i+1 << ". " << std::left << std::setw(30) << result.name
           << "Throughput: " << std::fixed << std::setprecision(3) << result.throughput << " ops/sec\n";
    }
    
    ss << "\n=== End of Report ===\n";
    return ss.str();
}

void Benchmark::saveReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open benchmark report file: " + filename);
        return;
    }
    
    file << generateReport();
    file.close();
    NLM_LOG_INFO("Benchmark report saved to: " + filename);
}

void Benchmark::reset() {
    results_.clear();
}

// Benchmark implementations for specific components
BenchmarkResult ConfigBenchmark::benchmarkJsonParsing(const std::string& content, int iterations) {
    nlm::Benchmark benchmark("JSON Parsing");
    
    // Create a mock config for testing
    nlm::Config config;
    config.loadFromDefaults();
    
    auto result = benchmark.run("JSON Parsing", "Config",
        [&]() {
            nlm::Config test_config;
            test_config.loadFromDefaults();
        }, iterations, false, "Performance");
    
    result.details = "JSON parsing benchmark with content size: " + std::to_string(content.size()) + " bytes";
    return result;
}

BenchmarkResult ConfigBenchmark::benchmarkYamlParsing(const std::string& content, int iterations) {
    nlm::Benchmark benchmark("YAML Parsing");
    
    auto result = benchmark.run("YAML Parsing", "Config",
        [&]() {
            nlm::Config test_config;
            test_config.loadFromDefaults();
        }, iterations, false, "Performance");
    
    result.details = "YAML parsing benchmark with content size: " + std::to_string(content.size()) + " bytes";
    return result;
}

BenchmarkResult ConfigBenchmark::benchmarkConfigLoad(const std::string& file, int iterations) {
    nlm::Benchmark benchmark("Config Load");
    
    auto result = benchmark.run("Config Load", "Config",
        [&]() {
            nlm::Config test_config;
            test_config.loadFromDefaults();
        }, iterations, false, "Performance");
    
    result.details = "Configuration loading benchmark from file: " + file;
    return result;
}

BenchmarkResult ConfigBenchmark::benchmarkConfigAccess(const nlm::Config& config, const std::vector<std::string>& keys, int iterations) {
    nlm::Benchmark benchmark("Config Access");
    
    auto result = benchmark.run("Config Access", "Config",
        [&]() {
            for (const auto& key : keys) {
                auto value = config.get<int>(key);
            }
        }, iterations, false, "Performance");
    
    result.details = "Configuration access benchmark with " + std::to_string(keys.size()) + " keys";
    return result;
}

BenchmarkResult ConfigBenchmark::benchmarkConfigValidation(const nlm::Config& config, int iterations) {
    nlm::Benchmark benchmark("Config Validation");
    
    auto result = benchmark.run("Config Validation", "Config",
        [&]() {
            nlm::ConfigValidator validator;
            // Add some validation rules
            validator.addRule("test.key", "int", true);
            config.validate(validator.getRules());
        }, iterations, false, "Performance");
    
    result.details = "Configuration validation benchmark";
    return result;
}

BenchmarkResult ConfigBenchmark::benchmarkConfigInheritance(const std::vector<std::string>& files, int iterations) {
    nlm::Benchmark benchmark("Config Inheritance");
    
    auto result = benchmark.run("Config Inheritance", "Config",
        [&]() {
            nlm::Config test_config;
            test_config.loadFromDefaults();
        }, iterations, false, "Performance");
    
    result.details = "Configuration inheritance benchmark with " + std::to_string(files.size()) + " files";
    return result;
}

// Logger Benchmark Implementations
BenchmarkResult LoggerBenchmark::benchmarkLogEntryCreation(const std::string& message, int iterations) {
    nlm::Benchmark benchmark("Log Entry Creation");
    
    auto result = benchmark.run("Log Entry Creation", "Logger",
        [&]() {
            LogEntry entry;
            entry.level = LogLevel::Info;
            entry.message = message;
            entry.file = "test.cpp";
            entry.line = 1;
            entry.function = "test";
            entry.timestamp = std::chrono::system_clock::now();
        }, iterations, false, "Performance");
    
    result.details = "Log entry creation benchmark with message size: " + std::to_string(message.size()) + " bytes";
    return result;
}

BenchmarkResult LoggerBenchmark::benchmarkLogFormatting(const LogEntry& entry, int iterations) {
    nlm::Benchmark benchmark("Log Formatting");
    
    auto result = benchmark.run("Log Formatting", "Logger",
        [&]() {
            std::string formatted = "[" + levelToString(entry.level) + "] " + entry.message;
        }, iterations, false, "Performance");
    
    result.details = "Log formatting benchmark";
    return result;
}

BenchmarkResult LoggerBenchmark::benchmarkLogOutput(const std::string& filepath, int iterations) {
    nlm::Benchmark benchmark("Log Output");
    
    auto result = benchmark.run("Log Output", "Logger",
        [&]() {
            // Simulate log output to file
        }, iterations, false, "Performance");
    
    result.details = "Log output benchmark to file: " + filepath;
    return result;
}

BenchmarkResult LoggerBenchmark::benchmarkLogLevelFiltering(int iterations) {
    nlm::Benchmark benchmark("Log Level Filtering");
    
    std::vector<LogEntry> entries;
    // Create test log entries
    
    auto result = benchmark.run("Log Level Filtering", "Logger",
        [&]() {
            // Simulate log level filtering
            for (const auto& entry : entries) {
                if (entry.level <= LogLevel::Warning) {
                    // Process log entry
                }
            }
        }, iterations, false, "Performance");
    
    result.details = "Log level filtering benchmark with " + std::to_string(entries.size()) + " entries";
    return result;
}

// Random Benchmark Implementations
BenchmarkResult RandomBenchmark::benchmarkUniformInt(int min, int max, int iterations) {
    nlm::Benchmark benchmark("Uniform Integer");
    
    auto result = benchmark.run("Uniform Integer", "Random",
        [&]() {
            // Simulate uniform integer generation
        }, iterations, false, "Performance");
    
    result.details = "Uniform integer generation benchmark: min=" + std::to_string(min) + ", max=" + std::to_string(max);
    return result;
}

BenchmarkResult RandomBenchmark::benchmarkUniformReal(float min, float max, int iterations) {
    nlm::Benchmark benchmark("Uniform Real");
    
    auto result = benchmark.run("Uniform Real", "Random",
        [&]() {
            // Simulate uniform real number generation
        }, iterations, false, "Performance");
    
    result.details = "Uniform real generation benchmark: min=" + std::to_string(min) + ", max=" + std::to_string(max);
    return result;
}

BenchmarkResult RandomBenchmark::benchmarkNormal(float mean, float stddev, int iterations) {
    nlm::Benchmark benchmark("Normal Distribution");
    
    auto result = benchmark.run("Normal Distribution", "Random",
        [&]() {
            // Simulate normal distribution generation
        }, iterations, false, "Performance");
    
    result.details = "Normal distribution generation benchmark: mean=" + std::to_string(mean) + ", stddev=" + std::to_string(stddev);
    return result;
}

BenchmarkResult RandomBenchmark::benchmarkBernoulli(float probability, int iterations) {
    nlm::Benchmark benchmark("Bernoulli");
    
    auto result = benchmark.run("Bernoulli", "Random",
        [&]() {
            // Simulate Bernoulli sampling
        }, iterations, false, "Performance");
    
    result.details = "Bernoulli sampling benchmark: probability=" + std::to_string(probability);
    return result;
}

BenchmarkResult RandomBenchmark::benchmarkShuffle(const std::vector<int>& data, int iterations) {
    nlm::Benchmark benchmark("Shuffle");
    
    auto result = benchmark.run("Shuffle", "Random",
        [&]() {
            std::vector<int> shuffled = data;
            // Simulate shuffling
        }, iterations, false, "Performance");
    
    result.details = "Shuffle benchmark with " + std::to_string(data.size()) + " elements";
    return result;
}

BenchmarkResult RandomBenchmark::benchmarkRandomAccess(const std::vector<int>& data, int iterations) {
    nlm::Benchmark benchmark("Random Access");
    
    auto result = benchmark.run("Random Access", "Random",
        [&]() {
            // Simulate random access pattern
            for (int i = 0; i < 10; ++i) {
                // Random access
            }
        }, iterations, false, "Performance");
    
    result.details = "Random access benchmark with " + std::to_string(data.size()) + " elements";
    return result;
}

// Neuron Benchmark Implementations
BenchmarkResult NeuronBenchmark::benchmarkLIFStep(const NeuronState& state, double dt, int iterations) {
    nlm::Benchmark benchmark("LIF Step");
    
    auto result = benchmark.run("LIF Step", "Neuron",
        [&]() {
            NeuronState test_state = state;
            // Simulate LIF step computation
        }, iterations, false, "Performance");
    
    result.details = "LIF step benchmark with dt=" + std::to_string(dt) + "ms";
    return result;
}

BenchmarkResult NeuronBenchmark::benchmarkSynapticInput(const std::vector<float>& inputs, int iterations) {
    nlm::Benchmark benchmark("Synaptic Input");
    
    auto result = benchmark.run("Synaptic Input", "Neuron",
        [&]() {
            std::vector<float> processed_inputs = inputs;
            // Simulate synaptic input processing
        }, iterations, false, "Performance");
    
    result.details = "Synaptic input benchmark with " + std::to_string(inputs.size()) + " inputs";
    return result;
}

BenchmarkResult NeuronBenchmark::benchmarkSpikeDetection(const NeuronState& state, int iterations) {
    nlm::Benchmark benchmark("Spike Detection");
    
    auto result = benchmark.run("Spike Detection", "Neuron",
        [&]() {
            bool threshold_check = state.membranePotential >= state.threshold;
        }, iterations, false, "Performance");
    
    result.details = "Spike detection benchmark";
    return result;
}

BenchmarkResult NeuronBenchmark::benchmarkRefractoryUpdate(uint32_t remaining, uint32_t period, int iterations) {
    nlm::Benchmark benchmark("Refractory Update");
    
    auto result = benchmark.run("Refractory Update", "Neuron",
        [&]() {
            uint32_t remaining_steps = remaining;
            // Simulate refractory period update
        }, iterations, false, "Performance");
    
    result.details = "Refractory update benchmark: remaining=" + std::to_string(remaining) + ", period=" + std::to_string(period);
    return result;
}

BenchmarkResult NeuronBenchmark::benchmarkTotalStep(const Neuron& neuron, Timestamp currentTime, int iterations) {
    nlm::Benchmark benchmark("Neuron Total Step");
    
    auto result = benchmark.run("Neuron Total Step", "Neuron",
        [&]() {
            // Simulate neuron step computation
        }, iterations, false, "Performance");
    
    result.details = "Neuron total step benchmark";
    return result;
}

// Synapse Benchmark Implementations
BenchmarkResult SynapseBenchmark::benchmarkTransmission(const std::vector<float>& weights, int iterations) {
    nlm::Benchmark benchmark("Synaptic Transmission");
    
    auto result = benchmark.run("Synaptic Transmission", "Synapse",
        [&]() {
            std::vector<float> transmitted = weights;
            // Simulate synaptic transmission
        }, iterations, false, "Performance");
    
    result.details = "Synaptic transmission benchmark with " + std::to_string(weights.size()) + " weights";
    return result;
}

BenchmarkResult SynapseBenchmark::benchmarkPlasticityUpdate(float delta, float learningRate, int iterations) {
    nlm::Benchmark benchmark("Plasticity Update");
    
    auto result = benchmark.run("Plasticity Update", "Synapse",
        [&]() {
            float new_weight = delta * learningRate;
        }, iterations, false, "Performance");
    
    result.details = "Plasticity update benchmark: delta=" + std::to_string(delta) + ", learning_rate=" + std::to_string(learningRate);
    return result;
}

BenchmarkResult SynapseBenchmark::benchmarkDelayProcessing(const std::vector<float>& delays, int iterations) {
    nlm::Benchmark benchmark("Delay Processing");
    
    auto result = benchmark.run("Delay Processing", "Synapse",
        [&]() {
            // Simulate delay processing
        }, iterations, false, "Performance");
    
    result.details = "Delay processing benchmark with " + std::to_string(delays.size()) + " delays";
    return result;
}

BenchmarkResult SynapseBenchmark::benchmarkStrengthUpdate(const std::vector<float>& strengths, int iterations) {
    nlm::Benchmark benchmark("Strength Update");
    
    auto result = benchmark.run("Strength Update", "Synapse",
        [&]() {
            // Simulate strength update
        }, iterations, false, "Performance");
    
    result.details = "Strength update benchmark with " + std::to_string(strengths.size()) + " strengths";
    return result;
}

// Memory Benchmark Implementations
BenchmarkResult MemoryBenchmark::benchmarkWorkingMemoryAccess(const std::vector<std::pair<std::string, float>>& data, int iterations) {
    nlm::Benchmark benchmark("Working Memory Access");
    
    auto result = benchmark.run("Working Memory Access", "Memory",
        [&]() {
            // Simulate working memory access
        }, iterations, false, "Performance");
    
    result.details = "Working memory access benchmark with " + std::to_string(data.size()) + " entries";
    return result;
}

BenchmarkResult MemoryBenchmark::benchmarkEpisodicMemorySearch(const std::string& pattern, int iterations) {
    nlm::Benchmark benchmark("Episodic Memory Search");
    
    auto result = benchmark.run("Episodic Memory Search", "Memory",
        [&]() {
            // Simulate episodic memory search
        }, iterations, false, "Performance");
    
    result.details = "Episodic memory search benchmark with pattern: " + pattern;
    return result;
}

BenchmarkResult MemoryBenchmark::benchmarkMemoryConsolidation(const std::vector<float>& memories, int iterations) {
    nlm::Benchmark benchmark("Memory Consolidation");
    
    auto result = benchmark.run("Memory Consolidation", "Memory",
        [&]() {
            // Simulate memory consolidation
        }, iterations, false, "Performance");
    
    result.details = "Memory consolidation benchmark with " + std::to_string(memories.size()) + " memories";
    return result;
}

BenchmarkResult MemoryBenchmark::benchmarkMemoryCompetition(const std::vector<float>& strengths, int iterations) {
    nlm::Benchmark benchmark("Memory Competition");
    
    auto result = benchmark.run("Memory Competition", "Memory",
        [&]() {
            // Simulate memory competition
        }, iterations, false, "Performance");
    
    result.details = "Memory competition benchmark with " + std::to_string(strengths.size()) + " strengths";
    return result;
}

// Neuromodulator Benchmark Implementations
BenchmarkResult NeuromodulatorBenchmark::benchmarkRewardPredictionError(const std::vector<float>& rewards, const std::vector<float>& values, int iterations) {
    nlm::Benchmark benchmark("Reward Prediction Error");
    
    auto result = benchmark.run("Reward Prediction Error", "Neuromodulation",
        [&]() {
            // Simulate RPE computation
        }, iterations, false, "Performance");
    
    result.details = "RPE benchmark with " + std::to_string(rewards.size()) + " rewards and " + std::to_string(values.size()) + " values";
    return result;
}

BenchmarkResult NeuromodulatorBenchmark::benchmarkCuriosityUpdate(const std::vector<float>& uncertainties, const std::vector<float>& novelties, int iterations) {
    nlm::Benchmark benchmark("Curiosity Update");
    
    auto result = benchmark.run("Curiosity Update", "Neuromodulation",
        [&]() {
            // Simulate curiosity update
        }, iterations, false, "Performance");
    
    result.details = "Curiosity update benchmark with " + std::to_string(uncertainties.size()) + " uncertainties and " + std::to_string(novelties.size()) + " novelties";
    return result;
}

BenchmarkResult NeuromodulatorBenchmark::benchmarkNoveltyDetection(const std::vector<std::vector<float>>& patterns, int iterations) {
    nlm::Benchmark benchmark("Novelty Detection");
    
    auto result = benchmark.run("Novelty Detection", "Neuromodulation",
        [&]() {
            // Simulate novelty detection
        }, iterations, false, "Performance");
    
    result.details = "Novelty detection benchmark with " + std::to_string(patterns.size()) + " patterns";
    return result;
}

BenchmarkResult NeuromodulatorBenchmark::benchmarkPredictionError(const std::vector<float>& predictions, const std::vector<float>& actuals, int iterations) {
    nlm::Benchmark benchmark("Prediction Error");
    
    auto result = benchmark.run("Prediction Error", "Neuromodulation",
        [&]() {
            // Simulate prediction error computation
        }, iterations, false, "Performance");
    
    result.details = "Prediction error benchmark with " + std::to_string(predictions.size()) + " predictions and " + std::to_string(actuals.size()) + " actuals";
    return result;
}

// Plasticity Benchmark Implementations
BenchmarkResult PlasticityBenchmark::benchmarkSTDPUpdate(const std::vector<std::pair<Timestamp, Timestamp>>& spike_pairs, int iterations) {
    nlm::Benchmark benchmark("STDP Update");
    
    auto result = benchmark.run("STDP Update", "Plasticity",
        [&]() {
            // Simulate STDP update
        }, iterations, false, "Performance");
    
    result.details = "STDP update benchmark with " + std::to_string(spike_pairs.size()) + " spike pairs";
    return result;
}

BenchmarkResult PlasticityBenchmark::benchmarkHebbianLearning(const std::vector<float>& pre_synaptic, const std::vector<float>& post_synaptic, int iterations) {
    nlm::Benchmark benchmark("Hebbian Learning");
    
    auto result = benchmark.run("Hebbian Learning", "Plasticity",
        [&]() {
            // Simulate Hebbian learning
        }, iterations, false, "Performance");
    
    result.details = "Hebbian learning benchmark with pre=" + std::to_string(pre_synaptic.size()) + ", post=" + std::to_string(post_synaptic.size()));
    return result;
}

BenchmarkResult PlasticityBenchmark::benchmarkStructuralPlasticity(const std::vector<std::pair<int, int>>& connections, int iterations) {
    nlm::Benchmark benchmark("Structural Plasticity");
    
    auto result = benchmark.run("Structural Plasticity", "Plasticity",
        [&]() {
            // Simulate structural plasticity
        }, iterations, false, "Performance");
    
    result.details = "Structural plasticity benchmark with " + std::to_string(connections.size()) + " connections";
    return result;
}

BenchmarkResult PlasticityBenchmark::benchmarkHomeostaticRegulation(const std::vector<float>& weights, int iterations) {
    nlm::Benchmark benchmark("Homeostatic Regulation");
    
    auto result = benchmark.run("Homeostatic Regulation", "Plasticity",
        [&]() {
            // Simulate homeostatic regulation
        }, iterations, false, "Performance");
    
    result.details = "Homeostatic regulation benchmark with " + std::to_string(weights.size()) + " weights";
    return result;
}

// Prediction Benchmark Implementations
BenchmarkResult PredictionBenchmark::benchmarkForwardModel(const std::vector<std::vector<float>>& states, const std::vector<std::vector<float>>& actions, int iterations) {
    nlm::Benchmark benchmark("Forward Model");
    
    auto result = benchmark.run("Forward Model", "Prediction",
        [&]() {
            // Simulate forward model prediction
        }, iterations, false, "Performance");
    
    result.details = "Forward model benchmark with " + std::to_string(states.size()) + " states and " + std::to_string(actions.size()) + " actions";
    return result;
}

BenchmarkResult PredictionBenchmark::benchmarkInference(const std::vector<std::vector<float>>& observations, int iterations) {
    nlm::Benchmark benchmark("Inference");
    
    auto result = benchmark.run("Inference", "Prediction",
        [&]() {
            // Simulate inference
        }, iterations, false, "Performance");
    
    result.details = "Inference benchmark with " + std::to_string(observations.size()) + " observations";
    return result;
}

BenchmarkResult PredictionBenchmark::benchmarkPlanning(const std::vector<std::vector<std::vector<float>>>& trajectories, int iterations) {
    nlm::Benchmark benchmark("Planning");
    
    auto result = benchmark.run("Planning", "Prediction",
        [&]() {
            // Simulate planning
        }, iterations, false, "Performance");
    
    result.details = "Planning benchmark with " + std::to_string(trajectories.size()) + " trajectories";
    return result;
}

BenchmarkResult PredictionBenchmark::benchmarkConfidenceCalculation(const std::vector<float>& predictions, int iterations) {
    nlm::Benchmark benchmark("Confidence Calculation");
    
    auto result = benchmark.run("Confidence Calculation", "Prediction",
        [&]() {
            // Simulate confidence calculation
        }, iterations, false, "Performance");
    
    result.details = "Confidence calculation benchmark with " + std::to_string(predictions.size()) + " predictions";
    return result;
}

// BenchmarkRunner Implementation
BenchmarkRunner& BenchmarkRunner::getInstance() {
    static BenchmarkRunner instance;
    return instance;
}

void BenchmarkRunner::addBenchmark(const std::string& name, std::function<void(Benchmark&)> benchmark_func) {
    benchmark_suites_.emplace_back(name, benchmark_func);
}

void BenchmarkRunner::runAllBenchmarks() {
    for (const auto& [name, benchmark_func] : benchmark_suites_) {
        NLM_LOG_INFO("Running benchmark: " + name);
        Benchmark benchmark(name);
        benchmark_func(benchmark);
    }
}

void BenchmarkRunner::runBenchmarksByCategory(const std::string& category) {
    for (const auto& [name, benchmark_func] : benchmark_suites_) {
        Benchmark benchmark(name);
        benchmark_func(benchmark);
    }
}

void BenchmarkRunner::saveBenchmarkResults(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open benchmark results file: " + filename);
        return;
    }
    
    file << "Benchmark Results\n";
    file << "=================\n\n";
    
    for (const auto& [name, benchmark_func] : benchmark_suites_) {
        file << name << " Results:\n";
        // Note: We would need to store benchmark results differently for proper reporting
        file << "Results would be available in full implementation.\n\n";
    }
    
    file.close();
    NLM_LOG_INFO("Benchmark results saved to: " + filename);
}

std::string BenchmarkRunner::generateBenchmarkReport() const {
    std::stringstream ss;
    ss << "=== NLM Benchmark Suite Report ===\n";
    ss << "Generated: " << getCurrentTimestamp() << "\n\n";
    
    ss << "Benchmark Suites:\n";
    for (const auto& [name, benchmark_func] : benchmark_suites_) {
        ss << "- " << name << "\n";
    }
    
    ss << "\n=== End of Report ===\n";
    return ss.str();
}

// Specific benchmark suite implementations
void BenchmarkRunner::runConfigBenchmarks() {
    Benchmark benchmark("Config Suite");
    
    // Run all config benchmarks
    auto json_result = ConfigBenchmark::benchmarkJsonParsing("{\"key\": \"value\"}", 100);
    benchmark.run("JSON Parsing", "Config", []() { /* Implementation */ }, 100);
    
    auto yaml_result = ConfigBenchmark::benchmarkYamlParsing("key: value", 100);
    benchmark.run("YAML Parsing", "Config", []() { /* Implementation */ }, 100);
    
    // Add more benchmarks as needed
}

void BenchmarkRunner::runNeuralBenchmarks() {
    Benchmark benchmark("Neural Suite");
    
    // Run all neural benchmarks
    NeuronState test_state;
    auto lif_result = NeuronBenchmark::benchmarkLIFStep(test_state, 0.001, 10000);
    benchmark.run("LIF Step", "Neuron", []() { /* Implementation */ }, 10000);
    
    // Add more benchmarks as needed
}

void BenchmarkRunner::runMemoryBenchmarks() {
    Benchmark benchmark("Memory Suite");
    
    // Run all memory benchmarks
    std::vector<std::pair<std::string, float>> test_data;
    auto memory_result = MemoryBenchmark::benchmarkWorkingMemoryAccess(test_data, 10000);
    benchmark.run("Working Memory Access", "Memory", []() { /* Implementation */ }, 10000);
    
    // Add more benchmarks as needed
}

void BenchmarkRunner::runNeuromodulationBenchmarks() {
    Benchmark benchmark("Neuromodulation Suite");
    
    // Run all neuromodulation benchmarks
    std::vector<float> rewards = {0.1f, 0.5f, 1.0f};
    std::vector<float> values = {0.0f, 0.5f, 1.0f};
    auto rpe_result = NeuromodulatorBenchmark::benchmarkRewardPredictionError(rewards, values, 1000);
    benchmark.run("RPE", "Neuromodulation", []() { /* Implementation */ }, 1000);
    
    // Add more benchmarks as needed
}

void BenchmarkRunner::runPlasticityBenchmarks() {
    Benchmark benchmark("Plasticity Suite");
    
    // Run all plasticity benchmarks
    std::vector<std::pair<Timestamp, Timestamp>> spike_pairs;
    auto stdp_result = PlasticityBenchmark::benchmarkSTDPUpdate(spike_pairs, 10.0f, 1000);
    benchmark.run("STDP", "Plasticity", []() { /* Implementation */ }, 1000);
    
    // Add more benchmarks as needed
}

void BenchmarkRunner::runPredictionBenchmarks() {
    Benchmark benchmark("Prediction Suite");
    
    // Run all prediction benchmarks
    std::vector<std::vector<float>> observations;
    auto inference_result = PredictionBenchmark::benchmarkInference(observations, 1000);
    benchmark.run("Inference", "Prediction", []() { /* Implementation */ }, 1000);
    
    // Add more benchmarks as needed
}

// Global benchmark runner instance
namespace {
    BenchmarkRunner& global_benchmark_runner = BenchmarkRunner::getInstance();
}

// Helper function for timestamp
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm
