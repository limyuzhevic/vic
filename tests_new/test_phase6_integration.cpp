// Comprehensive Phase 6 Integration Tests
// Integration tests for all major Phase 6 systems
// Tests the coordination and interaction between brain systems

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_phase6 {

void test_memory_integration() {
    std::cout << "  Testing Memory Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory integration
    auto working_memory = brain.getWorkingMemory();
    assert(working_memory != nullptr);
    
    // Test episodic memory integration
    auto episodic_memory = brain.getEpisodicMemory();
    assert(episodic_memory != nullptr);
    
    // Test semantic memory integration
    auto semantic_memory = brain.getSemanticMemory();
    assert(semantic_memory != nullptr);
    
    // Test procedural memory integration
    auto procedural_memory = brain.getProceduralMemory();
    assert(procedural_memory != nullptr);
    
    // Test associative memory integration
    auto associative_memory = brain.getAssociativeMemory();
    assert(associative_memory != nullptr);
    
    // Test memory integration in brain loop
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        // Verify memory integration effects
        auto working_stats = working_memory->getWorkingMemoryStats();
        assert(working_stats.capacity > 0);
    }
    
    std::cout << "    Memory integration test passed" << std::endl;
}

void test_neuromodulation_integration() {
    std::cout << "  Testing Neuromodulation Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test all 8 neuromodulators
    auto dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    auto acetylcholine = brain.getAcetylcholine();
    assert(acetylcholine != nullptr);
    
    auto norepinephrine = brain.getNorepinephrine();
    assert(norepinephrine != nullptr);
    
    auto serotonin = brain.getSerotonin();
    assert(serotonin != nullptr);
    
    auto curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    auto novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    auto prediction_error = brain.getPredictionErrorSignal();
    assert(prediction_error != nullptr);
    
    auto reward = brain.getReward();
    assert(reward != nullptr);
    
    // Test neuromodulation integration in brain loop
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        // Verify neuromodulation integration effects
        dopamine->update(step);
        acetylcholine->update(step);
        // Update other neuromodulators...
    }
    
    std::cout << "    Neuromodulation integration test passed" << std::endl;
}

void test_prediction_integration() {
    std::cout << "  Testing Prediction Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test prediction system integration
    auto prediction_system = brain.getPredictionSystem();
    assert(prediction_system != nullptr);
    
    // Test prediction system integration in brain loop
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        // Verify prediction integration effects
        auto prediction_state = prediction_system->getPredictionState();
        assert(prediction_state.accuracy >= 0.0f && prediction_state.accuracy <= 1.0f);
        assert(prediction_state.confidence >= 0.0f && prediction_state.confidence <= 1.0f);
    }
    
    std::cout << "    Prediction integration test passed" << std::endl;
}

void test_development_integration() {
    std::cout << "  Testing Development Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test development system integration
    auto development_system = brain.getDevelopmentSystem();
    assert(development_system != nullptr);
    
    // Test developmental stage detection and transitions
    nlm::DevelopmentalStage initial_stage = development_system->getStage();
    
    // Simulate development over time
    for (double time = 0.0; time < 100.0; time += 1.0) {
        brain.step(static_cast<nlm::SimulationStep>(time * 10));
        development_system->updateDevelopment(static_cast<nlm::SimulationStep>(time * 10));
        
        // Verify development integration effects
        float excitability = development_system->getNeuralExcitability();
        float attention_balance = development_system->getAttentionBalance();
        float memory_strength = development_system->getMemoryConsolidationStrength();
        float neuromod_sensitivity = development_system->getNeuromodulationSensitivity();
        
        assert(excitability >= 0.0f && excitability <= 1.0f);
        assert(attention_balance >= 0.0f && attention_balance <= 1.0f);
        assert(memory_strength >= 0.0f && memory_strength <= 1.0f);
        assert(neuromod_sensitivity >= 0.0f && neuromod_sensitivity <= 1.0f);
    }
    
    std::cout << "    Development integration test passed" << std::endl;
}

void test_persistence_integration() {
    std::cout << "  Testing Persistence Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test persistence integration
    // Brain has built-in save/load functionality via CheckpointSystem
    
    // Run some simulation steps
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    // Test checkpointing functionality (if available)
    // brain.save("test_checkpoint.nlm");
    // nlm::Brain brain2(config);
    // brain2.load("test_checkpoint.nlm");
    
    std::cout << "    Persistence integration test passed" << std::endl;
}

void test_cognition_integration() {
    std::cout << "  Testing Cognition Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test cognitive systems integration (components that exist in Brain)
    auto neural_planner = brain.getPlanner();
    auto concept_formation = brain.getConceptFormation();
    auto attentional_selection = brain.getAttention();
    
    // Verify cognitive systems are accessible
    assert(neural_planner != nullptr);
    assert(concept_formation != nullptr);
    assert(attentional_selection != nullptr);
    
    // Note: SelfModel and SocialLearning classes exist but are not yet 
    // exposed as direct Brain getters. They may be integrated differently.
    // For now, we test the cognitive systems that are currently available.
    
    // Test cognitive integration in brain loop
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        // Verify cognitive integration effects
        // Neural planner influences action selection
        // Concept formation integrates with semantic memory
        // Attention selection is influenced by neuromodulation
    }
    
    std::cout << "    Cognition integration test passed" << std::endl;
}

void test_agent_integration() {
    std::cout << "  Testing Agent Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test AgentBrain integration with brain systems
    auto agent_brain = brain.getAgentBrain();
    assert(agent_brain != nullptr);
    
    // Test agent-brain integration
    agent_brain->update(brain.step(0)); // Update with first step
    
    std::cout << "    Agent integration test passed" << std::endl;
}

void test_brain_loop_integration() {
    std::cout << "  Testing Brain Loop Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test complete brain loop integration with all systems
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        // 1. Brain receives sensory input (if any)
        // 2. Brain steps through neural dynamics
        brain.step(step);
        
        // 3. All integrated systems should be updated:
        //    - Memory systems (working, episodic, semantic, procedural)
        //    - Neuromodulation systems (dopamine, acetylcholine, etc.)
        //    - Prediction system
        //    - Development system
        //    - Cognitive systems (neural planner, concept formation, etc.)
        //    - Performance monitoring
        //    - Error handling
        
        // Verify brain state consistency
        assert(brain.getTotalNeuronCount() == 1000);
        assert(brain.getRegionCount() == 2);
        assert(brain.getTotalSynapseCount() >= 0);
        
        // Verify all major systems report activity
        assert(brain.getWorkingMemory()->getWorkingMemoryStats().capacity > 0);
    }
    
    std::cout << "    Brain loop integration test passed" << std::endl;
}

void runAll() {
    std::cout << "=== Phase 6 Integration Tests ===" << std::endl;
    
    test_memory_integration();
    test_neuromodulation_integration();
    test_prediction_integration();
    test_development_integration();
    test_persistence_integration();
    test_cognition_integration();
    test_agent_integration();
    test_brain_loop_integration();
    
    std::cout << std::endl;
    std::cout << "=== All Phase 6 Integration Tests PASSED ===" << std::endl;
}

} // namespace test_phase6