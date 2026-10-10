#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <fstream>
#include <exception>
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
#include "performance/Performance.hpp"
#include "visualization/VisualizationInterface.hpp"

using namespace nlm;

// Error handling functions
void logError(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
}

void logWarning(const std::string& message) {
    std::cerr << "[WARNING] " << message << std::endl;
}

void logSuccess(const std::string& message) {
    std::cout << "[SUCCESS] " << message << std::endl;
}

void logInfo(const std::string& message) {
    std::cout << "[INFO] " << message << std::endl;
}

// Enhanced header printing functions
void printEnhancedHeader(const std::string& title) {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(70, '=') << "\n";
}

void printEnhancedSection(const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
}

void printEnhancedResult(const std::string& name, float value, float max = 1.0f, bool showPercent = true) {
    std::cout << "  " << std::left << std::setw(35) << name << ": ";
    std::cout << std::fixed << std::setprecision(4) << value;
    if (showPercent && max > 0) {
        std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
    }
    std::cout << "\n";
}

// Enhanced configuration structure for Phase 4 demo
struct Phase4DemoConfig {
    size_t numTrials = 50;
    size_t neuronCount = 1000;
    size_t regionCount = 2;
    float connectionProbability = 0.1f;
    uint64_t randomSeed = 42;
    double simulationTimestep = 0.001;
    float stdpLtpWeight = 0.01f;
    float stdpLtdWeight = 0.012f;
    bool enableVerboseLogging = true;
    bool enableProgressReports = true;
    bool enableRealTimeVisualization = false;
    std::string outputFile = "";
    bool saveResults = false;
    bool runAllExperiments = true;
    std::vector<std::string> enabledExperiments;
    
    // Validate configuration
    bool validate() const {
        if (numTrials == 0) return false;
        if (neuronCount == 0) return false;
        if (regionCount == 0) return false;
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) return false;
        if (simulationTimestep <= 0.0) return false;
        
        // Validate experiment configuration
        if (enabledExperiments.empty() && !runAllExperiments) {
            // If not running all, at least one experiment should be enabled
            return false;
        }
        
        return true;
    }
    
    // Get help text
    void printHelp() const {
        std::cout << "NLM Phase 4 Enhanced Demo Options:" << std::endl;
        std::cout << "  --trials <count>                Number of trials (default: 50)" << std::endl;
        std::cout << "  --neurons <count>              Number of neurons (default: 1000)" << std::endl;
        std::cout << "  --regions <count>              Number of regions (default: 2)" << std::endl;
        std::cout << "  --connection-prob <float>       Connection probability (default: 0.1)" << std::endl;
        std::cout << "  --seed <seed>                   Random seed (default: 42)" << std::endl;
        std::cout << "  --no-verbose                    Disable verbose logging" << std::endl;
        std::cout << "  --no-progress                    Disable progress reports" << std::endl;
        std::cout << "  --enable-viz                     Enable real-time visualization" << std::endl;
        std::cout << "  --output <file>                Save results to file" << std::endl;
        std::cout << "  --experiments <list>            Comma-separated list of experiments to run" << std::endl;
        std::cout << "  --help, -h                      Show this help message" << std::endl;
        
        std::cout << "\nAvailable experiments:" << std::endl;
        std::cout << "  1 - Temporal Prediction Learning" << std::endl;
        std::cout << "  2 - Working Memory" << std::endl;
        std::cout << "  3 - Episodic Memory" << std::endl;
        std::cout << "  4 - Concept Formation" << std::endl;
        std::cout << "  5 - Neural Attention" << std::endl;
        std::cout << "  6 - Multi-Step Planning" << std::endl;
        std::cout << "  7 - Self-Model" << std::endl;
        std::cout << "  8 - Social Learning" << std::endl;
        std::cout << "  9 - Continual Learning" << std::endl;
        std::cout << "  10 - Integrated Phase 4" << std::endl;
        std::cout << "  Use --experiments to specify a subset (e.g., --experiments 1,3,5)" << std::endl;
    }
    
    // Parse experiment string
    std::vector<size_t> parseExperiments(const std::string& expString) const {
        std::vector<size_t> experiments;
        std::stringstream ss(expString);
        std::string token;
        
        while (std::getline(ss, token, ',')) {
            try {
                size_t expNum = std::stoull(token);
                if (expNum >= 1 && expNum <= 10) {
                    experiments.push_back(expNum);
                }
            } catch (...) {
                // Ignore invalid entries
            }
        }
        return experiments;
    }
    
    // Check if experiment is enabled
    bool isExperimentEnabled(size_t expNum) const {
        if (runAllExperiments) return true;
        if (enabledExperiments.empty()) return false;
        
        for (const auto& exp : enabledExperiments) {
            try {
                if (std::stoull(exp) == expNum) return true;
            } catch (...) {
                // Ignore invalid entries
            }
        }
        return false;
    }
};

// Enhanced experiment runner class
class Phase4ExperimentRunner {
private:
    std::shared_ptr<Brain> brain_;
    Phase4DemoConfig config_;
    bool verboseLogging_;
    
public:
    Phase4ExperimentRunner(std::shared_ptr<Brain> brain, const Phase4DemoConfig& config)
        : brain_(brain), config_(config), verboseLogging_(config.enableVerboseLogging) {}
    
    bool runAllExperiments() {
        bool success = true;
        
        // Run experiments in logical order
        if (config_.isExperimentEnabled(1)) {
            success &= runTemporalPredictionExperiment();
        }
        
        if (config_.isExperimentEnabled(2)) {
            success &= runWorkingMemoryExperiment();
        }
        
        if (config_.isExperimentEnabled(3)) {
            success &= runEpisodicRecallExperiment();
        }
        
        if (config_.isExperimentEnabled(4)) {
            success &= runConceptFormationExperiment();
        }
        
        if (config_.isExperimentEnabled(5)) {
            success &= runAttentionExperiment();
        }
        
        if (config_.isExperimentEnabled(6)) {
            success &= runPlanningExperiment();
        }
        
        if (config_.isExperimentEnabled(7)) {
            success &= runSelfModelExperiment();
        }
        
        if (config_.isExperimentEnabled(8)) {
            success &= runSocialLearningExperiment();
        }
        
        if (config_.isExperimentEnabled(9)) {
            success &= runContinualLearningExperiment();
        }
        
        if (config_.isExperimentEnabled(10)) {
            success &= runIntegratedExperiment();
        }
        
        return success;
    }
    
private:
    bool runTemporalPredictionExperiment() {
        printEnhancedSection("1. Temporal Prediction Learning");
        
        try {
            TemporalPredictionExperiment experiment;
            Phase4Results results = experiment.run(brain_.get(), config_.numTrials);
            
            printEnhancedResult("Initial Prediction Error", results.predictionErrorInitial);
            printEnhancedResult("Final Prediction Error", results.predictionErrorFinal);
            printEnhancedResult("Improvement", results.predictionAccuracyImprovement);
            
            float improvement = results.predictionErrorInitial - results.predictionErrorFinal;
            if (improvement > 0) {
                logSuccess("Prediction improved through experience!");
            } else {
                logInfo("No significant prediction improvement yet.");
            }
            
            return true;
        } catch (const std::exception& e) {
            logError(std::string("Temporal Prediction Experiment failed: ") + e.what());
            return false;
        }
    }
    
    bool runWorkingMemoryExperiment() {
        printEnhancedSection("2. Working Memory");
        
        try {
            WorkingMemoryExperiment experiment;
            Phase4Results results = experiment.run(brain_.get(), config_.numTrials);
            
            printEnhancedResult("Initial Retention", results.workingMemoryRetentionInitial);
            printEnhancedResult("Final Retention", results.workingMemoryRetentionFinal);
            printEnhancedResult("Capacity", results.workingMemoryCapacity, 100.0f);
            
            return true;
        } catch (const std::exception& e) {
            logError(std::string("Working Memory Experiment failed: ") + e.what());
            return false;
        }
    }
    
    bool runEpisodicRecallExperiment() {
        printEnhancedSection("3. Episodic Memory");
        
        try {
            EpisodicRecallExperiment experiment;
            Phase4Results results = experiment.run(brain_.get(), config_.numTrials);
            
            printEnhancedResult("Recall Accuracy", results.episodicRecallAccuracy);
            printEnhancedResult("Episodes Stored", results.episodesStored);
            printEnhancedResult("Episodic Influence", results.episodicInfluence);
            
            return true;
        } catch (const std::exception& e) {
            logError(std::string("Episodic Recall Experiment failed: ") + e.what());
            return false;
        }
    }
    
    bool runConceptFormationExperiment() {
        printEnhancedSection("4. Concept Formation");
        
        try {
            ConceptFormationExperiment experiment;
            Phase4Results results = experiment.run(brain_.get(), config_.numTrials);
            
            printEnhancedResult("Concepts Formed", results.conceptsFormed);
            printEnhancedResult("Concept Stability", results.conceptStability);
            printEnhancedResult("Generalization Ability", results.generalizationAbility);
            
            if (results.conceptsFormed > 0) {
                logSuccess("Concepts emerged from repeated experience!");
            }
            
            return true;
        } catch (const std::exception& e) {
            logError(std::string("Concept Formation Experiment failed: ") + e.what());
            return false;
        }
    }
    
    bool runAttentionExperiment() {
        printEnhancedSection("5. Neural Attention");
        
        try {
            AttentionExperiment experiment;
            Phase4Results results = experiment.run(brain_.get(), config_.numTrials);
            
            printEnhancedResult("Attention Selectivity", results.attentionSelectivity);
            printEnhancedResult("Distraction Resistance", results.distractionResistance);
            
            if (results.attentionSelectivity > 0.5f) {
                logSuccess("Selective attention emerged!");
            }
            
            return true;
        } catch (const std::exception& e) {
            logError(std::string("Attention Experiment failed: ") + e.what());
            return false;
        }
    }
    
    bool runPlanningExperiment() {
        printEnhancedSection("6. Multi-Step Planning");
        
        try {
            PlanningExperiment experiment;
            Phase4Results results = experiment.run(brain_.get(), config_.numTrials / 2);
            
            printEnhancedResult("Planning Accuracy", results.planningAccuracy);
            printEnhancedResult("Planning Confidence", results.planningConfidence);
            
            if (results.planningAccuracy > 0.4f) {
                logSuccess("Planning ability demonstrated!");
            }
            
            return true;
        } catch (const std::exception& e) {
            logError(std::string("Planning Experiment failed: ") + e.what());
            return false;
        }
    }
    
    bool runSelfModelExperiment() {
        printEnhancedSection("7. Self-Model");
        
        try {
            SelfModelExperiment experiment;
            Phase4Results results = experiment.run(brain_.get(), config_.numTrials);
            
            printEnhancedResult("Self-Prediction Accuracy", results.selfPredictionAccuracy);
            printEnhancedResult("Body Awareness", results.bodyAwareness);
            
            if (results.selfPredictionAccuracy > 0.3f) {
                logSuccess("Self-model developing!");
            }
            
            return true;
        } catch (const std::exception& e) {
            logError(std::string("Self-Model Experiment failed: ") + e.what());
            return false;
        }
    }
    
    bool runSocialLearningExperiment() {
        printEnhancedSection("8. Social Learning");
        
        try {
            SocialLearningExperiment experiment;
            Phase4Results results = experiment.run(brain_.get(), config_.numTrials / 2);
            
            printEnhancedResult("Imitation Accuracy", results.imitationAccuracy);
            printEnhancedResult("Observations from Others", results.observationsFromOthers);
            
            if (results.imitationAccuracy > 0.2f) {
                logSuccess("Social learning observed!");
            }
            
            return true;
        } catch (const std::exception& e) {
            logError(std::string("Social Learning Experiment failed: ") + e.what());
            return false;
        }
    }
    
    bool runContinualLearningExperiment() {
        printEnhancedSection("9. Continual Learning");
        
        try {
            ContinualLearningExperiment experiment;
            Phase4Results results = experiment.run(brain_.get(), config_.numTrials);
            
            printEnhancedResult("Behavior Improvement", results.behaviorImprovement);
            printEnhancedResult("Transfer Performance", results.transferPerformance);
            
            return true;
        } catch (const std::exception& e) {
            logError(std::string("Continual Learning Experiment failed: ") + e.what());
            return false;
        }
    }
    
    bool runIntegratedExperiment() {
        printEnhancedSection("10. Integrated Phase 4");
        
        try {
            Phase4IntegratedExperiment experiment;
            Phase4Results results = experiment.run(brain_.get(), 10, 50);
            
            printEnhancedResult("Total Reward", results.totalReward);
            printEnhancedResult("Episodes Stored", results.episodesStored);
            printEnhancedResult("Concepts Formed", results.conceptsFormed);
            printEnhancedResult("Planning Confidence", results.planningConfidence);
            printEnhancedResult("Body Awareness", results.bodyAwareness);
            
            return true;
        } catch (const std::exception& e) {
            logError(std::string("Integrated Experiment failed: ") + e.what());
            return false;
        }
    }
    
    // Helper function to parse command line arguments
    static Phase4DemoConfig parseCommandLineArgs(int argc, char* argv[]) {
        Phase4DemoConfig config;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                config.printHelp();
                exit(0);
            }
            
            if (arg == "--no-verbose") {
                config.enableVerboseLogging = false;
            } else if (arg == "--no-progress") {
                config.enableProgressReports = false;
            } else if (arg == "--enable-viz") {
                config.enableRealTimeVisualization = true;
            } else if (arg.find("--trials=") == 0) {
                try {
                    config.numTrials = std::stoull(arg.substr(9));
                } catch (...) {
                    std::cerr << "Error: Invalid trial count" << std::endl;
                }
            } else if (arg.find("--neurons=") == 0) {
                try {
                    config.neuronCount = std::stoull(arg.substr(10));
                } catch (...) {
                    std::cerr << "Error: Invalid neuron count" << std::endl;
                }
            } else if (arg.find("--regions=") == 0) {
                try {
                    config.regionCount = std::stoull(arg.substr(9));
                } catch (...) {
                    std::cerr << "Error: Invalid region count" << std::endl;
                }
            } else if (arg.find("--connection-prob=") == 0) {
                try {
                    config.connectionProbability = std::stof(arg.substr(20));
                } catch (...) {
                    std::cerr << "Error: Invalid connection probability" << std::endl;
                }
            } else if (arg.find("--seed=") == 0) {
                try {
                    config.randomSeed = std::stoull(arg.substr(7));
                } catch (...) {
                    std::cerr << "Error: Invalid seed" << std::endl;
                }
            } else if (arg.find("--output=") == 0) {
                config.outputFile = arg.substr(9);
                config.saveResults = true;
            } else if (arg.find("--experiments=") == 0) {
                config.enabledExperiments = config.parseExperiments(arg.substr(15));
                config.runAllExperiments = false;
            }
        }
        
        return config;
    }
};

int main(int argc, char* argv[]) {
    try {
        std::cout << "NLM Phase 4: Emerging Cognition\n";
        std::cout << "================================\n\n";
        
        // Parse command line arguments
        Phase4DemoConfig config = Phase4DemoConfig::parseCommandLineArgs(argc, argv);
        
        // Validate configuration
        if (!config.validate()) {
            logError("Invalid configuration provided");
            config.printHelp();
            return 1;
        }
        
        // Initialize logging
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(config.enableVerboseLogging ? LogLevel::Info : LogLevel::Warning);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Create configuration
        auto configObj = std::make_shared<Config>();
        configObj->set("neuron_count", static_cast<int64_t>(config.neuronCount));
        configObj->set("region_count", static_cast<int64_t>(config.regionCount));
        configObj->set("connection_probability", config.connectionProbability);
        configObj->set("random_seed", static_cast<int64_t>(config.randomSeed));
        configObj->set("simulation_timestep", config.simulationTimestep);
        configObj->set("stdp_ltp_weight", config.stdpLtpWeight);
        configObj->set("stdp_ltd_weight", config.stdpLtdWeight);
        
        // Create brain
        std::cout << "Initializing brain...\n";
        auto brain = std::make_shared<Brain>(configObj);
        if (!brain->initialize()) {
            logError("Failed to initialize brain!");
            return 1;
        }
        
        printEnhancedHeader("PHASE 4 EXPERIMENTS");
        
        // Create experiment runner and run all experiments
        Phase4ExperimentRunner runner(brain, config);
        bool success = runner.runAllExperiments();
        
        // Final summary
        printEnhancedHeader("PHASE 4 SUMMARY");
        
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
        
        std::cout << "\n" << std::string(70, '=') << "\n";
        std::cout << "Phase 4 " << (success ? "Complete!" : "Complete with Errors!") << "\n";
        std::cout << std::string(70, '=') << "\n";
        
        return success ? 0 : 1;
        
    } catch (const std::exception& e) {
        logError(std::string("Fatal exception: ") + e.what());
        return 1;
    } catch (...) {
        logError("Unknown fatal exception");
        return 1;
    }
}

} // namespace nlm