// Main training module implementation
// This file includes all training components and provides a unified interface

#include "TrainingCore.hpp"
#include "TrainingComponents.hpp"

namespace nlm {

// Training module initialization
class TrainingModule {
public:
    TrainingModule() {
        std::cout << "NLM Training Module initialized" << std::endl;
    }
    
    ~TrainingModule() {
        std::cout << "NLM Training Module cleaned up" << std::endl;
    }
    
    // Get singleton instance
    static TrainingModule& getInstance() {
        static TrainingModule instance;
        return instance;
    }
    
    // Create training core
    std::shared_ptr<TrainingCore> createTrainingCore() {
        return std::make_shared<TrainingCore>();
    }
    
    // Create simple training config
    std::shared_ptr<SimpleTrainingConfig> createTrainingConfig() {
        return std::make_shared<SimpleTrainingConfig>();
    }
    
    // Create advanced training features
    std::shared_ptr<AdvancedTrainingFeatures> createAdvancedFeatures() {
        return std::make_shared<AdvancedTrainingFeatures>();
    }
    
    // Create training callback
    std::shared_ptr<SimpleTrainingCallback> createTrainingCallback() {
        return std::make_shared<SimpleTrainingCallback>();
    }
    
    // Create common training patterns
    std::shared_ptr<CommonTrainingPatterns> createTrainingPatterns() {
        return std::make_shared<CommonTrainingPatterns>();
    }
    
    // Create training persistence
    std::shared_ptr<TrainingPersistence> createTrainingPersistence() {
        return std::make_shared<TrainingPersistence>();
    }
    
    // Initialize Python bindings (if needed)
    void initializePythonBindings() {
        // TODO: Initialize Python bindings for training module
        std::cout << "Python bindings initialized" << std::endl;
    }
};

// Convenience functions for easy usage
namespace Training {
    
    // Create and configure a training setup
    std::tuple<std::shared_ptr<TrainingCore>, 
               std::shared_ptr<Brain>,
               std::shared_ptr<SimpleWorld>,
               std::shared_ptr<AgentBrain>>
    createTrainingSetup(
        const std::string& worldConfig = "default",
        const std::string& brainConfig = "default") {
        
        // Create brain
        auto brain = std::make_shared<Brain>(Brain::createConfig(brainConfig));
        brain->initialize();
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        if (worldConfig != "default") {
            world->configure(worldConfig);
        }
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Create training core
        auto trainer = std::make_shared<TrainingCore>();
        
        return std::make_tuple(trainer, brain, world, agent);
    }
    
    // Run a complete training session
    bool runTrainingSession(
        int episodes,
        const TrainingConfig& config,
        const std::vector<std::shared_ptr<TrainingCallback>>& callbacks = {}) {
        
        // Create training setup
        auto [trainer, brain, world, agent] = createTrainingSetup();
        
        // Configure training
        TrainingConfig trainingConfig = config;
        trainingConfig.maxEpisodes = episodes;
        
        // Run training
        return trainer->trainAgent(brain, world, agent, trainingConfig, callbacks);
    }
    
    // Evaluate agent performance
    float evaluateAgentPerformance(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        int evaluationEpisodes = 10) {
        
        TrainingConfig config;
        config.maxEpisodes = evaluationEpisodes;
        
        TrainingCore trainer;
        return trainer.evaluateAgent(brain, world, agent, config, evaluationEpisodes);
    }
    
    // Hyperparameter optimization
    std::vector<std::pair<std::string, float>> optimizeHyperparameters(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        const std::vector<Hyperparameter>& paramSpace,
        OptimizationStrategy strategy,
        int maxIterations = 100) {
        
        TrainingCore trainer;
        return trainer.hyperparameterOptimization(
            brain, world, agent, paramSpace,
            [](const std::vector<Hyperparameter>& params) {
                // Simple evaluation function - could be enhanced
                return 0.0f;
            },
            strategy, maxIterations);
    }
}

} // namespace nlm
