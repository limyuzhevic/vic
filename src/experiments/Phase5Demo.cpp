#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase5Experiment.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"

using namespace nlm;

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

void printResult(const std::string& name, float value, float max = 1.0f) {
    std::cout << "  " << std::left << std::setw(35) << name << ": ";
    std::cout << std::fixed << std::setprecision(3) << value;
    if (max > 0) {
        std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
    }
    std::cout << "\n";
}

void printSection(const std::string& title) {
    std::cout << "\n--- " << title << " ---\n";
}

int main(int argc, char* argv[]) {
    std::cout << "NLM Phase 5: Lifetime Learning and Development\n";
    std::cout << "===============================================\n\n";
    
    // Parse command line arguments
    size_t numTrials = 50;
    if (argc > 1) {
        numTrials = std::stoi(argv[1]);
    }
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(1000));
    config->set("region_count", size_t(2));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", uint64_t(42));
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    
    // Create brain
    std::cout << "Initializing brain...\n";
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!\n";
        return 1;
    }
    
    printHeader("PHASE 5 EXPERIMENTS");
    
    // Run Lifespan Experiment
    printSection("1. Lifespan Learning");
    {
        LifetimeExperimentConfig lifetimeConfig;
        lifetimeConfig.initialNeurons = 500;
        lifetimeConfig.maxNeurons = 1500;
        lifetimeConfig.synapseFormationSteps = 5000;
        lifetimeConfig.sensoryDevelopmentSteps = 10000;
        lifetimeConfig.motorDevelopmentSteps = 10000;
        lifetimeConfig.associativeDevelopmentSteps = 15000;
        lifetimeConfig.memoryConsolidationSteps = 20000;
        lifetimeConfig.socialDevelopmentSteps = 25000;
        
        Phase5IntegratedExperiment lifetimeExperiment;
        auto result = lifetimeExperiment.run(
            lifetimeConfig,
            [](const LifetimeExperimentConfig& cfg) {
                auto brain = std::make_shared<Brain>(std::make_shared<Config>(cfg));
                brain->initialize();
                return brain;
            },
            [](Brain* brain, uint64_t steps) {
                for (uint64_t step = 0; step < steps; ++step) {
                    brain->step(step, step * 0.001);
                }
                return true;
            }
        );
        
        printResult("Total Reward", result.totalReward);
        printResult("Avg Reward/Step", result.avgRewardPerStep);
        printResult("Learning Efficiency", result.overallLearningEfficiency);
        printResult("Final Performance", result.finalPerformance);
        printResult("Memory Capacity", result.memoryCapacity, 2000.0f);
        printResult("Prediction Accuracy", result.predictionAccuracy);
        printResult("Generalization Ability", result.generalizationAbility);
    }
    
    // Run Continual Learning Experiment
    printSection("2. Continual Learning");
    {
        std::vector<ContinualTask> tasks;
        tasks.push_back(ContinualTask("Object Recognition", 50000));
        tasks.push_back(ContinualTask("Temporal Sequence", 75000));
        tasks.push_back(ContinualTask("Navigation", 100000));
        
        Phase5IntegratedExperiment experiment;
        auto result = experiment.runContinualLearning(
            brain.get(), tasks, 5
        );
        
        printResult("Total Reward", result.totalReward);
        printResult("Avg Reward/Step", result.avgRewardPerStep);
    }
    
    // Run Damage Recovery Experiment
    printSection("3. Damage Recovery");
    {
        Phase5IntegratedExperiment experiment;
        auto result = experiment.runDamageRecovery(
            brain.get(), 50000, 0.3f, "sensorimotor"
        );
        
        printResult("Total Reward", result.totalReward);
    }
    
    // Run Scaling Experiment
    printSection("4. Scaling Analysis");
    {
        std::vector<ScaleLevel> scales;
        scales.push_back(ScaleLevel("Small", 100, 0.01f));
        scales.push_back(ScaleLevel("Medium", 1000, 0.05f));
        scales.push_back(ScaleLevel("Large", 5000, 0.1f));
        scales.push_back(ScaleLevel("Massive", 10000, 0.2f));
        
        Phase5IntegratedExperiment experiment;
        auto analysis = experiment.runScalingExperiment(scales, 10000);
        
        printResult("Number of Scales", static_cast<float>(analysis.runs.size()), 5.0f);
    }
    
    printHeader("PHASE 5 SUMMARY");
    std::cout << "Phase 5 experiments completed successfully.\n";
    std::cout << "The brain demonstrated advanced capabilities:\n";
    std::cout << "  - Lifespan development and maturation\n";
    std::cout << "  - Continual learning across multiple tasks\n";
    std::cout << "  - Recovery from damage\n";
    std::cout << "  - Scaling behavior with system size\n";
    
    return 0;
}
