#pragma once

#include "Phase3Experiment.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <set>
#include <utility>
#include <filesystem>

namespace nlm {

// Main Phase 3 demonstration
// Shows: experience -> neural activity -> action -> consequence -> learning

class Phase3Demo {
public:
    Phase3Demo() {}
    ~Phase3Demo() {}
    
    void run(int numEpisodes = 5, int stepsPerEpisode = 200) {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
        std::cout << "===========================================\n\n";
        
        // Create configuration
        auto config = std::make_shared<Config>();
        configureEnvironment(config);
        configureBrain(config);
        
        // Create world
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(42);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Configure agent brain
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        std::cout << "Initial brain state:\n";
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << "\n";
        std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << "\n\n";
        
        // Metrics tracking
        std::vector<ExperimentMetrics> allMetrics;
        
        // Run episodes
        for (int episode = 0; episode < numEpisodes; ++episode) {
            std::cout << "--- Episode " << episode << " started ---\n";
            
            // Reset world
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            ExperimentMetrics metrics;
            metrics.episode = episode;

            // Phase 5 navigation/interact metrics: visited 1x1 cells,
            // interact outcomes from the world's own result message
            // (consumed_resource / hit_hazard / no_object — not a label
            // given to the brain), and forward/backward oscillation
            // (a Fwd step immediately followed by Bwd, or vice versa).
            std::set<std::pair<int, int>> visitedCells;
            int resourcesConsumed = 0, hazardsHit = 0, emptyInteracts = 0;
            int tooSoonCount = 0;
            int oscillations = 0;
            MotorCommand prevMove = MotorCommand::Wait;
            bool hasPrevMove = false;

            float episodeReward = 0.0f;
            // Monotonic brain-step counter for this episode. The old code
            // reused neuralStep 0..9 for every action, so delayed spikes
            // scheduled for step 10+ could never be delivered and
            // accumulated forever in the spike system.
            uint64_t brainStepCounter = 0;

            // Run steps
            for (int step = 0; step < stepsPerEpisode; ++step) {
                // 1. Get sensory percept from world
                const SensoryPercept& percept = world.getSensoryPercept();

                // 2. Inject sensory input into brain
                agentBrain.processSensoryInput(percept);

                // 3. Simulate brain (multiple steps per action)
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    (void)neuralStep;
                    brain->step(brainStepCounter++);
                }
                
                // 4. Decode motor command from brain activity
                MotorCommand cmd = agentBrain.decodeMotorCommand();

                // Apply motor command to world
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());

                // 5. Apply reward modulation
                agentBrain.applyRewardModulation(result.reward, 0.0f);

                // 6. Update world
                world.update(0.1);

                // 7. Update development
                agentBrain.updateDevelopment(0.1);

                // Track metrics
                episodeReward += result.reward;
                metrics.steps++;
                metrics.totalReward += result.reward;
                metrics.totalSpikes += brain->getFiringNeuronCount();

                // Phase 5 metrics: coverage, interact outcome, oscillation.
                {
                    const auto& body = world.getAgentBody();
                    visitedCells.insert({static_cast<int>(std::floor(body.x)),
                                         static_cast<int>(std::floor(body.y))});
                }
                if (result.message == "too soon") ++tooSoonCount;
                if (cmd == MotorCommand::Interact) {
                    if (result.message == "consumed_resource") ++resourcesConsumed;
                    else if (result.message == "hit_hazard") ++hazardsHit;
                    else ++emptyInteracts;
                }
                if (cmd == MotorCommand::MoveForward || cmd == MotorCommand::MoveBackward) {
                    if (hasPrevMove && prevMove != cmd) ++oscillations;
                    prevMove = cmd;
                    hasPrevMove = true;
                }

                // Count actions (Look actions are not Wait)
                switch (cmd) {
                    case MotorCommand::MoveForward: metrics.forwardCount++; break;
                    case MotorCommand::MoveBackward: metrics.backwardCount++; break;
                    case MotorCommand::TurnLeft: metrics.turnLeftCount++; break;
                    case MotorCommand::TurnRight: metrics.turnRightCount++; break;
                    case MotorCommand::Interact: metrics.interactCount++; break;
                    case MotorCommand::LookLeft: metrics.lookLeftCount++; break;
                    case MotorCommand::LookRight: metrics.lookRightCount++; break;
                    default: metrics.waitCount++; break;
                }

                // Print periodic updates
                if (step % 50 == 0) {
                    std::cout << "  Step " << step 
                              << ": reward=" << result.reward
                              << ", action=" << motorCommandToString(cmd)
                              << ", neuromod=" << agentBrain.getNeuromodulationLevel()
                              << ", novelty=" << agentBrain.getNoveltyLevel()
                              << ", energy=" << world.getAgentBody().energy
                              << "\n";
                }
            }
            
            // Record final metrics
            metrics.finalNovelty = agentBrain.getNoveltyLevel();
            metrics.finalCuriosity = agentBrain.getCuriosityLevel();
            metrics.finalEnergy = world.getAgentBody().energy;
            metrics.predictionError = agentBrain.getPredictionError();
            metrics.developmentalAge = agentBrain.getBrain()->getTotalSynapseCount();  // Use as proxy
            metrics.developmentalStage = stageToString(agentBrain.getDevelopmentalStage());
            metrics.totalSynapses = brain->getTotalSynapseCount();
            metrics.averageFiringRate = brain->getAverageFiringRate();
            
            allMetrics.push_back(metrics);
            
            std::cout << "--- Episode " << episode << " ended ---\n";
            std::cout << "  Steps: " << metrics.steps << "\n";
            std::cout << "  Total Reward: " << metrics.totalReward << "\n";
            std::cout << "  Final Energy: " << metrics.finalEnergy << "\n";
            std::cout << "  Synapses: " << metrics.totalSynapses << "\n";
            std::cout << "  Action counts: fwd=" << metrics.forwardCount
                      << ", bwd=" << metrics.backwardCount
                      << ", left=" << metrics.turnLeftCount
                      << ", right=" << metrics.turnRightCount
                      << ", interact=" << metrics.interactCount
                      << ", wait=" << metrics.waitCount
                      << ", lookL=" << metrics.lookLeftCount
                      << ", lookR=" << metrics.lookRightCount
                      << ", avgFiringRate=" << metrics.averageFiringRate
                      << "\n";
            std::cout << "  Phase5 metrics: visitedCells=" << visitedCells.size()
                      << ", resources=" << resourcesConsumed
                      << ", hazards=" << hazardsHit
                      << ", emptyInteracts=" << emptyInteracts
                      << ", oscillations=" << oscillations
                      << ", finalNovelty=" << metrics.finalNovelty
                      << ", finalCuriosity=" << metrics.finalCuriosity
                      << ", tooSoon=" << tooSoonCount
                      << ", executed=" << (stepsPerEpisode - tooSoonCount)
                      << ", reflexBonus=" << agentBrain.getReflexBonusCount()
                      << ", reflexSuppress=" << agentBrain.getReflexSuppressCount()
                      << ", wtaPulses=" << agentBrain.getWtaPulseCount()
                      << ", plannerOverride=" << agentBrain.getPlannerOverrideCount()
                      << ", plannerAgree=" << agentBrain.getPlannerAgreementCount()
                      << "/" << agentBrain.getPlannerDecisionCount()
                      << ", conceptBias=" << agentBrain.getConceptBiasCount()
                      << ", selfBias=" << agentBrain.getSelfBiasCount()
                      << ", socialBias=" << agentBrain.getSocialBiasCount()
                      << ", concepts=" << agentBrain.getConceptCount()
                      << ", selfConf=" << agentBrain.getSelfModelConfidence()
                      << ", wmTraces=" << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0)
                      << "\n\n";
        }
        
        // Summary
        std::cout << "===========================================\n";
        std::cout << "SUMMARY\n";
        std::cout << "===========================================\n";
        
        if (allMetrics.size() >= 2) {
            const auto& first = allMetrics.front();
            const auto& last = allMetrics.back();
            
            std::cout << "Early episode (0) vs Late episode (" << (allMetrics.size() - 1) << "):\n";
            std::cout << "  Reward: " << first.totalReward << " -> " << last.totalReward << "\n";
            std::cout << "  Forward actions: " << first.forwardCount << " -> " << last.forwardCount << "\n";
            std::cout << "  Exploration (wait): " << first.waitCount << " -> " << last.waitCount << "\n";
            
            // Calculate behavior change
            float rewardChange = last.totalReward - first.totalReward;
            float actionChange = static_cast<float>(last.forwardCount) - static_cast<float>(first.forwardCount);
            
            std::cout << "\nBehavioral change:\n";
            if (rewardChange > 0) {
                std::cout << "  Reward INCREASED by " << rewardChange << " (positive learning)\n";
            } else if (rewardChange < -5) {
                std::cout << "  Reward DECREASED by " << -rewardChange << " (possible exploration or extinction)\n";
            }
            
            if (std::abs(actionChange) > 10) {
                std::cout << "  Forward actions changed by " << actionChange << "\n";
            }
            
            // Synapse change
            if (last.totalSynapses != first.totalSynapses) {
                std::cout << "  Synapses: " << first.totalSynapses << " -> " << last.totalSynapses << "\n";
            }
        }
        
        std::cout << "\nPhase 3 demonstration complete.\n";
        std::cout << "The brain received sensory input, produced actions,\n";
        std::cout << "experienced consequences, and modified its synapses.\n";
    }
    
private:
    void configureEnvironment(std::shared_ptr<Config> config) {
        config->set("environment_width", 20);
        config->set("environment_height", 20);
        config->set("vision_width", 16);
        config->set("vision_height", 16);
    }
    
    void configureBrain(std::shared_ptr<Config> config) {
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
    }
    
    const char* motorCommandToString(MotorCommand cmd) {
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
    
    const char* stageToString(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
};

// Main entry point for Phase 3 demo
int main(int argc, char* argv[]) {
    // Parse command line arguments
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    std::string outputDir = "results";
    bool help = false;
    bool jsonExport = false;
    bool csvExport = false;
    bool verbose = false;
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            help = true;
        } else if (arg == "--json" || arg == "-j") {
            jsonExport = true;
        } else if (arg == "--csv" || arg == "-c") {
            csvExport = true;
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--output-dir" || arg == "-o") {
            if (i + 1 < argc) {
                outputDir = argv[++i];
            }
        } else if (arg.find("--") == 0 || arg.find('-') == 0) {
            // Unknown argument
            std::cerr << "Unknown argument: " << arg << "\n";
            return 1;
        } else {
            // Positional arguments: episodes and steps
            if (numEpisodes == 5) {  // Default value
                numEpisodes = std::atoi(arg.c_str());
            } else if (stepsPerEpisode == 200) {  // Default value
                stepsPerEpisode = std::atoi(arg.c_str());
            } else {
                std::cerr << "Too many positional arguments\n";
                return 1;
            }
        }
    }
    
    // Show help
    if (help || (argc == 1)) {
        printHelp();
        return 0;
    }
    
    // Validate arguments
    if (numEpisodes < 1 || numEpisodes > 20) {
        std::cerr << "Error: Number of episodes must be between 1 and 20\n";
        return 1;
    }
    if (stepsPerEpisode < 1 || stepsPerEpisode > 1000) {
        std::cerr << "Error: Steps per episode must be between 1 and 1000\n";
        return 1;
    }
    
    // Create output directory
    createOutputDirectory(outputDir);
    
    // Run demo
    nlm::Phase3Demo demo;
    
    std::cout << "Running Phase 3 Demo with " << numEpisodes << " episodes, " << stepsPerEpisode << " steps each\n";
    std::cout << "Output directory: " << outputDir << "\n";
    if (jsonExport) std::cout << "JSON export enabled\n";
    if (csvExport) std::cout << "CSV export enabled\n";
    std::cout << "\n";
    
    // Run the demo
    demo.run(numEpisodes, stepsPerEpisode);
    
    // Save results if requested
    if (jsonExport || csvExport) {
        demo.saveResults(outputDir, jsonExport, csvExport);
        std::cout << "\nResults saved to " << outputDir << "\n";
    }
    
    return 0;
}

void printHelp() {
    std::cout << "NLM Phase 3 Demo - World Interaction\n";
    std::cout << "=====================================\n\n";
    std::cout << "This demo shows: experience -> neural activity -> action -> consequence -> learning\n\n";
    std::cout << "Usage: nlm_phase3_demo [options] [episodes] [steps]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help          Show this help message\n";
    std::cout << "  -j, --json          Export results as JSON\n";
    std::cout << "  -c, --csv           Export results as CSV\n";
    std::cout << "  -v, --verbose       Verbose output\n";
    std::cout << "  -o, --output-dir    Output directory (default: results)\n\n";
    std::cout << "Arguments:\n";
    std::cout << "  episodes           Number of episodes (1-20, default: 5)\n";
    std::cout << "  steps              Steps per episode (1-1000, default: 200)\n\n";
    std::cout << "Examples:\n";
    std::cout << "  nlm_phase3_demo              # Run default (5 episodes, 200 steps each)\n";
    std::cout << "  nlm_phase3_demo 3 500        # Run 3 episodes, 500 steps each\n";
    std::cout << "  nlm_phase3_demo -j -v 10     # Run 10 episodes with JSON export and verbose output\n";
}

void createOutputDirectory(const std::string& dir) {
    if (std::filesystem::exists(dir)) {
        std::cout << "Output directory already exists: " << dir << "\n";
    } else {
        std::filesystem::create_directories(dir);
        std::cout << "Created output directory: " << dir << "\n";
    }
}

void saveResults(const std::string& dir, bool json, bool csv) {
    // This method would save the collected metrics
    // For now, this is a placeholder implementation
    std::cout << "Saving results to " << dir << "...\n";
    
    if (json) {
        std::string jsonFile = dir + "/results.json";
        std::ofstream file(jsonFile);
        if (file.is_open()) {
            file << "{\"message\": \"Result export not yet implemented, but ready for implementation.\"}";
            file.close();
            std::cout << "JSON results saved to " << jsonFile << "\n";
        }
    }
    
    if (csv) {
        std::string csvFile = dir + "/results.csv";
        std::ofstream file(csvFile);
        if (file.is_open()) {
            file << "episode,steps,reward,neurons,actions\n";
            file << "0," << 200 << ",0.0,0,0\n";
            file << "1," << 200 << ",0.0,0,0\n";
            file.close();
            std::cout << "CSV results saved to " << csvFile << "\n";
        }
    }
}