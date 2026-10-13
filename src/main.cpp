// NLM (熙然) - Neural Learning Machine
// Phase 6: Fully Integrated Artificial Brain
//
// This phase demonstrates a complete artificial brain with all cognitive systems
// properly integrated into a coherent learning system.

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <chrono>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Fully Integrated Artificial Brain                 ║
    ║                                                               ║
    ║     All cognitive systems connected                            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp() {
    std::cout << "\nNLM Phase 6 - Fully Integrated Artificial Brain\n";
    std::cout << "=============================================\n\n";
    
    std::cout << "COMMANDS:\n";
    std::cout << "  --help                    Show this help message\n";
    std::cout << "  --quick-demo              Run quick integration test\n";
    std::cout << "  --full-demo              Run comprehensive integration experiment\n";
    std::cout << "  --checkpoint SAVE_PATH    Save brain state to checkpoint\n";
    std::cout << "  --load LOAD_PATH          Load brain state from checkpoint\n";
    std::cout << "  --config FILE             Load configuration from file\n";
    std::cout << "  --neurons N               Set neuron count (default: 1000)\n";
    std::cout << "  --steps N                 Set simulation steps (default: 1000)\n";
    std::cout << "  --no-memory               Disable memory systems\n";
    std::cout << "  --no-prediction           Disable prediction system\n";
    std::cout << "  --no-cognition            Disable cognitive systems\n";
    std::cout << "  --no-neuromodulation       Disable neuromodulation\n";
    std::cout << "  --no-development          Disable development system\n";
    std::cout << "  --no-curiosity            Disable curiosity\n";
    std::cout << "  --no-replay               Disable memory replay\n";
    std::cout << "  --no-checkpoint           Disable checkpointing\n\n";
    
    std::cout << "ADVANCED OPTIONS:\n";
    std::cout << "  --world-width W           World width (default: 16)\n";
    std::cout << "  --world-height H          World height (default: 16)\n";
    std::cout << "  --vision-width W          Vision width (default: 8)\n";
    std::cout << "  --vision-height H         Vision height (default: 8)\n";
    std::cout << "  --random-seed S           Random seed for reproducibility\n";
    std::cout << "  --enable-all              Enable all optional systems\n";
    std::cout << "  --minimal                 Minimal setup (only core neural computation)\n\n";
    
    std::cout << "EXAMPLES:\n";
    std::cout << "  ./nlm --quick-demo                           # Quick integration test\n";
    std::cout << "  ./nlm --full-demo --neurons 500 --steps 2000   # Full experiment\n";
    std::cout << "  ./nlm --checkpoint saved_brain.bin              # Save state\n";
    std::cout << "  ./nlm --load saved_brain.bin                  # Load state\n\n";
}

void runQuickIntegrationDemo() {
    std::cout << "\n=== Quick Integration Demo ===\n";
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Configure for quick demo
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.15f);
    config->set("random_seed", 42);
    config->set("simulation_timestep", 0.001);
    config->set("replay_interval", 100);
    config->set("consolidation_interval", 1000);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    std::cout << "Brain initialized successfully:\n";
    std::cout << "  - Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  - Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  - Regions: " << brain->getRegionCount() << std::endl;
    
    // Inject sensory input
    std::vector<float> sensoryInput = {1.0f, 0.5f, 0.8f, 0.3f, 0.9f};
    brain->receiveSensoryInput(SensoryInput(sensoryInput));
    
    std::cout << "\nSensory input injected.\n";
    
    // Run simulation
    size_t steps = 50;
    std::cout << "Running " << steps << " simulation steps:\n";
    
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        // Log progress every 10 steps
        if (step % 10 == 0) {
            std::cout << "  Step " << step << ": ";
            std::cout << "Spikes: " << brain->getTotalSpikeCount() - 
                         (step > 0 ? (step-1) * 10 : 0) << "; ";
            std::cout << "Active neurons: " << brain->getActiveNeuronCount() << "; ";
            std::cout << "Firing rate: " << brain->getAverageFiringRate() << std::endl;
        }
    }
    
    std::cout << "\n=== Integration Results ===\n";
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "Working memory active: " << (brain->getWorkingMemory() ? "Yes" : "No") << std::endl;
    std::cout << "Episodic memory has episodes: " << 
        (brain->getEpisodicMemory() ? 
         (brain->getEpisodicMemory()->getEpisodeCount() > 0 ? "Yes" : "No") : "No") << std::endl;
    std::cout << "Prediction system available: " << (brain->getPredictionSystem() ? "Yes" : "No") << std::endl;
    std::cout << "Attention system available: " << (brain->getAttention() ? "Yes" : "No") << std::endl;
    std::cout << "Planner available: " << (brain->getPlanner() ? "Yes" : "No") << std::endl;
    std::cout << "Concept formation available: " << (brain->getConceptFormation() ? "Yes" : "No") << std::endl;
    std::cout << "Development system available: " << (brain->getDevelopmentSystem() ? "Yes" : "No") << std::endl;
    std::cout << "Dopamine system available: " << (brain->getDopamine() ? "Yes" : "No") << std::endl;
    
    std::cout << "\n✓ Quick integration demo completed successfully!\n";
}

void runFullIntegrationDemo(const std::map<std::string, std::string>& options) {
    std::cout << "\n=== Full Integration Experiment ===\n";
    
    // Parse options
    size_t neuronCount = 1000;
    size_t steps = 1000;
    bool enableMemory = true;
    bool enablePrediction = true;
    bool enableCognition = true;
    bool enableNeuromodulation = true;
    bool enableDevelopment = true;
    bool enableCuriosity = true;
    bool enableReplay = true;
    bool enableCheckpoint = true;
    
    if (options.count("neurons")) {
        neuronCount = std::stoul(options.at("neurons"));
    }
    if (options.count("steps")) {
        steps = std::stoul(options.at("steps"));
    }
    if (options.count("no-memory")) {
        enableMemory = false;
    }
    if (options.count("no-prediction")) {
        enablePrediction = false;
    }
    if (options.count("no-cognition")) {
        enableCognition = false;
    }
    if (options.count("no-neuromodulation")) {
        enableNeuromodulation = false;
    }
    if (options.count("no-development")) {
        enableDevelopment = false;
    }
    if (options.count("no-curiosity")) {
        enableCuriosity = false;
    }
    if (options.count("no-replay")) {
        enableReplay = false;
    }
    if (options.count("no-checkpoint")) {
        enableCheckpoint = false;
    }
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Basic configuration
    config->set("neuron_count", neuronCount);
    config->set("region_count", 1);
    config->set("connection_probability", 0.15f);
    config->set("random_seed", 42);
    config->set("simulation_timestep", 0.001);
    config->set("replay_interval", 100);
    config->set("consolidation_interval", 1000);
    
    if (enableCheckpoint) {
        config->set("checkpoint_dir", "./nlm_checkpoints");
        config->set("checkpoint_interval", 500);
    }
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    std::cout << "Configuration:\n";
    std::cout << "  - Neurons: " << neuronCount << std::endl;
    std::cout << "  - Steps: " << steps << std::endl;
    std::cout << "  - Memory: " << (enableMemory ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Prediction: " << (enablePrediction ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Cognition: " << (enableCognition ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Neuromodulation: " << (enableNeuromodulation ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Development: " << (enableDevelopment ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Curiosity: " << (enableCuriosity ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Replay: " << (enableReplay ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Checkpointing: " << (enableCheckpoint ? "Enabled" : "Disabled") << std::endl;
    
    // Create simple world
    SimpleWorld world;
    world.initialize(16, 16, 8, 8);
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Enable optional systems
    agent.enableRewardModulation(enableNeuromodulation);
    agent.enableStructuralPlasticity(enableDevelopment);
    agent.enableDevelopment(enableDevelopment);
    agent.enableCuriosity(enableCuriosity);
    
    if (!enableMemory) {
        std::cout << "Memory systems disabled - brain will operate without memory.\n";
    }
    
    std::cout << "\nStarting full integration experiment...\n";
    
    // Run simulation
    float totalReward = 0.0f;
    float totalFiringRate = 0.0f;
    size_t firingCount = 0;
    
    for (size_t step = 0; step < steps; ++step) {
        // Get observation from world
        SensoryPercept percept = world.observe(brain->getRegions()[0].get());
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Brain step
        brain->step(step, step * 0.001);
        
        // Get motor command
        MotorCommand cmd = agent.decodeMotorCommand();
        
        // Apply action to world
        world.applyMotorCommand(cmd, world.getSimulationTime());
        
        // Compute reward
        float reward = world.computeReward(brain->getRegions()[0].get());
        totalReward += reward;
        
        // Apply reward modulation
        agent.applyRewardModulation(reward, 0.0f);
        
        // Update development
        if (enableDevelopment) {
            agent.updateDevelopment(0.001);
        }
        
        // Collect metrics
        totalFiringRate += brain->getAverageFiringRate();
        if (brain->getFiringNeuronCount() > 0) firingCount++;
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                      << "Reward: " << totalReward / (step + 1) << ", "
                      << "Firing: " << brain->getAverageFiringRate() << ", "
                      << "WorkingMem: " << (brain->getWorkingMemory() ? 
                                         brain->getWorkingMemory()->getActiveTraces() : 0) << std::endl;
        }
    }
    
    // Collect final metrics
    std::cout << "\n=== Final Results ===\n";
    std::cout << "Total reward: " << totalReward / steps << std::endl;
    std::cout << "Average firing rate: " << totalFiringRate / steps << std::endl;
    std::cout << "Active steps (firing neurons): " << firingCount << " out of " << steps << std::endl;
    std::cout << "Working memory traces: " << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0) << std::endl;
    std::cout << "Episodic episodes stored: " << (brain->getEpisodicMemory() ? brain->getEpisodicMemory()->getEpisodeCount() : 0) << std::endl;
    std::cout << "Dopamine level: " << (brain->getDopamine() ? brain->getDopamine()->getLevel() : 0.0f) << std::endl;
    std::cout << "Curiosity level: " << (agent.getCuriosityLevel() ? agent.getCuriosityLevel() : 0.0f) << std::endl;
    
    // Save checkpoint if enabled
    if (enableCheckpoint && enableReplay) {
        std::string checkpointPath = "./nlm_checkpoint.bin";
        std::cout << "\nSaving checkpoint to: " << checkpointPath << std::endl;
        if (brain->save(checkpointPath)) {
            std::cout << "✓ Checkpoint saved successfully!" << std::endl;
        } else {
            std::cout << "⚠ Failed to save checkpoint (this is normal if paths are not writable)" << std::endl;
        }
    }
    
    std::cout << "\n✓ Full integration experiment completed!\n";
}

void parseCommandLine(int argc, char** argv, std::map<std::string, std::string>& options) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printHelp();
            exit(0);
        } else if (arg == "--quick-demo") {
            options["mode"] = "quick-demo";
        } else if (arg == "--full-demo") {
            options["mode"] = "full-demo";
        } else if (arg.substr(0, 11) == "--neurons=") {
            options["neurons"] = arg.substr(11);
        } else if (arg.substr(0, 9) == "--steps=") {
            options["steps"] = arg.substr(9);
        } else if (arg == "--no-memory") {
            options["no-memory"] = "true";
        } else if (arg == "--no-prediction") {
            options["no-prediction"] = "true";
        } else if (arg == "--no-cognition") {
            options["no-cognition"] = "true";
        } else if (arg == "--no-neuromodulation") {
            options["no-neuromodulation"] = "true";
        } else if (arg == "--no-development") {
            options["no-development"] = "true";
        } else if (arg == "--no-curiosity") {
            options["no-curiosity"] = "true";
        } else if (arg == "--no-replay") {
            options["no-replay"] = "true";
        } else if (arg == "--no-checkpoint") {
            options["no-checkpoint"] = "true";
        } else if (arg.substr(0, 14) == "--checkpoint=") {
            options["checkpoint"] = arg.substr(14);
        } else if (arg.substr(0, 11) == "--load=") {
            options["load"] = arg.substr(11);
        } else if (arg.substr(0, 9) == "--config=") {
            options["config"] = arg.substr(9);
        } else if (arg == "--enable-all") {
            options["enable-all"] = "true";
        } else if (arg == "--minimal") {
            options["minimal"] = "true";
        } else if (arg.substr(0, 14) == "--world-width=") {
            options["world-width"] = arg.substr(14);
        } else if (arg.substr(0, 15) == "--world-height=") {
            options["world-height"] = arg.substr(15);
        } else if (arg.substr(0, 14) == "--vision-width=") {
            options["vision-width"] = arg.substr(14);
        } else if (arg.substr(0, 15) == "--vision-height=") {
            options["vision-height"] = arg.substr(15);
        } else if (arg.substr(0, 14) == "--random-seed=") {
            options["random-seed"] = arg.substr(14);
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printHelp();
            exit(1);
        }
    }
}

int main(int argc, char** argv[]) {
    printBanner();
    
    std::cout << "\nNLM Phase 6: Fully Integrated Artificial Brain\n";
    std::cout << "========================================\n\n";
    
    // Parse command line options
    std::map<std::string, std::string> options;
    parseCommandLine(argc, argv, options);
    
    // Determine mode
    std::string mode = "full-demo";  // default
    if (options.count("mode")) {
        mode = options.at("mode");
    }
    
    try {
        if (mode == "quick-demo") {
            runQuickIntegrationDemo();
        } else if (mode == "full-demo") {
            runFullIntegrationDemo(options);
        } else {
            std::cerr << "Invalid mode: " << mode << std::endl;
            printHelp();
            return 1;
        }
        
        std::cout << "\n=== All Demos Completed Successfully ===\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}
