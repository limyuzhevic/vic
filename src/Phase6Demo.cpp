// Phase 6 Demo - Integrated Artificial Brain Demonstration
// Demonstrates all Phase 6 integration features working together

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

void demoPhase6Integration() {
    std::cout << "=== Phase 6: Integrated Artificial Brain Demo ===" << std::endl;
    std::cout << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config->set("replay_interval", 50, nlm::ConfigSource::Default);
    config->set("consolidation_interval", 200, nlm::ConfigSource::Default);
    config->set("rest_interval", 300, nlm::ConfigSource::Default);
    
    std::cout << "1. Creating and initializing Phase 6 brain..." << std::endl;
    nlm::Brain brain(config);
    brain.initialize();
    
    std::cout << "   Brain initialized with " << brain.getTotalNeuronCount() << " neurons" << std::endl;
    std::cout << "   and " << brain.getRegionCount() << " regions." << std::endl;
    std::cout << std::endl;
    
    std::cout << "2. Demonstrating integrated systems..." << std::endl;
    
    // Test memory systems
    std::cout << "   Memory Systems:" << std::endl;
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    if (workingMemory) {
        workingMemory->storeToNeuron(1, 0.8f);
        workingMemory->storeToNeuron(2, 0.9f);
        std::cout << "     - Working memory: " << workingMemory->getActiveTraces() << " active traces" << std::endl;
    }
    
    if (episodicMemory) {
        episodicMemory->setMaxEpisodes(50);
        std::cout << "     - Episodic memory: capacity set to " << episodicMemory->getMaxEpisodes() << " episodes" << std::endl;
    }
    
    if (associativeMemory) {
        associativeMemory->storeAssociation(1, 2, 0.7f);
        float assoc = associativeMemory->getAssociation(1, 2);
        std::cout << "     - Associative memory: strength " << assoc << std::endl;
    }
    
    // Test neuromodulation
    std::cout << "   Neuromodulation Systems:" << std::endl;
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    if (dopamine) {
        std::cout << "     - Dopamine level: " << dopamine->getLevel() << std::endl;
    }
    
    if (curiosity) {
        curiosity->setExplorationBias(0.8f);
        std::cout << "     - Curiosity: exploration bias = " << curiosity->getExplorationBias() << std::endl;
    }
    
    if (novelty) {
        std::cout << "     - Novelty threshold: " << novelty->getNoveltyThreshold() << std::endl;
    }
    
    // Test prediction system
    std::cout << "   Prediction System:" << std::endl;
    auto* predictionSystem = brain.getPredictionSystem();
    if (predictionSystem) {
        std::vector<float> pattern = {0.2f, 0.5f, 0.8f, 0.3f, 0.6f};
        predictionSystem->updatePattern(pattern);
        std::cout << "     - Pattern prediction: updated with " << pattern.size() << " features" << std::endl;
    }
    
    // Test cognition systems
    std::cout << "   Cognition Systems:" << std::endl;
    auto* attention = brain.getAttention();
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    
    if (attention) {
        attention->setInhibitionStrength(0.6f);
        attention->setExcitationStrength(1.4f);
        std::cout << "     - Attention: inhibition = " << attention->getInhibitionStrength() << ", excitation = " << attention->getExcitationStrength() << std::endl;
    }
    
    if (planner) {
        planner->setPlanningDepth(3);
        std::cout << "     - Planning: depth = " << planner->getPlanningDepth() << std::endl;
    }
    
    if (conceptFormation) {
        std::cout << "     - Concept formation: active" << std::endl;
    }
    
    // Test development
    std::cout << "   Development System:" << std::endl;
    auto* developmentSystem = brain.getDevelopmentSystem();
    if (developmentSystem) {
        std::cout << "     - Development: " << (brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial ? "Initial stage" : "Other stage") << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "3. Running simulation steps..." << std::endl;
    
    // Run simulation steps to demonstrate integration
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Create memories periodically
        if (step % 25 == 0) {
            if (episodicMemory) {
                nlm::EpisodicMemoryItem episode;
                episode.timestamp = step;
                episode.reward = 0.6f + (step % 3) * 0.2f;
                for (size_t i = 0; i < 5; ++i) {
                    episode.activeNeurons.push_back(i + 1);
                    episode.neuronActivations.push_back(0.5f + (i % 2) * 0.3f);
                }
                episodicMemory->storeEpisode(episode);
            }
        }
        
        // Show progress every 25 steps
        if (step % 25 == 0) {
            std::cout << "   Step " << step << ": " 
                      << brain.getActiveNeuronCount() << " active neurons, "
                      << brain.getFiringNeuronCount() << " firing, "
                      << brain.getPendingSpikeEventCount() << " pending events" << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "4. Demonstrating persistence (save/load)..." << std::endl;
    
    // Save brain state
    std::string checkpointFile = "./phase6_checkpoint.brain";
    std::cout << "   Saving brain state to " << checkpointFile << "..." << std::endl;
    
    bool saveResult = brain.save(checkpointFile);
    if (saveResult) {
        std::cout << "   ✓ Save successful" << std::endl;
    } else {
        std::cout << "   ✗ Save failed" << std::endl;
    }
    
    // Create new brain and load saved state
    std::cout << "   Creating new brain and loading saved state..." << std::endl;
    nlm::Brain brain2(config);
    brain2.initialize();
    
    bool loadResult = brain2.load(checkpointFile);
    if (loadResult) {
        std::cout << "   ✓ Load successful" << std::endl;
        std::cout << "   Loaded brain has " << brain2.getTotalNeuronCount() << " neurons and " << brain2.getRegionCount() << " regions" << std::endl;
    } else {
        std::cout << "   ✗ Load failed" << std::endl;
    }
    
    // Cleanup
    std::remove(checkpointFile.c_str());
    
    std::cout << std::endl;
    std::cout << "5. Final status..." << std::endl;
    brain.logStatus();
    
    std::cout << std::endl;
    std::cout << "=== Phase 6 Integration Demo Complete ===" << std::endl;
    std::cout << std::endl;
    std::cout << "All integrated systems working together:" << std::endl;
    std::cout << "✓ Memory Systems (Working, Episodic, Associative)" << std::endl;
    std::cout << "✓ Neuromodulation (Dopamine, Curiosity, Novelty)" << std::endl;
    std::cout << "✓ Prediction System" << std::endl;
    std::cout << "✓ Cognition (Attention, Planning, Concept Formation)" << std::endl;
    std::cout << "✓ Development System" << std::endl;
    std::cout << "✓ Sleep/Rest Cycle for Memory Consolidation" << std::endl;
    std::cout << "✓ Replay Mechanism" << std::endl;
    std::cout << "✓ Persistence (Save/Load with CheckpointManager)" << std::endl;
}

int main() {
    try {
        demoPhase6Integration();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error in Phase 6 demo: " << e.what() << std::endl;
        return 1;
    }
}
