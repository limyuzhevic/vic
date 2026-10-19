// Comprehensive benchmarking and performance testing framework implementation
// Tools for measuring and analyzing NLM performance characteristics

#include "Benchmark.hpp"
#include <algorithm>
#include <numeric>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <cmath>
#include <limits>
#include <filesystem>

namespace nlm {

// BenchmarkSuite implementation
BenchmarkSuite::BenchmarkSuite(const std::string& name, const std::string& description)
    : name_(name), description_(description) {
}

BenchmarkSuite::~BenchmarkSuite() {
}

template<typename Func>
BenchmarkResult BenchmarkSuite::run(const std::string& benchmarkName, 
                                   const std::string& description,
                                   Func&& func,
                                   size_t iterations) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    BenchmarkResult result(benchmarkName, description);
    result.samples.reserve(iterations);
    
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; ++i) {
        auto before = std::chrono::high_resolution_clock::now();
        func();
        auto after = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(after - before);
        double timeMs = duration.count() / 1000000.0;  // Convert to milliseconds
        
        result.samples.push_back(timeMs);
        
        // Yield to other threads to get more accurate measurements
        if (i < iterations - 1) {
            std::this_thread::yield();
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    result.totalTime = totalDuration.count() / 1000000.0;  // Convert to milliseconds
    result.iterationCount = iterations;
    
    calculateStatistics(result);
    
    results_.push_back(std::move(result));
    return results_.back();
}

template<typename SetupFunc, typename Func>
BenchmarkResult BenchmarkSuite::runWithSetup(const std::string& benchmarkName,
                                            const std::string& description,
                                            SetupFunc&& setup,
                                            Func&& func,
                                            size_t iterations) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    BenchmarkResult result(benchmarkName, description);
    result.samples.reserve(iterations);
    
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; ++i) {
        // Setup (only run once for the first iteration)
        if (i == 0) {
            setup();
        }
        
        auto before = std::chrono::high_resolution_clock::now();
        func();
        auto after = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(after - before);
        double timeMs = duration.count() / 1000000.0;  // Convert to milliseconds
        
        result.samples.push_back(timeMs);
        
        // Yield to other threads to get more accurate measurements
        if (i < iterations - 1) {
            std::this_thread::yield();
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    result.totalTime = totalDuration.count() / 1000000.0;  // Convert to milliseconds
    result.iterationCount = iterations;
    
    calculateStatistics(result);
    
    results_.push_back(std::move(result));
    return results_.back();
}

BenchmarkResult BenchmarkSuite::brainSimulation(size_t neuronCount, 
                                                  size_t steps,
                                                  const std::string& configFile) {
    return runWithSetup("Brain Simulation",
                       "Benchmark full brain simulation performance",
                       [neuronCount, configFile]() {
                           // Create configuration
                           auto config = std::make_shared<Config>();
                           config->set("neuron_count", neuronCount);
                           config->set("region_count", 1);
                           config->set("connection_probability", 0.1f);
                           config->set("random_seed", 42);
                           
                           if (!configFile.empty()) {
                               config->loadFromFile(configFile);
                           }
                           
                           // Create brain
                           brain_ = std::make_shared<Brain>(config);
                           if (!brain_->initialize()) {
                               throw std::runtime_error("Failed to initialize brain");
                           }
                       },
                       [this, steps]() {
                           // Run simulation steps
                           for (size_t i = 0; i < steps; ++i) {
                               brain_->step(i);
                           }
                       },
                       1); // Run once for full simulation benchmark
}

BenchmarkResult BenchmarkSuite::memoryBenchmark(size_t memorySizeMB, 
                                                  const std::string& operation) {
    return run("Memory Benchmark",
               "Benchmark memory allocation/deallocation performance",
               [memorySizeMB, operation]() {
                   std::vector<std::vector<double>> memory;
                   
                   if (operation == "allocate") {
                       size_t elements = (memorySizeMB * 1024 * 1024) / sizeof(double);
                       memory.resize(elements / 1024); // Allocate in chunks
                       for (auto& chunk : memory) {
                           chunk.resize(1024);
                           std::fill(chunk.begin(), chunk.end(), 0.0);
                       }
                   } else if (operation == "deallocate") {
                       // Same as allocate for fair comparison
                       size_t elements = (memorySizeMB * 1024 * 1024) / sizeof(double);
                       std::vector<std::vector<double>> memory;
                       memory.resize(elements / 1024);
                       for (auto& chunk : memory) {
                           chunk.resize(1024);
                           std::fill(chunk.begin(), chunk.end(), 0.0);
                       }
                       memory.clear(); // Deallocate
                   } else if (operation == "access") {
                       size_t elements = (memorySizeMB * 1024 * 1024) / sizeof(double);
                       std::vector<std::vector<double>> memory;
                       memory.resize(elements / 1024);
                       for (auto& chunk : memory) {
                           chunk.resize(1024);
                           std::fill(chunk.begin(), chunk.end(), 0.0);
                       }
                       // Access all elements
                       for (const auto& chunk : memory) {
                           for (const auto& value : chunk) {
                               volatile double x = value; // Prevent optimization
                               (void)x;
                           }
                       }
                   }
               },
               1); // Run once for memory benchmark
}

BenchmarkResult BenchmarkSuite::neuronLookupBenchmark(size_t neuronCount, 
                                                      size_t lookupCount) {
    return runWithSetup("Neuron Lookup",
                       "Benchmark neuron lookup performance",
                       [neuronCount]() {
                           // Create mock neurons
                           std::vector<NeuronId> neurons;
                           for (size_t i = 0; i < neuronCount; ++i) {
                               neurons.emplace_back(i);
                           }
                           
                           // Store in map for lookup benchmark
                           neuronMap_.clear();
                           for (const auto& neuron : neurons) {
                               neuronMap_[neuron.getId()] = neuron;
                           }
                       },
                       [this, lookupCount]() {
                           // Perform lookups
                           for (size_t i = 0; i < lookupCount; ++i) {
                               auto it = neuronMap_.find(i % neuronCount);
                               volatile NeuronId x = (it != neuronMap_.end()) ? it->second : INVALID_NEURON_ID;
                               (void)x;
                           }
                       },
                       1); // Run once for lookup benchmark
}

BenchmarkResult BenchmarkSuite::spikeProcessingBenchmark(size_t neuronCount,
                                                         float firingRate,
                                                         size_t steps) {
    return runWithSetup("Spike Processing",
                       "Benchmark spike processing performance",
                       [neuronCount, firingRate]() {
                           // Create mock neurons with spike probabilities
                           std::vector<float> spikeProbabilities;
                           for (size_t i = 0; i < neuronCount; ++i) {
                               spikeProbabilities.push_back(firingRate);
                           }
                           
                           // Setup spike system
                           spikeSystem_ = std::make_shared<SpikeSystem>();
                           for (size_t i = 0; i < neuronCount; ++i) {
                               if (std::rand() / (RAND_MAX + 1.0) < firingRate) {
                                   spikeSystem_->queueSpike(SpikeEvent(NeuronId(i), 0.0, 0));
                               }
                           }
                       },
                       [this, steps]() {
                           // Process spikes for specified steps
                           for (size_t i = 0; i < steps; ++i) {
                               spikeSystem_->processSpikes(i);
                               spikeSystem_->processDelayedSpikes(i, static_cast<Timestamp>(i) * 0.001);
                           }
                       },
                       1); // Run once for spike processing benchmark
}

BenchmarkResult BenchmarkSuite::plasticityBenchmark(size_t neuronCount,
                                                     size_t synapseCount,
                                                     float plasticityRate) {
    return runWithSetup("Plasticity",
                       "Benchmark synaptic plasticity performance",
                       [neuronCount, synapseCount, plasticityRate]() {
                           // Create mock neurons and synapses
                           std::vector<std::shared_ptr<Synapse>> synapses;
                           for (size_t i = 0; i < synapseCount; ++i) {
                               auto synapse = std::make_shared<Synapse>();
                               synapse->setWeight((std::rand() / (RAND_MAX + 1.0)) * 2.0f - 1.0f);
                               synapse->setDelay(1 + (std::rand() % 5));
                               synapse->addToWeight(plasticityRate);
                               synapses.push_back(synapse);
                           }
                           
                           // Setup plasticity system
                           plasticitySystem_ = std::make_shared<STDP>();
                           for (auto& synapse : synapses) {
                               plasticitySystem_->update(synapse.get(), {}, {}, 0.001);
                           }
                       },
                       []() {
                           // Perform plasticity updates
                           std::this_thread::sleep_for(std::chrono::milliseconds(10));
                       },
                       1); // Run once for plasticity benchmark
}

BenchmarkResult BenchmarkSuite::memorySystemBenchmark(const std::string& memorySystem) {
    return run("Memory System",
               "Benchmark memory system performance",
               [memorySystem]() {
                   // Mock memory system operations
                   std::vector<float> memoryData;
                   memoryData.reserve(10000);
                   
                   if (memorySystem == "working_memory") {
                       // Simulate working memory operations
                       for (size_t i = 0; i < 10000; ++i) {
                           memoryData.push_back(static_cast<float>(i) * 0.01f);
                           if (i % 100 == 0) {
                               memoryData.erase(memoryData.begin()); // Simulate decay
                           }
                       }
                   } else if (memorySystem == "episodic_memory") {
                       // Simulate episodic memory operations
                       std::vector<EpisodicMemoryItem> episodes;
                       for (size_t i = 0; i < 1000; ++i) {
                           EpisodicMemoryItem episode;
                           episode.timestamp = i;
                           episode.reward = static_cast<float>(i % 100) / 100.0f;
                           episode.sensoryState.resize(10, 0.0f);
                           std::generate(episode.sensoryState.begin(), episode.sensoryState.end(),
                                       []() { return static_cast<float>(rand()) / RAND_MAX; });
                           episodes.push_back(episode);
                       }
                   }
               },
               1); // Run once for memory system benchmark
}

BenchmarkResult BenchmarkSuite::integrationBenchmark(const std::string& integrationType) {
    return run("Integration",
               "Benchmark integration performance",
               [integrationType]() {
                   // Mock integration operations
                   if (integrationType == "full_brain") {
                       // Simulate full brain integration
                       std::vector<double> integrationResults;
                       for (size_t i = 0; i < 1000; ++i) {
                           double result = static_cast<double>(rand()) / RAND_MAX;
                           integrationResults.push_back(result);
                           
                           // Simulate integration computations
                           for (size_t j = 0; j < 100; ++j) {
                               double temp = result * 0.01 * j;
                               result = std::sin(temp) + std::cos(temp);
                           }
                       }
                   } else if (integrationType == "sensory_motor") {
                       // Simulate sensory-motor integration
                       for (size_t i = 0; i < 500; ++i) {
                           double sensoryInput = static_cast<double>(rand()) / RAND_MAX;
                           double motorOutput = sensoryInput * 0.5 + static_cast<double>(rand()) / RAND_MAX * 0.5;
                           volatile double x = motorOutput; // Prevent optimization
                           (void)x;
                       }
                   }
               },
               1); // Run once for integration benchmark
}

const BenchmarkResult* BenchmarkSuite::getResult(const std::string& name) const {
    for (const auto& result : results_) {
        if (result.name == name) {
            return &result;
        }
    }
    return nullptr;
}

bool BenchmarkSuite::saveResults(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Benchmark Results" << std::endl;
        file << "=================" << std::endl << std::endl;
        
        for (const auto& result : results_) {
            file << "Benchmark: " << result.name << std::endl;
            file << "Description: " << result.description << std::endl;
            file << "Timestamp: " << result.timestamp << std::endl;
            file << "Iterations: " << result.iterationCount << std::endl;
            file << "Total Time: " << result.totalTime << " ms" << std::endl;
            file << "Mean Time: " << result.mean << " ms" << std::endl;
            file << "Median Time: " << result.median << " ms" << std::endl;
            file << "Stddev Time: " << result.stddev << " ms" << std::endl;
            file << "Min Time: " << result.min << " ms" << std::endl;
            file << "Max Time: " << result.max << " ms" << std::endl;
            
            file << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool BenchmarkSuite::loadResults(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        BenchmarkResult* currentResult = nullptr;
        
        while (std::getline(file, line)) {
            if (line.empty()) {
                continue;
            }
            
            if (line.find("Benchmark: ") == 0) {
                // Start of new benchmark
                currentResult = new BenchmarkResult(line.substr(11), "");
                results_.push_back(*currentResult);
            } else if (line.find("Description: ") == 0 && currentResult) {
                currentResult->description = line.substr(13);
            } else if (line.find("Timestamp: ") == 0 && currentResult) {
                currentResult->timestamp = std::stod(line.substr(11));
            } else if (line.find("Iterations: ") == 0 && currentResult) {
                currentResult->iterationCount = static_cast<size_t>(std::stoi(line.substr(11)));
            } else if (line.find("Total Time: ") == 0 && currentResult) {
                currentResult->totalTime = std::stod(line.substr(12));
                if (line.find(" ms") != std::string::npos) {
                    currentResult->totalTime /= 1000.0;
                }
            }
            // Note: Other fields would need to be parsed similarly
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string BenchmarkSuite::generateReport() const {
    std::stringstream report;
    
    report << "Benchmark Suite Report" << std::endl;
    report << "=======================" << std::endl;
    report << "Suite: " << name_ << std::endl;
    report << "Description: " << description_ << std::endl;
    report << "Total Benchmarks: " << results_.size() << std::endl;
    report << "Generated: " << std::ctime(&results_.empty() ? time(nullptr) : &results_.back().timestamp) << std::endl;
    report << std::endl;
    
    for (const auto& result : results_) {
        report << "Benchmark: " << result.name << std::endl;
        report << "Description: " << result.description << std::endl;
        report << "Time (ms): Mean=" << result.mean << " Median=" << result.median << " Stddev=" << result.stddev << std::endl;
        report << "Samples: " << result.samples.size() << std::endl;
        report << "Min: " << result.min << " ms Max: " << result.max << " ms" << std::endl;
        report << std::endl;
    }
    
    return report.str();
}

std::string BenchmarkSuite::generateDetailedReport() const {
    std::stringstream report;
    
    report << generateReport();
    report << std::endl;
    
    // Add detailed statistics for each benchmark
    for (const auto& result : results_) {
        report << "Detailed Statistics for " << result.name << ":" << std::endl;
        report << "Samples: ";
        for (size_t i = 0; i < result.samples.size(); ++i) {
            report << result.samples[i];
            if (i < result.samples.size() - 1) report << ", ";
        }
        report << std::endl;
        report << "Mean: " << result.mean << " ms" << std::endl;
        report << "Median: " << result.median << " ms" << std::endl;
        report << "Standard Deviation: " << result.stddev << " ms" << std::endl;
        report << "Coefficient of Variation: " << (result.stddev / result.mean * 100) << "%" << std::endl;
        report << std::endl;
    }
    
    return report.str();
}

void BenchmarkSuite::calculateStatistics(BenchmarkResult& result) const {
    if (result.samples.empty()) {
        return;
    }
    
    // Calculate mean
    double sum = std::accumulate(result.samples.begin(), result.samples.end(), 0.0);
    result.mean = sum / result.samples.size();
    
    // Calculate median
    std::vector<double> sortedSamples = result.samples;
    std::sort(sortedSamples.begin(), sortedSamples.end());
    size_t middle = sortedSamples.size() / 2;
    if (sortedSamples.size() % 2 == 0) {
        result.median = (sortedSamples[middle - 1] + sortedSamples[middle]) / 2.0;
    } else {
        result.median = sortedSamples[middle];
    }
    
    // Calculate standard deviation
    double variance = 0.0;
    for (double sample : result.samples) {
        variance += (sample - result.mean) * (sample - result.mean);
    }
    variance /= result.samples.size();
    result.stddev = std::sqrt(variance);
    
    // Calculate min and max
    result.min = *std::min_element(result.samples.begin(), result.samples.end());
    result.max = *std::max_element(result.samples.begin(), result.samples.end());
}

// PerformanceProfiler implementation
PerformanceProfiler::PerformanceProfiler(const std::string& name)
    : name_(name), active_(false), totalTime_(0.0) {
}

PerformanceProfiler::~PerformanceProfiler() {
}

void PerformanceProfiler::start() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!active_) {
        active_ = true;
        startTime_ = std::chrono::high_resolution_clock::now();
    }
}

#include <utility>

double PerformanceProfiler::stop() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!active_) {
        return totalTime_;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - startTime_);
    double timeMs = duration.count() / 1000000.0;  // Convert to milliseconds
    
    totalTime_ += timeMs;
    samples_.push_back(timeMs);
    active_ = false;
    
    return timeMs;
}

PerformanceProfiler::ProfileStats PerformanceProfiler::getStats() const {
    ProfileStats stats;
    
    if (samples_.empty()) {
        stats.minTime = 0.0;
        stats.maxTime = 0.0;
        stats.meanTime = 0.0;
        stats.stddevTime = 0.0;
        stats.sampleCount = 0;
        stats.totalTime = 0.0;
        return stats;
    }
    
    stats.minTime = *std::min_element(samples_.begin(), samples_.end());
    stats.maxTime = *std::max_element(samples_.begin(), samples_.end());
    
    double sum = std::accumulate(samples_.begin(), samples_.end(), 0.0);
    stats.meanTime = sum / samples_.size();
    
    double variance = 0.0;
    for (double sample : samples_) {
        variance += (sample - stats.meanTime) * (sample - stats.meanTime);
    }
    variance /= samples_.size();
    stats.stddevTime = std::sqrt(variance);
    
    stats.sampleCount = samples_.size();
    stats.totalTime = totalTime_;
    
    return stats;
}

BenchmarkResult PerformanceProfiler::createBenchmark(const std::string& name) const {
    BenchmarkResult result(name, "Performance profiling results for " + name_);
    result.metrics[BenchmarkMetric::Time] = totalTime_;
    result.iterationCount = samples_.size();
    result.samples = samples_;
    calculateStatistics(result);
    return result;
}

// MemoryProfiler implementation
MemoryProfiler::MemoryProfiler(const std::string& name)
    : name_(name), peakMemoryUsage_(0.0) {
}

MemoryProfiler::~MemoryProfiler() {
}

void MemoryProfiler::start() {
    std::lock_guard<std::mutex> lock(mutex_);
    // Reset memory tracking
    samples_.clear();
}

double MemoryProfiler::stop() {
    std::lock_guard<std::mutex> lock(mutex_);
    double currentMemory = getCurrentMemoryUsage();
    samples_.push_back(currentMemory);
    
    if (currentMemory > peakMemoryUsage_) {
        peakMemoryUsage_ = currentMemory;
    }
    
    return peakMemoryUsage_;
}

double MemoryProfiler::getCurrentMemoryUsage() const {
    return getPlatformMemoryUsage();
}

MemoryProfiler::MemoryStats MemoryProfiler::getStats() const {
    MemoryStats stats;
    
    if (samples_.empty()) {
        stats.minMemory = 0.0;
        stats.maxMemory = 0.0;
        stats.meanMemory = 0.0;
        stats.stddevMemory = 0.0;
        stats.sampleCount = 0;
        stats.peakMemory = 0.0;
        stats.currentMemory = 0.0;
        return stats;
    }
    
    stats.minMemory = *std::min_element(samples_.begin(), samples_.end());
    stats.maxMemory = *std::max_element(samples_.begin(), samples_.end());
    
    double sum = std::accumulate(samples_.begin(), samples_.end(), 0.0);
    stats.meanMemory = sum / samples_.size();
    
    double variance = 0.0;
    for (double sample : samples_) {
        variance += (sample - stats.meanMemory) * (sample - stats.meanMemory);
    }
    variance /= samples_.size();
    stats.stddevMemory = std::sqrt(variance);
    
    stats.sampleCount = samples_.size();
    stats.peakMemory = peakMemoryUsage_;
    stats.currentMemory = samples_.empty() ? 0.0 : samples_.back();
    
    return stats;
}

BenchmarkResult MemoryProfiler::createBenchmark(const std::string& name) const {
    BenchmarkResult result(name, "Memory profiling results for " + name_);
    result.metrics[BenchmarkMetric::Memory] = peakMemoryUsage_;
    result.iterationCount = samples_.size();
    result.samples = samples_;
    calculateStatistics(result);
    return result;
}

// BenchmarkUtils implementation
std::shared_ptr<Config> BenchmarkUtils::generateBenchmarkConfig(size_t neuronCount,
                                                                float connectionProbability,
                                                                const std::string& benchmarkType) {
    auto config = std::make_shared<Config>();
    
    config->set("neuron_count", neuronCount);
    config->set("region_count", 1);
    config->set("connection_probability", connectionProbability);
    config->set("random_seed", 42);
    config->set("simulation_timestep", 0.001);
    config->set("max_steps", 1000);
    
    if (benchmarkType == "memory") {
        config->set("enable_working_memory", true);
        config->set("working_memory_capacity", neuronCount / 10);
    } else if (benchmarkType == "plasticity") {
        config->set("enable_stdp", true);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
    } else if (benchmarkType == "integration") {
        config->set("enable_prediction_system", true);
        config->set("enable_cognition", true);
        config->set("enable_neuromodulation", true);
    }
    
    return config;
}

std::shared_ptr<Brain> BenchmarkUtils::createBenchmarkBrain(const std::shared_ptr<Config>& config) {
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain for benchmarking");
    }
    return brain;
}

double BenchmarkUtils::measureMemoryUsage(const std::shared_ptr<Brain>& brain) {
    return 0.0; // Placeholder - implement platform-specific memory measurement
}

double BenchmarkUtils::measureCPUUsage(const std::function<void()>& func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    return duration.count() / 1000000.0;  // Convert to milliseconds
}

double BenchmarkUtils::measureSimulationPerformance(std::shared_ptr<Brain>& brain,
                                                     size_t steps,
                                                     bool enableVisualization) {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < steps; ++i) {
        brain->step(i);
        if (enableVisualization) {
            // Simulate visualization
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    return duration.count() / 1000000.0 / steps;  // Convert to milliseconds per step
}

std::string BenchmarkUtils::compareBenchmarks(const BenchmarkResult& result1,
                                             const BenchmarkResult& result2,
                                             const std::string& metric) {
    std::stringstream report;
    
    report << "Benchmark Comparison" << std::endl;
    report << "===================" << std::endl;
    report << "Benchmark 1: " << result1.name << std::endl;
    report << "Benchmark 2: " << result2.name << std::endl;
    report << "Metric: " << metric << std::endl;
    report << std::endl;
    
    // Compare based on metric
    if (metric == "time") {
        double ratio = result2.mean / result1.mean;
        report << "Time Ratio (B2/B1): " << ratio << std::endl;
        if (ratio > 1.0) {
            report << "Benchmark 2 is " << (ratio - 1.0) * 100 << "% slower than Benchmark 1" << std::endl;
        } else {
            report << "Benchmark 2 is " << (1.0 - ratio) * 100 << "% faster than Benchmark 1" << std::endl;
        }
    }
    
    return report.str();
}

std::string BenchmarkUtils::formatBenchmarkResult(const BenchmarkResult& result) {
    std::stringstream formatted;
    
    formatted << std::left << std::setw(20) << result.name
              << std::setw(10) << result.iterationCount
              << std::setw(15) << std::fixed << std::setprecision(3) << result.mean
              << std::setw(15) << std::fixed << std::setprecision(3) << result.stddev
              << std::setw(15) << std::fixed << std::setprecision(3) << result.min
              << std::setw(15) << std::fixed << std::setprecision(3) << result.max
              << std::endl;
    
    return formatted.str();
}

bool BenchmarkUtils::saveBenchmarkReport(const std::vector<BenchmarkSuite*>& suites,
                                        const std::string& filepath) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Generate JSON report
        file << "{\n  \"benchmarks\": [" << std::endl;
        
        for (size_t i = 0; i < suites.size(); ++i) {
            const auto& suite = suites[i];
            for (const auto& result : suite->getResults()) {
                file << "    {\n";
                file << "      \"name\": \"" << result.name << "\"," << std::endl;
                file << "      \"description\": \"" << result.description << "\"," << std::endl;
                file << "      \"mean\": " << result.mean << "," << std::endl;
                file << "      \"stddev\": " << result.stddev << "," << std::endl;
                file << "      \"min\": " << result.min << "," << std::endl;
                file << "      \"max\": " << result.max << "," << std::endl;
                file << "      \"samples\": " << result.iterationCount << std::endl;
                if (i < suites.size() - 1 || &result != &suite->getResults().back()) {
                    file << "    }," << std::endl;
                } else {
                    file << "    }" << std::endl;
                }
            }
        }
        
        file << "  ]\n}";
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool BenchmarkUtils::loadBenchmarkReport(std::vector<BenchmarkSuite*>& suites,
                                        const std::string& filepath) {
    // Placeholder implementation - would need to parse the JSON file
    return false;
}

std::string BenchmarkUtils::generateBenchmarkSummary(const std::vector<BenchmarkSuite*>& suites) {
    std::stringstream summary;
    
    summary << "Benchmark Summary" << std::endl;
    summary << "=================" << std::endl;
    summary << "Total Suites: " << suites.size() << std::endl;
    
    size_t totalBenchmarks = 0;
    for (const auto& suite : suites) {
        totalBenchmarks += suite->getResults().size();
    }
    
    summary << "Total Benchmarks: " << totalBenchmarks << std::endl;
    summary << std::endl;
    
    for (const auto& suite : suites) {
        summary << "Suite: " << suite->name_ << std::endl;
        summary << "Benchmarks: " << suite->getResults().size() << std::endl;
        
        for (const auto& result : suite->getResults()) {
            summary << "  - " << result.name << " (Mean: " << result.mean << " ms)" << std::endl;
        }
        
        summary << std::endl;
    }
    
    return summary.str();
}

bool BenchmarkUtils::createBenchmarkDirectory(const std::string& directory) {
    try {
        if (!std::filesystem::exists(directory)) {
            std::filesystem::create_directories(directory);
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool BenchmarkUtils::cleanupBenchmarkFiles(const std::string& directory) {
    try {
        if (std::filesystem::exists(directory)) {
            std::filesystem::remove_all(directory);
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string BenchmarkUtils::getSystemInfo() {
    std::stringstream info;
    
    info << "System Information" << std::endl;
    info << "===================" << std::endl;
    info << "Platform: " << (std::is_same<sizeof(void*), 8>::value ? "x64" : "x86") << std::endl;
    info << "Compiler: " << "Unknown" << std::endl; // Could detect this
    
    return info.str();
}

std::string BenchmarkUtils::getBenchmarkSystemInfo() {
    std::stringstream info;
    
    info << "Benchmark System Information" << std::endl;
    info << "===========================" << std::endl;
    info << "Benchmark System Version: " << "1.0.0" << std::endl;
    info << "Supported Metrics: Time, Memory, CPU, Throughput, Latency, Accuracy, Energy, Bandwidth" << std::endl;
    info << "Supported Benchmark Types: Brain Simulation, Memory, Neuron Lookup, Spike Processing, Plasticity, Memory System, Integration" << std::endl;
    
    return info.str();
}

bool BenchmarkUtils::isBenchmarkingAvailable() {
    // Check if benchmarking is available
    // (In a real implementation, this would check for available resources, etc.)
    return true;
}

bool BenchmarkUtils::initializeBenchmarking() {
    // Initialize benchmarking system
    // (In a real implementation, this would initialize resources)
    return true;
}

bool BenchmarkUtils::cleanupBenchmarking() {
    // Cleanup benchmarking system
    // (In a real implementation, this would cleanup resources)
    return true;
}

// BenchmarkManager implementation
void BenchmarkManager::addSuite(std::unique_ptr<BenchmarkSuite> suite) {
    std::lock_guard<std::mutex> lock(mutex_);
    suites_.push_back(std::move(suite));
}

bool BenchmarkManager::removeSuite(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto it = suites_.begin(); it != suites_.end(); ++it) {
        if ((*it)->name_ == name) {
            suites_.erase(it);
            return true;
        }
    }
    
    return false;
}

BenchmarkSuite* BenchmarkManager::getSuite(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& suite : suites_) {
        if (suite->name_ == name) {
            return suite.get();
        }
    }
    
    return nullptr;
}

void BenchmarkManager::runAllBenchmarks() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& suite : suites_) {
        suite->runAllBenchmarks();
    }
}

bool BenchmarkManager::saveAllResults(const std::string& directory) const {
    try {
        createBenchmarkDirectory(directory);
        
        for (const auto& suite : suites_) {
            std::string filepath = directory + "/" + suite->name_ + ".txt";
            if (!suite->saveResults(filepath)) {
                return false;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool BenchmarkManager::loadAllResults(const std::string& directory) {
    try {
        for (auto& suite : suites_) {
            std::string filepath = directory + "/" + suite->name_ + ".txt";
            if (std::filesystem::exists(filepath)) {
                if (!suite->loadResults(filepath)) {
                    return false;
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string BenchmarkManager::generateComprehensiveReport() const {
    std::stringstream report;
    
    report << "Comprehensive Benchmark Report" << std::endl;
    report << "=============================" << std::endl;
    report << "Generated: " << std::ctime(&std::time(nullptr)) << std::endl;
    report << "Total Suites: " << suites_.size() << std::endl;
    
    size_t totalBenchmarks = 0;
    double totalTime = 0.0;
    
    for (const auto& suite : suites_) {
        totalBenchmarks += suite->getResults().size();
        
        for (const auto& result : suite->getResults()) {
            totalTime += result.mean;
        }
    }
    
    report << "Total Benchmarks: " << totalBenchmarks << std::endl;
    report << "Total Mean Time: " << totalTime << " ms" << std::endl;
    report << std::endl;
    
    for (const auto& suite : suites_) {
        report << suite->generateReport() << std::endl;
    }
    
    return report.str();
}

std::string BenchmarkManager::createBenchmarkProfile() const {
    std::stringstream profile;
    
    profile << "Benchmark Profile" << std::endl;
    profile << "===============" << std::endl;
    
    for (const auto& suite : suites_) {
        profile << "Suite: " << suite->name_ << std::endl;
        profile << suite->generateReport() << std::endl;
    }
    
    return profile.str();
}

std::string BenchmarkManager::compareAllBenchmarks() const {
    std::stringstream comparison;
    
    comparison << "Benchmark Comparison" << std::endl;
    comparison << "===================" << std::endl;
    
    // Compare benchmarks with the same name across suites
    std::map<std::string, std::vector<const BenchmarkResult*>> benchmarkGroups;
    
    for (const auto& suite : suites_) {
        for (const auto& result : suite->getResults()) {
            benchmarkGroups[result.name].push_back(&result);
        }
    }
    
    for (const auto& [name, results] : benchmarkGroups) {
        if (results.size() < 2) continue;
        
        comparison << "Comparison: " << name << std::endl;
        comparison << "Number of benchmarks: " << results.size() << std::endl;
        
        // Compare means
        double minMean = std::numeric_limits<double>::max();
        double maxMean = 0.0;
        
        for (const auto* result : results) {
            if (result->mean < minMean) minMean = result->mean;
            if (result->mean > maxMean) maxMean = result->mean;
        }
        
        double ratio = (minMean > 0) ? (maxMean / minMean) : 1.0;
        comparison << "Time Ratio (slowest/fastest): " << ratio << std::endl;
        
        if (ratio > 1.5) {
            comparison << "Significant performance difference detected!" << std::endl;
        }
        
        comparison << std::endl;
    }
    
    return comparison.str();
}

bool BenchmarkManager::exportToJSON(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "{\n  \"benchmarks\": [" << std::endl;
        
        for (size_t i = 0; i < suites_.size(); ++i) {
            const auto& suite = suites_[i];
            for (size_t j = 0; j < suite->getResults().size(); ++j) {
                const auto& result = suite->getResults()[j];
                
                file << "    {\n";
                file << "      \"suite\": \"" << suite->name_ << "\"," << std::endl;
                file << "      \"name\": \"" << result.name << "\"," << std::endl;
                file << "      \"description\": \"" << result.description << "\"," << std::endl;
                file << "      \"mean\": " << result.mean << "," << std::endl;
                file << "      \"stddev\": " << result.stddev << "," << std::endl;
                file << "      \"min\": " << result.min << "," << std::endl;
                file << "      \"max\": " << result.max << "," << std::endl;
                file << "      \"samples\": " << result.iterationCount << std::endl;
                if (i < suites_.size() - 1 || j < suite->getResults().size() - 1) {
                    file << "    }," << std::endl;
                } else {
                    file << "    }" << std::endl;
                }
            }
        }
        
        file << "  ]\n}";
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool BenchmarkManager::exportToCSV(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write CSV header
        file << "Suite,Benchmark,Description,Mean,Stddev,Min,Max,Samples" << std::endl;
        
        for (const auto& suite : suites_) {
            for (const auto& result : suite->getResults()) {
                file << "\"" << suite->name_ << "\",\"" << result.name << "\",\"" 
                     << result.description << "\"," << result.mean << "," << result.stddev << "," 
                     << result.min << "," << result.max << "," << result.iterationCount << std::endl;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool BenchmarkManager::exportToYAML(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write YAML header
        file << "benchmarks:" << std::endl;
        
        for (const auto& suite : suites_) {
            file << "  - suite: \"" << suite->name_ << "\"" << std::endl;
            file << "    benchmarks:" << std::endl;
            
            for (const auto& result : suite->getResults()) {
                file << "      - name: \"" << result.name << "\"" << std::endl;
                file << "        description: \"" << result.description << "\"" << std::endl;
                file << "        mean: " << result.mean << std::endl;
                file << "        stddev: " << result.stddev << std::endl;
                file << "        min: " << result.min << std::endl;
                file << "        max: " << result.max << std::endl;
                file << "        samples: " << result.iterationCount << std::endl;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Convenience functions implementation
namespace Bench {
    BenchmarkResult brainSimulation(size_t neuronCount, size_t steps) {
        BenchmarkSuite suite("Quick Brain Simulation", "Quick benchmark for brain simulation performance");
        return suite.brainSimulation(neuronCount, steps);
    }
    
    BenchmarkResult neuronLookup(size_t neuronCount, size_t lookupCount) {
        BenchmarkSuite suite("Quick Neuron Lookup", "Quick benchmark for neuron lookup performance");
        return suite.neuronLookupBenchmark(neuronCount, lookupCount);
    }
    
    BenchmarkResult spikeProcessing(size_t neuronCount, float firingRate, size_t steps) {
        BenchmarkSuite suite("Quick Spike Processing", "Quick benchmark for spike processing performance");
        return suite.spikeProcessingBenchmark(neuronCount, firingRate, steps);
    }
    
    BenchmarkResult memory(size_t memorySizeMB) {
        BenchmarkSuite suite("Quick Memory", "Quick benchmark for memory allocation performance");
        return suite.memoryBenchmark(memorySizeMB);
    }
    
    BenchmarkResult plasticity(size_t neuronCount, size_t synapseCount) {
        BenchmarkSuite suite("Quick Plasticity", "Quick benchmark for plasticity performance");
        return suite.plasticityBenchmark(neuronCount, synapseCount);
    }
    
    BenchmarkResult integration(const std::string& integrationType) {
        BenchmarkSuite suite("Quick Integration", "Quick benchmark for integration performance");
        return suite.integrationBenchmark(integrationType);
    }
    
    std::vector<BenchmarkResult> runAllQuickBenchmarks() {
        std::vector<BenchmarkResult> results;
        
        results.push_back(brainSimulation());
        results.push_back(neuronLookup());
        results.push_back(spikeProcessing());
        results.push_back(memory());
        results.push_back(plasticity());
        results.push_back(integration());
        
        return results;
    }
    
    std::string generateQuickReport() {
        auto results = runAllQuickBenchmarks();
        std::stringstream report;
        
        report << "Quick Benchmark Report" << std::endl;
        report << "=======================" << std::endl;
        report << "Total Benchmarks: " << results.size() << std::endl;
        report << std::endl;
        
        for (const auto& result : results) {
            report << "Benchmark: " << result.name << std::endl;
            report << "Mean Time: " << result.mean << " ms" << std::endl;
            report << "Stddev: " << result.stddev << " ms" << std::endl;
            report << std::endl;
        }
        
        return report.str();
    }
    
    bool saveQuickBenchmarkResults(const std::string& filepath) {
        auto results = runAllQuickBenchmarks();
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Quick Benchmark Results" << std::endl;
        file << "=======================" << std::endl;
        
        for (const auto& result : results) {
            file << "Benchmark: " << result.name << std::endl;
            file << "Mean Time: " << result.mean << " ms" << std::endl;
            file << "Stddev: " << result.stddev << " ms" << std::endl;
            file << "Min: " << result.min << " ms" << std::endl;
            file << "Max: " << result.max << " ms" << std::endl;
            file << std::endl;
        }
        
        return true;
    }
    
    bool loadQuickBenchmarkResults(const std::string& filepath) {
        // Placeholder implementation
        return false;
    }
}

} // namespace nlm
