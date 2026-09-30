#include <iostream>
#include <memory>
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"

int main() {
    std::cout << "=== NLM Neuromodulator Integration Test ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("random_seed", 42);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully!" << std::endl;
    
    // Test neuromodulator accessors
    std::cout << "\n=== Testing Neuromodulator Accessors ===" << std::endl;
    
    // Dopamine
    if (auto* dopamine = brain->getDopamine()) {
        std::cout << "✓ Dopamine accessible: Level = " << dopamine->getLevel() << std::endl;
        dopamine->setLevel(0.5f);
        std::cout << "  Set dopamine level to 0.5" << std::endl;
    } else {
        std::cerr << "✗ Dopamine not accessible!" << std::endl;
    }
    
    // Curiosity
    if (auto* curiosity = brain->getCuriosity()) {
        std::cout << "✓ Curiosity accessible: Level = " << curiosity->getLevel() << std::endl;
    } else {
        std::cerr << "✗ Curiosity not accessible!" << std::endl;
    }
    
    // Novelty
    if (auto* novelty = brain->getNovelty()) {
        std::cout << "✓ Novelty accessible: Level = " << novelty->getLevel() << std::endl;
    } else {
        std::cerr << "✗ Novelty not accessible!" << std::endl;
    }
    
    // NEW: Acetylcholine
    if (auto* acetylcholine = brain->getAcetylcholine()) {
        std::cout << "✓ Acetylcholine accessible: Level = " << acetylcholine->getLevel() << std::endl;
        acetylcholine->setLevel(0.7f);
        acetylcholine->setAChTone(0.6f);
        acetylcholine->modulateAttention(0.8f);
        acetylcholine->triggerMemoryConsolidation();
        std::cout << "  Set ACh level to 0.7, tone to 0.6, modulated attention, triggered consolidation" << std::endl;
    } else {
        std::cerr << "✗ Acetylcholine not accessible!" << std::endl;
    }
    
    // NEW: Norepinephrine
    if (auto* norepinephrine = brain->getNorepinephrine()) {
        std::cout << "✓ Norepinephrine accessible: Level = " << norepinephrine->getLevel() << std::endl;
        norepinephrine->setLevel(0.3f);
        norepinephrine->setArousal(0.9f);
        norepinephrine->setLCFireRate(0.7f);
        norepinephrine->triggerStressResponse(0.4f);
        std::cout << "  Set NE level to 0.3, arousal to 0.9, LC rate to 0.7, triggered stress response" << std::endl;
    } else {
        std::cerr << "✗ Norepinephrine not accessible!" << std::endl;
    }
    
    // NEW: Serotonin
    if (auto* serotonin = brain->getSerotonin()) {
        std::cout << "✓ Serotonin accessible: Level = " << serotonin->getLevel() << std::endl;
        serotonin->setLevel(0.8f);
        serotonin->setMood(0.6f);
        serotonin->setRapheActivity(0.5f);
        serotonin->modulateSocialBehavior(0.7f);
        std::cout << "  Set 5-HT level to 0.8, mood to 0.6, raphi activity to 0.5, modulated social behavior" << std::endl;
    } else {
        std::cerr << "✗ Serotonin not accessible!" << std::endl;
    }
    
    std::cout << "\n=== Testing Brain Step (Integration Test) ===" << std::endl;
    
    // Run a few simulation steps to test integration
    for (int step = 0; step < 5; ++step) {
        brain->step(step);
        std::cout << "Step " << step << ": " 
                  << brain->getFiringNeuronCount() << " firing neurons, "
                  << brain->getTotalSpikeCount() << " total spikes" << std::endl;
        
        // Check that all neuromodulators are still accessible
        if (brain->getDopamine()) std::cout << "  Dopamine: " << brain->getDopamine()->getLevel() << std::endl;
        if (brain->getAcetylcholine()) std::cout << "  Acetylcholine: " << brain->getAcetylcholine()->getLevel() << std::endl;
        if (brain->getNorepinephrine()) std::cout << "  Norepinephrine: " << brain->getNorepinephrine()->getLevel() << std::endl;
        if (brain->getSerotonin()) std::cout << "  Serotonin: " << brain->getSerotonin()->getLevel() << std::endl;
    }
    
    std::cout << "\n=== Testing Checkpoint Save/Load ===" << std::endl;
    
    // Test checkpoint save/load
    std::string checkpointFile = "test_checkpoint.bin";
    if (brain->save(checkpointFile)) {
        std::cout << "✓ Checkpoint saved successfully!" << std::endl;
        
        // Create new brain and load
        auto brain2 = std::make_shared<nlm::Brain>(config);
        if (brain2->initialize()) {
            if (brain2->load(checkpointFile)) {
                std::cout << "✓ Checkpoint loaded successfully!" << std::endl;
            } else {
                std::cerr << "✗ Failed to load checkpoint!" << std::endl;
            }
        } else {
            std::cerr << "✗ Failed to initialize second brain!" << std::endl;
        }
    } else {
        std::cerr << "✗ Failed to save checkpoint!" << std::endl;
    }
    
    // Clean up
    std::remove(checkpointFile.c_str());
    
    std::cout << "\n=== All Tests Completed ===" << std::endl;
    
    return 0;
}
