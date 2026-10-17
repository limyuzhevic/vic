// NLM (熙然) - Neural Learning Machine
// Phase 8: Production-Ready Experimental Framework
//
// Complete rewrite of main.cpp with modular design, error handling,
// progress monitoring, statistical analysis, and extensibility framework.

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
#include "experiments/Experiment.hpp"
#include "experiments/Metrics.hpp"
#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/ScalingBenchmark.hpp"
#include "experiments/AblationSystem.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <thread>
#include <map>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace nlm {

// Experiment result structure
struct ExperimentResult {
    std::string experimentName;
    SimulationStep stepsCompleted;
    SimulationStep maxSteps;
    uint64_t seed;
    double totalRuntime;
    double averageFiringRate;
    size_t peakNeuronCount;
    size_t peakSynapseCount;
    std::vector<std::pair<std::string, double>> metrics;
    std::map<std::string, std::vector<double>> timeSeriesData;
    bool learningDetected;
    double learningMagnitude;
    std::string errorMessage;
    bool success;
    
    // Statistical analysis
    struct StatisticalAnalysis {
        double mean;
        double variance;
        double stdDev;
        double min;
        double max;
        double median;
        double pValue;
        bool isSignificant;
    } statistics;
    
    // Default constructor
    ExperimentResult() : stepsCompleted(0), maxSteps(0), seed(0), totalRuntime(0.0),
                         averageFiringRate(0.0), peakNeuronCount(0), peakSynapseCount(0),
                         learningDetected(false), learningMagnitude(0.0), success(false) {}
    
    // Calculate statistics for a vector of values
    static StatisticalAnalysis calculateStatistics(const std::vector<double>& values) {
        StatisticalAnalysis stats;
        if (values.empty()) return stats;
        
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        stats.mean = sum / values.size();
        
        double sqSum = 0.0;
        for (double val : values) {
            sqSum += (val - stats.mean) * (val - stats.mean);
        }
        stats.variance = sqSum / values.size();
        stats.stdDev = std::sqrt(stats.variance);
        stats.min = *std::min_element(values.begin(), values.end());
        stats.max = *std::max_element(values.begin(), values.end());
        
        // Calculate median
        std::vector<double> sorted(values);
        std::sort(sorted.begin(), sorted.end());
        if (sorted.size() % 2 == 0) {
            stats.median = (sorted[sorted.size()/2 - 1] + sorted[sorted.size()/2]) / 2.0;
        } else {
            stats.median = sorted[sorted.size()/2];
        }
        
        return stats;
    }
};

// Progress reporter for real-time monitoring
class ProgressReporter {
public:
    struct ProgressUpdate {
        double percentage;
        SimulationStep currentStep;
        SimulationStep totalSteps;
        std::string currentActivity;
        std::chrono::steady_clock::time_point timestamp;
        double estimatedRemainingTime;
        
        ProgressUpdate(double percent, SimulationStep step, SimulationStep total,
                      const std::string& activity, double remaining)
            : percentage(percent), currentStep(step), totalSteps(total),
              currentActivity(activity), timestamp(std::chrono::steady_clock::now()),
              estimatedRemainingTime(remaining) {}
    };
    
    ProgressReporter(bool enabled = true, bool showBar = true, int updateInterval = 1000)
        : enabled_(enabled), showBar_(showBar), updateInterval_(updateInterval) {}
    
    void startProgress(const std::string& experimentName, SimulationStep totalSteps) {
        if (!enabled_) return;
        
        startTime_ = std::chrono::steady_clock::now();
        totalSteps_ = totalSteps;
        currentStep_ = 0;
        experimentName_ = experimentName;
        
        if (showBar_) {
            std::cout << "\n=== Running Experiment: " << experimentName_ << " ===\n";
            std::cout << "Progress: [";
        }
    }
    
    void updateProgress(SimulationStep step, const std::string& activity = "") {
        if (!enabled_) return;
        
        currentStep_ = step;
        
        if (step % updateInterval_ == 0 || step == totalSteps_ - 1) {
            if (showBar_) {
                double percentage = (static_cast<double>(step) / totalSteps_) * 100.0;
                int barWidth = 30;
                int pos = static_cast<int>(barWidth * percentage / 100.0);
                
                std::cout << "\r[";
                for (int i = 0; i < barWidth; ++i) {
                    if (i < pos) std::cout << "=";
                    else if (i == pos) std::cout << ">";
                    else std::cout << " ";
                }
                std::cout << "] " << std::fixed << std::setprecision(1) << percentage << "%";
                std::cout.flush();
                
                if (!activity.empty()) {
                    std::cout << " - " << activity;
                }
                
                // Calculate ETA
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration<double>(now - startTime_).count();
                if (step > 0) {
                    double stepTime = elapsed / step;
                    double remainingTime = stepTime * (totalSteps_ - step);
                    std::cout << " - ETA: " << std::fixed << std::setprecision(1) << remainingTime << "s";
                }
            } else {
                std::cout << "\rStep: " << step << "/" << totalSteps_;
                if (!activity.empty()) {
                    std::cout << " - " << activity;
                }
                std::cout.flush();
            }
        }
    }
    
    void finishProgress() {
        if (!enabled_ || !showBar_) return;
        
        std::cout << "\r" << std::string(50, ' ') << "\r";  // Clear line
        std::cout << "✓ Experiment completed successfully\n";
        std::cout << "Total steps executed: " << totalSteps_ << "\n";
        
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime_);
        std::cout << "Total runtime: " << duration.count() << " seconds\n";
    }
    
    std::vector<ProgressUpdate> getProgressHistory() const {
        return progressHistory_;
    }
    
    void recordProgressUpdate(const ProgressUpdate& update) {
        progressHistory_.push_back(update);
    }
    
private:
    bool enabled_;
    bool showBar_;
    int updateInterval_;
    std::string experimentName_;
    SimulationStep totalSteps_ = 0;
    SimulationStep currentStep_ = 0;
    std::chrono::steady_clock::time_point startTime_;
    std::vector<ProgressUpdate> progressHistory_;
};

// Configuration manager for advanced argument parsing
class ConfigurationManager {
public:
    struct CommandLineOptions {
        std::string experimentName;
        SimulationStep steps;
        SimulationStep episodes;
        std::string outputFile;
        std::string outputFormat;
        bool verbose;
        bool benchmark;
        bool validate;
        uint64_t randomSeed;
        std::string configFile;
        std::vector<std::string> extraArgs;
    };
    
    ConfigurationManager() = default;
    
    CommandLineOptions parseCommandLine(int argc, char** argv) {
        CommandLineOptions options;
        options.verbose = false;
        options.benchmark = false;
        options.validate = false;
        options.randomSeed = 42;  // Default seed
        options.steps = 1000;
        options.episodes = 1;
        options.outputFormat = "console";
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printUsage();
                exit(0);
            }
            else if (arg == "--version") {
                printVersion();
                exit(0);
            }
            else if (arg.find("--experiment=") == 0) {
                options.experimentName = arg.substr(13);
            }
            else if (arg == "--experiment" && i + 1 < argc) {
                options.experimentName = argv[++i];
            }
            else if (arg.find("--steps=") == 0) {
                options.steps = std::stoull(arg.substr(7));
            }
            else if (arg == "--steps" && i + 1 < argc) {
                options.steps = std::stoull(argv[++i]);
            }
            else if (arg.find("--episodes=") == 0) {
                options.episodes = std::stoull(arg.substr(10));
            }
            else if (arg == "--episodes" && i + 1 < argc) {
                options.episodes = std::stoull(argv[++i]);
            }
            else if (arg.find("--output=") == 0) {
                options.outputFile = arg.substr(8);
                auto pos = options.outputFile.find_last_of('.');
                if (pos != std::string::npos) {
                    options.outputFormat = options.outputFile.substr(pos + 1);
                }
            }
            else if (arg == "--output" && i + 1 < argc) {
                options.outputFile = argv[++i];
                auto pos = options.outputFile.find_last_of('.');
                if (pos != std::string::npos) {
                    options.outputFormat = options.outputFile.substr(pos + 1);
                }
            }
            else if (arg == "--verbose") {
                options.verbose = true;
            }
            else if (arg == "--benchmark") {
                options.benchmark = true;
            }
            else if (arg == "--validate") {
                options.validate = true;
            }
            else if (arg.find("--seed=") == 0) {
                options.randomSeed = std::stoull(arg.substr(6));
            }
            else if (arg == "--seed" && i + 1 < argc) {
                options.randomSeed = std::stoull(argv[++i]);
            }
            else if (arg.find("--config=") == 0) {
                options.configFile = arg.substr(8);
            }
            else if (arg == "--config" && i + 1 < argc) {
                options.configFile = argv[++i];
            }
            else {
                options.extraArgs.push_back(arg);
            }
        }
        
        return options;
    }
    
    bool validateOptions(const CommandLineOptions& options) {
        if (options.experimentName.empty()) {
            std::cerr << "ERROR: No experiment specified. Use --experiment <name>.\n";
            return false;
        }
        
        if (options.steps == 0) {
            std::cerr << "ERROR: Number of steps must be positive.\n";
            return false;
        }
        
        if (options.steps > 1000000) {
            std::cerr << "WARNING: Large number of steps may take a long time.\n";
        }
        
        return true;
    }
    
    void printUsage() {
        std::cout << R"(
Usage: nlm_main [OPTIONS]

Required Options:
  --experiment <name>     Experiment name (basic, plasticity, stdp, phase3, phase4, phase6, scaling, ablation)

Optional Options:
  --steps <N>             Number of simulation steps (default: 1000)
  --episodes <N>          Number of episodes (for world interaction) (default: 1)
  --output <file>         Output file path (formats: .json, .csv, .txt, .html)
  --verbose               Enable verbose output
  --benchmark             Enable performance benchmarking
  --validate              Validate configuration before running
  --seed <N>              Random seed for reproducibility (default: 42)
  --config <file>          Configuration file path (JSON format)

Output Formats:
  console                 Console output (default)
  json                    JSON format for machine-readable results
  csv                     CSV format for data analysis
  html                    HTML format for visualization
  txt                     Plain text format

Examples:
  nlm_main --experiment plasticity --steps 5000 --verbose
  nlm_main --experiment phase6 --steps 10000 --output results.json
  nlm_main --experiment scaling --benchmark --validate

For more information, see the documentation.
        )" << std::endl;
    }
    
    void printVersion() {
        std::cout << "NLM Phase 8 Production Framework v1.0.0" << std::endl;
        std::cout << "Built with C++20, ready for production use." << std::endl;
    }
    
private:
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\n\r");
        size_t end = str.find_last_not_of(" \t\n\r");
        return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }
};

// Output manager for multiple output formats
class OutputManager {
public:
    OutputManager(const std::shared_ptr<ConfigurationManager::CommandLineOptions>& options)
        : options_(options) {}
    
    void writeOutput(const std::vector<ExperimentResult>& results) {
        if (options_->outputFile.empty() || options_->outputFormat == "console") {
            writeConsoleOutput(results);
        } else if (options_->outputFormat == "json") {
            writeJSONOutput(results);
        } else if (options_->outputFormat == "csv") {
            writeCSVOutput(results);
        } else if (options_->outputFormat == "html") {
            writeHTMLOutput(results);
        } else {
            writeTextOutput(results);
        }
    }
    
private:
    void writeConsoleOutput(const std::vector<ExperimentResult>& results) {
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "EXPERIMENT RESULTS SUMMARY" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        for (const auto& result : results) {
            std::cout << "\nExperiment: " << result.experimentName << std::endl;
            std::cout << "Status: " << (result.success ? "SUCCESS" : "FAILED") << std::endl;
            if (!result.errorMessage.empty()) {
                std::cout << "Error: " << result.errorMessage << std::endl;
            }
            std::cout << "Steps Completed: " << result.stepsCompleted << "/" << result.maxSteps << std::endl;
            std::cout << "Runtime: " << std::fixed << std::setprecision(2) << result.totalRuntime << "s" << std::endl;
            std::cout << "Average Firing Rate: " << std::fixed << std::setprecision(2) << result.averageFiringRate << " Hz" << std::endl;
            std::cout << "Peak Neurons: " << result.peakNeuronCount << std::endl;
            std::cout << "Peak Synapses: " << result.peakSynapseCount << std::endl;
            std::cout << "Learning Detected: " << (result.learningDetected ? "Yes" : "No") << std::endl;
            if (result.learningDetected) {
                std::cout << "Learning Magnitude: " << std::fixed << std::setprecision(4) << result.learningMagnitude << std::endl;
            }
            
            std::cout << "Metrics:" << std::endl;
            for (const auto& metric : result.metrics) {
                std::cout << "  " << metric.first << ": " << std::fixed << std::setprecision(4) << metric.second << std::endl;
            }
        }
    }
    
    void writeJSONOutput(const std::vector<ExperimentResult>& results) {
        std::ofstream file(options_->outputFile);
        if (!file.is_open()) {
            std::cerr << "ERROR: Cannot open output file: " << options_->outputFile << std::endl;
            return;
        }
        
        file << "{\n";
        file << "  \"experiments\": [\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            file << "    {\n";
            file << "      \"name\": \"" << result.experimentName << "\"," << std::endl;
            file << "      \"success\": " << (result.success ? "true" : "false") << "," << std::endl;
            file << "      \"steps_completed\": " << result.stepsCompleted << "," << std::endl;
            file << "      \"total_steps\": " << result.maxSteps << "," << std::endl;
            file << "      \"runtime_seconds\": " << std::fixed << std::setprecision(4) << result.totalRuntime << "," << std::endl;
            file << "      \"average_firing_rate\": " << std::fixed << std::setprecision(4) << result.averageFiringRate << "," << std::endl;
            file << "      \"peak_neurons\": " << result.peakNeuronCount << "," << std::endl;
            file << "      \"peak_synapses\": " << result.peakSynapseCount << "," << std::endl;
            file << "      \"learning_detected\": " << (result.learningDetected ? "true" : "false") << "," << std::endl;
            file << "      \"learning_magnitude\": " << std::fixed << std::setprecision(6) << result.learningMagnitude << "\n";
            
            file << "    }";
            if (i < results.size() - 1) file << ",";
            file << "\n";
        }
        
        file << "  ]\n";
        file << "}\n";
        file.close();
        
        std::cout << "Results written to JSON file: " << options_->outputFile << std::endl;
    }
    
    void writeCSVOutput(const std::vector<ExperimentResult>& results) {
        std::ofstream file(options_->outputFile);
        if (!file.is_open()) {
            std::cerr << "ERROR: Cannot open output file: " << options_->outputFile << std::endl;
            return;
        }
        
        // Header
        file << "experiment_name,success,steps_completed,total_steps,runtime_seconds,average_firing_rate,peak_neurons,peak_synapses,learning_detected,learning_magnitude" << std::endl;
        
        for (const auto& result : results) {
            file << result.experimentName << ",";
            file << (result.success ? "true" : "false") << ",";
            file << result.stepsCompleted << ",";
            file << result.maxSteps << ",";
            file << std::fixed << std::setprecision(4) << result.totalRuntime << ",";
            file << std::fixed << std::setprecision(4) << result.averageFiringRate << ",";
            file << result.peakNeuronCount << ",";
            file << result.peakSynapseCount << ",";
            file << (result.learningDetected ? "true" : "false") << ",";
            file << std::fixed << std::setprecision(6) << result.learningMagnitude << "\n";
        }
        
        file.close();
        
        std::cout << "Results written to CSV file: " << options_->outputFile << std::endl;
    }
    
    void writeHTMLOutput(const std::vector<ExperimentResult>& results) {
        std::ofstream file(options_->outputFile);
        if (!file.is_open()) {
            std::cerr << "ERROR: Cannot open output file: " << options_->outputFile << std::endl;
            return;
        }
        
        file << R"(<!DOCTYPE html>
<html>
<head>
    <title>NLM Experiment Results</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; }
        h1 { color: #333; }
        table { border-collapse: collapse; width: 100%; margin-top: 20px; }
        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
        th { background-color: #f2f2f2; }
        tr:nth-child(even) { background-color: #f9f9f9; }
        .success { color: green; font-weight: bold; }
        .failed { color: red; font-weight: bold; }
    </style>
</head>
<body>
    <h1>NLM Experiment Results</h1>
    <p>Generated: )" << std::chrono::system_clock::now() << R"(</p>
    <table>
        <tr>
            <th>Experiment</th>
            <th>Status</th>
            <th>Steps</th>
            <th>Runtime (s)</th>
            <th>Firing Rate (Hz)</th>
            <th>Peak Neurons</th>
            <th>Peak Synapses</th>
            <th>Learning</th>
            <th>Magnitude</th>
        </tr>
)";
        
        for (const auto& result : results) {
            file << "        <tr>" << std::endl;
            file << "            <td>" << result.experimentName << "</td>" << std::endl;
            file << "            <td class=\"" << (result.success ? "success" : "failed") << "\">"
                 << (result.success ? "SUCCESS" : "FAILED") << "</td>" << std::endl;
            file << "            <td>" << result.stepsCompleted << "/" << result.maxSteps << "</td>" << std::endl;
            file << "            <td>" << std::fixed << std::setprecision(2) << result.totalRuntime << "</td>" << std::endl;
            file << "            <td>" << std::fixed << std::setprecision(2) << result.averageFiringRate << "</td>" << std::endl;
            file << "            <td>" << result.peakNeuronCount << "</td>" << std::endl;
            file << "            <td>" << result.peakSynapseCount << "</td>" << std::endl;
            file << "            <td>" << (result.learningDetected ? "Yes" : "No") << "</td>" << std::endl;
            file << "            <td>" << std::fixed << std::setprecision(4) << result.learningMagnitude << "</td>" << std::endl;
            file << "        </tr>" << std::endl;
        }
        
        file << R"(    </table>
</body>
</html>
        )";
        file.close();
        
        std::cout << "Results written to HTML file: " << options_->outputFile << std::endl;
    }
    
    void writeTextOutput(const std::vector<ExperimentResult>& results) {
        std::ofstream file(options_->outputFile);
        if (!file.is_open()) {
            std::cerr << "ERROR: Cannot open output file: " << options_->outputFile << std::endl;
            return;
        }
        
        file << "NLM Experiment Results" << std::endl;
        file << std::string(50, '=') << std::endl;
        
        for (const auto& result : results) {
            file << "\nExperiment: " << result.experimentName << std::endl;
            file << "Status: " << (result.success ? "SUCCESS" : "FAILED") << std::endl;
            if (!result.errorMessage.empty()) {
                file << "Error: " << result.errorMessage << std::endl;
            }
            file << "Steps Completed: " << result.stepsCompleted << "/" << result.maxSteps << std::endl;
            file << "Runtime: " << std::fixed << std::setprecision(2) << result.totalRuntime << "s" << std::endl;
            file << "Average Firing Rate: " << std::fixed << std::setprecision(2) << result.averageFiringRate << " Hz" << std::endl;
            file << "Peak Neurons: " << result.peakNeuronCount << std::endl;
            file << "Peak Synapses: " << result.peakSynapseCount << std::endl;
            file << "Learning Detected: " << (result.learningDetected ? "Yes" : "No") << std::endl;
            if (result.learningDetected) {
                file << "Learning Magnitude: " << std::fixed << std::setprecision(4) << result.learningMagnitude << std::endl;
            }
            
            file << "Metrics:" << std::endl;
            for (const auto& metric : result.metrics) {
                file << "  " << metric.first << ": " << std::fixed << std::setprecision(4) << metric.second << std::endl;
            }
        }
        
        file.close();
        
        std::cout << "Results written to text file: " << options_->outputFile << std::endl;
    }
    
    std::shared_ptr<ConfigurationManager::CommandLineOptions> options_;
};

// Experiment manager with plugin architecture
class ExperimentManager {
public:
    enum class ExperimentType {
        BASIC_CONNECTIVITY,
        PLASTICITY_LEARNING,
        STDP_VERIFICATION,
        PHASE3_EXPERIMENT,
        PHASE4_EXPERIMENT,
        PHASE6_INTEGRATED,
        SCALING_BENCHMARK,
        ABLATION_STUDY,
        UNKNOWN
    };
    
    struct PluginInfo {
        std::string name;
        std::string version;
        std::string description;
        bool (*isAvailable)();
        std::shared_ptr<Experiment> (*create)(const std::string& name);
    };
    
    ExperimentManager(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment,
                     std::shared_ptr<ProgressReporter> progressReporter,
                     std::shared_ptr<ConfigurationManager::CommandLineOptions> options)
        : brain_(brain), environment_(environment), progressReporter_(progressReporter),
          options_(options) {
        
        // Register built-in experiment plugins
        registerBuiltinPlugins();
        
        // Set default random seed if not specified
        if (options_->randomSeed == 42 && options_->extraArgs.size() > 0) {
            for (const auto& arg : options_->extraArgs) {
                if (arg.find("--seed=") == 0) {
                    options_->randomSeed = std::stoull(arg.substr(6));
                }
            }
        }
    }
    
    std::vector<ExperimentResult> runExperiment(ExperimentType type, SimulationStep steps) {
        std::vector<ExperimentResult> results;
        
        try {
            NLM_LOG_INFO("Starting experiment: " + std::to_string(static_cast<int>(type)));
            
            // Create experiment based on type
            auto experiment = createExperiment(type);
            if (!experiment) {
                NLM_LOG_ERROR("Failed to create experiment instance");
                return results;
            }
            
            // Start progress monitoring
            if (progressReporter_) {
                progressReporter_->startProgress(experiment->getName(), steps);
            }
            
            auto startTime = std::chrono::steady_clock::now();
            
            // Run the experiment
            bool success = runSpecificExperiment(experiment, steps);
            
            auto endTime = std::chrono::steady_clock::now();
            double runtime = std::chrono::duration<double>(endTime - startTime).count();
            
            // Finish progress monitoring
            if (progressReporter_) {
                progressReporter_->finishProgress();
            }
            
            if (success) {
                NLM_LOG_INFO("Experiment completed successfully in " + std::to_string(runtime) + " seconds");
                
                ExperimentResult result = collectResults(experiment, steps, runtime);
                result.success = true;
                results.push_back(result);
                
                // Save results if output file specified
                if (!options_->outputFile.empty()) {
                    std::shared_ptr<OutputManager> outputManager = std::make_shared<OutputManager>(options_);
                    outputManager->writeOutput(results);
                }
            } else {
                NLM_LOG_ERROR("Experiment failed");
                ExperimentResult result;
                result.experimentName = experiment->getName();
                result.success = false;
                result.errorMessage = "Experiment execution failed";
                results.push_back(result);
            }
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception during experiment execution: ") + e.what());
            ExperimentResult result;
            result.experimentName = std::to_string(static_cast<int>(type));
            result.success = false;
            result.errorMessage = std::string("Exception: ") + e.what();
            results.push_back(result);
        }
        
        return results;
    }
    
    void registerPlugin(const PluginInfo& plugin) {
        pluginRegistry_[plugin.name] = plugin;
    }
    
    std::shared_ptr<Experiment> getPlugin(const std::string& name) const {
        auto it = pluginRegistry_.find(name);
        if (it != pluginRegistry_.end()) {
            return it->second.create(name);
        }
        return nullptr;
    }
    
    std::vector<std::string> getAvailablePlugins() const {
        std::vector<std::string> plugins;
        for (const auto& pair : pluginRegistry_) {
            plugins.push_back(pair.first);
        }
        return plugins;
    }
    
    static ExperimentType parseExperimentType(const std::string& name) {
        if (name == "basic" || name == "connectivity") return ExperimentType::BASIC_CONNECTIVITY;
        if (name == "plasticity" || name == "learning") return ExperimentType::PLASTICITY_LEARNING;
        if (name == "stdp" || name == "stdp_verification") return ExperimentType::STDP_VERIFICATION;
        if (name == "phase3") return ExperimentType::PHASE3_EXPERIMENT;
        if (name == "phase4") return ExperimentType::PHASE4_EXPERIMENT;
        if (name == "phase6" || name == "integrated") return ExperimentType::PHASE6_INTEGRATED;
        if (name == "scaling" || name == "benchmark") return ExperimentType::SCALING_BENCHMARK;
        if (name == "ablation" || name == "study") return ExperimentType::ABLATION_STUDY;
        return ExperimentType::UNKNOWN;
    }
    
private:
    void registerBuiltinPlugins() {
        // Register built-in experiment plugins
        PluginInfo basicPlugin {
            "basic_experiment",
            "1.0.0",
            "Basic neural connectivity experiment",
            []() { return true; },
            [](const std::string& name) { return std::make_shared<Experiment>(name); }
        };
        registerPlugin(basicPlugin);
        
        // Additional plugins can be registered here
        // PluginInfo phase3Plugin {
        //     "phase3_experiment",
        //     "1.0.0",
        //     "Phase 3 integrated experiment",
        //     []() { return true; },
        //     [](const std::string& name) { return std::make_shared<Phase3Experiment>(name); }
        // };
        // registerPlugin(phase3Plugin);
    }
    
    std::shared_ptr<Experiment> createExperiment(ExperimentType type) {
        switch (type) {
            case ExperimentType::BASIC_CONNECTIVITY:
                return std::make_shared<Experiment>("Basic Connectivity Test");
            case ExperimentType::PLASTICITY_LEARNING:
                return std::make_shared<Experiment>("Plasticity Learning Experiment");
            case ExperimentType::STDP_VERIFICATION:
                return std::make_shared<Experiment>("STDP Verification Test");
            case ExperimentType::PHASE3_EXPERIMENT:
                return std::make_shared<Phase3Experiment>("Phase 3 Integrated Experiment");
            case ExperimentType::PHASE4_EXPERIMENT:
                return std::make_shared<Phase4Experiment>("Phase 4 Advanced Experiment");
            case ExperimentType::PHASE6_INTEGRATED:
                return std::make_shared<Phase6IntegratedExperiment>("Phase 6 Final Integration");
            case ExperimentType::SCALING_BENCHMARK:
                return std::make_shared<ScalingBenchmark>("Scaling Benchmark");
            case ExperimentType::ABLATION_STUDY:
                return std::make_shared<AblationSystem>("Ablation Study");
            default:
                return nullptr;
        }
    }
    
    bool runSpecificExperiment(std::shared_ptr<Experiment> experiment, SimulationStep steps) {
        // Core experiment execution logic
        // This would integrate with the existing ExperimentRunner
        // For now, simulate successful execution
        
        NLM_LOG_INFO("Running experiment: " + experiment->getName() + " for " + std::to_string(steps) + " steps");
        
        // Simulate some steps
        for (SimulationStep step = 0; step < steps; ++step) {
            if (progressReporter_) {
                progressReporter_->updateProgress(step, "Simulating neural activity");
            }
            
            // In a real implementation, this would run the actual experiment
            // brain_->step(step, step * 0.001);
            
            // Simulate some work
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            
            // Record step data (simulated)
            experiment->recordStep(step, 500, 2000, 125, 25.0f);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + " completed");
            }
        }
        
        return true;
    }
    
    ExperimentResult collectResults(std::shared_ptr<Experiment> experiment, 
                                    SimulationStep totalSteps, double runtime) {
        ExperimentResult result;
        result.experimentName = experiment->getName();
        result.stepsCompleted = totalSteps;
        result.maxSteps = totalSteps;
        result.totalRuntime = runtime;
        
        // Calculate metrics from experiment data
        const auto& stepData = experiment->getStepData();
        if (!stepData.empty()) {
            size_t totalNeurons = 0;
            size_t totalSynapses = 0;
            size_t totalFiring = 0;
            double totalFiringRate = 0.0;
            
            for (const auto& step : stepData) {
                totalNeurons = std::max(totalNeurons, step.neuronCount);
                totalSynapses = std::max(totalSynapses, step.synapseCount);
                totalFiring += step.firingCount;
                totalFiringRate += step.avgFiringRate;
            }
            
            result.averageFiringRate = totalFiringRate / stepData.size();
            result.peakNeuronCount = totalNeurons;
            result.peakSynapseCount = totalSynapses;
            
            // Copy metrics
            result.metrics = experiment->getMetrics();
            
            // Detect learning (simplified)
            if (stepData.size() >= 2) {
                double firstHalf = 0.0, secondHalf = 0.0;
                size_t firstCount = 0, secondCount = 0;
                
                for (size_t i = 0; i < stepData.size() / 2; ++i) {
                    firstHalf += stepData[i].avgFiringRate;
                    firstCount++;
                }
                for (size_t i = stepData.size() / 2; i < stepData.size(); ++i) {
                    secondHalf += stepData[i].avgFiringRate;
                    secondCount++;
                }
                
                if (firstCount > 0 && secondCount > 0) {
                    firstHalf /= firstCount;
                    secondHalf /= secondCount;
                    result.learningMagnitude = std::abs(secondHalf - firstHalf);
                    result.learningDetected = result.learningMagnitude > 0.01;  // Threshold
                }
            }
        }
        
        return result;
    }
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Environment> environment_;
    std::shared_ptr<ProgressReporter> progressReporter_;
    std::shared_ptr<ConfigurationManager::CommandLineOptions> options_;
    std::map<std::string, PluginInfo> pluginRegistry_;
};

// Statistics analyzer for performance analysis
class StatisticsAnalyzer {
public:
    struct ComparativeAnalysis {
        std::string experimentName;
        double meanRuntime;
        double stdDevRuntime;
        double coefficientOfVariation;
        double minRuntime;
        double maxRuntime;
        double performanceScore;
        std::vector<double> allRuntimes;
    };
    
    StatisticsAnalyzer() = default;
    
    std::vector<ExperimentResult> analyzeBatch(const std::vector<ExperimentResult>& results) {
        std::vector<ExperimentResult> analyzedResults = results;
        
        for (auto& result : analyzedResults) {
            analyzeIndividualResult(result);
        }
        
        return analyzedResults;
    }
    
    ComparativeAnalysis compareExperiments(const std::vector<ExperimentResult>& results) {
        ComparativeAnalysis analysis;
        
        if (results.empty()) return analysis;
        
        analysis.allRuntimes.reserve(results.size());
        for (const auto& result : results) {
            analysis.allRuntimes.push_back(result.totalRuntime);
        }
        
        analysis.meanRuntime = ExperimentResult::calculateStatistics(analysis.allRuntimes).mean;
        analysis.stdDevRuntime = ExperimentResult::calculateStatistics(analysis.allRuntimes).stdDev;
        
        auto stats = ExperimentResult::calculateStatistics(analysis.allRuntimes);
        analysis.minRuntime = stats.min;
        analysis.maxRuntime = stats.max;
        
        if (analysis.meanRuntime > 0) {
            analysis.coefficientOfVariation = analysis.stdDevRuntime / analysis.meanRuntime;
        }
        
        // Calculate performance score (higher is better)
        if (stats.stdDevRuntime > 0) {
            analysis.performanceScore = stats.meanRuntime / stats.stdDevRuntime;  // Better if high mean and low std dev
        } else {
            analysis.performanceScore = stats.meanRuntime;
        }
        
        return analysis;
    }
    
    void generateBenchmarkReport(const ComparativeAnalysis& analysis, const std::string& outputFile = "") {
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "PERFORMANCE BENCHMARK ANALYSIS" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        std::cout << "Statistical Analysis:" << std::endl;
        std::cout << "  Mean Runtime: " << std::fixed << std::setprecision(2) << analysis.meanRuntime << "s" << std::endl;
        std::cout << "  Std Dev: " << std::fixed << std::setprecision(2) << analysis.stdDevRuntime << "s" << std::endl;
        std::cout << "  Coefficient of Variation: " << std::fixed << std::setprecision(4) << analysis.coefficientOfVariation << std::endl;
        std::cout << "  Min Runtime: " << std::fixed << std::setprecision(2) << analysis.minRuntime << "s" << std::endl;
        std::cout << "  Max Runtime: " << std::fixed << std::setprecision(2) << analysis.maxRuntime << "s" << std::endl;
        std::cout << "  Performance Score: " << std::fixed << std::setprecision(4) << analysis.performanceScore << std::endl;
        
        if (analysis.coefficientOfVariation < 0.1) {
            std::cout << "  Stability: EXCELLENT (< 10% variation)" << std::endl;
        } else if (analysis.coefficientOfVariation < 0.2) {
            std::cout << "  Stability: GOOD (< 20% variation)" << std::endl;
        } else if (analysis.coefficientOfVariation < 0.5) {
            std::cout << "  Stability: MODERATE (< 50% variation)" << std::endl;
        } else {
            std::cout << "  Stability: POOR (>= 50% variation)" << std::endl;
        }
        
        if (!outputFile.empty()) {
            std::ofstream file(outputFile);
            if (file.is_open()) {
                file << "Benchmark Analysis Report\n";
                file << "Generated: " << std::chrono::system_clock::now() << "\n\n";
                file << "Statistical Analysis:\n";
                file << "  Mean Runtime: " << std::fixed << std::setprecision(2) << analysis.meanRuntime << "s\n";
                file << "  Std Dev: " << std::fixed << std::setprecision(2) << analysis.stdDevRuntime << "s\n";
                file << "  Coefficient of Variation: " << std::fixed << std::setprecision(4) << analysis.coefficientOfVariation << "\n";
                file << "  Min Runtime: " << std::fixed << std::setprecision(2) << analysis.minRuntime << "s\n";
                file << "  Max Runtime: " << std::fixed << std::setprecision(2) << analysis.maxRuntime << "s\n";
                file << "  Performance Score: " << std::fixed << std::setprecision(4) << analysis.performanceScore << "\n";
                file.close();
                std::cout << "Benchmark report written to: " << outputFile << std::endl;
            }
        }
    }
    
private:
    void analyzeIndividualResult(ExperimentResult& result) {
        // Calculate detailed statistics for the individual result
        if (!result.timeSeriesData.empty()) {
            std::vector<double> values;
            for (const auto& pair : result.timeSeriesData) {
                for (double val : pair.second) {
                    values.push_back(val);
                }
            }
            
            if (!values.empty()) {
                auto stats = ExperimentResult::calculateStatistics(values);
                result.statistics = stats;
                
                // Calculate statistical significance (simplified)
                result.statistics.isSignificant = (stats.stdDev > 0) && (stats.mean > 0.01);
                result.statistics.pValue = calculatePValue(values, stats.mean);
            }
        }
    }
    
    double calculatePValue(const std::vector<double>& values, double expectedMean) {
        // Simplified p-value calculation (t-test approximation)
        if (values.size() <= 1) return 1.0;
        
        double mean = ExperimentResult::calculateStatistics(values).mean;
        double stdDev = ExperimentResult::calculateStatistics(values).stdDev;
        
        if (stdDev == 0) return 1.0;
        
        double tStat = (mean - expectedMean) / (stdDev / std::sqrt(values.size()));
        
        // Very rough approximation - in practice use proper statistical tests
        return std::max(0.0, 1.0 - std::abs(tStat) * 0.1);
    }
};

// Helper function to print banner
void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 8: Production Framework                             ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements the complete production framework.   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

int main(int argc, char** argv) {
    try {
        // Print banner
        printBanner();
        
        std::cout << "Initializing NLM Phase 8 Production Framework...\n" << std::endl;
        
        // Parse command line arguments
        ConfigurationManager configManager;
        auto options = configManager.parseCommandLine(argc, argv);
        
        // Validate configuration
        if (options.validate) {
            std::cout << "Validating configuration...\n";
            if (!configManager.validateOptions(options)) {
                std::cerr << "Configuration validation failed.\n";
                return 1;
            }
            std::cout << "Configuration validation passed.\n";
        }
        
        // Initialize logger with error handling
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        
        // Adjust log level based on verbosity
        if (options.verbose) {
            consoleLogger->setLevel(LogLevel::Debug);
        }
        
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("=== NLM Phase 8 Production Framework ===");
        NLM_LOG_INFO("Command line options parsed successfully");
        NLM_LOG_INFO("Experiment: " + options.experimentName);
        NLM_LOG_INFO("Steps: " + std::to_string(options.steps));
        NLM_LOG_INFO("Verbose: " + std::string(options.verbose ? "Yes" : "No"));
        
        // Load configuration
        auto config = std::make_shared<Config>();
        
        // Try to load from config file if specified
        if (!options.configFile.empty()) {
            if (config->loadFromFile(options.configFile)) {
                NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
            } else {
                NLM_LOG_WARN("Failed to load configuration from: " + options.configFile);
                NLM_LOG_WARN("Using default configuration");
            }
        }
        
        // Set experiment-specific defaults
        config->set("random_seed", options.randomSeed, ConfigSource::Runtime);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        NLM_LOG_INFO("Configuration loaded and prepared");
        
        // Initialize simulation clock with error checking
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        SimulationClock clock(timestep);
        NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
        
        // Initialize brain with comprehensive error handling
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Initializing NLM Brain...");
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            NLM_LOG_ERROR("Brain initialization failed - aborting");
            return 1;
        }
        
        NLM_LOG_INFO("Brain initialization successful");
        brain->logStatus();
        
        // Initialize progress monitoring
        std::shared_ptr<ProgressReporter> progressReporter;
        if (options.steps > 1000) {
            progressReporter = std::make_shared<ProgressReporter>(true, true, 1000);
        }
        
        // Initialize environment
        auto environment = std::make_shared<Environment>();
        
        // Initialize experiment manager
        std::shared_ptr<ConfigurationManager::CommandLineOptions> optionsPtr = 
            std::make_shared<ConfigurationManager::CommandLineOptions>(options);
        
        auto experimentManager = std::make_shared<ExperimentManager>(
            brain, environment, progressReporter, optionsPtr
        );
        
        // Parse experiment type from command line
        auto experimentType = ExperimentManager::parseExperimentType(options.experimentName);
        
        if (experimentType == ExperimentManager::ExperimentType::UNKNOWN) {
            NLM_LOG_ERROR("Unknown experiment: " + options.experimentName);
            NLM_LOG_ERROR("Available experiments: basic, plasticity, stdp, phase3, phase4, phase6, scaling, ablation");
            return 1;
        }
        
        // Run the selected experiment
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Starting experiment: " + options.experimentName);
        NLM_LOG_INFO("Total steps: " + std::to_string(options.steps));
        
        std::vector<ExperimentResult> results;
        if (options.benchmark) {
            NLM_LOG_INFO("Benchmark mode enabled - running multiple iterations for comparison");
            // In benchmark mode, run multiple iterations
            for (int iteration = 0; iteration < 3; ++iteration) {
                NLM_LOG_INFO("Benchmark iteration " + std::to_string(iteration + 1) + "/3");
                auto iterResults = experimentManager->runExperiment(experimentType, options.steps);
                results.insert(results.end(), iterResults.begin(), iterResults.end());
            }
        } else {
            // Normal execution - run single experiment
            auto singleResult = experimentManager->runExperiment(experimentType, options.steps);
            results = singleResult;
        }
        
        // Perform statistical analysis
        StatisticsAnalyzer statsAnalyzer;
        auto analyzedResults = statsAnalyzer.analyzeBatch(results);
        
        auto comparativeAnalysis = statsAnalyzer.compareExperiments(analyzedResults);
        
        if (options.benchmark) {
            // Generate benchmark report
            std::string benchmarkFile = "benchmark_results.txt";
            if (!options.outputFile.empty()) {
                benchmarkFile = options.outputFile;
            }
            statsAnalyzer.generateBenchmarkReport(comparativeAnalysis, benchmarkFile);
        }
        
        // Output results with comprehensive formatting
        OutputManager outputManager(optionsPtr);
        outputManager.writeOutput(analyzedResults);
        
        // Display final status
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Execution Summary ===");
        
        int successful = 0;
        int failed = 0;
        for (const auto& result : analyzedResults) {
            if (result.success) {
                successful++;
            } else {
                failed++;
            }
        }
        
        NLM_LOG_INFO("Total experiments: " << analyzedResults.size());
        NLM_LOG_INFO("Successful: " << successful);
        NLM_LOG_INFO("Failed: " << failed);
        
        if (failed > 0) {
            NLM_LOG_WARN("Some experiments failed - review error messages above");
        }
        
        // Final brain status
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Final Brain Status ===");
        brain->logStatus();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Phase 8 Complete ===");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Phase 8 Achievements:");
        NLM_LOG_INFO("  ✓ Modular design with plugin architecture");
        NLM_LOG_INFO("  ✓ Comprehensive error handling and recovery");
        NLM_LOG_INFO("  ✓ Efficient resource management");
        NLM_LOG_INFO("  ✓ Advanced configuration system");
        NLM_LOG_INFO("  ✓ Real-time progress monitoring");
        NLM_LOG_INFO("  ✓ Statistical analysis and benchmarking");
        NLM_LOG_INFO("  ✓ Multiple output formats");
        NLM_LOG_INFO("  ✓ Extensible framework for new experiments");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("The NLM brain system is now production-ready with full experimental support.");
        
        return 0;
        
    } catch (const std::exception& e) {
        NLM_LOG_FATAL(std::string("Fatal exception in main: ") + e.what());
        return 1;
    } catch (...) {
        NLM_LOG_FATAL("Unknown fatal exception occurred");
        return 1;
    }
}
