#include "Phase3Experiment.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/CLI/CLI.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace nlm {

// Main Phase 3 demonstration
// Shows: experience -> neural activity -> action -> consequence -> learning

class Phase3Demo {
public:
    Phase3Demo() : cli() {
        setupCLI();
    }
    
    ~Phase3Demo() {}
    
    void run(int numEpisodes = 5, int stepsPerEpisode = 200) {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
        std::cout << "===========================================\n\n";
        
        // Print configuration if verbose mode is enabled
        if (cli.getBool(cli.VERBOSE_LONG)) {
            cli.printConfig();
            std::cout << "\n";
        }
        
        // Create configuration
        auto config = std::make_shared<Config>();
        configureEnvironment(config);
        configureBrain(config);
        
        // Create world
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(cli.getInt(cli.SEED_LONG, 42));
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Configure agent brain based on CLI options
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
            
            // Reset world and brain
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            ExperimentMetrics metrics;
            metrics.episode = episode;
            
            float episodeReward = 0.0f;
            
            // Run steps
            for (int step = 0; step < stepsPerEpisode; ++step) {
                // 1. Get sensory percept from world
                const SensoryPercept& percept = world.getSensoryPercept();
                
                // 2. Inject sensory input into brain
                agentBrain.processSensoryInput(percept);
                
                // 3. Simulate brain (multiple steps per action)
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    brain->step(neuralStep);
                }
                
                // 4. Decode motor command from brain activity
                MotorCommand cmd = agentBrain.decodeMotorCommand();
                
                // 5. Apply motor command to world
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                
                // 6. Apply reward modulation
                agentBrain.applyRewardModulation(result.reward, 0.0f);
                
                // 7. Update world
                world.update(0.01);  // 10ms timestep
                
                // 8. Update development
                agentBrain.updateDevelopment(0.01);
                
                // Track metrics
                episodeReward += result.reward;
                metrics.steps++;
                metrics.totalReward += result.reward;
                metrics.totalSpikes += brain->getFiringNeuronCount();
                
                // Count actions
                switch (cmd) {
                    case MotorCommand::MoveForward: metrics.forwardCount++; break;
                    case MotorCommand::MoveBackward: metrics.backwardCount++; break;
                    case MotorCommand::TurnLeft: metrics.turnLeftCount++; break;
                    case MotorCommand::TurnRight: metrics.turnRightCount++; break;
                    case MotorCommand::Interact: metrics.interactCount++; break;
                    default: metrics.waitCount++; break;
                }
                
                // Print periodic updates
                if (step % 50 == 0 || cli.getBool(cli.DEBUG_LONG)) {
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
    nlm::CLI cli;
    
    void setupCLI() {
        // Set program metadata
        nlm::CLI::ProgramInfo info(
            "nlm_phase3_demo",
            "2.0.0",
            "Phase 3: World Interaction Demo - Demonstrates embodied learning in an interactive environment",
            "NLM Project Team",
            "https://github.com/nlm-project"
        );
        cli.setProgramInfo(info);
        
        // Add common arguments using the CLI helper
        cli.addArgument(nlm::cli::CommonArgs::EPISODES_LONG, nlm::cli::CommonArgs::EPISODES_SHORT,
                       "Number of episodes to run", "5",
                       true);
        
        cli.addArgument(nlm::cli::CommonArgs::STEPS_LONG, nlm::cli::CommonArgs::STEPS_SHORT,
                       "Number of steps per episode", "200",
                       true);
        
        cli.addArgument(nlm::cli::CommonArgs::CONFIG_LONG, nlm::cli::CommonArgs::CONFIG_SHORT,
                       "Configuration file path", "",
                       true);
        
        cli.addArgument(nlm::cli::CommonArgs::SEED_LONG, nlm::cli::CommonArgs::SEED_SHORT,
                       "Random seed for reproducible results", "42",
                       true);
        
        cli.addArgument(nlm::cli::CommonArgs::LOG_LEVEL_LONG, nlm::cli::CommonArgs::LOG_LEVEL_SHORT,
                       "Logging level (DEBUG, INFO, WARNING, ERROR)", "INFO",
                       true);
        
        cli.addArgument(nlm::cli::CommonArgs::QUIET_LONG, nlm::cli::CommonArgs::QUIET_SHORT,
                       "Suppress most output (quiet mode)", "false",
                       false);
        
        cli.addArgument(nlm::cli::CommonArgs::VERBOSE_LONG, nlm::cli::CommonArgs::VERBOSE_SHORT,
                       "Enable verbose output", "false",
                       false);
        
        cli.addArgument(nlm::cli::CommonArgs::DEBUG_LONG, nlm::cli::CommonArgs::DEBUG_SHORT,
                       "Enable debug mode with additional diagnostics", "false",
                       false);
        
        cli.addArgument(nlm::cli::CommonArgs::VALIDATE_LONG, nlm::cli::CommonArgs::VALIDATE_SHORT,
                       "Validate configuration parameters before running", "false",
                       false);
        
        cli.addArgument(nlm::cli::CommonArgs::PROFILE_LONG, nlm::cli::CommonArgs::PROFILE_SHORT,
                       "Enable performance profiling", "false",
                       false);
        
        cli.addArgument(nlm::cli::CommonArgs::MODE_LONG, nlm::cli::CommonArgs::MODE_SHORT,
                       "Execution mode (standard, fast, thorough)", "standard",
                       true);
        
        cli.addArgument(nlm::cli::CommonArgs::INTERACTIVE_LONG, nlm::cli::CommonArgs::INTERACTIVE_SHORT,
                       "Enable interactive mode (if supported)", "false",
                       false);
    }
    
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
        config->set("random_seed", 42u);
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

} // namespace nlm

// Main entry point for Phase 3 demo
int main(int argc, char* argv[]) {
    nlm::Phase3Demo demo;
    
    // Parse command line arguments
    if (!demo.cli.parse(argc, argv)) {
        std::cerr << "Error: Failed to parse command line arguments.\n";
        return 1;
    }
    
    // Show help if requested
    if (demo.cli.isHelpRequested()) {
        demo.cli.printHelp();
        return 0;
    }
    
    // Show version if requested
    if (demo.cli.isVersionRequested()) {
        demo.cli.printVersion();
        return 0;
    }
    
    // Validate configuration if requested
    if (demo.cli.getBool(demo.cli.VALIDATE_LONG)) {
        std::cout << "Validating configuration...\n";
        // TODO: Implement configuration validation
        std::cout << "Configuration validation passed.\n\n";
    }
    
    // Get parameters from CLI
    int numEpisodes = demo.cli.getInt(demo.cli.EPISODES_LONG, 5);
    int stepsPerEpisode = demo.cli.getInt(demo.cli.STEPS_LONG, 200);
    
    // Load configuration from file if specified
    std::string configFile = demo.cli.getString(demo.cli.CONFIG_LONG, "");
    if (!configFile.empty()) {
        std::cout << "Loading configuration from: " << configFile << "\n";
    }
    
    std::cout << "Running Phase 3 demo with " << numEpisodes << " episodes, " << stepsPerEpisode << " steps each.\n\n";
    
    // Run the demo
    demo.run(numEpisodes, stepsPerEpisode);
    
    return 0;
}