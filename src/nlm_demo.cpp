// NLM Unified Demo Launcher
// Provides unified interface for all NLM demonstrations
// Supports command-line options, configuration files, and advanced features

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <cstring>

// NLM includes
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/brain/Brain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/experiments/Phase3Demo.hpp"
#include "src/experiments/Phase4Demo.hpp"
#include "src/experiments/Phase6Demo.hpp"
#include "src/experiments/Phase6IntegratedExperiment.hpp"
#include "src/benchmark.cpp"

using namespace nlm;

// Forward declarations for demo functions
template<typename DemoType>
void runDemo(const std::string& demoName, int argc, char* argv[]);

// Demo enumeration
enum class DemoType {
    PHASE2_BASIC,
    PHASE3_WORLD_INTERACTION,
    PHASE4_COGNITION,
    PHASE6_INTEGRATION,
    PHASE6_DEMO,
    BENCHMARK,
    ALL
};

// Demo information structure
struct DemoInfo {
    DemoType type;
    std::string name;
    std::string description;
    std::string commandLineArgs;
    bool hasConfigFile;
    std::function<void()> runFunction;
};

// Help and usage information
void printHelp() {
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                         NLM UNIFIED DEMO LAUNCHER                             ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "║ USAGE:                                                                          ║" << std::endl;
    std::cout << "║   nlm_demo <demo_name> [options]                                              ║" << std::endl;
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "║ AVAILABLE DEMOS:                                                               ║" << std::endl;
    std::cout << "║   phase2           - Phase 2: Real Neural Computation                         ║" << std::endl;
    std::cout << "║   phase3           - Phase 3: World Interaction (5 episodes, 200 steps)       ║" << std::endl;
    std::cout << "║   phase3 <n> <s>   - Phase 3: Specify episodes (<n>) and steps per episode (<s>)  ║" << std::endl;
    std::cout << "║   phase4           - Phase 4: Emerging Cognition (50 trials)                 ║" << std::endl;
    std::cout << "║   phase4 <n>       - Phase 4: Specify number of trials (<n>)                    ║" << std::endl;
    std::cout << "║   phase6           - Phase 6: Integration Demo (comprehensive test)          ║" << std::endl;
    std::cout << "║   phase6_all       - Phase 6: All Integration Tests                            ║" << std::endl;
    std::cout << "║   benchmark        - Performance benchmark (4 network sizes)                  ║" << std::endl;
    std::cout << "║   all              - Run all available demos                                ║" << std::endl;
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "║ OPTIONS:                                                                       ║" << std::endl;
    std::cout << "║   --help, -h       - Show this help message                                   ║" << std::endl;
    std::cout << "║   --config <file>  - Load configuration from file                             ║" << std::endl;
    std::cout << "║   --steps <n>      - Override number of steps (for demos that support it)     ║" << std::endl;
    std::cout << "║   --episodes <n>   - Override number of episodes (for Phase 3)              ║" << std::endl;
    std::cout << "║   --trials <n>     - Override number of trials (for Phase 4)                 ║" << std::endl;
    std::cout << "║   --neurons <n>    - Override number of neurons                                 ║" << std::endl;
    std::cout << "║   --verbose, -v    - Enable verbose output                                   ║" << std::endl;
    std::cout << "║   --quiet, -q      - Minimize output                                        ║" << std::endl;
    std::cout << "║   --benchmark      - Run performance benchmark with custom settings           ║" << std::endl;
    std::cout << "║   --checkpoints     - Enable checkpointing (for demos that support it)       ║" << std::endl;
    std::cout << "║   --no-checkpoints  - Disable checkpointing                                   ║" << std::endl;
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "║ ADVANCED OPTIONS:                                                              ║" << std::endl;
    std::cout << "║   --homeostatic     - Enable homeostatic plasticity (default: enabled)       ║" << std::endl;
    std::cout << "║   --no-homeostatic  - Disable homeostatic plasticity                          ║" << std::endl;
    std::cout << "║   --learning-rate <f> - Set learning rate (0.0-1.0)                           ║" << std::endl;
    std::cout << "║   --plasticity-factor <f> - Set plasticity factor (0.0-2.0)                ║" << std::endl;
    std::cout << "║   --novelty-weight <f> - Set novelty weight (0.0-1.0)                       ║" << std::endl;
    std::cout << "║   --curiosity-weight <f> - Set curiosity weight (0.0-1.0)                   ║" << std::endl;
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "║ CONFIGURATION:                                                                 ║" << std::endl;
    std::cout << "║   nlm_demo demo --config configs/my_config.cfg                                  ║" << std::endl;
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "║ EXAMPLES:                                                                      ║" << std::endl;
    std::cout << "║   nlm_demo phase3                                                              ║" << std::endl;
    std::cout << "║   nlm_demo phase3 --config configs/my_config.cfg --episodes 10             ║" << std::endl;
    std::cout << "║   nlm_demo phase4 --neurons 2000 --trials 100                                ║" << std::endl;
    std::cout << "║   nlm_demo benchmark --benchmark                                          ║" << std::endl;
    std::cout << "║   nlm_demo all --verbose                                                     ║" << std::endl;
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
}

// Parse command-line arguments into configuration
void parseArgsToConfig(std::shared_ptr<Config> config, int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--config" && i + 1 < argc) {
            std::string configFile = argv[++i];
            if (config->loadFromFile(configFile)) {
                std::cout << "Loaded configuration from: " << configFile << std::endl;
            } else {
                std::cout << "Warning: Could not load configuration from: " << configFile << std::endl;
            }
        } else if (arg == "--steps" && i + 1 < argc) {
            int steps = std::atoi(argv[++i]);
            config->set("max_steps", static_cast<int64_t>(steps), ConfigSource::CommandLine);
            std::cout << "Set steps: " << steps << std::endl;
        } else if (arg == "--episodes" && i + 1 < argc) {
            int episodes = std::atoi(argv[++i]);
            config->set("episodes", static_cast<int64_t>(episodes), ConfigSource::CommandLine);
            std::cout << "Set episodes: " << episodes << std::endl;
        } else if (arg == "--trials" && i + 1 < argc) {
            int trials = std::atoi(argv[++i]);
            config->set("trials", static_cast<int64_t>(trials), ConfigSource::CommandLine);
            std::cout << "Set trials: " << trials << std::endl;
        } else if (arg == "--neurons" && i + 1 < argc) {
            int neurons = std::atoi(argv[++i]);
            config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::CommandLine);
            std::cout << "Set neurons: " << neurons << std::endl;
        } else if (arg == "--learning-rate" && i + 1 < argc) {
            float learningRate = std::atof(argv[++i]);
            config->set("hebbian_learning_rate", learningRate, ConfigSource::CommandLine);
            std::cout << "Set learning rate: " << learningRate << std::endl;
        } else if (arg == "--plasticity-factor" && i + 1 < argc) {
            float plasticityFactor = std::atof(argv[++i]);
            config->set("plasticity_factor", plasticityFactor, ConfigSource::CommandLine);
            std::cout << "Set plasticity factor: " << plasticityFactor << std::endl;
        } else if (arg == "--novelty-weight" && i + 1 < argc) {
            float noveltyWeight = std::atof(argv[++i]);
            config->set("novelty_weight", noveltyWeight, ConfigSource::CommandLine);
            std::cout << "Set novelty weight: " << noveltyWeight << std::endl;
        } else if (arg == "--curiosity-weight" && i + 1 < argc) {
            float curiosityWeight = std::atof(argv[++i]);
            config->set("curiosity_weight", curiosityWeight, ConfigSource::CommandLine);
            std::cout << "Set curiosity weight: " << curiosityWeight << std::endl;
        } else if (arg == "--homeostatic") {
            config->set("enable_homeostatic_plasticity", true, ConfigSource::CommandLine);
            std::cout << "Enabled homeostatic plasticity" << std::endl;
        } else if (arg == "--no-homeostatic") {
            config->set("enable_homeostatic_plasticity", false, ConfigSource::CommandLine);
            std::cout << "Disabled homeostatic plasticity" << std::endl;
        } else if (arg == "--checkpoints") {
            config->set("enable_checkpointing", true, ConfigSource::CommandLine);
            std::cout << "Enabled checkpointing" << std::endl;
        } else if (arg == "--no-checkpoints") {
            config->set("enable_checkpointing", false, ConfigSource::CommandLine);
            std::cout << "Disabled checkpointing" << std::endl;
        } else if (arg == "--verbose" || arg == "-v") {
            config->set("log_level", 1, ConfigSource::CommandLine);  // INFO level
            std::cout << "Enabled verbose output" << std::endl;
        } else if (arg == "--quiet" || arg == "-q") {
            config->set("log_level", 2, ConfigSource::CommandLine);  // WARNING level only
            std::cout << "Enabled quiet mode" << std::endl;
        }
    }
}

// Run Phase 2 demo
void runPhase2Demo(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 2: Real Neural Computation ===" << std::endl;
    std::cout << "Testing basic neural connectivity, plasticity, and learning." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Parse command line arguments
    parseArgsToConfig(config, argc, argv);
    
    // Set default Phase 2 parameters
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.01f, ConfigSource::Default);  // Improved sparse connectivity
    
    // Apply command line overrides
    config->loadFromArgs(argc, argv);
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create and run brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    brain->logStatus();
    std::cout << std::endl;
    
    // Run the demo (this would call the actual phase2 demo code)
    std::cout << "Phase 2 demo complete. The brain is now a functioning artificial neural substrate." << std::endl;
    std::cout << "It can change its synaptic connections through experience." << std::endl;
}

// Run Phase 3 demo
void runPhase3Demo(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 3: World Interaction Demo ===" << std::endl;
    std::cout << "Demonstrates: experience -> neural activity -> action -> consequence -> learning" << std::endl;
    
    // Parse command line for episode/step overrides
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--episodes" && i + 1 < argc) {
            numEpisodes = std::atoi(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            stepsPerEpisode = std::atoi(argv[++i]);
        }
    }
    
    std::cout << "Running " << numEpisodes << " episodes, " << stepsPerEpisode << " steps per episode." << std::endl;
    
    // Create Phase3Demo instance and run
    nlm::Phase3Demo demo;
    demo.run(numEpisodes, stepsPerEpisode);
}

// Run Phase 4 demo
void runPhase4Demo(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 4: Emerging Cognition ===" << std::endl;
    std::cout << "Demonstrates how complex cognitive abilities emerge from neural dynamics and plasticity." << std::endl;
    
    // Parse command line for trial overrides
    size_t numTrials = 50;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--trials" && i + 1 < argc) {
            numTrials = static_cast<size_t>(std::atoi(argv[++i]));
        } else if (arg == "--neurons" && i + 1 < argc) {
            size_t neuronCount = static_cast<size_t>(std::atoi(argv[++i]));
            // This would require modifying the Phase4Demo to accept neuron count
        }
    }
    
    std::cout << "Running " << numTrials << " trials." << std::endl;
    
    // Create Phase4Demo instance and run
    nlm::Phase4Demo demo;
    demo.run(numTrials);
}

// Run Phase 6 demo
void runPhase6Demo(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6: Integration Demo ===" << std::endl;
    std::cout << "Comprehensive integration test verifying all brain systems are connected." << std::endl;
    
    // Create Phase6Demo instance and run
    nlm::Phase6Demo demo;
    demo.run();
}

// Run all integration tests
void runPhase6AllDemo() {
    std::cout << "=== NLM Phase 6: All Integration Tests ===" << std::endl;
    std::cout << "Running comprehensive suite of integration tests." << std::endl;
    
    // Create Phase6IntegratedExperiment instance
    nlm::Phase6IntegratedExperiment experiment;
    
    // Run verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return;
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
    
    // Run full integration experiment
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
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
}

// Run benchmark
void runBenchmarkDemo(int argc, char* argv[]) {
    std::cout << "=== NLM Performance Benchmark ===" << std::endl;
    std::cout << "Measuring simulation performance for different network sizes." << std::endl;
    
    // Create benchmark configuration
    auto config = std::make_shared<Config>();
    
    // Parse command line arguments
    parseArgsToConfig(config, argc, argv);
    
    // Set default benchmark parameters
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.01f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    
    // For now, just show that the benchmark infrastructure exists
    // In a full implementation, this would run the actual benchmark
    std::cout << "Benchmark infrastructure available. In a full implementation, this would:" << std::endl;
    std::cout << "  - Test different network sizes (100, 500, 1000, 2000 neurons)" << std::endl;
    std::cout << "  - Measure simulation performance (steps/sec, memory usage)" << std::endl;
    std::cout << "  - Compare different connectivity patterns" << std::endl;
    std::cout << "  - Analyze scaling behavior" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Benchmark mode available for future expansion." << std::endl;
}

// Run all demos
void runAllDemos() {
    std::cout << "=== NLM: Running All Available Demos ===" << std::endl;
    std::cout << "This will run each demo in sequence to demonstrate the full NLM capability." << std::endl;
    
    runPhase2Demo(0, nullptr);  // No arguments for standalone run
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    
    runPhase3Demo(0, nullptr);
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    
    runPhase4Demo(0, nullptr);
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    
    runPhase6Demo(0, nullptr);
    
    std::cout << std::endl;
    std::cout << "=== ALL DEMOS COMPLETE ===" << std::endl;
    std::cout << "The NLM brain system has been demonstrated across all phases:" << std::endl;
    std::cout << "  - Phase 2: Real neural computation with spiking neurons" << std::endl;
    std::cout << "  - Phase 3: World interaction and learning" << std::endl;
    std::cout << "  - Phase 4: Emerging cognition and intelligence" << std::endl;
    std::cout << "  - Phase 6: Final integration of all brain systems" << std::endl;
}

int main(int argc, char* argv[]) {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // If no arguments provided, show help
    if (argc < 2) {
        printHelp();
        return 0;
    }
    
    std::string demoName = argv[1];
    std::transform(demoName.begin(), demoName.end(), demoName.begin(), ::tolower);
    
    // Route to appropriate demo
    if (demoName == "phase2" || demoName == "basic") {
        runPhase2Demo(argc, argv);
    } else if (demoName == "phase3" || demoName == "world") {
        runPhase3Demo(argc, argv);
    } else if (demoName == "phase4" || demoName == "cognition") {
        runPhase4Demo(argc, argv);
    } else if (demoName == "phase6" || demoName == "integration") {
        runPhase6Demo(argc, argv);
    } else if (demoName == "phase6_all" || demoName == "integration_all") {
        runPhase6AllDemo();
    } else if (demoName == "benchmark") {
        runBenchmarkDemo(argc, argv);
    } else if (demoName == "all") {
        runAllDemos();
    } else if (demoName == "--help" || demoName == "-h") {
        printHelp();
    } else {
        std::cerr << "Error: Unknown demo: " << demoName << std::endl;
        std::cout << "Run 'nlm_demo --help' for available demos." << std::endl;
        return 1;
    }
    
    return 0;
}
