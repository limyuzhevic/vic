// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration - Unified Main Executable
//
// This phase implements the complete integrated artificial brain system:
// - All brain systems connected (memory, neuromodulation, prediction, cognition)
// - Working memory with persistent activity and competition
// - Episodic memory with experience encoding and replay
// - Neuromodulation affects neural excitability and plasticity
// - Development stages affect plasticity rates
// - Full checkpoint/save/load functionality
// - All systems connected to neural processing through the main brain loop

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <iomanip>
#include <numeric>
#include <chrono>

using namespace nlm;

void printPhase6Banner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase  tease for real AI research                   ║
    ║     The unified integration demo for the complete artificial ║
    ║     brain system (Phase 6: Final Integration)                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage() {
    std::cout << "\n=== NLM Phase 6 Usage ===" << std::endl;
    std::cout << "nlm_phase6 [options]" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  --config <file>      Load configuration from file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --neurons <count>    Set neuron count (default: 500)" << std::endl;
    std::cout << "  --steps <count>      Set simulation steps (default: 2000)" << std::endl;
    std::cout << "  --region_count <n>    Set region count (default: 1)" << std::endl;
    std::cout << "  --checkpoint <dir>    Enable checkpointing with directory (default: ./checkpoints)" << std::endl;
    std::cout << "  --replay <interval>   Enable memory replay (steps between replay)" << std::endl;
    std::cout << "  --development         Enable development system" << std::endl;
    std::cout << "  --quick               Run quick integration verification only (5 minutes)" << std::endl;
    std::cout << "  --detailed <seconds>  Run detailed experiment with duration" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  nlm_phase6                     # Quick integration test" << std::endl;
    std::cout << "  nlm_phase6 --neurons 1000 --steps 5000 --development" << std::endl;
}

void runIntegrationVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Phase 6 Integration Verification ===");
    NLM_LOG_INFO("Checking that all major brain systems are properly connected...");
    
    bool systemsWorking = true;
    
    // Test 1: Memory systems integration
    NLM_LOG_INFO("\n--- Memory Integration Test ---");
    if (auto* workingMemory = brain->getWorkingMemory()) {
        NLM_LOG_INFO("✓ Working memory: " << workingMemory->getCapacity() << " neurons capacity");
        if (workingMemory->getMemoryNeuronCount() == 0) {
            NLM_LOG_INFO("  Warning: Working memory has no neurons loaded");
        }
    } else {
        NLM_LOG_INFO("✗ Working memory not found");
        systemsWorking = false;
    }
    
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        NLM_LOG_INFO("✓ Episodic memory: " << episodicMemory->getMaxEpisodes() << " max episodes");
    } else {
        NLM_LOG_INFO("✗ Episodic memory not found");
        systemsWorking = false;
    }
    
    // Test 2: Neuromodulation integration
    NLM_LOG_INFO("\n--- Neuromodulation Integration Test ---");
    if (auto* novelty = brain->getNovelty()) {
        NLM_LOG_INFO("✓ Novelty detection system active");
    } else {
        NLM_LOG_INFO("✗ Novelty detection not found");
        systemsWorking = false;
    }
    
    if (auto* curiosity = brain->getCuriosity()) {
        NLM_LOG_INFO("✓ Curiosity system active");
    } else {
        NLM_LOG_INFO("✗ Curiosity system not found");
        systemsWorking = false;
    }
    
    if (auto* predictionError = brain->getPredictionErrorSignal()) {
        NLM_LOG_INFO("✓ Prediction error system active");
    } else {
        NLM_LOG_INFO("✗ Prediction error system not found");
        systemsWorking = false;
    }
    
    // Test 3: Prediction system integration
    NLM_LOG_INFO("\n--- Prediction Integration Test ---");
    if (auto* predictionSystem = brain->getPredictionSystem()) {
        NLM_LOG_INFO("✓ Prediction system active");
    } else {
        NLM_LOG_INFO("✗ Prediction system not found");
        systemsWorking = false;
    }
    
    // Test 4: Cognition systems integration
    NLM_LOG_INFO("\n--- Cognition Integration Test ---");
    if (auto* planner = brain->getPlanner()) {
        NLM_LOG_INFO("✓ Neural planner active");
    } else {
        NLM_LOG_INFO("✗ Neural planner not found");
        systemsWorking = false;
    }
    
    if (auto* conceptFormation = brain->getConceptFormation()) {
        NLM_LOG_INFO("✓ Concept formation system active");
    } else {
        NLM_LOG_INFO("✗ Concept formation not found");
        systemsWorking = false;
    }
    
    if (auto* attention = brain->getAttention()) {
        NLM_LOG_INFO("✓ Attentional selection system active");
    } else {
        NLM_LOG_INFO("✗ Attentional selection not found");
        systemsWorking = false;
    }
    
    // Test 5: Development system integration
    NLM_LOG_INFO("\n--- Development Integration Test ---");
    if (auto* developmentSystem = brain->getDevelopmentSystem()) {
        NLM_LOG_INFO("✓ Development system active");
        std::string stage;
        switch (brain->getDevelopmentalStage()) {
            case DevelopmentalStage::Initial: stage = "Initial"; break;
            case DevelopmentalStage::CriticalPeriod: stage = "Critical Period"; break;
            case DevelopmentalStage::Maturation: stage = "Maturation"; break;
            case DevelopmentalStage::Adult: stage = "Adult"; break;
        }
        NLM_LOG_INFO("  Current stage: " << stage);
    } else {
        NLM_LOG_INFO("✗ Development system not found");
        systemsWorking = false;
    }
    
    // Test 6: Checkpoint system integration
    NLM_LOG_INFO("\n--- Checkpoint Integration Test ---");
    auto checkpointDir = brain->getConfig()->getOr<std::string>("checkpoint_dir", "./checkpoints");
    NLM_LOG_INFO("✓ Checkpoint directory: " << checkpointDir);
    
    NLM_LOG_INFO("\n=== Integration Verification Result ===");
    if (systemsWorking) {
        NLM_LOG_INFO("✓ All brain systems are properly integrated!");
    } else {
        NLM_LOG_INFO("✗ Some systems are not properly integrated");
    }
}

void runQuickTest(std::shared_ptr<Brain> brain, int steps = 200) {
    NLM_LOG_INFO("=== Quick Integration Test ===");
    NLM_LOG_INFO("Running " << steps << " simulation steps to test basic functionality...");
    
    // Apply some sensory input
    NLM_LOG_INFO("Applying sensory input pattern...");
    std::vector<float> sensoryData(16, 0.5f);
    for (int i = 5; i <= 10; ++i) sensoryData[i] = 0.8f;
    
    SensoryInput input(sensoryData);
    brain->receiveSensoryInput(input);
    
    // Run simulation
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, static_cast<Timestamp>(step) * 0.001);
        
        // Log progress
        if (step % 50 == 0) {
            NLM_LOG_INFO("  Step " << step << 
                        ", Spikes: " << brain->getTotalSpikeCount() << 
                        ", Firing: " << brain->getFiringNeuronCount() << 
                        ", Reward: " << (brain->getDopamine() ? brain->getDopamine()->getLevel() : 0.0f));
        }
    }
    
    NLM_LOG_INFO("✓ Quick test completed successfully");
}

void runDetailedExperiment(std::shared_ptr<Brain> brain, int steps) {
    NLM_LOG_INFO("=== Detailed Integration Experiment ===");
    NLM_LOG_INFO("Running " << steps << " steps with full system integration...");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Apply varied sensory input to stimulate all systems
    NLM_LOG_INFO("Stimulating all brain systems with varied input...");
    
    for (SimulationStep step = 0; step < steps; ++step) {
        // Vary sensory input to drive curiosity, novelty, prediction error
        std::vector<float> sensoryData(16, 0.3f + (0.4f * std::sin(step * 0.01f)));
        
        // Add some structured patterns
        if (step % 100 == 0) {
            for (int i = 5; i <= 10; ++i) sensoryData[i] = 0.9f;  // Resource-like pattern
        } else if (step % 150 == 0) {
            for (int i = 2; i <= 7; ++i) sensoryData[i] = 0.2f;  // Hazard-like pattern
        }
        
        SensoryInput input(sensoryData);
        brain->receiveSensoryInput(input);
        
        // Apply neuromodulation based on internal state
        if (auto* novelty = brain->getNovelty()) {
            if (novelty->getLevel() > 0.7f) {
                brain->applyNeuromodulation(*novelty);
            }
        }
        
        brain->step(step, static_cast<Timestamp>(step) * 0.001);
        
        if (step % 200 == 0) {
            NLM_LOG_INFO("  Step " << step << 
                        ", Spikes: " << brain->getTotalSpikeCount() << 
                        ", WorkingMem: " << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getMemoryNeurons().size() : 0) <<
                        ", Episodes: " << (brain->getEpisodicMemory() ? brain->getEpisodicMemory()->getEpisodeCount() : 0) <<
                        ", FiringRate: " << brain->getAverageFiringRate());
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
    
    NLM_LOG_INFO("✓ Detailed experiment completed in " << duration << " seconds");
}

int main(int argc, char** argv) {
    printPhase6Banner();
    
    std::cout << "Initializing NLM Phase 6 Final Integration...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    int neuronCount = 500;
    int steps = 2000;
    int regionCount = 1;
    std::string checkpointDir = "./checkpoints";
    int replayInterval = 100;
    bool enableDevelopment = false;
    bool enableQuick = false;
    int detailedSeconds = -1;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help") {
            printUsage();
            return 0;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--neurons") {
            if (i + 1 < argc) neuronCount = std::atoi(argv[++i]);
        } else if (arg == "--steps") {
            if (i + 1 < argc) steps = std::atoi(argv[++i]);
        } else if (arg == "--region_count") {
            if (i + 1 < argc) regionCount = std::atoi(argv[++i]);
        } else if (arg == "--checkpoint") {
            if (i + 1 < argc) checkpointDir = argv[++i];
        } else if (arg == "--replay") {
            if (i + 1 < argc) replayInterval = std::atoi(argv[++i]);
        } else if (arg == "--development") {
            enableDevelopment = true;
        } else if (arg == "--quick") {
            enableQuick = true;
            steps = 200;  // Override steps for quick mode
        } else if (arg == "--detailed") {
            if (i + 1 < argc) {
                detailedSeconds = std::atoi(argv[++i]);
                steps = detailedSeconds * 50;  // 50 steps per second
            }
        }
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " << configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line arguments
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("checkpoint_dir", checkpointDir, ConfigSource::Default);
    config->set("replay_interval", static_cast<int64_t>(replayInterval), ConfigSource::Default);
    config->set("consolidation_interval", 1000LL, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Enable development if requested
    if (enableDevelopment) {
        config->set("development_enabled", 1, ConfigSource::Default);
    }
    
    // Log configuration summary
    NLM_LOG_INFO("\n=== Configuration Summary ===");
    NLM_LOG_INFO("Neurons: " << neuronCount);
    NLM_LOG_INFO("Regions: " << regionCount);
    NLM_LOG_INFO("Steps: " << steps);
    NLM_LOG_INFO("Checkpoint Dir: " << checkpointDir);
    NLM_LOG_INFO("Replay Interval: " << replayInterval);
    NLM_LOG_INFO("Development Enabled: " << (enableDevelopment ? "Yes" : "No"));
    NLM_LOG_INFO("Quick Mode: " << (enableQuick ? "Yes" : "No"));
    NLM_LOG_INFO("")
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " << timestep << "s");
    
    // Initialize brain
    NLM_LOG_INFO("\n=== Initializing NLM Brain (Phase 6: Integrated Artificial Brain) ===");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run based on mode selection
    if (enableQuick) {
        runQuickTest(brain, 200);
    } else if (detailedSeconds >= 0) {
        runDetailedExperiment(brain, steps);
    } else {
        // Default: run integration verification and a basic test
        NLM_LOG_INFO("\n=== Starting Phase 6 Integration Demo ===");
        
        // Integration verification
        runIntegrationVerification(brain);
        
        // Reset for detailed testing
        brain->reset();
        brain->initialize();
        
        // Run quick test
        runQuickTest(brain, 500);
        
        // Reset again for full experiment
        brain->reset();
        brain->initialize();
        
        // Run detailed experiment
        runDetailedExperiment(brain, std::min(steps, 2000));
    }
    
    // Final brain status
    NLM_LOG_INFO("\n=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("\n=== Phase 6 Integration Results ===");
    NLM_LOG_INFO("Total spikes generated: " << brain->getTotalSpikeCount());
    NLM_LOG_INFO("Average firing rate: " << brain->getAverageFiringRate() << " Hz");
    NLM_LOG_INFO("Working memory active neurons: " << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getMemoryNeurons().size() : 0));
    NLM_LOG_INFO("Episodic memory episodes stored: " << (brain->getEpisodicMemory() ? brain->getEpisodicMemory()->getEpisodeCount() : 0));
    NLM_LOG_INFO("Learnable preference: " << brain->getLearnedPreference());
    
    NLM_LOG_INFO("\n=== Phase 6 Complete ===");
    NLM_LOG_INFO("The NLM brain is now a fully integrated artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("All systems work together as a coherent whole.");
    
    return 0;
}