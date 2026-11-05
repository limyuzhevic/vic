// Enhanced Phase 3 demo application with unified CLI and configuration
// This file has been completely refactored with:
// - Unified CLI framework integration
// - Enhanced configuration system
// - Performance monitoring
// - Results export functionality
// - Improved error handling and logging

#include "Phase3Experiment.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/CLI/UnifiedCLI.hpp"
#include "../core/Logging/AdvancedLogger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <set>
#include <utility>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>

namespace nlm {

// Enhanced Phase 3 demo application with unified CLI and configuration
class EnhancedPhase3Demo : public DemoApp {
private:
    size_t numEpisodes_;
    size_t stepsPerEpisode_;
    std::chrono::steady_clock::time_point startTime_;
    PerformanceMonitor perfMonitor_;
    AdvancedLogger logger_;
    
public:
    EnhancedPhase3Demo()
        : DemoApp("nlm_phase3_demo", "1.0.0", "NLM Phase 3 - Enhanced World Interaction Demo\nShows: experience -> neural activity -> action -> consequence -> learning"),
          numEpisodes_(5), stepsPerEpisode_(200) {
        
        // Set up logger
        logger_.setLogToFile("phase3_demo.log");
        logger_.setLogLevel("INFO");
        
        // Add demo-specific CLI options
        getCLI()->addOption(CLI::CLIOption("episodes", 'e', "Number of episodes", true, "5", "int"));
        getCLI()->addOption(CLI::CLIOption("steps", 's', "Number of steps per episode", true, "200", "int"));
        getCLI()->addOption(CLI::CLIOption("config-file", '\0', "Load configuration from file", true, "", "filename"));
        getCLI()->addOption(CLI::CLIOption("results-output", '\0', "Export results to file", true, "", "filename"));
        getCLI()->addOption(CLI::CLIOption("benchmark", 'b', "Enable benchmarking mode", false, "false", "flag"));
        getCLI()->addOption(CLI::CLIOption("perf-level", '\0', "Performance monitoring level", true, "1", "int"));
        getCLI()->addOption(CLI::CLIOption("log-level", '\0', "Log level", true, "INFO", "string"));
        
        // Enable performance monitoring
        logger_.setEnabled(true);
        perfMonitor_.setEnabled(true);
    }
    
    void configureFromConfig() override {
        DemoApp::configureFromConfig();
        
        // Set demo-specific parameters
        try {
            if (config_->hasKey("num_episodes")) {
                numEpisodes_ = config_->get<size_t>("num_episodes");
            } else {
                numEpisodes_ = getCLI()->getIntArg("episodes");
            }
            
            if (config_->hasKey("steps_per_episode")) {
                stepsPerEpisode_ = config_->get<size_t>("steps_per_episode");
            } else {
                stepsPerEpisode_ = getCLI()->getIntArg("steps");
            }
            
            // Set log level from CLI/config
            std::string logLevel = getCLI()->getArg("log-level");
            if (!logLevel.empty()) {
                logger_.setLogLevel(logLevel);
            }
            
            // Log configuration
            logger_.info("Configuration loaded from: " + getCLI()->getArg("config-file"));
            logger_.info("Episodes: " + std::to_string(numEpisodes_));
            logger_.info("Steps per episode: " + std::to_string(stepsPerEpisode_));
            
        } catch (const std::exception& e) {
            logger_.error("Failed to configure demo: " + std::string(e.what()));
            numEpisodes_ = 5;
            stepsPerEpisode_ = 200;
        }
    }
    
    int execute() override {
        startTime_ = std::chrono::steady_clock::now();
        logger_.info("Enhanced Phase 3 Demo started at " + getCurrentTimestamp());
        logger_.info("Benchmark mode: " + std::string(getCLI()->getBoolArg("benchmark") ? "enabled" : "disabled"));
        
        perfMonitor_.startTask("demo_setup");
        runPhase3Demo();
        perfMonitor_.endTask();
        
        // Export results if requested
        std::string resultsFile = getCLI()->getArg("results-output");
        if (!resultsFile.empty()) {
            exportResults(resultsFile);
        }
        
        // Print performance report
        perfMonitor_.printReport();
        
        return 0;
    }
    
    void runPhase3Demo() {
        logger_.info("Starting Phase 3 demo with " + std::to_string(numEpisodes_) + " episodes, " + std::to_string(stepsPerEpisode_) + " steps each");
        
        // Create configuration
        perfMonitor_.startTask("config_setup");
        auto config = std::make_shared<Config>();
        configureEnvironment(config);
        configureBrain(config);
        perfMonitor_.endTask();
        
        // Create world
        perfMonitor_.startTask("world_setup");
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(42);
        perfMonitor_.endTask();
        
        // Create brain
        perfMonitor_.startTask("brain_setup");
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        perfMonitor_.endTask();
        
        // Create agent brain interface
        perfMonitor_.startTask("agent_setup");
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        perfMonitor_.endTask();
        
        // Configure agent brain
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        logger_.info("Initial brain state:");
        logger_.info("  Neurons: " + std::to_string(brain->getTotalNeuronCount()));
        logger_.info("  Synapses: " + std::to_string(brain->getTotalSynapseCount()));
        logger_.info("  Sensory input size: " + std::to_string(agentBrain.getSensoryInputSize()));
        logger_.info("  Motor output size: " + std::to_string(agentBrain.getMotorOutputSize()));
        logger_.info("");
        
        // Metrics tracking
        std::vector<ExperimentMetrics> allMetrics;
        
        // Start benchmark timer
        auto demoStart = std::chrono::steady_clock::now();
        
        // Run episodes
        for (size_t episode = 0; episode < numEpisodes_; ++episode) {
            logger_.info("--- Episode " + std::to_string(episode) + " started ---");
            
            // Reset world
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            ExperimentMetrics metrics;
            metrics.episode = episode;
            
            // Phase 5 navigation/interact metrics
            std::set<std::pair<int, int>> visitedCells;
            int resourcesConsumed = 0, hazardsHit = 0, emptyInteracts = 0;
            int tooSoonCount = 0;
            int oscillations = 0;
            MotorCommand prevMove = MotorCommand::Wait;
            bool hasPrevMove = false;
            
            float episodeReward = 0.0f;
            uint64_t brainStepCounter = 0;
            
            // Run steps
            for (size_t step = 0; step < stepsPerEpisode_; ++step) {
                // 1. Get sensory percept from world
                perfMonitor_.startTask("sensory_processing");
                const SensoryPercept& percept = world.getSensoryPercept();
                perfMonitor_.endTask();
                
                // 2. Inject sensory input into brain
                perfMonitor_.startTask("neural_processing");
                agentBrain.processSensoryInput(percept);
                
                // 3. Simulate brain (multiple steps per action)
                for (size_t neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    (void)neuralStep;
                    brain->step(brainStepCounter++);
                }
                perfMonitor_.endTask();
                
                // 4. Decode motor command from brain activity
                perfMonitor_.startTask("motor_decoding");
                MotorCommand cmd = agentBrain.decodeMotorCommand();
                perfMonitor_.endTask();
                
                // 5. Apply motor command to world
                perfMonitor_.startTask("action_execution");
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                perfMonitor_.endTask();
                
                // 6. Apply reward modulation
                perfMonitor_.startTask("reward_modulation");
                agentBrain.applyRewardModulation(result.reward, 0.0f);
                perfMonitor_.endTask();
                
                // 7. Update world
                perfMonitor_.startTask("world_update");
                world.update(0.1);
                perfMonitor_.endTask();
                
                // 8. Update development
                perfMonitor_.startTask("development_update");
                agentBrain.updateDevelopment(0.1);
                perfMonitor_.endTask();
                
                // Track metrics
                episodeReward += result.reward;
                metrics.steps++;
                metrics.totalReward += result.reward;
                metrics.totalSpikes += brain->getFiringNeuronCount();
                
                // Phase 5 metrics
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
                
                // Count actions
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
                    logger_.info("  Step " + std::to_string(step) +
                                 ": reward=" + std::to_string(result.reward) +
                                 ", action=" + motorCommandToString(cmd) +
                                 ", neuromod=" + std::to_string(agentBrain.getNeuromodulationLevel()) +
                                 ", novelty=" + std::to_string(agentBrain.getNoveltyLevel()) +
                                 ", energy=" + std::to_string(world.getAgentBody().energy));
                }
            }
            
            // Record final metrics
            metrics.finalNovelty = agentBrain.getNoveltyLevel();
            metrics.finalCuriosity = agentBrain.getCuriosityLevel();
            metrics.finalEnergy = world.getAgentBody().energy;
            metrics.predictionError = agentBrain.getPredictionError();
            metrics.developmentalAge = agentBrain.getBrain()->getTotalSynapseCount();
            metrics.developmentalStage = stageToString(agentBrain.getDevelopmentalStage());
            metrics.totalSynapses = brain->getTotalSynapseCount();
            metrics.averageFiringRate = brain->getAverageFiringRate();
            
            allMetrics.push_back(metrics);
            
            logger_.info("--- Episode " + std::to_string(episode) + " ended ---");
            logger_.info("  Steps: " + std::to_string(metrics.steps));
            logger_.info("  Total Reward: " + std::to_string(metrics.totalReward));
            logger_.info("  Final Energy: " + std::to_string(metrics.finalEnergy));
            logger_.info("  Synapses: " + std::to_string(metrics.totalSynapses));
            logger_.info("  Action counts: fwd=" + std::to_string(metrics.forwardCount) +
                         ", bwd=" + std::to_string(metrics.backwardCount) +
                         ", left=" + std::to_string(metrics.turnLeftCount) +
                         ", right=" + std::to_string(metrics.turnRightCount) +
                         ", interact=" + std::to_string(metrics.interactCount) +
                         ", wait=" + std::to_string(metrics.waitCount) +
                         ", lookL=" + std::to_string(metrics.lookLeftCount) +
                         ", lookR=" + std::to_string(metrics.lookRightCount) +
                         ", avgFiringRate=" + std::to_string(metrics.averageFiringRate));
            logger_.info("  Phase5 metrics: visitedCells=" + std::to_string(visitedCells.size()) +
                         ", resources=" + std::to_string(resourcesConsumed) +
                         ", hazards=" + std::to_string(hazardsHit) +
                         ", emptyInteracts=" + std::to_string(emptyInteracts) +
                         ", oscillations=" + std::to_string(oscillations) +
                         ", finalNovelty=" + std::to_string(metrics.finalNovelty) +
                         ", finalCuriosity=" + std::to_string(metrics.finalCuriosity) +
                         ", tooSoon=" + std::to_string(tooSoonCount) +
                         ", executed=" + std::to_string(stepsPerEpisode_ - tooSoonCount) +
                         ", reflexBonus=" + std::to_string(agentBrain.getReflexBonusCount()) +
                         ", reflexSuppress=" + std::to_string(agentBrain.getReflexSuppressCount()) +
                         ", wtaPulses=" + std::to_string(agentBrain.getWtaPulseCount()) +
                         ", plannerOverride=" + std::to_string(agentBrain.getPlannerOverrideCount()) +
                         ", plannerAgree=" + std::to_string(agentBrain.getPlannerAgreementCount()) +
                         "/" + std::to_string(agentBrain.getPlannerDecisionCount()) +
                         ", conceptBias=" + std::to_string(agentBrain.getConceptBiasCount()) +
                         ", selfBias=" + std::to_string(agentBrain.getSelfBiasCount()) +
                         ", socialBias=" + std::to_string(agentBrain.getSocialBiasCount()) +
                         ", concepts=" + std::to_string(agentBrain.getConceptCount()) +
                         ", selfConf=" + std::to_string(agentBrain.getSelfModelConfidence()) +
                         ", wmTraces=" + std::to_string((brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0)));
        }
        
        // End benchmark timer
        auto demoEnd = std::chrono::steady_clock::now();
        auto demoDuration = std::chrono::duration_cast<std::chrono::milliseconds>(demoEnd - demoStart).count();
        
        // Calculate performance metrics
        float totalSteps = static_cast<float>(numEpisodes_ * stepsPerEpisode_);
        float stepsPerSecond = totalSteps / (demoDuration / 1000.0f);
        
        // Summary
        logger_.info("==========================================");
        logger_.info("ENHANCED PHASE 3 SUMMARY");
        logger_.info("==========================================");
        logger_.info("");
        
        if (allMetrics.size() >= 2) {
            const auto& first = allMetrics.front();
            const auto& last = allMetrics.back();
            
            logger_.info("Early episode (0) vs Late episode (" + std::to_string(allMetrics.size() - 1) + "):");
            logger_.info("  Reward: " + std::to_string(first.totalReward) + " -> " + std::to_string(last.totalReward));
            logger_.info("  Forward actions: " + std::to_string(first.forwardCount) + " -> " + std::to_string(last.forwardCount));
            logger_.info("  Exploration (wait): " + std::to_string(first.waitCount) + " -> " + std::to_string(last.waitCount));
            
            // Calculate behavior change
            float rewardChange = last.totalReward - first.totalReward;
            float actionChange = static_cast<float>(last.forwardCount) - static_cast<float>(first.forwardCount);
            
            logger_.info("");
            logger_.info("Behavioral change:");
            if (rewardChange > 0) {
                logger_.info("  Reward INCREASED by " + std::to_string(rewardChange) + " (positive learning)");
            } else if (rewardChange < -5) {
                logger_.info("  Reward DECREASED by " + std::to_string(-rewardChange) + " (possible exploration or extinction)");
            }
            
            if (std::abs(actionChange) > 10) {
                logger_.info("  Forward actions changed by " + std::to_string(actionChange));
            }
            
            // Synapse change
            if (last.totalSynapses != first.totalSynapses) {
                logger_.info("  Synapses: " + std::to_string(first.totalSynapses) + " -> " + std::to_string(last.totalSynapses));
            }
        }
        
        logger_.info("");
        logger_.info("Enhanced Phase 3 demonstration complete.");
        logger_.info("The brain received sensory input, produced actions,");
        logger_.info("experienced consequences, and modified its synapses.");
        logger_.info("");
        logger_.info("Performance metrics:");
        logger_.info("  Total episodes: " + std::to_string(numEpisodes_));
        logger_.info("  Steps per episode: " + std::to_string(stepsPerEpisode_));
        logger_.info("  Total steps: " + std::to_string(numEpisodes_ * stepsPerEpisode_));
        logger_.info("  Demo duration: " + std::to_string(demoDuration) + " ms");
        logger_.info("  Steps per second: " + std::to_string(stepsPerSecond));
        logger_.info("  Average spikes per step: " + std::to_string(getAverageSpikesPerStep(allMetrics)));
        
        // Store metrics for export
        episodeMetrics_ = allMetrics;
    }
    
    void exportResults(const std::string& filename) {
        logger_.info("Exporting results to: " + filename);
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            logger_.error("Failed to open output file: " + filename);
            return;
        }
        
        file << "NLM Phase 3 Demo Results" << std::endl;
        file << "Generated at: " << getCurrentTimestamp() << std::endl;
        file << "" << std::endl;
        file << "Configuration:" << std::endl;
        file << "  Episodes: " << numEpisodes_ << std::endl;
        file << "  Steps per episode: " << stepsPerEpisode_ << std::endl;
        file << "  Total steps: " << numEpisodes_ * stepsPerEpisode_ << std::endl;
        file << "" << std::endl;
        
        // Export episode metrics
        for (size_t i = 0; i < episodeMetrics_.size(); ++i) {
            const auto& metrics = episodeMetrics_[i];
            file << "Episode " << i << ":" << std::endl;
            file << "  Steps: " << metrics.steps << std::endl;
            file << "  Total Reward: " << metrics.totalReward << std::endl;
            file << "  Total Spikes: " << metrics.totalSpikes << std::endl;
            file << "  Synapses: " << metrics.totalSynapses << std::endl;
            file << "  Average Firing Rate: " << metrics.averageFiringRate << std::endl;
            file << "" << std::endl;
        }
        
        file.close();
        logger_.info("Results exported successfully to: " + filename);
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    float getAverageSpikesPerStep(const std::vector<ExperimentMetrics>& metrics) {
        if (metrics.empty()) return 0.0f;
        
        float totalSpikes = 0.0f;
        float totalSteps = 0.0f;
        
        for (const auto& m : metrics) {
            totalSpikes += m.totalSpikes;
            totalSteps += m.steps;
        }
        
        return totalSteps > 0.0f ? totalSpikes / totalSteps : 0.0f;
    }
    
private:
    void configureEnvironment(std::shared_ptr<Config> config) {
        config->set("environment_width", 20);
        config->set("environment_height", 20);
        config->set("vision_width", 16);
        config->set("vision_height", 16);
    }
    
    void configureBrain(std::shared_ptr<Config> config) {
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(2));
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
    
    // Store episode metrics for export
    std::vector<ExperimentMetrics> episodeMetrics_;
};

int main(int argc, char* argv[]) {
    EnhancedPhase3Demo demo;
    return demo.run(argc, argv);
}

} // namespace nlm