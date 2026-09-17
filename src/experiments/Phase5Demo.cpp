#include "Phase5Experiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

namespace nlm {

Phase5Demo::Phase5Demo() {
    NLM_LOG_INFO("Initializing Phase 5 Demo");
}

Phase5Demo::~Phase5Demo() = default;

void Phase5Demo::runDemo() {
    NLM_LOG_INFO("=== Phase 5 Demo - Lifetime Integration ===");
    NLM_LOG_INFO("This demo demonstrates the integration of all previous phases:");
    NLM_LOG_INFO("  - Neural plasticity (Phase 1)");
    NLM_LOG_INFO("  - Memory systems (Phase 3)");
    NLM_LOG_INFO("  - Prediction and cognition (Phase 4)");
    NLM_LOG_INFO("  - Neuromodulation and development (Phase 2)");
    NLM_LOG_INFO("  - Integration and lifetime learning (Phase 5)");
    
    // Create experiment
    auto experiment = std::make_shared<Phase5IntegratedExperiment>();
    
    // Run demonstration
    auto result = experiment->run(createDefaultConfig());
    
    NLM_LOG_INFO("Demo results:");
    NLM_LOG_INFO("  Total Reward: " + std::to_string(result.totalReward));
    NLM_LOG_INFO("  Average Reward per Step: " + std::to_string(result.avgRewardPerStep));
    NLM_LOG_INFO("  Learning Efficiency: " + std::to_string(result.overallLearningEfficiency));
    NLM_LOG_INFO("  Final Performance: " + std::to_string(result.finalPerformance));
    
    // Show phase breakdown
    NLM_LOG_INFO("Phase breakdown:");
    for (const auto& phase : result.phaseResults) {
        NLM_LOG_INFO("  " + phase.phaseName + ": steps " + std::to_string(phase.startStep) 
                   + "-" + std::to_string(phase.endStep) + ", reward=" + std::to_string(phase.totalReward));
    }
}

LifetimeExperimentConfig Phase5Demo::createDefaultConfig() {
    LifetimeExperimentConfig config;
    config.initialNeurons = 500;
    config.maxNeurons = 2000;
    config.synapseFormationSteps = 1000;
    config.initialConnections = 200;
    config.connectionGrowthRate = 0.01f;
    config.learningRate = 0.001f;
    config.rewardDecayRate = 0.9f;
    config.memoryRetentionRate = 0.95f;
    config.predictionAccuracyTarget = 0.8f;
    config.generalizationTarget = 0.7f;
    config.maxPhases = 5;
    
    return config;
}

} // namespace nlm
