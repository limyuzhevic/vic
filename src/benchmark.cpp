// NLM Performance Profiling and Benchmarking Infrastructure
// Comprehensive performance measurement, analysis, and reporting

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"

#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <memory>
#include <algorithm>
#include <numeric>
#include <map>
#include <unordered_map>
#include <fstream>
#include <cmath>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>

using namespace nlm;

// Comprehensive performance metrics collection
struct DetailedBenchmarkMetrics {
    // Network topology metrics
    size_t neuronCount = 0;
    size_t synapseCount = 0;
    size_t regionCount = 0;
    size_t activeNeuronCount = 0;
    size_t firingNeuronCount = 0;
    float averageFiringRate = 0.0f;
    float excitationInhibitionRatio = 0.0f;
    
    // Performance metrics
    double wallClockTime = 0.0;        // Real time
    double cpuTime = 0.0;              // CPU time
    double simulationTime = 0.0;       // Simulation time
    size_t stepsPerSecond = 0;
    size_t stepsExecuted = 0;
    
    // Neural activity metrics
    size_t totalSpikeCount = 0;
    double spikesPerSecond = 0.0;
    double averageFiringFrequency = 0.0; // Hz
    size_t mostActiveNeurons = 0;
    double activityCoefficient = 0.0;    // Measure of network activity
    
    // Plasticity metrics
    size_t synapticWeightChanges = 0;
    float weightChangeMagnitude = 0.0f;
    float averageWeight = 0.0f;
    float weightStandardDeviation = 0.0f;
    float weightRange = 0.0f;
    
    // Memory and resource metrics
    size_t memoryUsage = 0;            // In bytes
    size_t peakMemoryUsage = 0;
    size_t checkpointSize = 0;
    
    // Structural plasticity metrics
    size_t synaptogenesisEvents = 0;
    size_t pruningEvents = 0;
    float synaptogenesisRate = 0.0f;
    float pruningRate = 0.0f;
    
    // Temporal metrics
    double timeStep = 0.0;
    double simulationDuration = 0.0;
    std::vector<double> stepTimes;      // Individual step times for analysis
    
    // Configuration metrics
    std::string configHash;             // Hash of configuration for reproducibility
    std::map<std::string, std::string> configValues;
    
    // System metrics
    double cpuUsage = 0.0;              // Percentage
    int systemLoad = 0;                 // 1-100
    
    // Learning metrics
    float learningCurveProgress = 0.0f;
    double adaptationRate = 0.0;
    float neuromodulationLevel = 0.0f;
    
    Timestamp simulationStartTime = 0.0;
    Timestamp simulationEndTime = 0.0;
    
    DetailedBenchmarkMetrics() = default;
    
    // Calculate derived metrics
    void calculateDerivedMetrics() {
        if (stepsExecuted > 0) {
            stepsPerSecond = static_cast<size_t>(stepsExecuted / wallClockTime);
            spikesPerSecond = totalSpikeCount / wallClockTime;
            averageFiringFrequency = spikesPerSecond / neuronCount * 1000.0f; // Convert to Hz
        }
        
        if (weightCount > 0) {
            weightRange = weightMax - weightMin;
        }
        
        if (activeNeuronCount > 0) {
            activityCoefficient = static_cast<double>(firingNeuronCount) / activeNeuronCount;
        }
        
        // Calculate standard deviation of weights
        if (weightCount > 1) {
            double sum = 0.0;
            double sumSq = 0.0;
            for (float weight : weightSamples) {
                sum += weight;
                sumSq += weight * weight;
            }
            double mean = sum / weightCount;
            weightStandardDeviation = std::sqrt((sumSq - sum * sum / weightCount) / (weightCount - 1));
        }
        
        // Calculate learning curve progress (simplified)
        if (totalSpikeCount > 0) {
            learningCurveProgress = std::min(1.0f, totalSpikeCount / 10000.0f);
        }
    }
    
    // Get performance score (normalized 0-100)
    double getPerformanceScore() const {
        double score = 0.0;
        
        // Step throughput (40%)
        double stepsScore = std::min(1.0, stepsPerSecond / 100000.0);
        score += stepsScore * 0.4;
        
        // Spike activity (20%)
        double spikeScore = std::min(1.0, spikesPerSecond / 1000.0);
        score += spikeScore * 0.2;
        
        // Network activity (20%)
        double activityScore = std::min(1.0, activityCoefficient);
        score += activityScore * 0.2;
        
        // Plasticity (10%)
        double plasticityScore = std::min(1.0, synapticWeightChanges / 1000.0);
        score += plasticityScore * 0.1;
        
        // Learning efficiency (10%)
        double learningScore = learningCurveProgress;
        score += learningScore * 0.1;
        
        return score * 100.0;
    }
    
    // Format metrics for output
    std::string formatForCSV() const {
        std::stringstream ss;
        ss << neuronCount << ","
           << synapseCount << ","
           << stepsPerSecond << ","
           << spikesPerSecond << ","
           << averageFiringFrequency << ","
           << wallClockTime << ","
           << stepsExecuted << ","
           << totalSpikeCount << ","
           << excitationInhibitionRatio << ","
           << synapticWeightChanges << ","
           << weightChangeMagnitude << ","
           << synaptogenesisEvents << ","
           << pruningEvents << ","
           << memoryUsage << ","
           << peakMemoryUsage;
        return ss.str();
    }
    
    // Format metrics for human-readable output
    std::string formatForHuman() const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Neurons: " << neuronCount << " | "
           << "Synapses: " << synapseCount << " | "
           << "Steps/sec: " << stepsPerSecond << " | "
           << "Spikes/sec: " << spikesPerSecond << " | "
           << "Firing Hz: " << averageFiringFrequency << " | "
           << "Wall Time: " << wallClockTime << "s | "
           << "Simulation Time: " << simulationTime << "s | "
           << "Activity: " << activityCoefficient << " | "
           << "Learning Score: " << getPerformanceScore();
        return ss.str();
    }
};

// Advanced benchmarking suite with multiple test types
class AdvancedBenchmarkSuite {
public:
    struct BenchmarkConfig {
        size_t neuronCount = 1000;
        size_t steps = 1000;
        size_t warmupSteps = 100;
        double timeStep = 0.001;
        std::string name = "default";
        bool enablePlasticity = true;
        bool enableDevelopment = false;
        bool enableNeuromodulation = true;
        bool enableMemory = true;
        std::string scenario = "balanced"; // balanced, active, quiet, fluctuating
    };
    
    struct BenchmarkProfile {
        std::string name;
        std::vector<BenchmarkConfig> configs;
        std::function<void(BenchmarkConfig&, DetailedBenchmarkMetrics&)> setupFunction;
        std::function<void(BenchmarkConfig&, DetailedBenchmarkMetrics&, size_t)> stepFunction;
        std::function<void(BenchmarkConfig&, DetailedBenchmarkMetrics&)> finalizeFunction;
    };
    
    AdvancedBenchmarkSuite() {
        initializeBenchmarkProfiles();
    }
    
    // Run a single benchmark with comprehensive metrics collection
    DetailedBenchmarkMetrics runBenchmark(const BenchmarkConfig& config) {
        NLM_LOG_INFO(std::string("Running benchmark: ") + config.name + " - " +
                    std::to_string(config.neuronCount) + " neurons, " +
                    std::to_string(config.steps) + " steps");
        
        DetailedBenchmarkMetrics metrics;
        metrics.neuronCount = config.neuronCount;
        metrics.simulationStartTime = 0.0;
        metrics.timeStep = config.timeStep;
        
        // Create configuration
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
        cfg->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        cfg->set("connection_probability", getConnectionProbability(config.scenario), ConfigSource::Default);
        cfg->set("simulation_timestep", config.timeStep, ConfigSource::Default);
        cfg->set("random_seed", 42, ConfigSource::Default);
        
        if (config.enablePlasticity) {
            cfg->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
            cfg->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
            cfg->set("stdp_tau", 20.0f, ConfigSource::Default);
            cfg->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
            cfg->set("pruning_rate", 0.00001f, ConfigSource::Default);
        }
        
        if (config.enableDevelopment) {
            cfg->set("development_stage", static_cast<int64_t>(2), ConfigSource::Default); // Critical period
        }
        
        if (config.enableNeuromodulation) {
            cfg->set("dopamine_enabled", true, ConfigSource::Default);
            cfg->set("curiosity_enabled", true, ConfigSource::Default);
        }
        
        if (config.enableMemory) {
            cfg->set("working_memory_capacity", static_cast<int64_t>(config.neuronCount / 10), ConfigSource::Default);
            cfg->set("episodic_memory_max_episodes", 1000, ConfigSource::Default);
        }
        
        // Create brain with timing
        auto brain = std::make_shared<Brain>(cfg);
        
        // Start timing
        auto wallStart = std::chrono::high_resolution_clock::now();
        auto cpuStart = std::chrono::system_clock::now();
        
        // Initialize brain
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for benchmark");
            return metrics;
        }
        
        metrics.simulationStartTime = 0.0;
        
        // Warmup phase
        NLM_LOG_INFO("  Warming up...");
        for (size_t step = 0; step < config.warmupSteps; ++step) {
            brain->step(step, step * config.timeStep);
            
            // Inject warmup activity based on scenario
            injectScenarioActivity(brain, config.scenario, step);
        }
        
        // Reset metrics after warmup
        metrics.totalSpikeCount = 0;
        metrics.stepsExecuted = 0;
        
        // Reset brain for actual benchmark
        brain->reset();
        brain->initialize();
        
        // Main benchmark loop with detailed metrics collection
        NLM_LOG_INFO("  Starting benchmark loop...");
        
        auto stepStart = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < config.steps; ++step) {
            auto stepCpuStart = std::chrono::system_clock::now();
            
            // Inject scenario-specific activity
            injectScenarioActivity(brain, config.scenario, step);
            
            // Execute simulation step
            brain->step(step, step * config.timeStep);
            
            // Collect metrics
            collectBenchmarkMetrics(brain, metrics, step);
            
            // Record step time
            auto stepEnd = std::chrono::high_resolution_clock::now();
            double stepTime = std::chrono::duration<double>(stepEnd - stepStart).count();
            metrics.stepTimes.push_back(stepTime);
            stepStart = std::chrono::high_resolution_clock::now();
            
            // Log progress every 10% or every 100 steps
            if (step % std::max(size_t(100), config.steps / 10) == 0) {
                NLM_LOG_INFO("    Progress: " + std::to_string((step * 100) / config.steps) + "% - " +
                            metrics.formatForHuman());
            }
        }
        
        // Finalize timing
        auto wallEnd = std::chrono::high_resolution_clock::now();
        auto cpuEnd = std::chrono::system_clock::now();
        
        metrics.wallClockTime = std::chrono::duration<double>(wallEnd - wallStart).count();
        metrics.simulationTime = metrics.wallClockTime - config.warmupSteps * config.timeStep;
        metrics.simulationEndTime = metrics.wallClockTime;
        
        // Convert CPU time to seconds (approximate)
        metrics.cpuTime = std::chrono::duration<double>(cpuEnd - cpuStart).count();
        
        // Calculate derived metrics
        metrics.calculateDerivedMetrics();
        
        // Collect system metrics
        collectSystemMetrics(metrics);
        
        // Collect memory metrics
        collectMemoryMetrics(metrics);
        
        NLM_LOG_INFO("  Benchmark completed: " + metrics.formatForHuman());
        
        return metrics;
    }
    
    // Run benchmark suite with multiple configurations
    std::vector<DetailedBenchmarkMetrics> runBenchmarkSuite(const std::vector<BenchmarkConfig>& configs) {
        std::vector<DetailedBenchmarkMetrics> results;
        results.reserve(configs.size());
        
        for (const auto& config : configs) {
            DetailedBenchmarkMetrics metrics = runBenchmark(config);
            metrics.name = config.name;
            results.push_back(metrics);
            
            // Small delay between benchmarks to allow system stabilization
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        return results;
    }
    
    // Save benchmark results to file
    bool saveResults(const std::vector<DetailedBenchmarkMetrics>& results, const std::string& filename, const std::string& format = "csv") {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open benchmark results file: " + filename);
            return false;
        }
        
        if (format == "csv") {
            // Write CSV header
            file << "benchmark_name,neurons,synapses,regions,steps_per_second,spikes_per_second," <<
                     "average_firing_frequency,wall_clock_time,simulation_time,total_spikes," <<
                     "activity_coefficient,excitement_inhibition_ratio,synaptic_weight_changes," <<
                     "weight_change_magnitude,synaptogenesis_events,pruning_events,memory_usage," <<
                     "peak_memory_usage,learning_score\n";
            
            // Write results
            for (const auto& metrics : results) {
                file << metrics.name << "," << metrics.formatForCSV() << "\n";
            }
        } else if (format == "json") {
            file << "{\n";
            file << "  \"benchmarks\": [\n";
            for (size_t i = 0; i < results.size(); ++i) {
                const auto& metrics = results[i];
                file << "    {\n";
                file << "      \"name\": \"" << metrics.name << "\",\n";
                file << "      \"neuron_count\": " << metrics.neuronCount << ",\n";
                file << "      \"synapse_count\": " << metrics.synapseCount << ",\n";
                file << "      \"steps_per_second\": " << metrics.stepsPerSecond << ",\n";
                file << "      \"spikes_per_second\": " << metrics.spikesPerSecond << ",\n";
                file << "      \"performance_score\": " << metrics.getPerformanceScore() << "\n";
                if (i < results.size() - 1) {
                    file << "    },\n";
                } else {
                    file << "    }\n";
                }
            }
            file << "  ]\n";
            file << "}\n";
        } else {
            NLM_LOG_ERROR("Unsupported benchmark format: " + format);
            return false;
        }
        
        file.close();
        NLM_LOG_INFO("Benchmark results saved to: " + filename);
        return true;
    }
    
    // Generate benchmark report
    std::string generateReport(const std::vector<DetailedBenchmarkMetrics>& results) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        
        ss << "╔════════════════════════════════════════════════════════════════════════════════╗\n";
        ss << "║                          NLM BENCHMARK REPORT                                 ║\n";
        ss << "╠════════════════════════════════════════════════════════════════════════════════╣\n";
        
        if (results.empty()) {
            ss << "║                    No benchmark results available                             ║\n";
            ss << "╚════════════════════════════════════════════════════════════════════════════════╝\n";
            return ss.str();
        }
        
        ss << "║ Total benchmarks run: " << results.size() << "                                       ║\n";
        ss << "║ Time range: " << results.front().wallClockTime << "s to " << results.back().wallClockTime << "s    ║\n";
        
        // Calculate statistics
        double avgNeurons = 0, avgStepsPerSec = 0, avgPerformanceScore = 0;
        size_t totalNeurons = 0;
        
        for (const auto& metrics : results) {
            avgNeurons += metrics.neuronCount;
            avgStepsPerSec += metrics.stepsPerSecond;
            avgPerformanceScore += metrics.getPerformanceScore();
            totalNeurons += metrics.neuronCount;
        }
        
        avgNeurons /= results.size();
        avgStepsPerSec /= results.size();
        avgPerformanceScore /= results.size();
        
        ss << "║ Average neurons: " << avgNeurons << "                              ║\n";
        ss << "║ Average steps/sec: " << avgStepsPerSec << "                        ║\n";
        ss << "║ Average performance score: " << avgPerformanceScore << "/100                 ║\n";
        
        // Find best and worst performers
        auto best = std::max_element(results.begin(), results.end(),
            [](const DetailedBenchmarkMetrics& a, const DetailedBenchmarkMetrics& b) {
                return a.getPerformanceScore() < b.getPerformanceScore();
            });
        
        auto worst = std::min_element(results.begin(), results.end(),
            [](const DetailedBenchmarkMetrics& a, const DetailedBenchmarkMetrics& b) {
                return a.getPerformanceScore() < b.getPerformanceScore();
            });
        
        ss << "║ Best performer: " << best->name << " (score: " << best->getPerformanceScore() << ")            ║\n";
        ss << "║ Worst performer: " << worst->name << " (score: " << worst->getPerformanceScore() << ")            ║\n";
        
        ss << "╚════════════════════════════════════════════════════════════════════════════════╝\n";
        
        // Detailed results table
        ss << "\nDetailed Results:\n\n";
        ss << std::setw(15) << "Benchmark" << std::setw(12) << "Neurons" << std::setw(12) << "Synapses" 
           << std::setw(14) << "Steps/sec" << std::setw(14) << "Spikes/sec" << std::setw(14) << "Activity" 
           << std::setw(12) << "Score\n";
        ss << std::setfill('-') << std::setw(95) << "" << std::setfill(' ') << "\n";
        
        for (const auto& metrics : results) {
            ss << std::setw(15) << metrics.name << std::setw(12) << metrics.neuronCount 
               << std::setw(12) << metrics.synapseCount << std::setw(14) << metrics.stepsPerSecond
               << std::setw(14) << metrics.spikesPerSecond << std::setw(14) << metrics.activityCoefficient
               << std::setw(12) << metrics.getPerformanceScore() << "\n";
        }
        
        return ss.str();
    }
    
private:
    float getConnectionProbability(const std::string& scenario) {
        if (scenario == "active") return 0.3f;
        else if (scenario == "quiet") return 0.05f;
        else if (scenario == "fluctuating") return 0.2f;
        else return 0.1f; // balanced
    }
    
    void initializeBenchmarkProfiles() {
        // Balanced benchmark profile
        BenchmarkConfig balancedConfig;
        balancedConfig.name = "balanced_1k";
        balancedConfig.neuronCount = 1000;
        balancedConfig.steps = 1000;
        balancedConfig.scenario = "balanced";
        
        // Active benchmark profile
        BenchmarkConfig activeConfig;
        activeConfig.name = "active_1k";
        activeConfig.neuronCount = 1000;
        activeConfig.steps = 1000;
        activeConfig.scenario = "active";
        
        // Quiet benchmark profile
        BenchmarkConfig quietConfig;
        quietConfig.name = "quiet_1k";
        quietConfig.neuronCount = 1000;
        quietConfig.steps = 1000;
        quietConfig.scenario = "quiet";
        
        // Create default profile
        BenchmarkProfile profile;
        profile.name = "default_benchmarks";
        profile.configs = {balancedConfig, activeConfig, quietConfig};
        
        profiles_["default"] = profile;
    }
    
    void injectScenarioActivity(std::shared_ptr<Brain> brain, const std::string& scenario, size_t step) {
        if (scenario == "active") {
            // High activity for active scenario
            brain->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
            brain->injectCurrentToNeurons(NeuronType::Internal, 20.0f);
        } else if (scenario == "quiet") {
            // Low activity for quiet scenario
            if (step % 10 == 0) { // Every 10th step
                brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
            }
        } else if (scenario == "fluctuating") {
            // Fluctuating activity
            float baseActivity = 15.0f + 10.0f * std::sin(step * 0.1f);
            brain->injectCurrentToNeurons(NeuronType::Sensory, baseActivity);
        } else {
            // Balanced scenario
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        }
    }
    
    void collectBenchmarkMetrics(std::shared_ptr<Brain> brain, DetailedBenchmarkMetrics& metrics, size_t step) {
        // Update basic metrics
        metrics.stepsExecuted = step + 1;
        metrics.totalSpikeCount = brain->getTotalSpikeCount();
        metrics.activeNeuronCount = brain->getActiveNeuronCount();
        metrics.firingNeuronCount = brain->getFiringNeuronCount();
        metrics.averageFiringRate = brain->getAverageFiringRate();
        metrics.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        
        // Collect plasticity metrics
        if (auto* stdp = brain->getSTDP()) {
            // Record weight changes if available
        }
        
        if (auto* plasticity = brain->getStructuralPlasticity()) {
            metrics.synaptogenesisEvents = plasticity->getSynaptogenesisCount();
            metrics.pruningEvents = plasticity->getPruningCount();
            metrics.synaptogenesisRate = plasticity->getSynaptogenesisRate();
            metrics.pruningRate = plasticity->getPruningRate();
        }
        
        // Collect memory metrics
        if (auto* memory = brain->getWorkingMemory()) {
            // Record memory usage
        }
        
        // Collect neuromodulation metrics
        if (auto* dopamine = brain->getDopamine()) {
            metrics.neuromodulationLevel = dopamine->getLevel();
        }
        
        // Update region count
        metrics.regionCount = brain->getRegionCount();
        metrics.synapseCount = brain->getTotalSynapseCount();
    }
    
    void collectSystemMetrics(DetailedBenchmarkMetrics& metrics) {
        // Collect basic system metrics
        metrics.cpuUsage = 50.0; // Placeholder
        metrics.systemLoad = 30; // Placeholder
    }
    
    void collectMemoryMetrics(DetailedBenchmarkMetrics& metrics) {
        // Collect memory usage (placeholder)
        metrics.memoryUsage = 0; // Placeholder
        metrics.peakMemoryUsage = 0; // Placeholder
        metrics.checkpointSize = 0; // Placeholder
    }
    
    std::vector<BenchmarkProfile> profiles_;
    std::mutex metricsMutex_;
    std::atomic<size_t> weightCount_{0};
    std::vector<float> weightSamples_;
    float weightMin_{0.0f};
    float weightMax_{0.0f};
};

// Command-line benchmark tool
class BenchmarkTool {
public:
    static void runBenchmarks(const std::vector<std::string>& args) {
        NLM_LOG_INFO("=== NLM Advanced Benchmarking Suite ===");
        
        // Default benchmark configurations
        std::vector<AdvancedBenchmarkSuite::BenchmarkConfig> configs = {
            {"small_100", 100, 500, 50, 0.001, "balanced"},
            {"medium_500", 500, 1000, 100, 0.001, "balanced"},
            {"large_1000", 1000, 1000, 100, 0.001, "balanced"},
            {"xlarge_2000", 2000, 500, 50, 0.001, "balanced"},
            {"active_1k", 1000, 1000, 100, 0.001, "active"},
            {"quiet_1k", 1000, 1000, 100, 0.001, "quiet"},
            {"fluctuating_1k", 1000, 1000, 100, 0.001, "fluctuating"},
        };
        
        // Parse command line arguments for benchmark customization
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == "--neuron-count" && i + 1 < args.size()) {
                int count = std::stoi(args[i + 1]);
                for (auto& config : configs) {
                    config.neuronCount = count;
                }
                i++; // Skip next argument
            } else if (args[i] == "--steps" && i + 1 < args.size()) {
                int steps = std::stoi(args[i + 1]);
                for (auto& config : configs) {
                    config.steps = steps;
                }
                i++; // Skip next argument
            } else if (args[i] == "--scenario" && i + 1 < args.size()) {
                std::string scenario = args[i + 1];
                for (auto& config : configs) {
                    config.scenario = scenario;
                }
                i++; // Skip next argument
            } else if (args[i] == "--output" && i + 1 < args.size()) {
                std::string outputFile = args[i + 1];
                i++; // Skip next argument
                
                // Run benchmarks and save results
                AdvancedBenchmarkSuite suite;
                std::vector<DetailedBenchmarkMetrics> results;
                
                for (const auto& config : configs) {
                    NLM_LOG_INFO("Running benchmark: " + config.name);
                    DetailedBenchmarkMetrics metrics = suite.runBenchmark(config);
                    results.push_back(metrics);
                }
                
                // Save results
                bool saveCSV = suite.saveResults(results, outputFile + ".csv", "csv");
                bool saveJSON = suite.saveResults(results, outputFile + ".json", "json");
                
                if (saveCSV && saveJSON) {
                    // Generate and display report
                    std::string report = suite.generateReport(results);
                    std::cout << report << std::endl;
                    
                    // Save report to file
                    std::string reportFile = outputFile + "_report.txt";
                    std::ofstream reportOut(reportFile);
                    if (reportOut.is_open()) {
                        reportOut << report;
                        reportOut.close();
                    }
                }
                
                return;
            }
        }
        
        // Run benchmarks with default configurations
        AdvancedBenchmarkSuite suite;
        std::vector<DetailedBenchmarkMetrics> results;
        
        NLM_LOG_INFO("Running benchmark suite with " + std::to_string(configs.size()) + " configurations");
        
        for (const auto& config : configs) {
            NLM_LOG_INFO("Running benchmark: " + config.name);
            DetailedBenchmarkMetrics metrics = suite.runBenchmark(config);
            results.push_back(metrics);
        }
        
        // Display summary
        std::string report = suite.generateReport(results);
        std::cout << report << std::endl;
        
        // Save default results
        suite.saveResults(results, "benchmark_results", "csv");
        suite.saveResults(results, "benchmark_results", "json");
        
        NLM_LOG_INFO("Benchmark suite completed successfully");
    }
};