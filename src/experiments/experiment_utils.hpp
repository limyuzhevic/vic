#pragma once

#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Config/Config.hpp"
#include "../experiments/Experiment.hpp"
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>

namespace nlm {

/**
 * Common utility functions for experiment setup and execution
 */
namespace ExperimentUtils {
    /**
     * Create standard configuration for experiments
     */
    inline std::shared_ptr<Config> createStandardConfig() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        return config;
    }
    
    /**
     * Create standard world configuration
     */
    inline void configureStandardWorld(SimpleWorld& world) {
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(42);
        world.reset();
    }
    
    /**
     * Create brain with standard configuration
     */
    inline std::shared_ptr<Brain> createStandardBrain() {
        auto config = createStandardConfig();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }
    
    /**
     * Create agent brain with standard configuration
     */
    inline std::unique_ptr<AgentBrain> createStandardAgentBrain(std::shared_ptr<Brain> brain, SimpleWorld& world) {
        auto agentBrain = std::make_unique<AgentBrain>(brain);
        agentBrain->initialize(world);
        agentBrain->enableRewardModulation(true);
        agentBrain->enableCuriosity(true);
        agentBrain->enableStructuralPlasticity(true);
        agentBrain->enableDevelopment(true);
        return agentBrain;
    }
    
    /**
     * Parse command line arguments
     */
    struct CommandLineArgs {
        int numEpisodes{5};
        int stepsPerEpisode{200};
        std::string configFile;
        bool verbose{false};
        
        static CommandLineArgs parse(int argc, char* argv[]) {
            CommandLineArgs args;
            
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                
                if (arg == "--episodes" && i + 1 < argc) {
                    args.numEpisodes = std::atoi(argv[++i]);
                } else if (arg == "--steps" && i + 1 < argc) {
                    args.stepsPerEpisode = std::atoi(argv[++i]);
                } else if (arg == "--config" && i + 1 < argc) {
                    args.configFile = argv[++i];
                } else if (arg == "--verbose") {
                    args.verbose = true;
                }
            }
            
            return args;
        }
    };
    
    /**
     * Format and log experiment results
     */
    class OutputFormatter {
    public:
        static void logHeader(const std::string& title) {
            std::cout << "\n" << std::string(60, '=') << "\n";
            std::cout << "  " << title << "\n";
            std::cout << std::string(60, '=') << "\n";
        }
        
        static void logSection(const std::string& title) {
            std::cout << "\n--- " << title << " ---\n";
        }
        
        static void logResult(const std::string& name, float value, float max = 1.0f) {
            std::cout << "  " << std::left << std::setw(30) << name << ": ";
            std::cout << std::fixed << std::setprecision(3) << value;
            if (max > 0) {
                std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
            }
            std::cout << "\n";
        }
        
        static void logSimulationStep(int step, float reward, const std::string& action,
                                     float neuromod, float novelty, float curiosity,
                                     float energy) {
            std::cout << "  Step " << step << ": reward=" << reward
                      << ", action=" << action
                      << ", neuromod=" << neuromod
                      << ", novelty=" << novelty
                      << ", curiosity=" << curiosity
                      << ", energy=" << energy << "\n";
        }
    };
    
    /**
     * Calculate metric improvements between episodes
     */
    inline void calculateImprovement(const std::vector<ExperimentMetrics>& metrics,
                                   const std::string& metricName,
                                   float& improvement, float& variance) {
        if (metrics.size() >= 2) {
            float first = metrics.front().getMetric(metricName);
            float last = metrics.back().getMetric(metricName);
            improvement = last - first;
            variance = std::abs(improvement);
        }
    }
    
    /**
     * Run a single step of the simulation loop
     */
    inline void runSimulationStep(AgentBrain& agentBrain, SimpleWorld& world, 
                                 uint64_t& brainStepCounter) {
        // 1. Get sensory percept from world
        const SensoryPercept& percept = world.getSensoryPercept();
        
        // 2. Inject sensory input into brain
        agentBrain.processSensoryInput(percept);
        
        // 3. Simulate brain (multiple steps per action)
        for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
            (void)neuralStep;
            agentBrain.getBrain()->step(brainStepCounter++);
        }
        
        // 4. Decode motor command from brain activity
        MotorCommand cmd = agentBrain.decodeMotorCommand();
        
        // 5. Apply motor command to world
        ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
        
        // 6. Apply reward modulation
        agentBrain.applyRewardModulation(result.reward, 0.0f);
        
        // 7. Update world
        world.update(0.1);
        
        // 8. Update development
        agentBrain.updateDevelopment(0.1);
        
        return result;
    }
    
    /**
     * Convert motor command to string
     */
    inline const char* motorCommandToString(MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            default: return "Wait";
        }
    }
    
    /**
     * Convert developmental stage to string
     */
    inline const char* stageToString(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
}

} // namespace nlm
