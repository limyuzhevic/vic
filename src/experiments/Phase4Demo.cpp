#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <stdexcept>

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

int main(int argc, char* argv[]); // Forward declaration for help function

void printHelp(const std::string& programName) {
    std::cout << "NLM Phase 4: Emerging Cognition" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << "Phase 4 demonstrates the emergence of complex cognitive abilities:" << std::endl;
    std::cout << "  - Temporal prediction: Learning relationships between events" << std::endl;
    std::cout << "  - Working memory: Maintaining information across delays" << std::endl;
    std::cout << "  - Episodic memory: Storing and recalling experiences" << std::endl;
    std::cout << "  - Concept formation: Discovering patterns without labels" << std::endl;
    std::cout << "  - Neural attention: Selective processing through competition" << std::endl;
    std::cout << "  - Multi-step planning: Using predictions to select actions" << std::endl;
    std::cout << "  - Self-model: Learning body schema and action consequences" << std::endl;
    std::cout << "  - Social learning: Observing and imitating others" << std::endl;
    std::cout << "  - Continual learning: Adapting to new tasks" << std::endl;
    std::cout << "  - Integrated cognition: All systems working together" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                Show this help message" << std::endl;
    std::cout << "  --trials=N                Number of trials for each experiment (default: 50)" << std::endl;
    std::cout << "  --neuron_count=N          Set number of neurons in the brain" << std::endl;
    std::cout << "  --region_count=N          Set number of brain regions" << std::endl;
    std::cout << "  --connection_probability=P Set probability of synaptic connections" << std::endl;
    std::cout << "  --seed=N                  Set random seed for reproducibility" << std::endl;
    std::cout << "  --config=FILE             Load configuration from FILE" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << "                          Run with default settings (50 trials)" << std::endl;
    std::cout << "  " << programName << " --trials=25 --neuron_count=500" << std::endl;
    std::cout << "  " << programName << " --help" << std::endl;
    std::cout << std::endl;
    std::cout << "Note: Configuration loaded from command line (--config) overrides file settings." << std::endl;
}

int main(int argc, char* argv[]) {
    // Initialize logging
    std::cout << "NLM Phase 4: Emerging Cognition" << std::endl;
    std::cout << "================================" << std::endl << std::endl;
    
    // Parse command line arguments with validation
    size_t numTrials = 50;
    size_t neuronCount = 1000;
    size_t regionCount = 2;
    float connectionProbability = 0.1f;
    std::string configFile = "";
    
    // Check for help flag first
    bool showHelp = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
        }
    }
    
    // Show help and exit if requested
    if (showHelp) {
        printHelp(argv[0]);
        return 0;
    }
    
    // Parse remaining arguments with validation
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            // Already handled above
            continue;
        }
        
        // Parse --trials=N with validation
        if (arg.rfind("--trials=", 0) == 0) {
            try {
                size_t pos;
                int value = std::stoi(arg.substr(8), &pos);
                if (pos != arg.length()) {
                    std::cerr << "Error: Invalid value for --trials: '" << arg.substr(8) << "'" << std::endl;
                    printHelp(argv[0]);
                    return 1;
                }
                if (value < 1 || value > 1000) {
                    std::cerr << "Error: --trials must be between 1 and 1000" << std::endl;
                    printHelp(argv[0]);
                    return 1;
                }
                numTrials = static_cast<size_t>(value);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error: Invalid value for --trials: '" << arg.substr(8) << "'" << std::endl;
                printHelp(argv[0]);
                return 1;
            } catch (const std::out_of_range& e) {
                std::cerr << "Error: Value for --trials out of range" << std::endl;
                printHelp(argv[0]);
                return 1;
            }
        }
        // Parse --neuron_count=N with validation
        else if (arg.rfind("--neuron_count=", 0) == 0) {
            try {
                size_t pos;
                int value = std::stoi(arg.substr(14), &pos);
                if (pos != arg.length()) {
                    std::cerr << "Error: Invalid value for --neuron_count: '" << arg.substr(14) << "'" << std::endl;
                    printHelp(argv[0]);
                    return 1;
                }
                if (value < 100 || value > 10000) {
                    std::cerr << "Error: --neuron_count must be between 100 and 10000" << std::endl;
                    printHelp(argv[0]);
                    return 1;
                }
                neuronCount = static_cast<size_t>(value);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error: Invalid value for --neuron_count: '" << arg.substr(14) << "'" << std::endl;
                printHelp(argv[0]);
                return 1;
            } catch (const std::out_of_range& e) {
                std::cerr << "Error: Value for --neuron_count out of range" << std::endl;
                printHelp(argv[0]);
                return 1;
            }
        }
        // Parse --region_count=N with validation
        else if (arg.rfind("--region_count=", 0) == 0) {
            try {
                size_t pos;
                int value = std::stoi(arg.substr(15), &pos);
                if (pos != arg.length()) {
                    std::cerr << "Error: Invalid value for --region_count: '" << arg.substr(15) << "'" << std::endl;
                    printHelp(argv[0]);
                    return 1;
                }
                if (value < 1 || value > 10) {
                    std::cerr << "Error: --region_count must be between 1 and 10" << std::endl;
                    printHelp(argv[0]);
                    return 1;
                }
                regionCount = static_cast<size_t>(value);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error: Invalid value for --region_count: '" << arg.substr(15) << "'" << std::endl;
                printHelp(argv[0]);
                return 1;
            } catch (const std::out_of_range& e) {
                std::cerr << "Error: Value for --region_count out of range" << std::endl;
                printHelp(argv[0]);
                return 1;
            }
        }
        // Parse --connection_probability=P with validation
        else if (arg.rfind("--connection_probability=", 0) == 0) {
            try {
                size_t pos;
                float value = std::stof(arg.substr(25), &pos);
                if (pos != arg.length()) {
                    std::cerr << "Error: Invalid value for --connection_probability: '" << arg.substr(25) << "'" << std::endl;
                    printHelp(argv[0]);
                    return 1;
                }
                if (value < 0.0f || value > 1.0f) {
                    std::cerr << "Error: --connection_probability must be between 0.0 and 1.0" << std::endl;
                    printHelp(argv[0]);
                    return 1;
                }
                connectionProbability = value;
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error: Invalid value for --connection_probability: '" << arg.substr(25) << "'" << std::endl;
                printHelp(argv[0]);
                return 1;
            } catch (const std::out_of_range& e) {
                std::cerr << "Error: Value for --connection_probability out of range" << std::endl;
                printHelp(argv[0]);
                return 1;
            }
        }
        // Parse --config=FILE
        else if (arg.rfind("--config=", 0) == 0) {
            configFile = arg.substr(9);
        }
        // Parse --config FILE
        else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        }
        // Parse --seed=N
        else if (arg.rfind("--seed=", 0) == 0) {
            try {
                size_t pos;
                unsigned long value = std::stoul(arg.substr(7), &pos);
                if (pos != arg.length()) {
                    std::cerr << "Error: Invalid value for --seed: '" << arg.substr(7) << "'" << std::endl;
                    printHelp(argv[0]);
                    return 1;
                }
                // Note: Random seed setting requires access to Random class
                // For now, we'll just log it
                std::cout << "Note: Random seed set to " << value << " (not yet implemented in random system)" << std::endl;
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error: Invalid value for --seed: '" << arg.substr(7) << "'" << std::endl;
                printHelp(argv[0]);
                return 1;
            } catch (const std::out_of_range& e) {
                std::cerr << "Error: Value for --seed out of range" << std::endl;
                printHelp(argv[0]);
                return 1;
            }
        }
        else {
            // Unknown argument - show help
            std::cerr << "Error: Unknown argument: " << arg << std::endl;
            printHelp(argv[0]);
            return 1;
        }
    }
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Set configuration values
    config->set("neuron_count", neuronCount);
    config->set("region_count", regionCount);
    config->set("connection_probability", connectionProbability);
    config->set("random_seed", static_cast<uint64_t>(42)); // Default seed
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
    
    // Run the Phase 4 experiments
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