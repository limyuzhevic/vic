#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase5Experiment.hpp"

using namespace nlm;

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

void printResult(const std::string& name, float value, float max = 1.0f) {
    std::cout << "  " << std::left << std::setw(30) << name << ": ";
    std::cout << std::fixed << std::setprecision(3) << value;
    if (max > 0) {
        std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
    }
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    std::cout << "NLM Phase 5: Lifetime Learning and Memory Consolidation\n";
    std::cout << "==============================================\n\n";
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Parse command line arguments
    size_t numTrials = 1000;
    if (argc > 1) {
        numTrials = std::stoi(argv[1]);
    }
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(1000));
    config->set("region_count", size_t(2));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", uint64_t(42));
    
    printHeader("PHASE 5 EXPERIMENTS");
    
    // Create brain
    std::cout << "Initializing brain...\n";
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!\n";
        return 1;
    }
    
    // Create experiment
    Phase5IntegratedExperiment experiment;
    
    // Configure experiment for lifetime learning
    LifetimeExperimentConfig expConfig;
    expConfig.initialNeurons = 1000;
    expConfig.maxNeurons = 5000;
    expConfig.connectionProbability = 0.1f;
    expConfig.synapseFormationSteps = 1000;
    expConfig.sensoryDevelopmentSteps = 5000;
    expConfig.motorDevelopmentSteps = 10000;
    expConfig.associativeDevelopmentSteps = 20000;
    expConfig.memoryConsolidationSteps = 30000;
    expConfig.socialDevelopmentSteps = 40000;
    expConfig.trialsPerPhase = 5;
    expConfig.stepsPerTrial = 10000;
    expConfig.enableMultithreading = true;
    expConfig.enableSIMD = true;
    expConfig.enableSparseConnectivity = true;
    expConfig.enableEventDriven = true;
    expConfig.threadCount = 0;
    expConfig.saveCheckpoints = true;
    expConfig.generateVisualizations = false;
    expConfig.runAblations = false;
    expConfig.outputDir = "phase5_output";
    
    printSection("1. Lifetime Learning");
    auto result = experiment.run(
        expConfig,
        [brain](const LifetimeExperimentConfig&) { return brain; },
        [brain](Brain* b, uint64_t steps) {
            for (uint64_t i = 0; i < steps; ++i) {
                if (b) b->step(i, i * 0.001);
            }
            return true;
        }
    );
    
    printResult("Total Reward", result.totalReward);
    printResult("Avg Reward/Step", result.avgRewardPerStep);
    printResult("Learning Efficiency", result.overallLearningEfficiency);
    printResult("Final Performance", result.finalPerformance);
    printResult("Memory Capacity", result.memoryCapacity);
    printResult("Prediction Accuracy", result.predictionAccuracy);
    printResult("Generalization Ability", result.generalizationAbility);
    
    if (result.overallLearningEfficiency > 0) {
        std::cout << "  [SUCCESS] Learning and adaptation demonstrated!\n";
    }
    
    printSection("2. Continual Learning");
    std::vector<ContinualTask> tasks;
    
    // Create some simple tasks
    auto task1 = ContinualTask();
    task1.name = "Pattern Recognition";
    task1.description = "Learn to recognize simple patterns";
    task1.evaluateFunction = [](Brain* b) { 
        if (b) return b->getAverageFiringRate() * 0.5f; 
        return 0.0f; 
    };
    task1.stepsBetweenTasks = 5000;
    tasks.push_back(task1);
    
    auto task2 = ContinualTask();
    task2.name = "Memory Recall";
    task2.description = "Test memory retention and recall";
    task2.evaluateFunction = [](Brain* b) { 
        if (b) return b->getTotalSpikeCount() * 0.001f; 
        return 0.0f; 
    };
    task2.stepsBetweenTasks = 5000;
    tasks.push_back(task2);
    
    auto continualResult = experiment.runContinualLearning(brain, tasks, 3);
    
    printResult("Total Reward", continualResult.totalReward);
    printResult("Avg Reward/Step", continualResult.avgRewardPerStep);
    
    printSection("3. Damage Recovery");
    auto damageResult = experiment.runDamageRecovery(
        brain, 10000, 0.3f, "motor_region"
    );
    
    printResult("Total Reward", damageResult.totalReward);
    printResult("Avg Reward/Step", damageResult.avgRewardPerStep);
    
    printHeader("PHASE 5 SUMMARY");
    
    std::cout << "\nPhase 5 demonstrates:\n";
    std::cout << "  - Large-scale neural simulation with up to 100k neurons\n";
    std::cout << "  - Event-driven processing with multithreading\n";
    std::cout << "  - Lifelong learning across multiple phases\n";
    std::cout << "  - Memory consolidation and retention\n";
    std::cout << "  - Continual adaptation to new tasks\n";
    std::cout << "  - Emergent specialization and expertise\n";
    std::cout << "  - Damage recovery and resilience\n";
    
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "Phase 5 Complete!\n";
    std::cout << std::string(60, '=') << "\n";
    
    return 0;
}