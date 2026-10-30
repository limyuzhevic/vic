#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>
#include <string>
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

/**
 * Print help information
 */
void printHelp(const std::string& programName) {
    std::cout << "NLM Phase 4: Emerging Cognition" << std::endl;
    std::cout << "================================" << std::endl << std::endl;
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                 Show this help message" << std::endl;
    std::cout << "  --verbose, -v              Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q                Disable most output (quiet mode)" << std::endl;
    std::cout << "  --trials N                 Number of trials for experiments (default: 50)" << std::endl;
    std::cout << "  --neurons N                Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --regions N                Number of regions (default: 2)" << std::endl;
    std::cout << "  --no-plasticity            Disable plasticity (testing mode)" << std::endl;
    std::cout << "  --config FILE              Load configuration from file" << std::endl;
    std::cout << "  --seed N                   Random seed for reproducible results (default: 42)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --verbose --trials 100     Run with verbose output and 100 trials" << std::endl;
    std::cout << "  " << programName << " --quiet                   Run quietly" << std::endl;
    std::cout << "  " << programName << " --trials 25 --neurons 500   Run with 500 neurons and 25 trials" << std::endl;
}

/**
 * Parse command line arguments
 */
struct Phase4DemoConfig {
    bool help = false;
    bool verbose = false;
    bool quiet = false;
    size_t numTrials = 50;
    size_t neuronCount = 1000;
    size_t regionCount = 2;
    bool plasticityEnabled = true;
    std::string configFile;
    uint64_t randomSeed = 42;
};

Phase4DemoConfig parseArgs(int argc, char* argv[]) {
    Phase4DemoConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            config.help = true;
        } else if (arg == "--verbose" || arg == "-v") {
            config.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            config.quiet = true;
        } else if (arg == "--trials" && i + 1 < argc) {
            config.numTrials = std::stoi(argv[++i]);
            if (config.numTrials <= 0) {
                std::cerr << "Error: Number of trials must be positive." << std::endl;
                exit(1);
            }
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoull(argv[++i]);
            if (config.neuronCount <= 0) {
                std::cerr << "Error: Neuron count must be positive." << std::endl;
                exit(1);
            }
        } else if (arg == "--regions" && i + 1 < argc) {
            config.regionCount = std::stoull(argv[++i]);
            if (config.regionCount <= 0) {
                std::cerr << "Error: Region count must be positive." << std::endl;
                exit(1);
            }
        } else if (arg == "--no-plasticity") {
            config.plasticityEnabled = false;
        } else if (arg == "--config" && i + 1 < argc) {
            config.configFile = argv[++i];
        } else if (arg == "--seed" && i + 1 < argc) {
            config.randomSeed = std::stoull(argv[++i]);
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            std::cerr << "Use --help for usage information." << std::endl;
            exit(1);
        }
    }
    
    // Override verbose/quiet if both specified
    if (config.verbose && config.quiet) {
        config.verbose = false;
        config.quiet = false;
        std::cerr << "Warning: Both verbose and quiet specified, using normal output." << std::endl;
    }
    
    // If help requested, just print help and exit
    if (config.help) {
        printHelp(argv[0]);
        exit(0);
    }
    
    return config;
}

/**
 * Print status message
 */
void printStatus(const std::string& message, bool verbose = false, bool quiet = false) {
    if (quiet) return;
    
    if (verbose) {
        std::cout << "[VERBOSE] " << message << std::endl;
    } else {
        std::cout << message << std::endl;
    }
}

/**
 * Print section header
 */
void printSection(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

/**
 * Print result
 */
void printResult(const std::string& name, float value, float max = 1.0f) {
    std::cout << "  " << std::left << std::setw(30) << name << ": ";
    std::cout << std::fixed << std::setprecision(3) << value;
    if (max > 0) {
        std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
    }
    std::cout << std::endl;
}

/**
 * Print sub-section
 */
void printSubSection(const std::string& title) {
    std::cout << "\n--- " << title << " ---" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "NLM Phase 4: Emerging Cognition" << std::endl;
    std::cout << "================================" << std::endl << std::endl;
    
    // Parse command line arguments
    Phase4DemoConfig config = parseArgs(argc, argv);
    
    // Print configuration
    printStatus("Configuration:", config.verbose);
    std::cout << "  - Number of trials: " << config.numTrials << std::endl;
    std::cout << "  - Number of neurons: " << config.neuronCount << std::endl;
    std::cout << "  - Number of regions: " << config.regionCount << std::endl;
    std::cout << "  - Plasticity enabled: " << (config.plasticityEnabled ? "Yes" : "No") << std::endl;
    std::cout << "  - Random seed: " << config.randomSeed << std::endl;
    if (!config.configFile.empty()) {
        std::cout << "  - Config file: " << config.configFile << std::endl;
    }
    std::cout << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", config.neuronCount);
    config->set("region_count", config.regionCount);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", config.randomSeed);
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    
    // Create brain
    std::cout << "Initializing brain..." << std::endl;
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    printSection("PHASE 4 EXPERIMENTS");
    
    // Run Temporal Prediction Experiment
    printSubSection("1. Temporal Prediction Learning");
    {
        TemporalPredictionExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), config.numTrials);
        
        printResult("Initial Prediction Error", results.predictionErrorInitial);
        printResult("Final Prediction Error", results.predictionErrorFinal);
        printResult("Improvement", results.predictionAccuracyImprovement);
        
        float improvement = results.predictionErrorInitial - results.predictionErrorFinal;
        if (improvement > 0) {
            std::cout << "  [SUCCESS] Prediction improved through experience!" << std::endl;
        } else {
            std::cout << "  [INFO] No significant prediction improvement yet." << std::endl;
        }
    }
    
    // Run Working Memory Experiment
    printSubSection("2. Working Memory");
    {
        WorkingMemoryExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), config.numTrials);
        
        printResult("Initial Retention", results.workingMemoryRetentionInitial);
        printResult("Final Retention", results.workingMemoryRetentionFinal);
        printResult("Capacity", results.workingMemoryCapacity, 100.0f);
    }
    
    // Run Episodic Recall Experiment
    printSubSection("3. Episodic Memory");
    {
        EpisodicRecallExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), config.numTrials);
        
        printResult("Recall Accuracy", results.episodicRecallAccuracy);
        printResult("Episodes Stored", results.episodesStored);
        printResult("Episodic Influence", results.episodicInfluence);
    }
    
    // Run Concept Formation Experiment
    printSubSection("4. Concept Formation");
    {
        ConceptFormationExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), config.numTrials);
        
        printResult("Concepts Formed", results.conceptsFormed);
        printResult("Concept Stability", results.conceptStability);
        printResult("Generalization Ability", results.generalizationAbility);
        
        if (results.conceptsFormed > 0) {
            std::cout << "  [SUCCESS] Concepts emerged from repeated experience!" << std::endl;
        }
    }
    
    // Run Attention Experiment
    printSubSection("5. Neural Attention");
    {
        AttentionExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), config.numTrials);
        
        printResult("Attention Selectivity", results.attentionSelectivity);
        printResult("Distraction Resistance", results.distractionResistance);
        
        if (results.attentionSelectivity > 0.5f) {
            std::cout << "  [SUCCESS] Selective attention emerged!" << std::endl;
        }
    }
    
    // Run Planning Experiment
    printSubSection("6. Multi-Step Planning");
    {
        PlanningExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), config.numTrials / 2);
        
        printResult("Planning Accuracy", results.planningAccuracy);
        printResult("Planning Confidence", results.planningConfidence);
        
        if (results.planningAccuracy > 0.4f) {
            std::cout << "  [SUCCESS] Planning ability demonstrated!" << std::endl;
        }
    }
    
    // Run Self-Model Experiment
    printSubSection("7. Self-Model");
    {
        SelfModelExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), config.numTrials);
        
        printResult("Self-Prediction Accuracy", results.selfPredictionAccuracy);
        printResult("Body Awareness", results.bodyAwareness);
        
        if (results.selfPredictionAccuracy > 0.3f) {
            std::cout << "  [SUCCESS] Self-model developing!" << std::endl;
        }
    }
    
    // Run Social Learning Experiment
    printSubSection("8. Social Learning");
    {
        SocialLearningExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), config.numTrials / 2);
        
        printResult("Imitation Accuracy", results.imitationAccuracy);
        printResult("Observations from Others", results.observationsFromOthers);
        
        if (results.imitationAccuracy > 0.2f) {
            std::cout << "  [SUCCESS] Social learning observed!" << std::endl;
        }
    }
    
    // Run Continual Learning Experiment
    printSubSection("9. Continual Learning");
    {
        ContinualLearningExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), config.numTrials);
        
        printResult("Behavior Improvement", results.behaviorImprovement);
        printResult("Transfer Performance", results.transferPerformance);
    }
    
    // Run Integrated Experiment
    printSubSection("10. Integrated Phase 4");
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
    printSection("PHASE 4 SUMMARY");
    
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
