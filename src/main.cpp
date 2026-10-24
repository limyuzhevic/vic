// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration - Modern Main Executable
//
// This is the primary entry point for the completed NLM Phase 6 system.
// It provides a comprehensive interface for running integration tests,
// demonstrations, and individual system validation with modern C++20 features.

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <thread>

using namespace nlm;

void printWelcomeBanner() {
    std::cout << R"(
    ╔══════════════════════════════════════════════════════════════════╗
    ║                                                              ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                              ║
    ║     Phase 6: Final Integration                                 ║
    ║                                                              ║
    ║     Complete artificial brain system with all subsystems      ║
    ║     integrated and functional.                                 ║
    ║                                                              ║
    ╚══════════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --mode MODE        Operation mode: demo, integration, minimal, benchmark (default: demo)" << std::endl;
    std::cout << "  --neurons N        Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps S          Number of simulation steps (default: 1000)" << std::endl;
    std::cout << "  --regions R        Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --checkpoint PATH   Enable checkpointing with specified path" << std::endl;
    std::cout << "  --no-dev           Disable developmental processes" << std::endl;
    std::cout << "  --no-plasticity    Disable synaptic plasticity" << std::endl;
    std::cout << "  --verbose          Enable verbose logging" << std::endl;
    std::cout << "  --help, -h         Show this help message" << std::endl;
    std::cout << "  --version, -v      Show version information" << std::endl;
}

void printVersion() {
    std::cout << "NLM Phase 6 Final Integration v0.1.0" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
}

class NLMCommander {
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
    bool plasticityEnabled_;
    bool developmentEnabled_;
    
public:
    NLMCommander() : plasticityEnabled_(true), developmentEnabled_(true) {
        // Initialize logger
        logger_ = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger_->addLogger(consoleLogger);
        Logger::setGlobal(logger_);
    }
    
    bool initialize(const Phase6Config& phaseConfig) {
        NLM_LOG_INFO("=== NLM Commander Initialization ===");
        
        // Create configuration
        config_ = std::make_shared<Config>();
        
        // Set configuration parameters
        config_->set("neuron_count", phaseConfig.neuronCount);
        config_->set("region_count", phaseConfig.regionCount);
        config_->set("connection_probability", 0.1f);
        
        // Plasticity settings
        config_->set("stdp_ltp_weight", 0.01f);
        config_->set("stdp_ltd_weight", 0.012f);
        config_->set("stdp_tau", 20.0f);
        config_->set("synaptogenesis_rate", 0.0001f);
        config_->set("pruning_rate", 0.00001f);
        
        // Development settings
        config_->set("critical_period_duration", 100.0f);
        config_->set("maturation_rate", 0.001f);
        
        // Neuromodulation settings
        config_->set("dopamine_scale", 1.0f);
        config_->set("curiosity_threshold", 0.5f);
        config_->set("novelty_sensitivity", 0.3f);
        
        NLM_LOG_INFO("Configuration created with " + std::to_string(phaseConfig.neuronCount) + " neurons");
        
        // Initialize brain
        NLM_LOG_INFO("Initializing brain...");
        brain_ = std::make_shared<Brain>(config_);
        
        if (!brain_->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        // Create environment
        NLM_LOG_INFO("Creating environment...");
        world_ = std::make_shared<SimpleWorld>();
        world_->initialize(16, 16);
        
        // Initialize agent
        NLM_LOG_INFO("Initializing agent...");
        agent_ = std::make_shared<AgentBrain>(brain_);
        agent_->initialize(*world_);
        
        // Configure agent subsystems
        agent_->enableRewardModulation(true);
        agent_->enableStructuralPlasticity(plasticityEnabled_);
        agent_->enableDevelopment(developmentEnabled_);
        agent_->enableCuriosity(true);
        agent_->enableNoveltyDetection(true);
        
        NLM_LOG_INFO("=== Initialization Complete ===");
        brain_->logStatus();
        return true;
    }
    
    void setPlasticity(bool enabled) {
        plasticityEnabled_ = enabled;
        if (agent_) {
            agent_->enableStructuralPlasticity(enabled);
        }
    }
    
    void setDevelopment(bool enabled) {
        developmentEnabled_ = enabled;
        if (agent_) {
            agent_->enableDevelopment(enabled);
        }
    }
    
    bool runDemonstration(uint64_t steps) {
        NLM_LOG_INFO("=== Starting Demonstration Run ===");
        NLM_LOG_INFO("Running " + std::to_string(steps) + " simulation steps");
        
        float totalReward = 0.0f;
        float totalFiringRate = 0.0f;
        size_t firingCount = 0;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (uint64_t step = 0; step < steps; ++step) {
            // Update environment
            world_->update(0.001);
            
            // Get sensory input
            SensoryPercept percept = world_->observe(brain_->getRegions()[0].get());
            
            // Process sensory input
            agent_->processSensoryInput(percept);
            
            // Run brain step
            brain_->step(step, step * 0.001);
            
            // Get motor command
            MotorCommand cmd = agent_->decodeMotorCommand();
            
            // Apply action
            world_->applyAction(brain_->getRegions()[0].get(), cmd);
            
            // Compute reward
            float reward = world_->computeReward(brain_->getRegions()[0].get());
            totalReward += reward;
            
            // Apply reward modulation
            agent_->applyRewardModulation(reward, 0.0f);
            
            // Update development
            if (developmentEnabled_) {
                agent_->updateDevelopment(0.001);
            }
            
            // Collect metrics
            totalFiringRate += brain_->getAverageFiringRate();
            if (brain_->getFiringNeuronCount() > 0) firingCount++;
            
            // Progress reporting
            if (step % 200 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           " | Reward: " + std::to_string(totalReward / (step + 1)) +
                           " | Firing: " + std::to_string(brain_->getAverageFiringRate()) +
                           " | WorkingMem: " + std::to_string(brain_->getWorkingMemory() ? 
                               brain_->getWorkingMemory()->getActiveTraces() : 0));
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(endTime - startTime).count();
        
        // Calculate final metrics
        float avgFiringRate = firingCount > 0 ? totalFiringRate / firingCount : 0.0f;
        
        NLM_LOG_INFO("=== Demonstration Complete ===");
        NLM_LOG_INFO("Duration: " + std::to_string(elapsed) + "s");
        NLM_LOG_INFO("Average Reward: " + std::to_string(totalReward / steps));
        NLM_LOG_INFO("Average Firing Rate: " + std::to_string(avgFiringRate));
        NLM_LOG_INFO("Working Memory Traces: " + std::to_string(brain_->getWorkingMemory() ? 
            brain_->getWorkingMemory()->getActiveTraces() : 0));
        NLM_LOG_INFO("Episodic Memory Episodes: " + std::to_string(brain_->getEpisodicMemory() ? 
            brain_->getEpisodicMemory()->getEpisodeCount() : 0));
        NLM_LOG_INFO("Dopamine Level: " + std::to_string(agent_->getNeuromodulationLevel()));
        NLM_LOG_INFO("Curiosity Level: " + std::to_string(agent_->getCuriosityLevel()));
        NLM_LOG_INFO("Novelty Level: " + std::to_string(agent_->getNoveltyLevel()));
        
        return true;
    }
    
    bool runIntegrationTest() {
        NLM_LOG_INFO("=== Running Integration Test ===");
        
        Phase6IntegratedExperiment experiment;
        Phase6Config config;
        config.neuronCount = 200;
        config.maxSteps = 500;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        auto result = experiment.run(config);
        
        NLM_LOG_INFO("=== Integration Test Results ===");
        NLM_LOG_INFO("Working Memory Integrated: " + std::string(result.memoryWorkingMemoryIntegrated ? "YES" : "NO"));
        NLM_LOG_INFO("Episodic Memory Integrated: " + std::string(result.memoryEpisodicMemoryIntegrated ? "YES" : "NO"));
        NLM_LOG_INFO("Neuromodulation Integrated: " + std::string(result.neuromodulationIntegrated ? "YES" : "NO"));
        NLM_LOG_INFO("Prediction Integrated: " + std::string(result.predictionIntegrated ? "YES" : "NO"));
        NLM_LOG_INFO("Development Integrated: " + std::string(result.developmentIntegrated ? "YES" : "NO"));
        NLM_LOG_INFO("Checkpointing Works: " + std::string(result.checkpointingWorks ? "YES" : "NO"));
        NLM_LOG_INFO("Replay Works: " + std::string(result.replayWorks ? "YES" : "NO"));
        
        return result.memoryWorkingMemoryIntegrated && 
               result.memoryEpisodicMemoryIntegrated &&
               result.neuromodulationIntegrated &&
               result.predictionIntegrated &&
               result.developmentIntegrated &&
               result.checkpointingWorks &&
               result.replayWorks;
    }
    
    bool runMinimalTest() {
        NLM_LOG_INFO("=== Running Minimal Test ===");
        
        // Quick connectivity test
        auto* region = brain_->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No brain regions found");
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_ERROR("No neurons found");
            return false;
        }
        
        // Record initial spike count
        size_t initialSpikes = brain_->getTotalSpikeCount();
        
        // Inject current into first neuron
        neurons[0]->injectCurrent(30.0f);
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain_->step(step, step * 0.001);
        }
        
        size_t spikes = brain_->getTotalSpikeCount() - initialSpikes;
        
        NLM_LOG_INFO("Minimal test results:");
        NLM_LOG_INFO("  Neurons: " + std::to_string(neurons.size()));
        NLM_LOG_INFO("  Synapses: " + std::to_string(region->getSynapses().size()));
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("✓ Basic connectivity test PASSED");
            return true;
        } else {
            NLM_LOG_INFO("✗ Basic connectivity test FAILED - no spikes generated");
            return false;
        }
    }
    
    void saveCheckpoint(const std::string& path) {
        if (brain_) {
            if (brain_->save(path)) {
                NLM_LOG_INFO("Checkpoint saved to: " + path);
            } else {
                NLM_LOG_ERROR("Failed to save checkpoint to: " + path);
            }
        }
    }
    
    void loadCheckpoint(const std::string& path) {
        if (brain_) {
            auto newBrain = std::make_shared<Brain>(config_);
            newBrain->initialize();
            
            if (newBrain->load(path)) {
                NLM_LOG_INFO("Checkpoint loaded from: " + path);
                // Note: For now, we don't replace the current brain
                // In a real scenario, you might want to update brain_ with newBrain
            } else {
                NLM_LOG_ERROR("Failed to load checkpoint from: " + path);
            }
        }
    }
};

int main(int argc, char* argv[]) {
    printWelcomeBanner();
    
    std::string mode = "demo";
    uint64_t neurons = 500;
    uint64_t steps = 1000;
    uint64_t regions = 1;
    std::string checkpointPath = "";
    bool enableDevelopment = true;
    bool enablePlasticity = true;
    bool verbose = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return 0;
        } else if (arg == "--mode" && i + 1 < argc) {
            mode = argv[++i];
        } else if (arg == "--neurons" && i + 1 < argc) {
            neurons = std::stoull(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            steps = std::stoull(argv[++i]);
        } else if (arg == "--regions" && i + 1 < argc) {
            regions = std::stoull(argv[++i]);
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            checkpointPath = argv[++i];
        } else if (arg == "--no-dev") {
            enableDevelopment = false;
        } else if (arg == "--no-plasticity") {
            enablePlasticity = false;
        } else if (arg == "--verbose") {
            verbose = true;
        }
    }
    
    // Set log level based on verbosity
    if (verbose) {
        Logger::getGlobal().setLevel(LogLevel::Debug);
    }
    
    NLM_LOG_INFO("=== NLM Phase 6 Commander Starting ===");
    NLM_LOG_INFO("Mode: " + mode);
    NLM_LOG_INFO("Neurons: " + std::to_string(neurons));
    NLM_LOG_INFO("Steps: " + std::to_string(steps));
    NLM_LOG_INFO("Regions: " + std::to_string(regions));
    
    if (!checkpointPath.empty()) {
        NLM_LOG_INFO("Checkpoint: " + checkpointPath);
    }
    
    // Create commander
    NLMCommander commander;
    
    // Configure options
    commander.setDevelopment(enableDevelopment);
    commander.setPlasticity(enablePlasticity);
    
    // Create Phase 6 config
    Phase6Config phaseConfig;
    phaseConfig.neuronCount = neurons;
    phaseConfig.regionCount = regions;
    phaseConfig.maxSteps = steps;
    phaseConfig.checkpointPath = checkpointPath;
    phaseConfig.enableDevelopment = enableDevelopment;
    
    // Initialize
    if (!commander.initialize(phaseConfig)) {
        NLM_LOG_ERROR("Failed to initialize NLM system");
        return 1;
    }
    
    // Load checkpoint if specified
    if (!checkpointPath.empty()) {
        NLM_LOG_INFO("Loading checkpoint...");
        commander.loadCheckpoint(checkpointPath);
    }
    
    // Run selected mode
    bool success = false;
    
    if (mode == "integration") {
        success = commander.runIntegrationTest();
    } else if (mode == "minimal") {
        success = commander.runMinimalTest();
    } else if (mode == "benchmark") {
        NLM_LOG_INFO("Benchmark mode not yet implemented - running demonstration instead");
        success = commander.runDemonstration(steps);
    } else if (mode == "demo" || mode == "default") {
        success = commander.runDemonstration(steps);
    } else {
        NLM_LOG_ERROR("Unknown mode: " + mode);
        printUsage(argv[0]);
        return 1;
    }
    
    // Save checkpoint if requested
    if (!checkpointPath.empty() && success) {
        commander.saveCheckpoint(checkpointPath);
    }
    
    NLM_LOG_INFO("=== Execution Complete ===");
    
    if (success) {
        NLM_LOG_INFO("✓ NLM Phase 6 operation completed successfully");
        return 0;
    } else {
        NLM_LOG_ERROR("✗ NLM Phase 6 operation failed");
        return 1;
    }
}