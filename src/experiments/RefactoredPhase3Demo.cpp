#include "Phase3Experiment.hpp"
#include "../experiments/experiment_base.hpp"
#include "../experiments/experiment_factory.hpp"
#include "../experiments/experiment_utils.hpp"
#include <iostream>
#include <iomanip>

namespace nlm {

// Refactored Phase 3 demo using common experiment infrastructure
class RefactoredPhase3Demo : public ExperimentBase {
public:
    RefactoredPhase3Demo(const std::string& name = "Phase3RefactoredDemo") 
        : ExperimentBase(name) {}
    
    bool setup() override {
        std::cout << "Setting up refactored Phase 3 demo...\n";
        
        // Use experiment factory to create all components
        brain_ = ExperimentFactory::createBrain(getConfig());
        world_ = ExperimentFactory::createSimpleWorld(getConfig());
        agentBrain_ = ExperimentFactory::createAgentBrain(brain_, world_, getConfig());
        
        // Configure with common settings
        agentBrain_->enableRewardModulation(true);
        agentBrain_->enableStructuralPlasticity(true);
        agentBrain_->enableDevelopment(true);
        agentBrain_->enableCuriosity(true);
        
        std::cout << "Setup complete.\n";
        return true;
    }
    
    bool execute() override {
        std::cout << "Running refactored Phase 3 demo...\n";
        
        // Use experiment utilities for the simulation loop
        auto results = ExperimentUtils::runSimulation(
            brain_, world_, agentBrain_,
            getConfig().getOr<int>("numEpisodes", 5),
            getConfig().getOr<int>("stepsPerEpisode", 200)
        );
        
        // Record results using common utilities
        for (const auto& episode : results.episodes) {
            recordEpisodeMetrics(episode);
        }
        
        printResults();
        return true;
    }
    
    bool teardown() override {
        std::cout << "Tearing down Phase 3 demo...\n";
        brain_.reset();
        world_.reset();
        agentBrain_.reset();
        std::cout << "Teardown complete.\n";
        return true;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agentBrain_;
    
    void recordEpisodeMetrics(const ExperimentUtils::EpisodeResult& episode) {
        ExperimentMetrics metrics;
        metrics.episode = episode.episodeNumber;
        metrics.steps = episode.steps;
        metrics.totalReward = episode.totalReward;
        
        // Record additional metrics using common utilities
        metrics.totalSpikes = episode.totalSpikes;
        metrics.finalEnergy = world_->getAgentBody().energy;
        
        // Add to experiment results
        addEpisodeResult(metrics);
    }
    
    void printResults() {
        std::cout << "\n=== Phase 3 Demo Results ===\n";
        for (const auto& metrics : getEpisodeResults()) {
            std::cout << "Episode " << metrics.episode 
                     << ": Reward = " << std::fixed << std::setprecision(2) 
                     << metrics.totalReward 
                     << ", Steps = " << metrics.steps 
                     << ", Spikes = " << metrics.totalSpikes << "\n";
        }
    }
};

// Integration function - create and run the refactored demo
void runRefactoredPhase3Demo(int numEpisodes = 5, int stepsPerEpisode = 200) {
    // Create configuration using experiment utils
    auto config = std::make_shared<Config>();
    
    // Use experiment utils for common configuration setup
    ExperimentUtils::configureEnvironment(config);
    ExperimentUtils::configureBrain(config);
    
    // Set experiment-specific parameters
    config->set("numEpisodes", numEpisodes, ConfigSource::Runtime);
    config->set("stepsPerEpisode", stepsPerEpisode, ConfigSource::Runtime);
    
    std::cout << "===========================================\n";
    std::cout << "NLM PHASE 3 - REFACTORED WORLD INTERACTION DEMO\n";
    std::cout << "===========================================\n\n";
    
    std::cout << "Initial configuration:\n";
    std::cout << config->summary();
    
    // Create and run the refactored demo
    RefactoredPhase3Demo demo;
    if (demo.initialize(config)) {
        demo.execute();
        demo.finalize();
    }
}

// Legacy wrapper for backward compatibility
void runOriginalPhase3Demo(int numEpisodes = 5, int stepsPerEpisode = 200) {
    std::cout << "Running original Phase 3 demo for backward compatibility...\n";
    // This would run the original implementation
    // For now, we'll delegate to the refactored version
    runRefactoredPhase3Demo(numEpisodes, stepsPerEpisode);
}

} // namespace nlm
