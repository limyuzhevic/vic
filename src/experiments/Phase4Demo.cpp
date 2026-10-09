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
#include "common/CommonUtils.hpp"

using namespace nlm;

void printHeader(const std::string& title) {
    OutputManager::printHeader(title);
}

void printResult(const std::string& name, float value, float max = 1.0f) {
    OutputManager::printResult(name, value, max);
}

void printSection(const std::string& title) {
    OutputManager::printSection(title);
}

ExecutionMode parseMode(const std::string& modeStr) {
    std::string lower = modeStr;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "test") return ExecutionMode::TEST;
    if (lower == "run") return ExecutionMode::RUN;
    if (lower == "benchmark") return ExecutionMode::BENCHMARK;
    if (lower == "help") return ExecutionMode::HELP;
    if (lower == "version") return ExecutionMode::VERSION;
    
    return ExecutionMode::UNKNOWN;
}

int main(int argc, char* argv[]) {
    OutputManager::printHeader("NLM Phase 4: Emerging Cognition");
    
    // Parse command line arguments using CommonUtils framework
    std::vector<std::string> allowedFlags = {"help", "version", "verbose", "quiet", "config", "output", "log-level", "mode", "trials"};
    std::map<std::string, std::string> flagDescriptions = {
        {"help", "Show help message"},
        {"version", "Show version information"},
        {"verbose", "Enable verbose output"},
        {"quiet", "Suppress non-essential output"},
        {"config", "Configuration file"},
        {"output", "Output file"},
        {"log-level", "Log level"},
        {"mode", "Execution mode"},
        {"trials", "Number of trials"}
    };
    
    CLIArgs args = CLIUtils::parseArgs(argc, argv, allowedFlags, flagDescriptions);
    
    // Handle help and version requests
    if (args.help) {
        std::cout << "Usage: nlm_phase4_demo [options]\n\n";
        std::cout << "NLM Phase 4: Emerging Cognition\n\n";
        std::cout << "Options:\n";
        std::cout << "  --help, -h           Show this help message\n";
        std::cout << "  --version, -v        Show version information\n";
        std::cout << "  --config <file>      Load configuration from file\n";
        std::cout << "  --output <file>      Write output to file\n";
        std::cout << "  --log-level <level>  Set log level (DEBUG, INFO, WARNING, ERROR)\n";
        std::cout << "  --mode <mode>        Execution mode (test, run, benchmark)\n";
        std::cout << "  --verbose, -V        Enable verbose output\n";
        std::cout << "  --quiet, -q          Suppress non-essential output\n";
        std::cout << "  --trials <n>         Number of trials (default: 50)\n\n";
        return 0;
    }
    
    if (args.version) {
        std::cout << "NLM Phase 4: Emerging Cognition\n";
        std::cout << "Version: 2.0.0\n";
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << "\n";
        return 0;
    }
    
    // Set output format
    OutputManager::setOutputFormat(OutputFormat::CONSOLE);
    if (!args.outputFile.empty()) {
        OutputManager::setOutputFile(args.outputFile);
    }
    
    // Initialize logging system
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Set log level from args
    if (!args.logLevel.empty()) {
        std::string level = args.logLevel;
        std::transform(level.begin(), level.end(), level.begin(), ::tolower);
        if (level == "debug") consoleLogger->setLevel(LogLevel::Debug);
        else if (level == "warning") consoleLogger->setLevel(LogLevel::Warning);
        else if (level == "error") consoleLogger->setLevel(LogLevel::Error);
        else consoleLogger->setLevel(LogLevel::Info); // Default
    }
    
    // Parse trials
    size_t numTrials = 50;
    if (args.customArgs.count("trials")) {
        numTrials = static_cast<size_t>(std::atoi(args.customArgs["trials"].c_str()));
    }
    
    // Determine execution mode
    ExecutionMode mode = ExecutionMode::RUN; // Default
    if (!args.mode.empty()) {
        mode = parseMode(args.mode);
        if (mode == ExecutionMode::UNKNOWN) {
            OutputManager::printWarning("Unknown mode '" + args.mode + "', using default");
        }
    }
    
    OutputManager::printInfo("Execution mode: " + std::to_string(static_cast<int>(mode)));
    OutputManager::printInfo("Number of trials: " + std::to_string(numTrials));
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(1000));
    config->set("region_count", size_t(2));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", uint64_t(42));
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    
    // Create brain
    OutputManager::printInfo("Initializing brain...");
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        OutputManager::printError("Failed to initialize brain!");
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
            OutputManager::printSuccess("Prediction improved through experience!");
        } else {
            OutputManager::printInfo("No significant prediction improvement yet.");
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
            OutputManager::printSuccess("Concepts emerged from repeated experience!");
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
            OutputManager::printSuccess("Selective attention emerged!");
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
            OutputManager::printSuccess("Planning ability demonstrated!");
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
            OutputManager::printSuccess("Self-model developing!");
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
            OutputManager::printSuccess("Social learning observed!");
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
    
    OutputManager::printInfo("\nPhase 4 demonstrates:");
    OutputManager::printInfo("  - Temporal prediction: Learning relationships between events");
    OutputManager::printInfo("  - Working memory: Maintaining information across delays");
    OutputManager::printInfo("  - Episodic memory: Storing and recalling experiences");
    OutputManager::printInfo("  - Concept formation: Discovering patterns without labels");
    OutputManager::printInfo("  - Neural attention: Selective processing through competition");
    OutputManager::printInfo("  - Multi-step planning: Using predictions to select actions");
    OutputManager::printInfo("  - Self-model: Learning body schema and action consequences");
    OutputManager::printInfo("  - Social learning: Observing and imitating others");
    OutputManager::printInfo("  - Continual learning: Adapting to new tasks");
    
    OutputManager::printInfo("\nAll mechanisms emerge from neural dynamics and plasticity.");
    OutputManager::printInfo("No predefined concepts, rules, or symbolic AI were used.");
    
    OutputManager::printInfo("\n" + std::string(60, '='));
    OutputManager::printInfo("Phase 4 Complete!");
    OutputManager::printInfo(std::string(60, '='));
    
    return 0;
}
