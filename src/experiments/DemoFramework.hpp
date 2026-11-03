#ifndef NLM_DEMO_FRAMEWORK_HPP
#define NLM_DEMO_FRAMEWORK_HPP

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include <map>
#include <optional>

#include "core/Config/Config.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

/**
 * @brief Exception class for demo errors
 */
class DemoException : public std::runtime_error {
public:
    explicit DemoException(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @brief Configuration factory for common demo configurations
 */
class ConfigFactory {
public:
    /**
     * @brief Create a standard configuration for phase 3 world interaction
     * 
     * @param neuronCount Number of neurons in the brain
     * @param regionCount Number of brain regions
     * @param randomSeed Random seed for reproducibility
     * @return Shared pointer to configuration
     */
    static std::shared_ptr<Config> createPhase3Config(
        int64_t neuronCount = 500,
        int64_t regionCount = 2,
        int64_t randomSeed = 42) {
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", regionCount);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", randomSeed);
        config->set("simulation_timestep", 0.001f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        config->set("environment_width", 20);
        config->set("environment_height", 20);
        config->set("vision_width", 16);
        config->set("vision_height", 16);
        
        return config;
    }
    
    /**
     * @brief Create a standard configuration for phase 4 cognitive demo
     * 
     * @param neuronCount Number of neurons in the brain
     * @return Shared pointer to configuration
     */
    static std::shared_ptr<Config> createPhase4Config(
        int64_t neuronCount = 1000) {
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        
        return config;
    }
    
    /**
     * @brief Create a standard configuration for phase 5 personality demo
     * 
     * @return Shared pointer to configuration
     */
    static std::shared_ptr<Config> createPhase5Config() {
        return createPhase3Config();
    }
    
    /**
     * @brief Create a standard configuration for phase 6 integration demo
     * 
     * @return Shared pointer to configuration
     */
    static std::shared_ptr<Config> createPhase6Config() {
        return createPhase4Config();
    }
    
    /**
     * @brief Apply environment configuration to a config object
     * 
     * @param config Configuration object to modify
     * @param width Environment width
     * @param height Environment height
     * @param visionWidth Vision width
     * @param visionHeight Vision height
     */
    static void configureEnvironment(
        std::shared_ptr<Config> config,
        int width = 20,
        int height = 20,
        int visionWidth = 16,
        int visionHeight = 16) {
        
        config->set("environment_width", width);
        config->set("environment_height", height);
        config->set("vision_width", visionWidth);
        config->set("vision_height", visionHeight);
    }
    
    /**
     * @brief Apply brain configuration to a config object
     * 
     * @param config Configuration object to modify
     * @param neuronCount Number of neurons
     * @param regionCount Number of regions
     * @param connectionProbability Connection probability
     * @param stdpLtpWeight STDP LTP weight
     * @param stdpLtdWeight STDP LTD weight
     */
    static void configureBrain(
        std::shared_ptr<Config> config,
        int64_t neuronCount = 500,
        int64_t regionCount = 2,
        float connectionProbability = 0.1f,
        float stdpLtpWeight = 0.01f,
        float stdpLtdWeight = 0.012f) {
        
        config->set("neuron_count", neuronCount);
        config->set("region_count", regionCount);
        config->set("connection_probability", connectionProbability);
        config->set("stdp_ltp_weight", stdpLtpWeight);
        config->set("stdp_ltd_weight", stdpLtdWeight);
    }
};

/**
 * @brief Common simulation engine for running brain-world interaction
 */
class SimulationEngine {
public:
    /**
     * @brief Run a single simulation step
     * 
     * @param world World simulation object
     * @param agentBrain Agent brain interface
     * @param brain Brain object
     * @param deltaTime Time step duration
     * @return bool true if simulation completed successfully
     */
    static bool step(
        SimpleWorld& world,
        AgentBrain& agentBrain,
        std::shared_ptr<Brain> brain,
        float deltaTime = 0.1f) {
        
        // Get sensory percept from world
        const SensoryPercept& percept = world.getSensoryPercept();
        
        // Inject sensory input into brain
        agentBrain.processSensoryInput(percept);
        
        // Simulate brain (multiple steps per action)
        static uint64_t globalBrainStepCounter = 0;
        for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
            brain->step(globalBrainStepCounter++);
        }
        
        // Decode motor command from brain activity
        MotorCommand cmd = agentBrain.decodeMotorCommand();
        
        // Apply motor command to world
        ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
        
        // Apply reward modulation
        agentBrain.applyRewardModulation(result.reward, 0.0f);
        
        // Update world
        world.update(deltaTime);
        
        // Update development
        agentBrain.updateDevelopment(deltaTime);
        
        return true;
    }
    
    /**
     * @brief Run a complete episode with metrics tracking
     * 
     * @tparam TWorld World type (must have configure, reset, etc.)
     * @tparam TAgent Agent brain type
     * @tparam TBrain Brain type
     * @param world World simulation object
     * @param agentBrain Agent brain interface
     * @param brain Brain object
     * @param numSteps Number of simulation steps
     * @param episodeNumber Episode number for logging
     * @param metricsCallback Callback function to record metrics
     * @return std::vector<float> Collected metrics
     */
    template<typename TWorld, typename TAgent, typename TBrain>
    static std::vector<float> runEpisode(
        TWorld& world,
        TAgent& agentBrain,
        std::shared_ptr<TBrain> brain,
        int numSteps,
        int episodeNumber,
        std::function<void(const std::string&, float)> metricsCallback = nullptr) {
        
        std::vector<float> metrics;
        
        // Reset state
        world.reset();
        agentBrain.reset();
        brain->reset();
        
        // Run steps
        for (int step = 0; step < numSteps; ++step) {
            bool success = step(world, agentBrain, brain);
            if (!success) {
                throw DemoException("Simulation step failed");
            }
            
            if (metricsCallback) {
                metricsCallback("step", step);
            }
        }
        
        return metrics;
    }
    
    /**
     * @brief Run multiple episodes
     * 
     * @tparam TWorld World type
     * @tparam TAgent Agent brain type
     * @tparam TBrain Brain type
     * @param world World simulation object
     * @param agentBrain Agent brain interface
     * @param brain Brain object
     * @param numEpisodes Number of episodes to run
     * @param stepsPerEpisode Steps per episode
     * @param episodeCallback Callback function called for each episode
     * @return std::vector<std::vector<float>> Metrics for each episode
     */
    template<typename TWorld, typename TAgent, typename TBrain>
    static std::vector<std::vector<float>> runEpisodes(
        TWorld& world,
        TAgent& agentBrain,
        std::shared_ptr<TBrain> brain,
        int numEpisodes,
        int stepsPerEpisode,
        std::function<void(int, const std::vector<float>&)> episodeCallback = nullptr) {
        
        std::vector<std::vector<float>> allMetrics;
        
        for (int episode = 0; episode < numEpisodes; ++episode) {
            auto metrics = runEpisode(world, agentBrain, brain, stepsPerEpisode, episode, episodeCallback);
            allMetrics.push_back(metrics);
            
            if (episodeCallback) {
                episodeCallback(episode, metrics);
            }
        }
        
        return allMetrics;
    }
};

/**
 * @brief Base class for all NLM demos
 */
class DemoBase {
public:
    DemoBase() : brainStepCounter_(0) {}
    virtual ~DemoBase() {}
    
    /**
     * @brief Run the demo with specified parameters
     * 
     * @param numEpisodes Number of episodes to run
     * @param stepsPerEpisode Steps per episode
     */
    void run(int numEpisodes = 5, int stepsPerEpisode = 200) {
        std::cout << "===========================================\n";
        std::cout << "NLM DEMO - " << getDemoName() << "\n";
        std::cout << "===========================================\n\n";
        
        try {
            onSetup();
            onRun(numEpisodes, stepsPerEpisode);
            onComplete();
        } catch (const DemoException& e) {
            std::cerr << "Demo error: " << e.what() << std::endl;
            throw;
        } catch (const std::exception& e) {
            std::cerr << "Unexpected error: " << e.what() << std::endl;
            throw DemoException(std::string("Demo failed: ") + e.what());
        }
    }
    
    /**
     * @brief Get the demo name for display purposes
     * 
     * @return std::string Demo name
     */
    virtual std::string getDemoName() const = 0;
    
    /**
     * @brief Called during setup phase
     */
    virtual void onSetup() {}
    
    /**
     * @brief Called to run the main demo logic
     * 
     * @param numEpisodes Number of episodes
     * @param stepsPerEpisode Steps per episode
     */
    virtual void onRun(int numEpisodes, int stepsPerEpisode) = 0;
    
    /**
     * @brief Called after demo completion
     */
    virtual void onComplete() {}
    
    /**
     * @brief Configure environment for the demo
     * 
     * @param config Configuration object to modify
     */
    virtual void configureEnvironment(std::shared_ptr<Config> config) {
        ConfigFactory::configureEnvironment(config);
    }
    
    /**
     * @brief Configure brain for the demo
     * 
     * @param config Configuration object to modify
     * @param neuronCount Number of neurons
     * @param regionCount Number of regions
     */
    virtual void configureBrain(std::shared_ptr<Config> config, 
                               int64_t neuronCount = 500,
                               int64_t regionCount = 2) {
        ConfigFactory::configureBrain(config, neuronCount, regionCount);
    }
    
protected:
    uint64_t brainStepCounter_;
};

/**
 * @brief Common helper functions for all demos
 */
namespace DemoHelpers {
    /**
     * @brief Convert motor command to string for display
     * 
     * @param cmd Motor command
     * @return const char* String representation
     */
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
    
    /**
     * @brief Convert developmental stage to string
     * 
     * @param stage Developmental stage
     * @return const char* String representation
     */
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
    
    /**
     * @brief Convert motor command to string (alternative version)
     * 
     * @param cmd Motor command
     * @return const char* String representation
     */
    const char* cmdName(MotorCommand cmd) {
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
}

} // namespace nlm

#endif // NLM_DEMO_FRAMEWORK_HPP
