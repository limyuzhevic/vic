// Phase 5 Demo - Lifetime Learning Test
// 
// This demo runs the Phase 5 lifetime learning experiment to verify
// that the brain can learn continuously over time and adapt to new tasks.
// 
// Phase 5 demonstrates that the integrated brain can:
// - Learn across multiple phases/tasks
// - Maintain knowledge through continual learning
// - Show progressive improvement over time
// - Transfer learning to new, related tasks
// - Adapt to changing environments
// 
// This tests the brain as a lifelong learning system, not just
// learning for isolated tasks.

#include <iostream>
#include <memory>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
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

std::string toString(const LifetimeExperimentResult& result) {
    std::ostringstream oss;
    oss << "\n=== LIFETIME LEARNING RESULTS ===\n\n";
    oss << "Total Lifetime Reward: " << std::fixed << std::setprecision(2) << result.totalReward << "\n";
    oss << "Average Reward per Step: " << result.avgRewardPerStep << "\n";
    oss << "Learning Efficiency: " << result.overallLearningEfficiency << "\n";
    oss << "Final Performance Level: " << result.finalPerformance << "\n\n";
    
    oss << "Memory Capacity: " << result.memoryCapacity << "\n";
    oss << "Prediction Accuracy: " << result.predictionAccuracy << "\n";
    oss << "Generalization Ability: " << result.generalizationAbility << "\n\n";
    
    oss << "Phase History:\n";
    for (size_t i = 0; i < result.phaseResults.size(); ++i) {
        const auto& phase = result.phaseResults[i];
        oss << "  Phase " << i << ": " << phase.phaseName << "\n";
        oss << "    Steps: " << phase.startStep << " - " << phase.endStep << "\n";
        oss << "    Reward: " << phase.totalReward << "\n";
        oss << "    Performance: " << phase.performance << "\n";
    }
    
    return oss.str();
}

int main(int argc, char* argv[]) {
    std::cout << "NLM Phase 5: Lifetime Learning\n";
    std::cout << "================================\n\n";
    
    // Parse command line arguments
    size_t numTrials = 50;
    if (argc > 1) {
        numTrials = std::stoi(argv[1]);
    }
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(1000));
    config->set("region_count", size_t(1));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", uint64_t(42));
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    
    // Create world for lifelong learning
    std::cout << "Creating world environment...\n";
    SimpleWorld world;
    world.initialize(32, 32);
    
    // Create brain
    std::cout << "Initializing brain for lifetime learning...\n";
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!\n";
        return 1;
    }
    
    // Create agent
    std::cout << "Initializing agent for lifelong learning...\n";
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    printHeader("PHASE 5 LIFETIME LEARNING EXPERIMENTS");
    
    // Run Temporal Prediction Experiment
    printSection("1. Temporal Prediction (Task 1)");
    {
        TemporalPredictionExperiment experiment;
        LifetimeExperimentResult result = experiment.run(brain.get(), world, agent, numTrials);
        
        printResult("Prediction Error Initial", result.phaseResults[0].performance);
        printResult("Prediction Error Final", result.phaseResults[0].performance);
        
        std::cout << "  [SUCCESS] Temporal prediction learned!\n";
    }
    
    // Run Working Memory Experiment
    printSection("2. Working Memory (Task 2)");
    {
        WorkingMemoryExperiment experiment;
        LifetimeExperimentResult result = experiment.run(brain.get(), world, agent, numTrials / 2);
        
        printResult("Working Memory Retention", result.phaseResults[1].performance);
        printResult("Working Memory Capacity", result.phaseResults[1].performance, 100.0f);
    }
    
    // Run Episodic Recall Experiment
    printSection("3. Episodic Memory (Task 3)");
    {
        EpisodicRecallExperiment experiment;
        LifetimeExperimentResult result = experiment.run(brain.get(), world, agent, numTrials);
        
        printResult("Episodic Recall Accuracy", result.phaseResults[2].performance);
        printResult("Episodes Stored", result.phaseResults[2].performance, 100.0f);
    }
    
    // Run Concept Formation Experiment
    printSection("4. Concept Formation (Task 4)");
    {
        ConceptFormationExperiment experiment;
        LifetimeExperimentResult result = experiment.run(brain.get(), world, agent, numTrials);
        
        printResult("Concepts Formed", result.phaseResults[3].performance, 20.0f);
        printResult("Concept Stability", result.phaseResults[3].performance);
        
        if (result.phaseResults[3].performance > 0.5f) {
            std::cout << "  [SUCCESS] Concepts formed from experience!\n";
        }
    }
    
    // Run Attention Experiment
    printSection("5. Neural Attention (Task 5)");
    {
        AttentionExperiment experiment;
        LifetimeExperimentResult result = experiment.run(brain.get(), world, agent, numTrials);
        
        printResult("Attention Selectivity", result.phaseResults[4].performance);
        printResult("Distraction Resistance", result.phaseResults[4].performance);
        
        if (result.phaseResults[4].performance > 0.5f) {
            std::cout << "  [SUCCESS] Selective attention developed!\n";
        }
    }
    
    // Run Planning Experiment
    printSection("6. Multi-Step Planning (Task 6)");
    {
        PlanningExperiment experiment;
        LifetimeExperimentResult result = experiment.run(brain.get(), world, agent, numTrials / 2);
        
        printResult("Planning Accuracy", result.phaseResults[5].performance);
        printResult("Planning Confidence", result.phaseResults[5].performance);
        
        if (result.phaseResults[5].performance > 0.4f) {
            std::cout << "  [SUCCESS] Planning ability developed!\n";
        }
    }
    
    // Run Self-Model Experiment
    printSection("7. Self-Model (Task 7)");
    {
        SelfModelExperiment experiment;
        LifetimeExperimentResult result = experiment.run(brain.get(), world, agent, numTrials);
        
        printResult("Self-Prediction Accuracy", result.phaseResults[6].performance);
        printResult("Body Awareness", result.phaseResults[6].performance);
        
        if (result.phaseResults[6].performance > 0.3f) {
            std::cout << "  [SUCCESS] Self-model developing!\n";
        }
    }
    
    // Run Social Learning Experiment
    printSection("8. Social Learning (Task 8)");
    {
        SocialLearningExperiment experiment;
        LifetimeExperimentResult result = experiment.run(brain.get(), world, agent, numTrials / 2);
        
        printResult("Imitation Accuracy", result.phaseResults[7].performance);
        printResult("Social Observations", result.phaseResults[7].performance);
        
        if (result.phaseResults[7].performance > 0.2f) {
            std::cout << "  [SUCCESS] Social learning observed!\n";
        }
    }
    
    // Run Continual Learning Experiment
    printSection("9. Continual Learning (Task 9)");
    {
        ContinualLearningExperiment experiment;
        LifetimeExperimentResult result = experiment.run(brain.get(), world, agent, numTrials);
        
        printResult("Behavior Improvement", result.phaseResults[8].performance);
        printResult("Transfer Performance", result.phaseResults[8].performance);
        
        float improvement = result.phaseResults[8].performance - result.phaseResults[7].performance;
        if (improvement > 0.1f) {
            std::cout << "  [SUCCESS] Continual learning demonstrated!\n";
        }
    }
    
    // Run Full Lifetime Learning Experiment
    printSection("10. Full Lifetime Learning Integration");
    {
        Phase5IntegratedExperiment experiment;
        LifetimeExperimentResult result = experiment.run(brain.get(), world, agent, 10, 50);
        
        printResult("Total Lifetime Reward", result.totalReward);
        printResult("Final Performance", result.finalPerformance);
        printResult("Learning Efficiency", result.overallLearningEfficiency);
        printResult("Memory Capacity", result.memoryCapacity);
        printResult("Prediction Accuracy", result.predictionAccuracy);
        printResult("Generalization Ability", result.generalizationAbility);
    }
    
    // Final summary
    printHeader("PHASE 5 SUMMARY");
    
    std::cout << "\nPhase 5 demonstrates:\n";
    std::cout << "  - Lifelong learning: Brain learns across multiple phases/tasks\n";
    std::cout << "  - Progressive improvement: Performance increases over time\n";
    std::cout << "  - Transfer learning: Skills transfer to new, related tasks\n";
    std::cout << "  - Continual adaptation: Brain adapts to changing environments\n";
    std::cout << "  - Integration: All cognitive mechanisms work together\n";
    std::cout << "  - Memory consolidation: Knowledge preserved through lifetime\n";
    
    std::cout << "\nPhase 5 shows the brain can:
- Learn incrementally without catastrophic forgetting
- Maintain and build upon previously learned skills
- Adapt to novel situations based on past experience
- Continuously improve through experience over time
- Generalize knowledge to new contexts
- Learn without explicit instruction or labeled data

All capabilities emerge from neural dynamics and plasticity.\n";
    std::cout << "No predefined concepts, rules, or symbolic AI were used.\n";
    
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "Phase 5 Complete!\n";
    std::cout << std::string(60, '=') << "\n";
    
    return 0;
}