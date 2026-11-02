#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <set>
#include <utility>
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>
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

// Command line argument parsing for Phase 4 demo
struct Phase4Args {
    size_t numTrials = 50;
    bool help = false;
    bool version = false;
    std::string configFile = "configs/default.cfg";
    std::string outputFile;
    bool validate = false;
    bool verbose = false;
    bool quiet = false;
};

// Print help for Phase 4 demo
void printHelp() {
    std::cout << "NLM Phase 4: Emerging Cognition\n";
    std::cout << "================================\n\n";
    std::cout << "USAGE:\n";
    std::cout << "  nlm_phase4_demo [OPTIONS]\n\n";
    std::cout << "OPTIONS:\n";
    std::cout << "  --help                      Show this help message and exit\n";
    std::cout << "  --version                   Show version information and exit\n";
    std::cout << "  --config <file>             Load configuration from file\n";
    std::cout << "  --output <file>             Save results to output file\n";
    std::cout << "  --validate                  Validate configuration before running\n";
    std::cout << "  --verbose, -v               Enable verbose output\n";
    std::cout << "  --quiet, -q                 Suppress non-essential output\n";
    std::cout << "  --trials <n>                Number of trials (default: 50)\n\n";
    std::cout << "EXAMPLES:\n";
    std::cout << "  nlm_phase4_demo --help                        Show help\n";
    std::cout << "  nlm_phase4_demo --config my_config.json        Load JSON config\n";
    std::cout << "  nlm_phase4_demo --trials 100                  Run with 100 trials\n";
    std::cout << "  nlm_phase4_demo --validate --output results.json Validate and save results\n\n";
    std::cout << "Phase 4 demonstrates emerging cognition through neural mechanisms:\n";
    std::cout << "  - Temporal prediction: Learning relationships between events\n";
    std::cout << "  - Working memory: Maintaining information across delays\n";
    std::cout << "  - Episodic memory: Storing and recalling experiences\n";
    std::cout << "  - Concept formation: Discovering patterns without labels\n";
    std::cout << "  - Neural attention: Selective processing through competition\n";
    std::cout << "  - Multi-step planning: Using predictions to select actions\n";
    std::cout << "  - Self-model: Learning body schema and action consequences\n";
    std::cout << "  - Social learning: Observing and imitating others\n";
    std::cout << "  - Continual learning: Adapting to new tasks\n";
}

// Print version information for Phase 4 demo
void printVersion() {
    std::cout << "NLM Phase 4: Emerging Cognition\n";
    std::cout << "================================\n\n";
    std::cout << "Version: 0.2.0 (Phase 4: Emerging Cognition)\n";
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << "\n\n";
    std::cout << "Phase 4 demonstrates emerging cognition through neural mechanisms:\n";
    std::cout << "  - Temporal prediction: Learning relationships between events\n";
    std::cout << "  - Working memory: Maintaining information across delays\n";
    std::cout << "  - Episodic memory: Storing and recalling experiences\n";
    std::cout << "  - Concept formation: Discovering patterns without labels\n";
    std::cout << "  - Neural attention: Selective processing through competition\n";
    std::cout << "  - Multi-step planning: Using predictions to select actions\n";
    std::cout << "  - Self-model: Learning body schema and action consequences\n";
    std::cout << "  - Social learning: Observing and imitating others\n";
    std::cout << "  - Continual learning: Adapting to new tasks\n\n";
    std::cout << "All mechanisms emerge from neural dynamics and plasticity.\n";
    std::cout << "No predefined concepts, rules, or symbolic AI were used.\n";
}

// Parse command line arguments for Phase 4 demo
Phase4Args parseArguments(int argc, char* argv[]) {
    Phase4Args args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            args.help = true;
        } else if (arg == "--version") {
            args.version = true;
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            args.quiet = true;
        } else if (arg == "--validate") {
            args.validate = true;
        } else if (arg.find("--config=") == 0) {
            args.configFile = arg.substr(9);
        } else if (arg == "--config" && i + 1 < argc) {
            args.configFile = argv[++i];
        } else if (arg.find("--output=") == 0) {
            args.outputFile = arg.substr(9);
        } else if (arg == "--output" && i + 1 < argc) {
            args.outputFile = argv[++i];
        } else if (arg.find("--trials=") == 0) {
            args.numTrials = std::stoul(arg.substr(9));
        } else if (arg == "--trials" && i + 1 < argc) {
            args.numTrials = std::stoul(argv[++i]);
        } else if (arg[0] == '-') {
            // Unknown option
            continue;
        } else {
            // Unknown positional argument
            continue;
        }
    }
    
    return args;
}

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
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Parse command line arguments
    Phase4Args args = parseArguments(argc, argv);
    
    // Handle special cases
    if (args.help) {
        printHelp();
        return 0;
    }
    
    if (args.version) {
        printVersion();
        return 0;
    }
    
    std::cout << "NLM Phase 4: Emerging Cognition\n";
    std::cout << "================================\n\n";
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(1000));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    
    // Load configuration from file
    if (config->loadFromFile(args.configFile)) {
        std::cout << "Loaded configuration from: " << args.configFile << "\n";
    } else {
        std::cout << "Using default configuration.\n";
    }
    
    // Validate configuration if requested
    if (args.validate) {
        if (!config->validate()) {
            std::cerr << "Configuration validation failed!\n";
            return 1;
        } else {
            std::cout << "Configuration validation passed.\n";
        }
    }
    
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
        Phase4Results results = experiment.run(brain.get(), args.numTrials);
        
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
        Phase4Results results = experiment.run(brain.get(), args.numTrials);
        
        printResult("Initial Retention", results.workingMemoryRetentionInitial);
        printResult("Final Retention", results.workingMemoryRetentionFinal);
        printResult("Capacity", results.workingMemoryCapacity, 100.0f);
    }
    
    // Run Episodic Recall Experiment
    printSection("3. Episodic Memory");
    {
        EpisodicRecallExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), args.numTrials);
        
        printResult("Recall Accuracy", results.episodicRecallAccuracy);
        printResult("Episodes Stored", results.episodesStored);
        printResult("Episodic Influence", results.episodicInfluence);
    }
    
    // Run Concept Formation Experiment
    printSection("4. Concept Formation");
    {
        ConceptFormationExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), args.numTrials);
        
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
        Phase4Results results = experiment.run(brain.get(), args.numTrials);
        
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
        Phase4Results results = experiment.run(brain.get(), args.numTrials / 2);
        
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
        Phase4Results results = experiment.run(brain.get(), args.numTrials);
        
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
        Phase4Results results = experiment.run(brain.get(), args.numTrials / 2);
        
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
        Phase4Results results = experiment.run(brain.get(), args.numTrials);
        
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
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "\nPerformance:\n";
    std::cout << "  Total runtime: " << duration.count() << " ms\n";
    std::cout << "  Trials: " << args.numTrials << "\n";
    
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "Phase 4 Complete!\n";
    std::cout << std::string(60, '=') << "\n";
    
    return 0;
}