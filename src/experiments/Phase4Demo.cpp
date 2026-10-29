#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>
#include <map>
#include <string>
#include <algorithm>
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

// Forward declarations for argument parsing
namespace nlm {
    class ArgumentParser {
    public:
        static void printHelp();
        static bool printVersion();
        static bool parseArgs(int argc, char** argv, std::map<std::string, std::string>& parsedArgs);
        static std::string getVersion();
    };
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

namespace nlm {

void ArgumentParser::printHelp() {
    std::cout << "Usage: nlm_phase4_demo [options]" << std::endl;
    std::cout << "Neural learning machine Phase 4 demonstration - Emerging cognition" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --version, -v                 Show version information" << std::endl;
    std::cout << "  --trials <count>               Number of trials per experiment (default: 50)" << std::endl;
    std::cout << "  --verbose, -V                 Enable verbose output" << std::endl;
    std::cout << "  --output <file>               Save results to file" << std::endl;
    std::cout << "  --seed <seed>                 Random seed for reproducible results" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm_phase4_demo --help" << std::endl;
    std::cout << "  nlm_phase4_demo --version" << std::endl;
    std::cout << "  nlm_phase4_demo --trials 100" << std::endl;
    std::cout << "  nlm_phase4_demo --seed 12345" << std::endl;
}

bool ArgumentParser::printVersion() {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Phase 4: Emerging Cognition" << std::endl;
    std::cout << std::endl;
    std::cout << "Features demonstrated:" << std::endl;
    std::cout << "  - Temporal prediction: Learning cause-effect relationships" << std::endl;
    std::cout << "  - Working memory: Maintaining information across delays" << std::endl;
    std::cout << "  - Episodic memory: Storing and recalling experiences" << std::endl;
    std::cout << "  - Concept formation: Discovering patterns without labels" << std::endl;
    std::cout << "  - Neural attention: Selective processing through competition" << std::endl;
    std::cout << "  - Multi-step planning: Using predictions to select actions" << std::endl;
    std::cout << "  - Self-model: Learning body schema and action consequences" << std::endl;
    std::cout << "  - Social learning: Observing and imitating others" << std::endl;
    std::cout << "  - Continual learning: Adapting to new tasks" << std::endl;
    std::cout << std::endl;
    std::cout << "Build Information:" << std::endl;
    std::cout << "  C++ Standard: C++20" << std::endl;
    std::cout << "  Author: NLM Project Contributors" << std::endl;
    std::cout << "  License: MIT" << std::endl;
    return true;
}

std::string ArgumentParser::getVersion() {
    return "NLM 0.1.0 (Phase 4: Emerging Cognition)";
}

bool ArgumentParser::parseArgs(int argc, char** argv, std::map<std::string, std::string>& parsedArgs) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key value or --key=value patterns
        if (arg.find('--') == 0) {
            std::string key;
            std::string value;
            size_t equalsPos = arg.find('=');
            
            if (equalsPos != std::string::npos) {
                key = arg.substr(2, equalsPos - 2);
                value = arg.substr(equalsPos + 1);
            } else {
                key = arg.substr(2);
                // Check if next argument exists and doesn't start with --
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    value = argv[++i];
                } else {
                    value = "true"; // Flag argument
                }
            }
            
            // Convert key to canonical form (remove dashes)
            std::replace(key.begin(), key.end(), '-', '_');
            parsedArgs[key] = value;
        } else if (arg.find('-') == 0 && arg.size() > 1 && !std::isalnum(arg[1])) {
            // Handle short options like -h, -v
            std::string key = arg.substr(1, 1);
            std::string value = "true";
            
            // Check for combined short options like -hV
            if (arg.size() > 2) {
                for (size_t j = 1; j < arg.size(); ++j) {
                    std::string singleKey(1, arg[j]);
                    parsedArgs[singleKey] = "true";
                }
                continue;
            }
            
            parsedArgs[key] = value;
        }
    }
    return true;
}

} // namespace nlm

int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::map<std::string, std::string> parsedArgs;
    if (!nlm::ArgumentParser::parseArgs(argc, argv, parsedArgs)) {
        std::cerr << "ERROR: Failed to parse arguments." << std::endl;
        return 1;
    }
    
    // Handle help and version flags
    if (parsedArgs.find("h") != parsedArgs.end() || parsedArgs.find("help") != parsedArgs.end()) {
        nlm::ArgumentParser::printHelp();
        return 0;
    }
    
    if (parsedArgs.find("v") != parsedArgs.end() || parsedArgs.find("version") != parsedArgs.end()) {
        nlm::ArgumentParser::printVersion();
        return 0;
    }
    
    std::cout << "NLM Phase 4: Emerging Cognition\n";
    std::cout << "================================\n\n";
    
    // Parse and validate trials argument
    size_t numTrials = 50;
    if (parsedArgs.find("trials") != parsedArgs.end()) {
        try {
            numTrials = std::stoul(parsedArgs["trials"]);
            if (numTrials == 0) {
                std::cerr << "ERROR: Number of trials must be positive. Got: " << numTrials << std::endl;
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Invalid trials value: " << parsedArgs["trials"] << std::endl;
            return 1;
        }
    }
    
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