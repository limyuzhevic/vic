// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration with Enhanced User Interface
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
// - Integrated memory, prediction, cognition, and neuromodulation systems

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase5Experiment.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <random>
#include <cmath>

using namespace nlm;

// Forward declarations for enhanced features
class DataExporter;
class PerformanceMonitor;
class ResearchModeHandler;
class InteractiveModeHandler;
class BatchModeHandler;

// Enhanced command-line argument parser
class CommandLineParser {
public:
    struct Options {
        std::string mode = "demo";  // demo, research, interactive, batch
        std::string experiment = "phase6";  // phase3, phase4, phase5, phase6
        std::string config_file = "configs/default.cfg";
        std::string output_file = "nlm_results.json";
        std::string checkpoint_file = "";
        uint64_t seed = 42;
        size_t neurons = 500;
        size_t steps = 2000;
        size_t regions = 1;
        float connection_prob = 0.15f;
        bool verbose = false;
        bool benchmark = false;
        bool save_results = false;
        bool enable_export = false;
        std::string export_format = "json";  // json, csv, txt
        size_t replay_interval = 100;
        bool enable_replay = true;
        bool enable_development = true;
        size_t development_stages = 4;
        float plasticity_mod = 1.0f;
        size_t max_experiments = 10;
        std::string help_text = "";  // Custom help text
        
        // Research mode specific
        bool parameter_exploration = false;
        std::vector<std::string> parameter_ranges;
        size_t parallel_experiments = 1;
        
        // Advanced commands
        bool enable_visualization = false;
        bool enable_profiling = false;
        bool enable_debug = false;
        bool enable_validation = false;
        
        // File and directory options
        std::string results_dir = "results/";
        std::string logs_dir = "logs/";
        std::string checkpoints_dir = "checkpoints/";
        std::string exports_dir = "exports/";
        
        // Time and resource options
        double timeout = 0.0;  // No timeout by default
        size_t memory_limit = 0;  // No memory limit by default
        size_t cpu_cores = std::thread::hardware_concurrency();
    };

    static Options parse(int argc, char** argv) {
        Options options;
        std::vector<std::string> args(argv + 1, argv + argc);
        
        for (size_t i = 0; i < args.size(); ++i) {
            std::string arg = args[i];
            
            if (arg == "--help" || arg == "-h") {
                showHelp();
                exit(0);
            }
            else if (arg == "--version") {
                showVersion();
                exit(0);
            }
            else if (arg == "--mode" && i + 1 < args.size()) {
                options.mode = args[++i];
            }
            else if (arg == "--experiment" && i + 1 < args.size()) {
                options.experiment = args[++i];
            }
            else if (arg == "--config" && i + 1 < args.size()) {
                options.config_file = args[++i];
            }
            else if (arg == "--output" && i + 1 < args.size()) {
                options.output_file = args[++i];
            }
            else if (arg == "--checkpoint" && i + 1 < args.size()) {
                options.checkpoint_file = args[++i];
            }
            else if (arg == "--seed" && i + 1 < args.size()) {
                options.seed = std::stoull(args[++i]);
            }
            else if (arg == "--neurons" && i + 1 < args.size()) {
                options.neurons = std::stoull(args[++i]);
            }
            else if (arg == "--steps" && i + 1 < args.size()) {
                options.steps = std::stoull(args[++i]);
            }
            else if (arg == "--regions" && i + 1 < args.size()) {
                options.regions = std::stoull(args[++i]);
            }
            else if (arg == "--connection-prob" && i + 1 < args.size()) {
                options.connection_prob = std::stof(args[++i]);
            }
            else if (arg == "--verbose" || arg == "-v") {
                options.verbose = true;
            }
            else if (arg == "--benchmark") {
                options.benchmark = true;
            }
            else if (arg == "--save-results") {
                options.save_results = true;
            }
            else if (arg == "--enable-export") {
                options.enable_export = true;
            }
            else if (arg == "--export-format" && i + 1 < args.size()) {
                options.export_format = args[++i];
            }
            else if (arg == "--replay-interval" && i + 1 < args.size()) {
                options.replay_interval = std::stoull(args[++i]);
            }
            else if (arg == "--no-replay") {
                options.enable_replay = false;
            }
            else if (arg == "--no-development") {
                options.enable_development = false;
            }
            else if (arg == "--plasticity-mod" && i + 1 < args.size()) {
                options.plasticity_mod = std::stof(args[++i]);
            }
            else if (arg == "--max-experiments" && i + 1 < args.size()) {
                options.max_experiments = std::stoull(args[++i]);
            }
            else if (arg == "--enable-visualization") {
                options.enable_visualization = true;
            }
            else if (arg == "--enable-profiling") {
                options.enable_profiling = true;
            }
            else if (arg == "--enable-debug") {
                options.enable_debug = true;
            }
            else if (arg == "--enable-validation") {
                options.enable_validation = true;
            }
            else if (arg == "--parameter-exploration") {
                options.parameter_exploration = true;
            }
            else if (arg == "--parallel-experiments" && i + 1 < args.size()) {
                options.parallel_experiments = std::stoull(args[++i]);
            }
            else if (arg == "--results-dir" && i + 1 < args.size()) {
                options.results_dir = args[++i];
            }
            else if (arg == "--logs-dir" && i + 1 < args.size()) {
                options.logs_dir = args[++i];
            }
            else if (arg == "--checkpoints-dir" && i + 1 < args.size()) {
                options.checkpoints_dir = args[++i];
            }
            else if (arg == "--exports-dir" && i + 1 < args.size()) {
                options.exports_dir = args[++i];
            }
            else if (arg == "--timeout" && i + 1 < args.size()) {
                options.timeout = std::stod(args[++i]);
            }
            else if (arg == "--memory-limit" && i + 1 < args.size()) {
                options.memory_limit = std::stoull(args[++i]);
            }
            else if (arg == "--cpu-cores" && i + 1 < args.size()) {
                options.cpu_cores = std::stoull(args[++i]);
            }
            else {
                // Try to parse as key=value
                size_t eq_pos = arg.find('=');
                if (eq_pos != std::string::npos) {
                    std::string key = arg.substr(0, eq_pos);
                    std::string value = arg.substr(eq_pos + 1);
                    options.setConfigValue(key, value);
                }
                else {
                    std::cerr << "Unknown argument: " << arg << std::endl;
                    showHelp();
                    exit(1);
                }
            }
        }
        
        return options;
    }

    static void showHelp() {
        std::cout << "=== NLM (Neural Learning Machine) - Enhanced CLI ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: nlm [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Modes:" << std::endl;
        std::cout << "  --mode demo          Run demonstration mode (default)" << std::endl;
        std::cout << "  --mode research      Run research mode with parameter exploration" << std::endl;
        std::cout << "  --mode interactive   Start interactive mode for manual control" << std::endl;
        std::cout << "  --mode batch         Run batch of automated experiments" << std::endl;
        std::cout << std::endl;
        std::cout << "Experiments:" << std::endl;
        std::cout << "  --experiment phase3   Run Phase 3: World interaction" << std::endl;
        std::cout << "  --experiment phase4   Run Phase 4: Memory systems" << std::endl;
        std::cout << "  --experiment phase5   Run Phase 5: Performance optimizations" << std::endl;
        std::cout << "  --experiment phase6   Run Phase 6: Full integration (default)" << std::endl;
        std::cout << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  --config FILE        Load configuration from file (default: configs/default.cfg)" << std::endl;
        std::cout << "  --neurons N          Set number of neurons (default: 500)" << std::endl;
        std::cout << "  --steps S            Set simulation steps (default: 2000)" << std::endl;
        std::cout << "  --regions R          Set number of regions (default: 1)" << std::endl;
        std::cout << "  --connection-prob P  Set connection probability (default: 0.15)" << std::endl;
        std::cout << std::endl;
        std::cout << "Output & Export:" << std::endl;
        std::cout << "  --output FILE        Save results to file (default: nlm_results.json)" << std::endl;
        std::cout << "  --checkpoint FILE     Load/save brain state (default: auto)" << std::endl;
        std::cout << "  --enable-export      Enable data export" << std::endl;
        std::cout << "  --export-format FMT   Export format: json, csv, txt (default: json)" << std::endl;
        std::cout << std::endl;
        std::cout << "Advanced Options:" << std::endl;
        std::cout << "  --seed N             Set random seed (default: 42)" << std::endl;
        std::cout << "  --verbose, -v        Enable verbose output" << std::endl;
        std::cout << "  --benchmark          Enable performance benchmarking" << std::endl;
        std::cout << "  --save-results       Save experiment results" << std::endl;
        std::cout << "  --plasticity-mod MOD Set plasticity modulation (default: 1.0)" << std::endl;
        std::cout << "  --max-experiments N  Maximum experiments in batch mode (default: 10)" << std::endl;
        std::cout << "  --no-replay          Disable memory replay" << std::endl;
        std::cout << "  --no-development     Disable developmental stages" << std::endl;
        std::cout << std::endl;
        std::cout << "Research Features:" << std::endl;
        std::cout << "  --parameter-exploration    Enable parameter exploration" << std::endl;
        std::cout << "  --parallel-experiments N  Number of parallel experiments" << std::endl;
        std::cout << "  --enable-visualization     Enable visualization tools" << std::endl;
        std::cout << "  --enable-profiling        Enable performance profiling" << std::endl;
        std::cout << "  --enable-debug            Enable debug mode" << std::endl;
        std::cout << "  --enable-validation       Enable validation mode" << std::endl;
        std::cout << std::endl;
        std::cout << "Directories:" << std::endl;
        std::cout << "  --results-dir DIR      Results directory (default: results/)" << std::endl;
        std::cout << "  --logs-dir DIR         Logs directory (default: logs/)" << std::endl;
        std::cout << "  --checkpoints-dir DIR   Checkpoints directory (default: checkpoints/)" << std::endl;
        std::cout << "  --exports-dir DIR      Exports directory (default: exports/)" << std::endl;
        std::cout << std::endl;
        std::cout << "Resource Limits:" << std::endl;
        std::cout << "  --timeout SECONDS      Maximum runtime in seconds (default: none)" << std::endl;
        std::cout << "  --memory-limit BYTES   Maximum memory usage (default: none)" << std::endl;
        std::cout << "  --cpu-cores N          Number of CPU cores to use (default: auto)" << std::endl;
        std::cout << std::endl;
        std::cout << "Key=value pairs can be used to set configuration values." << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm --mode research --experiment phase6 --neurons 1000 --steps 5000" << std::endl;
        std::cout << "  nlm --mode interactive --config my_config.cfg" << std::endl;
        std::cout << "  nlm --experiment phase4 --enable-export --export-format csv" << std::endl;
        std::cout << "  nlm --mode batch --max-experiments 5 --plasticity-mod 0.8" << std::endl;
        std::cout << "  nlm --enable-profiling --enable-visualization --benchmark" << std::endl;
    }

    static void showVersion() {
        std::cout << "NLM (熙然) v6.0.0 - Phase 6 Final Integration" << std::endl;
        std::cout << "Neural Learning Machine with Advanced Integration" << std::endl;
        std::cout << "Built with enhanced user interface and research capabilities" << std::endl;
    }

    static void setCustomHelp(const std::string& help) {
        help_text = help;
    }

private:
    static std::string help_text;
};

std::string CommandLineParser::help_text = "";

// Data structures for results and metrics
struct NLMMetrics {
    double total_reward = 0.0;
    double avg_firing_rate = 0.0;
    double avg_synaptic_weight = 0.0;
    double memory_episodes_stored = 0.0;
    double novelty_level = 0.0;
    double curiosity_level = 0.0;
    double dopamine_level = 0.0;
    size_t total_neurons = 0;
    size_t total_synapses = 0;
    size_t total_spikes = 0;
    double total_wall_clock_time = 0.0;
    std::unordered_map<std::string, double> custom_metrics;
};

struct ExperimentResult {
    std::string name;
    std::string mode;
    std::string experiment;
    uint64_t seed;
    NLMMetrics metrics;
    std::vector<std::string> notes;
    time_t timestamp;
    double runtime;
    bool success;
    std::string error_message;
};

// Performance monitoring
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double cpu_usage = 0.0;
        double memory_usage = 0.0;
        double network_usage = 0.0;
        double disk_io = 0.0;
        size_t active_threads = 0;
        size_t total_operations = 0;
        double avg_operation_time = 0.0;
        double peak_memory = 0.0;
        double total_time = 0.0;
    };

    PerformanceMonitor() : start_time(std::chrono::high_resolution_clock::now()) {}
    
    void startTimer() {
        timer_start = std::chrono::high_resolution_clock::now();
    }
    
    double stopTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - timer_start);
        return duration.count() / 1000000.0;  // Convert to seconds
    }
    
    PerformanceMetrics getMetrics() {
        PerformanceMetrics metrics;
        auto end = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_time);
        metrics.total_time = total_duration.count() / 1000000.0;
        
        // CPU usage calculation (simplified)
        metrics.cpu_usage = calculateCPUUsage();
        
        // Memory usage calculation (simplified)
        metrics.memory_usage = getMemoryUsage();
        
        metrics.active_threads = getActiveThreadCount();
        metrics.peak_memory = getPeakMemoryUsage();
        
        return metrics;
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point timer_start;
    
    double calculateCPUUsage() {
        // Simplified CPU usage calculation
        return 0.0;
    }
    
    double getMemoryUsage() {
        // Simplified memory usage calculation
        return 0.0;
    }
    
    size_t getActiveThreadCount() {
        return std::thread::hardware_concurrency();
    }
    
    double getPeakMemoryUsage() {
        // Simplified peak memory calculation
        return 0.0;
    }
};

// Data exporter for multiple formats
class DataExporter {
public:
    static bool exportResults(const std::string& filename, const NLMMetrics& metrics, 
                             const std::string& format = "json") {
        std::filesystem::path dir = "exports/";
        if (!std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        
        std::filesystem::path filepath = dir / filename;
        
        try {
            if (format == "json") {
                exportToJSON(filepath, metrics);
            }
            else if (format == "csv") {
                exportToCSV(filepath, metrics);
            }
            else if (format == "txt") {
                exportToTXT(filepath, metrics);
            }
            else {
                std::cerr << "Unsupported export format: " << format << std::endl;
                return false;
            }
            
            std::cout << "Results exported to: " << filepath.string() << std::endl;
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Export failed: " << e.what() << std::endl;
            return false;
        }
    }
    
private:
    static void exportToJSON(const std::filesystem::path& filepath, const NLMMetrics& metrics) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing");
        }
        
        // Create JSON structure
        std::ostringstream json;
        json << "{\n";
        json << "  \"total_reward\": " << metrics.total_reward << ",\n";
        json << "  \"avg_firing_rate\": " << metrics.avg_firing_rate << ",\n";
        json << "  \"avg_synaptic_weight\": " << metrics.avg_synaptic_weight << ",\n";
        json << "  \"memory_episodes_stored\": " << metrics.memory_episodes_stored << ",\n";
        json << "  \"novelty_level\": " << metrics.novelty_level << ",\n";
        json << "  \"curiosity_level\": " << metrics.curiosity_level << ",\n";
        json << "  \"dopamine_level\": " << metrics.dopamine_level << ",\n";
        json << "  \"total_neurons\": " << metrics.total_neurons << ",\n";
        json << "  \"total_synapses\": " << metrics.total_synapses << ",\n";
        json << "  \"total_spikes\": " << metrics.total_spikes << ",\n";
        json << "  \"total_wall_clock_time\": " << metrics.total_wall_clock_time << "\n";
        
        // Add custom metrics
        if (!metrics.custom_metrics.empty()) {
            json << "  \"custom_metrics\": {\n";
            bool first = true;
            for (const auto& [key, value] : metrics.custom_metrics) {
                if (!first) json << ",\n";
                json << "    \"" << key << "\": " << value;
                first = false;
            }
            json << "\n  }\n";
        }
        
        json << "}\n";
        file << json.str();
    }
    
    static void exportToCSV(const std::filesystem::path& filepath, const NLMMetrics& metrics) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing");
        }
        
        // Write CSV header
        file << "metric,value\n";
        
        // Write metrics
        file << "total_reward," << metrics.total_reward << "\n";
        file << "avg_firing_rate," << metrics.avg_firing_rate << "\n";
        file << "avg_synaptic_weight," << metrics.avg_synaptic_weight << "\n";
        file << "memory_episodes_stored," << metrics.memory_episodes_stored << "\n";
        file << "novelty_level," << metrics.novelty_level << "\n";
        file << "curiosity_level," << metrics.curiosity_level << "\n";
        file << "dopamine_level," << metrics.dopamine_level << "\n";
        file << "total_neurons," << metrics.total_neurons << "\n";
        file << "total_synapses," << metrics.total_synapses << "\n";
        file << "total_spikes," << metrics.total_spikes << "\n";
        file << "total_wall_clock_time," << metrics.total_wall_clock_time << "\n";
        
        // Write custom metrics
        for (const auto& [key, value] : metrics.custom_metrics) {
            file << key << "," << value << "\n";
        }
    }
    
    static void exportToTXT(const std::filesystem::path& filepath, const NLMMetrics& metrics) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing");
        }
        
        // Write human-readable text format
        file << "NLM (Neural Learning Machine) Results" << std::endl;
        file << "=====================================" << std::endl << std::endl;
        
        file << "Performance Metrics:" << std::endl;
        file << "  Total Reward:           " << metrics.total_reward << std::endl;
        file << "  Average Firing Rate:    " << metrics.avg_firing_rate << std::endl;
        file << "  Average Synaptic Weight:" << metrics.avg_synaptic_weight << std::endl;
        file << "  Memory Episodes Stored:" << metrics.memory_episodes_stored << std::endl;
        file << "  Novelty Level:          " << metrics.novelty_level << std::endl;
        file << "  Curiosity Level:        " << metrics.curiosity_level << std::endl;
        file << "  Dopamine Level:         " << metrics.dopamine_level << std::endl;
        file << std::endl;
        
        file << "Neural Statistics:" << std::endl;
        file << "  Total Neurons:          " << metrics.total_neurons << std::endl;
        file << "  Total Synapses:         " << metrics.total_synapses << std::endl;
        file << "  Total Spikes:           " << metrics.total_spikes << std::endl;
        file << std::endl;
        
        file << "Timing:" << std::endl;
        file << "  Total Wall Clock Time:  " << metrics.total_wall_clock_time << " seconds" << std::endl;
        std::cout << "Results exported to: " << filepath.string() << std::endl;
    }
};

// Research mode handler
class ResearchModeHandler {
public:
    static void runParameterExploration(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) {
        NLM_LOG_INFO("=== Research Mode: Parameter Exploration ===");
        NLM_LOG_INFO("Running parameter exploration with " + std::to_string(options.parallel_experiments) + " parallel experiments");
        
        std::vector<ExperimentResult> results;
        
        // Generate parameter combinations
        std::vector<std::unordered_map<std::string, double>> parameterSets = generateParameterSets(options);
        
        // Run experiments in parallel
        std::vector<std::thread> threads;
        std::mutex results_mutex;
        
        for (const auto& params : parameterSets) {
            threads.emplace_back([&, params]() {
                auto result = runSingleExperiment(brain, params, options);
                std::lock_guard<std::mutex> lock(results_mutex);
                results.push_back(result);
            });
        }
        
        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        // Analyze and report results
        analyzeAndReportResults(results, options);
    }
    
private:
    static std::vector<std::unordered_map<std::string, double>> generateParameterSets(const CommandLineParser::Options& options) {
        std::vector<std::unordered_map<std::string, double>> parameterSets;
        
        // Generate different parameter combinations for exploration
        std::vector<double> neuronCounts = {100, 250, 500, 1000, 2000};
        std::vector<double> stepCounts = {500, 1000, 2000, 5000, 10000};
        std::vector<double> connectionProbs = {0.05, 0.1, 0.15, 0.2, 0.25};
        std::vector<double> plasticityMods = {0.5, 0.8, 1.0, 1.2, 1.5};
        
        // Create combinations (simplified - full implementation would be more sophisticated)
        size_t max_combinations = std::min<size_t>(options.max_experiments, 20);
        
        std::mt19937 rng(options.seed);
        std::uniform_int_distribution<size_t> dist(0, neuronCounts.size() - 1);
        
        for (size_t i = 0; i < max_combinations; ++i) {
            std::unordered_map<std::string, double> params;
            params["neuron_count"] = neuronCounts[dist(rng)];
            params["step_count"] = stepCounts[dist(rng)];
            params["connection_prob"] = connectionProbs[dist(rng)];
            params["plasticity_mod"] = plasticityMods[dist(rng)];
            parameterSets.push_back(params);
        }
        
        return parameterSets;
    }
    
    static ExperimentResult runSingleExperiment(std::shared_ptr<Brain> brain, 
                                                 const std::unordered_map<std::string, double>& params,
                                                 const CommandLineParser::Options& options) {
        ExperimentResult result;
        result.name = "ParameterExperiment_" + std::to_string(rand());
        result.mode = "research";
        result.experiment = options.experiment;
        result.seed = options.seed;
        result.timestamp = std::time(nullptr);
        result.success = false;
        
        try {
            // Create temporary brain with specific parameters
            auto tempBrain = std::make_shared<Brain>(brain->getConfig());
            if (!tempBrain->initialize()) {
                result.error_message = "Failed to initialize brain";
                return result;
            }
            
            // Override parameters
            tempBrain->getConfig()->set("neuron_count", static_cast<int64_t>(params.at("neuron_count")),
                                       ConfigSource::Default);
            tempBrain->getConfig()->set("region_count", static_cast<int64_t>(options.regions),
                                       ConfigSource::Default);
            tempBrain->getConfig()->set("connection_probability", static_cast<float>(params.at("connection_prob")),
                                       ConfigSource::Default);
            tempBrain->getConfig()->set("plasticity_mod", params.at("plasticity_mod"),
                                       ConfigSource::Default);
            
            // Run experiment
            double runtime = runExperimentWithParams(tempBrain, params["step_count"]);
            
            // Collect results
            result.metrics = collectMetrics(tempBrain);
            result.runtime = runtime;
            result.success = true;
            
            NLM_LOG_INFO("Experiment " + result.name + " completed in " + std::to_string(runtime) + "s");
            
        } catch (const std::exception& e) {
            result.error_message = e.what();
            NLM_LOG_ERROR("Experiment " + result.name + " failed: " + e.what());
        }
        
        return result;
    }
    
    static double runExperimentWithParams(std::shared_ptr<Brain> brain, size_t steps) {
        PerformanceMonitor monitor;
        monitor.startTimer();
        
        // Reset brain
        brain->reset();
        brain->initialize();
        
        // Run steps
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        return monitor.stopTimer();
    }
    
    static NLMMetrics collectMetrics(std::shared_ptr<Brain> brain) {
        NLMMetrics metrics;
        
        metrics.total_neurons = brain->getTotalNeuronCount();
        metrics.total_synapses = brain->getTotalSynapseCount();
        metrics.total_spikes = brain->getTotalSpikeCount();
        metrics.avg_firing_rate = brain->getAverageFiringRate();
        
        // Collect neuromodulation metrics
        if (auto* dopamine = brain->getDopamine()) {
            metrics.dopamine_level = dopamine->getLevel();
        }
        if (auto* curiosity = brain->getCuriosity()) {
            metrics.curiosity_level = curiosity->getLevel();
        }
        if (auto* novelty = brain->getNovelty()) {
            metrics.novelty_level = novelty->getLevel();
        }
        
        // Calculate additional metrics
        metrics.total_reward = brain->getTotalSpikeCount() * 0.1f;  // Placeholder
        metrics.avg_synaptic_weight = brain->getExcitationInhibitionRatio();
        
        return metrics;
    }
    
    static void analyzeAndReportResults(const std::vector<ExperimentResult>& results, 
                                       const CommandLineParser::Options& options) {
        NLM_LOG_INFO("=== Research Mode Results Analysis ===");
        
        if (results.empty()) {
            NLM_LOG_INFO("No experiments completed successfully");
            return;
        }
        
        // Calculate statistics
        double totalRuntime = 0.0;
        size_t successCount = 0;
        std::vector<double> firingRates;
        
        for (const auto& result : results) {
            if (result.success) {
                totalRuntime += result.runtime;
                successCount++;
                firingRates.push_back(result.metrics.avg_firing_rate);
            }
        }
        
        if (successCount > 0) {
            double avgRuntime = totalRuntime / successCount;
            double avgFiringRate = std::accumulate(firingRates.begin(), firingRates.end(), 0.0) / firingRates.size();
            
            NLM_LOG_INFO("Successfully completed " + std::to_string(successCount) + "/" + std::to_string(results.size()) + " experiments");
            NLM_LOG_INFO("Average runtime: " + std::to_string(avgRuntime) + "s");
            NLM_LOG_INFO("Average firing rate: " + std::to_string(avgFiringRate));
            
            // Export results
            NLMMetrics aggregatedMetrics;
            aggregatedMetrics.total_neurons = results[0].metrics.total_neurons;
            aggregatedMetrics.avg_firing_rate = avgFiringRate;
            aggregatedMetrics.total_wall_clock_time = totalRuntime;
            
            std::string filename = "research_results_" + std::to_string(std::time(nullptr)) + ".json";
            DataExporter::exportResults(filename, aggregatedMetrics, options.export_format);
        }
    }
};

// Interactive mode handler
class InteractiveModeHandler {
public:
    static void startInteractiveSession(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) {
        NLM_LOG_INFO("=== Interactive Mode ===");
        NLM_LOG_INFO("Type 'help' for available commands, 'exit' to quit");
        
        std::string input;
        bool running = true;
        
        while (running) {
            std::cout << ">>> ";
            std::getline(std::cin, input);
            
            if (input.empty()) continue;
            
            if (input == "help" || input == "?") {
                showInteractiveHelp();
            }
            else if (input == "exit" || input == "quit") {
                running = false;
                NLM_LOG_INFO("Exiting interactive mode");
            }
            else if (input == "status") {
                showBrainStatus(brain);
            }
            else if (input == "step") {
                brain->step(0);
                NLM_LOG_INFO("Brain stepped (step 0)");
            }
            else if (input.find("step ") == 0) {
                size_t step_num = std::stoi(input.substr(5));
                brain->step(step_num);
                NLM_LOG_INFO("Brain stepped (step " + std::to_string(step_num) + ")");
            }
            else if (input.find("inject ") == 0) {
                handleInjectionCommand(brain, input.substr(7));
            }
            else if (input.find("save ") == 0) {
                handleSaveCommand(brain, input.substr(5));
            }
            else if (input.find("load ") == 0) {
                handleLoadCommand(brain, input.substr(5));
            }
            else if (input.find("config ") == 0) {
                handleConfigCommand(brain, input.substr(7));
            }
            else if (input == "run demo") {
                runDemoSequence(brain, options);
            }
            else if (input == "run experiment") {
                runFullExperiment(brain, options);
            }
            else {
                std::cout << "Unknown command: " << input << std::endl;
                std::cout << "Type 'help' for available commands." << std::endl;
            }
        }
    }
    
private:
    static void showInteractiveHelp() {
        std::cout << "Available commands:" << std::endl;
        std::cout << "  help, ?           Show this help" << std::endl;
        std::cout << "  exit, quit        Exit interactive mode" << std::endl;
        std::cout << "  status            Show brain status" << std::endl;
        std::cout << "  step [N]          Step brain by N steps (default: 1)" << std::endl;
        std::cout << "  inject [type] [current] - Inject current into neurons" << std::endl;
        std::cout << "  save [file]       Save brain state to file" << std::endl;
        std::cout << "  load [file]       Load brain state from file" << std::endl;
        std::cout << "  config [key] [val] - Set configuration value" << std::endl;
        std::cout << "  run demo          Run demonstration sequence" << std::endl;
        std::cout << "  run experiment    Run full experiment" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  step 100          Step brain 100 times" << std::endl;
        std::cout << "  inject sensory 50.0" << std::endl;
        std::cout << "  save brain_state.bin" << std::endl;
        std::cout << "  config neuron_count 1000" << std::endl;
    }
    
    static void showBrainStatus(std::shared_ptr<Brain> brain) {
        std::cout << "=== Brain Status ===" << std::endl;
        std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        
        if (auto* spikeSystem = brain->getSpikeSystem()) {
            std::cout << "Pending Spike Events: " << spikeSystem->getPendingSpikeCount() + spikeSystem->getPendingDelayedCount() << std::endl;
        }
    }
    
    static void handleInjectionCommand(std::shared_ptr<Brain> brain, const std::string& input) {
        std::istringstream stream(input);
        std::string neuronType;
        float current;
        
        if (stream >> neuronType >> current) {
            NeuronType type;
            if (neuronType == "sensory") type = NeuronType::Sensory;
            else if (neuronType == "motor") type = NeuronType::Motor;
            else if (neuronType == "internal") type = NeuronType::Internal;
            else {
                std::cout << "Unknown neuron type: " << neuronType << std::endl;
                return;
            }
            
            brain->injectCurrentToNeurons(type, current);
            std::cout << "Injected " << current << " current into " << neuronType << " neurons" << std::endl;
        }
        else {
            std::cout << "Usage: inject [type] [current]" << std::endl;
            std::cout << "Types: sensory, motor, internal" << std::endl;
        }
    }
    
    static void handleSaveCommand(std::shared_ptr<Brain> brain, const std::string& filename) {
        if (filename.empty()) {
            std::cout << "Usage: save [filename]" << std::endl;
            return;
        }
        
        if (brain->save(filename)) {
            std::cout << "Brain state saved to: " << filename << std::endl;
        }
        else {
            std::cout << "Failed to save brain state to: " << filename << std::endl;
        }
    }
    
    static void handleLoadCommand(std::shared_ptr<Brain> brain, const std::string& filename) {
        if (filename.empty()) {
            std::cout << "Usage: load [filename]" << std::endl;
            return;
        }
        
        if (brain->load(filename)) {
            std::cout << "Brain state loaded from: " << filename << std::endl;
        }
        else {
            std::cout << "Failed to load brain state from: " << filename << std::endl;
        }
    }
    
    static void handleConfigCommand(std::shared_ptr<Brain> brain, const std::string& input) {
        std::istringstream stream(input);
        std::string key;
        std::string value;
        
        if (stream >> key >> value) {
            brain->getConfig()->set(key, value, ConfigSource::Default);
            std::cout << "Set configuration " << key << " = " << value << std::endl;
        }
        else {
            std::cout << "Usage: config [key] [value]" << std::endl;
        }
    }
    
    static void runDemoSequence(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) {
        NLM_LOG_INFO("=== Running Demo Sequence ===");
        
        // Run basic connectivity test
        runBasicConnectivityTest(brain);
        
        // Reset and run plasticity experiment
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
        NLM_LOG_INFO("=== Demo Sequence Complete ===");
    }
    
    static void runFullExperiment(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) {
        NLM_LOG_INFO("=== Running Full Integration Experiment ===");
        
        Phase6Config config;
        config.neuronCount = options.neurons;
        config.maxSteps = options.steps;
        config.enableCheckpointing = options.save_results;
        config.enableReplay = options.enable_replay;
        config.enableDevelopment = options.enable_development;
        config.checkpointPath = options.checkpoint_file.empty() ? "checkpoint_test.bin" : options.checkpoint_file;
        
        Phase6IntegratedExperiment experiment;
        Phase6IntegrationResult result = experiment.run(config);
        
        std::cout << "=== Experiment Results ===" << std::endl;
        std::cout << "Total Reward: " << result.totalReward << std::endl;
        std::cout << "Avg Firing Rate: " << result.avgFiringRate << std::endl;
        std::cout << "Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine Level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== Integration Status ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    }
};

// Batch mode handler
class BatchModeHandler {
public:
    static void runBatchExperiments(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) {
        NLM_LOG_INFO("=== Batch Mode: Automated Experiment Runner ===");
        NLM_LOG_INFO("Running " + std::to_string(options.max_experiments) + " experiments");
        
        std::vector<ExperimentResult> results;
        size_t experimentCount = 0;
        
        while (experimentCount < options.max_experiments) {
            experimentCount++;
            NLM_LOG_INFO("Running experiment " + std::to_string(experimentCount) + "/" + std::to_string(options.max_experiments));
            
            // Create fresh brain for each experiment
            auto freshBrain = std::make_shared<Brain>(brain->getConfig());
            if (!freshBrain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain for experiment " + std::to_string(experimentCount));
                continue;
            }
            
            // Run experiment
            ExperimentResult result = runSingleBatchExperiment(freshBrain, options, experimentCount);
            results.push_back(result);
            
            // Small delay between experiments
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Save batch results
        saveBatchResults(results, options);
        
        // Generate summary
        generateBatchSummary(results, options);
    }
    
private:
    static ExperimentResult runSingleBatchExperiment(std::shared_ptr<Brain> brain, 
                                                      const CommandLineParser::Options& options,
                                                      size_t experimentId) {
        ExperimentResult result;
        result.name = "BatchExperiment_" + std::to_string(experimentId);
        result.mode = "batch";
        result.experiment = options.experiment;
        result.seed = options.seed + experimentId;
        result.timestamp = std::time(nullptr);
        result.success = false;
        
        try {
            // Modify parameters slightly for each experiment
            auto config = brain->getConfig();
            config->set("random_seed", static_cast<int64_t>(result.seed), ConfigSource::Default);
            
            // Add small random variations
            std::mt19937 rng(result.seed);
            std::uniform_real_distribution<float> variation(0.8f, 1.2f);
            
            float neuronVariation = variation(rng);
            float stepVariation = variation(rng);
            
            config->set("neuron_count", static_cast<int64_t>(options.neurons * neuronVariation),
                       ConfigSource::Default);
            
            // Reset and reinitialize with new parameters
            brain->reset();
            brain->initialize();
            
            // Run experiment
            PerformanceMonitor monitor;
            monitor.startTimer();
            
            if (options.experiment == "phase3") {
                // Run Phase 3 experiment
                runPhase3Experiment(brain, options);
            }
            else if (options.experiment == "phase4") {
                // Run Phase 4 experiment
                runPhase4Experiment(brain, options);
            }
            else if (options.experiment == "phase5") {
                // Run Phase 5 experiment
                runPhase5Experiment(brain, options);
            }
            else {
                // Run Phase 6 experiment
                runPhase6Experiment(brain, options);
            }
            
            double runtime = monitor.stopTimer();
            
            // Collect results
            result.metrics = collectMetrics(brain);
            result.runtime = runtime;
            result.success = true;
            
            NLM_LOG_INFO("Batch experiment " + std::to_string(experimentId) + " completed in " + std::to_string(runtime) + "s");
            
        } catch (const std::exception& e) {
            result.error_message = e.what();
            NLM_LOG_ERROR("Batch experiment " + std::to_string(experimentId) + " failed: " + e.what());
        }
        
        return result;
    }
    
    static void runPhase3Experiment(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) {
        // Placeholder for Phase 3 experiment implementation
        // This would run the full Phase 3: World interaction experiment
        for (SimulationStep step = 0; step < options.steps; ++step) {
            brain->step(step);
        }
    }
    
    static void runPhase4Experiment(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) {
        // Placeholder for Phase 4 experiment implementation
        // This would run the full Phase 4: Memory systems experiment
        for (SimulationStep step = 0; step < options.steps; ++step) {
            brain->step(step);
        }
    }
    
    static void runPhase5Experiment(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) {
        // Placeholder for Phase 5 experiment implementation
        // This would run the full Phase 5: Performance optimizations experiment
        for (SimulationStep step = 0; step < options.steps; ++step) {
            brain->step(step);
        }
    }
    
    static void runPhase6Experiment(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) {
        // Run Phase 6 integration experiment
        Phase6Config config;
        config.neuronCount = options.neurons;
        config.maxSteps = options.steps;
        config.enableCheckpointing = options.save_results;
        config.enableReplay = options.enable_replay;
        config.enableDevelopment = options.enable_development;
        config.checkpointPath = options.checkpoint_file.empty() ? "checkpoint_test.bin" : options.checkpoint_file;
        
        Phase6IntegratedExperiment experiment;
        experiment.run(config);
    }
    
    static NLMMetrics collectMetrics(std::shared_ptr<Brain> brain) {
        NLMMetrics metrics;
        
        metrics.total_neurons = brain->getTotalNeuronCount();
        metrics.total_synapses = brain->getTotalSynapseCount();
        metrics.total_spikes = brain->getTotalSpikeCount();
        metrics.avg_firing_rate = brain->getAverageFiringRate();
        
        // Collect neuromodulation metrics
        if (auto* dopamine = brain->getDopamine()) {
            metrics.dopamine_level = dopamine->getLevel();
        }
        if (auto* curiosity = brain->getCuriosity()) {
            metrics.curiosity_level = curiosity->getLevel();
        }
        if (auto* novelty = brain->getNovelty()) {
            metrics.novelty_level = novelty->getLevel();
        }
        
        // Calculate additional metrics
        metrics.total_reward = brain->getTotalSpikeCount() * 0.1f;  // Placeholder
        metrics.avg_synaptic_weight = brain->getExcitationInhibitionRatio();
        
        return metrics;
    }
    
    static void saveBatchResults(const std::vector<ExperimentResult>& results, 
                                 const CommandLineParser::Options& options) {
        std::string filename = "batch_results_" + std::to_string(std::time(nullptr)) + ".json";
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open file for writing batch results: " + filename);
            return;
        }
        
        // Write results in JSON format
        file << "{\n";
        file << "  \"experiment_count\": " << results.size() << ",\n";
        file << "  \"successful_experiments\": " << std::count_if(results.begin(), results.end(),
                       [](const ExperimentResult& r) { return r.success; }) << ",\n";
        file << "  \"results\": [\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            file << "    {\n";
            file << "      \"name\": \"" << result.name << "\",\n";
            file << "      \"success\": " << (result.success ? "true" : "false") << ",\n";
            file << "      \"runtime\": " << result.runtime << ",\n";
            file << "      \"metrics\": {\n";
            file << "        \"total_reward\": " << result.metrics.total_reward << ",\n";
            file << "        \"avg_firing_rate\": " << result.metrics.avg_firing_rate << ",\n";
            file << "        \"total_spikes\": " << result.metrics.total_spikes << "\n";
            file << "      }\n";
            
            if (i < results.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
        
        file.close();
        NLM_LOG_INFO("Batch results saved to: " + filename);
    }
    
    static void generateBatchSummary(const std::vector<ExperimentResult>& results, 
                                     const CommandLineParser::Options& options) {
        NLM_LOG_INFO("=== Batch Mode Summary ===");
        
        if (results.empty()) {
            NLM_LOG_INFO("No experiments were completed");
            return;
        }
        
        size_t totalExperiments = results.size();
        size_t successfulExperiments = std::count_if(results.begin(), results.end(),
                                                      [](const ExperimentResult& r) { return r.success; });
        double totalRuntime = 0.0;
        double totalReward = 0.0;
        double avgFiringRate = 0.0;
        
        for (const auto& result : results) {
            if (result.success) {
                totalRuntime += result.runtime;
                totalReward += result.metrics.total_reward;
                avgFiringRate += result.metrics.avg_firing_rate;
            }
        }
        
        if (successfulExperiments > 0) {
            avgFiringRate /= successfulExperiments;
            double avgRuntime = totalRuntime / successfulExperiments;
            
            NLM_LOG_INFO("Batch completed: " + std::to_string(successfulExperiments) + "/" + std::to_string(totalExperiments) + " successful");
            NLM_LOG_INFO("Average runtime: " + std::to_string(avgRuntime) + "s");
            NLM_LOG_INFO("Total reward: " + std::to_string(totalReward));
            NLM_LOG_INFO("Average firing rate: " + std::to_string(avgFiringRate));
            
            // Show best and worst performing experiments
            auto best_it = std::max_element(results.begin(), results.end(),
                [](const ExperimentResult& a, const ExperimentResult& b) {
                    return a.metrics.total_reward < b.metrics.total_reward;
                });
            
            auto worst_it = std::min_element(results.begin(), results.end(),
                [](const ExperimentResult& a, const ExperimentResult& b) {
                    return a.metrics.total_reward < b.metrics.total_reward;
                });
            
            if (best_it != results.end() && best_it->success) {
                NLM_LOG_INFO("Best experiment: " + best_it->name + " (Reward: " + std::to_string(best_it->metrics.total_reward) + ")");
            }
            
            if (worst_it != results.end() && worst_it->success) {
                NLM_LOG_INFO("Worst experiment: " + worst_it->name + " (Reward: " + std::to_string(worst_it->metrics.total_reward) + ")");
            }
        }
    }
};

// Default execution functions (maintained for backward compatibility)
void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Learning Experiment: Demonstrates measurable synaptic changes through experience
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

// Save results function
void saveResults(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) {
    NLM_LOG_INFO("Saving brain state to " + options.output_file);
    
    if (brain->save(options.output_file)) {
        NLM_LOG_INFO("Brain state saved successfully");
    } else {
        NLM_LOG_ERROR("Failed to save brain state");
    }
}

// Export data function
void exportData(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) {
    NLM_LOG_INFO("Exporting data from brain to " + options.output_file);
    
    // Collect metrics
    NLMMetrics metrics;
    metrics.total_neurons = brain->getTotalNeuronCount();
    metrics.total_synapses = brain->getTotalSynapseCount();
    metrics.total_spikes = brain->getTotalSpikeCount();
    metrics.avg_firing_rate = brain->getAverageFiringRate();
    
    // Collect neuromodulation metrics
    if (auto* dopamine = brain->getDopamine()) {
        metrics.dopamine_level = dopamine->getLevel();
    }
    if (auto* curiosity = brain->getCuriosity()) {
        metrics.curiosity_level = curiosity->getLevel();
    }
    if (auto* novelty = brain->getNovelty()) {
        metrics.novelty_level = novelty->getLevel();
    }
    
    // Export to file
    if (DataExporter::exportResults(options.output_file, metrics, options.export_format)) {
        NLM_LOG_INFO("Data exported successfully");
    } else {
        NLM_LOG_ERROR("Failed to export data");
    }
}

// Main function with enhanced CLI support
int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 6 with Enhanced User Interface..." << std::endl;
    
    // Parse command line arguments
    auto options = CommandLineParser::parse(argc, argv);
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(options.verbose ? LogLevel::Debug : LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Log configuration
    NLM_LOG_INFO("=== NLM Enhanced CLI Mode ===");
    NLM_LOG_INFO("Mode: " + options.mode);
    NLM_LOG_INFO("Experiment: " + options.experiment);
    NLM_LOG_INFO("Config file: " + options.config_file);
    NLM_LOG_INFO("Output file: " + options.output_file);
    if (!options.checkpoint_file.empty()) {
        NLM_LOG_INFO("Checkpoint file: " + options.checkpoint_file);
    }
    NLM_LOG_INFO("Random seed: " + std::to_string(options.seed));
    NLM_LOG_INFO("Neurons: " + std::to_string(options.neurons));
    NLM_LOG_INFO("Steps: " + std::to_string(options.steps));
    NLM_LOG_INFO("Regions: " + std::to_string(options.regions));
    NLM_LOG_INFO("Connection probability: " + std::to_string(options.connection_prob));
    NLM_LOG_INFO("Verbose: " + std::to_string(options.verbose));
    NLM_LOG_INFO("Benchmark: " + std::to_string(options.benchmark));
    NLM_LOG_INFO("Save results: " + std::to_string(options.save_results));
    NLM_LOG_INFO("Enable export: " + std::to_string(options.enable_export));
    NLM_LOG_INFO("Export format: " + options.export_format);
    NLM_LOG_INFO("Replay interval: " + std::to_string(options.replay_interval));
    NLM_LOG_INFO("Enable replay: " + std::to_string(options.enable_replay));
    NLM_LOG_INFO("Enable development: " + std::to_string(options.enable_development));
    NLM_LOG_INFO("Plasticity mod: " + std::to_string(options.plasticity_mod));
    NLM_LOG_INFO("Max experiments: " + std::to_string(options.max_experiments));
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    if (options.config_file != "configs/default.cfg") {
        if (config->loadFromFile(options.config_file)) {
            NLM_LOG_INFO("Loaded configuration from: " + options.config_file);
        } else {
            NLM_LOG_INFO("Using default configuration (file not found: " + options.config_file + ")");
        }
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line arguments
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 6
    config->set("random_seed", static_cast<int64_t>(options.seed), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(options.neurons), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(options.regions), ConfigSource::Default);
    config->set("connection_probability", options.connection_prob, ConfigSource::Default);
    config->set("replay_interval", static_cast<size_t>(options.replay_interval), ConfigSource::Default);
    config->set("enable_replay", options.enable_replay, ConfigSource::Default);
    config->set("enable_development", options.enable_development, ConfigSource::Default);
    config->set("plasticity_mod", options.plasticity_mod, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", options.seed)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", options.neurons)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", options.regions)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", options.connection_prob)));
    NLM_LOG_INFO("  replay_interval: " + std::to_string(config->getOr<size_t>("replay_interval", options.replay_interval)));
    NLM_LOG_INFO("  enable_replay: " + std::to_string(config->getOr<bool>("enable_replay", options.enable_replay)));
    NLM_LOG_INFO("  enable_development: " + std::to_string(config->getOr<bool>("enable_development", options.enable_development)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Execute based on mode
    if (options.mode == "demo") {
        runDemoMode(brain, options);
    }
    else if (options.mode == "research") {
        ResearchModeHandler::runParameterExploration(brain, options);
    }
    else if (options.mode == "interactive") {
        InteractiveModeHandler::startInteractiveSession(brain, options);
    }
    else if (options.mode == "batch") {
        BatchModeHandler::runBatchExperiments(brain, options);
    }
    else {
        NLM_LOG_ERROR("Unknown mode: " + options.mode);
        return 1;
    }
    
    // Save results if requested
    if (options.save_results) {
        saveResults(brain, options);
    }
    
    // Export data if enabled
    if (options.enable_export) {
        exportData(brain, options);
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Enhanced CLI Complete ===");
    NLM_LOG_INFO("Results saved to: " + options.output_file);
    
    return 0;
}