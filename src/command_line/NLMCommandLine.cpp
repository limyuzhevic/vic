// NLM Command Line Interface
// Enhanced command-line tool for running NLM simulations with improved argument parsing

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <map>
#include <set>

namespace nlm {

// Command-line argument parser
class CommandLineParser {
public:
    struct Option {
        std::string longName;
        char shortName;
        std::string description;
        std::string defaultValue;
        bool required;
        std::string type;
    };
    
    struct Argument {
        std::string key;
        std::string value;
        std::string source;
    };
    
    CommandLineParser() {
        options = {
            {"--help", '?', "Show help information", "", false, "flag"},
            {"--version", 'v', "Show version information", "", false, "flag"},
            {"--demo", 'd', "Run demonstration mode", "", false, "flag"},
            {"--benchmark", 'b', "Run performance benchmark", "", false, "flag"},
            {"--save", 's', "Save configuration to file", "", false, "filename"},
            {"--load", 'l', "Load configuration from file", "", false, "filename"},
            {"--config", 'c', "Load configuration from file", "", false, "filename"},
            {"--episodes", 'e', "Number of episodes (default: 5)", "5", false, "integer"},
            {"--steps", 't', "Steps per episode (default: 200)", "200", false, "integer"},
            {"--seed", 'S', "Random seed (default: 42)", "42", false, "integer"},
            {"--neurons", 'n', "Number of neurons (default: 500)", "500", false, "integer"},
            {"--threads", 'T', "Number of threads for parallel processing", "1", false, "integer"},
            {"--quiet", 'q', "Suppress output", "", false, "flag"},
            {"--verbose", 'V', "Enable verbose output", "", false, "flag"},
            {"--log-level", 'L', "Set log level (DEBUG, INFO, WARNING, ERROR)", "INFO", false, "level"},
            {"--log-file", 'F', "Write logs to file", "", false, "filename"},
            {"--checkpoint", 'k', "Save checkpoint to file", "", false, "filename"},
            {"--resume", 'r', "Resume from checkpoint file", "", false, "filename"},
            {"--validate", 'V', "Validate configuration and exit", "", false, "flag"}
        };
        
        this->parseOptions();
    }
    
    void parseOptions() {
        std::vector<std::string> args = getCommandLineArgs();
        
        for (size_t i = 1; i < args.size(); ++i) {
            std::string arg = args[i];
            
            // Check for long options
            if (arg.find("--") == 0) {
                processLongOption(arg, args, i);
            }
            // Check for short options
            else if (arg.find("-") == 0 && arg.length() > 1) {
                processShortOption(arg, args, i);
            }
            // Check for key=value format
            else if (arg.find("=") != std::string::npos) {
                processKeyValueOption(arg);
            }
        }
    }
    
    void processLongOption(const std::string& arg, const std::vector<std::string>& args, size_t& i) {
        std::string option = arg.substr(2);
        size_t valuePos = option.find("=");
        
        if (valuePos != std::string::npos) {
            std::string key = option.substr(0, valuePos);
            std::string value = option.substr(valuePos + 1);
            parsedArgs.push_back({key, value, "command-line"});
        } else {
            // Check if it's a flag (no argument expected)
            if (option == "help" || option == "version" || option == "demo" || 
                option == "benchmark" || option == "quiet" || option == "verbose" || 
                option == "validate") {
                parsedArgs.push_back({option, "1", "command-line"});
            } else {
                // Option requires an argument
                if (i + 1 < args.size()) {
                    std::string key = option;
                    std::string value = args[++i];
                    parsedArgs.push_back({key, value, "command-line"});
                }
            }
        }
    }
    
    void processShortOption(const std::string& arg, const std::vector<std::string>& args, size_t& i) {
        std::string option = arg.substr(1);
        size_t valuePos = option.find("=");
        
        if (valuePos != std::string::npos) {
            std::string key = option.substr(0, valuePos);
            std::string value = option.substr(valuePos + 1);
            parsedArgs.push_back({key, value, "command-line"});
        } else if (option.length() == 1) {
            // Single character option - check if it has a value
            auto optIt = std::find_if(options.begin(), options.end(),
                [option](const Option& o) { return o.shortName == option[0]; });n
            if (optIt != options.end() && optIt->type != "flag") {
                // Option expects a value
                if (i + 1 < args.size()) {
                    std::string key = optIt->longName.substr(2); // Remove "--"
                    std::string value = args[++i];
                    parsedArgs.push_back({key, value, "command-line"});
                }
            } else {
                // Flag option
                parsedArgs.push_back({option, "1", "command-line"});
            }
        }
    }
    
    void processKeyValueOption(const std::string& arg) {
        size_t equalsPos = arg.find("=");
        if (equalsPos != std::string::npos) {
            std::string key = arg.substr(0, equalsPos);
            std::string value = arg.substr(equalsPos + 1);
            parsedArgs.push_back({key, value, "command-line"});
        }
    }
    
    std::vector<std::string> getCommandLineArgs() {
        // Get the number of arguments
        int argc = 0;
        char** argv = nullptr;
        
        // Use standard C++ approach
        extern char** environ;
        extern int __argc;
        extern char** __argv;
        
        if (__argc > 0) {
            argc = __argc;
            argv = __argv;
        }
        
        std::vector<std::string> args;
        for (int i = 0; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        return args;
    }
    
    void printHelp() const {
        std::cout << "NLM - Neural Learning Machine Command Line Interface\n\n";n        std::cout << "Usage:\n";
        std::cout << "  nlm-cli [options] [key=value pairs...]\n\n";
        std::cout << "Options:\n";
        
        for (const Option& opt : options) {
            std::cout << "  " << opt.shortName << "\t--" << opt.longName.substr(2);
            std::cout << std::setw(20) << std::left << "(" << opt.description << ")";
            std::cout << "Default: " << opt.defaultValue << "\n";
        }
        
        std::cout << "\nExamples:\n";
        std::cout << "  nlm-cli --demo -e 10 -t 500\n";
        std::cout << "  nlm-cli --benchmark -n 1000 -s 1000\n";
        std::cout << "  nlm-cli --load config.json --save output.json\n";
        std::cout << "  nlm-cli --seed 123 --log-level INFO\n\n";
    }
    
    void printVersion() const {
        std::cout << "NLM Version 0.1.0\n";
        std::cout << "Phase 6 Final Integration\n";
        std::cout << "Neural Learning Machine for experimental artificial brain research\n";
    }
    
    bool hasOption(const std::string& option) const {
        return std::any_of(parsedArgs.begin(), parsedArgs.end(),
            [option](const Argument& arg) { return arg.key == option; });
    }
    
    std::string getOptionValue(const std::string& option, const std::string& defaultValue = "") const {
        auto it = std::find_if(parsedArgs.begin(), parsedArgs.end(),
            [option](const Argument& arg) { return arg.key == option; });n
        if (it != parsedArgs.end()) {
            return it->value;
        }
        return defaultValue;
    }
    
    std::string getOptionSource(const std::string& option) const {
        auto it = std::find_if(parsedArgs.begin(), parsedArgs.end(),
            [option](const Argument& arg) { return arg.key == option; });n
        if (it != parsedArgs.end()) {
            return it->source;
        }
        return "default";
    }
    
    // Validate option types
    bool validateOption(const std::string& option, const std::string& value) const {
        // Get option definition
        auto optIt = std::find_if(options.begin(), options.end(),
            [option](const Option& o) { 
                return (o.longName.substr(2) == option) || 
                       (o.shortName != '?' && o.shortName == option[0]); });n
        
        if (optIt == options.end()) {
            return true; // Unknown option, assume valid
        }
        
        // Basic validation based on type
        if (optIt->type == "integer") {
            try {
                std::stoi(value);
                return true;
            } catch (...) {
                return false;
            }
        } else if (optIt->type == "float") {
            try {
                std::stod(value);
                return true;
            } catch (...) {
                return false;
            }
        } else if (optIt->type == "level") {
            std::string level = toLower(value);
            return (level == "debug" || level == "info" || level == "warning" || level == "error");
        }
        
        return true;
    }
    
    std::vector<Argument> getAllArguments() const {
        return parsedArgs;
    }
    
private:
    std::vector<Option> options;
    std::vector<Argument> parsedArgs;
};

// Standalone NLM command-line tool
class NLMCommandLine {
public:
    static int run(int argc, char** argv) {
        CommandLineParser parser;
        
        // Check for help/version flags first
        if (parser.hasOption("help")) {
            parser.printHelp();
            return 0;
        }
        
        if (parser.hasOption("version")) {
            parser.printVersion();
            return 0;
        }
        
        if (parser.hasOption("validate")) {
            return validateConfiguration(parser);
        }
        
        // Setup logging based on command line arguments
        LogLevel logLevel = LogLevel::Info;
        std::string logFile = "";
        
        std::string logLevelStr = parser.getOptionValue("log-level");
        if (logLevelStr == "DEBUG") logLevel = LogLevel::Debug;
        else if (logLevelStr == "WARNING") logLevel = LogLevel::Warning;
        else if (logLevelStr == "ERROR") logLevel = LogLevel::Error;
        
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
        logger->addLogger(consoleLogger);
        
        if (!logFile.empty()) {
            auto fileLogger = std::make_shared<FileLogger>(logFile);
            logger->addLogger(fileLogger);
        }
        
        Logger::setGlobal(logger);
        
        // Check for benchmark mode
        if (parser.hasOption("benchmark")) {
            return runBenchmark(parser);
        }
        
        // Check for demo mode
        if (parser.hasOption("demo")) {
            return runDemo(parser);
        }
        
        // Default: run Phase 6 demo
        return runPhase6Demo(parser);
    }
    
private:
    static int validateConfiguration(CommandLineParser& parser) {
        std::cout << "Validating configuration...\n";
        
        auto config = std::make_shared<Config>();
        
        // Load from file if specified
        std::string configFile = parser.getOptionValue("config");
        if (!configFile.empty()) {
            if (!config->loadFromFile(configFile)) {
                std::cerr << "ERROR: Failed to load configuration from " << configFile << std::endl;
                return 1;
            }
        }
        
        // Apply command line arguments
        std::vector<std::string> args;
        for (const auto& arg : parser.getAllArguments()) {
            args.push_back("--" + arg.key + "=" + arg.value);
        }
        
        config->loadFromArgs(args.size(), &args[0]);
        
        // Validate configuration
        std::cout << "Configuration loaded successfully.\n";
        std::cout << config->summary() << std::endl;
        
        return 0;
    }
    
    static int runBenchmark(CommandLineParser& parser) {
        std::cout << "===========================================\n";
        std::cout << "NLM BENCHMARK MODE\n";
        std::cout << "===========================================\n\n";
        
        size_t neurons = std::stoi(parser.getOptionValue("neurons", "500"));
        size_t steps = std::stoi(parser.getOptionValue("steps", "1000"));
        size_t threads = std::stoi(parser.getOptionValue("threads", "1"));
        
        std::cout << "Running benchmark with " << neurons << " neurons, " 
                  << steps << " steps, " << threads << " threads...\n\n";
        
        // Create configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run benchmark
        BenchmarkResult result = runBenchmark(*brain, steps);
        
        // Print results
        printBenchmarkResults(result);
        
        return 0;
    }
    
    static int runDemo(CommandLineParser& parser) {
        std::cout << "===========================================\n";
        std::cout << "NLM DEMO MODE\n";
        std::cout << "===========================================\n\n";
        
        int episodes = std::stoi(parser.getOptionValue("episodes", "5"));
        int stepsPerEpisode = std::stoi(parser.getOptionValue("steps", "200"));
        uint64_t seed = std::stoull(parser.getOptionValue("seed", "42"));
        
        std::cout << "Running " << episodes << " episodes with " << stepsPerEpisode << " steps each...\n\n";
        
        // Create configuration
        auto config = std::make_shared<Config>();
        configureEnvironment(config);
        configureBrain(config);
        config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::Default);
        
        // Create world
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(seed);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Configure agent brain
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        agentBrain.enableRewardModulation(true);
        
        // Run demo
        runDemoEpisodes(agentBrain, world, episodes, stepsPerEpisode);
        
        return 0;
    }
    
    static int runPhase6Demo(CommandLineParser& parser) {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 6 DEMO - FINAL INTEGRATION\n";
        std::cout << "===========================================\n\n";
        
        // Create configuration
        auto config = std::make_shared<Config>();
        configureEnvironment(config);
        configureBrain(config);
        
        // Create Phase 6 integrated experiment
        Phase6IntegratedExperiment experiment(config);
        
        // Run the integrated experiment
        experiment.run();
        
        return 0;
    }
    
    static void configureEnvironment(std::shared_ptr<Config> config) {
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("environment_name", "GridWorld", ConfigSource::Default);
        config->set("environment_width", 20, ConfigSource::Default);
        config->set("environment_height", 20, ConfigSource::Default);
    }
    
    static void configureBrain(std::shared_ptr<Config> config) {
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    }
    
    static void runDemoEpisodes(AgentBrain& agentBrain, SimpleWorld& world, 
                               int episodes, int stepsPerEpisode) {
        for (int episode = 0; episode < episodes; ++episode) {
            std::cout << "=== Episode " << episode + 1 << "/" << episodes << " ===\n";
            
            world.reset();
            agentBrain.reset();
            
            for (int step = 0; step < stepsPerEpisode; ++step) {
                // Get observation from world
                SensoryPercept percept = world.getSensoryPercept();
                
                // Process sensory input
                agentBrain.processSensoryInput(percept);
                
                // Run brain step
                agentBrain.getBrain()->step(step, step * 0.001);
                
                // Decode motor command
                MotorCommand cmd = agentBrain.decodeMotorCommand();
                
                // Apply action to world
                world.applyAction(cmd, step * 0.001);
                
                // Compute reward
                float reward = world.computeReward(agentBrain.getAgentBody());
                float predictedReward = agentBrain.getPredictionError();
                
                // Apply reward modulation
                agentBrain.applyRewardModulation(reward, predictedReward);
                
                // Update development
                agentBrain.updateDevelopment(0.001f);
                
                // Log progress every 50 steps
                if (step % 50 == 0) {
                    std::cout << "  Step " << step << "/" << stepsPerEpisode 
                              << " | Reward: " << reward 
                              << " | Curiosity: " << agentBrain.getCuriosityLevel() 
                              << " | Novelty: " << agentBrain.getNoveltyLevel() << "\n";
                }
            }
            
            std::cout << "Episode " << episode + 1 << " completed.\n\n";
        }
    }
    
    static BenchmarkResult runBenchmark(Brain& brain, size_t steps) {
        BenchmarkResult result;
        result.neurons = brain.getTotalNeuronCount();
        result.synapses = brain.getTotalSynapseCount();
        result.steps = steps;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            brain.step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        result.totalTime = duration.count() / 1000.0;
        result.stepsPerSecond = result.steps / result.totalTime;
        result.spikesPerStep = static_cast<double>(brain.getTotalSpikeCount()) / result.steps;
        result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
        
        return result;
    }
    
    static void printBenchmarkResults(const BenchmarkResult& result) {
        std::cout << "Benchmark Results:\n";
        std::cout << "===================\n";
        std::cout << "Neurons: " << result.neurons << "\n";
        std::cout << "Synapses: " << result.synapses << "\n";
        std::cout << "Steps: " << result.steps << "\n";
        std::cout << "Total time: " << std::fixed << std::setprecision(3) 
                  << result.totalTime << " seconds\n";
        std::cout << "Steps per second: " << std::fixed << std::setprecision(1) 
                  << result.stepsPerSecond << "\n";
        std::cout << "Spikes per step: " << std::fixed << std::setprecision(3) 
                  << result.spikesPerStep << "\n";
        std::cout << "Synapses per neuron: " << std::fixed << std::setprecision(3) 
                  << result.synapsesPerNeuron << "\n";
    }
};

// Main entry point for command-line tool
int main(int argc, char** argv) {
    try {
        return NLMCommandLine::run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception occurred" << std::endl;
        return 1;
    }
}