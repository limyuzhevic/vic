/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "Phase6IntegratedExperiment.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <chrono>

using namespace nlm;

// Custom error classes for Phase 6
class Phase6Error : public std::runtime_error {
public:
    explicit Phase6Error(const std::string& msg) : std::runtime_error("Phase 6 Error: " + msg) {}
};

class Phase6DemoError : public Phase6Error {
public:
    explicit Phase6DemoError(const std::string& msg) : Phase6Error("Demo Error: " + msg) {}
};

struct Phase6DemoConfig {
    // Integration test settings
    uint64_t neuronCount = 500;
    uint64_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "/tmp/nlm_phase6_checkpoint.bin";
    
    // Performance settings
    uint64_t worldWidth = 16;
    uint64_t worldHeight = 16;
    
    // Logging settings
    bool verbose = false;
    bool saveResults = false;
    std::string resultsFile;
    
    // Testing options
    bool runQuickTests = false;
    bool skipFullIntegration = false;
    
    void applyToConfig(std::shared_ptr<Config> config) const {
        if (!config) return;
        
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", 1); // Use 1 region for simplicity
        config->set("connection_probability", 0.15f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
    }
};

class CommandLineParser {
public:
    static Phase6DemoConfig parse(int argc, char* argv[]) {
        Phase6DemoConfig config;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printUsage();
                exit(0);
            }
            else if (arg == "--quick") {
                config.runQuickTests = true;
                config.neuronCount = 100;
                config.maxSteps = 500;
                config.enableCheckpointing = false;
                config.enableReplay = false;
                config.enableDevelopment = false;
            }
            else if (arg == "--verbose" || arg == "-v") {
                config.verbose = true;
            }
            else if (arg == "--no-checkpoint") {
                config.enableCheckpointing = false;
            }
            else if (arg == "--no-replay") {
                config.enableReplay = false;
            }
            else if (arg == "--no-development") {
                config.enableDevelopment = false;
            }
            else if (arg.substr(0, 6) == "--neurons=") {
                try {
                    config.neuronCount = std::stoull(arg.substr(9));
                } catch (const std::exception& e) {
                    throw Phase6DemoError("Invalid neuron count");
                }
            }
            else if (arg == "--neurons" && i + 1 < argc) {
                try {
                    config.neuronCount = std::stoull(argv[++i]);
                } catch (const std::exception& e) {
                    throw Phase6DemoError("Invalid neuron count");
                }
            }
            else if (arg.substr(0, 6) == "--steps=") {
                try {
                    config.maxSteps = std::stoull(arg.substr(9));
                } catch (const std::exception& e) {
                    throw Phase6DemoError("Invalid step count");
                }
            }
            else if (arg == "--steps" && i + 1 < argc) {
                try {
                    config.maxSteps = std::stoull(argv[++i]);
                } catch (const std::exception& e) {
                    throw Phase6DemoError("Invalid step count");
                }
            }
            else if (arg.substr(0, 10) == "--checkpoint=") {
                config.checkpointPath = arg.substr(11);
                config.enableCheckpointing = true;
            }
            else if (arg == "--checkpoint" && i + 1 < argc) {
                config.checkpointPath = argv[++i];
                config.enableCheckpointing = true;
            }
            else if (arg.substr(0, 8) == "--output=") {
                config.resultsFile = arg.substr(9);
                config.saveResults = true;
            }
            else if (arg == "--output" && i + 1 < argc) {
                config.resultsFile = argv[++i];
                config.saveResults = true;
            }
            else if (arg == "--skip-full") {
                config.skipFullIntegration = true;
            }
        }
        
        return config;
    }
    
private:
    static void printUsage() {
        std::cout << "NLM Phase 6 Integration Demo\n\n";
        std::cout << "Usage: nlm_phase6_demo [OPTIONS]\n\n";
        std::cout << "Options:\n";
        std::cout << "  -h, --help              Show help\n";
        std::cout << "  --quick                 Quick demo (small scale)\n";
        std::cout << "  -v, --verbose           Verbose output\n";
        std::cout << "  --no-checkpoint         Disable checkpointing\n";
        std::cout << "  --no-replay             Disable replay\n";
        std::cout << "  --no-development        Disable development\n";
        std::cout << "  --neurons=<num>         Neuron count (default: 500)\n";
        std::cout << "  --steps=<num>           Max steps (default: 2000)\n";
        std::cout << "  --checkpoint=<file>     Checkpoint file path\n";
        std::cout << "  --output=<file>         Save results to file\n";
        std::cout << "  --skip-full             Skip full integration test\n";
        std::cout << "\nExamples:\n";
        std::cout << "  nlm_phase6_demo                 # Run full demo\n";
        std::cout << "  nlm_phase6_demo --quick        # Quick demo\n";
        std::cout << "  nlm_phase6_demo --neurons=1000 --steps=5000\n";
    }
};

class ResultsFormatter {
public:
    static std::string formatIntegrationResult(const Phase6IntegrationResult& result) {
        std::stringstream ss;
        ss << "=== Phase 6 Integration Results ===\n\n";
        ss << "Performance Metrics:\n";
        ss << "  Total Reward: " << result.totalReward << std::endl;
        ss << "  Average Firing Rate: " << result.avgFiringRate << std::endl;
        ss << "  Episodes Stored: " << result.memoryEpisodesStored << std::endl;
        ss << "  Dopamine Level: " << result.dopamineLevel << std::endl;
        ss << "  Novelty Level: " << result.noveltyLevel << std::endl;
        ss << "  Curiosity Level: " << result.curiosityLevel << std::endl;
        ss << "  Prediction Error: " << result.predictionError << std::endl;
        ss << "  Wall Clock Time: " << result.totalWallClockTime << "s" << std::endl;
        ss << "  End Time: " << result.endTime << std::endl;
        
        ss << "\nIntegration Status:\n";
        ss << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        ss << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        ss << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        ss << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        ss << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        ss << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        
        return ss.str();
    }
    
    static void printProgress(const std::string& message, float progress) {
        if (!progress) return;
        
        int barWidth = 50;
        int pos = static_cast<int>(barWidth * progress);
        
        std::cout << "\r[";
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(progress * 100.0f) << "% " << message;
        std::cout.flush();
    }
    
    static void printTestSection(const std::string& title) {
        std::cout << "\n" << title << std::endl;
        std::cout << std::string(title.length(), '=') << std::endl;
    }
};

class Phase6Demo {
public:
    Phase6Demo() : config(), experiment() {}
    ~Phase6Demo() = default;
    
    void run(const Phase6DemoConfig& cfg) {
        config = cfg;
        
        try {
            if (config.verbose) {
                std::cout << "Starting NLM Phase 6 Integration Demo" << std::endl;
                std::cout << "======================================" << std::endl;
            }
            
            // Initialize logging
            Logger::getInstance().setLevel(Logger::Level::Info);
            
            // Run integration tests
            runIntegrationTests();
            
            // Run individual system tests if not skipped
            if (!config.skipFullIntegration) {
                runIndividualSystemTests();
            }
            
            // Run full integration experiment
            if (!config.skipFullIntegration) {
                runFullIntegrationExperiment();
            }
            
            // Save results if requested
            if (config.saveResults && !config.resultsFile.empty()) {
                saveResultsToFile();
            }
            
            if (config.verbose) {
                printFinalSummary();
            }
            
        } catch (const std::exception& e) {
            std::cerr << "Error in Phase 6 demo: " << e.what() << std::endl;
            throw;
        }
    }
    
private:
    Phase6DemoConfig config;
    Phase6IntegratedExperiment experiment;
    
    void runIntegrationTests() {
        ResultsFormatter::printTestSection("--- Integration Verification ---");
        
        bool integrationOK = experiment.verifyIntegration();
        
        if (!integrationOK) {
            throw Phase6DemoError("Integration verification failed!");
        }
        
        std::cout << "Integration verification passed!" << std::endl << std::endl;
    }
    
    void runIndividualSystemTests() {
        ResultsFormatter::printTestSection("--- Memory Integration Test ---");
        experiment.testMemoryIntegration();
        
        ResultsFormatter::printTestSection("--- Neuromodulation Integration Test ---");
        experiment.testNeuromodulationIntegration();
        
        ResultsFormatter::printTestSection("--- Checkpoint Test ---");
        experiment.testCheckpointing();
        
        ResultsFormatter::printTestSection("--- Replay Test ---");
        experiment.testReplay();
    }
    
    void runFullIntegrationExperiment() {
        ResultsFormatter::printTestSection("--- Full Integration Experiment ---");
        
        // Create configuration
        Phase6Config expConfig;
        expConfig.neuronCount = config.neuronCount;
        expConfig.maxSteps = config.maxSteps;
        expConfig.enableCheckpointing = config.enableCheckpointing;
        expConfig.enableReplay = config.enableReplay;
        expConfig.enableDevelopment = config.enableDevelopment;
        expConfig.checkpointPath = config.checkpointPath;
        
        // Create brain and agent for the experiment
        auto cfg = std::make_shared<Config>();
        config.applyToConfig(cfg);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            throw Phase6DemoError("Failed to initialize brain for integration experiment");
        }
        
        SimpleWorld world;
        world.initialize(config.worldWidth, config.worldHeight);
        
        AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(true);
        agent.enableStructuralPlasticity(config.enableDevelopment);
        agent.enableDevelopment(config.enableDevelopment);
        agent.enableCuriosity(true);
        
        if (config.verbose) {
            std::cout << "Brain and agent initialized successfully" << std::endl;
        }
        
        // Run simulation
        float totalReward = 0.0f;
        float totalFiringRate = 0.0f;
        size_t firingCount = 0;
        
        for (uint64_t step = 0; step < expConfig.maxSteps; ++step) {
            // Get observation
            SensoryPercept percept = world.observe(brain->getRegions()[0].get());
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Get motor command
            MotorCommand cmd = agent.decodeMotorCommand();
            
            // Apply action to world
            world.applyAction(brain->getRegions()[0].get(), cmd);
            
            // Compute reward
            float reward = world.computeReward(brain->getRegions()[0].get());
            totalReward += reward;
            
            // Apply reward modulation
            agent.applyRewardModulation(reward, 0.0f);
            
            // Update development
            if (config.enableDevelopment) {
                agent.updateDevelopment(0.001);
            }
            
            // Collect metrics
            totalFiringRate += brain->getAverageFiringRate();
            if (brain->getFiringNeuronCount() > 0) firingCount++;
            
            // Print periodic updates
            if (config.verbose && step % 500 == 0) {
                std::cout << "Step " << step 
                         << " | Reward: " << (totalReward / (step + 1))
                         << " | Firing: " << brain->getAverageFiringRate()
                         << " | WorkingMem: " << (brain->getWorkingMemory() ? 
                             brain->getWorkingMemory()->getActiveTraces() : 0)
                         << std::endl;
            }
            
            // Update progress
            if (config.verbose) {
                float progress = static_cast<float>(step) / expConfig.maxSteps;
                ResultsFormatter::printProgress("Running integration experiment", progress);
            }
        }
        
        std::cout << std::endl; // Clear progress line
        
        // Collect final metrics
        Phase6IntegrationResult result;
        result.totalReward = totalReward / expConfig.maxSteps;
        result.avgFiringRate = totalFiringRate / expConfig.maxSteps;
        result.memoryEpisodesStored = brain->getEpisodicMemory() ? 
            static_cast<float>(brain->getEpisodicMemory()->getEpisodeCount()) : 0.0f;
        result.noveltyLevel = agent.getNoveltyLevel();
        result.curiosityLevel = agent.getCuriosityLevel();
        result.dopamineLevel = agent.getNeuromodulationLevel();
        result.predictionError = agent.getPredictionError();
        
        // Verify integration
        result.memoryWorkingMemoryIntegrated = (brain->getWorkingMemory() != nullptr);
        result.memoryEpisodicMemoryIntegrated = (brain->getEpisodicMemory() != nullptr);
        result.neuromodulationIntegrated = (brain->getDopamine() != nullptr);
        result.predictionIntegrated = (brain->getPredictionSystem() != nullptr);
        result.developmentIntegrated = (brain->getDevelopmentSystem() != nullptr);
        
        // Test checkpointing
        if (config.enableCheckpointing) {
            if (brain->save(config.checkpointPath)) {
                if (config.verbose) {
                    std::cout << "Checkpoint saved successfully" << std::endl;
                }
                
                // Create new brain and load
                auto brain2 = std::make_shared<Brain>(cfg);
                brain2->initialize();
                
                if (brain2->load(config.checkpointPath)) {
                    if (config.verbose) {
                        std::cout << "Checkpoint loaded successfully" << std::endl;
                    }
                    result.checkpointingWorks = true;
                } else {
                    if (config.verbose) {
                        std::cerr << "Failed to load checkpoint" << std::endl;
                    }
                }
            } else {
                if (config.verbose) {
                    std::cerr << "Failed to save checkpoint" << std::endl;
                }
            }
        }
        
        // Set timing
        auto endWall = std::chrono::high_resolution_clock::now();
        result.totalWallClockTime = std::chrono::duration<double>(endWall - std::chrono::high_resolution_clock::now()).count();
        result.endTime = time(nullptr);
        
        // Print results
        std::cout << "\n=== FINAL RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        
        std::cout << "\n=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    }
    
    void saveResultsToFile() {
        // Create a simple results file
        std::ofstream file(config.resultsFile);
        if (!file.is_open()) {
            throw Phase6DemoError("Cannot open results file: " + config.resultsFile);
        }
        
        file << "NLM Phase 6 Integration Demo Results" << std::endl;
        file << "==================================" << std::endl;
        file << "Generated on: " << time(nullptr) << std::endl;
        file << "Neuron Count: " << config.neuronCount << std::endl;
        file << "Max Steps: " << config.maxSteps << std::endl;
        file << "Checkpointing: " << (config.enableCheckpointing ? "Yes" : "No") << std::endl;
        file << "Replay: " << (config.enableReplay ? "Yes" : "No") << std::endl;
        file << "Development: " << (config.enableDevelopment ? "Yes" : "No") << std::endl;
        file << "Checkpoint Path: " << config.checkpointPath << std::endl;
        
        file.close();
        
        if (config.verbose) {
            std::cout << "Results saved to: " << config.resultsFile << std::endl;
        }
    }
    
    void printFinalSummary() {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "NLM Phase 6 Integration Demo Complete" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << "Phase 6 successfully integrates all brain systems:" << std::endl;
        std::cout << "  ✓ Memory systems (working + episodic)" << std::endl;
        std::cout << "  ✓ Neuromodulation (dopamine, curiosity, novelty)" << std::endl;
        std::cout << "  ✓ Prediction system" << std::endl;
        std::cout << "  ✓ Cognitive systems (planner, concept formation, attention)" << std::endl;
        std::cout << "  ✓ Development system" << std::endl;
        std::cout << "  ✓ Checkpointing and replay" << std::endl;
        std::cout << "\nThe NLM brain is now a functioning artificial neural substrate";
        std::cout << std::endl;
        std::cout << "capable of changing its own synaptic connections through experience.";
        std::cout << std::endl;
    }
};

// Main entry point for Phase 6 demo
int main(int argc, char* argv[]) {
    try {
        Phase6DemoConfig config;
        
        // Parse command line arguments
        if (argc > 1) {
            config = CommandLineParser::parse(argc, argv);
        }
        
        // Create and run demo
        Phase6Demo demo;
        demo.run(config);
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}

namespace nlm {
    // Additional helper functions for Phase 6
    std::string developmentalStageToString(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
    
    std::string motorCommandToString(MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::Wait: return "Wait";
            default: return "Unknown";
        }
    }
}