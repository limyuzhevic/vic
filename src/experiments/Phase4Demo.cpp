#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>
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

int main(int argc, char* argv[]) {
    std::cout << "NLM Phase 4: Emerging Cognition\n";
    std::cout << "================================\n\n";
    
    // Parse command line arguments with better error handling
    size_t numTrials = 50;
    
    // Check for help argument
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--help" || std::string(argv[i]) == "-h") {
            std::cout << "Usage: " << argv[0] << " [num_trials]\n";
            std::cout << "  num_trials: Number of trials to run (default: 50)\n";
            std::cout << "Example: " << argv[0] << " 100\n";
            return 0;
        }
    }
    
    // Parse number of trials with validation
    if (argc > 1) {
        try {
            numTrials = std::stoull(argv[1]);
            if (numTrials < 1) {
                std::cerr << "Error: Number of trials must be at least 1\n";
                return 1;
            }
            if (numTrials > 1000) {
                std::cout << "Warning: High number of trials (" << numTrials << ") may take a long time\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid number of trials: " << argv[1] << "\n";
            std::cerr << "Reason: " << e.what() << "\n";
            return 1;
        }
    }
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(1000));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", static_cast<int64_t>(42));
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
    
    printHeader("PHASE 4 EXPERIMENTS");
    
    // Run Temporal Prediction Experiment
    printSection("1. Temporal Prediction Learning");
    {
        TemporalPredictionExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), numTrials);
        
        printResult("Initial Prediction Error", results.predictionErrorInitial);
        printResult("Final Prediction Error", results.predictionErrorFinal);
        printResult("Improvement", results.predictionAccuracyImprovement);
        
        float improvement = results.predictionErrorInitial - results.predictionErrorFinal;
        if (improvement > 0) {
            std::cout << "  [SUCCESS] Prediction improved through experience!\n";
        } else {
            std::cout << "  [INFO] No significant prediction improvement yet.\n";
        }
    }
    
    // Run Working Memory Experiment
    printSection("2. Working Memory");
    {
        WorkingMemoryExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), numTrials);
        
        printResult("Initial Retention", results.workingMemoryRetentionInitial);
        printResult("Final Retention", results.workingMemoryRetentionFinal);
        printResult("Capacity", results.workingMemoryCapacity, 100.0f);
    }
    
    // Run Episodic Recall Experiment
    printSection("3. Episodic Memory");
    {
        EpisodicRecallExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), numTrials);
        
        printResult("Recall Accuracy", results.episodicRecallAccuracy);
        printResult("Episodes Stored", results.episodesStored);
        printResult("Episodic Influence", results.episodicInfluence);
    }
    
    // Run Concept Formation Experiment
    printSection("4. Concept Formation");
    {
        ConceptFormationExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), numTrials);
        
        printResult("Concepts Formed", results.conceptsFormed);
        printResult("Concept Stability", results.conceptStability);
        printResult("Generalization Ability", results.generalizationAbility);
        
        if (results.conceptsFormed > 0) {
            std::cout << "  [SUCCESS] Concepts emerged from repeated experience!\n";
        }
    }
    
    // Run Attention Experiment
    printSection("5. Neural Attention");
    {
        AttentionExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), numTrials);
        
        printResult("Attention Selectivity", results.attentionSelectivity);
        printResult("Distraction Resistance", results.distractionResistance);
        
        if (results.attentionSelectivity > 0.5f) {
            std::cout << "  [SUCCESS] Selective attention emerged!\n";
        }
    }
    
    // Run Planning Experiment
    printSection("6. Multi-Step Planning");
    {
        PlanningExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), numTrials / 2);
        
        printResult("Planning Accuracy", results.planningAccuracy);
        printResult("Planning Confidence", results.planningConfidence);
        
        if (results.planningAccuracy > 0.4f) {
            std::cout << "  [SUCCESS] Planning ability demonstrated!\n";
        }
    }
    
    // Run Self-Model Experiment
    printSection("7. Self-Model");
    {
        SelfModelExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), numTrials);
        
        printResult("Self-Prediction Accuracy", results.selfPredictionAccuracy);
        printResult("Body Awareness", results.bodyAwareness);
        
        if (results.selfPredictionAccuracy > 0.3f) {
            std::cout << "  [SUCCESS] Self-model developing!\n";
        }
    }
    
    // Run Social Learning Experiment
    printSection("8. Social Learning");
    {
        SocialLearningExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), numTrials / 2);
        
        printResult("Imitation Accuracy", results.imitationAccuracy);
        printResult("Observations from Others", results.observationsFromOthers);
        
        if (results.imitationAccuracy > 0.2f) {
            std::cout << "  [SUCCESS] Social learning observed!\n";
        }
    }
    
    // Run Continual Learning Experiment
    printSection("9. Continual Learning");
    {
        ContinualLearningExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), numTrials);
        
        printResult("Behavior Improvement", results.behaviorImprovement);
        printResult("Transfer Performance", results.transferPerformance);
    }
    
        // Run all experiments with comprehensive error handling
    std::cout << "\n=== RUNNING ALL EXPERIMENTS ===\n";
    
    std::vector<std::pair<std::string, std::function<void()>>> experiments = {
        {"Temporal Prediction", [&]() {
            TemporalPredictionExperiment experiment;
            auto results = experiment.run(brain.get(), numTrials);
            printExperimentResults("1. Temporal Prediction Learning", results);
        }},
        {"Working Memory", [&]() {
            WorkingMemoryExperiment experiment;
            auto results = experiment.run(brain.get(), numTrials);
            printExperimentResults("2. Working Memory", results);
        }},
        {"Episodic Memory", [&]() {
            EpisodicRecallExperiment experiment;
            auto results = experiment.run(brain.get(), numTrials);
            printExperimentResults("3. Episodic Memory", results);
        }},
        {"Concept Formation", [&]() {
            ConceptFormationExperiment experiment;
            auto results = experiment.run(brain.get(), numTrials);
            printExperimentResults("4. Concept Formation", results);
            if (results.conceptsFormed > 0) {
                std::cout << "  [SUCCESS] Concepts emerged from repeated experience!\n";
            }
        }},
        {"Neural Attention", [&]() {
            AttentionExperiment experiment;
            auto results = experiment.run(brain.get(), numTrials);
            printExperimentResults("5. Neural Attention", results);
            if (results.attentionSelectivity > 0.5f) {
                std::cout << "  [SUCCESS] Selective attention emerged!\n";
            }
        }},
        {"Multi-Step Planning", [&]() {
            PlanningExperiment experiment;
            auto results = experiment.run(brain.get(), numTrials / 2);
            printExperimentResults("6. Multi-Step Planning", results);
            if (results.planningAccuracy > 0.4f) {
                std::cout << "  [SUCCESS] Planning ability demonstrated!\n";
            }
        }},
        {"Self-Model", [&]() {
            SelfModelExperiment experiment;
            auto results = experiment.run(brain.get(), numTrials);
            printExperimentResults("7. Self-Model", results);
            if (results.selfPredictionAccuracy > 0.3f) {
                std::cout << "  [SUCCESS] Self-model developing!\n";
            }
        }},
        {"Social Learning", [&]() {
            SocialLearningExperiment experiment;
            auto results = experiment.run(brain.get(), numTrials / 2);
            printExperimentResults("8. Social Learning", results);
            if (results.imitationAccuracy > 0.2f) {
                std::cout << "  [SUCCESS] Social learning observed!\n";
            }
        }},
        {"Continual Learning", [&]() {
            ContinualLearningExperiment experiment;
            auto results = experiment.run(brain.get(), numTrials);
            printExperimentResults("9. Continual Learning", results);
        }},
        {"Integrated Phase 4", [&]() {
            Phase4IntegratedExperiment experiment;
            auto results = experiment.run(brain.get(), 10, 50);
            printExperimentResults("10. Integrated Phase 4", results);
        }}
    };
    
    // Run experiments with error handling
    for (size_t i = 0; i < experiments.size(); ++i) {
        try {
            std::cout << "\n" << experiments[i].first << " experiment (" << (i + 1) << "/" << experiments.size() << ")...";
            experiments[i].second();
            std::cout << " ✓ Complete" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << " ✗ Failed: " << e.what() << std::endl;
            // Continue with other experiments
        } catch (...) {
            std::cerr << " ✗ Failed: Unknown error" << std::endl;
            // Continue with other experiments
        }
    }
    
    return 0;
}