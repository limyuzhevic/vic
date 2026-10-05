// NLM Experiment Runner CLI Interface
// Provides command-line interface for running configurable experiments
// and managing neural network simulations

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>

namespace nlm {

class ExperimentCLI {
public:
    ExperimentCLI() = default;
    ~ExperimentCLI() = default;
    
    // Parse and execute command line arguments
    int run(int argc, char** argv);
    
    // Display help information
    static void printHelp();
    
    // Display version information
    static void printVersion();
    
    // List available experiments
    void listExperiments() const;
    
    // Run a specific experiment with configuration
    bool runExperiment(const std::string& experimentName, 
                      const std::shared_ptr<Config>& config);
    
    // Run benchmark suite
    bool runBenchmarkSuite(const std::shared_ptr<Config>& config);
    
    // Export results to file
    bool exportResults(const std::string& outputFile, 
                      const ExperimentResults& results);
    
    // Load experiment configuration from file
    bool loadExperimentConfig(const std::string& configFile, 
                             std::shared_ptr<Config>& config);
    
    // Validate configuration
    bool validateConfig(const std::shared_ptr<Config>& config) const;
    
    // Get available experiment names
    std::vector<std::string> getAvailableExperiments() const;
    
private:
    // Command handlers
    bool handleRunCommand(const std::vector<std::string>& args, 
                         const std::shared_ptr<Config>& config);
    bool handleBenchmarkCommand(const std::vector<std::string>& args, 
                               const std::shared_ptr<Config>& config);
    bool handleListCommand(const std::vector<std::string>& args);
    bool handleConfigCommand(const std::vector<std::string>& args, 
                            const std::shared_ptr<Config>& config);
    bool handleExportCommand(const std::vector<std::string>& args,
                           const std::shared_ptr<Config>& config);
    
    // Argument parsing utilities
    static std::vector<std::string> parseArguments(int argc, char** argv);
    static std::string trim(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    
    // Experiment registry
    static std::map<std::string, std::function<bool(const std::shared_ptr<Config>&)>> experimentRegistry;
};

// Global experiment registry instance
inline ExperimentCLI experimentCLI;

// Experiment base class for custom experiment implementations
class Experiment {
public:
    virtual ~Experiment() = default;
    
    // Get experiment name
    virtual std::string getName() const = 0;
    
    // Get experiment description
    virtual std::string getDescription() const = 0;
    
    // Get required configuration parameters
    virtual std::vector<std::string> getRequiredParams() const = 0;
    
    // Run the experiment
    virtual bool run(const std::shared_ptr<Config>& config) = 0;
    
    // Validate experiment configuration
    virtual bool validate(const std::shared_ptr<Config>& config) const = 0;
    
    // Get experiment results
    virtual ExperimentResults getResults() const = 0;
protected:
    ExperimentResults results;
};

// Experiment runner for configurable parameter sweeps
class ConfigurableExperiment : public Experiment {
public:
    ConfigurableExperiment(const std::string& name, const std::string& description);
    
    std::string getName() const override { return name_; }
    std::string getDescription() const override { return description_; }
    std::vector<std::string> getRequiredParams() const override { return requiredParams_; }
    
    // Add a parameter to sweep
    void addParameter(const std::string& paramName, 
                     const std::vector<float>& values);
    
    // Add a parameter with ranges
    void addParameterRange(const std::string& paramName,
                          float min, float max, float step);
    
    // Add a fixed parameter
    void addFixedParameter(const std::string& paramName, float value);
    
    bool validate(const std::shared_ptr<Config>& config) const override;
    bool run(const std::shared_ptr<Config>& config) override;
    
    ExperimentResults getResults() const override { return results; }
    
    // Get the current parameter combination
    std::map<std::string, float> getCurrentParams() const { return currentParams_; }
    
private:
    std::string name_;
    std::string description_;
    std::vector<std::string> requiredParams_;
    std::map<std::string, std::vector<float>> parameterSweeps_;
    std::map<std::string, float> fixedParameters_;
    std::map<std::string, float> currentParams_;
    int currentCombination_;
};

// Built-in experiment implementations
class BasicConnectivityExperiment : public Experiment {
public:
    std::string getName() const override { return "basic_connectivity"; }
    std::string getDescription() const override { 
        return "Tests basic neural connectivity and spike propagation"; 
    }
    std::vector<std::string> getRequiredParams() const override {
        return {"neuron_count", "injection_current", "steps"};
    }
    bool validate(const std::shared_ptr<Config>& config) const override;
    bool run(const std::shared_ptr<Config>& config) override;
    ExperimentResults getResults() const override { return results; }
};

class PlasticityExperiment : public Experiment {
public:
    std::string getName() const override { return "plasticity_learning"; }
    std::string getDescription() const override { 
        return "Tests synaptic plasticity through Hebbian and STDP rules"; 
    }
    std::vector<std::string> getRequiredParams() const override {
        return {"neuron_count", "hebbian_rate", "stdp_rate", "steps"};
    }
    bool validate(const std::shared_ptr<Config>& config) const override;
    bool run(const std::shared_ptr<Config>& config) override;
    ExperimentResults getResults() const override { return results; }
};

class STDPVerificationExperiment : public Experiment {
public:
    std::string getName() const override { return "stdp_verification"; }
    std::string getDescription() const override { 
        return "Verifies STDP learning rule with correlated pre-post activity"; 
    }
    std::vector<std::string> getRequiredParams() const override {
        return {"neuron_count", "pre_post_trials", "weight_threshold"};
    }
    bool validate(const std::shared_ptr<Config>& config) const override;
    bool run(const std::shared_ptr<Config>& config) override;
    ExperimentResults getResults() const override { return results; }
};

class ScalingExperiment : public ConfigurableExperiment {
public:
    ScalingExperiment() : ConfigurableExperiment("scaling", "Tests performance scaling with network size") {}
    
    bool run(const std::shared_ptr<Config>& config) override;
};

// Performance benchmarking utilities
class BenchmarkSuite {
public:
    struct BenchmarkResult {
        std::string name;
        double executionTimeMs;
        size_t memoryUsageKB;
        double spikesPerSecond;
        double synapsesPerMs;
    };
    
    BenchmarkSuite() = default;
    ~BenchmarkSuite() = default;
    
    // Run all benchmarks
    std::vector<BenchmarkResult> runAllBenchmarks(const std::shared_ptr<Config>& config);
    
    // Run specific benchmark
    BenchmarkResult runBenchmark(const std::string& benchmarkName, 
                                 const std::shared_ptr<Config>& config);
    
    // Get available benchmarks
    std::vector<std::string> getAvailableBenchmarks() const;
    
private:
    BenchmarkResult runConnectivityBenchmark(const std::shared_ptr<Config>& config);
    BenchmarkResult runPlasticityBenchmark(const std::shared_ptr<Config>& config);
    BenchmarkResult runMemoryBenchmark(const std::shared_ptr<Config>& config);
    BenchmarkResult runScalingBenchmark(const std::shared_ptr<Config>& config);
};

// Results export utilities
class ResultsExporter {
public:
    static bool exportCSV(const std::string& filename, const ExperimentResults& results);
    static bool exportJSON(const std::string& filename, const ExperimentResults& results);
    static bool exportHTML(const std::string& filename, const ExperimentResults& results);
    static bool exportText(const std::string& filename, const ExperimentResults& results);
};

// Error types
enum class CLIErrorType {
    SUCCESS = 0,
    INVALID_ARGUMENTS,
    EXPERIMENT_NOT_FOUND,
    CONFIG_FILE_ERROR,
    EXPERIMENT_VALIDATION_ERROR,
    EXECUTION_ERROR,
    EXPORT_ERROR,
    UNKNOWN_ERROR
};

class CLIException : public std::runtime_error {
public:
    CLIException(CLIErrorType type, const std::string& message);
    
    CLIErrorType getErrorType() const { return errorType_; }
    const char* what() const noexcept override { return message_.c_str(); }
    
private:
    CLIErrorType errorType_;
    std::string message_;
};

// Global experiment registry management
class ExperimentRegistry {
public:
    static void registerExperiment(const std::string& name, 
                                  std::function<bool(const std::shared_ptr<Config>&)> factory);
    static bool isExperimentRegistered(const std::string& name);
    static std::vector<std::string> getRegisteredExperiments();
    static std::function<bool(const std::shared_ptr<Config>&)> getExperimentFactory(const std::string& name);
    
private:
    static std::map<std::string, std::function<bool(const std::shared_ptr<Config>&)>> registry_;
};

// Factory function for creating experiment instances
extern "C" {
    // Plugin interface for loading custom experiments
    using ExperimentFactory = std::function<std::unique_ptr<Experiment>()>;
    using PluginEntryPoint = std::function<void(ExperimentRegistry&)>;
    
    std::unique_ptr<Experiment> createExperiment(const std::string& name);
}

} // namespace nlm

// Implementation details
namespace nlm {
    // ExperimentCLI implementation
    int ExperimentCLI::run(int argc, char** argv) {
        try {
            auto args = parseArguments(argc, argv);
            
            if (args.empty() || args[0] == "--help" || args[0] == "-h") {
                printHelp();
                return 0;
            }
            
            if (args[0] == "--version" || args[0] == "-v") {
                printVersion();
                return 0;
            }
            
            // Load configuration
            auto config = std::make_shared<Config>();
            std::string configFile = "configs/default.cfg";
            
            // Parse configuration from command line
            for (size_t i = 0; i < args.size(); ++i) {
                if (startsWith(args[i], "--config")) {
                    if (args[i].find('=') != std::string::npos) {
                        configFile = args[i].substr(args[i].find('=') + 1);
                    } else if (i + 1 < args.size()) {
                        configFile = args[++i];
                    }
                }
            }
            
            // Load configuration file
            if (configFile != "default" && !configFile.empty()) {
                loadExperimentConfig(configFile, config);
            }
            
            // Parse command line arguments for config
            for (size_t i = 0; i < args.size(); ++i) {
                if (startsWith(args[i], "--") && args[i].find('=') != std::string::npos) {
                    std::string key = args[i].substr(2, args[i].find('=') - 2);
                    std::string value = args[i].substr(args[i].find('=') + 1);
                    config->set(key, value, ConfigSource::CommandLine);
                }
            }
            
            // Dispatch to appropriate command handler
            if (args[0] == "run") {
                return handleRunCommand(std::vector<std::string>(args.begin() + 1, args.end()), config) ? 0 : 1;
            } else if (args[0] == "benchmark") {
                return handleBenchmarkCommand(std::vector<std::string>(args.begin() + 1, args.end()), config) ? 0 : 1;
            } else if (args[0] == "list") {
                return handleListCommand(std::vector<std::string>(args.begin() + 1, args.end())) ? 0 : 1;
            } else if (args[0] == "config") {
                return handleConfigCommand(std::vector<std::string>(args.begin() + 1, args.end()), config) ? 0 : 1;
            } else if (args[0] == "export") {
                return handleExportCommand(std::vector<std::string>(args.begin() + 1, args.end()), config) ? 0 : 1;
            } else {
                std::cerr << "Error: Unknown command: " << args[0] << std::endl;
                printHelp();
                return 1;
            }
            
        } catch (const CLIException& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return static_cast<int>(e.getErrorType());
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return static_cast<int>(CLIErrorType::UNKNOWN_ERROR);
        }
        
        return static_cast<int>(CLIErrorType::UNKNOWN_ERROR);
    }
    
    void ExperimentCLI::printHelp() {
        std::cout << "NLM Experiment Runner - Usage:" << std::endl;
        std::cout << "  nlm-experiment [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  run <experiment> [options]      Run a specific experiment" << std::endl;
        std::cout << "  benchmark [options]              Run performance benchmarks" << std::endl;
        std::cout << "  list [options]                   List available experiments/benchmarks" << std::endl;
        std::cout << "  config [options]                 Show or modify configuration" << std::endl;
        std::cout << "  export [options]                 Export results to file" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help, -h                       Show this help message" << std::endl;
        std::cout << "  --version, -v                    Show version information" << std::endl;
        std::cout << "  --config <file>                  Load configuration from file" << std::endl;
        std::cout << "  --output <file>                  Output file for results" << std::endl;
        std::cout << "  --format <format>                Output format (csv, json, html, text)" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm-experiment run basic_connectivity --neuron-count 100 --steps 1000" << std::endl;
        std::cout << "  nlm-experiment benchmark --runs 5 --scales neuron,synapse" << std::endl;
        std::cout << "  nlm-experiment list experiments" << std::endl;
        std::cout << "  nlm-experiment export --experiment basic_connectivity --output results.csv" << std::endl;
    }
    
    void ExperimentCLI::printVersion() {
        std::cout << "NLM Experiment Runner v2.0.0" << std::endl;
        std::cout << "Phase 2: Real Neural Computation" << std::endl;
        std::cout << "Built with performance optimizations and configurable experiments" << std::endl;
    }
    
    void ExperimentCLI::listExperiments() const {
        std::cout << "Available Experiments:" << std::endl;
        auto experiments = getAvailableExperiments();
        for (const auto& exp : experiments) {
            std::cout << "  " << exp << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "Available Benchmarks:" << std::endl;
        BenchmarkSuite benchmark;
        auto benchmarks = benchmark.getAvailableBenchmarks();
        for (const auto& bm : benchmarks) {
            std::cout << "  " << bm << std::endl;
        }
    }
    
    // Additional implementation would continue...
    
} // namespace nlm
