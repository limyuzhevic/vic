// Main Phase 5 demonstration
// Shows: continual learning across multiple tasks and domains

#include "Phase5Experiment.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>

namespace nlm {

class Phase5Demo {
public:
    Phase5Demo() {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 5 - CONTINUAL LEARNING DEMO\n";
        std::cout << "===========================================\n\n";
    }
    
    ~Phase5Demo() {}
    
    void run(int numTasks = 3, int cyclesPerTask = 5, uint64_t stepsPerCycle = 1000) {
        std::cout << "Running Phase 5 continual learning demonstration..." << std::endl << std::endl;
        
        // Create configuration
        auto config = std::make_shared<Config>();
        configureConfig(config);
        
        // Setup validation rules for Phase 5
        setupValidationRules(config);
        
        // Validate configuration
        auto validationResult = config->validate();
        if (!validationResult.valid) {
            std::cerr << "WARNING: Configuration validation failed:" << std::endl;
            std::cerr << config->getErrorMessage(validationResult.errors[0]) << std::endl;
        }
        
        // Create world
        SimpleWorld world;
        world.initialize(20, 20);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain" << std::endl;
            return;
        }
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Configure agent brain for Phase 5
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        std::cout << "Brain initialized successfully" << std::endl;
        std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << std::endl;
        
        // Create tasks for continual learning
        std::vector<ContinualTask> tasks;
        createTasks(tasks);
        
        // Run continual learning experiment
        std::cout << "Running continual learning across " << tasks.size() << " tasks, " 
                  << cyclesPerTask << " cycles each" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run the experiment
        auto result = brain->getIntegrationExperiment().runContinualLearning(
            brain, tasks, cyclesPerTask
        );
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double wallClockTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Display results
        displayResults(result, tasks, wallClockTime);
        
        // Generate JSON report
        std::string jsonReport = generateJSONReport(result, tasks);
        std::cout << "\nJSON Report:" << std::endl;
        std::cout << jsonReport << std::endl;
        
        // Save report to file
        std::string reportFilename = "phase5_demo_report.json";
        std::ofstream reportFile(reportFilename);
        if (reportFile.is_open()) {
            reportFile << jsonReport;
            std::cout << "Report saved to " << reportFilename << std::endl;
            reportFile.close();
        }
    }
    
private:
    void configureConfig(std::shared_ptr<Config> config) {
        config->set("neuron_count", 1000);
        config->set("region_count", 2);
        config->set("connection_probability", 0.15f);
        config->set("random_seed", 42u);
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        
        // Environment settings
        config->set("environment_width", 20);
        config->set("environment_height", 20);
        
        // Memory settings
        config->set("working_memory_capacity", 100);
        config->set("episodic_memory_capacity", 50);
        
        // Development settings
        config->set("development_enabled", true);
        config->set("maturation_rate", 0.001);
        
        // Neuromodulation settings
        config->set("dopamine_baseline", 0.1f);
        config->set("curiosity_baseline", 0.05f);
        
        // Learning settings
        config->set("learning_rate", 0.01f);
        config->set("discount_factor", 0.99f);
    }
    
    void setupValidationRules(std::shared_ptr<Config> config) {
        // Add validation rules for Phase 5 configuration
        Config::ValidationRule neuronCountRule;
        neuronCountRule.key = "neuron_count";
        neuronCountRule.description = "Number of neurons in the brain";
        neuronCountRule.required = true;
        neuronCountRule.minValue = "10";
        neuronCountRule.maxValue = "10000";
        neuronCountRule.validator = [](const Config::ConfigValue& value) -> bool {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) {
                    return arg >= 10 && arg <= 10000;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    return arg >= 10 && arg <= 10000;
                } else {
                    return false;
                }
            }, value);
        };
        config->addValidationRule(neuronCountRule);
        
        Config::ValidationRule connectionProbRule;
        connectionProbRule.key = "connection_probability";
        connectionProbRule.description = "Connection probability for random connectivity";
        connectionProbRule.minValue = "0.0";
        connectionProbRule.maxValue = "1.0";
        connectionProbRule.validator = [](const Config::ConfigValue& value) -> bool {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, double>) {
                    return arg >= 0.0 && arg <= 1.0;
                } else if constexpr (std::is_same_v<T, float>) {
                    return arg >= 0.0 && arg <= 1.0;
                } else {
                    return false;
                }
            }, value);
        };
        config->addValidationRule(connectionProbRule);
        
        Config::ValidationRule learningRateRule;
        learningRateRule.key = "learning_rate";
        learningRateRule.description = "Learning rate for plasticity rules";
        learningRateRule.minValue = "0.0";
        learningRateRule.maxValue = "1.0";
        learningRateRule.validator = [](const Config::ConfigValue& value) -> bool {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, double>) {
                    return arg >= 0.0 && arg <= 1.0;
                } else if constexpr (std::is_same_v<T, float>) {
                    return arg >= 0.0 && arg <= 1.0;
                } else {
                    return false;
                }
            }, value);
        };
        config->addValidationRule(learningRateRule);
    }
    
    void createTasks(std::vector<ContinualTask>& tasks) {
        // Task 1: Navigation
        ContinualTask navTask;
        navTask.name = "Navigation";
        navTask.stepsBetweenTasks = 100;
        navTask.evaluateFunction = [](Brain* brain) -> float {
            if (!brain) return 0.0f;
            return brain->getAverageFiringRate() * 0.1f;
        };
        tasks.push_back(navTask);
        
        // Task 2: Object Recognition  
        ContinualTask recTask;
        recTask.name = "Object Recognition";
        recTask.stepsBetweenTasks = 150;
        recTask.evaluateFunction = [](Brain* brain) -> float {
            if (!brain) return 0.0f;
            return brain->getEpisodicMemory() ? brain->getEpisodicMemory()->getEpisodeCount() * 0.5f : 0.0f;
        };
        tasks.push_back(recTask);
        
        // Task 3: Motor Control
        ContinualTask motorTask;
        motorTask.name = "Motor Control";
        motorTask.stepsBetweenTasks = 200;
        motorTask.evaluateFunction = [](Brain* brain) -> float {
            if (!brain) return 0.0f;
            return brain->getPredictionSystem() ? brain->getPredictionSystem()->getAccuracy() * 100.0f : 0.0f;
        };
        tasks.push_back(motorTask);
    }
    
    void displayResults(const LifetimeExperimentResult& result, 
                       const std::vector<ContinualTask>& tasks, 
                       double wallClockTime) {
        std::cout << "===========================================\n";
        std::cout << "PHASE 5 DEMONSTRATION RESULTS\n";
        std::cout << "===========================================\n\n";
        
        std::cout << "Performance Summary:" << std::endl;
        std::cout << "  Total Reward: " << std::fixed << std::setprecision(2) << result.totalReward << std::endl;
        std::cout << "  Average Reward per Step: " << std::fixed << std::setprecision(4) 
                  << result.avgRewardPerStep << std::endl;
        std::cout << "  Learning Efficiency: " << std::fixed << std::setprecision(4) 
                  << result.overallLearningEfficiency << std::endl;
        std::cout << "  Final Performance: " << std::fixed << std::setprecision(2) 
                  << result.finalPerformance << std::endl;
        std::cout << "  Memory Capacity: " << std::fixed << std::setprecision(1) 
                  << result.memoryCapacity << std::endl;
        std::cout << "  Prediction Accuracy: " << std::fixed << std::setprecision(2) 
                  << result.predictionAccuracy << std::endl;
        std::cout << "  Generalization Ability: " << std::fixed << std::setprecision(2) 
                  << result.generalizationAbility << std::endl;
        std::cout << "  Wall Clock Time: " << std::fixed << std::setprecision(2) << wallClockTime << "s" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Phase Breakdown:" << std::endl;
        for (size_t i = 0; i < result.phaseResults.size(); ++i) {
            const auto& phase = result.phaseResults[i];
            std::cout << "  Phase " << (i + 1) << ": " << phase.phaseName << std::endl;
            std::cout << "    Duration: " << phase.duration << "s" << std::endl;
            std::cout << "    Steps: " << phase.startStep << " - " << phase.endStep << std::endl;
            std::cout << "    Total Reward: " << std::fixed << std::setprecision(2) << phase.totalReward << std::endl;
            std::cout << "    Average Firing Rate: " << std::fixed << std::setprecision(2) << phase.avgFiringRate << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Continuum of Learning:" << std::endl;
        for (size_t i = 0; i < result.phaseResults.size(); ++i) {
            const auto& phase = result.phaseResults[i];
            if (i > 0) {
                const auto& prevPhase = result.phaseResults[i - 1];
                float rewardChange = phase.totalReward - prevPhase.totalReward;
                float synapseChange = static_cast<float>(phase.endStep - phase.startStep) / 1000.0f;  // Normalized
                
                if (std::abs(rewardChange) > 0.1f) {
                    std::cout << "    Phase " << (i + 1) << " vs Phase " << i << ": Reward changed by " 
                              << std::fixed << std::setprecision(2) << rewardChange << std::endl;
                }
            }
        }
    }
    
    std::string generateJSONReport(const LifetimeExperimentResult& result,
                                   const std::vector<ContinualTask>& tasks) {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"experiment_type\": \"Phase 5 Continual Learning\",\n";
        oss << "  \"total_reward\": " << result.totalReward << ",\n";
        oss << "  \"avg_reward_per_step\": " << result.avgRewardPerStep << ",\n";
        oss << "  \"overall_learning_efficiency\": " << result.overallLearningEfficiency << ",\n";
        oss << "  \"final_performance\": " << result.finalPerformance << ",\n";
        oss << "  \"memory_capacity\": " << result.memoryCapacity << ",\n";
        oss << "  \"prediction_accuracy\": " << result.predictionAccuracy << ",\n";
        oss << "  \"generalization_ability\": " << result.generalizationAbility << ",\n";
        oss << "  \"phases\": [\n";
        
        for (size_t i = 0; i < result.phaseResults.size(); ++i) {
            const auto& phase = result.phaseResults[i];
            oss << "    {\n";
            oss << "      \"name\": \"" << phase.phaseName << "\",\n";
            oss << "      \"start_step\": " << phase.startStep << ",\n";
            oss << "      \"end_step\": " << phase.endStep << ",\n";
            oss << "      \"duration\": " << phase.duration << ",\n";
            oss << "      \"total_reward\": " << phase.totalReward << "\n";
            oss << "    }";
            if (i < result.phaseResults.size() - 1) oss << ",";
            oss << "\n";
        }
        
        oss << "  ],\n";
        oss << "  \"tasks\": [\n";
        
        for (size_t i = 0; i < tasks.size(); ++i) {
            const auto& task = tasks[i];
            oss << "    {\n";
            oss << "      \"name\": \"" << task.name << "\",\n";
            oss << "      \"steps_between_tasks\": " << task.stepsBetweenTasks << "\n";
            oss << "    }";
            if (i < tasks.size() - 1) oss << ",";
            oss << "\n";
        }
        
        oss << "  ]\n";
        oss << "}\n";
        
        return oss.str();
    }
};

} // namespace nlm

// Main entry point for Phase 5 demo
int main(int argc, char* argv[]) {
    int numTasks = 3;
    int cyclesPerTask = 5;
    uint64_t stepsPerCycle = 1000;
    
    if (argc > 1) {
        numTasks = std::atoi(argv[1]);
    }
    if (argc > 2) {
        cyclesPerTask = std::atoi(argv[2]);
    }
    if (argc > 3) {
        stepsPerCycle = static_cast<uint64_t>(std::atoi(argv[3]));
    }
    
    nlm::Phase5Demo demo;
    demo.run(numTasks, cyclesPerTask, stepsPerCycle);
    
    return 0;
}
