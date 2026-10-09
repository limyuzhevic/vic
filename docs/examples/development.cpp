/**
 * NLM Development System Example
 * 
 * This example demonstrates the developmental processes in NLM,
 * including developmental stages, structural plasticity, and
 * how development affects learning and behavior over time.
 */

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "development/DevelopmentSystem.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <chrono>

using namespace nlm;

void printDevelopmentTimeline(const std::vector<DevelopmentalStage>& stages,
                             const std::vector<float>& plasticityRates,
                             const std::vector<float>& learningRates) {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "  DEVELOPMENTAL TIMELINE\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    std::cout << "Stage\t\tDuration\tPlasticity\tLearning Rate\n";
    std::cout << "----\t\t--------\t----------\t-------------\n";
    
    for (size_t i = 0; i < stages.size(); ++i) {
        std::cout << static_cast<int>(stages[i]) << "\t\t";
        
        if (i < stages.size() - 1) {
            float duration = plasticityRates[i];
            std::cout << std::fixed << std::setprecision(1) << duration << "s\t\t";
        } else {
            std::cout << "--\t\t";
        }
        
        std::cout << std::setprecision(4) << plasticityRates[i] << "\t\t";
        std::cout << std::setprecision(5) << learningRates[i] << "\n";
    }
}

int main() {
    std::cout << "NLM Development System Example" << std::endl;
    std::cout << "============================\n";
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Warning);
    
    printSection("Setup");
    std::cout << "Configuring developmental system...\n";
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(600));
    config->set("region_count", size_t(1));
    config->set("enable_development", true);
    config->set("initial_stage", static_cast<int>(DevelopmentalStage::Initial));
    config->set("enable_structural_plasticity", true);
    
    printSection("Initialize Components");
    std::cout << "Creating brain and development system...\n";
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    printSection("Development Configuration");
    std::cout << "Setting up developmental stages and plasticity...\n";
    
    // Create development system
    auto developmentSystem = std::make_shared<DevelopmentSystem>(brain);
    
    // Create structural plasticity system
    auto structuralPlasticity = std::make_shared<StructuralPlasticity>(brain);
    
    printSection("Developmental Stage Analysis");
    std::cout << "Analyzing initial developmental state...\n";
    
    DevelopmentalStage currentStage = brain->getDevelopmentalStage();
    float plasticityRate = developmentSystem->getCurrentPlasticityRate();
    float learningRate = developmentSystem->getCurrentLearningRate();
    
    std::cout << "Initial developmental stage: " << static_cast<int>(currentStage) << std::endl;
    std::cout << "Initial plasticity rate: " << plasticityRate << std::endl;
    std::cout << "Initial learning rate: " << learningRate << std::endl;
    
    printSection("Development Timeline");
    std::cout << "Development characteristics:\n";
    
    // Simulate development through stages
    std::vector<DevelopmentalStage> stages;
    std::vector<float> plasticityRates;
    std::vector<float> learningRates;
    
    // Get developmental characteristics
    stages = developmentSystem->getDevelopmentalStages();
    plasticityRates = developmentSystem->getPlasticityRates();
    learningRates = developmentSystem->getLearningRates();
    
    printDevelopmentTimeline(stages, plasticityRates, learningRates);
    
    printSection("Developmental Experiment");
    std::cout << "Running developmental simulation...\n";
    std::cout << std::string(80, '-') << "\n";
    
    int totalSimulationSteps = 200;
    bool stageTransitioned = false;
    int stageTransitionStep = 0;
    
    for (int step = 0; step < totalSimulationSteps; ++step) {
        // Update development system
        brain->updateDevelopment(0.1f);
        
        // Check for stage transition
        DevelopmentalStage newStage = brain->getDevelopmentalStage();
        if (newStage != currentStage) {
            stageTransitioned = true;
            stageTransitionStep = step;
            currentStage = newStage;
            
            std::cout << "\nStage Transition at Step " << step << ": " 
                      << static_cast<int>(newStage) << "\n";
            std::cout << "  New plasticity rate: " << developmentSystem->getCurrentPlasticityRate() << "\n";
            std::cout << "  New learning rate: " << developmentSystem->getCurrentLearningRate() << "\n";
        }
        
        // Run brain step
        brain->step(step);
        
        // Periodic structural changes
        if (step % 40 == 0) {
            // Structural plasticity occurs at specific developmental stages
            if (currentStage == DevelopmentalStage::CriticalPeriod || 
                currentStage == DevelopmentalStage::Maturation) {
                
                bool addedSynapse = structuralPlasticity->growNewSynapse(step);
                if (addedSynapse) {
                    std::cout << "  Added new synapse at step " << step << "\n";
                }
                
                if (step >= 100) {
                    bool removedSynapse = structuralPlasticity->pruneSynapse(step);
                    if (removedSynapse) {
                        std::cout << "  Pruned synapse at step " << step << "\n";
                    }
                }
            }
        }
        
        // Print progress
        if (step % 50 == 0 || step == totalSimulationSteps - 1 || stageTransitioned) {
            float plasticity = developmentSystem->getCurrentPlasticityRate();
            float learning = developmentSystem->getCurrentLearningRate();
            int synapseCount = brain->getTotalSynapseCount();
            
            std::cout << "\nStep " << step << ": ";
            std::cout << "Stage: " << static_cast<int>(currentStage) << ", ";
            std::cout << "Plasticity: " << std::setprecision(4) << plasticity << ", ";
            std::cout << "Learning: " << std::setprecision(5) << learning << ", ";
            std::cout << "Synapses: " << synapseCount << "\n";
            
            stageTransitioned = false;
        }
    }
    
    printSection("Final Development Analysis");
    std::cout << std::string(80, '-') << "\n";
    
    // Get final development statistics
    float finalPlasticity = developmentSystem->getCurrentPlasticityRate();
    float finalLearning = developmentSystem->getCurrentLearningRate();
    int finalSynapses = brain->getTotalSynapseCount();
    int finalNeurons = brain->getTotalNeuronCount();
    int totalSpikes = brain->getTotalSpikeCount();
    
    std::cout << "Final developmental state:\n";
    std::cout << "  Final developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
    std::cout << "  Final plasticity rate: " << finalPlasticity << "\n";
    std::cout << "  Final learning rate: " << finalLearning << "\n";
    std::cout << "  Final synapse count: " << finalSynapses << "\n";
    std::cout << "  Final neuron count: " << finalNeurons << "\n";
    std::cout << "  Total spikes generated: " << totalSpikes << "\n\n";
    
    // Analysis of development effects
    std::cout << "Development effects analysis:\n";
    
    // Compare initial vs final
    float initialPlasticity = 1.0f; // Would get from actual initial state
    float plasticityChange = (finalPlasticity - initialPlasticity) / initialPlasticity * 100;
    
    std::cout << "  Plasticity change: " << std::setprecision(1) << plasticityChange << "%\n";
    std::cout << "  Synapse growth: " << (finalSynapses - 1000) << " synapses\n";
    std::cout << "  Learning efficiency: " << (finalLearning * 100000) << " (scaled)\n";
    
    printSection("Development Summary");
    std::cout << "\nDevelopmental processes demonstrated:\n";
    std::cout << "  • Age-dependent plasticity rates\n";
    std::cout << "  • Critical period for structural changes\n";
    std::cout << "  • Activity-dependent synapse formation\n";
    std::cout << "  • Synaptic pruning with age\n";
    std::cout << "  • Developmental stage modulation of learning\n";
    std::cout << "  • Integration of development with plasticity\n";
    
    return 0;
}
