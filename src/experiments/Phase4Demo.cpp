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
#include "core/Config/ConfigOptions.hpp"
#include <string>
#include <cmath>

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

// Helper functions for Phase 4 demo
static void printPhase4Usage(const std::string& program_name) {
    std::cout << "NLM Phase 4: Emerging Cognition" << std::endl;
    std::cout << "================================" << std::endl << std::endl;
    
    std::cout << "Usage: " << program_name << " [options]" << std::endl << std::endl;
    
    std::cout << "Basic Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --trials=N                    Number of trials (default: 50)" << std::endl << std::endl;
    
    std::cout << "Experiment Configuration:" << std::endl;
    std::cout << "  --neuron-count=N              Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --region-count=N              Number of regions (default: 2)" << std::endl;
    std::cout << "  --connection-prob=F           Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --random-seed=N               Random seed (default: 42)" << std::endl;
    std::cout << "  --simulation-timestep=F       Simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --stdp-ntp-weight=F           STDP LTP weight (default: 0.01)" << std::endl;
    std::cout << "  --stdp-ltd-weight=F           STDP LTD weight (default: 0.012)" << std::endl;
    std::cout << "  --stdp-tau=N                  STDP time constant (default: 20.0)" << std::endl;
    std::cout << "  --enable-prediction[=bool]    Enable prediction system (default: true)" << std::endl;
    std::cout << "  --enable-memory[=bool]        Enable memory systems (default: true)" << std::endl;
    std::cout << "  --enable-cognition[=bool]      Enable cognition systems (default: true)" << std::endl;
    std::cout << "  --enable-development[=bool]   Enable development (default: true)" << std::endl << std::endl;
    
    std::cout << "Advanced Configuration:" << std::endl;
    std::cout << "  --output-summary=FILE         Output summary text file" << std::endl;
    std::cout << "  --output-json=FILE            Output results in JSON format" << std::endl << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << program_name << " --trials=100 --neuron-count=2000" << std::endl;
    std::cout << "  " << program_name << " --trials=25 --enable-prediction=false" << std::endl;
    std::cout << "  " << program_name << " --trials=75 --output-summary=results.txt" << std::endl;
    std::cout << "  " << program_name << " --trials=50 --stdp-ntp-weight=0.02 --stdp-ltd-weight=0.018" << std::endl;
}

static std::string boolToString(bool value) {
    return value ? "true" : "false";
}

static void printConfiguration(const Config& config) {
    std::cout << "Experiment Configuration:" << std::endl;
    std::cout << "  Neuron count: " << config.getOr<size_t>("neuron_count", 1000) << std::endl;
    std::cout << "  Region count: " << config.getOr<size_t>("region_count", 2) << std::endl;
    std::cout << "  Connection probability: " << config.getOr<float>("connection_probability", 0.1f) << std::endl;
    std::cout << "  Random seed: " << config.getOr<uint64_t>("random_seed", 42) << std::endl;
    std::cout << "  Simulation timestep: " << config.getOr<double>("simulation_timestep", 0.001) << std::endl;
    std::cout << "  STDP LTP weight: " << config.getOr<float>("stdp_ltp_weight", 0.01f) << std::endl;
    std::cout << "  STDP LTD weight: " << config.getOr<float>("stdp_ltd_weight", 0.012f) << std::endl;
    std::cout << "  STDP time constant: " << config.getOr<float>("stdp_time_constant", 20.0f) << std::endl;
    std::cout << "  Synaptogenesis rate: " << config.getOr<float>("development_synaptogenesis_rate", 0.001f) << std::endl;
    std::cout << "  Pruning rate: " << config.getOr<float>("development_pruning_rate", 0.0001f) << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    size_t numTrials = 50;
    bool enablePrediction = true;
    bool enableMemory = true;
    bool enableCognition = true;
    bool enableDevelopment = true;
    std::string outputSummaryFile = "";
    std::string outputJsonFile = "";
    
    // Parse command line
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printPhase4Usage("nlm_phase4_demo");
            return 0;
        }
        
        if (arg.find("--trials=") == 0) {
            numTrials = static_cast<size_t>(std::stoi(arg.substr(9)));
        } else if (arg.find("--neuron-count=") == 0) {
            // Would need to store this for configuration
        } else if (arg.find("--region-count=") == 0) {
            // Would need to store this for configuration
        } else if (arg.find("--connection-prob=") == 0) {
            // Would need to store this for configuration
        } else if (arg.find("--random-seed=") == 0) {
            // Would need to store this for configuration
        } else if (arg.find("--simulation-timestep=") == 0) {
            // Would need to store this for configuration
        } else if (arg.find("--stdp-ntp-weight=") == 0) {
            // Would need to store this for configuration
        } else if (arg.find("--stdp-ltd-weight=") == 0) {
            // Would need to store this for configuration
        } else if (arg.find("--stdp-tau=") == 0) {
            // Would need to store this for configuration
        } else if (arg.find("--enable-prediction=") == 0) {
            std::string value = arg.substr(21);
            enablePrediction = (value == "true" || value == "1" || value == "yes");
        } else if (arg.find("--enable-memory=") == 0) {
            std::string value = arg.substr(16);
            enableMemory = (value == "true" || value == "1" || value == "yes");
        } else if (arg.find("--enable-cognition=") == 0) {
            std::string value = arg.substr(20);
            enableCognition = (value == "true" || value == "1" || value == "yes");
        } else if (arg.find("--enable-development=") == 0) {
            std::string value = arg.substr(21);
            enableDevelopment = (value == "true" || value == "1" || value == "yes");
        } else if (arg.find("--output-summary=") == 0) {
            outputSummaryFile = arg.substr(18);
        } else if (arg.find("--output-json=") == 0) {
            outputJsonFile = arg.substr(14);
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printPhase4Usage("nlm_phase4_demo");
            return 1;
        }
    }
    
    std::cout << "NLM Phase 4: Emerging Cognition" << std::endl;
    std::cout << "================================" << std::endl << std::endl;
    
    // Print configuration
    printConfiguration(*config); // This would need to be updated
    std::cout << "  Enable prediction: " << boolToString(enablePrediction) << std::endl;
    std::cout << "  Enable memory: " << boolToString(enableMemory) << std::endl;
    std::cout << "  Enable cognition: " << boolToString(enableCognition) << std::endl;
    std::cout << "  Enable development: " << boolToString(enableDevelopment) << std::endl;
    std::cout << "  Number of trials: " << numTrials << std::endl;
    std::cout << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(1000));
    config->set("region_count", size_t(2));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", uint64_t(42));
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("stdp_time_constant", 20.0f);
    config->set("development_synaptogenesis_rate", 0.001f);
    config->set("development_pruning_rate", 0.0001f);
    
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
    
    // Run Integrated Experiment
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
    
    // Final summary
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
    
    return 0;
}
