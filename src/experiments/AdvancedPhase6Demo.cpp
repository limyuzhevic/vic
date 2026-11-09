// NLM Advanced Demo - Phase 6 Integration Demo
// This demo demonstrates the unified CLI framework and advanced features

#include "../core/CLI/CLIFramework.hpp"
#include "../core/Demos/AdvancedDemoBase.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <chrono>

using namespace nlm;

// Advanced Phase 6 Demo Implementation
class AdvancedPhase6Demo : public AdvancedDemoBase {
public:
    AdvancedPhase6Demo() : 
        episodeCount_(5),
        stepsPerEpisode_(200),
        enableDebugMode_(false),
        enableSaveReplay_(false),
        replaySteps_(100),
        neuralPruning_(0.001f),
        synapticRate_(0.0001f) {}
    
    std::string getDemoName() const override {
        return "advanced_phase6_demo";
    }
    
    std::string getDemoDescription() const override {
        return "Advanced Phase 6 integration demo with performance monitoring and checkpointing";
    }
    
    void registerArguments(CLIFramework& framework) override {
        framework.registerArgument("episodes", "Number of episodes to run", true, "5");
        framework.registerArgument("steps", "Number of steps per episode", true, "200");
        framework.registerArgument("neuron-count", "Number of neurons in the brain", false, "1000");
        framework.registerArgument("region-count", "Number of neural regions", false, "1");
        framework.registerArgument("connection-prob", "Connection probability between neurons", false, "0.1");
    }
    
    void registerOptions(CLIFramework& framework) override {
        framework.registerOption("debug", "Enable debug mode", &enableDebugMode_);
        framework.registerOption("save-replay", "Save replay data", &enableSaveReplay_);
        framework.registerOption("neural-prune", "Neural pruning rate", &neuralPruning_);
        framework.registerOption("synaptic-rate", "Synaptic rate for plasticity", &synapticRate_);
    }
    
    bool validateConfig(const CLIConfig& config) const override {
        if (episodeCount_ <= 0) {
            std::cerr << "Error: Episode count must be positive" << std::endl;
            return false;
        }
        if (stepsPerEpisode_ <= 0) {
            std::cerr << "Error: Steps per episode must be positive" << std::endl;
            return false;
        }
        if (config.maxMemoryUsageMB <= 0) {
            std::cerr << "Error: Max memory usage must be positive" << std::endl;
            return false;
        }
        return true;
    }
    
    bool initialize(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig) override {
        NLM_LOG_INFO("Initializing Advanced Phase 6 Demo");
        NLM_LOG_INFO("Demo name: " + getDemoName());
        
        if (config.enablePerformanceMonitoring) {
            NLM_LOG_INFO("Performance monitoring enabled");
        }
        
        // Configure simulation parameters
        if (nlmConfig) {
            nlmConfig->set("neuron_count", static_cast<int64_t>(neuronCount_));
            nlmConfig->set("region_count", static_cast<int64_t>(regionCount_));
            nlmConfig->set("connection_probability", connectionProbability_);
            nlmConfig->set("stdp_ltp_weight", 0.01f);
            nlmConfig->set("stdp_ltd_weight", 0.012f);
            nlmConfig->set("synaptogenesis_rate", synapticRate_);
            nlmConfig->set("pruning_rate", neuralPruning_);
        }
        
        NLM_LOG_INFO("Configuration applied successfully");
        return true;
    }
    
    int run(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig) override {
        NLM_LOG_INFO("Starting Advanced Phase 6 Demo");
        
        // Create configuration
        auto cfg = std::make_shared<Config>();
        if (nlmConfig) {
            // Copy settings from nlmConfig if provided
            for (const auto& key : nlmConfig->getKeys()) {
                cfg->set(key, nlmConfig->get(key).value_or(ConfigValue{}));
            }
        }
        
        // Override with our parameters
        cfg->set("neuron_count", static_cast<int64_t>(neuronCount_));
        cfg->set("region_count", static_cast<int64_t>(regionCount_));
        cfg->set("connection_probability", connectionProbability_);
        
        // Create brain
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return 1;
        }
        
        setBrain(brain);
        
        // Create world
        SimpleWorld world;
        world.configure(16, 16, 16, 16);
        world.reset();
        
        // Create agent
        AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(true);
        agent.enableStructuralPlasticity(config.enableDevelopment);
        agent.enableDevelopment(config.enableDevelopment);
        agent.enableCuriosity(true);
        
        NLM_LOG_INFO("Starting simulation with " + std::to_string(episodeCount_) + " episodes");
        NLM_LOG_INFO("Each episode has " + std::to_string(stepsPerEpisode_) + " steps");
        
        // Performance metrics tracking
        float totalReward = 0.0f;
        float totalFiringRate = 0.0f;
        size_t firingCount = 0;
        uint64_t totalSteps = 0;
        
        // Run episodes
        for (int episode = 0; episode < episodeCount_; ++episode) {
            NLM_LOG_INFO("--- Episode " << episode << " ---");
            
            // Reset world and brain for each episode
            world.reset();
            agent.reset();
            brain->reset();
            brain->initialize();
            
            float episodeReward = 0.0f;
            
            // Run steps
            for (int step = 0; step < stepsPerEpisode_; ++step) {
                // Get observation
                SensoryPercept percept = world.getSensoryPercept();
                
                // Process sensory input
                agent.processSensoryInput(percept);
                
                // Simulate brain (10 neural steps per action)
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    brain->step(totalSteps++);
                }
                
                // Get motor command
                MotorCommand cmd = agent.decodeMotorCommand();
                
                // Apply action to world
                ActionResult actionResult = world.applyMotorCommand(cmd, totalSteps * 0.001f);
                
                // Apply reward modulation
                agent.applyRewardModulation(actionResult.reward, 0.0f);
                
                // Update development
                if (config.enableDevelopment) {
                    agent.updateDevelopment(0.001f);
                }
                
                // Collect metrics
                episodeReward += actionResult.reward;
                totalReward += actionResult.reward;
                totalFiringRate += brain->getAverageFiringRate();
                if (brain->getFiringNeuronCount() > 0) firingCount++;
                
                // Periodic status updates
                if (step % 50 == 0) {
                    NLM_LOG_INFO("  Step " << step << ": Reward=" << actionResult.reward 
                                      << ", Action=" << motorCommandToString(cmd)
                                      << ", Firing=" << brain->getFiringNeuronCount()
                                      << ", FiringRate=" << brain->getAverageFiringRate());
                }
            }
            
            // Episode summary
            NLM_LOG_INFO("Episode " << episode << " complete:");
            NLM_LOG_INFO("  Total Reward: " << episodeReward);
            NLM_LOG_INFO("  Avg Firing Rate: " << totalFiringRate / episodeCount_);
            NLM_LOG_INFO("  Active Time: " << firingCount << " steps");
        }
        
        // Final summary
        NLM_LOG_INFO("\n=== Advanced Phase 6 Demo Complete ===");
        NLM_LOG_INFO("Total Episodes: " << episodeCount_);
        NLM_LOG_INFO("Total Steps: " << stepsPerEpisode_ * episodeCount_);
        NLM_LOG_INFO("Total Reward: " << totalReward);
        NLM_LOG_INFO("Average Firing Rate: " << (totalFiringRate / (stepsPerEpisode_ * episodeCount_)));
        NLM_LOG_INFO("Average Reward per Step: " << (totalReward / (stepsPerEpisode_ * episodeCount_)));
        
        // Save replay if enabled
        if (enableSaveReplay_ && brain) {
            NLM_LOG_INFO("Saving replay data...");
            std::string replayPath = "/tmp/advanced_phase6_replay.bin";
            if (brain->save(replayPath)) {
                NLM_LOG_INFO("Replay saved successfully to: " << replayPath);
            } else {
                NLM_LOG_WARNING("Failed to save replay data");
            }
        }
        
        // Performance metrics for CLI framework
        if (config.enablePerformanceMonitoring && brain) {
            framework->updatePerformanceMetrics(brain);
        }
        
        return 0;
    }
    
    void setBrain(const std::shared_ptr<Brain>& brain) override {
        this->brain_ = brain;
    }
    
    std::shared_ptr<Brain> getBrain() const override {
        return brain_;
    }
    
private:
    int episodeCount_;
    int stepsPerEpisode_;
    bool enableDebugMode_;
    bool enableSaveReplay_;
    int replaySteps_;
    float neuralPruning_;
    float synapticRate_;
    size_t neuronCount_ = 1000;
    size_t regionCount_ = 1;
    float connectionProbability_ = 0.1f;
    
    std::shared_ptr<Brain> brain_;
    
    const char* motorCommandToString(MotorCommand cmd) {
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
};

// Advanced demo main function
int main(int argc, char** argv) {
    // Create CLI framework
    CLIFramework framework;
    
    // Create advanced demo
    auto demo = std::make_shared<AdvancedPhase6Demo>();
    
    // Initialize framework
    if (!framework.initialize(demo->getDemoName(), argc, argv)) {
        std::cerr << "Failed to initialize CLI framework" << std::endl;
        return 1;
    }
    
    // Show configuration if verbose
    if (framework.getConfig().enableVerbose) {
        framework.showConfiguration();
    }
    
    // Execute demo
    return framework.executeDemo([demo](const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig) {
        return demo->executeWithErrorHandling(config, nlmConfig);
    });
}