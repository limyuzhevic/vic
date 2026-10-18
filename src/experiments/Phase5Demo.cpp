#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase5Experiment.hpp"

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

int main(int argc, char* argv[]) {
    std::cout << "NLM Phase 5: Lifetime Learning Experiments\n";
    std::cout << "========================================\n\n";
    
    // Parse command line arguments
    size_t numTrials = 20;
    if (argc > 1) {
        numTrials = std::stoi(argv[1]);
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
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    
    // Create brain
    std::cout << "Initializing brain...\n";
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!\n";
        return 1;
    }
    
    printHeader("PHASE 5 EXPERIMENTS");
    
    // Run Early Development Experiment
    printSection("1. Early Development");
    {
        EarlyDevelopmentExperiment experiment;
        Phase5Results results = experiment.run(brain.get(), 5000);
        
        printResult("Synapse Formation Rate", results.synapseFormationRate);
        printResult("Initial Connectivity", results.initialConnectivity);
        printResult("Development Efficiency", results.developmentEfficiency);
    }
    
    // Reset brain for next experiment
    brain->reset();
    brain->initialize();
    
    // Run Sensory Development Experiment
    printSection("2. Sensory Development");
    {
        SensoryDevelopmentExperiment experiment;
        Phase5Results results = experiment.run(brain.get(), 3000);
        
        printResult("Sensory Response Strength", results.sensoryResponseStrength);
        printResult("Feature Extraction Accuracy", results.featureExtractionAccuracy);
    }
    
    brain->reset();
    brain->initialize();
    
    // Run Motor Development Experiment
    printSection("3. Motor Development");
    {
        MotorDevelopmentExperiment experiment;
        Phase5Results results = experiment.run(brain.get(), 3000);
        
        printResult("Motor Coordination", results.motorCoordination);
        printResult("Action Refinement Ability", results.actionRefinementAbility);
    }
    
    brain->reset();
    brain->initialize();
    
    // Run Associative Development Experiment
    printSection("4. Associative Development");
    {
        AssociativeDevelopmentExperiment experiment;
        Phase5Results results = experiment.run(brain.get(), 4000);
        
        printResult("Associative Strength", results.associativeStrength);
        printResult("Pattern Completion", results.patternCompletion);
    }
    
    brain->reset();
    brain->initialize();
    
    // Run Memory Consolidation Experiment
    printSection("5. Memory Consolidation");
    {
        MemoryConsolidationExperiment experiment;
        Phase5Results results = experiment.run(brain.get(), 5000);
        
        printResult("Memory Retention", results.memoryRetention);
        printResult("Consolidation Efficiency", results.consolidationEfficiency);
    }
    
    brain->reset();
    brain->initialize();
    
    // Run Social Development Experiment
    printSection("6. Social Development");
    {
        SocialDevelopmentExperiment experiment;
        Phase5Results results = experiment.run(brain.get(), 4000);
        
        printResult("Social Learning Rate", results.socialLearningRate);
        printResult("Cooperation Level", results.cooperationLevel);
    }
    
    brain->reset();
    brain->initialize();
    
    // Run Continual Learning Experiment
    printSection("7. Continual Learning");
    {
        ContinualLearningExperiment experiment;
        Phase5Results results = experiment.run(brain.get(), 6000);
        
        printResult("Learning Rate", results.learningRate);
        printResult("Transfer Performance", results.transferPerformance);
        printResult("Catastrophic Forgetting", results.catastrophicForgetting);
    }
    
    brain->reset();
    brain->initialize();
    
    // Run Damage Recovery Experiment
    printSection("8. Damage Recovery");
    {
        DamageRecoveryExperiment experiment;
        Phase5Results results = experiment.run(brain.get(), 50000, 0.2f, "sensory_region");
        
        printResult("Recovery Speed", results.recoverySpeed);
        printResult("Functional Recovery", results.functionalRecovery);
    }
    
    brain->reset();
    brain->initialize();
    
    // Run Multi-Seed Experiment
    printSection("9. Multi-Seed Reliability");
    {
        MultiSeedExperimentRunner runner;
        auto results = runner.runWithMultipleSeeds(5, config);
        
        printResult("Mean Performance", results.meanReward);
        printResult("Performance Stability", 1.0f - results.coefficientOfVariation);
    }
    
    // Final summary
    printHeader("PHASE 5 SUMMARY");
    
    std::cout << "\nPhase 5 investigates:\n";
    std::cout << "  - Early developmental stages and synapse formation\n";
    std::cout << "  - Sensory systems and feature extraction\n";
    std::cout << "  - Motor systems and action coordination\n";
    std::cout << "  - Associative learning and pattern completion\n";
    std::cout << "  - Memory consolidation and retention\n";
    std::cout << "  - Social learning and cooperation\n";
    std::cout << "  - Continual learning across tasks\n";
    std::cout << "  - Recovery from neural damage\n";
    std::cout << "  - Reliability across random seeds\n";
    
    std::cout << "\nAll mechanisms emerge from neural dynamics and developmental plasticity.\n";
    std::cout << "Brains develop capabilities through interaction with environment.\n";
    
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "Phase 5 Complete!\n";
    std::cout << std::string(60, '=') << "\n";
    
    return 0;
}
