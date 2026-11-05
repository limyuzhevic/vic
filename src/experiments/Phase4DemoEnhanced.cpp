// NLM Phase 4 Demo - Enhanced CLI
// Updated to use the new unified CLI system

#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/CLI.hpp"
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

// Phase 4 options structure
struct Phase4Options {
    size_t numTrials;
    bool verbose;
    bool quiet;
    std::string logLevel;
    bool help;
    bool version;
    size_t workingMemoryTrials;
    size_t episodicTrials;
    size_t planningTrials;
    size_t selfModelTrials;
    size_t socialLearningTrials;
    size_t continualLearningTrials;
    std::string outputFile;
    bool enablePlasticity;
    bool enableDevelopment;
    bool enableCuriosity;
    bool enableRewardModulation;
    double timestep;
    int neuronCount;
    int regionCount;
    double connectionProbability;
};

// Parse command line arguments for Phase 4
Phase4Options parsePhase4Options(int argc, char* argv[]) {
    Phase4Options options;
    
    // Set defaults
    options.numTrials = 50;
    options.verbose = false;
    options.quiet = false;
    options.logLevel = "info";
    options.help = false;
    options.version = false;
    options.workingMemoryTrials = 50;
    options.episodicTrials = 50;
    options.planningTrials = 25;
    options.selfModelTrials = 50;
    options.socialLearningTrials = 25;
    options.continualLearningTrials = 50;
    options.outputFile = "";
    options.enablePlasticity = true;
    options.enableDevelopment = true;
    options.enableCuriosity = true;
    options.enableRewardModulation = true;
    options.timestep = 0.001;
    options.neuronCount = 1000;
    options.regionCount = 2;
    options.connectionProbability = 0.1f;
    
    // Parse common options first
    auto commonOptions = parseCommonOptions(argc, argv, "nlm_phase4_demo");
    
    // Override with specific options (simple parsing for now)
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version") {
            options.version = true;
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
        } else if (arg.substr(0, 2) == "--") {
            std::string opt = arg.substr(2);
            size_t equalsPos = opt.find('=');
            if (equalsPos != std::string::npos) {
                std::string value = opt.substr(equalsPos + 1);
                opt = opt.substr(0, equalsPos);
                
                if (opt == "trials") options.numTrials = std::stoul(value);
                else if (opt == "working-memory-trials") options.workingMemoryTrials = std::stoul(value);
                else if (opt == "episodic-trials") options.episodicTrials = std::stoul(value);
                else if (opt == "planning-trials") options.planningTrials = std::stoul(value);
                else if (opt == "self-model-trials") options.selfModelTrials = std::stoul(value);
                else if (opt == "social-learning-trials") options.socialLearningTrials = std::stoul(value);
                else if (opt == "continual-learning-trials") options.continualLearningTrials = std::stoul(value);
                else if (opt == "timestep") options.timestep = std::stod(value);
                else if (opt == "neuron-count") options.neuronCount = std::stoi(value);
                else if (opt == "region-count") options.regionCount = std::stoi(value);
                else if (opt == "connection-prob") options.connectionProbability = std::stof(value);
            }
        }
    }
    
    // Set flags from common options
    options.enablePlasticity = commonOptions.enablePlasticity;
    options.enableDevelopment = commonOptions.enableDevelopment;
    options.enableCuriosity = commonOptions.enableCuriosity;
    options.enableRewardModulation = commonOptions.enableRewardModulation;
    
    return options;
}

void printPhase4Help() {
    std::cout << "NLM Phase 4 Demo - Enhanced CLI" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm_phase4_demo [OPTIONS] [TRIALS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Arguments:" << std::endl;
    std::cout << "  TRIALS            Number of trials per experiment (default: 50)" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h        Show this help message" << std::endl;
    std::cout << "  --version         Show version information" << std::endl;
    std::cout << "  --verbose, -v     Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q       Quiet mode (suppress info messages)" << std::endl;
    std::cout << "  --plasticity      Enable synaptic plasticity" << std::endl;
    std::cout << "  --no-plasticity   Disable synaptic plasticity" << std::endl;
    std::cout << "  --development     Enable developmental processes" << std::endl;
    std::cout << "  --no-development  Disable developmental processes" << std::endl;
    std::cout << "  --curiosity       Enable curiosity-driven exploration" << std::endl;
    std::cout << "  --no-curiosity    Disable curiosity-driven exploration" << std::endl;
    std::cout << "  --reward          Enable reward modulation" << std::endl;
    std::cout << "  --no-reward       Disable reward modulation" << std::endl;
    std::cout << "  --config FILE     Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --output FILE     Output file for results" << std::endl;
    std::cout << "  --timestep VAL    Simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --neuron-count N  Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --region-count R  Number of regions (default: 2)" << std::endl;
    std::cout << "  --connection-prob P Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --working-memory-trials N Working memory trials (default: 50)" << std::endl;
    std::cout << "  --episodic-trials N Episodic memory trials (default: 50)" << std::endl;
    std::cout << "  --planning-trials N Planning trials (default: 25)" << std::endl;
    std::cout << "  --self-model-trials N Self-model trials (default: 50)" << std::endl;
    std::cout << "  --social-learning-trials N Social learning trials (default: 25)" << std::endl;
    std::cout << "  --continual-learning-trials N Continual learning trials (default: 50)" << std::endl;
    std::cout << std::endl;
    std::cout << "The enhanced CLI provides better help, more configuration options,
 and improved user experience." << std::endl;
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
    // Parse options
    Phase4Options options = parsePhase4Options(argc, argv);
    
    if (options.help) {
        printPhase4Help();
        return 0;
    }
    
    if (options.version) {
        std::cout << "NLM Phase 4 Demo - Enhanced Version 0.1.0" << std::endl;
        return 0;
    }
    
    // Setup logging based on options
    setupCommonLogging(options.logLevel, options.verbose, options.quiet);
    
    std::cout << "NLM Phase 4: Emerging Cognition\n";
    std::cout << "================================\n\n";
    
    // Parse command line arguments for trials
    size_t numTrials = options.numTrials;
    if (argc > 1 && argv[1][0] != '-') {
        numTrials = std::stoul(argv[1]);
    }
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount));
    config->set("region_count", static_cast<int64_t>(options.regionCount));
    config->set("connection_probability", options.connectionProbability);
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", options.timestep);
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
        Phase4Results results = experiment.run(brain.get(), options.workingMemoryTrials);
        
        printResult("Initial Retention", results.workingMemoryRetentionInitial);
        printResult("Final Retention", results.workingMemoryRetentionFinal);
        printResult("Capacity", results.workingMemoryCapacity, 100.0f);
    }
    
    // Run Episodic Recall Experiment
    printSection("3. Episodic Memory");
    {
        EpisodicRecallExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), options.episodicTrials);
        
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
        Phase4Results results = experiment.run(brain.get(), options.planningTrials);
        
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
        Phase4Results results = experiment.run(brain.get(), options.selfModelTrials);
        
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
        Phase4Results results = experiment.run(brain.get(), options.socialLearningTrials);
        
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
        Phase4Results results = experiment.run(brain.get(), options.continualLearningTrials);
        
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

} // namespace nlm