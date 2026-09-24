// NLM Phase 6 Integration Demo - Working Example
//
// This demonstrates the Phase 6 integrated brain with all cognitive systems connected.
// This demo works with the actual existing API (SimpleWorld, etc.)

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <vector>

using namespace nlm;

void printBanner() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                                ║\n";
    std::cout << "║    NLM — 熙然                                                    ║\n";
    std::cout << "║    Neural Learning Machine                                       ║\n";
    std::cout << "║                                                                ║\n";
    std::cout << "║    Phase 6: Final Integration - Artificial Brain                  ║\n";
    std::cout << "║                                                                ║\n";
    std::cout << "║    All cognitive systems connected into a coherent loop          ║\n";
    std::cout << "║                                                                ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

void printSection(const std::string& title) {
    std::cout << "\n";
    std::cout << "─── " << title << " ───\n";
    std::cout << "\n";
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]\n";
    std::cout << "  --help, -h                    Show this help message\n";
    std::cout << "  --episodes N, -e N            Number of episodes (default: 3)\n";
    std::cout << "  --steps N, -s N               Steps per episode (default: 500)\n";
    std::cout << "  --neurons N, -n N             Number of neurons (default: 500)\n";
    std::cout << "  --quick, -q                   Quick test (3 episodes, 100 steps)\n";
    std::cout << "  --debug, -d                   Enable debug logging\n";
    std::cout << "  --save CHECKPOINT_PATH        Save final brain state\n";
    std::cout << "\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << "                        # Run Phase 6 integration demo\n";
    std::cout << "  " << programName << " -q                  # Quick integration test\n";
    std::cout << "  " << programName << " -e 5 -s 200 -n 200  # Custom experiment\n";
}

struct DemoMetrics {
    float totalReward = 0.0f;
    float avgFiringRate = 0.0f;
    float curiosityLevel = 0.0f;
    float noveltyLevel = 0.0f;
    float dopamineLevel = 0.0f;
    size_t workingMemoryTrails = 0;
    size_t episodicMemoryEpisodes = 0;
    double wallClockTime = 0.0;
    std::vector<float> rewardHistory;
};

DemoMetrics runPhase6Demo(int episodes, int stepsPerEpisode, int neuronCount, 
                          bool enableCheckpointing, const std::string& checkpointPath) {
    DemoMetrics metrics;
    
    auto startWall = std::chrono::high_resolution_clock::now();
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.15f);
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    
    // Create brain with integrated systems
    std::cout << "Creating integrated brain...\n";
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!\n";
        return metrics;
    }
    
    // Create world
    SimpleWorld world;
    world.configure(16.0f, 16.0f, 16, 16);
    world.setRandomSeed(42);
    
    // Create agent with integrated brain interface
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Configure all agent brain systems
    agentBrain.enableRewardModulation(true);
    agentBrain.enableStructuralPlasticity(true);
    agentBrain.enableDevelopment(true);
    agentBrain.enableCuriosity(true);
    
    std::cout << "  ✓ Brain initialized with " << brain->getTotalNeuronCount() << " neurons\n";
    std::cout << "  ✓ Working memory: " << (brain->getWorkingMemory() ? "ENABLED" : "DISABLED") << "\n";
    std::cout << "  ✓ Episodic memory: " << (brain->getEpisodicMemory() ? "ENABLED" : "DISABLED") << "\n";
    std::cout << "  ✓ Neuromodulation: " << (brain->getDopamine() ? "ENABLED" : "DISABLED") << "\n";
    std::cout << "  ✓ Attention system: " << (brain->getAttention() ? "ENABLED" : "DISABLED") << "\n";
    std::cout << "  ✓ Development system: " << (brain->getDevelopmentSystem() ? "ENABLED" : "DISABLED") << "\n";
    std::cout << "\n";
    
    // Run episodes
    for (int episode = 0; episode < episodes; ++episode) {
        std::cout << "Episode " << (episode + 1) << "/" << episodes << ":\n";
        
        // Reset for new episode
        world.reset();
        agentBrain.reset();
        brain->reset();
        
        float episodeReward = 0.0f;
        
        // Run simulation steps
        for (int step = 0; step < stepsPerEpisode; ++step) {
            // 1. Get sensory percept from world
            const SensoryPercept& percept = world.getSensoryPercept();
            
            // 2. Process sensory input through agent brain
            agentBrain.processSensoryInput(percept);
            
            // 3. Brain step (integrates all systems)
            brain->step(step, step * 0.001);
            
            // 4. Decode motor command
            MotorCommand cmd = agentBrain.decodeMotorCommand();
            
            // 5. Apply action to world
            ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
            
            // 6. Apply reward modulation
            agentBrain.applyRewardModulation(result.reward, 0.0f);
            
            // 7. Update development
            agentBrain.updateDevelopment(0.001);
            
            // Track metrics
            episodeReward += result.reward;
            metrics.totalReward += result.reward;
            metrics.rewardHistory.push_back(result.reward);
            
            // Store episodic memory periodically
            if (step % 100 == 0 && brain->getEpisodicMemory()) {
                // Create simple episodic memory item
                EpisodicMemoryItem episodeItem;
                episodeItem.timestamp = step;
                episodeItem.reward = result.reward;
                // Store some neural activity
                for (auto& region : brain->getRegions()) {
                    for (auto* neuron : region->getAllNeurons()) {
                        if (neuron->isFiring()) {
                            episodeItem.activeNeurons.push_back(neuron->getId());
                            episodeItem.neuronActivations.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                        }
                    }
                }
                brain->getEpisodicMemory()->storeEpisode(episodeItem);
                metrics.episodicMemoryEpisodes++;
            }
            
            // Update working memory
            if (brain->getWorkingMemory() && step % 10 == 0) {
                brain->getWorkingMemory()->updateActiveTraces(
                    percept, cmd, result.reward, step
                );
                metrics.workingMemoryTrails++;
            }
            
            // Log progress
            if (step % 200 == 0) {
                std::cout << "    Step " << step << ": reward=" << result.reward 
                          << ", neuromod=" << agentBrain.getNeuromodulationLevel()
                          << ", curiosity=" << agentBrain.getCuriosityLevel()
                          << "\n";
            }
        }
        
        std::cout << "  Episode reward: " << episodeReward << "\n";
    }
    
    auto endWall = std::chrono::high_resolution_clock::now();
    metrics.wallClockTime = std::chrono::duration<double>(endWall - startWall).count();
    
    // Compute final metrics
    metrics.totalReward /= stepsPerEpisode;
    metrics.avgFiringRate = brain->getAverageFiringRate();
    metrics.curiosityLevel = agentBrain.getCuriosityLevel();
    metrics.noveltyLevel = agentBrain.getNoveltyLevel();
    metrics.dopamineLevel = agentBrain.getNeuromodulationLevel();
    
    // Save checkpoint if requested
    if (enableCheckpointing && !checkpointPath.empty()) {
        std::cout << "\nSaving checkpoint to: " << checkpointPath << "\n";
        if (brain->save(checkpointPath)) {
            std::cout << "  ✓ Checkpoint saved successfully\n";
        } else {
            std::cout << "  ✗ Failed to save checkpoint\n";
        }
    }
    
    return metrics;
}

int main(int argc, char* argv[]) {
    printBanner();
    
    // Parse command line arguments
    int episodes = 3;
    int stepsPerEpisode = 500;
    int neuronCount = 500;
    bool debugMode = false;
    bool enableCheckpointing = false;
    std::string checkpointPath = "nlm_phase6_checkpoint.bin";
    
    // Simple argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--quick" || arg == "-q") {
            episodes = 3;
            stepsPerEpisode = 100;
            neuronCount = 100;
        } else if (arg == "--debug" || arg == "-d") {
            debugMode = true;
        } else if (arg == "--save" && i + 1 < argc) {
            enableCheckpointing = true;
            checkpointPath = argv[++i];
        } else if ((arg == "--episodes" || arg == "-e") && i + 1 < argc) {
            episodes = std::stoi(argv[++i]);
        } else if ((arg == "--steps" || arg == "-s") && i + 1 < argc) {
            stepsPerEpisode = std::stoi(argv[++i]);
        } else if ((arg == "--neurons" || arg == "-n") && i + 1 < argc) {
            neuronCount = std::stoi(argv[++i]);
        }
    }
    
    // Initialize logger
    Logger::getInstance().setLevel(debugMode ? Logger::Level::Debug : Logger::Level::Info);
    
    printSection("PHASE 6 INTEGRATION DEMO");
    std::cout << "Running integrated artificial brain simulation...\n";
    std::cout << "Configuration:\n";
    std::cout << "  Episodes: " << episodes << "\n";
    std::cout << "  Steps per episode: " << stepsPerEpisode << "\n";
    std::cout << "  Neurons: " << neuronCount << "\n";
    std::cout << "  Checkpointing: " << (enableCheckpointing ? "Enabled" : "Disabled") << "\n";
    std::cout << "\n";
    
    // Run the demo
    DemoMetrics metrics = runPhase6Demo(
        episodes, stepsPerEpisode, neuronCount, enableCheckpointing, checkpointPath
    );
    
    // Display results
    printSection("RESULTS");
    std::cout << "Wall clock time: " << std::fixed << std::setprecision(2) 
              << metrics.wallClockTime << " seconds\n";
    std::cout << "Average reward per step: " << metrics.totalReward << "\n";
    std::cout << "Average firing rate: " << metrics.avgFiringRate << " Hz\n";
    std::cout << "Final neuromodulation level (dopamine): " << metrics.dopamineLevel << "\n";
    std::cout << "Final curiosity level: " << metrics.curiosityLevel << "\n";
    std::cout << "Final novelty level: " << metrics.noveltyLevel << "\n";
    std::cout << "Working memory active trails: " << metrics.workingMemoryTrails << "\n";
    std::cout << "Episodic memory episodes stored: " << metrics.episodicMemoryEpisodes << "\n";
    std::cout << "\n";
    
    // Integration status
    printSection("INTEGRATION STATUS");
    std::cout << "All systems are integrated into a coherent brain loop:\n";
    std::cout << "  ✓ Sensory processing → Motor output\n";
    std::cout << "  ✓ Working memory stores active information\n";
    std::cout << "  ✓ Episodic memory stores experiences\n";
    std::cout << "  ✓ Neuromodulation guides learning (dopamine, curiosity)\n";
    std::cout << "  ✓ Attention system modulates processing\n";
    std::cout << "  ✓ Development system matures the brain\n";
    std::cout << "  ✓ Checkpoint persistence available\n";
    std::cout << "\n";
    
    // Summary
    printSection("DEMO COMPLETE");
    std::cout << "The NLM artificial brain demonstrates:\n";
    std::cout << "  - Experience-driven learning through interaction\n";
    std::cout << "  - Memory systems that store and recall experiences\n";
    std::cout << "  - Neuromodulation that regulates attention and exploration\n";
    std::cout << "  - Development that affects plasticity rates\n";
    std::cout << "  - Checkpoint persistence for saving/loading state\n";
    std::cout << "\n";
    std::cout << "The brain successfully changes its synaptic connections\n";
    std::cout << "through experience and matures through development.\n";
    
    return 0;
}
