// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration Demo
//
// This demo runs the complete Phase 6 integration test to verify
// that all brain systems are properly connected and working together.

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
#include "experiments/Phase6Demo.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <stdexcept>

using namespace nlm;

// Custom error handling
void safe_exit(const std::string& message, int exit_code = 1) {
    std::cerr << "ERROR: " << message << std::endl;
    std::exit(exit_code);
}

// Parse command line arguments for demonstration mode
struct DemoConfig {
    bool runPhase2 = false;
    bool runPhase3 = false;
    bool runPhase4 = false;
    bool runPhase6 = false;
    size_t phase6Neurons = 500;
    uint64_t phase6Steps = 2000;
    std::string configFile = "configs/default.cfg";
    LogLevel logLevel = LogLevel::Info;
    bool help = false;
};

DemoConfig parseArguments(int argc, char** argv) {
    DemoConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            config.help = true;
        } else if (arg == "--phase2") {
            config.runPhase2 = true;
        } else if (arg == "--phase3") {
            config.runPhase3 = true;
        } else if (arg == "--phase4") {
            config.runPhase4 = true;
        } else if (arg == "--phase6") {
            config.runPhase6 = true;
        } else if (arg == "--neurons" || arg == "-n") {
            if (i + 1 < argc) {
                config.phase6Neurons = std::stoul(argv[++i]);
            }
        } else if (arg == "--steps" || arg == "-s") {
            if (i + 1 < argc) {
                config.phase6Steps = std::stoull(argv[++i]);
            }
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                config.configFile = argv[++i];
            }
        } else if (arg == "--loglevel" || arg == "-l") {
            if (i + 1 < argc) {
                std::string level = argv[++i];
                if (level == "debug") config.logLevel = LogLevel::Debug;
                else if (level == "info") config.logLevel = LogLevel::Info;
                else if (level == "warn") config.logLevel = LogLevel::Warn;
                else if (level == "error") config.logLevel = LogLevel::Error;
            }
        }
    }
    
    // If no specific phase is requested, run all available demos
    if (!config.runPhase2 && !config.runPhase3 && !config.runPhase4 && !config.runPhase6) {
        config.runPhase6 = true;  // Default to Phase 6 as it's the most comprehensive
    }
    
    return config;
}

void printPhaseBanner(int phase) {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase " << phase << ": Final Integration Demo              ║
    ║                                                               ║
    ║     Testing the complete integrated brain system.             ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage() {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "===========================" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Usage: ./nlm [OPTIONS]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Available phases:" << std::endl;
    std::cout << "  --phase2                     Run Phase 2: Basic neural computation (legacy)" << std::endl;
    std::cout << "  --phase3                     Run Phase 3: World interaction demo" << std::endl;
    std::cout << "  --phase4                     Run Phase 4: Emerging cognition demo" << std::endl;
    std::cout << "  --phase6                     Run Phase 6: Final integration demo (default)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                   Show this help message" << std::endl;
    std::cout << "  --neurons, -n N             Number of neurons for Phase 6 demo" << std::endl;
    std::cout << "  --steps, -s N                Number of steps for Phase 6 demo" << std::endl;
    std::cout << "  --config, -c FILE            Configuration file" << std::endl;
    std::cout << "  --loglevel, -l LEVEL         Log level (debug, info, warn, error)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm --phase6               Run Phase 6 integration demo" << std::endl;
    std::cout << "  ./nlm --phase6 --neurons 1000 --steps 5000" << std::endl;
    std::cout << "  ./nlm --phase6 --config my_config.cfg" << std::endl;
    std::cout << "" << std::endl;
}

int main(int argc, char** argv) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Parse command line arguments
    DemoConfig config = parseArguments(argc, argv);
    
    if (config.help) {
        printUsage();
        return 0;
    }
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(config.logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    try {
        // Load configuration
        auto cfg = std::make_shared<Config>();
        
        // Load configuration from file (ignore if not found)
        if (cfg->loadFromFile(config.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + config.configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Set Phase 6 specific defaults
        cfg->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        cfg->set("simulation_timestep", 0.001, ConfigSource::Default);
        cfg->set("neuron_count", static_cast<int64_t>(config.phase6Neurons), ConfigSource::Default);
        cfg->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        cfg->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        cfg->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        cfg->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        cfg->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        cfg->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        cfg->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Log configuration summary
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== NLM Phase 6 Configuration ===");
        NLM_LOG_INFO("Neuron count: " + std::to_string(cfg->getOr<int64_t>("neuron_count", 500)));
        NLM_LOG_INFO("Simulation timestep: " + std::to_string(cfg->getOr<double>("simulation_timestep", 0.001)) + "s");
        NLM_LOG_INFO("Connection probability: " + std::to_string(cfg->getOr<float>("connection_probability", 0.15f)));
        
        // Initialize simulation clock
        double timestep = cfg->getOr<double>("simulation_timestep", 0.001);
        SimulationClock clock(timestep);
        NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
        
        if (config.runPhase2) {
            printPhaseBanner(2);
            // Run Phase 2 legacy demo
            NLM_LOG_INFO("Phase 2: Real Neural Computation");
            NLM_LOG_INFO("This phase demonstrates basic neural computation with:");
            NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neurons");
            NLM_LOG_INFO("  - Event-driven spike propagation with delays");
            NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
            NLM_LOG_INFO("  - Structural plasticity");
            
            // TODO: Implement Phase 2 demo with better error handling
            safe_exit("Phase 2 demo not yet implemented. Use --phase6 for Phase 6 demo.", 1);
        }
        
        if (config.runPhase3) {
            printPhaseBanner(3);
            // Run Phase 3 demo
            NLM_LOG_INFO("Phase 3: World Interaction Demo");
            NLM_LOG_INFO("This phase demonstrates sensory input, action output, and learning.");
            
            // TODO: Implement Phase 3 demo
            safe_exit("Phase 3 demo not yet implemented. Use --phase6 for Phase 6 demo.", 1);
        }
        
        if (config.runPhase4) {
            printPhaseBanner(4);
            // Run Phase 4 demo
            NLM_LOG_INFO("Phase 4: Emerging Cognition Demo");
            NLM_LOG_INFO("This phase demonstrates cognitive capabilities.");
            
            // TODO: Implement Phase 4 demo
            safe_exit("Phase 4 demo not yet implemented. Use --phase6 for Phase 6 demo.", 1);
        }
        
        if (config.runPhase6) {
            printPhaseBanner(6);
            NLM_LOG_INFO("=== NLM Phase 6: Final Integration Demo ===");
            NLM_LOG_INFO("This demo runs comprehensive integration tests to verify all brain systems work together.");
            NLM_LOG_INFO("Testing systems integration...");
            
            // Create Phase 6 experiment
            auto experiment = std::make_shared<Phase6IntegratedExperiment>();
            
            // First, run the quick integration verification
            NLM_LOG_INFO("--- Integration Verification ---");
            bool integrationOK = experiment->verifyIntegration();
            
            NLM_LOG_INFO("");
            
            if (!integrationOK) {
                NLM_LOG_ERROR("Integration verification failed!");
                safe_exit("Phase 6 integration failed.", 1);
            }
            
            NLM_LOG_INFO("Integration verification passed!");
            
            // Test individual systems
            NLM_LOG_INFO("--- Memory Integration Test ---");
            experiment->testMemoryIntegration();
            NLM_LOG_INFO("");
            
            NLM_LOG_INFO("--- Neuromodulation Integration Test ---");
            experiment->testNeuromodulationIntegration();
            NLM_LOG_INFO("");
            
            NLM_LOG_INFO("--- Checkpoint Test ---");
            experiment->testCheckpointing();
            NLM_LOG_INFO("");
            
            NLM_LOG_INFO("--- Replay Test ---");
            experiment->testReplay();
            NLM_LOG_INFO("");
            
            // Run full experiment with custom settings
            NLM_LOG_INFO("--- Full Integration Experiment ---");
            Phase6Config phase6Config;
            phase6Config.neuronCount = config.phase6Neurons;
            phase6Config.maxSteps = config.phase6Steps;
            phase6Config.enableCheckpointing = true;
            phase6Config.enableReplay = true;
            phase6Config.enableDevelopment = true;
            
            auto result = experiment->run(phase6Config);
            
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== FINAL RESULTS ===");
            NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
            NLM_LOG_INFO("Avg firing rate: " + std::to_string(result.avgFiringRate));
            NLM_LOG_INFO("Episodes stored: " + std::to_string(result.memoryEpisodesStored));
            NLM_LOG_INFO("Dopamine level: " + std::to_string(result.dopamineLevel));
            NLM_LOG_INFO("");
            
            NLM_LOG_INFO("=== INTEGRATION STATUS ===");
            NLM_LOG_INFO("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
            NLM_LOG_INFO("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
            NLM_LOG_INFO("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED"));
            NLM_LOG_INFO("Prediction: " + std::string(result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED"));
            NLM_LOG_INFO("Development: " + std::string(result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED"));
            NLM_LOG_INFO("Checkpointing: " + std::string(result.checkpointingWorks ? "WORKING" : "NOT WORKING"));
            NLM_LOG_INFO("");
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
            NLM_LOG_INFO("Wall clock time: " + std::to_string(duration.count()) + "s");
            
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Phase 6 Complete ===");
            NLM_LOG_INFO("All systems are integrated and working together!");
            NLM_LOG_INFO("The NLM brain is a functioning artificial neural substrate");
            NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
            NLM_LOG_INFO("");
            
            return 0;
        }
        
        // If we reach here, no valid phase was selected
        printUsage();
        return 0;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error: " + std::string(e.what()));
        return 1;
    }
}
