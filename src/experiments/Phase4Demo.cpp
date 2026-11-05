#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>
#include <string>
#include <stdexcept>
#include <chrono>
#include <map>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"

using namespace nlm;

// Enhanced Phase 4 demo with comprehensive error handling and diagnostics
// Demonstrates emerging cognitive abilities: prediction, memory, concept formation, planning

// Global statistics tracking
struct GlobalStatistics {
    int totalTrials = 0;
    int successfulTrials = 0;
    float totalPredictionImprovement = 0.0f;
    float totalMemoryRetention = 0.0f;
    float totalPlanningAccuracy = 0.0f;
    double totalExecutionTime = 0.0;
    std::map<std::string, float> systemMetrics;
};

GlobalStatistics globalStats;

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

void printResult(const std::string& name, float value, float max = 1.0f, const std::string& unit = "") {
    std::cout << "  " << std::left << std::setw(30) << name << ": ";
    std::cout << std::fixed << std::setprecision(3) << value;
    if (!unit.empty()) {
        std::cout << " " << unit;
    }
    if (max > 0) {
        std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
    }
    std::cout << "\n";
}

void printSection(const std::string& title) {
    std::cout << "\n--- " << title << " ---\n";
}

void updateGlobalStats(const std::string& experimentName, float improvement, float retention, float accuracy, double executionTime) {
    globalStats.totalTrials++;
    globalStats.totalPredictionImprovement += improvement;
    globalStats.totalMemoryRetention += retention;
    globalStats.totalPlanningAccuracy += accuracy;
    globalStats.totalExecutionTime += executionTime;
    
    if (improvement > 0.5f || retention > 0.7f || accuracy > 0.8f) {
        globalStats.successfulTrials++;
    }
    
    globalStats.systemMetrics[experimentName + "_improvement"] = improvement;
    globalStats.systemMetrics[experimentName + "_retention"] = retention;
    globalStats.systemMetrics[experimentName + "_accuracy"] = accuracy;
}

void printGlobalSummary() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "        PHASE 4 GLOBAL SUMMARY\n";
    std::cout << std::string(60, '=') << "\n";
    
    if (globalStats.totalTrials > 0) {
        printResult("Total trials completed", globalStats.totalTrials);
        printResult("Successful trials", globalStats.successfulTrials);
        printResult("Success rate", globalStats.successfulTrials * 100.0f / globalStats.totalTrials, 100.0f, "%");
        printResult("Average prediction improvement", 
                   globalStats.totalPredictionImprovement / globalStats.totalTrials, 1.0f, "%");
        printResult("Average memory retention", 
                   globalStats.totalMemoryRetention / globalStats.totalTrials, 1.0f, "%");
        printResult("Average planning accuracy", 
                   globalStats.totalPlanningAccuracy / globalStats.totalTrials, 1.0f, "%");
        printResult("Average execution time", 
                   globalStats.totalExecutionTime / globalStats.totalTrials, 1.0f, "s");
    }
}

void runTemporalPredictionExperiment(const std::shared_ptr<Brain>& brain, size_t numTrials) {
    printSection("1. Temporal Prediction Learning");
    
    try {
        TemporalPredictionExperiment experiment;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        Phase4Results results = experiment.run(brain.get(), numTrials);
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        printResult("Initial Prediction Error", results.predictionErrorInitial);
        printResult("Final Prediction Error", results.predictionErrorFinal);
        printResult("Improvement", results.predictionAccuracyImprovement);
        
        float improvement = results.predictionErrorInitial - results.predictionErrorFinal;
        if (improvement > 0) {
            std::cout << "  [SUCCESS] Prediction improved through experience!\n";
        } else {
            std::cout << "  [INFO] No significant prediction improvement yet.\n";
        }
        
        updateGlobalStats("TemporalPrediction", improvement, results.predictionAccuracyImprovement, 0.0f, duration.count());
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR in Temporal Prediction Experiment: " << e.what() << std::endl;
        updateGlobalStats("TemporalPrediction", -1.0f, 0.0f, 0.0f, 0.0);
    }
}

void runWorkingMemoryExperiment(const std::shared_ptr<Brain>& brain, size_t numTrials) {
    printSection("2. Working Memory");
    
    try {
        WorkingMemoryExperiment experiment;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        Phase4Results results = experiment.run(brain.get(), numTrials);
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        printResult("Initial Retention", results.workingMemoryRetentionInitial);
        printResult("Final Retention", results.workingMemoryRetentionFinal);
        printResult("Capacity", results.workingMemoryCapacity, 100.0f);
        
        float retentionImprovement = results.workingMemoryRetentionFinal - results.workingMemoryRetentionInitial;
        updateGlobalStats("WorkingMemory", 0.0f, retentionImprovement, results.workingMemoryCapacity / 100.0f, duration.count());
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR in Working Memory Experiment: " << e.what() << std::endl;
        updateGlobalStats("WorkingMemory", 0.0f, -1.0f, 0.0f, 0.0);
    }
}

void runEpisodicRecallExperiment(const std::shared_ptr<Brain>& brain, size_t numTrials) {
    printSection("3. Episodic Memory");
    
    try {
        EpisodicRecallExperiment experiment;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        Phase4Results results = experiment.run(brain.get(), numTrials);
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        printResult("Initial Recall Accuracy", results.episodicRecallInitial);
        printResult("Final Recall Accuracy", results.episodicRecallFinal);
        printResult("Recall Improvement", results.episodicRecallImprovement);
        
        float recallImprovement = results.episodicRecallFinal - results.episodicRecallInitial;
        updateGlobalStats("EpisodicRecall", 0.0f, recallImprovement, results.episodicRecallImprovement, duration.count());
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR in Episodic Recall Experiment: " << e.what() << std::endl;
        updateGlobalStats("EpisodicRecall", 0.0f, -1.0f, 0.0f, 0.0);
    }
}

void runConceptFormationExperiment(const std::shared_ptr<Brain>& brain, size_t numTrials) {
    printSection("4. Concept Formation");
    
    try {
        ConceptFormationExperiment experiment;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        Phase4Results results = experiment.run(brain.get(), numTrials);
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        printResult("Initial Concept Clarity", results.conceptClarityInitial);
        printResult("Final Concept Clarity", results.conceptClarityFinal);
        printResult("Concept Formation Rate", results.conceptFormationRate, 1.0f);
        
        float clarityImprovement = results.conceptClarityFinal - results.conceptClarityInitial;
        updateGlobalStats("ConceptFormation", 0.0f, clarityImprovement, results.conceptFormationRate, duration.count());
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR in Concept Formation Experiment: " << e.what() << std::endl;
        updateGlobalStats("ConceptFormation", 0.0f, -1.0f, 0.0f, 0.0);
    }
}

void runPlanningExperiment(const std::shared_ptr<Brain>& brain, size_t numTrials) {
    printSection("5. Planning and Decision Making");
    
    try {
        PlanningExperiment experiment;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        Phase4Results results = experiment.run(brain.get(), numTrials);
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        printResult("Initial Planning Accuracy", results.planningAccuracyInitial);
        printResult("Final Planning Accuracy", results.planningAccuracyFinal);
        printResult("Planning Improvement", results.planningAccuracyImprovement);
        
        float planningImprovement = results.planningAccuracyFinal - results.planningAccuracyInitial;
        updateGlobalStats("Planning", 0.0f, planningImprovement, results.planningAccuracyFinal, duration.count());
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR in Planning Experiment: " << e.what() << std::endl;
        updateGlobalStats("Planning", 0.0f, -1.0f, 0.0f, 0.0);
    }
}

void printEnhancedSystemStatus(const std::shared_ptr<Brain>& brain, const std::shared_ptr<AgentBrain>& agent) {
    printSection("System Status");
    
    std::cout << "  Brain System Status:";
    std::cout << "    Neurons: " << brain->getTotalNeuronCount() << "\n";
    std::cout << "    Synapses: " << brain->getTotalSynapseCount() << "\n";
    std::cout << "    E/I Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
    std::cout << "    Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
    std::cout << "    Total Spikes: " << brain->getTotalSpikeCount() << "\n";
    std::cout << "    Development Stage: " << brain->getDevelopmentalStage().name << "\n";
    
    std::cout << "  Agent System Status:";
    std::cout << "    Curiosity Level: " << agent->getCuriosityLevel() << "\n";
    std::cout << "    Novelty Level: " << agent->getNoveltyLevel() << "\n";
    std::cout << "    Neuromodulation Level: " << agent->getNeuromodulationLevel() << "\n";
    std::cout << "    Prediction Error: " << agent->getPredictionError() << "\n";
    
    if (brain->getWorkingMemory()) {
        std::cout << "    Working Memory Traces: " << brain->getWorkingMemory()->getActiveTraces() << "\n";
    }
    if (brain->getEpisodicMemory()) {
        std::cout << "    Episodic Episodes Stored: " << brain->getEpisodicMemory()->getEpisodeCount() << "\n";
    }
}

void printPerformanceMetrics(double startTime, double endTime) {
    double totalTime = endTime - startTime;
    std::cout << "\n  Performance Metrics:";
    std::cout << "    Total execution time: " << totalTime << " seconds\n";
    std::cout << "    Average per trial: " << (globalStats.totalTrials > 0 ? totalTime / globalStats.totalTrials : 0.0) << " seconds\n";
}

int main(int argc, char* argv[]) {
    std::cout << "NLM Phase 4: Emerging Cognition\n";
    std::cout << "================================\n\n";
    
    // Parse command line arguments
    size_t numTrials = 50;
    if (argc > 1) {
        try {
            numTrials = std::stoi(argv[1]);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Invalid trial count, using default (50): " << e.what() << std::endl;
        }
    }
    
    std::cout << "Phase 4 Configuration:";
    std::cout << "  Number of trials per experiment: " << numTrials << "\n\n";
    
    // Initialize logging
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    // Create configuration with enhanced error checking
    auto config = std::make_shared<Config>();
    try {
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        
        std::cout << "Configuration created successfully.\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Failed to create configuration: " << e.what() << std::endl;
        return 1;
    }
    
    // Create brain with error checking
    std::cout << "Initializing brain...\n";
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    printHeader("PHASE 4 EXPERIMENTS");
    printEnhancedSystemStatus(brain, nullptr);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run all Phase 4 experiments with comprehensive error handling
    try {
        runTemporalPredictionExperiment(brain, numTrials);
        runWorkingMemoryExperiment(brain, numTrials);
        runEpisodicRecallExperiment(brain, numTrials);
        runConceptFormationExperiment(brain, numTrials);
        runPlanningExperiment(brain, numTrials);
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Critical failure in Phase 4 experiments: " << e.what() << std::endl;
        // Continue with global summary even if some experiments fail
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalExecutionTime = std::chrono::duration<double>(endTime - startTime).count();
    
    printGlobalSummary();
    printPerformanceMetrics(startTime, endTime);
    
    // Display final system status
    printEnhancedSystemStatus(brain, nullptr);
    
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "       PHASE 4 EXECUTION COMPLETE\n";
    std::cout << std::string(60, '=') << "\n";
    
    if (globalStats.totalTrials > 0) {
        std::cout << "SUMMARY: " << globalStats.successfulTrials << "/" << globalStats.totalTrials << 
                  " experiments showed successful improvement.\n";
    }
    
    return 0;
}