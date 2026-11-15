/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <string>

using namespace nlm;

// Print banner
void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6 Integration Demo                                  ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Print help message
void printHelp() {
    std::cout << "\nUsage: nlm_phase6_demo [OPTIONS]\n\n";
    std::cout << "NLM Neural Learning Machine - Phase 6 Integration Demo\n\n";
    std::cout << "Options:\n";
    std::cout << "  --help, -h              Show this help message\n";
    std::cout << "  --version, --ver        Show version information\n";
    std::cout << "  --verbose, -v           Enable verbose logging\n";
    std::cout << "  --quiet                 Suppress output\n";
    std::cout << "  --config=<file>         Load configuration from file\n";
    std::cout << "  --neurons=<N>           Override neuron count (default: 500)\n";
    std::cout << "  --steps=<N>             Override simulation steps (default: 2000)\n";
    std::cout << "  --regions=<N>           Override region count (default: 1)\n";
    std::cout << "  --seed=<S>              Override random seed (default: 42)\n";
    std::cout << "  --timestep=<t>          Override simulation timestep (default: 0.001)\n";
    std::cout << "  --no-checkpoint         Disable checkpointing\n";
    std::cout << "  --no-replay             Disable replay\n";
    std::cout << "  --no-development        Disable development\n";
    std::cout << "\nExamples:\n";
    std::cout << "  nlm_phase6_demo --help                  Show help\n";
    std::cout << "  nlm_phase6_demo --verbose --neurons=1000 Run with 1000 neurons\n";
    std::cout << "  nlm_phase6_demo --steps=1000 --quiet    Run with 1000 steps, quiet output\n";
    std::cout << "  nlm_phase6_demo --config=myconfig.cfg    Use custom configuration\n";
    std::cout << "  nlm_phase6_demo --no-checkpoint         Run without checkpointing\n";
    std::cout << std::endl;
}

// Print version
void printVersion() {
    std::cout << "NLM Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "Phase 6 Integration Demo" << std::endl;
    std::cout << "Build with C++20, scikit-build-core, pybind11" << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    bool showHelp = false;
    bool showVersion = false;
    bool verboseMode = false;
    bool quietMode = false;
    std::string configFile = "configs/default.cfg";
    int neurons = 500;
    int steps = 2000;
    int regions = 1;
    int seed = 42;
    double timestep = 0.001;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
        } else if (arg == "--version" || arg == "--ver") {
            showVersion = true;
        } else if (arg == "--verbose" || arg == "-v") {
            verboseMode = true;
        } else if (arg == "--quiet") {
            quietMode = true;
        } else if (arg.substr(0, 9) == "--config=") {
            configFile = arg.substr(9);
        } else if (arg.substr(0, 11) == "--neurons=") {
            neurons = std::stoi(arg.substr(11));
        } else if (arg.substr(0, 8) == "--steps=") {
            steps = std::stoi(arg.substr(8));
        } else if (arg.substr(0, 10) == "--regions=") {
            regions = std::stoi(arg.substr(10));
        } else if (arg.substr(0, 8) == "--seed=") {
            seed = std::stoi(arg.substr(8));
        } else if (arg.substr(0, 12) == "--timestep=") {
            timestep = std::stod(arg.substr(12));
        } else if (arg == "--no-checkpoint") {
            enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            enableReplay = false;
        } else if (arg == "--no-development") {
            enableDevelopment = false;
        }
    }
    
    if (showHelp) {
        printBanner();
        printHelp();
        return 0;
    }
    
    if (showVersion) {
        printVersion();
        return 0;
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Configure logging based on verbosity
    auto logger = std::make_shared<Logger>();
    if (verboseMode) {
        logger->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Info));
    } else if (!quietMode) {
        logger->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Warning));
    }
    Logger::setGlobal(logger);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Load from file if provided
    if (config->loadFromFile(configFile)) {
        std::cout << "Loaded configuration from: " << configFile << std::endl;
    } else {
        std::cout << "Using default configuration." << std::endl;
    }
    
    // Override with command line arguments
    config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(regions), ConfigSource::CommandLine);
    config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::CommandLine);
    config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
    
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
    
    // Run full experiment with configurable settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config phase6Config;
    phase6Config.neuronCount = neurons;
    phase6Config.maxSteps = steps;
    phase6Config.enableCheckpointing = enableCheckpointing;
    phase6Config.enableReplay = enableReplay;
    phase6Config.enableDevelopment = enableDevelopment;
    
    auto result = experiment.run(phase6Config);
    
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
    
    return 0;
}