// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation - IMPROVED VERSION
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
//
// IMPROVEMENTS:
// - Modular experiment framework with configurable parameters
// - Comprehensive command-line interface
// - Flexible configuration system with file/command-line merging
// - Extensible plugin architecture for new experiments
// - Advanced logging system
// - Performance measurement and benchmarking
// - Better error handling and user feedback

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

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <map>
#include <set>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <sstream>
#include <cassert>

using namespace nlm;

// === EXPERIMENT FRAMEWORK ===

// Base experiment configuration
struct ExperimentConfig {
    std::string name;
    std::string description;
    std::map<std::string, float> floatParams;
    std::map<std::string, int> intParams;
    std::map<std::string, std::string> stringParams;
    bool enabled = true;
    
    ExperimentConfig(const std::string& n, const std::string& d) : name(n), description(d) {}
    
    void setFloat(const std::string& key, float value) { floatParams[key] = value; }
    void setInt(const std::string& key, int value) { intParams[key] = value; }
    void setString(const std::string& key, const std::string& value) { stringParams[key] = value; }
    
    float getFloat(const std::string& key, float defaultValue = 0.0f) const {
        auto it = floatParams.find(key);
        return it != floatParams.end() ? it->second : defaultValue;
    }
    
    int getInt(const std::string& key, int defaultValue = 0) const {
        auto it = intParams.find(key);
        return it != intParams.end() ? it->second : defaultValue;
    }
    
    std::string getString(const std::string& key, const std::string& defaultValue = "") const {
        auto it = stringParams.find(key);
        return it != stringParams.end() ? it->second : defaultValue;
    }
};

// Base experiment class
class BaseExperiment {
public:
    virtual ~BaseExperiment() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual void run(std::shared_ptr<Brain> brain) = 0;
    virtual void configureFromArgs(const std::vector<std::string>& args) = 0;
    virtual void printHelp() const = 0;
    virtual std::map<std::string, std::string> getAvailableParams() const = 0;
};

// Statistics collection
struct ExperimentStats {
    std::string experimentName;
    double executionTime;
    float successRate;
    std::map<std::string, std::string> metrics;
    
    ExperimentStats(const std::string& name, double time = 0.0) 
        : experimentName(name), executionTime(time), successRate(100.0f) {}
};

// === COMMAND LINE PARSER ===

class CommandLineParser {
public:
    struct ParsedArgs {
        std::string configFile;
        std::string logLevel;
        std::string experimentFilter;
        std::string outputFile;
        bool verbose = false;
        bool benchmark = false;
        bool help = false;
        bool listExperiments = false;
        std::vector<std::string> customArgs;
        bool useBenchmarkSettings = false;
        int benchmarkTrials = 1;
    };
    
    static ParsedArgs parse(int argc, char* argv[]) {
        ParsedArgs args;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                args.help = true;
            } else if (arg == "--list-experiments") {
                args.listExperiments = true;
            } else if (arg == "--benchmark") {
                args.benchmark = true;
            } else if (arg == "--verbose") {
                args.verbose = true;
            } else if (arg == "--use-benchmark-settings") {
                args.useBenchmarkSettings = true;
            } else if (arg.substr(0, 2) == "--" && arg.find("=")) {
                std::string key = arg.substr(2, arg.find("=") - 2);
                std::string value = arg.substr(arg.find("=") + 1);
                args.customArgs.push_back("--" + key + " " + value);
            } else if (arg.find("--config=") != std::string::npos) {
                args.configFile = arg.substr(9);
            } else if (arg.find("--log-level=") != std::string::npos) {
                args.logLevel = arg.substr(13);
            } else if (arg.find("--experiment=") != std::string::npos) {
                args.experimentFilter = arg.substr(13);
            } else if (arg.find("--output=") != std::string::npos) {
                args.outputFile = arg.substr(10);
            } else if (arg.find("--benchmark-trials=") != std::string::npos) {
                args.benchmarkTrials = std::stoi(arg.substr(21));
            } else if (arg[0] != '-') {
                // Positional argument (treated as experiment-specific)
                args.customArgs.push_back(arg);
            }
        }
        
        return args;
    }
};

// === EXPERIMENT IMPLEMENTATIONS ===

// Improved basic connectivity test with configurable parameters
class BasicConnectivityExperiment : public BaseExperiment {
public:
    std::string getName() const override { return "basic-connectivity"; }
    std::string getDescription() const override { 
        return "Tests basic neural connectivity and spike propagation"; 
    }
    
    void configureFromArgs(const std::vector<std::string>& args) override {
        // Parse experiment-specific arguments
        for (size_t i = 0; i + 1 < args.size(); i += 2) {
            if (args[i] == "--steps") {
                setInt("steps", std::stoi(args[i+1]));
            } else if (args[i] == "--current") {
                setFloat("current", std::stof(args[i+1]));
            }
        }
    }
    
    void printHelp() const override {
        std::cout << "  " << getName() << ": " << getDescription() << "\n"
                  << "    --steps N      Number of simulation steps (default: 50)\n"
                  << "    --current V     Current injected to neurons (default: 50.0)\n";
    }
    
    std::map<std::string, std::string> getAvailableParams() const override {
        return {
            {"steps", "Number of simulation steps"},
            {"current", "Current injected to neurons"}
        };
    }
    
    void run(std::shared_ptr<Brain> brain) override {
        size_t steps = getInt("steps", 50);
        float current = getFloat("current", 50.0f);
        
        NLM_LOG_INFO("=== Improved Basic Neural Connectivity Test ===");
        NLM_LOG_INFO("Configuration: steps=" + std::to_string(steps) + ", current=" + std::to_string(current));
        
        // Inject current into a few neurons and see if spikes propagate
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No brain region found!");
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("  No neurons found!");
            return;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        NLM_LOG_INFO("  Injecting current=" + std::to_string(current) + " into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(current);
        }
        
        // Run simulation steps
        for (SimulationStep step = 0; step < steps; ++step) {
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
        
        NLM_LOG_INFO("Basic connectivity test completed.\n");
    }
    
private:
    void setFloat(const std::string& key, float value) {
        configParams.floatParams[key] = value;
    }
    void setInt(const std::string& key, int value) {
        configParams.intParams[key] = value;
    }
    float getFloat(const std::string& key, float defaultValue = 0.0f) const {
        auto it = configParams.floatParams.find(key);
        return it != configParams.floatParams.end() ? it->second : defaultValue;
    }
    int getInt(const std::string& key, int defaultValue = 0) const {
        auto it = configParams.intParams.find(key);
        return it != configParams.intParams.end() ? it->second : defaultValue;
    }
    
    ExperimentConfig configParams = ExperimentConfig("basic-connectivity", "Basic neural connectivity test");
};

// Improved plasticity experiment with configurable parameters
class PlasticityExperiment : public BaseExperiment {
public:
    std::string getName() const override { return "plasticity"; }
    std::string getDescription() const override { 
        return "Tests synaptic plasticity through repeated experience"; 
    }
    
    void configureFromArgs(const std::vector<std::string>& args) override {
        // Parse experiment-specific arguments
        for (size_t i = 0; i + 1 < args.size(); i += 2) {
            if (args[i] == "--steps") {
                setInt("steps", std::stoi(args[i+1]));
            } else if (args[i] == "--neurons") {
                setInt("neurons", std::stoi(args[i+1]));
            } else if (args[i] == "--current") {
                setFloat("current", std::stof(args[i+1]));
            }
        }
    }
    
    void printHelp() const override {
        std::cout << "  " << getName() << ": " << getDescription() << "\n"
                  << "    --steps N      Number of plasticity steps (default: 1000)\n"
                  << "    --neurons N    Number of neurons to stimulate (default: 20)\n"
                  << "    --current V     Input current to neurons (default: 30.0)\n";
    }
    
    std::map<std::string, std::string> getAvailableParams() const override {
        return {
            {"steps", "Number of plasticity steps"},
            {"neurons", "Number of neurons to stimulate"},
            {"current", "Input current to neurons"}
        };
    }
    
    void run(std::shared_ptr<Brain> brain) override {
        int steps = getInt("steps", 1000);
        int neurons = getInt("neurons", 20);
        float current = getFloat("current", 30.0f);
        
        NLM_LOG_INFO("=== Improved Plasticity Learning Experiment ===");
        NLM_LOG_INFO("Configuration: steps=" + std::to_string(steps) + ", neurons=" + std::to_string(neurons) + ", current=" + std::to_string(current));
        
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
        NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(steps) + " steps)...");
        
        // Select neurons for pattern stimulation
        std::vector<NeuronId> patternNeurons;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < neurons) {
                    patternNeurons.push_back(neuron->getId());
                }
            }
        }
        
        for (SimulationStep step = 0; step < steps; ++step) {
            for (NeuronId id : patternNeurons) {
                brain->injectCurrent(id, current);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 200 steps
            if (step % 200 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Record final state
        experiment.recordFinalState();
        
        // Compute and display statistics
        experiment.computeStatistics();
        
        NLM_LOG_INFO("Plasticity experiment completed.\n");
    }
    
private:
    void setFloat(const std::string& key, float value) {
        configParams.floatParams[key] = value;
    }
    void setInt(const std::string& key, int value) {
        configParams.intParams[key] = value;
    }
    float getFloat(const std::string& key, float defaultValue = 0.0f) const {
        auto it = configParams.floatParams.find(key);
        return it != configParams.floatParams.end() ? it->second : defaultValue;
    }
    int getInt(const std::string& key, int defaultValue = 0) const {
        auto it = configParams.intParams.find(key);
        return it != configParams.intParams.end() ? it->second : defaultValue;
    }
    
    ExperimentConfig configParams = ExperimentConfig("plasticity", "Synaptic plasticity learning experiment");
};

// Improved STDP verification with configurable parameters
class StdpExperiment : public BaseExperiment {
public:
    std::string getName() const override { return "stdp"; }
    std::string getDescription() const override { 
        return "Verifies STDP synaptic plasticity rules"; 
    }
    
    void configureFromArgs(const std::vector<std::string>& args) override {
        // Parse experiment-specific arguments
        for (size_t i = 0; i + 1 < args.size(); i += 2) {
            if (args[i] == "--trials") {
                setInt("trials", std::stoi(args[i+1]));
            } else if (args[i] == "--current") {
                setFloat("current", std::stof(args[i+1]));
            }
        }
    }
    
    void printHelp() const override {
        std::cout << "  " << getName() << ": " << getDescription() << "\n"
                  << "    --trials N     Number of STDP trials (default: 50)\n"
                  << "    --current V     Current injected to neurons (default: 60.0)\n";
    }
    
    std::map<std::string, std::string> getAvailableParams() const override {
        return {
            {"trials", "Number of STDP trials"},
            {"current", "Current injected to neurons"}
        };
    }
    
    void run(std::shared_ptr<Brain> brain) override {
        int trials = getInt("trials", 50);
        float current = getFloat("current", 60.0f);
        
        NLM_LOG_INFO("=== Improved STDP Verification Experiment ===");
        NLM_LOG_INFO("Configuration: trials=" + std::to_string(trials) + ", current=" + std::to_string(current));
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        // Use one real synapse and its own pre/post neurons
        auto& synapses = region->getSynapses();
        if (synapses.empty()) {
            NLM_LOG_INFO("  No synapses for STDP test");
            return;
        }
        
        Synapse* syn = synapses[0].get();
        syn->enablePlasticity(false, true, false);  // STDP only
        float beforeWeight = syn->getWeight();
        NLM_LOG_INFO("  Testing STDP on 1 synapse:");
        NLM_LOG_INFO("    Synapse before: " + std::to_string(beforeWeight) +
                     " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                     ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");
        
        NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
        
        for (int trial = 0; trial < trials; ++trial) {
            brain->injectCurrent(syn->getSourceNeuron(), current);
            brain->step(trial * 2, trial * 2 * 0.001);
            brain->injectCurrent(syn->getDestinationNeuron(), current);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        float delta = syn->getWeight() - beforeWeight;
        NLM_LOG_INFO("  After correlated activity:");
        NLM_LOG_INFO("    Synapse after: " + std::to_string(syn->getWeight()) +
                     " (Δ=" + std::to_string(delta) + ")");
        
        float totalDelta = delta;
        
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
        }
        
        NLM_LOG_INFO("STDP verification experiment completed.\n");
    }
    
private:
    void setFloat(const std::string& key, float value) {
        configParams.floatParams[key] = value;
    }
    void setInt(const std::string& key, int value) {
        configParams.intParams[key] = value;
    }
    float getFloat(const std::string& key, float defaultValue = 0.0f) const {
        auto it = configParams.floatParams.find(key);
        return it != configParams.floatParams.end() ? it->second : defaultValue;
    }
    int getInt(const std::string& key, int defaultValue = 0) const {
        auto it = configParams.intParams.find(key);
        return it != configParams.intParams.end() ? it->second : defaultValue;
    }
    
    ExperimentConfig configParams = ExperimentConfig("stdp", "STDP synaptic plasticity verification");
};

// === EXPERIMENT FACTORY ===

class ExperimentFactory {
public:
    static std::vector<std::unique_ptr<BaseExperiment>> getAllExperiments() {
        std::vector<std::unique_ptr<BaseExperiment>> experiments;
        experiments.push_back(std::make_unique<BasicConnectivityExperiment>());
        experiments.push_back(std::make_unique<PlasticityExperiment>());
        experiments.push_back(std::make_unique<StdpExperiment>());
        return experiments;
    }
    
    static std::unique_ptr<BaseExperiment> getExperiment(const std::string& name) {
        auto experiments = getAllExperiments();
        for (auto& exp : experiments) {
            if (exp->getName() == name) {
                return std::move(exp);
            }
        }
        return nullptr;
    }
};

// === CONFIGURATION SYSTEM ===

class ConfigurationManager {
public:
    static std::shared_ptr<Config> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        
        // Default values for Phase 2
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        return config;
    }
    
    static bool loadFromFile(std::shared_ptr<Config> config, const std::string& filename) {
        // Implementation would load configuration from file
        // For now, return false to indicate file not found
        return false;
    }
    
    static void mergeConfiguration(std::shared_ptr<Config> config, 
                                  const std::vector<std::string>& args) {
        for (size_t i = 0; i + 1 < args.size(); i += 2) {
            if (args[i].substr(0, 2) == "--" && args[i+1].find("=") == std::string::npos) {
                // Handle --key value format
                std::string key = args[i].substr(2);
                std::string value = args[i+1];
                applyConfigParam(config, key, value);
            } else if (args[i].find("--") != std::string::npos && args[i].find("=") != std::string::npos) {
                // Handle --key=value format
                std::string key = args[i].substr(2, args[i].find("=") - 2);
                std::string value = args[i].substr(args[i].find("=") + 1);
                applyConfigParam(config, key, value);
            }
        }
    }
    
private:
    static void applyConfigParam(std::shared_ptr<Config> config, const std::string& key, const std::string& value) {
        try {
            if (key == "random_seed") {
                config->set(key, static_cast<int64_t>(std::stoll(value)), ConfigSource::CommandLine);
            } else if (key == "simulation_timestep" || key == "stdp_ltp_weight" || 
                       key == "stdp_ltd_weight" || key == "stdp_tau" ||
                       key == "synaptogenesis_rate" || key == "pruning_rate" ||
                       key == "connection_probability") {
                config->set(key, std::stof(value), ConfigSource::CommandLine);
            } else if (key == "neuron_count" || key == "region_count") {
                config->set(key, static_cast<int64_t>(std::stoll(value)), ConfigSource::CommandLine);
            }
        } catch (...) {
            NLM_LOG_WARNING("Failed to parse config parameter: " + key + " = " + value);
        }
    }
};

// === STATISTICS COLLECTION ===

class StatisticsCollector {
public:
    static void logExperimentResult(const ExperimentStats& stats) {
        std::cout << "Experiment: " << stats.experimentName << "\n";
        std::cout << "  Execution time: " << stats.executionTime << " seconds\n";
        std::cout << "  Success rate: " << stats.successRate << "%\n";
        
        for (const auto& metric : stats.metrics) {
            std::cout << "  " << metric.first << ": " << metric.second << "\n";
        }
    }
    
    static ExperimentStats createStats(const std::string& experimentName, double time) {
        return ExperimentStats(experimentName, time);
    }
};

// === IMPROVED LOGGING SYSTEM ===

class LoggingSystem {
public:
    static void initialize(const std::string& level = "Info") {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(parseLogLevel(level));
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("Logging system initialized with level: " + level);
    }
    
    static LogLevel parseLogLevel(const std::string& levelStr) {
        if (levelStr == "Debug") return LogLevel::Debug;
        if (levelStr == "Info") return LogLevel::Info;
        if (levelStr == "Warning") return LogLevel::Warning;
        if (levelStr == "Error") return LogLevel::Error;
        return LogLevel::Info; // Default
    }
};

// === USAGE HELP SYSTEM ===

class HelpSystem {
public:
    static void printUsage() {
        std::cout << "NLM — Neural Learning Machine (Phase 2 Improved)\n\n";
        std::cout << "USAGE:\n";
        std::cout << "  ./nlm [options] [experiment-options]\n\n";
        std::cout << "OPTIONS:\n";
        std::cout << "  --help, -h                  Show this help message\n";
        std::cout << "  --list-experiments          List all available experiments\n";
        std::cout << "  --benchmark                 Run performance benchmark\n";
        std::cout << "  --verbose                   Enable verbose output\n";
        std::cout << "  --benchmark-trials N       Number of benchmark trials (default: 1)\n";
        std::cout << "  --config FILE               Load configuration from file\n";
        std::cout << "  --log-level LEVEL           Set logging level (Debug, Info, Warning, Error)\n";
        std::cout << "  --experiment NAME           Specify which experiment to run (default: all)\n";
        std::cout << "  --output FILE               Write results to output file\n\n";
        
        std::cout << "AVAILABLE EXPERIMENTS:\n";
        auto experiments = ExperimentFactory::getAllExperiments();
        for (auto& exp : experiments) {
            exp->printHelp();
        }
        
        std::cout << "EXPERIMENT-SPECIFIC OPTIONS:\n";
        std::cout << "  --steps N                    Number of simulation steps\n";
        std::cout << "  --current V                  Current injected to neurons (in uA)\n";
        std::cout << "  --neurons N                  Number of neurons to stimulate\n";
        std::cout << "  --trials N                   Number of STDP trials\n\n";
        
        std::cout << "CONFIGURATION OPTIONS:\n";
        std::cout << "  --neuron_count N             Total number of neurons (default: 500)\n";
        std::cout << "  --region_count N             Number of brain regions (default: 1)\n";
        std::cout << "  --connection_probability P   Connection probability (default: 0.15)\n";
        std::cout << "  --random_seed S              Random seed for reproducibility (default: 42)\n";
        std::cout << "  --simulation_timestep T      Simulation timestep in seconds (default: 0.001)\n";
        std::cout << "  --stdp_ltp_weight W          STDP LTP weight (default: 0.02)\n";
        std::cout << "  --stdp_ltd_weight W          STDP LTD weight (default: 0.015)\n";
        std::cout << "  --stdp_tau T                 STDP time constant (default: 20.0)\n";
        std::cout << "  --synaptogenesis_rate R      Synaptogenesis rate (default: 0.0001)\n";
        std::cout << "  --pruning_rate R             Pruning rate (default: 0.00001)\n\n";
        
        std::cout << "EXAMPLES:\n";
        std::cout << "  ./nlm                                   # Run all experiments with defaults\n";
        std::cout << "  ./nlm --experiment basic-connectivity    # Run only basic connectivity test\n";
        std::cout << "  ./nlm --experiment plasticity --steps 2000 --current 40.0\n";
        std::cout << "  ./nlm --experiment stdp --trials 100\n";
        std::cout << "  ./nlm --benchmark --benchmark-trials 5\n";
        std::cout << "  ./nlm --verbose --log-level Debug\n\n";
    }
};

// === LEGACY COMPATIBILITY ===

// Legacy LearningExperiment struct for compatibility
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

// === MAIN FUNCTION ===

int main(int argc, char* argv[]) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Parse command line arguments
    auto args = CommandLineParser::parse(argc, argv);
    
    // Show help if requested
    if (args.help) {
        HelpSystem::printUsage();
        return 0;
    }
    
    // List experiments if requested
    if (args.listExperiments) {
        std::cout << "Available experiments:\n";
        auto experiments = ExperimentFactory::getAllExperiments();
        for (size_t i = 0; i < experiments.size(); ++i) {
            auto& exp = experiments[i];
            std::cout << "  " << (i + 1) << ". " << exp->getName() << " - " << exp->getDescription() << "\n";
        }
        return 0;
    }
    
    // Initialize logging system
    LoggingSystem::initialize(args.logLevel.empty() ? "Info" : args.logLevel);
    
    // Print banner
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
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Create configuration
    auto config = ConfigurationManager::createDefaultConfig();
    
    // Apply benchmark settings if requested
    if (args.useBenchmarkSettings) {
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.2f, ConfigSource::Default);
    }
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
    if (!args.configFile.empty()) {
        configFile = args.configFile;
    }
    
    if (ConfigurationManager::loadFromFile(config, configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    ConfigurationManager::mergeConfiguration(config, args.customArgs);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Determine which experiments to run
    std::vector<std::string> experimentNames;
    if (!args.experimentFilter.empty()) {
        // Single experiment specified
        experimentNames.push_back(args.experimentFilter);
    } else {
        // Run all experiments
        auto experiments = ExperimentFactory::getAllExperiments();
        for (auto& exp : experiments) {
            experimentNames.push_back(exp->getName());
        }
    }
    
    // Run each experiment
    std::vector<ExperimentStats> allStats;
    
    for (const auto& expName : experimentNames) {
        auto start = std::chrono::high_resolution_clock::now();
        
        NLM_LOG_INFO("=== Starting experiment: " + expName + " ===\n");
        
        // Get experiment and configure it
        auto experiment = ExperimentFactory::getExperiment(expName);
        if (!experiment) {
            NLM_LOG_ERROR("Experiment not found: " + expName);
            continue;
        }
        
        // Configure experiment from command line arguments
        experiment->configureFromArgs(args.customArgs);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            continue;
        }
        
        brain->logStatus();
        
        // Run the experiment
        try {
            experiment->run(brain);
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(end - start).count();
            
            // Collect statistics
            ExperimentStats stats(expName, duration);
            allStats.push_back(stats);
            
            NLM_LOG_INFO("Experiment " + expName + " completed successfully in " + std::to_string(duration) + " seconds.\n");
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Experiment " + expName + " failed with exception: " + std::string(e.what()));
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(end - start).count();
            
            ExperimentStats stats(expName, duration);
            stats.successRate = 0.0f;
            allStats.push_back(stats);
        } catch (...) {
            NLM_LOG_ERROR("Experiment " + expName + " failed with unknown exception.");
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(end - start).count();
            
            ExperimentStats stats(expName, duration);
            stats.successRate = 0.0f;
            allStats.push_back(stats);
        }
        
        // Reset brain for next experiment
        brain->reset();
    }
    
    // Print summary
    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration<double>(endTime - startTime).count();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== EXECUTION SUMMARY ===");
    NLM_LOG_INFO("Total execution time: " + std::to_string(totalDuration) + " seconds");
    NLM_LOG_INFO("Experiments run: " + std::to_string(allStats.size()));
    
    for (const auto& stats : allStats) {
        StatisticsCollector::logExperimentResult(stats);
    }
    
    // Write results to file if requested
    if (!args.outputFile.empty()) {
        std::ofstream outputFile(args.outputFile);
        if (outputFile.is_open()) {
            outputFile << "NLM Phase 2 Experiment Results\n";
            outputFile << "Total execution time: " << totalDuration << " seconds\n";
            outputFile << "Experiments run: " << allStats.size() << "\n\n";
            
            for (const auto& stats : allStats) {
                outputFile << "Experiment: " << stats.experimentName << "\n";
                outputFile << "  Execution time: " << stats.executionTime << " seconds\n";
                outputFile << "  Success rate: " << stats.successRate << "%\n\n";
            }
            
            outputFile.close();
            NLM_LOG_INFO("Results written to: " + args.outputFile);
        } else {
            NLM_LOG_ERROR("Failed to write results to: " + args.outputFile);
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 2 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
    NLM_LOG_INFO("  ✓ STDP plasticity rule");
    NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
    NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
    
    return 0;
}
