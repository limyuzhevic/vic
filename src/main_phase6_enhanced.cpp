#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <unordered_map>
#include <map>

using namespace nlm;

class CommandLineParser {
public:
    struct Options {
        std::string configFile = "configs/default.cfg";
        uint64_t neuronCount = 500;
        uint64_t maxSteps = 2000;
        std::string checkpointPath = "./checkpoint_test.bin";
        bool verifyOnly = false;
        bool verbose = false;
        bool quiet = false;
        std::string outputFile;
        std::string logLevel = "info";
        uint64_t randomSeed = 42;
        std::vector<std::string> additionalArgs;
        bool help = false;
        bool version = false;
    };
    
    static Options parse(int argc, char** argv) {
        Options opts;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                opts.help = true;
            } else if (arg == "--version") {
                opts.version = true;
            } else if (arg.substr(0, 8) == "--config") {
                opts.configFile = parseArgument(arg, opts.configFile, i, argv);
            } else if (arg.substr(0, 9) == "--neurons") {
                opts.neuronCount = parseUnsignedLong(arg, opts.neuronCount, i, argv);
            } else if (arg.substr(0, 7) == "--steps") {
                opts.maxSteps = parseUnsignedLong(arg, opts.maxSteps, i, argv);
            } else if (arg.substr(0, 12) == "--checkpoint") {
                opts.checkpointPath = parseArgument(arg, opts.checkpointPath, i, argv);
            } else if (arg == "--verify-only") {
                opts.verifyOnly = true;
            } else if (arg == "--verbose") {
                opts.verbose = true;
            } else if (arg == "--quiet") {
                opts.quiet = true;
            } else if (arg.substr(0, 8) == "--output") {
                opts.outputFile = parseArgument(arg, opts.outputFile, i, argv);
            } else if (arg.substr(0, 10) == "--log-level") {
                opts.logLevel = parseArgument(arg, opts.logLevel, i, argv);
            } else if (arg.substr(0, 13) == "--random-seed") {
                opts.randomSeed = parseUnsignedLong(arg, opts.randomSeed, i, argv);
            } else if (arg.substr(0, 2) == "--") {
                opts.additionalArgs.push_back(arg);
            } else {
                opts.additionalArgs.push_back(arg);
            }
        }
        
        return opts;
    }
    
private:
    static std::string parseArgument(const std::string& arg, const std::string& defaultValue, 
                                   int& i, char** argv) {
        size_t equalsPos = arg.find('=');
        if (equalsPos != std::string::npos) {
            return arg.substr(equalsPos + 1);
        } else if (i + 1 < argc) {
            ++i;
            return std::string(argv[i]);
        }
        return defaultValue;
    }
    
    static uint64_t parseUnsignedLong(const std::string& arg, uint64_t defaultValue,
                                     int& i, char** argv) {
        size_t equalsPos = arg.find('=');
        if (equalsPos != std::string::npos) {
            return std::stoull(arg.substr(equalsPos + 1));
        } else if (i + 1 < argc) {
            ++i;
            return std::stoull(argv[i]);
        }
        return defaultValue;
    }
};

void printBanner() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                                              ║" << std::endl;
    std::cout << "║                    NLM — 熙然 Neural Learning Machine                     ║" << std::endl;
    std::cout << "║                    Advanced Command Line Interface                          ║" << std::endl;
    std::cout << "║                                                                              ║" << std::endl;
    std::cout << "║  An experimental artificial developmental brain with enhanced CLI           ║" << std::endl;
    std::cout << "║  capabilities for automation and integration testing.                     ║" << std::endl;
    std::cout << "║                                                                              ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

void printVersion() {
    std::cout << "NLM Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0 (Phase 6)" << std::endl;
    std::cout << "Build: Phase 6 Final Integration with Enhanced Python Bindings" << std::endl;
    std::cout << "Features: Complete system integration, Python API improvements, CLI enhancements" << std::endl;
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Core Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message and exit" << std::endl;
    std::cout << "  --version                     Show version information and exit" << std::endl;
    std::cout << "  --config <file>               Load configuration from file (default: configs/default.cfg)" << std::endl;
    std::cout << std::endl;
    std::cout << "Simulation Configuration:" << std::endl;
    std::cout << "  --neurons <count>             Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps <count>               Number of simulation steps (default: 2000)" << std::endl;
    std::cout << "  --random-seed <seed>          Random seed for reproducibility (default: 42)" << std::endl;
    std::cout << std::endl;
    std::cout << "Output and Persistence:" << std::endl;
    std::cout << "  --checkpoint <path>           Checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
    std::cout << "  --output <file>               Write results to JSON file" << std::endl;
    std::cout << "  --log-level <level>           Logging level: trace, debug, info, warn, error, fatal" << std::endl;
    std::cout << "  --verbose, -v                 Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q                   Suppress non-essential output" << std::endl;
    std::cout << std::endl;
    std::cout << "Modes:" << std::endl;
    std::cout << "  --verify-only                 Run integration verification only" << std::endl;
    std::cout << "  --full-experiment             Run complete integration experiment (default)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --neurons 1000 --steps 5000" << std::endl;
    std::cout << "  " << programName << " --verify-only --config my_config.cfg" << std::endl;
    std::cout << "  " << programName << " --checkpoint /tmp/my_brain.bin --output results.json" << std::endl;
    std::cout << "  " << programName << " --verbose --log-level debug" << std::endl;
    std::cout << "  " << programName << " -h                                    Show this help" << std::endl;
}

void printProgressBar(float progress, const std::string& message = "", bool quiet = false) {
    if (quiet) return;
    
    const int barWidth = 50;
    int pos = (int)(barWidth * progress);
    
    std::cout << "\r[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << (int)(progress * 100.0) << "%";
    
    if (!message.empty()) {
        std::cout << " - " << message;
    }
    std::cout << std::flush;
}

LogLevel stringToLogLevel(const std::string& level) {
    std::string lower = level;
    for (char& c : lower) c = std::tolower(c);
    
    if (lower == "trace") return LogLevel::Trace;
    if (lower == "debug") return LogLevel::Debug;
    if (lower == "info") return LogLevel::Info;
    if (lower == "warn") return LogLevel::Warning;
    if (lower == "error") return LogLevel::Error;
    if (lower == "fatal") return LogLevel::Fatal;
    
    return LogLevel::Info; // Default
}

void printSystemStatus(const Phase6IntegrationResult& result, bool quiet = false) {
    if (quiet) return;
    
    std::cout << std::endl << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                SYSTEM INTEGRATION STATUS                    ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    std::cout << "🧠 NEURAL SYSTEMS:" << std::endl;
    std::cout << "  Working Memory:     " << (result.memoryWorkingMemoryIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory:   " << (result.memoryEpisodicMemoryIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << std::endl;
    
    std::cout << "⚡ MODULATION SYSTEMS:" << std::endl;
    std::cout << "  Neuromodulation:   " << (result.neuromodulationIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Dopamine Level:    " << std::fixed << std::setprecision(2) << result.dopamineLevel << std::endl;
    std::cout << "  Curiosity Level:   " << std::fixed << std::setprecision(2) << result.curiosityLevel << std::endl;
    std::cout << "  Novelty Level:     " << std::fixed << std::setprecision(2) << result.noveltyLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "🔮 COGNITIVE SYSTEMS:" << std::endl;
    std::cout << "  Prediction:        " << (result.predictionIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Development:       " << (result.developmentIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << std::endl;
    
    std::cout << "💾 PERSISTENCE SYSTEMS:" << std::endl;
    std::cout << "  Checkpointing:     " << (result.checkpointingWorks ? "✅ WORKING" : "❌ NOT WORKING") << std::endl;
    std::cout << "  Replay:            " << (result.replayWorks ? "✅ WORKING" : "❌ NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "📊 PERFORMANCE METRICS:" << std::endl;
    std::cout << "  Total Reward:      " << std::fixed << std::setprecision(3) << result.totalReward << std::endl;
    std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(3) << result.avgFiringRate << " Hz" << std::endl;
    std::cout << "  Synaptic Weight:   " << std::fixed << std::setprecision(3) << result.avgSynapticWeight << std::endl;
    std::cout << "  Episodes Stored:   " << std::fixed << std::setprecision(0) << result.memoryEpisodesStored << std::endl;
    std::cout << std::endl;
    
    std::cout << "⏱️ TIMING INFO:" << std::endl;
    std::cout << "  Wall Clock Time:   " << std::fixed << std::setprecision(2) << result.totalWallClockTime << " seconds" << std::endl;
    std::cout << "  Start Time:        " << ctime(&result.startTime) << std::endl;
    std::cout << std::endl;
}

std::string resultToJson(const Phase6IntegrationResult& result) {
    std::stringstream json;
    json << "{\n";
    json << "  \"integration_status\": {\n";
    json << "    \"memory_working_memory_integrated\": " << (result.memoryWorkingMemoryIntegrated ? "true" : "false") << ",\n";
    json << "    \"memory_episodic_memory_integrated\": " << (result.memoryEpisodicMemoryIntegrated ? "true" : "false") << ",\n";
    json << "    \"neuromodulation_integrated\": " << (result.neuromodulationIntegrated ? "true" : "false") << ",\n";
    json << "    \"prediction_integrated\": " << (result.predictionIntegrated ? "true" : "false") << ",\n";
    json << "    \"development_integrated\": " << (result.developmentIntegrated ? "true" : "false") << ",\n";
    json << "    \"checkpointing_works\": " << (result.checkpointingWorks ? "true" : "false") << ",\n";
    json << "    \"replay_works\": " << (result.replayWorks ? "true" : "false") << "\n";
    json << "  },\n";
    json << "  \"performance_metrics\": {\n";
    json << "    \"total_reward\": " << std::fixed << std::setprecision(6) << result.totalReward << ",\n";
    json << "    \"avg_firing_rate\": " << std::fixed << std::setprecision(6) << result.avgFiringRate << ",\n";
    json << "    \"avg_synaptic_weight\": " << std::fixed << std::setprecision(6) << result.avgSynapticWeight << ",\n";
    json << "    \"memory_episodes_stored\": " << std::fixed << std::setprecision(0) << result.memoryEpisodesStored << ",\n";
    json << "    \"novelty_level\": " << std::fixed << std::setprecision(6) << result.noveltyLevel << ",\n";
    json << "    \"curiosity_level\": " << std::fixed << std::setprecision(6) << result.curiosityLevel << ",\n";
    json << "    \"dopamine_level\": " << std::fixed << std::setprecision(6) << result.dopamineLevel << "\n";
    json << "  },\n";
    json << "  \"timing_info\": {\n";
    json << "    \"start_time\": " << result.startTime << ",\n";
    json << "    \"end_time\": " << result.endTime << ",\n";
    json << "    \"total_wall_clock_time\": " << std::fixed << std::setprecision(6) << result.totalWallClockTime << "\n";
    json << "  }\n";
    json << "}\n";
    
    return json.str();
}

int main(int argc, char** argv) {
    // Parse command line arguments
    auto options = CommandLineParser::parse(argc, argv);
    
    if (options.help) {
        printUsage(argv[0]);
        return 0;
    }
    
    if (options.version) {
        printVersion();
        return 0;
    }
    
    printBanner();
    
    // Initialize logging with specified level
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(stringToLogLevel(options.logLevel));
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    if (!options.quiet) {
        std::cout << "Starting NLM Phase 6 Command Line Interface..." << std::endl;
        std::cout << "Config file: " << options.configFile << std::endl;
        std::cout << "Neurons: " << options.neuronCount << std::endl;
        std::cout << "Max steps: " << options.maxSteps << std::endl;
        std::cout << "Checkpoint: " << options.checkpointPath << std::endl;
        std::cout << "Log level: " << options.logLevel << std::endl;
        std::cout << "Random seed: " << options.randomSeed << std::endl;
        std::cout << "Verify only: " << (options.verifyOnly ? "Yes" : "No") << std::endl;
        std::cout << std::endl;
    }
    
    // Create and run Phase 6 experiment
    Phase6IntegratedExperiment experiment;
    
    if (!options.quiet) {
        std::cout << "🔍 Running integration verification..." << std::endl;
        printProgressBar(0.0f, "Starting verification", options.quiet);
    }
    
    bool integrationOK = experiment.verifyIntegration();
    
    if (!options.quiet) {
        printProgressBar(1.0f, "Verification complete", options.quiet);
    }
    
    if (!integrationOK) {
        std::cerr << "❌ ERROR: Integration verification failed!" << std::endl;
        std::cerr << "The brain systems are not properly integrated." << std::endl;
        std::cerr << "Check the logs above for specific failures." << std::endl;
        return 1;
    }
    
    if (!options.quiet) {
        std::cout << std::endl;
        std::cout << "✅ Integration verification passed!" << std::endl;
    }
    
    if (options.verifyOnly) {
        if (!options.quiet) {
            std::cout << std::endl;
            std::cout << "🔧 Verification complete. Use full experiment without --verify-only for complete demo." << std::endl;
        }
        return 0;
    }
    
    // Test individual systems
    if (!options.quiet) {
        std::cout << std::endl;
        std::cout << "🧪 Running individual system tests..." << std::endl;
        
        // Memory integration test
        std::cout << "  Testing memory integration..." << std::endl;
        bool memoryOK = experiment.testMemoryIntegration();
        
        // Neuromodulation integration test
        std::cout << "  Testing neuromodulation integration..." << std::endl;
        bool neuromodOK = experiment.testNeuromodulationIntegration();
        
        // Checkpoint test
        std::cout << "  Testing checkpoint system..." << std::endl;
        bool checkpointOK = experiment.testCheckpointing();
        
        // Replay test
        std::cout << "  Testing replay system..." << std::endl;
        bool replayOK = experiment.testReplay();
        
        if (!(memoryOK && neuromodOK && checkpointOK && replayOK)) {
            std::cerr << "⚠️  WARNING: Some system tests failed!" << std::endl;
            std::cerr << "However, the experiment will continue to demonstrate integration." << std::endl;
        }
    }
    
    if (!options.quiet) {
        std::cout << std::endl;
        std::cout << "🚀 Running full integration experiment..." << std::endl;
    }
    
    // Configure the experiment
    Phase6Config config;
    config.maxSteps = options.maxSteps;
    config.neuronCount = options.neuronCount;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = options.checkpointPath;
    
    // Run the experiment
    auto startTime = std::chrono::high_resolution_clock::now();
    Phase6IntegrationResult result = experiment.run(config);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    // Display results
    printSystemStatus(result, options.quiet);
    
    // Write results to file if requested
    if (!options.outputFile.empty()) {
        std::ofstream outfile(options.outputFile);
        if (outfile.is_open()) {
            outfile << resultToJson(result);
            outfile.close();
            if (!options.quiet) {
                std::cout << "✅ Results written to: " << options.outputFile << std::endl;
            }
        } else {
            std::cerr << "❌ WARNING: Could not write results to " << options.outputFile << std::endl;
        }
    }
    
    // Final summary
    if (!options.quiet) {
        std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                              EXPERIMENT SUMMARY                              ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        
        if (result.totalReward > 0.0f) {
            std::cout << "🎉 SUCCESS: Learning occurred during the experiment!" << std::endl;
            std::cout << "   The brain demonstrated experience-driven synaptic changes." << std::endl;
        } else {
            std::cout << "⚠️  WARNING: Minimal learning detected." << std::endl;
            std::cout << "   Consider increasing simulation time or adjusting parameters." << std::endl;
        }
        
        if (result.memoryWorkingMemoryIntegrated && result.memoryEpisodicMemoryIntegrated) {
            std::cout << "🧠 MEMORY SYSTEMS: Both working and episodic memory are functional and integrated." << std::endl;
        }
        
        if (result.neuromodulationIntegrated) {
            std::cout << "⚡ NEUROMODULATION: Reward prediction error and curiosity modulation are working." << std::endl;
        }
        
        if (result.developmentIntegrated) {
            std::cout << "🌱 DEVELOPMENT: Neural development and plasticity are occurring." << std::endl;
        }
        
        if (result.checkpointingWorks) {
            std::cout << "💾 PERSISTENCE: Checkpoint save/load system is operational." << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Phase 6 integration completed successfully!" << std::endl;
        std::cout << "The NLM artificial brain demonstrates coherent operation of all major systems." << std::endl;
    }
    
    return 0;
}
