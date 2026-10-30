// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// This phase integrates all existing neural systems into a coherent artificial brain:
// - Memory systems (working memory, episodic memory) are connected to neural processing
// - Neuromodulation (dopamine, curiosity) affects plasticity and neural dynamics
// - Development affects plasticity rates and system maturation
// - Prediction system is integrated with learning and action selection
// - Replay and consolidation systems reinforce memory
// - Checkpoint save/load enables persistence and recovery
// - All systems work together as a unified brain loop

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// Print application banner
void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                 ║
    ║                                                               ║
    ║     Integrating all neural systems into a coherent artificial brain. ║
    ║     This phase implements:                                    ║
    ║     - Memory systems (working + episodic)                    ║
    ║     - Neuromodulation (dopamine, curiosity)                  ║
    ║     - Development and plasticity                              ║
    ║     - Prediction system integration                           ║
    ║     - Replay and consolidation                                ║
    ║     - Checkpoint save/load                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Print help/usage information
void printHelp() {
    std::cout << "=== NLM Phase 6: Neural Learning Machine ===" << std::endl;
    std::cout << std::endl;
    std::cout << "USAGE: ./nlm [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "BASIC OPTIONS:" << std::endl;
    std::cout << "  --help              Show this help message" << std::endl;
    std::cout << "  --version           Show version information" << std::endl;
    std::cout << "  --config FILE       Load configuration from FILE (default: configs/default.cfg)" << std::endl;
    std::cout << std::endl;
    std::cout << "INTEGRATION MODES:" << std::endl;
    std::cout << "  --demo             Run Phase 6 integration demo (default)" << std::endl;
    std::cout << "  --benchmark STEPS  Run neural computation benchmark (steps)" << std::endl;
    std::cout << "  --quick            Run quick integration test" << std::endl;
    std::cout << "  --verify           Only verify system integration (no execution)" << std::endl;
    std::cout << std::endl;
    std::cout << "PERFORMANCE OPTIONS:" << std::endl;
    std::cout << "  --profiler         Enable performance profiling" << std::endl;
    std::cout << "  --log-level LEVEL  Set logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)" << std::endl;
    std::cout << "  --no-colors        Disable colored output" << std::endl;
    std::cout << std::endl;
    std::cout << "ADVANCED OPTIONS:" << std::endl;
    std::cout << "  --checkpoint PATH  Checkpoint path for save/load testing" << std::endl;
    std::cout << "  --neuron-count N   Number of neurons (default: 500)" << std::endl;
    std::cout << "  --max-steps STEPS  Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --seed SEED        Random seed for reproducibility" << std::endl;
    std::cout << "  --quiet            Reduce output verbosity" << std::endl;
    std::cout << std::endl;
    std::cout << "SCRIPTING OPTIONS:" << std::endl;
    std::cout << "  --csv-output FILE  Output metrics to CSV file" << std::endl;
    std::cout << "  --json-output FILE  Output results to JSON file" << std::endl;
    std::cout << std::endl;
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  ./nlm                                    Run integration demo" << std::endl;
    std::cout << "  ./nlm --benchmark 1000                  Run benchmark with 1000 steps" << std::endl;
    std::cout << "  ./nlm --quick --profiler              Run quick test with profiling" << std::endl;
    std::cout << "  ./nlm --verify --log-level DEBUG      Just verify integration" << std::endl;
}

// Version information
void printVersion() {
    std::cout << "NLM Phase 6 Integration Framework" << std::endl;
    std::cout << "Version: 6.0.0 (Final Integration)" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Platform: Linux x86_64" << std::endl;
}

// Parse command-line arguments
struct CommandLineArgs {
    std::string configFile = "configs/default.cfg";
    std::string checkpointPath = "./checkpoint_test.bin";
    std::string csvOutput = "";
    std::string jsonOutput = "";
    std::string logLevel = "INFO";
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    uint64_t seed = 42;
    bool enableProfiler = false;
    bool noColors = false;
    bool quiet = false;
    std::string mode = "demo"; // demo, benchmark, quick, verify
    size_t benchmarkSteps = 0;
};

CommandLineArgs parseCommandLine(int argc, char** argv) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printHelp();
            std::exit(0);
        } else if (arg == "--version") {
            printVersion();
            std::exit(0);
        } else if (arg == "--quiet") {
            args.quiet = true;
        } else if (arg == "--no-colors") {
            args.noColors = true;
        } else if (arg == "--profiler") {
            args.enableProfiler = true;
        } else if (arg == "--demo") {
            args.mode = "demo";
        } else if (arg == "--quick") {
            args.mode = "quick";
        } else if (arg == "--verify") {
            args.mode = "verify";
        } else if (arg == "--benchmark" && i + 1 < argc) {
            args.mode = "benchmark";
            args.benchmarkSteps = std::stoul(argv[++i]);
        } else if (arg == "--config" && i + 1 < argc) {
            args.configFile = argv[++i];
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            args.checkpointPath = argv[++i];
        } else if (arg == "--csv-output" && i + 1 < argc) {
            args.csvOutput = argv[++i];
        } else if (arg == "--json-output" && i + 1 < argc) {
            args.jsonOutput = argv[++i];
        } else if (arg == "--log-level" && i + 1 < argc) {
            args.logLevel = argv[++i];
        } else if (arg == "--neuron-count" && i + 1 < argc) {
            args.neuronCount = std::stoul(argv[++i]);
        } else if (arg == "--max-steps" && i + 1 < argc) {
            args.maxSteps = std::stoul(argv[++i]);
        } else if (arg == "--seed" && i + 1 < argc) {
            args.seed = std::stoul(argv[++i]);
        } else {
            std::cerr << "WARNING: Unknown argument: " << arg << std::endl;
        }
    }
    
    return args;
}

// Set up logging based on command-line arguments
void setupLogging(const CommandLineArgs& args) {
    // Set global logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    
    // Set log level
    std::transform(args.logLevel.begin(), args.logLevel.end(), args.logLevel.begin(), ::toupper);
    if (args.logLevel == "DEBUG") {
        consoleLogger->setLevel(LogLevel::Debug);
    } else if (args.logLevel == "WARNING") {
        consoleLogger->setLevel(LogLevel::Warning);
    } else if (args.logLevel == "ERROR") {
        consoleLogger->setLevel(LogLevel::Error);
    } else if (args.logLevel == "CRITICAL") {
        consoleLogger->setLevel(LogLevel::Critical);
    } else {
        consoleLogger->setLevel(LogLevel::Info); // default
    }
    
    consoleLogger->setUseColors(!args.noColors);
    
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

// Enhanced error message printing
void printError(const std::string& context, const std::string& error) {
    if (!args.quiet) {
        std::cerr << "[ERROR] " << context << ": " << error << std::endl;
    }
}

// CSV output function
void writeCSVOutput(const std::string& filename, const std::vector<std::vector<std::string>>& data) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        printError("CSV output", "Failed to open file for writing: " + filename);
        return;
    }
    
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < data[i].size(); ++j) {
            file << data[i][j];
            if (j < data[i].size() - 1) {
                file << ",";
            }
        }
        if (i < data.size() - 1) {
            file << "\n";
        }
    }
    
    file.close();
    if (!args.quiet) {
        std::cout << "[INFO] CSV data written to: " << filename << std::endl;
    }
}

// JSON output function
void writeJSONOutput(const std::string& filename, const json& data) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        printError("JSON output", "Failed to open file for writing: " + filename);
        return;
    }
    
    file << data.dump(2); // Pretty print with 2-space indentation
    file.close();
    if (!args.quiet) {
        std::cout << "[INFO] JSON data written to: " << filename << std::endl;
    }
}

// Visualization function for brain state
void visualizeBrainState(std::shared_ptr<Brain> brain, const std::string& outputPrefix) {
    if (!brain) {
        printError("Visualization", "Brain is null");
        return;
    }
    
    NLM_LOG_INFO("=== Brain State Visualization ===");
    
    // Create CSV data
    std::vector<std::vector<std::string>> csvData;
    
    // Header
    csvData.push_back({"Step", "Total Neurons", "Total Synapses", "Firing Neurons", 
                      "Average Firing Rate", "Total Spikes", "Working Memory Traces",
                      "Episodic Memory Episodes", "Developmental Stage"});
    
    // Data row
    csvData.push_back({
        std::to_string(0),
        std::to_string(brain->getTotalNeuronCount()),
        std::to_string(brain->getTotalSynapseCount()),
        std::to_string(brain->getFiringNeuronCount()),
        std::to_string(brain->getAverageFiringRate()),
        std::to_string(brain->getTotalSpikeCount()),
        brain->getWorkingMemory() ? std::to_string(brain->getWorkingMemory()->getActiveTraces()) : "0",
        brain->getEpisodicMemory() ? std::to_string(brain->getEpisodicMemory()->getEpisodeCount()) : "0",
        "Unknown" // Would need developmental system API
    });
    
    // Write CSV
    if (!args.csvOutput.empty()) {
        writeCSVOutput(args.csvOutput, csvData);
    } else {
        // Write to default filename
        writeCSVOutput(outputPrefix + "_brain_state.csv", csvData);
    }
    
    // Create JSON data
    json j;
    j["brain_state"]["timestamp"] = std::time(nullptr);
    j["brain_state"]["step"] = 0;
    j["brain_state"]["total_neurons"] = brain->getTotalNeuronCount();
    j["brain_state"]["total_synapses"] = brain->getTotalSynapseCount();
    j["brain_state"]["firing_neurons"] = brain->getFiringNeuronCount();
    j["brain_state"]["average_firing_rate"] = brain->getAverageFiringRate();
    j["brain_state"]["total_spikes"] = brain->getTotalSpikeCount();
    j["brain_state"]["working_memory_traces"] = brain->getWorkingMemory() ? 
        brain->getWorkingMemory()->getActiveTraces() : 0;
    j["brain_state"]["episodic_memory_episodes"] = brain->getEpisodicMemory() ? 
        brain->getEpisodicMemory()->getEpisodeCount() : 0;
    j["brain_state"]["system_info"]["version"] = "6.0.0";
    j["brain_state"]["system_info"]["phase"] = "Phase 6: Final Integration";
    
    // Write JSON
    if (!args.jsonOutput.empty()) {
        writeJSONOutput(args.jsonOutput, j);
    } else {
        // Write to default filename
        writeJSONOutput(outputPrefix + "_brain_state.json", j);
    }
    
    NLM_LOG_INFO("Brain state visualization completed");
}
    printBanner();
    
    std::cout << "Initializing NLM Phase 6 Final Integration..." << std::endl;
    
    // Parse command line arguments
    CommandLineArgs args = parseCommandLine(argc, argv);
    
    // Setup logging
    setupLogging(args);
    
    NLM_LOG_INFO("=== NLM Phase 6: Final Integration ===");
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Load configuration from file
    if (config->loadFromFile(args.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + args.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line arguments
    config->set("random_seed", static_cast<int64_t>(args.seed), ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(args.neuronCount), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::CommandLine);
    config->set("connection_probability", 0.1f, ConfigSource::CommandLine);
    
    // Validate configuration and show warnings/errors
    auto configResult = validateConfig(*config);
    
    if (!configResult.valid) {
        printError("Configuration validation", configResult.errorMessage);
        if (!args.quiet) {
            std::cout << "Falling back to minimal configuration..." << std::endl;
        }
        
        // Reset to minimal safe configuration
        config->clear();
        config->set("neuron_count", 100, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("random_seed", args.seed, ConfigSource::Default);
    }
    
    // Show validation warnings
    for (const auto& warning : configResult.warnings) {
        printWarning("Configuration validation", warning);
    }
    
    // Test configuration loading
    if (!config->has("neuron_count") || !config->has("simulation_timestep")) {
        printError("Configuration error", "Configuration is incomplete after validation");
        return 1;
    }
    
    // Log configuration status
    NLM_LOG_INFO("");
    if (configResult.warnings.empty()) {
        NLM_LOG_INFO("✓ Configuration validated successfully");
    } else {
        NLM_LOG_INFO("⚠ Configuration has warnings (see above)");
    }
    
    // Create Phase 6 experiment
    Phase6IntegratedExperiment experiment;
    
    // Run based on mode
    if (args.mode == "verify") {
        NLM_LOG_INFO("=== System Verification Mode ===");
        bool integrationOK = experiment.verifyIntegration();
        
        if (integrationOK) {
            NLM_LOG_INFO("✓ SYSTEM INTEGRATION VERIFIED SUCCESSFULLY");
            return 0;
        } else {
            NLM_LOG_ERROR("✗ SYSTEM INTEGRATION VERIFICATION FAILED");
            return 1;
        }
    } else if (args.mode == "benchmark") {
        NLM_LOG_INFO("=== Benchmark Mode ===");
        
        // Create brain and initialize
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            printError("Brain initialization", "Failed to initialize brain");
            return 1;
        }
        
        brain->logStatus();
        
        // Run benchmark
        runBenchmark(brain, args.benchmarkSteps);
        
        // Run integration test
        runIntegrationTest(brain);
        
        return 0;
    } else if (args.mode == "quick") {
        NLM_LOG_INFO("=== Quick Integration Test ===");
        
        // Create brain and initialize with smaller settings
        auto configQuick = std::make_shared<Config>();
        configQuick->set("neuron_count", 100);
        configQuick->set("region_count", 1);
        
        auto brain = std::make_shared<Brain>(configQuick);
        if (!brain->initialize()) {
            printError("Brain initialization", "Failed to initialize brain");
            return 1;
        }
        
        // Run integration test
        runIntegrationTest(brain);
        
        // Run some simulation steps
        runBenchmark(brain, 100);
        
        return 0;
    } else if (args.mode == "demo") {
        NLM_LOG_INFO("=== Phase 6 Integration Demo ===");
        
        // Configure for demo
        Phase6Config demoConfig;
        demoConfig.neuronCount = args.neuronCount;
        demoConfig.maxSteps = args.maxSteps;
        demoConfig.enableCheckpointing = true;
        demoConfig.enableReplay = true;
        demoConfig.enableDevelopment = true;
        demoConfig.checkpointPath = args.checkpointPath;
        
        // Run full integration demo
        auto result = experiment.run(demoConfig);
        
        // Display results
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== FINAL RESULTS ===");
        NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
        NLM_LOG_INFO("Avg firing rate: " + std::to_string(result.avgFiringRate));
        NLM_LOG_INFO("Episodes stored: " + std::to_string(result.memoryEpisodesStored));
        NLM_LOG_INFO("Dopamine level: " + std::to_string(result.dopamineLevel));
        NLM_LOG_INFO("Wall clock time: " + std::to_string(result.totalWallClockTime) + "s");
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== INTEGRATION STATUS ===");
        NLM_LOG_INFO("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("Prediction: " + std::string(result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("Development: " + std::string(result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("Checkpointing: " + std::string(result.checkpointingWorks ? "WORKING" : "NOT WORKING"));
        
        if (result.totalReward > 0 || result.memoryEpisodesStored > 0) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("✓ PHASE 6 INTEGRATION SUCCESSFUL");
            NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
            NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
            return 0;
        } else {
            NLM_LOG_WARNING("✗ PHASE 6 INTEGRATION INCOMPLETE");
            return 1;
        }
    }
    
    return 0;
}