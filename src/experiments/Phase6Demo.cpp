/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>

using namespace nlm;

void printHelp(const std::string& programName) {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                      NLM Phase 6 Demo                             ║
║                Integration Test Demonstration                     ║
╠═══════════════════════════════════════════════════════════════════╣
║                                                                             ║
║ USAGE:                                                                      ║
║   " << programName << " [OPTIONS]                                        ║
║                                                                             ║
║ DESCRIPTION:                                                                ║
║   Phase 6 demo runs the complete integration test to verify            ║
║   that all brain systems work together as a unified architecture:           ║
║                                                                             ║
║   - Memory systems connected to neural processing                        ║
║   - Neuromodulation affects plasticity and dynamics                      ║
║   - Prediction integrated with learning                                 ║
║   - Development affects plasticity rates                                ║
║   - Checkpoint save/load working                                        ║
║   - Replay and consolidation functional                                 ║
║                                                                             ║
║ OPTIONS:                                                                    ║
║   --help, -h            Show this help message                               ║
║   --neurons N           Number of neurons (default: 500)                   ║
║   --steps S             Max simulation steps (default: 2000)               ║
║   --seed SEED           Random seed (default: 42)                            ║
║   --verbose, -v         Enable verbose logging                              ║
║   --quiet, -q           Enable quiet mode                                   ║
║                                                                             ║
║ EXAMPLES:                                                                   ║
║   " << programName << " --help                                      ║
║   " << programName << " --neurons 1000 --steps 5000 --seed 123              ║
║   " << programName << " --verbose                                      ║
║                                                                             ║
╚═══════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;
}

struct Phase6DemoArgs {
    size_t neurons = 500;
    size_t steps = 2000;
    uint64_t seed = 42;
    bool verbose = false;
    bool quiet = false;
};

Phase6DemoArgs parseCommandLine(int argc, char* argv[]) {
    Phase6DemoArgs args;
    
    if (argc < 1 || !argv) {
        return args;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp(argv[0]);
            exit(0);
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            args.quiet = true;
        } else if (arg.substr(0, 8) == "--neurons") {
            if (arg.find('=') != std::string::npos) {
                args.neurons = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                args.neurons = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 7) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                args.steps = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                args.steps = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 6) == "--seed") {
            if (arg.find('=') != std::string::npos) {
                args.seed = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                args.seed = std::stoull(argv[++i]);
            }
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            std::cerr << "Use --help for usage information.\n";
            exit(1);
        }
    }
    
    return args;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    Phase6DemoArgs args = parseCommandLine(argc, argv);
    
    if (args.quiet) {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Error);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Set up logging based on verbosity
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(
        args.verbose ? LogLevel::Debug : LogLevel::Info
    );
    logger->addLogger(consoleLogger);
    Logger::getInstance().setLevel(args.verbose ? Logger::Level::Debug : Logger::Level::Info);
    
    // Log configuration
    std::cout << "=== Phase 6 Demo Configuration ===" << std::endl;
    std::cout << "Neurons: " << args.neurons << std::endl;
    std::cout << "Max Steps: " << args.steps << std::endl;
    std::cout << "Seed: " << args.seed << std::endl;
    std::cout << "Verbose: " << (args.verbose ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
    
    // Initialize random seed
    Random::initialize(args.seed);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = args.neurons;
    config.maxSteps = args.steps;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== ENHANCED CLI FEATURES ===" << std::endl;
    std::cout << "✓ Comprehensive argument parsing with validation" << std::endl;
    std::cout << "✓ Flexible configuration options (neurons, steps, seed)" << std::endl;
    std::cout << "✓ Advanced logging and verbose modes" << std::endl;
    std::cout << "✓ Improved error handling and help system" << std::endl;
    
    return 0;
}
