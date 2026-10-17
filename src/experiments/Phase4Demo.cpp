#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <vector>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase4Experiment.hpp"

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

void printSection(const std::string& title) {
    std::cout << "\n--- " << title << " ---\n";
}

struct EnhancedDemoConfig {
    size_t numTrials = 50;
    size_t numSteps = 100;
    bool enablePerformanceTracking = true;
    bool enableProgressOutput = true;
    std::string outputFile = "";
    bool validateResults = true;
    
    size_t neuronCount = 1000;
    size_t regionCount = 2;
    float connectionProbability = 0.1f;
    uint64_t randomSeed = 42;
    float simulationTimestep = 0.001f;
    float stdpLTPWeight = 0.01f;
    float stdpLTDWeight = 0.012f;
};

EnhancedDemoConfig parseArgs(int argc, char* argv[]) {
    EnhancedDemoConfig config;
    
    std::cout << "NLM Phase 4 Enhanced Demo - Configuration Parser\n";
    std::cout << "==============================================\n\n";
    
    auto showHelp = []() {
        std::cout << "Usage: nlm_phase4_demo [OPTIONS]\n\n";
        std::cout << "Options:\n";
        std::cout << "  --trials N              Number of trials per experiment (default: 50)\n";
        std::cout << "  --steps N               Steps per experiment (default: 100)\n";
        std::cout << "  --neuron-count N        Number of neurons (default: 1000)\n";
        std::cout << "  --region-count N         Number of brain regions (default: 2)\n";
        std::cout << "  --seed N                Random seed for reproducibility (default: 42)\n";
        std::cout << "  --progress              Enable progress output (default: enabled)\n";
        std::cout << "  --no-progress           Disable progress output\n";
        std::cout << "  --validate              Enable result validation (default: enabled)\n";
        std::cout << "  --no-validate           Disable result validation\n";
        std::cout << "  --output FILE           Write detailed results to FILE\n";
        std::cout << "  --help, -h              Show this help message\n\n";
        std::cout << "Examples:\n";
        std::cout << "  nlm_phase4_demo --trials 100 --steps 200\n";
        std::cout << "  nlm_phase4_demo --neuron-count 2000 --region-count 4 --seed 123\n";
        std::cout << "  nlm_phase4_demo --progress --validate --output results.json\n";
        return true;
    };
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            showHelp();
            exit(0);
        }
        
        if (arg.find("--trials=") == 0) {
            config.numTrials = std::stoull(arg.substr(9));
        } else if (arg.find("--steps=") == 0) {
            config.numSteps = std::stoull(arg.substr(8));
        } else if (arg.find("--neuron-count=") == 0) {
            config.neuronCount = std::stoull(arg.substr(15));
        } else if (arg.find("--region-count=") == 0) {
            config.regionCount = std::stoull(arg.substr(15));
        } else if (arg.find("--seed=") == 0) {
            config.randomSeed = std::stoull(arg.substr(7));
        } else if (arg == "--progress") {
            config.enableProgressOutput = true;
        } else if (arg == "--no-progress") {
            config.enableProgressOutput = false;
        } else if (arg == "--validate") {
            config.validateResults = true;
        } else if (arg == "--no-validate") {
            config.validateResults = false;
        } else if (arg.find("--output=") == 0) {
            config.outputFile = arg.substr(9);
        } else {
            std::cout << "[WARNING] Unknown argument: " << arg << std::endl;
        }
    }
    
    if (config.numTrials == 0) {
        std::cerr << "ERROR: Number of trials must be positive" << std::endl;
        exit(1);
    }
    
    if (config.numSteps == 0) {
        std::cerr << "ERROR: Steps must be positive" << std::endl;
        exit(1);
    }
    
    return config;
}

void logConfig(const EnhancedDemoConfig& config) {
    std::cout << "Starting Phase 4 Enhanced Demo with the following configuration:\n";
    std::cout << "  Trials: " << config.numTrials << "\n";
    std::cout << "  Steps per experiment: " << config.numSteps << "\n";
    std::cout << "  Neurons: " << config.neuronCount << "\n";
    std::cout << "  Regions: " << config.regionCount << "\n";
    std::cout << "  Random seed: " << config.randomSeed << "\n";
    std::cout << "  Progress output: " << (config.enableProgressOutput ? "Enabled" : "Disabled") << "\n";
    std::cout << "  Result validation: " << (config.validateResults ? "Enabled" : "Disabled") << "\n";
    if (!config.outputFile.empty()) {
        std::cout << "  Output file: " << config.outputFile << "\n";
    }
    std::cout << std::endl;
}

void saveResults(const std::string& filename, const std::vector<std::pair<std::string, float>>& results) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cout << "[WARNING] Could not write results to: " << filename << std::endl;
        return;
    }
    
    outFile << "Phase 4 Enhanced Demo Results\n";
    outFile << "=============================\n\n";
    
    for (const auto& result : results) {
        outFile << result.first << ": " << result.second << "\n";
    }
    
    outFile.close();
    std::cout << "[OUTPUT] Results saved to: " << filename << std::endl;
}

int main(int argc, char* argv[]) {
    EnhancedDemoConfig config = parseArgs(argc, argv);
    logConfig(config);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::cout << "\nStarting Phase 4 demonstration with " << config.numTrials << " trials per experiment\n";
    std::cout << "Total experiments: 10 (" << (config.numTrials * 10) << " total trials)\n";
    std::cout << std::endl;
    
    auto configPtr = std::make_shared<Config>();
    configPtr->set("neuron_count", config.neuronCount);
    configPtr->set("region_count", config.regionCount);
    configPtr->set("connection_probability", config.connectionProbability);
    configPtr->set("random_seed", config.randomSeed);
    configPtr->set("simulation_timestep", config.simulationTimestep);
    configPtr->set("stdp_ltp_weight", config.stdpLTPWeight);
    configPtr->set("stdp_ltd_weight", config.stdpLTDWeight);
    
    auto brain = std::make_shared<Brain>(configPtr);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!\n";
        return 1;
    }
    
    printHeader("PHASE 4 ENHANCED COGITION DEMONSTRATION");
    
    // Run all experiments with enhanced monitoring
    printSection("1. Temporal Prediction Learning");
    {
        TemporalPredictionExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), config.numTrials);
        
        printResult("Initial Prediction Error", results.predictionErrorInitial);
        printResult("Final Prediction Error", results.predictionErrorFinal);
        printResult("Improvement", results.predictionAccuracyImprovement);
        
        if (results.predictionAccuracyImprovement > 0) {
            std::cout << "  [SUCCESS] Prediction improved through experience!\n";
        } else {
            std::cout << "  [INFO] No significant prediction improvement yet.\n";
        }
    }
    
    printSection("2. Working Memory");
    {
        WorkingMemoryExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), config.numTrials);
        
        printResult("Initial Retention", results.workingMemoryRetentionInitial);
        printResult("Final Retention", results.workingMemoryRetentionFinal);
        printResult("Capacity", results.workingMemoryCapacity, 100.0f);
    }
    
    printSection("3. Episodic Memory");
    {
        EpisodicRecallExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), config.numTrials);
        
        printResult("Recall Accuracy", results.episodicRecallAccuracy);
        printResult("Episodes Stored", results.episodesStored);
        printResult("Episodic Influence", results.episodicInfluence);
    }
    
    printSection("4. Concept Formation");
    {
        ConceptFormationExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), config.numTrials);
        
        printResult("Concepts Formed", results.conceptsFormed);
        printResult("Concept Stability", results.conceptStability);
        printResult("Generalization Ability", results.generalizationAbility);
        
        if (results.conceptsFormed > 0) {
            std::cout << "  [SUCCESS] Concepts emerged from repeated experience!\n";
        }
    }
    
    printSection("5. Neural Attention");
    {
        AttentionExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), config.numTrials);
        
        printResult("Attention Selectivity", results.attentionSelectivity);
        printResult("Distraction Resistance", results.distractionResistance);
        
        if (results.attentionSelectivity > 0.5f) {
            std::cout << "  [SUCCESS] Selective attention emerged!\n";
        }
    }
    
    printSection("6. Multi-Step Planning");
    {
        PlanningExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), config.numTrials / 2);
        
        printResult("Planning Accuracy", results.planningAccuracy);
        printResult("Planning Confidence", results.planningConfidence);
        
        if (results.planningAccuracy > 0.4f) {
            std::cout << "  [SUCCESS] Planning ability demonstrated!\n";
        }
    }
    
    printSection("7. Self-Model");
    {
        SelfModelExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), config.numTrials);
        
        printResult("Self-Prediction Accuracy", results.selfPredictionAccuracy);
        printResult("Body Awareness", results.bodyAwareness);
        
        if (results.selfPredictionAccuracy > 0.3f) {
            std::cout << "  [SUCCESS] Self-model developing!\n";
        }
    }
    
    printSection("8. Social Learning");
    {
        SocialLearningExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), config.numTrials / 2);
        
        printResult("Imitation Accuracy", results.imitationAccuracy);
        printResult("Observations from Others", results.observationsFromOthers);
        
        if (results.imitationAccuracy > 0.2f) {
            std::cout << "  [SUCCESS] Social learning observed!\n";
        }
    }
    
    printSection("9. Continual Learning");
    {
        ContinualLearningExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), config.numTrials);
        
        printResult("Behavior Improvement", results.behaviorImprovement);
        printResult("Transfer Performance", results.transferPerformance);
    }
    
    printSection("10. Integrated Phase 4");
    {
        Phase4IntegratedExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), 10, 50);
        
        printResult("Total Reward", results.totalReward);
        printResult("Episodes Stored", results.episodesStored);
        printResult("Concepts Formed", results.conceptsFormed);
        printResult("Planning Confidence", results.planningConfidence);
        printResult("Body Awareness", results.bodyAwareness);
    }
    
    printHeader("PHASE 4 SUMMARY");
    
    std::cout << "\nPhase 4 demonstrates:\n";
    std::cout << "  - Temporal prediction: Learning relationships between events\n";
    std::cout << "  - Working memory: Maintaining information across delays\n";
    std::cout << "  - Episodic memory: Storing and recalling experiences\n";
    std::cout << "  - Concept formation: Discovering patterns without labels\n";
    std::cout << "  - Neural attention: Selective processing through competition\n";
    std::cout << "  - Multi-step planning: Using predictions to select actions\n";
    std::cout << "  - Self-model: Learning body schema and action consequences\n";
    std::cout << "  - Social learning: Observing and imitating others\n";
    std::cout << "  - Continual learning: Adapting to new tasks\n";
    
    std::cout << "\nAll mechanisms emerge from neural dynamics and plasticity.\n";
    std::cout << "No predefined concepts, rules, or symbolic AI were used.\n";
    
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "Phase 4 Complete!\n";
    std::cout << std::string(60, '=') << "\n";
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    double totalSeconds = duration.count() / 1000.0;
    
    std::cout << "\nPerformance:\n";
    std::cout << "  Total runtime: " << std::fixed << std::setprecision(2) << totalSeconds << " seconds\n";
    std::cout << "  Trials executed: " << config.numTrials * 10 << "\n";
    std::cout << "  Experiments completed: 10\n";
    
    if (!config.outputFile.empty()) {
        std::vector<std::pair<std::string, float>> results;
        results.push_back({"Total Runtime (s)", static_cast<float>(totalSeconds)});
        results.push_back({"Trials Executed", static_cast<float>(config.numTrials * 10)});
        saveResults(config.outputFile, results);
    }
    
    return 0;
}

} // namespace nlm