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
#include "core/CLI/CLI.hpp"
#include "core/CLI/CLIApplication.hpp"

using namespace nlm;

class Phase4DemoApp : public CLIApplication {
public:
    Phase4DemoApp() : CLIApplication("nlm_phase4_demo", 
        "NLM Phase 4: Emerging Cognition\n"
        "Demonstrates: attention, planning, concept formation, self-model, social learning\n"
        "All cognitive mechanisms emerge from neural dynamics and plasticity, "
        "without predefined concepts or symbolic AI.") {
        setVersion("Phase 4.0");
        registerStandardArguments();
        
        // Add Phase 4 specific arguments
        getParser().addArgument("trials", "t", "Number of trials", "50",
                              CLI::validatePositiveInt, nullptr, false, "int");
        getParser().addArgument("neuron-count", "n", "Number of neurons", "1000",
                              CLI::validatePositiveInt, nullptr, false, "int");
        getParser().addArgument("mode", "", "Operating mode (basic, full, integrated)", "full",
                              [](const std::string& v) {
                                  return v == "basic" || v == "full" || v == "integrated";
                              }, nullptr, false, "string");
    }

    bool initialize() override {
        if (parser.hasArgument("log-level")) {
            auto levelOpt = parser.getArgument<std::string>("log-level");
            if (levelOpt) {
                logInfo("Log level set to: " + *levelOpt);
            }
        }
        
        if (parser.hasArgument("benchmark")) {
            logInfo("Benchmark mode enabled");
        }
        
        return true;
    }

    int execute() override {
        // Parse Phase 4 specific arguments
        size_t numTrials = 50;
        size_t neuronCount = 1000;
        std::string mode = "full";
        
        if (parser.hasArgument("trials")) {
            auto trialsOpt = parser.getArgument<size_t>("trials");
            if (trialsOpt) numTrials = *trialsOpt;
        }
        
        if (parser.hasArgument("neuron-count")) {
            auto neuronsOpt = parser.getArgument<size_t>("neuron-count");
            if (neuronsOpt) neuronCount = *neuronsOpt;
        }
        
        if (parser.hasArgument("mode")) {
            auto modeOpt = parser.getArgument<std::string>("mode");
            if (modeOpt) mode = *modeOpt;
        }
        
        logInfo("Starting Phase 4 demo with " + std::to_string(numTrials) + 
                " trials, " + std::to_string(neuronCount) + " neurons, mode: " + mode);
        
        runDemo(numTrials, neuronCount, mode);
        
        return 0;
    }

private:
    void runDemo(size_t numTrials, size_t neuronCount, const std::string& mode) {
        printHeader("PHASE 4 EXPERIMENTS");
        
        // Create configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
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
            return;
        }
        
        // Run experiments based on mode
        if (mode == "basic" || mode == "full") {
            runBasicExperiments(brain.get(), numTrials);
        }
        
        if (mode == "full" || mode == "integrated") {
            runIntegratedExperiment(brain.get(), numTrials);
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
    }
    
private:
    void runBasicExperiments(Brain* brain, size_t numTrials) {
        // Run Temporal Prediction Experiment
        printSection("1. Temporal Prediction Learning");
        {
            TemporalPredictionExperiment experiment;
            Phase4Results results = experiment.run(brain, numTrials);
            
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
            Phase4Results results = experiment.run(brain, numTrials);
            
            printResult("Initial Retention", results.workingMemoryRetentionInitial);
            printResult("Final Retention", results.workingMemoryRetentionFinal);
            printResult("Capacity", results.workingMemoryCapacity, 100.0f);
        }
        
        // Run Episodic Recall Experiment
        printSection("3. Episodic Memory");
        {
            EpisodicRecallExperiment experiment;
            Phase4Results results = experiment.run(brain, numTrials);
            
            printResult("Recall Accuracy", results.episodicRecallAccuracy);
            printResult("Episodes Stored", results.episodesStored);
            printResult("Episodic Influence", results.episodicInfluence);
        }
        
        // Run Concept Formation Experiment
        printSection("4. Concept Formation");
        {
            ConceptFormationExperiment experiment;
            Phase4Results results = experiment.run(brain, numTrials);
            
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
            Phase4Results results = experiment.run(brain, numTrials);
            
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
            Phase4Results results = experiment.run(brain, numTrials / 2);
            
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
            Phase4Results results = experiment.run(brain, numTrials);
            
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
            Phase4Results results = experiment.run(brain, numTrials / 2);
            
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
            Phase4Results results = experiment.run(brain, numTrials);
            
            printResult("Behavior Improvement", results.behaviorImprovement);
            printResult("Transfer Performance", results.transferPerformance);
        }
    }
    
    void runIntegratedExperiment(Brain* brain, size_t numTrials) {
        printSection("10. Integrated Phase 4");
        {
            Phase4IntegratedExperiment experiment;
            Phase4Results results = experiment.run(brain, 10, 50);
            
            printResult("Total Reward", results.totalReward);
            printResult("Episodes Stored", results.episodesStored);
            printResult("Concepts Formed", results.conceptsFormed);
            printResult("Planning Confidence", results.planningConfidence);
            printResult("Body Awareness", results.bodyAwareness);
        }
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
};

int main(int argc, char* argv[]) {
    Phase4DemoApp app;
    return app.run(argc, argv);
}
