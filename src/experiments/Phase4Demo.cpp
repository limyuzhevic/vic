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

void printHelp(const std::string& programName) {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                      NLM Phase 4 Demo                             ║
║                Emerging Cognition Demonstration                   ║
╠═══════════════════════════════════════════════════════════════════╣
║                                                                             ║
║ USAGE:                                                                      ║
║   " << programName << " [OPTIONS]                                        ║
║                                                                             ║
║ DESCRIPTION:                                                                ║
║   Phase 4 demo demonstrates emerging cognitive abilities in NLM:             ║
║   • Temporal prediction: Learning sequences of events                        ║
║   • Working memory: Maintaining information across delays                ║
║   • Episodic memory: Storing and recalling experiences                     ║
║   • Concept formation: Discovering patterns without labels                ║
║   • Neural attention: Selective processing through competition              ║
║   • Multi-step planning: Using predictions to select actions                ║
║   • Self-model: Learning body schema and action consequences               ║
║   • Social learning: Observing and imitating others                       ║
║   • Continual learning: Adapting to new tasks                             ║
║                                                                             ║
║ OPTIONS:                                                                    ║
║   --help, -h            Show this help message                               ║
║   --trials N            Number of trials per experiment (default: 50)         ║
║   --neurons N           Number of neurons (default: 1000)                   ║
║   --regions R           Number of brain regions (default: 2)                ║
║   --seed SEED           Random seed (default: 42)                            ║
║   --verbose, -v         Enable verbose logging                              ║
║   --quiet, -q           Enable quiet mode                                   ║
║                                                                             ║
║ EXAMPLES:                                                                   ║
║   " << programName << " --help                                      ║
║   " << programName << " --trials 100 --neurons 2000 --seed 123                ║
║   " << programName << " --verbose                                      ║
║                                                                             ║
╚═══════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;
}

struct Phase4DemoArgs {
    size_t trials = 50;
    size_t neurons = 1000;
    size_t regions = 2;
    uint64_t seed = 42;
    bool verbose = false;
    bool quiet = false;
};

Phase4DemoArgs parseCommandLine(int argc, char* argv[]) {
    Phase4DemoArgs args;
    
    if (argc < 1 || !argv) {
        return args;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp(argv[0]);
            exit(0);
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            args.quiet = true;
        } else if (arg.substr(0, 9) == "--trials") {
            if (arg.find('=') != std::string::npos) {
                args.trials = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                args.trials = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 9) == "--neurons") {
            if (arg.find('=') != std::string::npos) {
                args.neurons = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                args.neurons = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 8) == "--regions") {
            if (arg.find('=') != std::string::npos) {
                args.regions = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                args.regions = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 6) == "--seed") {
            if (arg.find('=') != std::string::npos) {
                args.seed = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                args.seed = std::stoull(argv[++i]);
            }
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            std::cerr << "Use --help for usage information.\n";
            exit(1);
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
    // Parse command line arguments
    Phase4DemoArgs args = parseCommandLine(argc, argv);
    
    if (args.quiet) {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Error);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    std::cout << "NLM Phase 4: Emerging Cognition\n";
    std::cout << "================================\n\n";
    
    // Set up logging based on verbosity
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(
        args.verbose ? LogLevel::Debug : LogLevel::Info
    );
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Log configuration
    std::cout << "=== Phase 4 Demo Configuration ===" << std::endl;
    std::cout << "Trials: " << args.trials << std::endl;
    std::cout << "Neurons: " << args.neurons << std::endl;
    std::cout << "Regions: " << args.regions << std::endl;
    std::cout << "Seed: " << args.seed << std::endl;
    std::cout << "Verbose: " << (args.verbose ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
    
    // Initialize random seed
    Random::initialize(args.seed);
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", args.neurons);
    config->set("region_count", args.regions);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", args.seed);
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
        Phase4Results results = experiment.run(brain.get(), args.trials);
        
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
        Phase4Results results = experiment.run(brain.get(), args.trials);
        
        printResult("Initial Retention", results.workingMemoryRetentionInitial);
        printResult("Final Retention", results.workingMemoryRetentionFinal);
        printResult("Capacity", results.workingMemoryCapacity, 100.0f);
    }
    
    // Run Episodic Recall Experiment
    printSection("3. Episodic Memory");
    {
        EpisodicRecallExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), args.trials);
        
        printResult("Recall Accuracy", results.episodicRecallAccuracy);
        printResult("Episodes Stored", results.episodesStored);
        printResult("Episodic Influence", results.episodicInfluence);
    }
    
    // Run Concept Formation Experiment
    printSection("4. Concept Formation");
    {
        ConceptFormationExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), args.trials);
        
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
        Phase4Results results = experiment.run(brain.get(), args.trials);
        
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
        Phase4Results results = experiment.run(brain.get(), args.trials / 2);
        
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
        Phase4Results results = experiment.run(brain.get(), args.trials);
        
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
        Phase4Results results = experiment.run(brain.get(), args.trials / 2);
        
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
        Phase4Results results = experiment.run(brain.get(), args.trials);
        
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