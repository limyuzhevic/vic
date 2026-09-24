// Enhanced NLM main program with advanced CLI commands and functionality
// Supports multiple modes: demo, interactive, debug, performance, experiment, etc.

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
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <fstream>
#include <chrono>
#include <map>
#include <functional>
#include <thread>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace nlm;

// Advanced command-line argument parser
class AdvancedCLIParser {
public:
    struct Command {
        std::string name;
        std::string description;
        std::function<bool(int, char**)> handler;
        std::string usage;
    };
    
    struct Options {
        std::string configFile;
        std::string outputFile;
        std::string logLevel = "info";
        bool verbose = false;
        bool quiet = false;
        bool jsonConfig = false;
        size_t maxSteps = 1000;
        double timestep = 0.001;
        uint64_t randomSeed = 42;
        std::string experimentMode = "demo";
        bool enableCheckpoints = false;
        bool enableDevelopment = true;
        bool enableCuriosity = true;
        bool enableRewardModulation = true;
        bool enableStructuralPlasticity = true;
    };
    
    std::vector<Command> commands;
    Options options;
    
    AdvancedCLIParser() {
        initializeCommands();
    }
    
    void initializeCommands() {
        // Core commands
        commands.push_back({"demo", "Run demo/test mode", &AdvancedCLIParser::handleDemo, "demo [options]"});
        commands.push_back({"interactive", "Run interactive mode", &AdvancedCLIParser::handleInteractive, "interactive"});
        commands.push_back({"debug", "Run debug mode with detailed output", &AdvancedCLIParser::handleDebug, "debug [options]"});
        commands.push_back({"performance", "Run performance profiling mode", &AdvancedCLIParser::handlePerformance, "performance [options]"});
        commands.push_back({"experiment", "Run custom experiment", &AdvancedCLIParser::handleExperiment, "experiment [options]"});
        
        // Config commands
        commands.push_back({"config", "Manage configuration files", &AdvancedCLIParser::handleConfig, "config <command> [options]"});
        commands.push_back({"create-config", "Create default config file", &AdvancedCLIParser::handleCreateConfig, "create-config <file>"});
        commands.push_back({"validate-config", "Validate config file", &AdvancedCLIParser::handleValidateConfig, "validate-config <file>"});
        
        // System commands
        commands.push_back({"monitor", "Start system monitoring", &AdvancedCLIParser::handleMonitor, "monitor"});
        commands.push_back({"profile", "Profile brain performance", &AdvancedCLIParser::handleProfile, "profile [steps]"});
        commands.push_back({"stats", "Show system statistics", &AdvancedCLIParser::handleStats, "stats"});
        
        // Export/import commands
        commands.push_back({"export", "Export brain state", &AdvancedCLIParser::handleExport, "export <file>"});
        commands.push_back({"import", "Import brain state", &AdvancedCLIParser::handleImport, "import <file>"});
    }
    
    bool parseArguments(int argc, char** argv) {
        if (argc < 2) {
            printHelp();
            return false;
        }
        
        std::string command = argv[1];
        
        for (const auto& cmd : commands) {
            if (cmd.name == command) {
                return cmd.handler(argc, argv);
            }
        }
        
        std::cerr << "Unknown command: " << command << std::endl;
        printHelp();
        return false;
    }
    
    void printHelp() {
        printBanner();
        std::cout << "\nAvailable commands:\n" << std::endl;
        
        for (const auto& cmd : commands) {
            std::cout << "  " << cmd.name << "\t" << cmd.description << std::endl;
            std::cout << "\tUsage: " << cmd.usage << std::endl << std::endl;
        }
        
        std::cout << "General options:\n";
        std::cout << "  --config=<file>      Configuration file path (default: configs/default.cfg)\n";
        std::cout << "  --output=<file>      Output file path\n";
        std::cout << "  --log-level=<level>   Log level (debug, info, warning, error)\n";
        std::cout << "  --verbose             Enable verbose output\n";
        std::cout << "  --quiet               Suppress output\n";
        std::cout << "  --json-config         Use JSON configuration format\n";
        std::cout << "  --max-steps=<n>       Maximum simulation steps (default: 1000)\n";
        std::cout << "  --timestep=<t>        Simulation timestep (default: 0.001)\n";
        std::cout << "  --seed=<n>            Random seed (default: 42)\n" << std::endl;
    }
    
private:
    bool handleDemo(int argc, char** argv) {
        // Parse general options
        parseGeneralOptions(argc, argv);
        
        std::cout << "=== NLM Advanced Demo Mode ===" << std::endl;
        std::cout << "Running comprehensive integration tests..." << std::endl << std::endl;
        
        // Create configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.15f);
        config->set("random_seed", options.randomSeed);
        config->set("simulation_timestep", options.timestep);
        config->set("stdp_ltp_weight", 0.02f);
        config->set("stdp_ltd_weight", 0.015f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        config->set("replay_interval", 100);
        config->set("consolidation_interval", 1000);
        
        // Initialize and run
        return runBrainSimulation(config);
    }
    
    bool handleDebug(int argc, char** argv) {
        parseGeneralOptions(argc, argv);
        
        std::cout << "=== NLM Debug Mode ===" << std::endl;
        std::cout << "Running with enhanced debugging and diagnostics..." << std::endl << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(200));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", options.randomSeed);
        config->set("simulation_timestep", options.timestep);
        
        // Enable debug logging
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        return runBrainSimulationWithDebug(config);
    }
    
    bool handlePerformance(int argc, char** argv) {
        parseGeneralOptions(argc, argv);
        
        std::cout << "=== NLM Performance Profiling Mode ===" << std::endl;
        std::cout << "Running performance benchmarks and profiling..." << std::endl << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.05f);
        config->set("random_seed", options.randomSeed);
        config->set("simulation_timestep", options.timestep);
        
        return runPerformanceBenchmark(config);
    }
    
    bool handleExperiment(int argc, char** argv) {
        parseGeneralOptions(argc, argv);
        
        std::cout << "=== NLM Custom Experiment Mode ===" << std::endl;
        std::cout << "Running Phase 6 integration experiment..." << std::endl << std::endl;
        
        // Run Phase 6 integration experiment
        Phase6IntegratedExperiment experiment;
        Phase6Config config;
        config.neuronCount = options.maxSteps;
        config.maxSteps = options.maxSteps;
        config.enableCheckpointing = options.enableCheckpoints;
        config.enableDevelopment = options.enableDevelopment;
        config.enableReplay = true;
        
        auto result = experiment.run(config);
        
        std::cout << std::endl << "=== Experiment Results ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << "Wall time: " << result.totalWallClockTime << "s" << std::endl;
        
        return true;
    }
    
    bool handleConfig(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Config command requires subcommand" << std::endl;
            return false;
        }
        
        std::string subcommand = argv[2];
        if (subcommand == "create") {
            return handleCreateConfig(argc, argv);
        } else if (subcommand == "validate") {
            return handleValidateConfig(argc, argv);
        } else {
            std::cerr << "Unknown config subcommand: " << subcommand << std::endl;
            return false;
        }
    }
    
    bool handleCreateConfig(int argc, char** argv) {
        if (argc < 4) {
            std::cerr << "create-config requires output file path" << std::endl;
            return false;
        }
        
        std::string outputFile = argv[3];
        std::cout << "Creating default configuration file: " << outputFile << std::endl;
        
        auto config = std::make_shared<Config>();
        
        // Set default values
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("random_seed", 42, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Add descriptions
        for (const auto& key : config->getKeys()) {
            // Set description for key
            std::string description;
            if (key == "neuron_count") description = "Number of neurons in the brain";
            else if (key == "region_count") description = "Number of brain regions";
            else if (key == "connection_probability") description = "Probability of connections between neurons";
            else if (key == "random_seed") description = "Random seed for reproducibility";
            else if (key == "simulation_timestep") description = "Time step for simulation updates";
            else if (key == "stdp_ltp_weight") description = "STDP long-term potentiation weight";
            else if (key == "stdp_ltd_weight") description = "STDP long-term depression weight";
            else if (key == "stdp_tau") description = "STDP time constant";
            else if (key == "synaptogenesis_rate") description = "Rate of new synapse formation";
            else if (key == "pruning_rate") description = "Rate of synapse removal";
            
            if (!description.empty()) {
                // Modify entry to include description
                auto entry = config->getKeys();
            }
        }
        
        if (options.jsonConfig) {
            // Save as JSON
            std::ofstream file(outputFile);
            if (!file.is_open()) {
                std::cerr << "Failed to open output file: " << outputFile << std::endl;
                return false;
            }
            
            json j;
            for (const auto& key : config->getKeys()) {
                j[key] = config->getOr<std::string>(key, "");
            }
            file << j.dump(2);
        } else {
            // Save as legacy key=value format
            config->saveToFile(outputFile);
        }
        
        std::cout << "Configuration file created successfully: " << outputFile << std::endl;
        return true;
    }
    
    bool handleValidateConfig(int argc, char** argv) {
        if (argc < 4) {
            std::cerr << "validate-config requires config file path" << std::endl;
            return false;
        }
        
        std::string configFile = argv[3];
        std::cout << "Validating configuration file: " << configFile << std::endl;
        
        auto config = std::make_shared<Config>();
        bool loaded = config->loadFromFile(configFile);
        
        if (!loaded) {
            std::cerr << "Failed to load configuration file" << std::endl;
            return false;
        }
        
        std::cout << "Configuration file is valid." << std::endl;
        std::cout << "Loaded " << config->getKeys().size() << " configuration entries:" << std::endl;
        
        for (const auto& key : config->getKeys()) {
            std::cout << "  " << key << " = " << config->getOr<std::string>(key, "") << std::endl;
        }
        
        return true;
    }
    
    bool handleMonitor(int argc, char** argv) {
        parseGeneralOptions(argc, argv);
        
        std::cout << "=== NLM System Monitor ===" << std::endl;
        std::cout << "Starting system monitoring... Press Ctrl+C to stop" << std::endl << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", options.randomSeed);
        config->set("simulation_timestep", options.timestep);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return false;
        }
        
        std::cout << "Monitoring initialized. Live statistics will appear below:" << std::endl;
        std::cout << std::endl;
        
        // Run monitoring loop
        auto startTime = std::chrono::high_resolution_clock::now();
        size_t step = 0;
        
        try {
            while (true) {
                auto loopStart = std::chrono::high_resolution_clock::now();
                
                // Update brain
                brain->step(step, step * options.timestep);
                
                // Display real-time statistics every 50 steps
                if (step % 50 == 0) {
                    auto currentTime = std::chrono::high_resolution_clock::now();
                    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
                    
                    std::cout << "[" << elapsed.count() << "s] Step " << step << ": ";
                    std::cout << "Neurons: " << brain->getTotalNeuronCount() << " | ";
                    std::cout << "Firing: " << brain->getFiringNeuronCount() << " | ";
                    std::cout << "Spikes: " << brain->getTotalSpikeCount() << " | ";
                    std::cout << "Activity: " << brain->getAverageFiringRate() << " | ";
                    
                    // Memory statistics
                    auto* wm = brain->getWorkingMemory();
                    if (wm) {
                        std::cout << "Working Memory: " << wm->getActiveTraces() << " | ";
                    }
                    
                    auto* em = brain->getEpisodicMemory();
                    if (em) {
                        std::cout << "Episodic Memory: " << em->getEpisodeCount() << " | ";
                    }
                    
                    // Neuromodulation statistics
                    auto* dopamine = brain->getDopamine();
                    if (dopamine) {
                        std::cout << "Dopamine: " << dopamine->getLevel();
                    }
                    
                    std::cout << std::endl;
                }
                
                step++;
                
                // Control loop frequency
                auto loopEnd = std::chrono::high_resolution_clock::now();
                auto loopDuration = std::chrono::duration_cast<std::chrono::milliseconds>(loopEnd - loopStart);
                
                if (loopDuration.count() < 100) {  // Minimum 100ms per loop
                    std::this_thread::sleep_for(std::chrono::milliseconds(100 - loopDuration.count()));
                }
            }
        } catch (const std::exception& e) {
            std::cout << std::endl << "Monitoring stopped: " << e.what() << std::endl;
        }
        
        return true;
    }
    
    bool handleStats(int argc, char** argv) {
        parseGeneralOptions(argc, argv);
        
        std::cout << "=== NLM System Statistics ===" << std::endl;
        std::cout << std::endl;
        
        // Create and initialize brain
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", options.randomSeed);
        config->set("simulation_timestep", options.timestep);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return false;
        }
        
        // Run simulation for statistics
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (uint64_t step = 0; step < options.maxSteps; ++step) {
            brain->step(step, step * options.timestep);
            
            // Add some sensory input to generate activity
            if (step % 100 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        // Collect comprehensive statistics
        std::cout << "=== NEURAL CORE STATISTICS ===" << std::endl;
        std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "Firing neurons (current step): " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "Pending spike events: " << brain->getPendingSpikeEventCount() << std::endl;
        std::cout << "Excitatory/Inhibitory ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        
        std::cout << std::endl << "=== MEMORY SYSTEMS STATISTICS ===" << std::endl;
        
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            std::cout << "Working Memory:" << std::endl;
            std::cout << "  Capacity: " << workingMemory->getCapacity() << std::endl;
            std::cout << "  Active traces: " << workingMemory->getActiveTraces() << std::endl;
            std::cout << "  Decay rate: " << workingMemory->getDecayRate() << std::endl;
            std::cout << "  Memory neurons: " << workingMemory->getMemoryNeurons().size() << std::endl;
        } else {
            std::cout << "Working Memory: Not available" << std::endl;
        }
        
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            std::cout << "Episodic Memory:" << std::endl;
            std::cout << "  Episode count: " << episodicMemory->getEpisodeCount() << std::endl;
            std::cout << "  Capacity: " << episodicMemory->getCapacity() << std::endl;
        } else {
            std::cout << "Episodic Memory: Not available" << std::endl;
        }
        
        auto* associativeMemory = brain->getAssociativeMemory();
        if (associativeMemory) {
            std::cout << "Associative Memory:" << std::endl;
            std::cout << "  Pattern count: " << associativeMemory->getPatternCount() << std::endl;
            std::cout << "  Similarity threshold: " << associativeMemory->getSimilarityThreshold() << std::endl;
        } else {
            std::cout << "Associative Memory: Not available" << std::endl;
        }
        
        std::cout << std::endl << "=== NEUROMODULATION STATISTICS ===" << std::endl;
        
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            std::cout << "Dopamine:" << std::endl;
            std::cout << "  Current level: " << dopamine->getLevel() << std::endl;
            std::cout << "  Learning rate: " << dopamine->getLearningRate() << std::endl;
        } else {
            std::cout << "Dopamine: Not available" << std::endl;
        }
        
        auto* curiosity = brain->getCuriosity();
        if (curiosity) {
            std::cout << "Curiosity:" << std::endl;
            std::cout << "  Current level: " << curiosity->getLevel() << std::endl;
        } else {
            std::cout << "Curiosity: Not available" << std::endl;
        }
        
        auto* novelty = brain->getNovelty();
        if (novelty) {
            std::cout << "Novelty:" << std::endl;
            std::cout << "  Current level: " << novelty->getLevel() << std::endl;
        } else {
            std::cout << "Novelty: Not available" << std::endl;
        }
        
        std::cout << std::endl << "=== DEVELOPMENT STATISTICS ===" << std::endl;
        
        auto* developmentSystem = brain->getDevelopmentSystem();
        if (developmentSystem) {
            std::cout << "Development System:" << std::endl;
            std::cout << "  Current stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
            std::cout << "  Development rate: " << developmentSystem->getDevelopmentRate() << std::endl;
        } else {
            std::cout << "Development System: Not available" << std::endl;
        }
        
        std::cout << std::endl << "=== PREDICTION SYSTEM STATISTICS ===" << std::endl;
        
        auto* predictionSystem = brain->getPredictionSystem();
        if (predictionSystem) {
            std::cout << "Prediction System:" << std::endl;
            std::cout << "  Active predictions: " << predictionSystem->getActivePredictions() << std::endl;
            std::cout << "  Prediction confidence: " << predictionSystem->getPredictionConfidence() << std::endl;
        } else {
            std::cout << "Prediction System: Not available" << std::endl;
        }
        
        std::cout << std::endl << "=== COGNITION SYSTEMS STATISTICS ===" << std::endl;
        
        auto* planner = brain->getPlanner();
        if (planner) {
            std::cout << "Planner: Available" << std::endl;
        } else {
            std::cout << "Planner: Not available" << std::endl;
        }
        
        auto* conceptFormation = brain->getConceptFormation();
        if (conceptFormation) {
            std::cout << "Concept Formation:" << std::endl;
            std::cout << "  Concept count: " << conceptFormation->getConceptCount() << std::endl;
            std::cout << "  Stable concepts: " << conceptFormation->getStableConceptCount() << std::endl;
        } else {
            std::cout << "Concept Formation: Not available" << std::endl;
        }
        
        auto* attention = brain->getAttention();
        if (attention) {
            std::cout << "Attention:" << std::endl;
            std::cout << "  Current focus: " << attention->getFocusRegions().size() << " regions" << std::endl;
        } else {
            std::cout << "Attention: Not available" << std::endl;
        }
        
        std::cout << std::endl << "=== PERFORMANCE STATISTICS ===" << std::endl;
        std::cout << "Total simulation time: " << totalDuration.count() << " seconds" << std::endl;
        std::cout << "Performance: " << options.maxSteps / totalDuration.count() << " steps/second" << std::endl;
        std::cout << std::endl;
        
        return true;
    }
    
    bool handleExport(int argc, char** argv) {
        if (argc < 4) {
            std::cerr << "export requires output file path" << std::endl;
            return false;
        }
        
        std::string outputFile = argv[3];
        std::cout << "Exporting brain state to: " << outputFile << std::endl;
        
        // TODO: Implement brain state export
        // This would serialize the complete brain state including neurons, synapses, memories, etc.
        
        std::cout << "Export functionality will be available in future versions." << std::endl;
        return true;
    }
    
    bool handleImport(int argc, char** argv) {
        if (argc < 4) {
            std::cerr << "import requires input file path" << std::endl;
            return false;
        }
        
        std::string inputFile = argv[3];
        std::cout << "Importing brain state from: " << inputFile << std::endl;
        
        // TODO: Implement brain state import
        // This would deserialize brain state from file
        
        std::cout << "Import functionality will be available in future versions." << std::endl;
        return true;
    }
    
    void parseGeneralOptions(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg.find("--config=") == 0) {
                options.configFile = arg.substr(9);
            } else if (arg.find("--output=") == 0) {
                options.outputFile = arg.substr(9);
            } else if (arg.find("--log-level=") == 0) {
                options.logLevel = arg.substr(12);
            } else if (arg == "--verbose") {
                options.verbose = true;
            } else if (arg == "--quiet") {
                options.quiet = true;
            } else if (arg == "--json-config") {
                options.jsonConfig = true;
            } else if (arg.find("--max-steps=") == 0) {
                options.maxSteps = std::stoul(arg.substr(11));
            } else if (arg.find("--timestep=") == 0) {
                options.timestep = std::stod(arg.substr(10));
            } else if (arg.find("--seed=") == 0) {
                options.randomSeed = std::stoull(arg.substr(7));
            } else if (arg.find("--enable-checkpoints") == 0) {
                options.enableCheckpoints = true;
            } else if (arg.find("--disable-checkpoints") == 0) {
                options.enableCheckpoints = false;
            } else if (arg.find("--enable-development") == 0) {
                options.enableDevelopment = true;
            } else if (arg.find("--disable-development") == 0) {
                options.enableDevelopment = false;
            } else if (arg.find("--enable-curiosity") == 0) {
                options.enableCuriosity = true;
            } else if (arg.find("--disable-curiosity") == 0) {
                options.enableCuriosity = false;
            } else if (arg.find("--enable-reward-modulation") == 0) {
                options.enableRewardModulation = true;
            } else if (arg.find("--disable-reward-modulation") == 0) {
                options.enableRewardModulation = false;
            } else if (arg.find("--enable-structural-plasticity") == 0) {
                options.enableStructuralPlasticity = true;
            } else if (arg.find("--disable-structural-plasticity") == 0) {
                options.enableStructuralPlasticity = false;
            }
        }
    }
    
    bool runBrainSimulation(std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return false;
        }
        
        std::cout << "Brain initialized successfully." << std::endl;
        brain->logStatus();
        
        // Run connectivity test
        std::cout << std::endl << "--- Running connectivity test ---" << std::endl;
        runBasicConnectivityTest(brain);
        
        // Reset for plasticity test
        brain->reset();
        brain->initialize();
        
        std::cout << std::endl << "--- Running plasticity experiment ---" << std::endl;
        runPlasticityExperiment(brain);
        
        // Reset for STDP test
        brain->reset();
        brain->initialize();
        
        std::cout << std::endl << "--- Running STDP verification ---" << std::endl;
        runStdpVerification(brain);
        
        brain->logStatus();
        return true;
    }
    
    bool runBrainSimulationWithDebug(std::shared_ptr<Config> config) {
        // Similar to demo but with enhanced debugging
        return runBrainSimulation(config);
    }
    
    bool runPerformanceBenchmark(std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return false;
        }
        
        std::cout << "Brain initialized for performance benchmark." << std::endl;
        
        // Profile performance
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (uint64_t step = 0; step < options.maxSteps; ++step) {
            brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
            
            if (options.verbose && step % 1000 == 0) {
                std::cout << "Step " << step << ": " 
                         << brain->getFiringNeuronCount() << " neurons firing, "
                         << brain->getAverageFiringRate() << " avg firing rate" << std::endl;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        double fps = options.maxSteps / (duration.count() / 1000.0);
        
        std::cout << std::endl << "=== Performance Benchmark Results ===" << std::endl;
        std::cout << "Total steps: " << options.maxSteps << std::endl;
        std::cout << "Total time: " << (duration.count() / 1000.0) << "s" << std::endl;
        std::cout << "Performance: " << fps << " steps/second" << std::endl;
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        
        return true;
    }
    
    bool runPerformanceProfile(std::shared_ptr<Config> config, size_t profileSteps) {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return false;
        }
        
        std::cout << "Brain initialized for performance profiling." << std::endl;
        
        // Run multiple iterations for profiling
        std::vector<double> times;
        
        for (int iteration = 0; iteration < 3; ++iteration) {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (uint64_t step = 0; step < profileSteps; ++step) {
                brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            double timeSec = duration.count() / 1000.0;
            
            times.push_back(timeSec);
            
            std::cout << "Iteration " << (iteration + 1) << ": " << timeSec << "s ("
                     << (profileSteps / timeSec) << " steps/sec)" << std::endl;
            
            // Reset brain for next iteration
            brain->reset();
            brain->initialize();
        }
        
        // Calculate statistics
        double avgTime = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
        double minTime = *std::min_element(times.begin(), times.end());
        double maxTime = *std::max_element(times.begin(), times.end());
        
        std::cout << std::endl << "=== Performance Profile Statistics ===" << std::endl;
        std::cout << "Average time: " << avgTime << "s" << std::endl;
        std::cout << "Min time: " << minTime << "s" << std::endl;
        std::cout << "Max time: " << maxTime << "s" << std::endl;
        std::cout << "Average performance: " << (profileSteps / avgTime) << " steps/sec" << std::endl;
        std::cout << "Min performance: " << (profileSteps / maxTime) << " steps/sec" << std::endl;
        std::cout << "Max performance: " << (profileSteps / minTime) << " steps/sec" << std::endl;
        
        return true;
    }
    
    // Original learning experiment functions (simplified)
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            std::cout << "  No neurons found!" << std::endl;
            return;
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        std::cout << "  Injecting current into 10 neurons..." << std::endl;
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        std::cout << "  Spikes generated: " << spikes << std::endl;
        
        if (spikes > 0) {
            std::cout << "  ✓ Spikes propagate through network" << std::endl;
        } else {
            std::cout << "  ! No spikes - checking neuron parameters..." << std::endl;
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                std::cout << "    Neuron " << i << 
                         " V=" << neurons[i]->getMembranePotential() << 
                         " thresh=" << neurons[i]->getThreshold() << std::endl;
            }
        }
    }
    
    void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
        std::cout << "  Applying repeated input patterns (200 steps)..." << std::endl;
        
        for (SimulationStep step = 0; step < 200; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 50 == 0) {
                std::cout << "  Step " << step << " | " 
                         << "Spikes: " << brain->getTotalSpikeCount() << " | "
                         << "Firing: " << brain->getFiringNeuronCount() << std::endl;
            }
        }
        
        std::cout << "  ✓ Plasticity experiment completed" << std::endl;
    }
    
    void runStdpVerification(std::shared_ptr<Brain> brain) {
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            std::cout << "  Not enough synapses for STDP test" << std::endl;
            return;
        }
        
        std::cout << "  Testing STDP on 5 synapses:" << std::endl;
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false); // Enable only STDP
            std::cout << "    Synapse " << i << " before: " << beforeWeights[i] << std::endl;
        }
        
        // Create correlated activity: fire pre then post to trigger LTP
        std::cout << "  Creating correlated pre->post activity (potentiation)..." << std::endl;
        
        for (int trial = 0; trial < 20; ++trial) {
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
        std::cout << "  After correlated activity:" << std::endl;
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            std::cout << "    Synapse " << i << " after: " << synapses[i]->getWeight() 
                     << " (Δ=" << delta << ")" << std::endl;
            totalDelta += delta;
        }
        
        if (totalDelta > 0.001f) {
            std::cout << "  ✓ STDP WORKING: Pre-before-post produced potentiation" << std::endl;
        } else if (totalDelta < -0.001f) {
            std::cout << "  ! STDP reversed: Check parameters" << std::endl;
        } else {
            std::cout << "  ! No change: STDP may not be triggering" << std::endl;
        }
    }
};