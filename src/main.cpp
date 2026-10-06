// NLM (熙然) - Neural Learning Machine
// Enhanced Main CLI
// Modern command-line interface for NLM with support for multiple execution modes,
// advanced configuration options, and comprehensive output handling

#include "cli/MainCLI.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <nlohmann/json.hpp>

// Include demo executables for backward compatibility
#include "experiments/Phase3Demo.cpp"
#include "experiments/Phase4Demo.cpp"
#include "experiments/Phase6Demo.cpp"

using json = nlohmann::json;
using namespace nlm;

// Legacy functions for backward compatibility
void runLegacyPhase2() {
    std::cout << "Legacy Phase 2 mode - Basic neural computation with STDP and plasticity" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Run basic tests
    std::cout << "Running basic connectivity test..." << std::endl;
    runBasicConnectivityTest(brain);
    
    // Reset for plasticity experiment
    brain->reset();
    brain->initialize();
    
    std::cout << "Running plasticity learning experiment..." << std::endl;
    runPlasticityExperiment(brain);
    
    // Reset for STDP verification
    brain->reset();
    brain->initialize();
    
    std::cout << "Running STDP verification..." << std::endl;
    runStdpVerification(brain);
}

void runLegacyPhase3Demo(int numEpisodes, int stepsPerEpisode) {
    std::cout << "Legacy Phase 3 Demo - Agent-world interaction simulation" << std::endl;
    
    nlm::Phase3Demo demo;
    demo.run(numEpisodes, stepsPerEpisode);
}

void runLegacyPhase4Demo(int numTrials) {
    std::cout << "Legacy Phase 4 Demo - Cognitive capabilities demonstration" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(1000));
    config->set("region_count", size_t(2));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", uint64_t(42));
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Run all Phase 4 experiments
    printHeader("PHASE 4 EXPERIMENTS");
    
    printSection("1. Temporal Prediction Learning");
    {
        TemporalPredictionExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), numTrials);
        printResult("Initial Prediction Error", results.predictionErrorInitial);
        printResult("Final Prediction Error", results.predictionErrorFinal);
    }
    
    printSection("2. Working Memory");
    {
        WorkingMemoryExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), numTrials);
        printResult("Initial Retention", results.workingMemoryRetentionInitial);
        printResult("Final Retention", results.workingMemoryRetentionFinal);
    }
    
    printSection("3. Episodic Recall");
    {
        EpisodicRecallExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), numTrials);
        printResult("Recall Accuracy", results.episodicRecallAccuracy);
        printResult("Episodes Stored", results.episodesStored);
    }
    
    printSection("4. Concept Formation");
    {
        ConceptFormationExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), numTrials);
        printResult("Concepts Formed", results.conceptsFormed);
        printResult("Concept Stability", results.conceptStability);
    }
    
    printSection("5. Neural Attention");
    {
        AttentionExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), numTrials);
        printResult("Attention Selectivity", results.attentionSelectivity);
        printResult("Distraction Resistance", results.distractionResistance);
    }
    
    printSection("6. Multi-Step Planning");
    {
        PlanningExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), numTrials / 2);
        printResult("Planning Accuracy", results.planningAccuracy);
        printResult("Planning Confidence", results.planningConfidence);
    }
    
    printSection("7. Self-Model");
    {
        SelfModelExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), numTrials);
        printResult("Self-Prediction Accuracy", results.selfPredictionAccuracy);
        printResult("Body Awareness", results.bodyAwareness);
    }
    
    printSection("8. Social Learning");
    {
        SocialLearningExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), numTrials / 2);
        printResult("Imitation Accuracy", results.imitationAccuracy);
        printResult("Observations from Others", results.observationsFromOthers);
    }
    
    printSection("9. Continual Learning");
    {
        ContinualLearningExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), numTrials);
        printResult("Behavior Improvement", results.behaviorImprovement);
        printResult("Transfer Performance", results.transferPerformance);
    }
    
    printSection("10. Integrated Phase 4");
    {
        Phase4IntegratedExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), 10, 50);
        printResult("Total Reward", results.totalReward);
        printResult("Episodes Stored", results.episodesStored);
        printResult("Concepts Formed", results.conceptsFormed);
    }
    
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
    
    printHeader("PHASE 4 COMPLETE!");
}

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

void printResult(const std::string& name, float value, float max) {
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

int main(int argc, char** argv) {
    try {
        // Print banner
        printBanner();
        
        // Initialize main CLI
        auto& mainCLI = MainCLI::getInstance();
        
        // Run with provided arguments or show help
        if (argc == 1) {
            mainCLI.printHelp();
            std::cout << std::endl;
            std::cout << "Use --help for more information about available options." << std::endl;
            return 0;
        }
        
        // Run main CLI
        return mainCLI.run(argc, argv);
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
