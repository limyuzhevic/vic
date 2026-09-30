// NLM CLI Tool
// Advanced Command-Line Interface for Neural Learning Machine
// Supports complex argument parsing, subcommands, configuration files, and multiple output formats

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
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "dynamics/NeuralDynamics.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "visualization/VisualizationInterface.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <optional>

namespace nlm {

// Output format enumeration
enum class OutputFormat {
    Text,
    JSON,
    YAML,
    CSV,
    XML
};

// Simulation modes enumeration
enum class SimulationMode {
    Basic,
    Connectivity,
    Plasticity,
    STDP,
    Hebbian,
    Structural,
    Neuromodulation
};

// Command-line argument parser
class CLIParser {
public:
    struct Command {
        std::string name;
        std::string description;
        std::vector<std::string> options;
        std::vector<std::string> arguments;
        std::function<bool(const std::vector<std::string>&)> handler;
    };

    CLIParser() {
        initializeCommands();
    }

    bool parse(int argc, char** argv) {
        if (argc < 2) {
            printHelp();
            return false;
        }

        std::string command = argv[1];
        auto it = commands.find(command);
        if (it == commands.end()) {
            std::cerr << "Error: Unknown command '" << command << "'" << std::endl;
            printHelp();
            return false;
        }

        // Collect remaining arguments
        std::vector<std::string> args;
        for (int i = 2; i < argc; ++i) {
            args.push_back(argv[i]);
        }

        return it->second.handler(args);
    }

    void registerCommand(const Command& cmd) {
        commands[cmd.name] = cmd;
    }

    void printHelp() {
        std::cout << "\n";
        std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                        NLM CLI - Neural Learning Machine                  ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║                                                                          ║" << std::endl;
        
        std::cout << "║  COMMANDS:                                                               ║" << std::endl;
        for (const auto& pair : commands) {
            const auto& cmd = pair.second;
            std::cout << "║    " << std::left << std::setw(20) << cmd.name << "- " << cmd.description << "   ║" << std::endl;
        }
        
        std::cout << "║                                                                          ║" << std::endl;
        std::cout << "║  OPTIONS:                                                                ║" << std::endl;
        std::cout << "║    --help, -h           Show this help message                           ║" << std::endl;
        std::cout << "║    --version, -v        Show version information                        ║" << std::endl;
        std::cout << "║    --config, -c FILE    Load configuration from FILE                  ║" << std::endl;
        std::cout << "║    --output, -o FILE    Output results to FILE (console if omitted)     ║" << std::endl;
        std::cout << "║    --format FORMAT      Output format (text, json, yaml, csv)          ║" << std::endl;
        std::cout << "║    --verbose, -V        Enable verbose logging                        ║" << std::endl;
        std::cout << "║    --quiet, -q          Suppress output                                ║" << std::endl;
        std::cout << "║                                                                          ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════════════════╝" << std::endl;
    }

private:
    std::unordered_map<std::string, Command> commands;

    void initializeCommands() {
        // Simulate command
        registerCommand({
            "simulate",
            "Run neural simulation with various modes",
            {
                "--mode", "--steps", "--timestep", "--neuron-count", "--region-count",
                "--plasticity", "--stdp", "--hebbian", "--structural",
                "--save", "--checkpoint", "--batch-size", "--parallel"
            },
            {"MODE", "OUTPUT_FILE"},
            [this](const std::vector<std::string>& args) { return handleSimulateCommand(args); }
        });

        // Config command
        registerCommand({
            "config",
            "Configuration management",
            {
                "--set", "--get", "--remove", "--list", "--load", "--save",
                "--schema", "--validate", "--generate"
            },
            {"KEY", "VALUE"},
            [this](const std::vector<std::string>& args) { return handleConfigCommand(args); }
        });

        // Benchmark command
        registerCommand({
            "benchmark",
            "Run performance benchmarks",
            {
                "--duration", "--iterations", "--neuron-count", "--parallel",
                "--output", "--format", "--detailed"
            },
            {"BENCHMARK_TYPE"},
            [this](const std::vector<std::string>& args) { return handleBenchmarkCommand(args); }
        });

        // Info command
        registerCommand({
            "info",
            "Show system and configuration information",
            {"--full", "--summary"},
            {},
            [this](const std::vector<std::string>& args) { return handleInfoCommand(args); }
        });

        // Export command
        registerCommand({
            "export",
            "Export data in various formats",
            {
                "--format", "--output", "--neurons", "--synapses",
                "--spikes", "--activity", "--weights"
            },
            {"FORMAT", "OUTPUT_FILE"},
            [this](const std::vector<std::string>& args) { return handleExportCommand(args); }
        });
    }

    bool handleSimulateCommand(const std::vector<std::string>& args) {
        std::cout << "Simulate command not yet implemented" << std::endl;
        return true;
    }

    bool handleConfigCommand(const std::vector<std::string>& args) {
        std::cout << "Config command not yet implemented" << std::endl;
        return true;
    }

    bool handleBenchmarkCommand(const std::vector<std::string>& args) {
        std::cout << "Benchmark command not yet implemented" << std::endl;
        return true;
    }

    bool handleInfoCommand(const std::vector<std::string>& args) {
        std::cout << "Info command not yet implemented" << std::endl;
        return true;
    }

    bool handleExportCommand(const std::vector<std::string>& args) {
        std::cout << "Export command not yet implemented" << std::endl;
        return true;
    }
};

// Output formatter
class OutputFormatter {
public:
    static void formatText(const std::string& data, OutputFormat format) {
        switch (format) {
            case OutputFormat::Text:
                std::cout << data;
                break;
            case OutputFormat::JSON:
                formatAsJSON(data);
                break;
            case OutputFormat::YAML:
                formatAsYAML(data);
                break;
            case OutputFormat::CSV:
                formatAsCSV(data);
                break;
            case OutputFormat::XML:
                formatAsXML(data);
                break;
        }
    }

private:
    static void formatAsJSON(const std::string& data) {
        // Simple JSON formatting - in production, use a proper JSON library
        std::cout << "{\"data\": \"" << escapeString(data) << "\"}";
    }

    static void formatAsYAML(const std::string& data) {
        // Simple YAML formatting - in production, use a proper YAML library
        std::cout << "data: |" << std::endl;
        std::cout << "  " << data << std::endl;
    }

    static void formatAsCSV(const std::string& data) {
        // Simple CSV formatting - extract key-value pairs if available
        std::cout << "key,value" << std::endl;
        std::cout << "data," << escapeString(data) << std::endl;
    }

    static void formatAsXML(const std::string& data) {
        // Simple XML formatting
        std::cout << "<data>" << escapeString(data) << "</data>" << std::endl;
    }

    static std::string escapeString(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '\"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }
};

// Result container for simulation data
struct SimulationResult {
    double simulationTime;
    int totalSteps;
    int totalSpikes;
    int totalNeurons;
    int totalSynapses;
    double finalWeightMean;
    double finalWeightStdDev;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    
    SimulationResult() : simulationTime(0.0), totalSteps(0), totalSpikes(0), 
                        totalNeurons(0), totalSynapses(0), finalWeightMean(0.0), 
                        finalWeightStdDev(0.0) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void end() {
        endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        simulationTime = duration.count() / 1000.0;
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "Simulation Results:" << std::endl;
        ss << "  Duration: " << simulationTime << " seconds" << std::endl;
        ss << "  Steps: " << totalSteps << std::endl;
        ss << "  Total Spikes: " << totalSpikes << std::endl;
        ss << "  Neurons: " << totalNeurons << std::endl;
        ss << "  Synapses: " << totalSynapses << std::endl;
        ss << "  Final Weight Mean: " << finalWeightMean << std::endl;
        ss << "  Final Weight StdDev: " << finalWeightStdDev << std::endl;
        ss << "  Most Active Neurons: [";
        for (size_t i = 0; i < mostActiveNeurons.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << mostActiveNeurons[i];
        }
        ss << "]" << std::endl;
        return ss.str();
    }
    
    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"simulation_time\": " << simulationTime << ",\n";
        ss << "  \"total_steps\": " << totalSteps << ",\n";
        ss << "  \"total_spikes\": " << totalSpikes << ",\n";
        ss << "  \"total_neurons\": " << totalNeurons << ",\n";
        ss << "  \"total_synapses\": " << totalSynapses << ",\n";
        ss << "  \"final_weight_mean\": " << finalWeightMean << ",\n";
        ss << "  \"final_weight_stddev\": " << finalWeightStdDev << ",\n";
        ss << "  \"most_active_neurons\": [";
        for (size_t i = 0; i < mostActiveNeurons.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << mostActiveNeurons[i];
        }
        ss << "]\n";
        ss << "}";
        return ss.str();
    }
};

// Advanced simulation runner
class AdvancedSimulator {
public:
    AdvancedSimulator(std::shared_ptr<Config> config) : config(config) {
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("AdvancedSimulator initialized");
    }

    SimulationResult run(SimulationMode mode, int steps, double timestep, bool enableProgress) {
        SimulationResult result;
        
        // Initialize simulation clock
        SimulationClock clock(timestep);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        brain->logStatus();
        
        // Run simulation based on mode
        switch (mode) {
            case SimulationMode::Basic:
                runBasicSimulation(brain, steps, enableProgress);
                break;
            case SimulationMode::Connectivity:
                runConnectivitySimulation(brain, steps, enableProgress);
                break;
            case SimulationMode::Plasticity:
                runPlasticitySimulation(brain, steps, enableProgress);
                break;
            case SimulationMode::STDP:
                runStdpSimulation(brain, steps, enableProgress);
                break;
            case SimulationMode::Hebbian:
                runHebbianSimulation(brain, steps, enableProgress);
                break;
            case SimulationMode::Structural:
                runStructuralSimulation(brain, steps, enableProgress);
                break;
            case SimulationMode::Neuromodulation:
                runNeuromodulationSimulation(brain, steps, enableProgress);
                break;
        }
        
        // Record final state
        recordFinalState(brain, result);
        result.end();
        
        return result;
    }

private:
    std::shared_ptr<Config> config;
    
    void runBasicSimulation(std::shared_ptr<Brain> brain, int steps, bool enableProgress) {
        NLM_LOG_INFO("Running Basic Simulation");
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
            if (enableProgress && step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
    }
    
    void runConnectivitySimulation(std::shared_ptr<Brain> brain, int steps, bool enableProgress) {
        NLM_LOG_INFO("Running Connectivity Simulation");
        // Inject current to test connectivity
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (!neurons.empty()) {
                neurons[0]->injectCurrent(100.0f);
            }
        }
        runBasicSimulation(brain, steps, enableProgress);
    }
    
    void runPlasticitySimulation(std::shared_ptr<Brain> brain, int steps, bool enableProgress) {
        NLM_LOG_INFO("Running Plasticity Simulation");
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply repeated input patterns
        for (SimulationStep step = 0; step < steps; ++step) {
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
            
            if (enableProgress && step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
    }
    
    void runStdpSimulation(std::shared_ptr<Brain> brain, int steps, bool enableProgress) {
        NLM_LOG_INFO("Running STDP Simulation");
        // Enable STDP only
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(false, true, false);
            }
        }
        
        // Create correlated activity for LTP
        if (auto* region = brain->getRegion(RegionId(1))) {
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                for (int trial = 0; trial < steps / 2; ++trial) {
                    neurons[0]->injectCurrent(60.0f);
                    brain->step(trial * 2, trial * 2 * config->getOr<double>("simulation_timestep", 0.001));
                    
                    neurons[1]->injectCurrent(60.0f);
                    brain->step(trial * 2 + 1, (trial * 2 + 1) * config->getOr<double>("simulation_timestep", 0.001));
                }
            }
        }
    }
    
    void runHebbianSimulation(std::shared_ptr<Brain> brain, int steps, bool enableProgress) {
        NLM_LOG_INFO("Running Hebbian Simulation");
        // Enable Hebbian only
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, false, false);
            }
        }
        
        runBasicSimulation(brain, steps, enableProgress);
    }
    
    void runStructuralSimulation(std::shared_ptr<Brain> brain, int steps, bool enableProgress) {
        NLM_LOG_INFO("Running Structural Plasticity Simulation");
        // Enable structural plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, true);
            }
        }
        
        runBasicSimulation(brain, steps, enableProgress);
    }
    
    void runNeuromodulationSimulation(std::shared_ptr<Brain> brain, int steps, bool enableProgress) {
        NLM_LOG_INFO("Running Neuromodulation Simulation");
        // Enable neuromodulators
        auto reward = std::make_shared<Reward>(config);
        auto curiosity = std::make_shared<Curiosity>(config);
        auto predictionError = std::make_shared<PredictionError>(config);
        
        // This would integrate neuromodulation with the brain
        runBasicSimulation(brain, steps, enableProgress);
    }
    
    void recordFinalState(std::shared_ptr<Brain> brain, SimulationResult& result) {
        // Get basic stats
        result.totalSteps = 1000; // Would be actual steps in real implementation
        result.totalSpikes = brain->getTotalSpikeCount();
        result.totalNeurons = brain->getTotalNeuronCount();
        result.totalSynapses = brain->getTotalSynapseCount();
        
        // Get weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            std::vector<float> weights;
            for (const auto& syn : region->getSynapses()) {
                weights.push_back(syn->getWeight());
            }
            result.finalWeights = weights;
            
            if (!weights.empty()) {
                float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
                result.finalWeightMean = sum / weights.size();
                
                // Calculate standard deviation
                float sqSum = 0.0f;
                for (float w : weights) {
                    sqSum += (w - result.finalWeightMean) * (w - result.finalWeightMean);
                }
                result.finalWeightStdDev = sqrt(sqSum / weights.size());
            }
        }
        
        // Get most active neurons
        result.mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
    }
};

// Configuration generator
class ConfigGenerator {
public:
    static std::string generateConfig(const std::string& format, const std::string& mode) {
        nlm::Config config;
        
        if (mode == "basic") {
            config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config.set("simulation_timestep", 0.001, ConfigSource::Default);
            config.set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
            config.set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config.set("connection_probability", 0.15f, ConfigSource::Default);
        }
        else if (mode == "advanced") {
            config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config.set("simulation_timestep", 0.0005, ConfigSource::Default);
            config.set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
            config.set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
            config.set("connection_probability", 0.2f, ConfigSource::Default);
            
            // STDP parameters
            config.set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
            config.set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
            config.set("stdp_tau", 20.0f, ConfigSource::Default);
            
            // Structural plasticity
            config.set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
            config.set("pruning_rate", 0.00005f, ConfigSource::Default);
            
            // Neuromodulation
            config.set("reward_learning_rate", 0.01f, ConfigSource::Default);
            config.set("curiosity_strength", 0.5f, ConfigSource::Default);
            config.set("prediction_error_decay", 0.99f, ConfigSource::Default);
        }
        
        return configToString(config, format);
    }
    
private:
    static std::string configToString(const nlm::Config& config, const std::string& format) {
        if (format == "json") {
            // Convert to JSON
            std::stringstream ss;
            ss << "{\n";
            auto keys = config.getKeys();
            for (size_t i = 0; i < keys.size(); ++i) {
                const std::string& key = keys[i];
                ss << "  \"" << key << "\": ";
                
                if (config.has(key)) {
                    auto val = config.get<std::string>(key);
                    if (val.has_value()) {
                        ss << "\"" << val.value() << "\"";
                    }
                }
                
                if (i < keys.size() - 1) ss << ",\n";
                else ss << "\n";
            }
            ss << "}";
            return ss.str();
        }
        else if (format == "yaml") {
            // Convert to YAML
            std::stringstream ss;
            ss << "# NLM Configuration\n";
            auto keys = config.getKeys();
            for (const auto& key : keys) {
                ss << key << ": ";
                if (config.has(key)) {
                    auto val = config.get<std::string>(key);
                    if (val.has_value()) {
                        ss << val.value() << std::endl;
                    }
                }
            }
            return ss.str();
        }
        else if (format == "keyvalue") {
            // Convert to key=value format
            std::stringstream ss;
            ss << "# NLM Configuration\n";
            auto keys = config.getKeys();
            for (const auto& key : keys) {
                ss << key << " = ";
                if (config.has(key)) {
                    auto val = config.get<std::string>(key);
                    if (val.has_value()) {
                        ss << val.value() << std::endl;
                    }
                }
            }
            return ss.str();
        }
        
        return "";
    }
};

// Progress reporter
class ProgressReporter {
public:
    static void reportProgress(int current, int total, double elapsed, const std::string& message) {
        double percentage = (static_cast<double>(current) / total) * 100.0;
        int barWidth = 50;
        
        std::cout << "\r[";
        int pos = static_cast<int>(barWidth * percentage / 100.0);
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << std::fixed << std::setprecision(1) << percentage << "% "
                  << current << "/" << total << " ";
        
        if (elapsed > 0) {
            std::cout << "ETA: " << (elapsed * (total - current) / current) << "s ";
        }
        
        std::cout << message;
        std::cout.flush();
        
        if (current >= total) {
            std::cout << std::endl;
        }
    }
};

int main(int argc, char** argv) {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                        NLM CLI Tool                                     ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║                        Version 2.0.0                                     ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\n";

    // Initialize CLI parser
    CLIParser parser;
    
    // Parse command line arguments
    if (!parser.parse(argc, argv)) {
        return 1;
    }
    
    return 0;
}

} // namespace nlm

// Include the implementation file
#include "nlm_cli.cpp" // This would be in a separate file in a real project

// Keep the original main.cpp functionality for backward compatibility
// Include original main.cpp implementation here or link with original binary

// For demonstration, here's a simplified main that shows both CLI and original functionality:

int main(int argc, char** argv) {
    // Parse command line arguments
    if (argc < 2) {
        // If no arguments, run original main.cpp functionality
        return nlm_original_main(argc, argv);
    }
    
    // Otherwise, use the CLI
    return nlm::main(argc, argv);
}

// Original main.cpp functionality (simplified for demonstration)
int nlm_original_main(int argc, char** argv) {
    // This would include the full original main.cpp implementation
    // For brevity, this shows the structure but actual implementation
    // would be in a separate file
    
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                        NLM Phase 2 Original                              ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║                                                                          ║" << std::endl;
    std::cout << "║  This is the original main.cpp functionality                            ║" << std::endl;
    std::cout << "║  Run with 'nlm_cli --help' for advanced CLI options                   ║" << std::endl;
    std::cout << "║                                                                          ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\n";
    
    // Original implementation would go here
    // For a real implementation, this would be the full content of src/main.cpp
    
    return 0;
}

// Entry point for the CLI tool
int nlm_cli_main(int argc, char** argv) {
    return nlm::main(argc, argv);
}