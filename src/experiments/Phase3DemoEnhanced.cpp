// NLM Phase 3 Demo with Enhanced CLI
// Updated to use the new unified CLI system

#include "Phase3Experiment.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/CLI.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <set>
#include <utility>

using namespace nlm;

// Common options structure for Phase 3
struct Phase3Options {
    int numEpisodes;
    int stepsPerEpisode;
    bool verbose;
    bool quiet;
    std::string logLevel;
    bool enablePlasticity;
    bool enableDevelopment;
    bool enableCuriosity;
    bool enableRewardModulation;
    double timestep;
    int neuronCount;
    int regionCount;
    double connectionProbability;
    double stdpLtpWeight;
    double stdpLtdWeight;
    double stdpTau;
    double synaptogenesisRate;
    double pruningRate;
    std::string outputFile;
    bool enableCheckpointing;
    std::string checkpointFile;
    bool help;
    bool version;
    std::string environmentWidth;
    std::string environmentHeight;
    std::string visionWidth;
    std::string visionHeight;
};

// Parse command line arguments for Phase 3
Phase3Options parsePhase3Options(int argc, char* argv[]) {
    Phase3Options options;
    
    // Set defaults
    options.numEpisodes = 5;
    options.stepsPerEpisode = 200;
    options.verbose = false;
    options.quiet = false;
    options.logLevel = "info";
    options.enablePlasticity = true;
    options.enableDevelopment = true;
    options.enableCuriosity = true;
    options.enableRewardModulation = true;
    options.timestep = 0.001;
    options.neuronCount = 500;
    options.regionCount = 2;
    options.connectionProbability = 0.1f;
    options.stdpLtpWeight = 0.01f;
    options.stdpLtdWeight = 0.012f;
    options.stdpTau = 20.0f;
    options.synaptogenesisRate = 0.0001f;
    options.pruningRate = 0.00001f;
    options.outputFile = "";
    options.enableCheckpointing = false;
    options.checkpointFile = "/tmp/nlm_phase3.ckpt";
    options.help = false;
    options.version = false;
    options.environmentWidth = "20";
    options.environmentHeight = "20";
    options.visionWidth = "16";
    options.visionHeight = "16";
    
    // Parse common options first
    auto commonOptions = parseCommonOptions(argc, argv, "nlm_phase3_demo");
    
    // Override with specific options (simple parsing for now)
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version") {
            options.version = true;
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
        } else if (arg.substr(0, 2) == "--") {
            std::string opt = arg.substr(2);
            size_t equalsPos = opt.find('=');
            if (equalsPos != std::string::npos) {
                std::string value = opt.substr(equalsPos + 1);
                opt = opt.substr(0, equalsPos);
                
                if (opt == "episodes") options.numEpisodes = std::stoi(value);
                else if (opt == "steps") options.stepsPerEpisode = std::stoi(value);
                else if (opt == "neuron-count") options.neuronCount = std::stoi(value);
                else if (opt == "region-count") options.regionCount = std::stoi(value);
                else if (opt == "connection-prob") options.connectionProbability = std::stof(value);
                else if (opt == "stdp-ltp") options.stdpLtpWeight = std::stof(value);
                else if (opt == "stdp-ltd") options.stdpLtdWeight = std::stof(value);
                else if (opt == "stdp-tau") options.stdpTau = std::stof(value);
                else if (opt == "env-width") options.environmentWidth = value;
                else if (opt == "env-height") options.environmentHeight = value;
                else if (opt == "vision-width") options.visionWidth = value;
                else if (opt == "vision-height") options.visionHeight = value;
                else if (opt == "timestep") options.timestep = std::stod(value);
            }
        }
    }
    
    // Set flags from common options
    options.enablePlasticity = commonOptions.enablePlasticity;
    options.enableDevelopment = commonOptions.enableDevelopment;
    options.enableCuriosity = commonOptions.enableCuriosity;
    options.enableRewardModulation = commonOptions.enableRewardModulation;
    options.enableCheckpointing = commonOptions.enableCheckpointing;
    
    return options;
}

void printPhase3Help() {
    std::cout << "NLM Phase 3 Demo - Enhanced CLI" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm_phase3_demo [OPTIONS] [EPISODES] [STEPS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Arguments:" << std::endl;
    std::cout << "  EPISODES        Number of episodes to run (default: 5)" << std::endl;
    std::cout << "  STEPS           Steps per episode (default: 200)" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h      Show this help message" << std::endl;
    std::cout << "  --version       Show version information" << std::endl;
    std::cout << "  --verbose, -v   Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q     Quiet mode (suppress info messages)" << std::endl;
    std::cout << "  --plasticity    Enable synaptic plasticity" << std::endl;
    std::cout << "  --no-plasticity Disable synaptic plasticity" << std::endl;
    std::cout << "  --development   Enable developmental processes" << std::endl;
    std::cout << "  --no-development Disable developmental processes" << std::endl;
    std::cout << "  --curiosity     Enable curiosity-driven exploration" << std::endl;
    std::cout << "  --no-curiosity  Disable curiosity-driven exploration" << std::endl;
    std::cout << "  --reward        Enable reward modulation" << std::endl;
    std::cout << "  --no-reward     Disable reward modulation" << std::endl;
    std::cout << "  --checkpoint     Enable checkpointing" << std::endl;
    std::cout << "  --no-checkpoint  Disable checkpointing" << std::endl;
    std::cout << "  --config FILE   Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --output FILE   Output file for results" << std::endl;
    std::cout << "  --timestep VAL  Simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --neuron-count N Number of neurons (default: 500)" << std::endl;
    std::cout << "  --region-count R Number of regions (default: 2)" << std::endl;
    std::cout << "  --connection-prob P Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --stdp-ltp W    STDP LTP weight (default: 0.01)" << std::endl;
    std::cout << "  --stdp-ltd W    STDP LTD weight (default: 0.012)" << std::endl;
    std::cout << "  --stdp-tau T    STDP time constant (default: 20.0)" << std::endl;
    std::cout << "  --env-width W   Environment width (default: 20)" << std::endl;
    std::cout << "  --env-height H  Environment height (default: 20)" << std::endl;
    std::cout << "  --vision-width W Vision width (default: 16)" << std::endl;
    std::cout << "  --vision-height H Vision height (default: 16)" << std::endl;
    std::cout << std::endl;
    std::cout << "The enhanced CLI provides better help, more configuration options,
 and improved user experience." << std::endl;
}

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
        world.configure(std::stod(options.environmentWidth), std::stod(options.environmentHeight),
                       std::stoi(options.visionWidth), std::stoi(options.visionHeight));
        world.setRandomSeed(42);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Configure agent brain based on options
        agentBrain.enableRewardModulation(options.enableRewardModulation);
        agentBrain.enableStructuralPlasticity(options.enablePlasticity);
        agentBrain.enableDevelopment(options.enableDevelopment);
        agentBrain.enableCuriosity(options.enableCuriosity);
        
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
    Phase3Options options;
    
    void configureEnvironment(std::shared_ptr<Config> config) {
        config->set("environment_width", std::stod(options.environmentWidth));
        config->set("environment_height", std::stod(options.environmentHeight));
        config->set("vision_width", std::stoi(options.visionWidth));
        config->set("vision_height", std::stoi(options.visionHeight));
    }
    
    void configureBrain(std::shared_ptr<Config> config) {
        config->set("neuron_count", options.neuronCount);
        config->set("region_count", options.regionCount);
        config->set("connection_probability", options.connectionProbability);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", options.timestep);
        config->set("stdp_ltp_weight", options.stdpLtpWeight);
        config->set("stdp_ltd_weight", options.stdpLtdWeight);
        config->set("stdp_tau", options.stdpTau);
        config->set("synaptogenesis_rate", options.synaptogenesisRate);
        config->set("pruning_rate", options.pruningRate);
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
    Phase3Options options = parsePhase3Options(argc, argv);
    
    if (options.help) {
        printPhase3Help();
        return 0;
    }
    
    if (options.version) {
        std::cout << "NLM Phase 3 Demo - Version 0.1.0" << std::endl;
        return 0;
    }
    
    // Setup logging
    setupCommonLogging(options.logLevel, options.verbose, options.quiet);
    
    // Create and run demo
    Phase3Demo demo;
    demo.run(options.numEpisodes, options.stepsPerEpisode);
    
    return 0;
}

} // namespace nlm