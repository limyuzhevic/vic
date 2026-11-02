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
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

namespace nlm {

// Command line argument parsing for Phase 3 demo
struct Phase3Args {
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    bool help = false;
    bool version = false;
    std::string configFile = "configs/default.cfg";
    std::string outputFile;
    bool validate = false;
    bool verbose = false;
    bool quiet = false;
};

// Print help for Phase 3 demo
void printHelp() {
    std::cout << "===========================================\n";
    std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
    std::cout << "===========================================\n\n";
    std::cout << "USAGE:\n";
    std::cout << "  nlm_phase3_demo [OPTIONS]\n\n";
    std::cout << "OPTIONS:\n";
    std::cout << "  --help                      Show this help message and exit\n";
    std::cout << "  --version                   Show version information and exit\n";
    std::cout << "  --config <file>             Load configuration from file\n";
    std::cout << "  --output <file>             Save results to output file\n";
    std::cout << "  --validate                  Validate configuration before running\n";
    std::cout << "  --verbose, -v               Enable verbose output\n";
    std::cout << "  --quiet, -q                 Suppress non-essential output\n";
    std::cout << "  --episodes <n>              Number of episodes (default: 5)\n";
    std::cout << "  --steps-per-episode <n>     Steps per episode (default: 200)\n\n";
    std::cout << "EXAMPLES:\n";
    std::cout << "  nlm_phase3_demo --help                        Show help\n";
    std::cout << "  nlm_phase3_demo --config my_config.json        Load JSON config\n";
    std::cout << "  nlm_phase3_demo --episodes 10 --steps-per-episode 300 Run with 10 episodes of 300 steps each\n";
    std::cout << "  nlm_phase3_demo --validate --output results.json Validate and save results\n\n";
    std::cout << "The Phase 3 demo shows: experience -> neural activity -> action -> consequence -> learning\n";
}

// Print version information for Phase 3 demo
void printVersion() {
    std::cout << "===========================================\n";
    std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
    std::cout << "===========================================\n\n";
    std::cout << "Version: 0.2.0 (Phase 3: World Interaction)\n";
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << "\n\n";
    std::cout << "Phase 3 demonstrates world interaction with:\n";
    std::cout << "  - Agent brain interface for action selection\n";
    std::cout << "  - Simple world environment with navigation\n";
    std::cout << "  - Reward-based learning and plasticity\n";
    std::cout << "  - Development and curiosity-driven exploration\n\n";
    std::cout << "This demo shows how neural systems learn from experience in dynamic environments.\n";
}

// Parse command line arguments for Phase 3 demo
Phase3Args parseArguments(int argc, char* argv[]) {
    Phase3Args args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            args.help = true;
        } else if (arg == "--version") {
            args.version = true;
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            args.quiet = true;
        } else if (arg == "--validate") {
            args.validate = true;
        } else if (arg.find("--config=") == 0) {
            args.configFile = arg.substr(9);
        } else if (arg == "--config" && i + 1 < argc) {
            args.configFile = argv[++i];
        } else if (arg.find("--output=") == 0) {
            args.outputFile = arg.substr(9);
        } else if (arg == "--output" && i + 1 < argc) {
            args.outputFile = argv[++i];
        } else if (arg.find("--episodes=") == 0) {
            args.numEpisodes = std::stoi(arg.substr(11));
        } else if (arg == "--episodes" && i + 1 < argc) {
            args.numEpisodes = std::stoi(argv[++i]);
        } else if (arg.find("--steps-per-episode=") == 0) {
            args.stepsPerEpisode = std::stoi(arg.substr(21));
        } else if (arg == "--steps-per-episode" && i + 1 < argc) {
            args.stepsPerEpisode = std::stoi(argv[++i]);
        } else if (arg[0] == '-') {
            // Unknown option
            continue;
        } else {
            // Unknown positional argument
            continue;
        }
    }
    
    return args;
}

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
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
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

                // Instrumentation (opt-in, off by default): log the exact
                // quantities the Wait-default hypothesis depends on, for
                // episode 0 only. Set NLM_PHASE3_INSTRUMENT=1 to enable.
                if (episode == 0 && std::getenv("NLM_PHASE3_INSTRUMENT")) {
                    auto acts = agentBrain.getMotorGroupActivities();
                    size_t visionSeen = 0;
                    for (float v : percept.getVision()) {
                        if (v < 0.999f) ++visionSeen;
                    }
                    std::cout << "INSTR step=" << step
                              << " firing=" << brain->getFiringNeuronCount()
                              << " avgRate=" << brain->getAverageFiringRate()
                              << " visionSeen=" << visionSeen << "/"
                              << percept.getVision().size();
                    for (const auto& a : acts) {
                        std::cout << " " << a.name << "=" << a.activity
                                  << "/rate=" << a.firingRate
                                  << "(n=" << a.size << ",refr=" << a.refractoryCount << ")";
                    }
                    std::cout << " cmd=" << motorCommandToString(cmd) << "\n";
                }
                
                // 5. Apply motor command to world
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());

                // 6. Apply reward modulation
                agentBrain.applyRewardModulation(result.reward, 0.0f);
                
                // 7. Update world — Phase 6 pacing fix: one decision
                // is one world action interval (0.1 s). The old
                // 0.01 s step meant the world's 0.1 s action gate
                // rejected 181/200 decisions as "too soon", so the
                // brain was judged on actions it never took. The
                // world gate itself is unchanged; the demo now
                // actually steps the world by the interval the
                // world's own physics (3.0 units/s, 2.0 rad/s) is
                // defined over.
                world.update(0.1);

                // 8. Update development
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
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
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
        
        std::cout << "\nPerformance:\n";
        std::cout << "  Total runtime: " << duration.count() << " ms\n";
        std::cout << "  Episodes: " << numEpisodes << "\n";
        std::cout << "  Steps per episode: " << stepsPerEpisode << "\n";
        std::cout << "  Total steps: " << numEpisodes * stepsPerEpisode << "\n";
        
        std::cout << "\nPhase 3 complete.\n";
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
    Phase3Args args = parseArguments(argc, argv);
    
    // Handle special cases
    if (args.help) {
        printHelp();
        return 0;
    }
    
    if (args.version) {
        printVersion();
        return 0;
    }
    
    nlm::Phase3Demo demo;
    demo.run(args.numEpisodes, args.stepsPerEpisode);
    
    return 0;
}

} // namespace nlm

// Main entry point for Phase 3 demo
int main(int argc, char* argv[]) {
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    
    if (argc > 1) {
        numEpisodes = std::atoi(argv[1]);
    }
    if (argc > 2) {
        stepsPerEpisode = std::atoi(argv[2]);
    }
    
    nlm::Phase3Demo demo;
    demo.run(numEpisodes, stepsPerEpisode);
    
    return 0;
}
