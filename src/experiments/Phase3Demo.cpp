#include "experiments/Phase3Demo.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <map>

// Parse advanced command-line options
struct AdvancedOptions {
    // Simulation parameters
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    int maxSteps = 2000;
    
    // Environment parameters
    int environmentWidth = 20;
    int environmentHeight = 20;
    int visionWidth = 16;
    int visionHeight = 16;
    
    // Brain parameters
    int neuronCount = 500;
    int regionCount = 2;
    float connectionProbability = 0.1f;
    int64_t randomSeed = 42;
    float simulationTimestep = 0.001f;
    
    // Plasticity parameters
    float stdpLTPWeight = 0.01f;
    float stdpLTDWeight = 0.012f;
    float stdpTimeConstant = 20.0f;
    
    // Development parameters
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    
    // Neuromodulation parameters
    float dopamineBaseline = 0.1f;
    
    // Reward parameters
    float rewardDiscountFactor = 0.99f;
    
    // Logging parameters
    std::string logLevel = "INFO";
    bool logToFile = false;
    std::string logFilename = "nlm.log";
    
    // Visualization parameters
    bool visualizationEnabled = false;
    float visualizationUpdateRate = 30.0f;
    
    // Checkpoint parameters
    bool enableCheckpointing = true;
    std::string checkpointPath = "/tmp/nlm_checkpoint.bin";
    
    // File paths
    std::string configFile = "configs/default.cfg";
    std::string checkpointFile = "";
    
    // Flags
    bool verbose = false;
    bool quiet = false;
    bool help = false;
    bool version = false;
};

// Print help information
void printHelp(const std::string& programName) {
    std::cout << "=== NLM Phase 3 Advanced Demo ===" << std::endl;
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Simulation Parameters:" << std::endl;
    std::cout << "  -episodes N      Number of episodes to run (default: 5)" << std::endl;
    std::cout << "  -steps N         Steps per episode (default: 200)" << std::endl;
    std::cout << "  -maxsteps N      Maximum total steps (default: 2000)" << std::endl;
    std::cout << std::endl;
    std::cout << "Environment Parameters:" << std::endl;
    std::cout << "  -width N         World width (default: 20)" << std::endl;
    std::cout << "  -height N        World height (default: 20)" << std::endl;
    std::cout << "  -vw N            Vision width (default: 16)" << std::endl;
    std::cout << "  -vh N            Vision height (default: 16)" << std::endl;
    std::cout << std::endl;
    std::cout << "Brain Parameters:" << std::endl;
    std::cout << "  -neurons N       Number of neurons (default: 500)" << std::endl;
    std::cout << "  -regions N       Number    regions (default: 2)" << std::endl;
    std::cout << "  -connprob F      Connection probability (default: 0.1)" << std::endl;
    std::cout << "  -seed N          Random seed (default: 42)" << std::endl;
    std::cout << "  -dt F            Simulation timestep (default: 0.001)" << std::endl;
    std::endl;
    std::cout << "Plasticity Parameters:" << std::endl;
    std::cout << "  -stdpltp F       STDP LTP weight (default: 0.01)" << std::endl;
    std::cout << "  -stdpltd F       STDP LTD weight (default: 0.012)" << std::endl;
    std::cout << "  -stdpta F        STDP time constant (default: 20.0)" << std::endl;
    std::cout << "  -synpt F         Synaptogenesis rate (default: 0.0001)" << std::endl;
    std::cout << "  -prune F         Pruning rate (default: 0.00001)" << std::endl;
    std::cout << std::endl;
    std::cout << "Development Parameters:" << std::endl;
    std::cout << "  -db F            Dopamine baseline (default: 0.1)" << std::endl;
    std::cout << std::endl;
    std::cout << "Reward Parameters:" << std::endl;
    std::cout << "  -discount F      Reward discount factor (default: 0.99)" << std::endl;
    std::cout << std::endl;
    std::cout << "Logging Parameters:" << std::endl;
    std::cout << "  -loglevel L      Log level (DEBUG, INFO, WARN, ERROR)" << std::endl;
    std::cout << "  -logfile F       Log to file (default: nlm.log)" << std::endl;
    std::cout << "  -verbose         Enable verbose output" << std::endl;
    std::cout << "  -quiet           Quiet mode (no progress output)" << std::endl;
    std::cout << std::endl;
    std::cout << "Checkpoint Parameters:" << std::endl;
    std::cout << "  -checkpoint F     Checkpoint file path (default: /tmp/nlm_checkpoint.bin)" << std::endl;
    std::cout << "  -nocheckpoint     Disable checkpointing" << std::endl;
    std::cout << std::endl;
    std::cout << "File Paths:" << std::endl;
    std::cout << "  -config F        Config file path (default: configs/default.cfg)" << std::endl;
    std::cout << std::endl;
    std::cout << "Special Options:" << std::endl;
    std::cout << "  -help            Show this help message" << std::endl;
    std::cout << "  -version         Show version information" << std::endl;
}

// Parse command-line arguments
AdvancedOptions parseAdvancedOptions(int argc, char* argv[]) {
    AdvancedOptions options;
    
    // Map of option flags to values
    std::map<std::string, std::string> argMap;
    
    // First pass: collect all --key value pairs
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            // --key value format
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                argMap[arg.substr(2)] = argv[++i];
            }
        } else if (arg.substr(0, 1) == "-") {
            // -key format (if supported)
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                argMap[arg.substr(1)] = argv[++i];
            }
        } else {
            // Positional arguments (for backward compatibility)
            if (i == 1) {
                options.numEpisodes = std::atoi(argv[i]);
            } else if (i == 2) {
                options.stepsPerEpisode = std::atoi(argv[i]);
            }
        }
    }
    
    // Second pass: apply options from map
    for (const auto& pair : argMap) {
        const std::string& key = pair.first;
        const std::string& value = pair.second;
        
        try {
            if (key == "episodes") {
                options.numEpisodes = std::stoi(value);
            } else if (key == "steps") {
                options.stepsPerEpisode = std::stoi(value);
            } else if (key == "maxsteps") {
                options.maxSteps = std::stoi(value);
            } else if (key == "width") {
                options.environmentWidth = std::stoi(value);
            } else if (key == "height") {
                options.environmentHeight = std::stoi(value);
            } else if (key == "vw") {
                options.visionWidth = std::stoi(value);
            } else if (key == "vh") {
                options.visionHeight = std::stoi(value);
            } else if (key == "neurons") {
                options.neuronCount = std::stoi(value);
            } else if (key == "regions") {
                options.regionCount = std::stoi(value);
            } else if (key == "connprob") {
                options.connectionProbability = std::stof(value);
            } else if (key == "seed") {
                options.randomSeed = std::stoll(value);
            } else if (key == "dt") {
                options.simulationTimestep = std::stof(value);
            } else if (key == "stdpltp") {
                options.stdpLTPWeight = std::stof(value);
            } else if (key == "stdpltd") {
                options.stdpLTDWeight = std::stof(value);
            } else if (key == "stdpta") {
                options.stdpTimeConstant = std::stof(value);
            } else if (key == "synpt") {
                options.synaptogenesisRate = std::stof(value);
            } else if (key == "prune") {
                options.pruningRate = std::stof(value);
            } else if (key == "db") {
                options.dopamineBaseline = std::stof(value);
            } else if (key == "discount") {
                options.rewardDiscountFactor = std::stof(value);
            } else if (key == "loglevel") {
                options.logLevel = value;
            } else if (key == "logfile") {
                options.logFilename = value;
            } else if (key == "checkpoint") {
                options.checkpointPath = value;
            } else if (key == "config") {
                options.configFile = value;
            } else if (key == "verbose") {
                options.verbose = true;
            } else if (key == "quiet") {
                options.quiet = true;
            } else if (key == "help") {
                options.help = true;
            } else if (key == "version") {
                options.version = true;
            } else if (key == "nocheckpoint") {
                options.enableCheckpointing = false;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error parsing option --" << key << ": " << e.what() << std::endl;
        }
    }
    
    return options;
}

// Print version information
void printVersion() {
    std::cout << "NLM Phase 3 Demo v0.1.0" << std::endl;
    std::cout << "Built with C++20" << std::endl;
    std::cout << "Phase 3: World interaction and learning" << std::endl;
}

// Main entry point for Phase 3 demo with advanced CLI
int main(int argc, char* argv[]) {
    // Parse command-line options
    AdvancedOptions options = parseAdvancedOptions(argc, argv);
    
    // Check for help or version requests
    if (options.help) {
        printHelp(argv[0]);
        return 0;
    }
    
    if (options.version) {
        printVersion();
        return 0;
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Set log level based on command line
    if (options.logLevel == "DEBUG") {
        Logger::getGlobal().setLevel(LogLevel::Debug);
    } else if (options.logLevel == "WARN") {
        Logger::getGlobal().setLevel(LogLevel::Warn);
    } else if (options.logLevel == "ERROR") {
        Logger::getGlobal().setLevel(LogLevel::Error);
    }
    
    if (options.verbose) {
        std::cout << "=== NLM Phase 3 Advanced Demo ===" << std::endl;
        std::cout << "Simulation Parameters:" << std::endl;
        std::cout << "  Episodes: " << options.numEpisodes << std::endl;
        std::cout << "  Steps per episode: " << options.stepsPerEpisode << std::endl;
        std::cout << "  Environment size: " << options.environmentWidth << "x" << options.environmentHeight << std::endl;
        std::cout << "  Vision: " << options.visionWidth << "x" << options.visionHeight << std::endl;
        std::cout << "  Neurons: " << options.neuronCount << std::endl;
        std::cout << "  Regions: " << options.regionCount << std::endl;
        std::cout << "  Connection probability: " << options.connectionProbability << std::endl;
        std::cout << "  Random seed: " << options.randomSeed << std::endl;
        std::cout << "  Simulation timestep: " << options.simulationTimestep << std::endl;
        std::cout << std::endl;
        
        std::cout << "Plasticity Parameters:" << std::endl;
        std::cout << "  STDP LTP weight: " << options.stdpLTPWeight << std::endl;
        std::cout << "  STDP LTD weight: " << options.stdpLTDWeight << std::endl;
        std::cout << "  STDP time constant: " << options.stdpTimeConstant << std::endl;
        std::cout << "  Synaptogenesis rate: " << options.synaptogenesisRate << std::endl;
        std::cout << "  Pruning rate: " << options.pruningRate << std::endl;
        std::cout << std::endl;
        
        std::cout << "Development Parameters:" << std::endl;
        std::cout << "  Dopamine baseline: " << options.dopamineBaseline << std::endl;
        std::cout << std::endl;
        
        std::cout << "Reward Parameters:" << std::endl;
        std::cout << "  Reward discount factor: " << options.rewardDiscountFactor << std::endl;
        std::cout << std::endl;
        
        std::cout << "Checkpointing: " << (options.enableCheckpointing ? "ENABLED" : "DISABLED") << std::endl;
        if (options.enableCheckpointing) {
            std::cout << "  Checkpoint path: " << options.checkpointPath << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Create the demo with advanced options
    nlm::Phase3Demo demo;
    
    // Configure the demo (this would require modifications to Phase3Demo class)
    // For now, we'll just use the demo with the basic options
    std::cout << "Starting Phase 3 demo with " << options.numEpisodes << " episodes..." << std::endl;
    
    if (!options.quiet) {
        std::cout << "Press Ctrl+C to interrupt" << std::endl;
    }
    
    demo.run(options.numEpisodes, options.stepsPerEpisode);
    
    if (!options.quiet) {
        std::cout << std::endl;
        std::cout << "=== Demo Complete ===" << std::endl;
        std::cout << "The agent learned from " << options.numEpisodes * options.stepsPerEpisode << " interaction steps." << std::endl;
        std::cout << "Check the output above for detailed metrics and learning progress." << std::endl;
    }
    
    return 0;
}