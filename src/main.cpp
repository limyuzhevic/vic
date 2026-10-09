// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// Advanced command-line interface with subcommands for comprehensive
// control over NLM brain simulation and analysis.

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
#include <algorithm>
#include <map>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <cstdlib>

using namespace nlm;

/**
 * @brief Print the application banner
 * 
 * Displays the NLM project information and phase 6 integration status.
 */
void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                 ║
    ║                                                               ║
    ║     Advanced Artificial Brain Simulation System                ║
    ║     with Integrated Memory, Learning, and Development          ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

/**
 * @brief Display usage information
 * 
 * Shows the available commands and their usage patterns.
 * 
 * @param programName Name of the program (argv[0])
 */
void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " <command> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  run        Run simulation with specified configuration" << std::endl;
    std::cout << "  explore    Interactive exploration mode" << std::endl;
    std::cout << "  test       Run comprehensive test suite" << std::endl;
    std::cout << "  benchmark  Run performance benchmarks" << std::endl;
    std::cout << "  config     Configuration management" << std::endl;
    std::cout << "  experiment Run specific experiments" << std::endl;
    std::cout << "  monitor    Monitor running brain" << std::endl;
    std::cout << "  help       Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " run --config my_config.cfg" << std::endl;
    std::cout << "  " << programName << " test --quick" << std::endl;
    std::cout << "  " << programName << " benchmark --duration 60s" << std::endl;
}

/**
 * @brief Display help for a specific command
 * 
 * Provides detailed information about the specified command and its options.
 * 
 * @param command The command to display help for
 */
void printHelp(const std::string& command) {
    if (command == "run") {
        std::cout << "Run NLM brain simulation." << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --config <file>           Configuration file (default: configs/default.cfg)" << std::endl;
        std::cout << "  --steps <n>               Number of simulation steps" << std::endl;
        std::cout << "  --timestep <t>            Simulation timestep in seconds" << std::endl;
        std::cout << "  --neurons <n>             Number of neurons" << std::endl;
        std::cout << "  --regions <n>             Number of brain regions" << std::endl;
        std::cout << "  --connections <p>         Connection probability (0.0-1.0)" << std::endl;
        std::cout << "  --seed <s>                Random seed for reproducible results" << std::endl;
        std::cout << "  --output <file>           Save brain state to file" << std::endl;
        std::cout << "  --checkpoint <file>       Load brain state from file" << std::endl;
        std::cout << "  --enable-plasticity       Enable synaptic plasticity" << std::endl;
        std::cout << "  --enable-neuromodulation   Enable neuromodulation" << std::endl;
        std::cout << "  --enable-development       Enable brain development" << std::endl;
        std::cout << "  --log-level <level>       Logging level (DEBUG, INFO, WARNING, ERROR)" << std::endl;
        std::cout << "  --log-file <file>         Log output to file" << std::endl;
        std::cout << "  --batch                   Run without interactive output" << std::endl;
    } else if (command == "test") {
        std::cout << "Run NLM test suite." << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --quick                   Run quick unit tests only" << std::endl;
        std::cout << "  --comprehensive           Run full test suite" << std::endl;
        std::cout << "  --verbose                 Enable verbose output" << std::endl;
        std::cout << "  --list                    List available tests" << std::endl;
        std::cout << "  --output <file>           Save test results to file" << std::endl;
    } else if (command == "benchmark") {
        std::cout << "Run performance benchmarks." << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --duration <t>            Benchmark duration in seconds" << std::endl;
        std::cout << "  --neurons <n>             Number of neurons to benchmark" << std::endl;
        std::cout << "  --steps <n>               Number of simulation steps" << std::endl;
        std::cout << "  --runs <n>                Number of benchmark runs" << std::endl;
        std::cout << "  --output <file>           Save benchmark results to file" << std::endl;
    } else if (command == "config") {
        std::cout << "Configuration management." << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --list                    List all configuration parameters" << std::endl;
        std::cout << "  --get <key>               Get value for configuration parameter" << std::endl;
        std::cout << "  --set <key> <value>        Set configuration parameter value" << std::endl;
        std::cout << "  --validate                Validate configuration" << std::endl;
        std::cout << "  --save <file>             Save configuration to file" << std::endl;
        std::cout << "  --load <file>             Load configuration from file" << std::endl;
    } else if (command == "experiment") {
        std::cout << "Run specific experiments." << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --phase6                   Run Phase 6 integration experiment" << std::endl;
        std::cout << "  --phase4                  Run Phase 4 demonstration" << std::endl;
        std::cout << "  --phase3                  Run Phase 3 demonstration" << std::endl;
        std::cout << "  --results <file>          Save experiment results to file" << std::endl;
    } else if (command == "monitor") {
        std::cout << "Monitor running brain simulation." << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --duration <t>            Monitoring duration in seconds" << std::endl;
        std::cout << "  --update-rate <r>         Update rate in Hz" << std::endl;
        std::cout << "  --history <n>             Number of history entries to keep" << std::endl;
    }
}

/**
 * @brief Display configuration in a readable format
 * 
 * Shows the current configuration with proper formatting and descriptions.
 * 
 * @param config The configuration to display
 */
void showConfiguration(const std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("Current Configuration:");
    NLM_LOG_INFO("====================");
    
    std::vector<std::string> keys = config->getKeys();
    std::sort(keys.begin(), keys.end());
    
    for (const auto& key : keys) {
        NLM_LOG_INFO("  " + std::left + std::setw(30) + key);
        
        auto value = config->get<std::string>(key);
        if (value) {
            NLM_LOG_INFO(*value);
        } else {
            auto intValue = config->get<int64_t>(key);
            if (intValue) {
                NLM_LOG_INFO(std::to_string(*intValue));
            } else {
                auto floatValue = config->get<float>(key);
                if (floatValue) {
                    NLM_LOG_INFO(std::to_string(*floatValue));
                } else {
                    auto doubleValue = config->get<double>(key);
                    if (doubleValue) {
                        NLM_LOG_INFO(std::to_string(*doubleValue));
                    } else {
                        auto boolValue = config->get<bool>(key);
                        if (boolValue) {
                            NLM_LOG_INFO(*boolValue ? "true" : "false");
                        } else {
                            NLM_LOG_INFO("[unrecognized type]");
                        }
                    }
                }
            }
        }
    }
}

/**
 * @brief Run simulation with error handling
 * 
 * Executes the NLM brain simulation with proper error handling and logging.
 * 
 * @param config The configuration to use
 * @param batchMode Whether to run in batch mode (no interactive output)
 */
void runSimulation(const std::shared_ptr<Config>& config, bool batchMode) {
    try {
        NLM_LOG_INFO("Initializing NLM Brain...");
        
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return;
        }
        
        brain->logStatus();
        
        size_t steps = config->getOr<size_t>("max_simulation_steps", 10000);
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        NLM_LOG_INFO("Running " + std::to_string(steps) + " simulation steps (" + 
                    std::to_string(timestep) + "s per step)...");
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
            
            if (!batchMode && step % 1000 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + "/" + std::to_string(steps) + 
                           " | Firing neurons: " + std::to_string(brain->getFiringNeuronCount()) +
                           " | Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
            }
        }
        
        NLM_LOG_INFO("Simulation complete!");
        brain->logStatus();
        
        // Save if configured
        std::string outputFile = config->getOr<std::string>("output_file", "");
        if (!outputFile.empty()) {
            NLM_LOG_INFO("Saving brain state to: " + outputFile);
            if (brain->save(outputFile)) {
                NLM_LOG_INFO("Save successful!");
            } else {
                NLM_LOG_ERROR("Failed to save brain state!");
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_CRITICAL("Simulation failed with exception: " + std::string(e.what()));
        throw;
    } catch (...) {
        NLM_LOG_CRITICAL("Simulation failed with unknown exception!");
        throw;
    }
}

/**
 * @brief Run quick test suite with error handling
 * 
 * Executes a quick test suite to verify basic functionality.
 * 
 * @param batchMode Whether to run in batch mode
 */
void runQuickTest() {
    try {
        NLM_LOG_INFO("Running quick test suite...");
        
        // Test configuration system
        auto config = std::make_shared<Config>();
        config->set("test.int", 42);
        config->set("test.float", 3.14f);
        config->set("test.string", "hello");
        config->set("test.bool", true);
        
        auto intValue = config->get<int64_t>("test.int");
        auto floatValue = config->get<float>("test.float");
        auto stringValue = config->get<std::string>("test.string");
        auto boolValue = config->get<bool>("test.bool");
        
        if (intValue == 42 && floatValue == 3.14f && stringValue == "hello" && boolValue == true) {
            NLM_LOG_INFO("✓ Configuration system test passed");
        } else {
            NLM_LOG_ERROR("✗ Configuration system test failed");
        }
        
        // Test type system
        NeuronId id1(1);
        NeuronId id2(1);
        NeuronId id3(2);
        
        if (id1 == id2 && id1 != id3) {
            NLM_LOG_INFO("✓ Neuron ID system test passed");
        } else {
            NLM_LOG_ERROR("✗ Neuron ID system test failed");
        }
        
        // Test enum system
        if (static_cast<int>(NeuronType::Sensory) == 3) {
            NLM_LOG_INFO("✓ Neuron type enum test passed");
        } else {
            NLM_LOG_ERROR("✗ Neuron type enum test failed");
        }
        
        NLM_LOG_INFO("Quick test complete!");
        
    } catch (const std::exception& e) {
        NLM_LOG_CRITICAL("Quick test suite failed with exception: " + std::string(e.what()));
        throw;
    } catch (...) {
        NLM_LOG_CRITICAL("Quick test suite failed with unknown exception!");
        throw;
    }
}

/**
 * @brief Run Phase 6 integration experiment with error handling
 * 
 * Executes the Phase 6 integration experiment to verify system integration.
 * 
 * @param outputFile Output file for results
 */
void runPhase6Experiment(const std::string& outputFile) {
    try {
        NLM_LOG_INFO("Running Phase 6 Integration Experiment...");
        
        Phase6IntegratedExperiment experiment;
        
        // Run quick integration verification
        NLM_LOG_INFO("--- Integration Verification ---");
        bool integrationOK = experiment.verifyIntegration();
        
        if (!integrationOK) {
            NLM_LOG_ERROR("ERROR: Integration verification failed!");
            return;
        }
        
        NLM_LOG_INFO("Integration verification passed!");
        
        // Test individual systems
        NLM_LOG_INFO("--- Memory Integration Test ---");
        experiment.testMemoryIntegration();
        NLM_LOG_INFO("--- Neuromodulation Integration Test ---");
        experiment.testNeuromodulationIntegration();
        NLM_LOG_INFO("--- Checkpoint Test ---");
        experiment.testCheckpointing();
        NLM_LOG_INFO("--- Replay Test ---");
        experiment.testReplay();
        
        // Run full experiment
        NLM_LOG_INFO("--- Full Integration Experiment ---");
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        auto result = experiment.run(config);
        
        NLM_LOG_INFO("=== FINAL RESULTS ===");
        NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
        NLM_LOG_INFO("Avg firing rate: " + std::to_string(result.avgFiringRate));
        NLM_LOG_INFO("Episodes stored: " + std::to_string(result.memoryEpisodesStored));
        NLM_LOG_INFO("Dopamine level: " + std::to_string(result.dopamineLevel));
        
        NLM_LOG_INFO("=== INTEGRATION STATUS ===");
        NLM_LOG_INFO("Working Memory: " + (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("Episodic Memory: " + (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("Neuromodulation: " + (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("Prediction: " + (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("Development: " + (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("Checkpointing: " + (result.checkpointingWorks ? "WORKING" : "NOT WORKING"));
        
        NLM_LOG_INFO("Wall clock time: " + std::to_string(result.totalWallClockTime) + "s");
        
        if (!outputFile.empty()) {
            NLM_LOG_INFO("Saving results to: " + outputFile);
            std::ofstream outFile(outputFile);
            if (outFile.is_open()) {
                outFile << "=== Phase 6 Integration Experiment Results ===" << std::endl;
                outFile << "Total reward: " << result.totalReward << std::endl;
                outFile << "Avg firing rate: " << result.avgFiringRate << std::endl;
                outFile << "Memory working memory integrated: " << (result.memoryWorkingMemoryIntegrated ? "true" : "false") << std::endl;
                outFile << "Memory episodic memory integrated: " << (result.memoryEpisodicMemoryIntegrated ? "true" : "false") << std::endl;
                outFile << "Neuromodulation integrated: " << (result.neuromodulationIntegrated ? "true" : "false") << std::endl;
                outFile << "Prediction integrated: " << (result.predictionIntegrated ? "true" : "false") << std::endl;
                outFile << "Development integrated: " << (result.developmentIntegrated ? "true" : "false") << std::endl;
                outFile << "Checkpointing works: " << (result.checkpointingWorks ? "true" : "false") << std::endl;
                outFile << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
                outFile.close();
                NLM_LOG_INFO("Results saved successfully!");
            } else {
                NLM_LOG_ERROR("ERROR: Failed to save results to file!");
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_CRITICAL("Phase 6 experiment failed with exception: " + std::string(e.what()));
        throw;
    } catch (...) {
        NLM_LOG_CRITICAL("Phase 6 experiment failed with unknown exception!");
        throw;
    }
}

/**
 * @brief Main entry point with comprehensive error handling
 * 
 * Parses command line arguments and executes the appropriate command
 * with proper error handling and logging throughout.
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return Exit code (0 for success, non-zero for failure)
 */
int main(int argc, char** argv) {
    try {
        // Initialize logging system
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        consoleLogger->setUseColors(true);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        printBanner();
        
        if (argc < 2) {
            printUsage(argv[0]);
            return 1;
        }
        
        std::string command = argv[1];
        
        if (command == "help") {
            if (argc > 2) {
                printHelp(argv[2]);
            } else {
                printUsage(argv[0]);
            }
            return 0;
        }
        
        if (command == "run") {
            // Parse run command arguments with error handling
            std::string configFile = "configs/default.cfg";
            bool batchMode = false;
            bool enablePlasticity = true;
            bool enableNeuromodulation = true;
            bool enableDevelopment = true;
            std::string outputFile;
            std::string checkpointFile;
            size_t steps = 10000;
            double timestep = 0.001;
            size_t neurons = 1000;
            size_t regions = 1;
            float connectionProbability = 0.1f;
            uint64_t seed = 42;
            std::string logLevel = "INFO";
            std::string logFile;
            
            for (int i = 2; i < argc; ++i) {
                std::string arg = argv[i];
                try {
                    if (arg == "--config" && i + 1 < argc) {
                        configFile = argv[++i];
                    } else if (arg == "--steps" && i + 1 < argc) {
                        steps = std::stoull(argv[++i]);
                        if (steps == 0) {
                            NLM_LOG_ERROR("Steps must be greater than 0");
                            return 1;
                        }
                    } else if (arg == "--timestep" && i + 1 < argc) {
                        timestep = std::stod(argv[++i]);
                        if (timestep <= 0.0) {
                            NLM_LOG_ERROR("Timestep must be greater than 0");
                            return 1;
                        }
                    } else if (arg == "--neurons" && i + 1 < argc) {
                        neurons = std::stoull(argv[++i]);
                        if (neurons == 0) {
                            NLM_LOG_ERROR("Number of neurons must be greater than 0");
                            return 1;
                        }
                    } else if (arg == "--regions" && i + 1 < argc) {
                        regions = std::stoull(argv[++i]);
                        if (regions == 0) {
                            NLM_LOG_ERROR("Number of regions must be greater than 0");
                            return 1;
                        }
                    } else if (arg == "--connections" && i + 1 < argc) {
                        connectionProbability = std::stof(argv[++i]);
                        if (connectionProbability < 0.0 || connectionProbability > 1.0) {
                            NLM_LOG_ERROR("Connection probability must be between 0.0 and 1.0");
                            return 1;
                        }
                    } else if (arg == "--seed" && i + 1 < argc) {
                        seed = std::stoull(argv[++i]);
                    } else if (arg == "--output" && i + 1 < argc) {
                        outputFile = argv[++i];
                    } else if (arg == "--checkpoint" && i + 1 < argc) {
                        checkpointFile = argv[++i];
                    } else if (arg == "--enable-plasticity") {
                        enablePlasticity = true;
                    } else if (arg == "--enable-neuromodulation") {
                        enableNeuromodulation = true;
                    } else if (arg == "--enable-development") {
                        enableDevelopment = true;
                    } else if (arg == "--log-level" && i + 1 < argc) {
                        logLevel = argv[++i];
                    } else if (arg == "--log-file" && i + 1 < argc) {
                        logFile = argv[++i];
                    } else if (arg == "--batch") {
                        batchMode = true;
                    } else {
                        NLM_LOG_ERROR("Unknown option: " + arg);
                        return 1;
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error parsing command line arguments: " + std::string(e.what()));
                    return 1;
                }
            }
            
            // Create and configure config
            auto config = std::make_shared<Config>();
            
            // Load from file if exists
            if (configFile != "configs/default.cfg") {
                try {
                    if (config->loadFromFile(configFile)) {
                        NLM_LOG_INFO("Loaded configuration from: " + configFile);
                    } else {
                        NLM_LOG_INFO("Using default configuration.");
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Failed to load configuration file: " + std::string(e.what()));
                    return 1;
                }
            }
            
            // Set from command line
            try {
                config->set("random_seed", seed);
                config->set("simulation_timestep", timestep);
                config->set("neuron_count", static_cast<int64_t>(neurons));
                config->set("region_count", static_cast<int64_t>(regions));
                config->set("connection_probability", connectionProbability);
                config->set("max_simulation_steps", static_cast<size_t>(steps));
                
                if (!checkpointFile.empty()) {
                    config->set("checkpoint_file", checkpointFile);
                }
                if (!outputFile.empty()) {
                    config->set("output_file", outputFile);
                }
                
                // Set plasticity and neuromodulation flags
                config->set("plasticity_enabled", enablePlasticity);
                config->set("neuromodulation_enabled", enableNeuromodulation);
                config->set("development_enabled", enableDevelopment);
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to set configuration values: " + std::string(e.what()));
                return 1;
            }
            
            // Configure logging
            if (logFile != "") {
                try {
                    // Configure file logging
                    for (auto& logger : Logger::getGlobal().getLoggers()) {
                        auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger);
                        if (consoleLogger) {
                            consoleLogger->setOutputFile(logFile);
                        }
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Failed to configure log file: " + std::string(e.what()));
                }
            }
            
            // Run simulation
            runSimulation(config, batchMode);
            
        } else if (command == "test") {
            bool quick = false;
            bool verbose = false;
            bool listTests = false;
            std::string outputFile;
            
            for (int i = 2; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg == "--quick") {
                    quick = true;
                } else if (arg == "--verbose") {
                    verbose = true;
                } else if (arg == "--list") {
                    listTests = true;
                } else if (arg == "--output" && i + 1 < argc) {
                    outputFile = argv[++i];
                }
            }
            
            if (listTests) {
                NLM_LOG_INFO("Available tests:");
                NLM_LOG_INFO("  1. Configuration system test");
                NLM_LOG_INFO("  2. Neuron ID system test");
                NLM_LOG_INFO("  3. Enum system test");
                NLM_LOG_INFO("  4. Basic neural computation test");
                NLM_LOG_INFO("  5. Memory system integration test");
            } else if (quick) {
                runQuickTest();
            } else {
                NLM_LOG_INFO("Running comprehensive test suite (requires full build with tests)...");
                // Would run full test suite here
                NLM_LOG_INFO("Note: Full test suite would run here.");
            }
            
            if (!outputFile.empty()) {
                NLM_LOG_INFO("Saving test results to: " + outputFile);
                // Save test results
            }
            
        } else if (command == "benchmark") {
            double duration = 60.0; // seconds
            size_t neurons = 500;
            size_t steps = 1000;
            size_t runs = 5;
            std::string outputFile;
            
            for (int i = 2; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg == "--duration" && i + 1 < argc) {
                    duration = std::stod(argv[++i]);
                } else if (arg == "--neurons" && i + 1 < argc) {
                    neurons = std::stoull(argv[++i]);
                } else if (arg == "--steps" && i + 1 < argc) {
                    steps = std::stoull(argv[++i]);
                } else if (arg == "--runs" && i + 1 < argc) {
                    runs = std::stoull(argv[++i]);
                } else if (arg == "--output" && i + 1 < argc) {
                    outputFile = argv[++i];
                }
            }
            
            NLM_LOG_INFO("Running benchmark: " + std::to_string(neurons) + " neurons, " + 
                        std::to_string(steps) + " steps, " + std::to_string(runs) + " runs");
            NLM_LOG_INFO("Duration: " + std::to_string(duration) + " seconds");
            NLM_LOG_INFO("Note: Full benchmark implementation would run here.");
            
            if (!outputFile.empty()) {
                NLM_LOG_INFO("Saving benchmark results to: " + outputFile);
            }
            
        } else if (command == "config") {
            std::string action = "";
            if (argc > 2) {
                action = argv[2];
            }
            
            if (action == "--list") {
                // List config parameters
                auto config = std::make_shared<Config>();
                showConfiguration(config);
            } else if (action == "--get" && argc > 3) {
                // Get config value
                auto config = std::make_shared<Config>();
                std::string key = argv[3];
                if (config->has(key)) {
                    auto value = config->get<std::string>(key);
                    if (value) {
                        NLM_LOG_INFO(key + " = " + *value);
                    } else {
                        NLM_LOG_INFO(key + " = [unrecognized type]");
                    }
                } else {
                    NLM_LOG_ERROR("Key not found: " + key);
                }
            } else if (action == "--set" && argc > 4) {
                // Set config value
                auto config = std::make_shared<Config>();
                std::string key = argv[3];
                std::string value = argv[4];
                
                try {
                    // Try to parse as int
                    try {
                        int intValue = std::stoi(value);
                        config->set(key, intValue);
                        NLM_LOG_INFO("Set " + key + " = " + std::to_string(intValue));
                        return 0;
                    } catch (...) {}
                    
                    // Try to parse as float
                    try {
                        float floatValue = std::stof(value);
                        config->set(key, floatValue);
                        NLM_LOG_INFO("Set " + key + " = " + std::to_string(floatValue));
                        return 0;
                    } catch (...) {}
                    
                    // Try to parse as bool
                    if (value == "true") {
                        config->set(key, true);
                        NLM_LOG_INFO("Set " + key + " = true");
                        return 0;
                    } else if (value == "false") {
                        config->set(key, false);
                        NLM_LOG_INFO("Set " + key + " = false");
                        return 0;
                    }
                    
                    // Treat as string
                    config->set(key, value);
                    NLM_LOG_INFO("Set " + key + " = \"" + value + "\"");
                    
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("ERROR: ") + e.what());
                    return 1;
                }
            } else {
                NLM_LOG_INFO("Configuration management commands:");
                NLM_LOG_INFO("  --list          List all configuration parameters");
                NLM_LOG_INFO("  --get <key>     Get value for configuration parameter");
                NLM_LOG_INFO("  --set <key> <value> Set configuration parameter value");
                NLM_LOG_INFO("  --validate      Validate configuration");
                NLM_LOG_INFO("  --save <file>   Save configuration to file");
                NLM_LOG_INFO("  --load <file>   Load configuration from file");
            }
            
        } else if (command == "experiment") {
            std::string experimentType = "phase6";
            std::string outputFile;
            
            for (int i = 2; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg == "--phase6") {
                    experimentType = "phase6";
                } else if (arg == "--phase4") {
                    experimentType = "phase4";
                } else if (arg == "--phase3") {
                    experimentType = "phase3";
                } else if (arg == "--results" && i + 1 < argc) {
                    outputFile = argv[++i];
                }
            }
            
            if (experimentType == "phase6") {
                runPhase6Experiment(outputFile);
            } else {
                NLM_LOG_ERROR("Experiment type \"" + experimentType + "\" not yet implemented.");
            }
            
        } else if (command == "explore") {
            NLM_LOG_INFO("Interactive exploration mode (requires GUI)...");
            NLM_LOG_INFO("Note: Interactive mode requires additional libraries for visualization.");
            NLM_LOG_INFO("Features would include:");
            NLM_LOG_INFO("  - Real-time brain visualization");
            NLM_LOG_INFO("  - Interactive parameter adjustment");
            NLM_LOG_INFO("  - Live monitoring of neural activity");
            NLM_LOG_INFO("  - Experiment recording and playback");
            
        } else if (command == "monitor") {
            double duration = 30.0; // seconds
            std::string updateRate = "1s";
            size_t history = 1000;
            
            for (int i = 2; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg == "--duration" && i + 1 < argc) {
                    duration = std::stod(argv[++i]);
                } else if (arg == "--update-rate" && i + 1 < argc) {
                    updateRate = argv[++i];
                } else if (arg == "--history" && i + 1 < argc) {
                    history = std::stoull(argv[++i]);
                }
            }
            
            NLM_LOG_INFO("Monitoring mode (placeholder)...");
            NLM_LOG_INFO("Duration: " + std::to_string(duration) + " seconds");
            NLM_LOG_INFO("Update rate: " + updateRate);
            NLM_LOG_INFO("History: " + std::to_string(history) + " entries");
            
        } else {
            NLM_LOG_ERROR("ERROR: Unknown command: " + command);
            printUsage(argv[0]);
            return 1;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_CRITICAL("Fatal error: " + std::string(e.what()));
        return 1;
    } catch (...) {
        NLM_LOG_CRITICAL("Unknown fatal error occurred!");
        return 1;
    }
    
    return 0;
}