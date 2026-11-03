#include "DemoFramework.hpp"
#include "Phase4Experiment.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>

using namespace nlm;

// Helper functions for phase 4 demo (consolidated)
namespace Phase4Helpers {
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
}

class Phase4Demo : public DemoBase {
public:
    Phase4Demo() : DemoBase() {}
    ~Phase4Demo() {}
    
    std::string getDemoName() const override {
        return "PHASE 4: EMERGING COGNITION";
    }
    
    void onSetup() override {
        std::cout << "Setting up Phase 4 demo...\n";
    }
    
    void onRun(int numTrials, int stepsPerEpisode) override {
        // Create configuration using factory
        auto config = ConfigFactory::createPhase4Config(1000);
        
        // Create brain
        std::cout << "Initializing brain...\n";
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw DemoException("Failed to initialize brain!");
        }
        
        printPhase4Header();
        
        // Run all experiments
        runTemporalPredictionExperiment(brain, numTrials);
        runWorkingMemoryExperiment(brain, numTrials);
        runEpisodicRecallExperiment(brain, numTrials);
        runConceptFormationExperiment(brain, numTrials);
        runAttentionExperiment(brain, numTrials);
        runPlanningExperiment(brain, numTrials / 2);
        runSelfModelExperiment(brain, numTrials);
        runSocialLearningExperiment(brain, numTrials / 2);
        runContinualLearningExperiment(brain, numTrials);
        runIntegratedExperiment(brain, 10, 50);
        
        printPhase4Summary();
    }
    
    void onComplete() override {
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "Phase 4 Complete!\n";
        std::cout << std::string(60, '=') << "\n";
    }
    
private:
    void printPhase4Header() {
        std::cout << "NLM Phase 4: Emerging Cognition\n";
        std::cout << "================================\n\n";
    }
    
    void printPhase4Summary() {
        Phase4Helpers::printHeader("PHASE 4 SUMMARY");
        
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
    }
    
    void runTemporalPredictionExperiment(std::shared_ptr<Brain> brain, size_t numTrials) {
        Phase4Helpers::printSection("1. Temporal Prediction Learning");
        {
            TemporalPredictionExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials);
            
            Phase4Helpers::printResult("Initial Prediction Error", results.predictionErrorInitial);
            Phase4Helpers::printResult("Final Prediction Error", results.predictionErrorFinal);
            Phase4Helpers::printResult("Improvement", results.predictionAccuracyImprovement);
            
            float improvement = results.predictionErrorInitial - results.predictionErrorFinal;
            if (improvement > 0) {
                std::cout << "  [SUCCESS] Prediction improved through experience!\n";
            } else {
                std::cout << "  [INFO] No significant prediction improvement yet.\n";
            }
        }
    }
    
    void runWorkingMemoryExperiment(std::shared_ptr<Brain> brain, size_t numTrials) {
        Phase4Helpers::printSection("2. Working Memory");
        {
            WorkingMemoryExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials);
            
            Phase4Helpers::printResult("Initial Retention", results.workingMemoryRetentionInitial);
            Phase4Helpers::printResult("Final Retention", results.workingMemoryRetentionFinal);
            Phase4Helpers::printResult("Capacity", results.workingMemoryCapacity, 100.0f);
        }
    }
    
    void runEpisodicRecallExperiment(std::shared_ptr<Brain> brain, size_t numTrials) {
        Phase4Helpers::printSection("3. Episodic Memory");
        {
            EpisodicRecallExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials);
            
            Phase4Helpers::printResult("Recall Accuracy", results.episodicRecallAccuracy);
            Phase4Helpers::printResult("Episodes Stored", results.episodesStored);
            Phase4Helpers::printResult("Episodic Influence", results.episodicInfluence);
        }
    }
    
    void runConceptFormationExperiment(std::shared_ptr<Brain> brain, size_t numTrials) {
        Phase4Helpers::printSection("4. Concept Formation");
        {
            ConceptFormationExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials);
            
            Phase4Helpers::printResult("Concepts Formed", results.conceptsFormed);
            Phase4Helpers::printResult("Concept Stability", results.conceptStability);
            Phase4Helpers::printResult("Generalization Ability", results.generalizationAbility);
            
            if (results.conceptsFormed > 0) {
                std::cout << "  [SUCCESS] Concepts emerged from repeated experience!\n";
            }
        }
    }
    
    void runAttentionExperiment(std::shared_ptr<Brain> brain, size_t numTrials) {
        Phase4Helpers::printSection("5. Neural Attention");
        {
            AttentionExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials);
            
            Phase4Helpers::printResult("Attention Selectivity", results.attentionSelectivity);
            Phase4Helpers::printResult("Distraction Resistance", results.distractionResistance);
            
            if (results.attentionSelectivity > 0.5f) {
                std::cout << "  [SUCCESS] Selective attention emerged!\n";
            }
        }
    }
    
    void runPlanningExperiment(std::shared_ptr<Brain> brain, size_t numTrials) {
        Phase4Helpers::printSection("6. Multi-Step Planning");
        {
            PlanningExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials);
            
            Phase4Helpers::printResult("Planning Accuracy", results.planningAccuracy);
            Phase4Helpers::printResult("Planning Confidence", results.planningConfidence);
            
            if (results.planningAccuracy > 0.4f) {
                std::cout << "  [SUCCESS] Planning ability demonstrated!\n";
            }
        }
    }
    
    void runSelfModelExperiment(std::shared_ptr<Brain> brain, size_t numTrials) {
        Phase4Helpers::printSection("7. Self-Model");
        {
            SelfModelExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials);
            
            Phase4Helpers::printResult("Self-Prediction Accuracy", results.selfPredictionAccuracy);
            Phase4Helpers::printResult("Body Awareness", results.bodyAwareness);
            
            if (results.selfPredictionAccuracy > 0.3f) {
                std::cout << "  [SUCCESS] Self-model developing!\n";
            }
        }
    }
    
    void runSocialLearningExperiment(std::shared_ptr<Brain> brain, size_t numTrials) {
        Phase4Helpers::printSection("8. Social Learning");
        {
            SocialLearningExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials);
            
            Phase4Helpers::printResult("Imitation Accuracy", results.imitationAccuracy);
            Phase4Helpers::printResult("Observations from Others", results.observationsFromOthers);
            
            if (results.imitationAccuracy > 0.2f) {
                std::cout << "  [SUCCESS] Social learning observed!\n";
            }
        }
    }
    
    void runContinualLearningExperiment(std::shared_ptr<Brain> brain, size_t numTrials) {
        Phase4Helpers::printSection("9. Continual Learning");
        {
            ContinualLearningExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials);
            
            Phase4Helpers::printResult("Behavior Improvement", results.behaviorImprovement);
            Phase4Helpers::printResult("Transfer Performance", results.transferPerformance);
        }
    }
    
    void runIntegratedExperiment(std::shared_ptr<Brain> brain, int episodes, int trialsPerEpisode) {
        Phase4Helpers::printSection("10. Integrated Phase 4");
        {
            Phase4IntegratedExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), episodes, trialsPerEpisode);
            
            Phase4Helpers::printResult("Total Reward", results.totalReward);
            Phase4Helpers::printResult("Episodes Stored", results.episodesStored);
            Phase4Helpers::printResult("Concepts Formed", results.conceptsFormed);
            Phase4Helpers::printResult("Planning Confidence", results.planningConfidence);
            Phase4Helpers::printResult("Body Awareness", results.bodyAwareness);
        }
    }
};

// Main entry point for Phase 4 demo
int main(int argc, char* argv[]) {
    try {
        std::cout << "NLM Phase 4: Emerging Cognition\n";
        std::cout << "================================\n\n";
        
        // Parse command line arguments
        size_t numTrials = 50;
        if (argc > 1) {
            numTrials = std::stoi(argv[1]);
        }
        
        nlm::Phase4Demo demo;
        demo.run(static_cast<int>(numTrials), 0); // stepsPerEpisode not used in phase 4
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

