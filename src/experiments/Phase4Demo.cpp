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
#include "experiments/NLMCLI.hpp"

using namespace nlm;

void printHeader(const std::string& title) {
    nlm::printHeader(title);
}

void printResult(const std::string& name, float value, float max = 1.0f) {
    nlm::printResult(name, value, max);
}

void printSection(const std::string& title) {
    nlm::printSection(title);
}

int main(int argc, char* argv[]) {
    // Parse command line arguments using the new CLI system
    auto cliOptions = CommandLineInterface::parseArgs(argc, argv);
    
    // Handle help/version options
    if (cliOptions.help) {
        CommandLineInterface::printUsage(argv[0]);
        return 0;
    }
    
    if (cliOptions.version) {
        CommandLineInterface::printVersion();
        return 0;
    }
    
    // Setup logging
    CommandLineInterface::setupLogging(cliOptions);
    
    // Print banner
    printBanner("Phase 4: Emerging Cognition");
    
    // Set default trials if not specified
    size_t numTrials = cliOptions.numTrials;
    if (numTrials == 0) {
        numTrials = 50;
    }
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(1000));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    
    // Load config file if specified
    if (!cliOptions.configFile.empty()) {
        if (config->loadFromFile(cliOptions.configFile)) {
            printInfo("Loaded configuration from: " + cliOptions.configFile);
        } else {
            printWarning("Failed to load configuration from: " + cliOptions.configFile);
        }
    }
    
    // Create brain
    printInfo("Initializing brain...");
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        printError("Failed to initialize brain!");
        return 1;
    }
    
    printInfo("Configuration: " + std::to_string(numTrials) + " trials, " + 
              std::to_string(brain->getTotalNeuronCount()) + " neurons");
    
    printHeader("PHASE 4 EXPERIMENTS");
    
    // Run all experiments
    std::stringstream results;
    
    // Run Temporal Prediction Experiment
    printSection("1. Temporal Prediction Learning");
    {
        TemporalPredictionExperiment experiment;
        Phase4Results expResults = experiment.run(brain.get(), numTrials);
        
        printResult("Initial Prediction Error", expResults.predictionErrorInitial);
        printResult("Final Prediction Error", expResults.predictionErrorFinal);
        printResult("Improvement", expResults.predictionAccuracyImprovement);
        
        float improvement = expResults.predictionErrorInitial - expResults.predictionErrorFinal;
        if (improvement > 0) {
            printSuccess("Prediction improved through experience!");
        } else {
            printInfo("No significant prediction improvement yet.");
        }
        
        // Save results to string
        results << "1. Temporal Prediction Learning:" << std::endl;
        results << "   Initial Prediction Error: " << expResults.predictionErrorInitial << std::endl;
        results << "   Final Prediction Error: " << expResults.predictionErrorFinal << std::endl;
        results << "   Improvement: " << expResults.predictionAccuracyImprovement << std::endl;
    }
    
    // Run Working Memory Experiment
    printSection("2. Working Memory");
    {
        WorkingMemoryExperiment experiment;
        Phase4Results expResults = experiment.run(brain.get(), numTrials);
        
        printResult("Initial Retention", expResults.workingMemoryRetentionInitial);
        printResult("Final Retention", expResults.workingMemoryRetentionFinal);
        printResult("Capacity", expResults.workingMemoryCapacity, 100.0f);
        
        results << "2. Working Memory:" << std::endl;
        results << "   Initial Retention: " << expResults.workingMemoryRetentionInitial << std::endl;
        results << "   Final Retention: " << expResults.workingMemoryRetentionFinal << std::endl;
        results << "   Capacity: " << expResults.workingMemoryCapacity << std::endl;
    }
    
    // Run Episodic Recall Experiment
    printSection("3. Episodic Memory");
    {
        EpisodicRecallExperiment experiment;
        Phase4Results expResults = experiment.run(brain.get(), numTrials);
        
        printResult("Recall Accuracy", expResults.episodicRecallAccuracy);
        printResult("Episodes Stored", expResults.episodesStored);
        printResult("Episodic Influence", expResults.episodicInfluence);
        
        results << "3. Episodic Memory:" << std::endl;
        results << "   Recall Accuracy: " << expResults.episodicRecallAccuracy << std::endl;
        results << "   Episodes Stored: " << expResults.episodesStored << std::endl;
        results << "   Episodic Influence: " << expResults.episodicInfluence << std::endl;
    }
    
    // Run Concept Formation Experiment
    printSection("4. Concept Formation");
    {
        ConceptFormationExperiment experiment;
        Phase4Results expResults = experiment.run(brain.get(), numTrials);
        
        printResult("Concepts Formed", expResults.conceptsFormed);
        printResult("Concept Stability", expResults.conceptStability);
        printResult("Generalization Ability", expResults.generalizationAbility);
        
        if (expResults.conceptsFormed > 0) {
            printSuccess("Concepts emerged from repeated experience!");
        }
        
        results << "4. Concept Formation:" << std::endl;
        results << "   Concepts Formed: " << expResults.conceptsFormed << std::endl;
        results << "   Concept Stability: " << expResults.conceptStability << std::endl;
        results << "   Generalization Ability: " << expResults.generalizationAbility << std::endl;
    }
    
    // Run Attention Experiment
    printSection("5. Neural Attention");
    {
        AttentionExperiment experiment;
        Phase4Results expResults = experiment.run(brain.get(), numTrials);
        
        printResult("Attention Selectivity", expResults.attentionSelectivity);
        printResult("Distraction Resistance", expResults.distractionResistance);
        
        if (expResults.attentionSelectivity > 0.5f) {
            printSuccess("Selective attention emerged!");
        }
        
        results << "5. Neural Attention:" << std::endl;
        results << "   Attention Selectivity: " << expResults.attentionSelectivity << std::endl;
        results << "   Distraction Resistance: " << expResults.distractionResistance << std::endl;
    }
    
    // Run Planning Experiment
    printSection("6. Multi-Step Planning");
    {
        PlanningExperiment experiment;
        Phase4Results expResults = experiment.run(brain.get(), numTrials / 2);
        
        printResult("Planning Accuracy", expResults.planningAccuracy);
        printResult("Planning Confidence", expResults.planningConfidence);
        
        if (expResults.planningAccuracy > 0.4f) {
            printSuccess("Planning ability demonstrated!");
        }
        
        results << "6. Multi-Step Planning:" << std::endl;
        results << "   Planning Accuracy: " << expResults.planningAccuracy << std::endl;
        results << "   Planning Confidence: " << expResults.planningConfidence << std::endl;
    }
    
    // Run Self-Model Experiment
    printSection("7. Self-Model");
    {
        SelfModelExperiment experiment;
        Phase4Results expResults = experiment.run(brain.get(), numTrials);
        
        printResult("Self-Prediction Accuracy", expResults.selfPredictionAccuracy);
        printResult("Body Awareness", expResults.bodyAwareness);
        
        if (expResults.selfPredictionAccuracy > 0.3f) {
            printSuccess("Self-model developing!");
        }
        
        results << "7. Self-Model:" << std::endl;
        results << "   Self-Prediction Accuracy: " << expResults.selfPredictionAccuracy << std::endl;
        results << "   Body Awareness: " << expResults.bodyAwareness << std::endl;
    }
    
    // Run Social Learning Experiment
    printSection("8. Social Learning");
    {
        SocialLearningExperiment experiment;
        Phase4Results expResults = experiment.run(brain.get(), numTrials / 2);
        
        printResult("Imitation Accuracy", expResults.imitationAccuracy);
        printResult("Observations from Others", expResults.observationsFromOthers);
        
        if (expResults.imitationAccuracy > 0.2f) {
            printSuccess("Social learning observed!");
        }
        
        results << "8. Social Learning:" << std::endl;
        results << "   Imitation Accuracy: " << expResults.imitationAccuracy << std::endl;
        results << "   Observations from Others: " << expResults.observationsFromOthers << std::endl;
    }
    
    // Run Continual Learning Experiment
    printSection("9. Continual Learning");
    {
        ContinualLearningExperiment experiment;
        Phase4Results expResults = experiment.run(brain.get(), numTrials);
        
        printResult("Behavior Improvement", expResults.behaviorImprovement);
        printResult("Transfer Performance", expResults.transferPerformance);
        
        results << "9. Continual Learning:" << std::endl;
        results << "   Behavior Improvement: " << expResults.behaviorImprovement << std::endl;
        results << "   Transfer Performance: " << expResults.transferPerformance << std::endl;
    }
    
    // Run Integrated Experiment
    printSection("10. Integrated Phase 4");
    {
        Phase4IntegratedExperiment experiment;
        Phase4Results expResults = experiment.run(brain.get(), 10, 50);
        
        printResult("Total Reward", expResults.totalReward);
        printResult("Episodes Stored", expResults.episodesStored);
        printResult("Concepts Formed", expResults.conceptsFormed);
        printResult("Planning Confidence", expResults.planningConfidence);
        printResult("Body Awareness", expResults.bodyAwareness);
        
        results << "10. Integrated Phase 4:" << std::endl;
        results << "   Total Reward: " << expResults.totalReward << std::endl;
        results << "   Episodes Stored: " << expResults.episodesStored << std::endl;
        results << "   Concepts Formed: " << expResults.conceptsFormed << std::endl;
        results << "   Planning Confidence: " << expResults.planningConfidence << std::endl;
        results << "   Body Awareness: " << expResults.bodyAwareness << std::endl;
    }
    
    // Final summary
    printHeader("PHASE 4 SUMMARY");
    
    std::cout << std::endl;
    std::cout << "Phase 4 demonstrates:" << std::endl;
    std::cout << "  - Temporal prediction: Learning relationships between events" << std::endl;
    std::cout << "  - Working memory: Maintaining information across delays" << std::endl;
    std::cout << "  - Episodic memory: Storing and recalling experiences" << std::endl;
    std::cout << "  - Concept formation: Discovering patterns without labels" << std::endl;
    std::cout << "  - Neural attention: Selective processing through competition" << std::endl;
    std::cout << "  - Multi-step planning: Using predictions to select actions" << std::endl;
    std::cout << "  - Self-model: Learning body schema and action consequences" << std::endl;
    std::cout << "  - Social learning: Observing and imitating others" << std::endl;
    std::cout << "  - Continual learning: Adapting to new tasks" << std::endl;
    
    std::cout << std::endl;
    std::cout << "All mechanisms emerge from neural dynamics and plasticity." << std::endl;
    std::cout << "No predefined concepts, rules, or symbolic AI were used." << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== EXPERIMENT RESULTS ===" << std::endl;
    std::cout << results.str();
    
    // Save results to file if requested
    if (!cliOptions.outputFile.empty()) {
        CommandLineInterface::saveResultsToFile(cliOptions.outputFile, results.str());
    }
    
    std::cout << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "Phase 4 Complete!" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    return 0;
}
