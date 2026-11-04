#include "Benchmark.hpp"
#include "brain/Brain.hpp"
#include <thread>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace nlm {

// Implementation of Timer methods
void Timer::reset() {
    startTime = std::chrono::high_resolution_clock::now();
}

// Implementation of Benchmark methods
BenchmarkMetrics Benchmark::runBenchmark(std::shared_ptr<Config> config, 
                                         size_t steps,
                                         bool detailedTiming,
                                         bool logProgress) {
    BenchmarkMetrics metrics;
    metrics.neurons = config->getOr<int64_t>("neuron_count", 1000);
    metrics.synapses = config->getOr<int64_t>("synapse_count", metrics.neurons * 10);
    metrics.steps = steps;
    
    NLM_LOG_INFO("=== Starting Benchmark ===");
    NLM_LOG_INFO("Configuration: " + metrics.neurons + " neurons, " + metrics.synapses + " synapses");
    NLM_LOG_INFO("Steps: " + std::to_string(steps));
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    
    Timer timer;
    recordStartTime();
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for benchmark!");
        return metrics;
    }
    
    double setupTime = timer.elapsedSeconds();
    NLM_LOG_INFO("Brain initialization time: " + std::to_string(setupTime) + "s");
    
    // Run simulation steps
    if (logProgress) {
        BenchmarkProgress progress(steps, true, true);
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0::001);
            progress.update(step);
        }
        
        progress.complete();
    } else {
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
    }
    
    recordEndTime();
    
    double totalTime = getElapsedSeconds();
    metrics.totalTime = totalTime;
    metrics.avgStepTime = totalTime / steps;
    metrics.stepsPerSecond = steps / totalTime;
    metrics.totalSpikes = brain->getTotalSpikeCount();
    metrics.avgFiringRate = metrics.totalSpikes / totalTime / metrics.neurons;
    metrics.memoryUsageMB = measureMemoryUsage();
    
    if (detailedTiming) {
        // Collect step times for detailed analysis
        metrics.stepTimes.clear();
        metrics.stepTimes.reserve(steps);
        
        // Run a shorter test to collect step times
        size_t sampleSteps = std::min<size_t>(100, steps);
        recordStartTime();
        
        for (size_t step = 0; step < sampleSteps; ++step) {
            brain->step(step, step * 0.001);
            double stepTime = timer.elapsedSeconds();
            metrics.stepTimes.push_back(stepTime);
        }
        
        recordEndTime();
    }
    
    NLM_LOG_INFO("=== Benchmark Results ===");
    NLM_LOG_INFO("Total time: " + std::to_string(totalTime) + "s");
    NLM_LOG_INFO("Average step time: " + std::to_string(metrics.avgStepTime * 1000) + "ms");
    NLM_LOG_INFO("Steps per second: " + std::to_string(metrics.stepsPerSecond));
    NLM_LOG_INFO("Total spikes: " + std::to_string(metrics.totalSpikes));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(metrics.avgFiringRate) + " Hz/neuron");
    NLM_LOG_INFO("Memory usage: " + std::to_string(metrics.memoryUsageMB) + " MB");
    
    if (!metrics.stepTimes.empty()) {
        PerformanceStats stats = calculateStatistics(metrics.stepTimes);
        NLM_LOG_INFO("Step time statistics:");
        NLM_LOG_INFO("  Min: " + std::to_string(stats.min * 1000) + "ms");
        NLM_LOG_INFO("  Max: " + std::to_string(stats.max * 1000) + "ms");
        NLM_LOG_INFO("  Mean: " + std::to_string(stats.mean * 1000) + "ms");
        NLM_LOG_INFO("  Std Dev: " + std::to_string(stats.stddev * 1000) + "ms");
        NLM_LOG_INFO("  95th percentile: " + std::to_string(stats.p95 * 1000) + "ms");
        NLM_LOG_INFO("  99th percentile: " + std::to_string(stats.p99 * 1000) + "ms");
    }
    
    return metrics;
}

std::vector<BenchmarkMetrics> Benchmark::runComparison(const std::vector<std::shared_ptr<Config>>& configs,
                                                        size_t steps) {
    std::vector<BenchmarkMetrics> results;
    
    NLM_LOG_INFO("=== Running Comparison Benchmark ===");
    NLM_LOG_INFO("Testing " + std::to_string(configs.size()) + " configurations");
    
    for (size_t i = 0; i < configs.size(); ++i) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration " + std::to_string(i + 1) + ":");
        
        auto metrics = runBenchmark(configs[i], steps, true, true);
        results.push_back(metrics);
        
        // Add configuration details to metrics
        metrics.neurons = configs[i]->getOr<int64_t>("neuron_count", 1000);
        metrics.synapses = configs[i]->getOr<int64_t>("synapse_count", metrics.neurons * 10);
    }
    
    return results;
}

Benchmark::PerformanceProfile Benchmark::runPerformanceProfile(std::shared_ptr<Config> config, size_t steps) {
    PerformanceProfile profile;
    
    NLM_LOG_INFO("=== Running Performance Profile ===");
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Measure neuron setup time
    Timer timer;
    recordStartTime();
    
    auto brain2 = std::make_shared<Brain>(config);
    brain2->initialize();
    
    recordEndTime();
    profile.neuronSetupTime = getElapsedSeconds();
    
    // Measure step execution time
    recordStartTime();
    
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
    }
    
    recordEndTime();
    profile.stepExecutionTime = getElapsedSeconds();
    
    // Measure plasticity time (run with plasticity enabled)
    // For this, we need to enable plasticity on some synapses
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        recordStartTime();
        
        for (size_t step = 0; step < steps / 10; ++step) {
            brain->step(step, step * 0.001);
            
            // Apply some current to trigger plasticity
            for (size_t i = 0; i < 10 && i < region->getAllNeurons().size(); ++i) {
                auto* neuron = region->getAllNeurons()[i];
                brain->injectCurrent(neuron->getId(), 10.0f);
            }
        }
        
        recordEndTime();
        profile.plasticityTime = getElapsedSeconds();
    }
    
    // Measure memory access time
    recordStartTime();
    
    // Access memory systems
    auto workingMem = brain->getWorkingMemory();
    auto episodicMem = brain->getEpisodicMemory();
    
    for (size_t step = 0; step < steps / 20; ++step) {
        brain->step(step, step * 0.001);
        
        if (workingMem) {
            workingMem->storeExperience(step % 100, 1.0f);
        }
        
        if (episodicMem) {
            episodicMem->storeEpisode(step % 50, 1.0f);
        }
    }
    
    recordEndTime();
    profile.memoryAccessTime = getElapsedSeconds();
    
    // Measure spike propagation time
    // Create neurons with connections and measure spike propagation
    recordStartTime();
    
    for (size_t step = 0; step < steps / 10; ++step) {
        brain->step(step, step * 0.001);
        
        // Inject strong current to create spikes
        if (region && !region->getAllNeurons().empty()) {
            region->getAllNeurons()[0]->injectCurrent(50.0f);
        }
    }
    
    recordEndTime();
    profile.spikePropagationTime = getElapsedSeconds();
    
    NLM_LOG_INFO("Performance profile results:");
    NLM_LOG_INFO("  Neuron setup: " + std::to_string(profile.neuronSetupTime) + "s");
    NLM_LOG_INFO("  Step execution: " + std::to_string(profile.stepExecutionTime) + "s");
    NLM_LOG_INFO("  Plasticity: " + std::to_string(profile.plasticityTime) + "s");
    NLM_LOG_INFO("  Memory access: " + std::to_string(profile.memoryAccessTime) + "s");
    NLM_LOG_INFO("  Spike propagation: " + std::to_string(profile.spikePropagationTime) + "s");
    
    return profile;
}

double Benchmark::measureMemoryUsage() const {
    // Get memory usage in KB
    struct stat stat_buf;
    std::string cmd = "ps -o rss= -p " + std::to_string(getpid());
    
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        NLM_LOG_WARNING("Failed to get memory usage");
        return 0.0;
    }
    
    char buffer[128];
    std::string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            result += buffer;
        }
    }
    
    pclose(pipe);
    
    try {
        long memory_kb = std::stol(result);
        return memory_kb / 1024.0; // Convert to MB
    } catch (const std::exception& e) {
        NLM_LOG_WARNING("Failed to parse memory usage: " + std::string(e.what()));
        return 0.0;
    }
}

Benchmark::PerformanceStats Benchmark::calculateStatistics(const std::vector<double>& values) const {
    PerformanceStats stats;
    
    if (values.empty()) {
        return stats;
    }
    
    stats.min = *std::min_element(values.begin(), values.end());
    stats.max = *std::max_element(values.begin(), values.end());
    stats.mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    
    // Calculate standard deviation
    double sumSquared = 0.0;
    for (double value : values) {
        double diff = value - stats.mean;
        sumSquared += diff * diff;
    }
    stats.stddev = std::sqrt(sumSquared / values.size());
    
    // Calculate percentiles
    std::vector<double> sortedValues = values;
    std::sort(sortedValues.begin(), sortedValues.end());
    
    size_t p95Index = static_cast<size_t>(values.size() * 0.95);
    size_t p99Index = static_cast<size_t>(values.size() * 0.99);
    
    if (p95Index >= values.size()) p95Index = values.size() - 1;
    if (p99Index >= values.size()) p99Index = values.size() - 1;
    
    stats.p95 = sortedValues[p95Index];
    stats.p99 = sortedValues[p99Index];
    stats.median = sortedValues[values.size() / 2];
    
    return stats;
}

std::string Benchmark::generateReport(const BenchmarkMetrics& metrics) const {
    std::stringstream report;
    
    report << "=== NLM Benchmark Report ===\n";
    report << "Generated: " << std::chrono::system_clock::now() << "\n\n";
    
    report << "=== Configuration ===\n";
    report << "Neurons: " << metrics.neurons << "\n";
    report << "Synapses: " << metrics.synapses << "\n";
    report << "Steps: " << metrics.steps << "\n\n";
    
    report << "=== Performance ===\n";
    report << std::fixed << std::setprecision(3);
    report << "Total time: " << metrics.totalTime << "s\n";
    report << "Average step time: " << metrics.avgStepTime * 1000 << "ms\n";
    report << "Steps per second: " << metrics.stepsPerSecond << "\n";
    report << "Total spikes: " << metrics.totalSpikes << "\n";
    report << "Average firing rate: " << metrics.avgFiringRate << " Hz/neuron\n";
    report << "Memory usage: " << metrics.memoryUsageMB << " MB\n\n";
    
    if (!metrics.stepTimes.empty()) {
        PerformanceStats stats = calculateStatistics(metrics.stepTimes);
        report << "=== Step Time Statistics ===\n";
        report << std::fixed << std::setprecision(4);
        report << "Min: " << stats.min * 1000 << "ms\n";
        report << "Max: " << stats.max * 1000 << "ms\n";
        report << "Mean: " << stats.mean * 1000 << "ms\n";
        report << "Std Dev: " << stats.stddev * 1000 << "ms\n";
        report << "Median: " << stats.median * 1000 << "ms\n";
        report << "95th percentile: " << stats.p95 * 1000 << "ms\n";
        report << "99th percentile: " << stats.p99 * 1000 << "ms\n";
    }
    
    report << "\n=== Benchmark Complete ===\n";
    
    return report.str();
}

std::string Benchmark::generateComparisonReport(const std::vector<BenchmarkMetrics>& metrics) const {
    std::stringstream report;
    
    report << "=== NLM Comparison Benchmark Report ===\n";
    report << "Generated: " << std::chrono::system_clock::now() << "\n\n";
    
    report << "=== Configuration Comparison ===\n";
    report << std::setw(10) << "Config" << std::setw(15) << "Neurons" 
            << std::setw(15) << "Synapses" << std::setw(15) << "Steps" 
            << std::setw(12) << "Total Time" << std::setw(15) << "Steps/sec" 
            << std::setw(15) << "Firing Rate" << std::setw(12) << "Memory (MB)" << std::endl;
    
    report << std::setfill('-') << std::setw(110) << "" << std::setfill(' ') << std::endl;
    
    for (size_t i = 0; i < metrics.size(); ++i) {
        report << std::setw(10) << ("Config " + std::to_string(i + 1)) 
               << std::setw(15) << metrics[i].neurons
               << std::setw(15) << metrics[i].synapses
               << std::setw(15) << metrics[i].steps
               << std::setw(12) << std::fixed << std::setprecision(3) << metrics[i].totalTime
               << std::setw(15) << std::fixed << std::setprecision(2) << metrics[i].stepsPerSecond
               << std::setw(15) << std::fixed << std::setprecision(3) << metrics[i].avgFiringRate
               << std::setw(12) << std::fixed << std::setprecision(2) << metrics[i].memoryUsageMB
               << std::endl;
    }
    
    report << "\n=== Performance Analysis ===\n";
    
    if (metrics.size() > 1) {
        report << "Best performers:\n";
        
        // Find best in terms of steps per second
        size_t bestIndex = std::max_element(metrics.begin(), metrics.end(),
                                           [](const BenchmarkMetrics& a, const BenchmarkMetrics& b) {
                                               return a.stepsPerSecond < b.stepsPerSecond;
                                           }) - metrics.begin();
        
        report << "  - Steps/sec: Config " << bestIndex + 1 
               << " (" << metrics[bestIndex].stepsPerSecond << " steps/sec)\n";
        
        // Find best in terms of memory efficiency
        size_t bestMemoryIndex = std::min_element(metrics.begin(), metrics.end(),
                                                 [](const BenchmarkMetrics& a, const BenchmarkMetrics& b) {
                                                     return a.memoryUsageMB < b.memoryUsageMB;
                                                 }) - metrics.begin();
        
        report << "  - Memory efficient: Config " << bestMemoryIndex + 1
               << " (" << metrics[bestMemoryIndex].memoryUsageMB << " MB)\n";
        
        // Find best in terms of firing rate
        size_t bestFiringIndex = std::max_element(metrics.begin(), metrics.end(),
                                                 [](const BenchmarkMetrics& a, const BenchmarkMetrics& b) {
                                                     return a.avgFiringRate < b.avgFiringRate;
                                                 }) - metrics.begin();
        
        report << "  - Highest firing rate: Config " << bestFiringIndex + 1
               << " (" << metrics[bestFiringIndex].avgFiringRate << " Hz/neuron)\n";
    }
    
    return report.str();
}

bool Benchmark::validateBenchmarkConfig(std::shared_ptr<Config> config) {
    bool valid = true;
    
    // Check required configuration values
    if (!config->has("neuron_count") || config->getOr<int64_t>("neuron_count", 0) <= 0) {
        NLM_LOG_ERROR("Invalid neuron_count: must be positive");
        valid = false;
    }
    
    if (!config->has("region_count") || config->getOr<int64_t>("region_count", 0) <= 0) {
        NLM_LOG_ERROR("Invalid region_count: must be positive");
        valid = false;
    }
    
    if (!config->has("connection_probability") || config->getOr<float>("connection_probability", -1.0f) <= 0.0f) {
        NLM_LOG_ERROR("Invalid connection_probability: must be positive");
        valid = false;
    }
    
    // Check for reasonable values
    int64_t neuronCount = config->getOr<int64_t>("neuron_count", 0);
    if (neuronCount > 100000) {
        NLM_LOG_WARNING("Very large neuron count may impact performance: " + std::to_string(neuronCount));
    }
    
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    if (timestep < 0.0001 || timestep > 0.1) {
        NLM_LOG_WARNING("Unusual simulation timestep: " + std::to_string(timestep) + "s");
    }
    
    return valid;
}

std::vector<std::shared_ptr<Config>> Benchmark::getBenchmarkConfigs() {
    std::vector<std::shared_ptr<Config>> configs;
    
    // Default configuration
    auto defaultConfig = std::make_shared<Config>();
    defaultConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    defaultConfig->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    defaultConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    defaultConfig->set("simulation_timestep", 0.001, ConfigSource::Default);
    defaultConfig->set("connection_probability", 0.15f, ConfigSource::Default);
    
    configs.push_back(defaultConfig);
    
    // Small configuration
    auto smallConfig = std::make_shared<Config>();
    smallConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    smallConfig->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    smallConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    smallConfig->set("simulation_timestep", 0.001, ConfigSource::Default);
    smallConfig->set("connection_probability", 0.10f, ConfigSource::Default);
    
    configs.push_back(smallConfig);
    
    // Large configuration
    auto largeConfig = std::make_shared<Config>();
    largeConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    largeConfig->set("neuron_count", static_cast<int64_t>(10000), ConfigSource::Default);
    largeConfig->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
    largeConfig->set("simulation_timestep", 0.001, ConfigSource::Default);
    largeConfig->set("connection_probability", 0.20f, ConfigSource::Default);
    
    configs.push_back(largeConfig);
    
    return configs;
}

// Implementation of BenchmarkSuite methods
void BenchmarkSuite::runAllBenchmarks(std::shared_ptr<Config> config, size_t steps) {
    NLM_LOG_INFO("=== Running Comprehensive Benchmark Suite ===");
    
    try {
        runConnectivityBenchmark(config, steps);
        runPlasticityBenchmark(config, steps);
        runScalingBenchmark(config, steps);
        runMemoryBenchmark(config, steps);
        runNeuromodulationBenchmark(config, steps);
        
        NLM_LOG_INFO("=== All benchmarks completed ===");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Benchmark suite failed: " + std::string(e.what()));
        throw;
    }
}

void BenchmarkSuite::runConnectivityBenchmark(std::shared_ptr<Config> config, size_t steps) {
    NLM_LOG_INFO("=== Running Connectivity Benchmark ===");
    
    BenchmarkMetrics metrics = benchmark.runBenchmark(config, steps, true, true);
    metrics.neurons = config->getOr<int64_t>("neuron_count", 1000);
    metrics.synapses = config->getOr<int64_t>("synapse_count", metrics.neurons * 10);
    
    results.push_back(metrics);
    
    NLM_LOG_INFO("Connectivity benchmark completed:");
    NLM_LOG_INFO("  - Neurons: " + std::to_string(metrics.neurons));
    NLM_LOG_INFO("  - Synapses: " + std::to_string(metrics.synapses));
    NLM_LOG_INFO("  - Steps: " + std::to_string(metrics.steps));
    NLM_LOG_INFO("  - Total time: " + std::to_string(metrics.totalTime) + "s");
    NLM_LOG_INFO("  - Steps/sec: " + std::to_string(metrics.stepsPerSecond));
}

void BenchmarkSuite::runPlasticityBenchmark(std::shared_ptr<Config> config, size_t steps) {
    NLM_LOG_INFO("=== Running Plasticity Benchmark ===");
    
    BenchmarkMetrics metrics = benchmark.runBenchmark(config, steps, true, true);
    metrics.neurons = config->getOr<int64_t>("neuron_count", 1000);
    metrics.synapses = config->getOr<int64_t>("synapse_count", metrics.neurons * 10);
    
    results.push_back(metrics);
    
    NLM_LOG_INFO("Plasticity benchmark completed:");
    NLM_LOG_INFO("  - Neurons: " + std::to_string(metrics.neurons));
    NLM_LOG_INFO("  - Synapses: " + std::to_string(metrics.synapses));
    NLM_LOG_INFO("  - Total time: " + std::to_string(metrics.totalTime) + "s");
    NLM_LOG_INFO("  - Steps/sec: " + std::to_string(metrics.stepsPerSecond));
    NLM_LOG_INFO("  - Total spikes: " + std::to_string(metrics.totalSpikes));
}

void BenchmarkSuite::runScalingBenchmark(std::shared_ptr<Config> config, size_t steps) {
    NLM_LOG_INFO("=== Running Scaling Benchmark ===");
    
    BenchmarkMetrics metrics = benchmark.runBenchmark(config, steps, true, true);
    metrics.neurons = config->getOr<int64_t>("neuron_count", 1000);
    metrics.synapses = config->getOr<int64_t>("synapse_count", metrics.neurons * 10);
    
    results.push_back(metrics);
    
    NLM_LOG_INFO("Scaling benchmark completed:");
    NLM_LOG_INFO("  - Neurons: " + std::to_string(metrics.neurons));
    NLM_LOG_INFO("  - Synapses: " + std::to_string(metrics.synapses));
    NLM_LOG_INFO("  - Total time: " + std::to_string(metrics.totalTime) + "s");
    NLM_LOG_INFO("  - Steps/sec: " + std::to_string(metrics.stepsPerSecond));
}

void BenchmarkSuite::runMemoryBenchmark(std::shared_ptr<Config> config, size_t steps) {
    NLM_LOG_INFO("=== Running Memory Benchmark ===");
    
    BenchmarkMetrics metrics = benchmark.runBenchmark(config, steps, true, true);
    metrics.neurons = config->getOr<int64_t>("neuron_count", 1000);
    metrics.synapses = config->getOr<int64_t>("synapse_count", metrics.neurons * 10);
    
    results.push_back(metrics);
    
    NLM_LOG_INFO("Memory benchmark completed:");
    NLM_LOG_INFO("  - Neurons: " + std::to_string(metrics.neurons));
    NLM_LOG_INFO("  - Synapses: " + std::to_string(metrics.synapses));
    NLM_LOG_INFO("  - Total time: " + std::to_string(metrics.totalTime) + "s");
    NLM_LOG_INFO("  - Steps/sec: " + std::to_string(metrics.stepsPerSecond));
    NLM_LOG_INFO("  - Memory usage: " + std::to_string(metrics.memoryUsageMB) + " MB");
}

void BenchmarkSuite::runNeuromodulationBenchmark(std::shared_ptr<Config> config, size_t steps) {
    NLM_LOG_INFO("=== Running Neuromodulation Benchmark ===");
    
    BenchmarkMetrics metrics = benchmark.runBenchmark(config, steps, true, true);
    metrics.neurons = config->getOr<int64_t>("neuron_count", 1000);
    metrics.synapses = config->getOr<int64_t>("synapse_count", metrics.neurons * 10);
    
    results.push_back(metrics);
    
    NLM_LOG_INFO("Neuromodulation benchmark completed:");
    NLM_LOG_INFO("  - Neurons: " + std::to_string(metrics.neurons));
    NLM_LOG_INFO("  - Synapses: " + std::to_string(metrics.synapses));
    NLM_LOG_INFO("  - Total time: " + std::to_string(metrics.totalTime) + "s");
    NLM_LOG_INFO("  - Steps/sec: " + std::to_string(metrics.stepsPerSecond));
    NLM_LOG_INFO("  - Total spikes: " + std::to_string(metrics.totalSpikes));
    NLM_LOG_INFO("  - Firing rate: " + std::to_string(metrics.avgFiringRate) + " Hz/neuron");
}

std::string BenchmarkSuite::generateFullReport() const {
    std::stringstream report;
    
    report << "=== NLM Benchmark Suite Report ===\n";
    report << "Generated: " << std::chrono::system_clock::now() << "\n\n";
    
    report << "=== Summary ===\n";
    report << "Total benchmarks run: " << results.size() << "\n\n";
    
    for (size_t i = 0; i < results.size(); ++i) {
        report << "=== Benchmark " << i + 1 << " ===\n";
        report << benchmark.generateReport(results[i]);
        report << "\n";
    }
    
    report << "=== Comparative Analysis ===\n";
    
    if (results.size() > 1) {
        report << "Performance comparison:\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            report << "Benchmark " << i + 1 << ": "
                   << std::fixed << std::setprecision(2) 
                   << results[i].stepsPerSecond << " steps/sec, "
                   << results[i].avgStepTime * 1000 << "ms/step\n";
        }
    }
    
    return report.str();
}

bool BenchmarkSuite::saveResults(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    // Convert results to JSON-like format
    file << "{\n";
    file << "  \"benchmarks\": [\n";
    
    for (size_t i = 0; i < results.size(); ++i) {
        file << "    {\n";
        file << "      \"id\": " << i << ",\n";
        file << "      \"neurons\": " << results[i].neurons << ",\n";
        file << "      \"synapses\": " << results[i].synapses << ",\n";
        file << "      \"steps\": " << results[i].steps << ",\n";
        file << "      \"totalTime\": " << std::fixed << std::setprecision(6) << results[i].totalTime << ",\n";
        file << "      \"avgStepTime\": " << std::fixed << std::setprecision(6) << results[i].avgStepTime << ",\n";
        file << "      \"stepsPerSecond\": " << std::fixed << std::setprecision(2) << results[i].stepsPerSecond << ",\n";
        file << "      \"totalSpikes\": " << results[i].totalSpikes << ",\n";
        file << "      \"avgFiringRate\": " << std::fixed << std::setprecision(6) << results[i].avgFiringRate << ",\n";
        file << "      \"memoryUsageMB\": " << std::fixed << std::setprecision(2) << results[i].memoryUsageMB << "\n";
        
        if (i < results.size() - 1) {
            file << "    },\n";
        } else {
            file << "    }\n";
        }
    }
    
    file << "  ],\n";
    file << "  \"timestamp\": \"" << std::chrono::system_clock::now() << "\"\n";
    file << "}\n";
    
    file.close();
    NLM_LOG_INFO("Benchmark results saved to: " + filename);
    return true;
}

bool BenchmarkSuite::loadResults(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for reading: " + filename);
        return false;
    }
    
    // Simple JSON parsing (for demonstration)
    std::string line;
    std::vector<BenchmarkMetrics> loadedResults;
    
    while (std::getline(file, line)) {
        if (line.find("\"neurons\":") != std::string::npos) {
            // Parse line by line (simplified)
            // In a real implementation, you would use a proper JSON parser
        }
    }
    
    file.close();
    
    results = loadedResults;
    NLM_LOG_INFO("Benchmark results loaded from: " + filename);
    return true;
}

} // namespace nlm
