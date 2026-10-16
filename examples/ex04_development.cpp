// ex04_development.cpp - Developmental Processes

#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <chrono>

int main() {
    // Initialize logging
    nlm::Logger::getInstance().setLevel(nlm::LogLevel::INFO);
    
    std::cout << "=== Example 4: Developmental Processes ===" << std::endl;
    
    // Create configuration with development
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.maxNeurons", 100);
    config->set("brain.development.enabled", true);
    config->set("brain.development.plasticityDecayRate", 0.05);
    config->set("brain.development.maturationRate", 0.1);
    config->set("brain.development.synaptogenesisRate", 0.02);
    config->set("brain.development.pruningRate", 0.01);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Development brain initialized!" << std::endl;
    
    // Get development system
    auto developmentSystem = brain->getDevelopmentSystem();
    
    std::cout << "\n--- Developmental Simulation ---" << std::endl;
    double developmentalAge = 0.0;
    const double lifespan = 10.0; // years in simulation
    const double timestep = 0.1;
    
    for (nlm::SimulationStep step = 0; step < 500; step++) {
        double currentTime = step * timestep;
        
        // Update developmental age
        developmentalAge = currentTime / lifespan;
        
        // Set developmental stage based on age
        nlm::DevelopmentalStage stage;
        if (developmentalAge < 0.3) {
            stage = nlm::DevelopmentalStage::JUVENILE;
        } else if (developmentalAge < 0.7) {
            stage = nlm::DevelopmentalStage::ADULT;
        } else {
            stage = nlm::DevelopmentalStage::AGING;
        }
        
        brain->setDevelopmentalStage(stage);
        
        // Step brain
        brain->step(step, currentTime);
        
        // Monitor development
        if (step % 50 == 0) {
            double plasticityMultiplier = developmentSystem->getPlasticityMultiplier();
            std::cout << "Step " << step << " (Time: " << currentTime << "s): "
                     << "Age: " << developmentalAge * 100 << "%, "
                     << "Stage: " << static_cast<int>(stage) << ", "
                     << "Plasticity: " << plasticityMultiplier << ", "
                     << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
        }
        
        // Inject learning signals at different developmental stages
        if (step % 25 == 0) {
            // Novelty-driven learning in juvenile stage
            if (stage == nlm::DevelopmentalStage::JUVENILE) {
                float novelty = std::min(1.0f, developmentalAge * 3.0f);
                auto curiosity = brain->getCuriosity();
                if (curiosity) {
                    curiosity->processNovelty(novelty, 0.5f);
                }
            }
            
            // Reward-based learning in adult stage
            if (stage == nlm::DevelopmentalStage::ADULT) {
                float reward = 0.5f + 0.5f * std::sin(currentTime * 0.1f);
                auto dopamine = brain->getDopamine();
                if (dopamine) {
                    dopamine->processReward(reward, 0.0f);
                }
            }
            
            // Reduced learning in aging stage
            if (stage == nlm::DevelopmentalStage::AGING) {
                float decay = 1.0f - developmentalAge * 0.5f;
                std::cout << "  Developmental decay: " << decay << std::endl;
            }
        }
    }
    
    // Demonstrate developmental stage transitions
    std::cout << "\n--- Developmental Stage Analysis ---" << std::endl;
    for (double age = 0.0; age <= 1.0; age += 0.1) {
        nlm::DevelopmentalStage stage;
        if (age < 0.3) {
            stage = nlm::DevelopmentalStage::Initial;
        } else if (age < 0.5) {
            stage = nlm::DevelopmentalStage::JUVENILE;
        } else if (age < 0.7) {
            stage = nlm::DevelopmentalStage::CriticalPeriod;
        } else if (age < 0.9) {
            stage = nlm::DevelopmentalStage::ADULT;
        } else {
            stage = nlm::DevelopmentalStage::AGING;
        }
        
        double plasticity = 1.0f;
        if (stage == nlm::DevelopmentalStage::Initial) plasticity = 1.0f;
        else if (stage == nlm::DevelopmentalStage::JUVENILE) plasticity = 0.8f;
        else if (stage == nlm::DevelopmentalStage::CriticalPeriod) plasticity = 0.6f;
        else if (stage == nlm::DevelopmentalStage::Adult) plasticity = 0.3f;
        else if (stage == nlm::DevelopmentalStage::Maturation) plasticity = 0.2f;
        else if (stage == nlm::DevelopmentalStage::Aging) plasticity = 0.1f;
        
        std::cout << "Age " << age * 100 << "% (Stage " << static_cast<int>(stage) << "): Plasticity = " 
                 << plasticity << std::endl;
    }
    
    // Demonstrate structural plasticity changes with development
    std::cout << "\n--- Structural Plasticity Development ---" << std::endl;
    auto structuralPlasticity = brain->getStructuralPlasticity();
    if (structuralPlasticity) {
        // Get current rates
        float synRate = structuralPlasticity->getSynaptogenesisRate();
        float pruneRate = structuralPlasticity->getPruningRate();
        
        std::cout << "Synaptogenesis rate: " << synRate << std::endl;
        std::cout << "Pruning rate: " << pruneRate << std::endl;
        
        // Simulate developmental changes in rates
        std::cout << "\nDevelopment effects on plasticity:" << std::endl;
        std::cout << "- High synaptogenesis in early development" << std::endl;
        std::cout << "- Increased pruning during maturation" << std::endl;
        std::cout << "- Balanced rates in adulthood" << std::endl;
        std::cout << "- Reduced plasticity in aging" << std::endl;
    }
    
    // Demonstrate memory development
    std::cout << "\n--- Memory System Development ---" << std::endl;
    auto workingMemory = brain->getWorkingMemory();
    auto episodicMemory = brain->getEpisodicMemory();
    
    if (workingMemory && episodicMemory) {
        // Working memory capacity may change with development
        std::cout << "Working memory capacity: " << workingMemory->getCapacity() << std::endl;
        
        // Episodic memory consolidation efficiency
        std::cout << "Episodic memory consolidation: Enabled" << std::endl;
        std::cout << "Memory replay: Available" << std::endl;
    }
    
    std::cout << "\n=== Example 4 Complete ===" << std::endl;
    std::cout << "Development simulation completed over " << lifespan << " years" << std::endl;
    return 0;
}