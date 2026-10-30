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
#include <ctime>
#include <iomanip>

using namespace nlm;

// Enhanced Phase 3 demo with unified CLI support
class EnhancedPhase3Demo {
public:
    EnhancedPhase3Demo() = default;
    ~EnhancedPhase3Demo() = default;
    
    // Enhanced demo with CLI support
    bool run(int argc, char* argv[]) {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - ENHANCED WORLD INTERACTION DEMO\n";
        std::cout << "===========================================\n\n";
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Create unified CLI instance
        UnifiedCLI cli;
        
        // Parse command line arguments
        if (!cli.parseArgs(argc, argv)) {
            std::cerr << "Error parsing command line arguments" << std::endl;
            return false;
        }
        
        // Handle help/version
        if (cli.getConfig().help) {
            cli.printHelp(argv[0]);
            return true;
        }
        
        if (cli.getConfig().version) {
            cli.printVersion();
            return true;
        }
        
        // Parse standard demo arguments
        int numEpisodes = 5;
        int stepsPerEpisode = 200;
        int numTrials = 50;
        int maxSteps = 2000;
        
        if (!cli.parseDemoArgs(argc, argv, numEpisodes, stepsPerEpisode, numTrials, maxSteps)) {
            std::cerr << "Error parsing demo arguments" << std::endl;
            return false;
        }
        
        // Override with CLI arguments if provided
        auto it = cli.getConfig().customArgs.find("episodes");
        if (it != cli.getConfig().customArgs.end()) {
            numEpisodes = std::atoi(it->second.c_str());
        }
        
        it = cli.getConfig().customArgs.find("steps");
        if (it != cli.getConfig().customArgs.end()) {
            stepsPerEpisode = std::atoi(it->second.c_str());
        }
        
        it = cli.getConfig().customArgs.find("trials");
        if (it != cli.getConfig().customArgs.end()) {
            numTrials = std::atoi(it->second.c_str());
        }
        
        it = cli.getConfig().customArgs.find("max_steps");
        if (it != cli.getConfig().customArgs.end()) {
            maxSteps = std::atoi(it->second.c_str());
        }
        
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Episodes: " << numEpisodes << std::endl;
        std::cout << "  Steps per episode: " << stepsPerEpisode << std::endl;
        std::cout << "  Trials: " << numTrials << std::endl;
        std::cout << "  Max steps: " << maxSteps << std::endl;
        std::cout << "  Output format: " << cli.outputFormatToString(cli.getConfig().outputFormat) << std::endl;
        std::cout << "  Output file: " << (cli.getConfig().outputFile.empty() ? "(console)" : cli.getConfig().outputFile) << std::endl;
        std::cout << std::endl;
        
        // Create configuration
        auto config = std::make_shared<Config>();
        configureEnvironment(config);
        configureBrain(config);
        
        // Apply CLI custom arguments
        cli.processConfig(config);
        
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
        
        std::cout << "Initial brain state:" << std::endl;
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << std::endl;
        std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << std::endl;
        std::cout << std::endl;
        
        // Metrics tracking
        std::vector<ExperimentMetrics> allMetrics;
        
        // Run episodes
        for (int episode = 0; episode < numEpisodes; ++episode) {
            std::cout << "--- Episode " << episode << " started ---" << std::endl;
            
            // Reset world
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
                if (step % 50 == 0) {
                    std::cout << "  Step " << step 
                              << ": reward=" << result.reward
                              << ", action=" << motorCommandToString(cmd)
                              << ", neuromod=" << agentBrain.getNeuromodulationLevel()
                              << ", novelty=" << agentBrain.getNoveltyLevel()
                              << ", energy=" << world.getAgentBody().energy
                              << std::endl;
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
            
            std::cout << "--- Episode " << episode << " ended ---" << std::endl;
            std::cout << "  Steps: " << metrics.steps << std::endl;
            std::cout << "  Total Reward: " << metrics.totalReward << std::endl;
            std::cout << "  Final Energy: " << metrics.finalEnergy << std::endl;
            std::cout << "  Synapses: " << metrics.totalSynapses << std::endl;
            std::cout << "  Action counts: fwd=" << metrics.forwardCount 
                      << ", bwd=" << metrics.backwardCount
                      << ", left=" << metrics.turnLeftCount
                      << ", right=" << metrics.turnRightCount
                      << ", interact=" << metrics.interactCount
                      << ", wait=" << metrics.waitCount
                      << std::endl;
        }
        
        // Summary
        std::cout << "===========================================" << std::endl;
        std::cout << "SUMMARY" << std::endl;
        std::cout << "===========================================" << std::endl;
        
        if (allMetrics.size() >= 2) {
            const auto& first = allMetrics.front();
            const auto& last = allMetrics.back();
            
            std::cout << "Early episode (0) vs Late episode (" << (allMetrics.size() - 1) << "):" << std::endl;
            std::cout << "  Reward: " << first.totalReward << " -> " << last.totalReward << std::endl;
            std::cout << "  Forward actions: " << first.forwardCount << " -> " << last.forwardCount << std::endl;
            std::cout << "  Exploration (wait): " << first.waitCount << " -> " << last.waitCount << std::endl;
            
            // Calculate behavior change
            float rewardChange = last.totalReward - first.totalReward;
            float actionChange = static_cast<float>(last.forwardCount) - static_cast<float>(first.forwardCount);
            
            std::cout << std::endl;
            std::cout << "Behavioral change:" << std::endl;
            if (rewardChange > 0) {
                std::cout << "  Reward INCREASED by " << rewardChange << " (positive learning)" << std::endl;
            } else if (rewardChange < -5) {
                std::cout << "  Reward DECREASED by " << -rewardChange << " (possible exploration or extinction)" << std::endl;
            }
            
            if (std::abs(actionChange) > 10) {
                std::cout << "  Forward actions changed by " << actionChange << std::endl;
            }
            
            // Synapse change
            if (last.totalSynapses != first.totalSynapses) {
                std::cout << "  Synapses: " << first.totalSynapses << " -> " << last.totalSynapses << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "Phase 3 demonstration complete." << std::endl;
        std::cout << "The brain received sensory input, produced actions," << std::endl;
        std::cout << "experienced consequences, and modified its synapses." << std::endl;
        
        // Export metrics if requested
        if (cli.getConfig().exportMetrics) {
            exportMetrics(allMetrics, "phase3_metrics.csv");
        }
        
        return true;
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
    
    void exportMetrics(const std::vector<ExperimentMetrics>& metrics, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open file for metrics export: " << filename << std::endl;
            return;
        }
        
        file << "episode,steps,total_reward,total_spikes,forward_actions,backward_actions,left_actions,right_actions,interact_actions,wait_actions,final_novelty,final_curiosity,final_energy,prediction_error,total_synapses,average_firing_rate" << std::endl;
        
        for (const auto& metric : metrics) {
            file << metric.episode << ","
                 << metric.steps << ","
                 << metric.totalReward << ","
                 << metric.totalSpikes << ","
                 << metric.forwardCount << ","
                 << metric.backwardCount << ","
                 << metric.turnLeftCount << ","
                 << metric.turnRightCount << ","
                 << metric.interactCount << ","
                 << metric.waitCount << ","
                 << metric.finalNovelty << ","
                 << metric.finalCuriosity << ","
                 << metric.finalEnergy << ","
                 << metric.predictionError << ","
                 << metric.totalSynapses << ","
                 << metric.averageFiringRate << std::endl;
        }
        
        file.close();
        std::cout << "Metrics exported to: " << filename << std::endl;
    }
};

int main(int argc, char* argv[]) {
    EnhancedPhase3Demo demo;
    return demo.run(argc, argv) ? 0 : 1;
}