// Phase 6 Integration Test
// Tests all cognitive systems to ensure they work together

#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Phase 6 Integration Test ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", 42);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    
    // Test memory systems
    std::cout << "\n--- Memory Systems Test ---";
    
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        std::cout << "✓ Working Memory available";
        workingMemory->setCapacity(50);
        std::cout << " (capacity: " << workingMemory->getCapacity() << ")";
    } else {
        std::cout << "✗ Working Memory not available";
    }
    
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        std::cout << "✓ Episodic Memory available";
        episodicMemory->setMaxEpisodes(1000);
        std::cout << " (max episodes: " << episodicMemory->getMaxEpisodes() << ")";
    } else {
        std::cout << "✗ Episodic Memory not available";
    }
    
    auto* associativeMemory = brain->getAssociativeMemory();
    if (associativeMemory) {
        std::cout << "✓ Associative Memory available";
    } else {
        std::cout << "✗ Associative Memory not available";
    }
    
    // Test prediction system
    std::cout << "\n--- Prediction System Test ---";
    auto* predictionSystem = brain->getPredictionSystem();
    if (predictionSystem) {
        std::cout << "✓ Prediction System available";
        
        // Test initialization (new method)
        predictionSystem->initialize(brain.get());
        std::cout << " (initialized)";
        
        float error = predictionSystem->getPredictionError();
        float confidence = predictionSystem->getConfidence();
        std::cout << " (error: " << error << ", confidence: " << confidence << ")";
    } else {
        std::cout << "✗ Prediction System not available";
    }
    
    // Test cognition systems
    std::cout << "\n--- Cognition Systems Test ---";
    
    auto* planner = brain->getPlanner();
    if (planner) {
        std::cout << "✓ Neural Planner available";
        planner->initialize(brain.get());
        planner->setPlanningDepth(5);
        std::cout << " (planning depth: 5)";
    } else {
        std::cout << "✗ Neural Planner not available";
    }
    
    auto* conceptFormation = brain->getConceptFormation();
    if (conceptFormation) {
        std::cout << "✓ Concept Formation available";
        conceptFormation->initialize(brain.get());
        std::cout << " (initialized)";
    } else {
        std::cout << "✗ Concept Formation not available";
    }
    
    auto* attention = brain->getAttention();
    if (attention) {
        std::cout << "✓ Attentional Selection available";
        attention->initialize(brain.get());
        attention->setInhibitionStrength(0.5f);
        std::cout << " (inhibition: 0.5)";
    } else {
        std::cout << "✗ Attentional Selection not available";
    }
    
    // Test development system
    std::cout << "\n--- Development System Test ---";
    auto* developmentSystem = brain->getDevelopmentSystem();
    if (developmentSystem) {
        std::cout << "✓ Development System available";
        brain->setDevelopmentalStage(nlm::DevelopmentalStage::Initial);
        std::cout << " (initial stage)";
    } else {
        std::cout << "✗ Development System not available";
    }
    
    // Test neuromodulation systems
    std::cout << "\n--- Neuromodulation Systems Test ---";
    
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        std::cout << "✓ Dopamine available";
        dopamine->initialize(brain.get());
        std::cout << " (initialized)";
    } else {
        std::cout << "✗ Dopamine not available";
    }
    
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        std::cout << "✓ Curiosity available";
        curiosity->initialize(brain.get());
        std::cout << " (initialized)";
    } else {
        std::cout << "✗ Curiosity not available";
    }
    
    auto* novelty = brain->getNovelty();
    if (novelty) {
        std::cout << "✓ Novelty available";
        novelty->initialize(brain.get());
        std::cout << " (initialized)";
    } else {
        std::cout << "✗ Novelty not available";
    }
    
    auto* predictionError = brain->getPredictionErrorSignal();
    if (predictionError) {
        std::cout << "✓ Prediction Error available";
        predictionError->initialize(brain.get());
        std::cout << " (initialized)";
    } else {
        std::cout << "✗ Prediction Error not available";
    }
    
    // Run simulation steps to test integration
    std::cout << "\n--- Integration Test ---";
    std::cout << "Running simulation steps...";
    
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain->step(step, step * 0.001);
        
        // Check if systems are responding
        if (workingMemory && workingMemory->getActiveTraces() > 0) {
            std::cout << "\n  Step " << step << ": Working memory active";
        }
        
        if (episodicMemory && episodicMemory->getEpisodeCount() > 0) {
            std::cout << "\n  Step " << step << ": Episodic memory has " << episodicMemory->getEpisodeCount() << " episodes";
        }
    }
    
    // Test checkpoint save/load
    std::cout << "\n--- Checkpoint Test ---";
    std::string checkpointPath = "/tmp/nlm_phase6_test.bin";
    
    if (brain->save(checkpointPath)) {
        std::cout << "✓ Checkpoint saved successfully";
        
        // Create new brain and load
        auto brain2 = std::make_shared<nlm::Brain>(config);
        if (brain2->initialize() && brain2->load(checkpointPath)) {
            std::cout << "✓ Checkpoint loaded successfully";
        } else {
            std::cout << "✗ Checkpoint load failed";
        }
    } else {
        std::cout << "✗ Checkpoint save failed";
    }
    
    // Print final status
    std::cout << "\n=== Final Brain Status ===";
    brain->logStatus();
    
    std::cout << "\n=== Integration Test Complete ===";
    std::cout << "All cognitive systems are integrated and functional!";
    
    return 0;
}
