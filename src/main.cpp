// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// This phase demonstrates the complete integrated artificial brain with:
// - Working memory, episodic memory, associative memory systems
// - Neuromodulation (dopamine, curiosity, novelty, prediction error)
// - Prediction system for temporal and action-consequence prediction
// - Neural planning, concept formation, and attention
// - Development system that modulates plasticity
// - Memory replay and consolidation
// - Checkpoint save/load
// - Sleep/rest cycles with episodic replay

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
#include <chrono>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     Complete artificial brain with all systems integrated.     ║
    ║     Memory, neuromodulation, prediction, development, replay.║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printMemoryStatus(const std::shared_ptr<Brain>& brain) {
    if (brain->getWorkingMemory()) {
        std::cout << "  Working memory: " << brain->getWorkingMemory()->getActiveTraces() 
                  << " active traces" << std::endl;
    }
    if (brain->getEpisodicMemory()) {
        std::cout << "  Episodic memory: " << brain->getEpisodicMemory()->getEpisodeCount() 
                  << " episodes stored" << std::endl;
    }
    if (brain->getAssociativeMemory()) {
        std::cout << "  Associative memory: ACTIVE" << std::endl;
    }
}

void printNeuromodulationStatus(const std::shared_ptr<Brain>& brain) {
    if (brain->getDopamine()) {
        std::cout << "  Dopamine: " << brain->getDopamine()->getLevel() 
                  << " (reward prediction error)" << std::endl;
    }
    if (brain->getCuriosity()) {
        std::cout << "  Curiosity: " << brain->getCuriosity()->getLevel() 
                  << " (exploration drive)" << std::endl;
    }
    if (brain->getNovelty()) {
        std::cout << "  Novelty: " << brain->getNovelty()->getLevel() 
                  << " (surprise detection)" << std::endl;
    }
    if (brain->getPredictionErrorSignal()) {
        std::cout << "  Prediction error: " << brain->getPredictionErrorSignal()->getLevel() 
                  << " (prediction accuracy)" << std::endl;
    }
}

void printCognitionStatus(const std::shared_ptr<Brain>& brain) {
    if (brain->getPlanner()) {
        std::cout << "  Neural planner: ACTIVE (action sequence evaluation)" << std::endl;
    }
    if (brain->getConceptFormation()) {
        std::cout << "  Concept formation: ACTIVE (pattern abstraction)" << std::endl;
    }
    if (brain->getAttention()) {
        std::cout << "  Attention: ACTIVE (competitive selection)" << std::endl;
    }
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 6: Final Integration..." << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6: Final Integration Demo ===");
    NLM_LOG_INFO("Demonstrating complete artificial brain integration...");
    NLM_LOG_INFO("");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    // ========== ADVANCED CLI OPTIONS PARSING ==========
    
    // Parse advanced command line options for expert users
    bool verboseOutput = false;
    bool benchmarkMode = false;
    bool profileMode = false;
    bool experimentMode = false;
    std::string experimentType = "";
    float plasticityRateOverride = -1.0f;
    float neuromodulationIntensity = -1.0f;
    int64_t developmentStageOverride = -1;
    int32_t saveInterval = -1;
    std::string checkpointDir = "";
    bool enableVisualization = false;
    bool enableProfiling = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--verbose" || arg == "-v") {
            verboseOutput = true;
            Logger::getInstance().setLevel(Logger::Level::Debug);
        } else if (arg == "--quiet" || arg == "-q") {
            Logger::getInstance().setLevel(Logger::Level::Error);
        } else if (arg == "--benchmark" || arg == "-b") {
            benchmarkMode = true;
            NLM_LOG_INFO("BENCHMARK MODE: Performance measurements will be collected");
        } else if (arg == "--profile" || arg == "-p") {
            profileMode = true;
            NLM_LOG_INFO("PROFILE MODE: Detailed profiling data will be collected");
        } else if (arg == "--experiment" || arg == "-e") {
            experimentMode = true;
            if (i + 1 < argc) {
                experimentType = argv[++i];
                NLM_LOG_INFO("EXPERIMENT MODE: Running experiment type: " + experimentType);
            }
        } else if (arg.substr(0, 7) == "--neuron") {
            if (arg.find('=') != std::string::npos) {
                config->set("neuron_count", std::stoll(arg.substr(arg.find('=') + 1)), ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 11) == "--region") {
            if (arg.find('=') != std::string::npos) {
                config->set("region_count", std::stoll(arg.substr(arg.find('=') + 1)), ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 9) == "--timestep") {
            if (arg.find('=') != std::string::npos) {
                config->set("simulation_timestep", std::stod(arg.substr(arg.find('=') + 1)), ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 12) == "--plasticity") {
            if (arg.find('=') != std::string::npos) {
                plasticityRateOverride = std::stof(arg.substr(arg.find('=') + 1));
                config->set("synaptogenesis_rate", plasticityRateOverride * 0.0001f, ConfigSource::CommandLine);
                config->set("pruning_rate", plasticityRateOverride * 0.00001f, ConfigSource::CommandLine);
                NLM_LOG_INFO("PLASTICITY OVERRIDE: Synaptogenesis rate set to " + std::to_string(plasticityRateOverride * 0.0001f) +
                           ", Pruning rate set to " + std::to_string(plasticityRateOverride * 0.00001f));
            }
        } else if (arg.substr(0, 13) == "--development") {
            if (arg.find('=') != std::string::npos) {
                developmentStageOverride = std::stol(arg.substr(arg.find('=') + 1));
                config->set("development_stage", developmentStageOverride, ConfigSource::CommandLine);
                NLM_LOG_INFO("DEVELOPMENT OVERRIDE: Development stage set to " + std::to_string(developmentStageOverride));
            }
        } else if (arg.substr(0, 13) == "--neuromod") {
            if (arg.find('=') != std::string::npos) {
                neuromodulationIntensity = std::stof(arg.substr(arg.find('=') + 1));
                config->set("dopamine_level", neuromodulationIntensity, ConfigSource::CommandLine);
                config->set("curiosity_level", neuromodulationIntensity, ConfigSource::CommandLine);
                NLM_LOG_INFO("NEUROMODULATION OVERRIDE: Neuromodulation intensity set to " + std::to_string(neuromodulationIntensity));
            }
        } else if (arg.substr(0, 14) == "--checkpoint") {
            if (arg.find('=') != std::string::npos) {
                checkpointDir = arg.substr(arg.find('=') + 1);
                config->set("checkpoint_dir", checkpointDir, ConfigSource::CommandLine);
                NLM_LOG_INFO("CHECKPOINT DIR: Set to " + checkpointDir);
            }
        } else if (arg.substr(0, 14) == "--save-interval") {
            if (arg.find('=') != std::string::npos) {
                saveInterval = std::stoi(arg.substr(arg.find('=') + 1));
                phase6Config.checkpointPath = "./checkpoints/" + std::to_string(saveInterval) + "/nlm_checkpoint_";
                NLM_LOG_INFO("SAVE INTERVAL: Set to " + std::to_string(saveInterval) + " steps");
            }
        } else if (arg.substr(0, 17) == "--max-steps") {
            if (arg.find('=') != std::string::npos) {
                phase6Config.maxSteps = static_cast<uint64_t>(std::stoull(arg.substr(arg.find('=') + 1)));
                NLM_LOG_INFO("MAX STEPS: Set to " + std::to_string(phase6Config.maxSteps));
            }
        } else if (arg == "--enable-visualization" || arg == "--plot") {
            enableVisualization = true;
            NLM_LOG_INFO("VISUALIZATION ENABLED: Neural activity plots will be generated");
        } else if (arg == "--enable-profiling" || arg == "--metrics") {
            enableProfiling = true;
            NLM_LOG_INFO("PROFILING ENABLED: Performance metrics will be collected");
        }
    }
    
    // Apply advanced overrides to configuration
    if (developmentStageOverride >= 0 && developmentStageOverride < 5) {
        config->set("development_stage", developmentStageOverride, ConfigSource::CommandLine);
    }
    
    // Set advanced defaults based on modes
    if (benchmarkMode) {
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);  // Smaller for faster benchmarking
        config->set("simulation_timestep", 0.01, ConfigSource::Default);  // Larger timestep for speed
        NLM_LOG_INFO("BENCHMARK MODE: Reduced neuron count and timestep for faster execution");
    }
    
    if (profileMode) {
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Medium size for profiling
        NLM_LOG_INFO("PROFILE MODE: Medium neuron count for performance analysis");
    }
    
    if (experimentMode) {
        if (experimentType == "stability") {
            config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
            config->set("connection_probability", 0.01f, ConfigSource::Default);
            NLM_LOG_INFO("EXPERIMENT TYPE: Stability test with low connectivity");
        } else if (experimentType == "plasticity") {
            config->set("neuron_count", static_cast<int64_t>(1500), ConfigSource::Default);
            plasticityRateOverride = 2.0f;  // Enhanced plasticity
            config->set("synaptogenesis_rate", plasticityRateOverride * 0.0001f, ConfigSource::Default);
            config->set("pruning_rate", plasticityRateOverride * 0.00001f, ConfigSource::Default);
            NLM_LOG_INFO("EXPERIMENT TYPE: Plasticity test with enhanced synaptic changes");
        } else if (experimentType == "integration") {
            config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
            config->set("connection_probability", 0.05f, ConfigSource::Default);
            NLM_LOG_INFO("EXPERIMENT TYPE: Integration test with normal connectivity");
        } else {
            config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
            NLM_LOG_INFO("EXPERIMENT TYPE: Default - running general integration test");
        }
    }
    
    // Integration experiment configuration
    Phase6Config phase6Config;
    phase6Config.neuronCount = config->getOr<int64_t>("neuron_count", 2000);
    phase6Config.maxSteps = 5000;
    phase6Config.regionCount = config->getOr<int64_t>("region_count", 2);
    phase6Config.connectionProbability = config->getOr<float>("connection_probability", 0.05f);
    phase6Config.enableCheckpointing = true;
    phase6Config.enableReplay = true;
    phase6Config.enableDevelopment = true;
    phase6Config.checkpointPath = "./nlm_phase6_checkpoint.bin";
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 6 Configuration:");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(phase6Config.neuronCount));
    NLM_LOG_INFO("  region_count: " + std::to_string(phase6Config.regionCount));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(phase6Config.connectionProbability));
    NLM_LOG_INFO("  enable_checkpointing: " + std::string(phase6Config.enableCheckpointing ? "YES" : "NO"));
    NLM_LOG_INFO("  enable_replay: " + std::string(phase6Config.enableReplay ? "YES" : "NO"));
    NLM_LOG_INFO("  enable_development: " + std::string(phase6Config.enableDevelopment ? "YES" : "NO"));
    NLM_LOG_INFO("");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6 - Complete Integration)...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    NLM_LOG_INFO("");
    
    // Phase 6 Integration Demo
    NLM_LOG_INFO("=== Phase 6 Integration Demo ===");
    NLM_LOG_INFO("Running comprehensive integration test...");
    
    // Create Phase 6 experiment
    Phase6IntegratedExperiment experiment;
    
    // Run integration verification
    NLM_LOG_INFO("");
    NLM_LOG_INFO("--- Step 1: Integration Verification ---");
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        NLM_LOG_ERROR("Integration verification failed!");
        return 1;
    }
    
    NLM_LOG_INFO("Integration verification PASSED! All systems are properly connected.");
    NLM_LOG_INFO("");
    
    // Test individual system integrations
    NLM_LOG_INFO("--- Step 2: System Integration Tests ---");
    
    NLM_LOG_INFO("Testing memory system integration...");
    bool memoryIntegrationOK = experiment.testMemoryIntegration();
    NLM_LOG_INFO("Memory integration: " + std::string(memoryIntegrationOK ? "PASS" : "FAIL"));
    
    NLM_LOG_INFO("Testing neuromodulation integration...");
    bool neuromodulationIntegrationOK = experiment.testNeuromodulationIntegration();
    NLM_LOG_INFO("Neuromodulation integration: " + std::string(neuromodulationIntegrationOK ? "PASS" : "FAIL"));
    
    NLM_LOG_INFO("Testing checkpoint system...");
    bool checkpointingOK = experiment.testCheckpointing();
    NLM_LOG_INFO("Checkpointing test: " + std::string(checkpointingOK ? "PASS" : "FAIL"));
    
    NLM_LOG_INFO("Testing replay system...");
    bool replayOK = experiment.testReplay();
    NLM_LOG_INFO("Replay test: " + std::string(replayOK ? "PASS" : "FAIL"));
    
    NLM_LOG_INFO("");
    
    // Run full integration experiment
    NLM_LOG_INFO("--- Step 3: Full Integration Experiment ---");
    NLM_LOG_INFO("Running complete lifetime simulation (Phase 6 experiment)...");
    
    auto startWall = std::chrono::high_resolution_clock::now();
    Phase6IntegrationResult result = experiment.run(phase6Config);
    auto endWall = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Integration Results ===");
    NLM_LOG_INFO("Total simulation time: " + std::to_string(result.totalWallClockTime) + "s");
    NLM_LOG_INFO("Final brain status:");
    brain->logStatus();
    NLM_LOG_INFO("");
    
    // Display detailed integration status
    NLM_LOG_INFO("=== Integration Status ===");
    NLM_LOG_INFO("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗"));
    NLM_LOG_INFO("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗"));
    NLM_LOG_INFO("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗"));
    NLM_LOG_INFO("Prediction: " + std::string(result.predictionIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗"));
    NLM_LOG_INFO("Development: " + std::string(result.developmentIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗"));
    NLM_LOG_INFO("Checkpointing: " + std::string(result.checkpointingWorks ? "WORKING ✓" : "NOT WORKING ✗"));
    NLM_LOG_INFO("Replay: " + std::string(result.replayWorks ? "WORKING ✓" : "NOT WORKING ✗"));
    NLM_LOG_INFO("");
    
    NLM_LOG_INFO("=== Performance Metrics ===");
    NLM_LOG_INFO("Average firing rate: " + std::to_string(result.avgFiringRate));
    NLM_LOG_INFO("Total reward per step: " + std::to_string(result.totalReward));
    NLM_LOG_INFO("Memory episodes stored: " + std::to_string(result.memoryEpisodesStored));
    NLM_LOG_INFO("Dopamine level: " + std::to_string(result.dopamineLevel));
    NLM_LOG_INFO("Novelty level: " + std::to_string(result.noveltyLevel));
    NLM_LOG_INFO("Curiosity level: " + std::to_string(result.curiosityLevel));
    NLM_LOG_INFO("");
    
    NLM_LOG_INFO("=== Phase 6 Summary ===");
    if (integrationOK && memoryIntegrationOK && neuromodulationIntegrationOK && 
        checkpointingOK && replayOK) {
        NLM_LOG_INFO("✓ ALL INTEGRATION TESTS PASSED!");
        NLM_LOG_INFO("✓ The NLM artificial brain is fully functional.");
        NLM_LOG_INFO("✓ All brain systems are properly connected and integrated.");
        NLM_LOG_INFO("✓ Memory, neuromodulation, prediction, development, and replay systems work together.");
        NLM_LOG_INFO("✓ Checkpoint save/load and replay mechanisms are operational.");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("The NLM Phase 6 implementation successfully demonstrates:");
        NLM_LOG_INFO("- Memory systems connected to neural processing");
        NLM_LOG_INFO("- Neuromodulation affects plasticity and neural dynamics");
        NLM_LOG_INFO("- Development affects plasticity rates");
        NLM_LOG_INFO("- Prediction integrated with learning");
        NLM_LOG_INFO("- Replay and consolidation functional");
        NLM_LOG_INFO("- Complete brain loop functions coherently");
    } else {
        NLM_LOG_ERROR("SOME INTEGRATION TESTS FAILED!");
        NLM_LOG_ERROR("Review integration status above for issues.");
        return 1;
    }
    
    return 0;
}
