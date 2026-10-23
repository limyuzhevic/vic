/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * Supports different demo modes:
 * - "quick": Run only integration verification (fast)
 * - "full": Run complete integration experiment with detailed reporting
 * - "interactive": Run with step-by-step progress output
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>

using namespace nlm;

void defineArgumentParsing() {
    std::string mode = "full";
    uint64_t neurons = 500;
    uint64_t steps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string configPath = "configs/default.cfg";
    
    return std::make_tuple(mode, neurons, steps, enableCheckpointing, enableReplay, enableDevelopment, configPath);
}

void printDemoBanner(const std::string& mode) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    if (mode == "quick") {
        std::cout << "Quick Integration Verification Mode" << std::endl;
        std::cout << "Testing if all brain systems are properly connected..." << std::endl << std::endl;
    } else if (mode == "interactive") {
        std::cout << "Interactive Phase 6 Demo Mode" << std::endl;
        std::cout << "Running simulation with step-by-step progress output..." << std::endl << std::endl;
    } else {
        std::cout << "Full Integration Test Mode" << std::endl;
        std::cout << "Running comprehensive integration experiment..." << std::endl << std::endl;
    }
}

void printProgressBar(int progress, int total, const std::string& label) {
    if (progress % 100 == 0 || progress == total) {
        float percentage = 100.0f * progress / total;
        std::cout << "\r" << label << ": [";
        int barWidth = 50;
        int pos = (int)(barWidth * percentage / 100.0f);
        for (int i = 0; i < barWidth; ++i) {
            std::cout << (i < pos ? "=" : " ");
        }
        std::cout << "] " << (int)percentage << "%"; 
        if (progress == total) std::cout << std::endl;
    }
}

void printFinalResults(const Phase6IntegrationResult& result) {
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Avg synaptic weight: " << result.avgSynapticWeight << std::endl;
    std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
}

void printIntegrationStatus(const Phase6IntegrationResult& result) {
    std::cout << std::endl;
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::string mode = "full";
    uint64_t neurons = 500;
    uint64_t steps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string configPath = "configs/default.cfg";
    
    // Simple argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--mode" && i + 1 < argc) {
            mode = argv[++i];
        } else if (arg == "--neurons" && i + 1 < argc) {
            neurons = std::stoull(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            steps = std::stoull(argv[++i]);
        } else if (arg == "--no-checkpoint") {
            enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            enableReplay = false;
        } else if (arg == "--no-development") {
            enableDevelopment = false;
        } else if (arg == "--config" && i + 1 < argc) {
            configPath = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "NLM Phase 6 Demo Options:" << std::endl;
            std::cout << "  --mode <mode>          Demo mode: quick|full|interactive (default: full)" << std::endl;
            std::cout << "  --neurons <count>      Number of neurons (default: 500)" << std::endl;
            std::cout << "  --steps <count>        Number of simulation steps (default: 2000)" << std::endl;
            std::cout << "  --no-checkpoint         Disable checkpointing" << std::endl;
            std::cout << "  --no-replay             Disable replay system" << std::endl;
            std::cout << "  --no-development        Disable developmental system" << std::endl;
            std::cout << "  --config <path>        Configuration file path (default: configs/default.cfg)" << std::endl;
            std::cout << "  --help, -h             Show this help message" << std::endl;
            return 0;
        }
    }
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    printDemoBanner(mode);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Run demo based on mode
    Phase6IntegrationResult result;
    
    if (mode == "quick") {
        std::cout << "--- Quick Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << std::endl;
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return 1;
        }
        
        std::cout << "✓ Integration verification passed!" << std::endl;
        std::cout << "  All brain systems are properly connected." << std::endl;
        
        return 0;
        
    } else if (mode == "interactive") {
        std::cout << "--- Interactive Integration Test ---" << std::endl;
        
        // Test individual systems with progress updates
        experiment.testMemoryIntegration();
        printProgressBar(1, 5, "Progress");
        
        experiment.testNeuromodulationIntegration();
        printProgressBar(2, 5, "Progress");
        
        experiment.testCheckpointing();
        printProgressBar(3, 5, "Progress");
        
        experiment.testReplay();
        printProgressBar(4, 5, "Progress");
        
        // Run full experiment with progress monitoring
        std::cout << "\n--- Running Full Integration Experiment ---" << std::endl;
        Phase6Config config;
        config.neuronCount = neurons;
        config.maxSteps = steps;
        config.enableCheckpointing = enableCheckpointing;
        config.enableReplay = enableReplay;
        config.enableDevelopment = enableDevelopment;
        
        // Monitor progress during experiment
        auto originalRun = experiment.run;
        experiment.run = [&](const Phase6Config& cfg) -> Phase6IntegrationResult {
            Phase6IntegrationResult res;
            res.startTime = time(nullptr);
            auto startWall = std::chrono::high_resolution_clock::now();
            
            NLM_LOG_INFO("=== Phase 6 Integration Experiment ===");
            NLM_LOG_INFO("Configuration: " + std::to_string(cfg.neuronCount) + " neurons, " +
                         std::to_string(cfg.maxSteps) + " steps");
            
            // Create configuration
            auto brainCfg = std::make_shared<Config>();
            brainCfg->set("neuron_count", cfg.neuronCount);
            brainCfg->set("region_count", cfg.regionCount);
            brainCfg->set("connection_probability", cfg.connectionProbability);
            brainCfg->set("stdp_ltp_weight", 0.01f);
            brainCfg->set("stdp_ltd_weight", 0.012f);
            brainCfg->set("synaptogenesis_rate", 0.0001f);
            brainCfg->set("pruning_rate", 0.00001f);
            
            // Create brain
            auto brain = std::make_shared<Brain>(brainCfg);
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain");
                return res;
            }
            
            // Create simple world
            SimpleWorld world;
            world.initialize(16, 16);
            
            // Create agent
            AgentBrain agent(brain);
            agent.initialize(world);
            agent.enableRewardModulation(true);
            agent.enableStructuralPlasticity(cfg.enableDevelopment);
            agent.enableDevelopment(cfg.enableDevelopment);
            agent.enableCuriosity(true);
            
            NLM_LOG_INFO("Brain and agent initialized successfully");
            
            // Run simulation with progress monitoring
            float totalReward = 0.0f;
            float totalFiringRate = 0.0f;
            size_t firingCount = 0;
            
            for (uint64_t step = 0; step < cfg.maxSteps; ++step) {
                // Get observation
                SensoryPercept percept = world.observe(agent.getBrain()->getRegions()[0].get());
                
                // Process sensory input
                agent.processSensoryInput(percept);
                
                // Brain step
                brain->step(step, step * 0.001);
                
                // Get motor command
                MotorCommand cmd = agent.decodeMotorCommand();
                
                // Apply action to world
                world.applyAction(agent.getBrain()->getRegions()[0].get(), cmd);
                
                // Compute reward
                float reward = world.computeReward(agent.getBrain()->getRegions()[0].get());
                totalReward += reward;
                
                // Apply reward modulation
                agent.applyRewardModulation(reward, 0.0f);
                
                // Update development
                if (cfg.enableDevelopment) {
                    agent.updateDevelopment(0.001);
                }
                
                // Collect metrics
                totalFiringRate += brain->getAverageFiringRate();
                if (brain->getFiringNeuronCount() > 0) firingCount++;
                
                // Progress update
                if (step % 500 == 0) {
                    printProgressBar(step, cfg.maxSteps, "Simulation");
                }
            }
            
            // Collect final metrics
            res.totalReward = totalReward / cfg.maxSteps;
            res.avgFiringRate = totalFiringRate / cfg.maxSteps;
            res.memoryEpisodesStored = brain->getEpisodicMemory() ? 
                static_cast<float>(brain->getEpisodicMemory()->getEpisodeCount()) : 0.0f;
            res.noveltyLevel = agent.getNoveltyLevel();
            res.curiosityLevel = agent.getCuriosityLevel();
            res.dopamineLevel = agent.getNeuromodulationLevel();
            
            // Verify integration
            res.memoryWorkingMemoryIntegrated = (brain->getWorkingMemory() != nullptr);
            res.memoryEpisodicMemoryIntegrated = (brain->getEpisodicMemory() != nullptr);
            res.neuromodulationIntegrated = (brain->getDopamine() != nullptr);
            res.predictionIntegrated = (brain->getPredictionSystem() != nullptr);
            res.developmentIntegrated = (brain->getDevelopmentSystem() != nullptr);
            
            NLM_LOG_INFO("=== Experiment Complete ===");
            NLM_LOG_INFO("Total reward: " + std::to_string(res.totalReward));
            NLM_LOG_INFO("Avg firing rate: " + std::to_string(res.avgFiringRate));
            NLM_LOG_INFO("Episodes stored: " + std::to_string(res.memoryEpisodesStored));
            
            // Test checkpointing
            if (cfg.enableCheckpointing) {
                NLM_LOG_INFO("Testing checkpoint save/load...");
                std::string path = "/tmp/nlm_checkpoint_test.bin";
                if (brain->save(path)) {
                    NLM_LOG_INFO("Checkpoint saved successfully");
                    
                    // Create new brain and load
                    auto brain2 = std::make_shared<Brain>(brainCfg);
                    brain2->initialize();
                    
                    if (brain2->load(path)) {
                        NLM_LOG_INFO("Checkpoint loaded successfully");
                        res.checkpointingWorks = true;
                    } else {
                        NLM_LOG_ERROR("Failed to load checkpoint");
                    }
                } else {
                    NLM_LOG_ERROR("Failed to save checkpoint");
                }
            }
            
            res.endTime = time(nullptr);
            auto endWall = std::chrono::high_resolution_clock::now();
            res.totalWallClockTime = std::chrono::duration<double>(endWall - startWall).count();
            
            return res;
        };
        
        result = experiment.run(config);
        
    } else { // mode == "full"
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        Phase6Config config;
        config.neuronCount = neurons;
        config.maxSteps = steps;
        config.enableCheckpointing = enableCheckpointing;
        config.enableReplay = enableReplay;
        config.enableDevelopment = enableDevelopment;
        
        auto result = experiment.run(config);
    }
    
    // Display final results
    printFinalResults(result);
    printIntegrationStatus(result);
    
    std::cout << std::endl;
    std::cout << "=== DEMO COMPLETED ===" << std::endl;
    
    return 0;
}
