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

#include <iostream>
#include <iomanip>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <cmath>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Logger/DebugLogger.hpp"
#include "core/Logger/FileLogger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"

// Forward declarations for helper functions
const char* resultToString(float value, float threshold);
void printSeparator(int length = 60);

// Custom logging wrapper for experiments
namespace experiment_utils {
    
    void logExperimentStart(const std::string& name) {
        std::cout << std::endl;
        printSeparator(60);
        std::cout << "  " << name << std::endl;
        printSeparator(60);
        std::cout << std::endl;
    }
    
    void logExperimentResult(const std::string& name, const Phase4Results& results) {
        std::cout << "  " << name << ": " << std::endl;
        std::cout << "    Initial Prediction Error:  " << std::setprecision(6) << results.predictionErrorInitial << std::endl;
        std::cout << "    Final Prediction Error:    " << std::setprecision(6) << results.predictionErrorFinal << std::endl;
        std::cout << "    Improvement:               " << std::setprecision(6) << results.predictionAccuracyImprovement << std::endl;
        
        float improvement = results.predictionErrorInitial - results.predictionErrorFinal;
        if (improvement > 0.01f) {
            std::cout << "    [SUCCESS] Learning achieved!" << std::endl;
        } else if (improvement < -0.01f) {
            std::cout << "    [WARNING] Prediction accuracy declined!" << std::endl;
        } else {
            std::cout << "    [INFO] No significant change." << std::endl;
        }
    }
    
    bool validateResults(const Phase4Results& results) {
        // Basic sanity checks
        if (results.predictionErrorInitial < 0 || results.predictionErrorFinal < 0) {
            std::cerr << "ERROR: Invalid prediction error values!" << std::endl;
            return false;
        }
        
        if (results.workingMemoryRetentionInitial > 100 || results.workingMemoryRetentionFinal > 100) {
            std::cerr << "ERROR: Invalid working memory retention values!" << std::endl;
            return false;
        }
        
        if (results.episodesStored < 0) {
            std::cerr << "ERROR: Invalid episodes stored value!" << std::endl;
            return false;
        }
        
        return true;
    }
} // namespace experiment_utils

// Helper function implementations
const char* resultToString(float value, float threshold) {
    if (value > threshold * 1.1f) {
        return "EXCELLENT";
    } else if (value > threshold * 0.5f) {
        return "GOOD";
    } else if (value > threshold * 0.1f) {
        return "ACCEPTABLE";
    } else {
        return "NEEDS IMPROVEMENT";
    }
}

void printSeparator(int length) {
    std::cout << std::string(length, '=') << std::endl;
}

// Improved Phase 4 demo with better error handling and user experience
int main(int argc, char* argv[]) {
    std::cout << "NLM Phase 4: Emerging Cognition" << std::endl;
    printSeparator(40);
    std::cout << std::endl;
    
    // Parse command line arguments with validation
    size_t numTrials = 50;
    bool verbose = false;
    bool debug = false;
    bool profile = false;
    bool quiet = false;
    
    // Store original argv for error reporting
    std::vector<std::string> args(argv, argv + argc);
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = args[i];
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: ./nlm_phase4_demo [TRIALS] [OPTIONS]" << std::endl;
            std::cout << std::endl;
            std::cout << "Arguments:" << std::endl;
            std::cout << "  [TRIALS]              Number of trials (default: 50)" << std::endl;
            std::cout << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --help, -h                Show this help message" << std::endl;
            std::cout << "  --verbose, -v             Enable verbose output" << std::endl;
            std::cout << "  --debug, -d               Enable debug logging (implies verbose)" << std::endl;
            std::cout << "  --profile, -p             Enable performance profiling (implies verbose)" << std::endl;
            std::cout << "  --quiet                   Suppress non-error output" << std::endl;
            std::cout << std::endl;
            std::cout << "Examples:" << std::endl;
            std::cout << "  ./nlm_phase4_demo 100 --debug" << std::endl;
            std::cout << "  ./nlm_phase4_demo 25 --profile" << std::endl;
            return 0;
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--debug" || arg == "-d") {
            debug = true;
            verbose = true;
        } else if (arg == "--profile" || arg == "-p") {
            profile = true;
            verbose = true;
        } else if (arg == "--quiet") {
            quiet = true;
        } else if (i == 1 && arg.find_first_not_of("0123456789") == std::string::npos) {
            // First argument that looks like a number could be trial count
            try {
                size_t trials = std::stoull(arg);
                if (trials > 0 && trials <= 1000) {
                    numTrials = trials;
                } else {
                    std::cerr << "Warning: Trial count out of recommended range (1-1000). Using default: " << numTrials << std::endl;
                }
            } catch (const std::exception& e) {
                // Not a number, continue
            }
        }
    }
    
    // Initialize logging with error handling
    auto& logger = Logger::getInstance();
    
    if (debug) {
        logger.setLevel(Logger::Level::Debug);
        auto debugLogger = std::make_shared<DebugLogger>(Logger::Level::Debug);
        logger.addLogger(debugLogger);
        std::cout << "Debug logging enabled." << std::endl;
    } else if (verbose) {
        logger.setLevel(Logger::Level::Info);
        auto consoleLogger = std::make_shared<ConsoleLogger>(Logger::Level::Info);
        logger.addLogger(consoleLogger);
    } else if (quiet) {
        logger.setLevel(Logger::Level::Error);
        auto consoleLogger = std::make_shared<ConsoleLogger>(Logger::Level::Error);
        logger.addLogger(consoleLogger);
    } else {
        logger.setLevel(Logger::Level::Warning);
        auto consoleLogger = std::make_shared<ConsoleLogger>(Logger::Level::Warning);
        logger.addLogger(consoleLogger);
    }
    
    // Log start
    auto startTime = std::chrono::high_resolution_clock::now();
    std::cout << "Starting Phase 4 experiment with " << numTrials << " trials..." << std::endl;
    
    if (profile) {
        std::cout << "Performance profiling enabled." << std::endl;
    }
    
    // Create configuration with error checking
    auto config = std::make_shared<Config>();
    try {
        config->set("neuron_count", size_t(1000));
        config->set("region_count", size_t(2));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", uint64_t(42));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Failed to configure experiment: " << e.what() << std::endl;
        return 1;
    }
    
    // Create brain with error handling
    std::cout << "Initializing brain..." << std::endl;
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    printSeparator(60);
    std::cout << "  PHASE 4 EXPERIMENTS" << std::endl;
    printSeparator(60);
    std::cout << std::endl;
    
    try {
        // Run Temporal Prediction Experiment
        experiment_utils::logExperimentStart("1. Temporal Prediction Learning");
        {
            TemporalPredictionExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials);
            
            if (experiment_utils::validateResults(results)) {
                experiment_utils::logExperimentResult("Temporal Prediction", results);
                
                float improvement = results.predictionErrorInitial - results.predictionErrorFinal;
                if (improvement > 0.01f) {
                    std::cout << "  [SUCCESS] Prediction improved through experience!" << std::endl;
                } else if (improvement < -0.01f) {
                    std::cout << "  [INFO] No significant prediction improvement yet." << std::endl;
                }
                std::cout << std::endl;
            }
        }
        
        // Run Working Memory Experiment
        experiment_utils::logExperimentStart("2. Working Memory");
        {
            WorkingMemoryExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials);
            
            if (experiment_utils::validateResults(results)) {
                experiment_utils::logExperimentResult("Working Memory", results);
                std::cout << "  Capacity: " << results.workingMemoryCapacity << "/100" << std::endl;
                std::cout << "  Retention Initial: " << std::setprecision(6) << results.workingMemoryRetentionInitial << "%" << std::endl;
                std::cout << "  Retention Final:   " << std::setprecision(6) << results.workingMemoryRetentionFinal << "%" << std::endl;
                std::cout << std::endl;
            }
        }
        
        // Run Episodic Recall Experiment
        experiment_utils::logExperimentStart("3. Episodic Memory");
        {
            EpisodicRecallExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials);
            
            if (experiment_utils::validateResults(results)) {
                experiment_utils::logExperimentResult("Episodic Memory", results);
                std::cout << "  Recall Accuracy:  " << std::setprecision(6) << results.episodicRecallAccuracy << " (" << resultToString(results.episodicRecallAccuracy, 0.5f) << ")" << std::endl;
                std::cout << "  Episodes Stored:   " << results.episodesStored << std::endl;
                std::cout << "  Episodic Influence: " << std::setprecision(6) << results.episodicInfluence << std::endl;
                std::cout << std::endl;
            }
        }
        
        // Run Concept Formation Experiment
        experiment_utils::logExperimentStart("4. Concept Formation");
        {
            ConceptFormationExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials);
            
            if (experiment_utils::validateResults(results)) {
                experiment_utils::logExperimentResult("Concept Formation", results);
                std::cout << "  Concepts Formed:   " << results.conceptsFormed << std::endl;
                std::cout << "  Concept Stability: " << std::setprecision(6) << results.conceptStability << " (" << resultToString(results.conceptStability, 0.3f) << ")" << std::endl;
                std::cout << "  Generalization:   " << std::setprecision(6) << results.generalizationAbility << " (" << resultToString(results.generalizationAbility, 0.4f) << ")" << std::endl;
                
                if (results.conceptsFormed > 0) {
                    std::cout << "  [SUCCESS] Concepts emerged from repeated experience!" << std::endl;
                }
                std::cout << std::endl;
            }
        }
        
        // Run Attention Experiment
        experiment_utils::logExperimentStart("5. Neural Attention");
        {
            AttentionExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials);
            
            if (experiment_utils::validateResults(results)) {
                experiment_utils::logExperimentResult("Neural Attention", results);
                std::cout << "  Attention Selectivity:  " << std::setprecision(6) << results.attentionSelectivity << " (" << resultToString(results.attentionSelectivity, 0.5f) << ")" << std::endl;
                std::cout << "  Distraction Resistance: " << std::setprecision(6) << results.distractionResistance << " (" << resultToString(results.distractionResistance, 0.3f) << ")" << std::endl;
                
                if (results.attentionSelectivity > 0.5f) {
                    std::cout << "  [SUCCESS] Selective attention emerged!" << std::endl;
                }
                std::cout << std::endl;
            }
        }
        
        // Run Planning Experiment
        experiment_utils::logExperimentStart("6. Multi-Step Planning");
        {
            PlanningExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials / 2);
            
            if (experiment_utils::validateResults(results)) {
                experiment_utils::logExperimentResult("Multi-Step Planning", results);
                std::cout << "  Planning Accuracy:  " << std::setprecision(6) << results.planningAccuracy << " (" << resultToString(results.planningAccuracy, 0.4f) << ")" << std::endl;
                std::cout << "  Planning Confidence: " << std::setprecision(6) << results.planningConfidence << " (" << resultToString(results.planningConfidence, 0.3f) << ")" << std::endl;
                
                if (results.planningAccuracy > 0.4f) {
                    std::cout << "  [SUCCESS] Planning ability demonstrated!" << std::endl;
                }
                std::cout << std::endl;
            }
        }
        
        // Run Self-Model Experiment
        experiment_utils::logExperimentStart("7. Self-Model");
        {
            SelfModelExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials);
            
            if (experiment_utils::validateResults(results)) {
                experiment_utils::logExperimentResult("Self-Model", results);
                std::cout << "  Self-Prediction Accuracy: " << std::setprecision(6) << results.selfPredictionAccuracy << " (" << resultToString(results.selfPredictionAccuracy, 0.3f) << ")" << std::endl;
                std::cout << "  Body Awareness:         " << std::setprecision(6) << results.bodyAwareness << " (" << resultToString(results.bodyAwareness, 0.3f) << ")" << std::endl;
                
                if (results.selfPredictionAccuracy > 0.3f) {
                    std::cout << "  [SUCCESS] Self-model developing!" << std::endl;
                }
                std::cout << std::endl;
            }
        }
        
        // Run Social Learning Experiment
        experiment_utils::logExperimentStart("8. Social Learning");
        {
            SocialLearningExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials / 2);
            
            if (experiment_utils::validateResults(results)) {
                experiment_utils::logExperimentResult("Social Learning", results);
                std::cout << "  Imitation Accuracy:      " << std::setprecision(6) << results.imitationAccuracy << " (" << resultToString(results.imitationAccuracy, 0.2f) << ")" << std::endl;
                std::cout << "  Observations from Others: " << results.observationsFromOthers << std::endl;
                
                if (results.imitationAccuracy > 0.2f) {
                    std::cout << "  [SUCCESS] Social learning observed!" << std::endl;
                }
                std::cout << std::endl;
            }
        }
        
        // Run Continual Learning Experiment
        experiment_utils::logExperimentStart("9. Continual Learning");
        {
            ContinualLearningExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials);
            
            if (experiment_utils::validateResults(results)) {
                experiment_utils::logExperimentResult("Continual Learning", results);
                std::cout << "  Behavior Improvement:   " << std::setprecision(6) << results.behaviorImprovement << " (" << resultToString(results.behaviorImprovement, 0.3f) << ")" << std::endl;
                std::cout << "  Transfer Performance:   " << std::setprecision(6) << results.transferPerformance << " (" << resultToString(results.transferPerformance, 0.4f) << ")" << std::endl;
                std::cout << std::endl;
            }
        }
        
        // Run Integrated Experiment
        experiment_utils::logExperimentStart("10. Integrated Phase 4");
        {
            Phase4IntegratedExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), 10, 50);
            
            if (experiment_utils::validateResults(results)) {
                experiment_utils::logExperimentResult("Integrated Phase 4", results);
                std::cout << "  Total Reward:           " << std::setprecision(6) << results.totalReward << std::endl;
                std::cout << "  Episodes Stored:        " << results.episodesStored << std::endl;
                std::cout << "  Concepts Formed:        " << results.conceptsFormed << std::endl;
                std::cout << "  Planning Confidence:    " << std::setprecision(6) << results.planningConfidence << " (" << resultToString(results.planningConfidence, 0.3f) << ")" << std::endl;
                std::cout << "  Body Awareness:        " << std::setprecision(6) << results.bodyAwareness << " (" << resultToString(results.bodyAwareness, 0.3f) << ")" << std::endl;
                std::cout << std::endl;
            }
        }
        
        // Performance timing
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        printSeparator(60);
        std::cout << "  PHASE 4 SUMMARY" << std::endl;
        printSeparator(60);
        
        std::cout << std::endl;
        std::cout << "Phase 4 demonstrates:" << std::endl;
        std::cout << "  - Temporal prediction: Learning relationships between events" << std::endl;
        std::cout << "  - Working memory: Maintaining information across delays" << std::endl;
        std::cout << "  - Episodic memory: Storing and recalling experiences" << std::endl;
        std::cout << "  - Concept formation: Discovering patterns without labels" << std::endl;
        std::cout << "  - Neural attention: Selective processing through competition" << std::endl;
        std::cout << "  - Multi-step planning: Using predictions to select actions" << std::endl;
        std::cout << "  - Self-model: Learning body schema and action consequences" << std::endl;
        std::cout << "  - Social learning: Observing and imitating others" << std::endl;
        std::cout << "  - Continual learning: Adapting to new tasks" << std::endl;
        
        std::cout << std::endl;
        std::cout << "All mechanisms emerge from neural dynamics and plasticity." << std::endl;
        std::cout << "No predefined concepts, rules, or symbolic AI were used." << std::endl;
        
        // Performance metrics
        if (profile || verbose) {
            std::cout << std::endl;
            std::cout << "=== PERFORMANCE METRICS ===" << std::endl;
            std::cout << "Total execution time: " << std::fixed << std::setprecision(3) << duration.count() / 1000.0 << "s" << std::endl;
            std::cout << "Trials completed: " << numTrials << std::endl;
            std::cout << "Time per trial: " << std::setprecision(3) << duration.count() / 1000.0 / numTrials << "s" << std::endl;
            std::cout << "Experiments per hour: " << (60.0 / (duration.count() / 1000.0 / numTrials)) << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << "Phase 4 Complete!" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << std::endl;
        std::cerr << "ERROR: Experiment failed with exception:" << std::endl;
        std::cerr << "  " << e.what() << std::endl;
        
        if (debug) {
            std::cerr << "  (Debug mode enabled - check logs for details)" << std::endl;
        }
        
        return 1;
    } catch (...) {
        std::cerr << std::endl;
        std::cerr << "ERROR: Experiment failed with unknown exception" << std::endl;
        
        if (debug) {
            std::cerr << "  (Debug mode enabled - check logs for details)" << std::endl;
        }
        
        return 1;
    }
}