#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "=== Testing NLM Brain Integration ===" << std::endl;
    
    // Test 1: Create brain with config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<uint64_t>(42), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    std::cout << "✓ Created brain with 100 neurons" << std::endl;
    
    // Test 2: Initialize brain
    bool initSuccess = brain.initialize();
    if (initSuccess) {
        std::cout << "✓ Brain initialized successfully" << std::endl;
    } else {
        std::cout << "✗ Brain initialization failed" << std::endl;
        return 1;
    }
    
    // Test 3: Check memory system integration
    std::cout << "\n=== Memory System Integration Test ===" << std::endl;
    
    nlm::NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
    if (workingMemory != nullptr) {
        std::cout << "✓ Working memory is properly integrated" << std::endl;
        std::cout << "  Active traces: " << workingMemory->getActiveTraces() << std::endl;
    } else {
        std::cout << "✗ Working memory is NOT integrated (returns nullptr)" << std::endl;
    }
    
    nlm::NeuralEpisodicMemory* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory != nullptr) {
        std::cout << "✓ Episodic memory is properly integrated" << std::endl;
        std::cout << "  Episodes: " << episodicMemory->getEpisodeCount() << std::endl;
    } else {
        std::cout << "✗ Episodic memory is NOT integrated (returns nullptr)" << std::endl;
    }
    
    nlm::NeuralAssociativeMemory* associativeMemory = brain.getAssociativeMemory();
    if (associativeMemory != nullptr) {
        std::cout << "✓ Associative memory is properly integrated" << std::endl;
    } else {
        std::cout << "✗ Associative memory is NOT integrated (returns nullptr)" << std::endl;
    }
    
    // Test 4: Check prediction system integration
    std::cout << "\n=== Prediction System Integration Test ===" << std::endl;
    
    nlm::PredictionSystem* predictionSystem = brain.getPredictionSystem();
    if (predictionSystem != nullptr) {
        std::cout << "✓ Prediction system is properly integrated" << std::endl;
    } else {
        std::cout << "✗ Prediction system is NOT integrated (returns nullptr)" << std::endl;
    }
    
    // Test 5: Check neuromodulation systems integration
    std::cout << "\n=== Neuromodulation Systems Integration Test ===" << std::endl;
    
    nlm::Dopamine* dopamine = brain.getDopamine();
    if (dopamine != nullptr) {
        std::cout << "✓ Dopamine system is properly integrated" << std::endl;
        std::cout << "  Level: " << dopamine->getLevel() << std::endl;
    } else {
        std::cout << "✗ Dopamine system is NOT integrated (returns nullptr)" << std::endl;
    }
    
    nlm::Curiosity* curiosity = brain.getCuriosity();
    if (curiosity != nullptr) {
        std::cout << "✓ Curiosity system is properly integrated" << std::endl;
        std::cout << "  Level: " << curiosity->getLevel() << std::endl;
    } else {
        std::cout << "✗ Curiosity system is NOT integrated (returns nullptr)" << std::endl;
    }
    
    nlm::Novelty* novelty = brain.getNovelty();
    if (novelty != nullptr) {
        std::cout << "✓ Novelty system is properly integrated" << std::endl;
        std::cout << "  Level: " << novelty->getLevel() << std::endl;
    } else {
        std::cout << "✗ Novelty system is NOT integrated (returns nullptr)" << std::endl;
    }
    
    nlm::PredictionError* predictionError = brain.getPredictionErrorSignal();
    if (predictionError != nullptr) {
        std::cout << "✓ Prediction Error system is properly integrated" << std::endl;
        std::cout << "  Level: " << predictionError->getLevel() << std::endl;
    } else {
        std::cout << "✗ Prediction Error system is NOT integrated (returns nullptr)" << std::endl;
    }
    
    // Test 6: Check cognition systems integration
    std::cout << "\n=== Cognition Systems Integration Test ===" << std::endl;
    
    nlm::NeuralPlanner* planner = brain.getPlanner();
    if (planner != nullptr) {
        std::cout << "✓ Neural Planner is properly integrated" << std::endl;
    } else {
        std::cout << "✗ Neural Planner is NOT integrated (returns nullptr)" << std::endl;
    }
    
    nlm::ConceptFormation* conceptFormation = brain.getConceptFormation();
    if (conceptFormation != nullptr) {
        std::cout << "✓ Concept Formation is properly integrated" << std::endl;
    } else {
        std::cout << "✗ Concept Formation is NOT integrated (returns nullptr)" << std::endl;
    }
    
    nlm::AttentionalSelection* attention = brain.getAttention();
    if (attention != nullptr) {
        std::cout << "✓ Attentional Selection is properly integrated" << std::endl;
    } else {
        std::cout << "✗ Attentional Selection is NOT integrated (returns nullptr)" << std::endl;
    }
    
    // Test 7: Check development system integration
    std::cout << "\n=== Development System Integration Test ===" << std::endl;
    
    nlm::DevelopmentSystem* developmentSystem = brain.getDevelopmentSystem();
    if (developmentSystem != nullptr) {
        std::cout << "✓ Development System is properly integrated" << std::endl;
        std::cout << "  Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
    } else {
        std::cout << "✗ Development System is NOT integrated (returns nullptr)" << std::endl;
    }
    
    // Test 8: Run brain steps and verify everything still works
    std::cout << "\n=== Brain Step Execution Test ===" << std::endl;
    
    for (int step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    std::cout << "✓ Brain executed 10 steps successfully" << std::endl;
    std::cout << "  Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "  Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "  Active neurons: " << brain.getActiveNeuronCount() << std::endl;
    std::cout << "  Firing neurons (this step): " << brain.getFiringNeuronCount() << std::endl;
    
    // Test 9: Test checkpoint save/load
    std::cout << "\n=== Checkpoint Test ===" << std::endl;
    
    std::string checkpointFile = "test_checkpoint.bin";
    bool saveSuccess = brain.save(checkpointFile);
    if (saveSuccess) {
        std::cout << "✓ Brain checkpoint saved successfully" << std::endl;
    } else {
        std::cout << "✗ Brain checkpoint save failed" << std::endl;
    }
    
    // Create a new brain and load from checkpoint
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config2->set("random_seed", static_cast<uint64_t>(42), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    brain2.initialize();
    
    bool loadSuccess = brain2.load(checkpointFile);
    if (loadSuccess) {
        std::cout << "✓ Brain checkpoint loaded successfully" << std::endl;
    } else {
        std::cout << "✗ Brain checkpoint load failed" << std::endl;
    }
    
    std::cout << "\n=== Integration Test Summary ===" << std::endl;
    std::cout << "This test verifies that Phase 6 systems are properly integrated:" << std::endl;
    std::cout << "  - Memory systems (working, episodic, associative)" << std::endl;
    std::cout << "  - Prediction system" << std::endl;
    std::cout << "  - Neuromodulation systems (dopamine, curiosity, novelty, prediction error)" << std::endl;
    std::cout << "  - Cognition systems (neural planner, concept formation, attention)" << std::endl;
    std::cout << "  - Development system" << std::endl;
    std::cout << "  - Checkpoint persistence" << std::endl;
    
    return 0;
}
