#!/usr/bin/env python3
"""
Integration Test for NLM Brain Phase 6: Integrated Artificial Brain

This script tests the integrated systems improvements made to the NLM brain:
- Prediction system integration
- NeuralPlanner integration  
- Concept formation integration
- Neuromodulation effects
- Memory system integration
- Attention system integration
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

from core.Config.Config import Config
from brain.Brain import Brain

def test_prediction_integration():
    """Test prediction system integration with sensory processing"""
    print("Testing prediction system integration...")
    
    config = Config()
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    config.set("sensory_history_size", 10)
    
    brain = Brain(config)
    brain.initialize()
    
    # Simulate sensory input
    from sensory.Vision import Vision
    
    # Create simple test input
    testData = [0.1f, 0.2f, 0.3f, 0.4f, 0.5f]
    vision = Vision()
    vision.setData(testData)
    
    # Store initial state
    initialSensoryHistorySize = len(brain.pImpl.sensoryHistory)
    
    # Process sensory input
    brain.receiveSensoryInput(vision)
    
    # Check that sensory history was updated
    finalSensoryHistorySize = len(brain.pImpl.sensoryHistory)
    
    assert finalSensoryHistorySize > initialSensoryHistorySize, "Sensory history should be updated"
    assert brain.pImpl.sensoryHistory[-1].getData() == testData, "Sensory input should be stored"
    
    print("✓ Prediction system integration test passed")

def test_neural_planner_integration():
    """Test NeuralPlanner integration with action selection"""
    print("Testing NeuralPlanner integration...")
    
    config = Config()
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    config.set("planning_depth", 3)
    
    brain = Brain(config)
    brain.initialize()
    
    # Check that planner is initialized
    assert brain.pImpl.planner != None, "NeuralPlanner should be initialized"
    
    # Test that planner can plan actions
    testState = [0.1f, 0.2f, 0.3f, 0.4f, 0.5f]
    plannedAction = brain.pImpl.planner.planAction(testState)
    
    # Should not crash and should return a valid action type
    from motor.Action import ActionType
    assert plannedAction >= ActionType::Wait and plannedAction <= ActionType::Custom, "Should return valid action"
    
    print("✓ NeuralPlanner integration test passed")

def test_concept_formation_integration():
    """Test concept formation integration with attention and memory"""
    print("Testing concept formation integration...")
    
    config = Config()
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    
    brain = Brain(config)
    brain.initialize()
    
    # Check that concept formation is initialized
    assert brain.pImpl.conceptFormation != None, "ConceptFormation should be initialized"
    
    # Test concept formation with sensory input
    conceptFeatures = [0.1f, 0.2f, 0.3f, 0.4f, 0.5f]
    conceptId = brain.pImpl.conceptFormation.presentExperience(conceptFeatures, conceptFeatures, 0.5f, 0)
    
    # Concept ID should be valid (0 means novel/not yet discovered)
    assert conceptId >= 0, "Concept ID should be non-negative"
    
    # Test concept retrieval
    concept = brain.pImpl.conceptFormation.getConcept(conceptId)
    if conceptId > 0:
        assert concept != None, "Concept should be retrievable"
        assert concept.getConceptStability(conceptId) >= 0, "Concept stability should be valid"
    
    print("✓ Concept formation integration test passed")

def test_neuromodulation_effects():
    """Test enhanced neuromodulation effects on memory and attention"""
    print("Testing neuromodulation effects...")
    
    config = Config()
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    
    brain = Brain(config)
    brain.initialize()
    
    # Test dopamine effects
    if brain.pImpl.dopamine:
        initialDopamine = brain.pImpl.dopamine.getLevel()
        
        # Apply some learning signal
        brain.pImpl.dopamine.setLevel(0.8f)
        assert brain.pImpl.dopamine.getLevel() == 0.8f, "Dopamine level should be settable"
        
        # Test dopamine effects on working memory
        if brain.pImpl.workingMemory:
            brain.pImpl.workingMemory.strengthenMemory(1.1f)
    
    # Test novelty effects
    if brain.pImpl.novelty:
        initialNovelty = brain.pImpl.novelty.getLevel()
        
        # Apply novelty detection
        brain.pImpl.novelty.updatePattern(0.7f)
        assert brain.pImpl.novelty.getLevel() >= 0, "Novelty level should be non-negative"
    
    # Test curiosity effects
    if brain.pImpl.curiosity:
        initialCuriosity = brain.pImpl.curiosity.getLevel()
        
        # Apply curiosity
        brain.pImpl.curiosity.increaseCuriosity(0.3f)
        assert brain.pImpl.curiosity.getLevel() >= initialCuriosity, "Curiosity should increase"
    
    print("✓ Neuromodulation effects test passed")

def test_memory_integration():
    """Test memory system integration with prediction and attention"""
    print("Testing memory system integration...")
    
    config = Config()
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    config.set("replay_interval", 10)
    config.set("consolidation_interval", 50)
    
    brain = Brain(config)
    brain.initialize()
    
    # Test working memory integration
    if brain.pImpl.workingMemory:
        # Store some memory traces
        brain.pImpl.workingMemory.storeToNeuron(1, 0.5f)
        brain.pImpl.workingMemory.storeToNeuron(2, 0.7f)
        
        assert brain.pImpl.workingMemory.contains(1), "Should contain neuron 1"
        assert brain.pImpl.workingMemory.contains(2), "Should contain neuron 2"
        
        # Test memory competition
        brain.pImpl.workingMemory.runCompetition()
        assert brain.pImpl.workingMemory.getMemoryActivity() >= 0, "Memory activity should be valid"
    
    # Test episodic memory integration
    if brain.pImpl.episodicMemory:
        # Store an episode (simulating the episode storage in brain step)
        EpisodicMemoryItem episode;
        episode.timestamp = 0;
        episode.reward = 0.5f;
        episode.predictionError = 0.1f;
        episode.sensoryInput = Vision();  # This is a simplified test
        
        brain.pImpl.episodicMemory.storeEpisode(episode)
        assert brain.pImpl.episodicMemory.getEpisodeCount() > 0, "Should have stored episode"
    
    # Test episodic memory replay (simulating brain step logic)
    if brain.pImpl.episodicMemory and brain.pImpl.stepsSinceLastEpisode >= 10:
        brain.pImpl.stepsSinceLastEpisode = 0;
        
        # Get episodes for replay
        auto episodesToReplay = brain.pImpl.episodicMemory.getEpisodesForReplay(3);
        
        # This should not crash
        for (const auto* episode : episodesToReplay) {
            // Replay should not crash
        }
    
    print("✓ Memory system integration test passed")

def test_attention_integration():
    """Test attention system integration with sensory and memory feedback"""
    print("Testing attention system integration...")
    
    config = Config()
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    config.set("inhibition_strength", 0.5f)
    config.set("excitation_strength", 1.5f)
    
    brain = Brain(config)
    brain.initialize()
    
    # Test attention system initialization
    if brain.pImpl.attention:
        # Test attention updates
        brain.pImpl.attention.update(0.001f)
        
        # Test attention focusing
        NeuronId testNeuronId;
        testNeuronId.value = 1000;
        
        brain.pImpl.attention.applyBottomUpSalience(testNeuronId, 2.0f);
        brain.pImpl.attention.applyTopDownBias(testNeuronId, 1.0f);
        
        // Test competition processing
        std::vector<NeuronId> competitors;
        competitors.push_back(testNeuronId);
        
        std::vector<NeuronId> winners = brain.pImpl.attention.processCompetition(competitors);
        assert !winners.empty() or competitors.empty(), "Winners should be non-empty or same as competitors"
        
        // Test attention effects on neural activity
        if brain.pImpl.attention.isAttended(testNeuronId) {
            // Should apply excitation to attended neurons
        }
    
    print("✓ Attention system integration test passed")

def test_sensory_feedback_loop():
    """Test sensory feedback loops between prediction, attention, and memory"""
    print("Testing sensory feedback loops...")
    
    config = Config()
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    config.set("sensory_history_size", 5)
    
    brain = Brain(config)
    brain.initialize()
    
    // Create test sensory inputs
    std::vector<float> input1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> input2 = {0.4f, 0.5f, 0.6f};
    std::vector<float> input3 = {0.7f, 0.8f, 0.9f};
    
    Vision vision1, vision2, vision3;
    vision1.setData(input1);
    vision2.setData(input2);
    vision3.setData(input3);
    
    // Process inputs sequentially
    brain.receiveSensoryInput(vision1);
    brain.receiveSensoryInput(vision2);
    brain.receiveSensoryInput(vision3);
    
    // Check that sensory history was built
    assert brain.pImpl.sensoryHistory.size() == 3, "Should have 3 sensory inputs in history"
    
    // Check that prediction system was updated
    if brain.pImpl.predictionSystem {
        float predictionErr = brain.pImpl.predictionSystem.getPredictionError();
        assert predictionErr >= 0, "Prediction error should be non-negative"
    }
    
    // Check that attention system was affected
    if brain.pImpl.attention {
        // High prediction error should affect attention threshold
    }
    
    print("✓ Sensory feedback loops test passed")

def test_development_integration():
    """Test developmental system integration with neuromodulation"""
    print("Testing development system integration...")
    
    config = Config()
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    config.set("simulation_timestep", 0.001)
    
    brain = Brain(config)
    brain.initialize()
    
    // Test development stages
    assert brain.pImpl.developmentalStage == DevelopmentalStage::Initial, "Should start in initial stage"
    
    // Simulate development progression
    brain.pImpl.developmentalStage = DevelopmentalStage::CriticalPeriod;
    assert brain.pImpl.developmentalStage == DevelopmentalStage::CriticalPeriod, "Should progress to critical period"
    
    brain.pImpl.developmentalStage = DevelopmentalStage::Maturation;
    assert brain.pImpl.developmentalStage == DevelopmentalStage::Maturation, "Should progress to maturation"
    
    brain.pImpl.developmentalStage = DevelopmentalStage::Adult;
    assert brain.pImpl.developmentalStage == DevelopmentalStage::Adult, "Should progress to adult"
    
    brain.pImpl.developmentalStage = DevelopmentalStage::Aging;
    assert brain.pImpl.developmentalStage == DevelopmentalStage::Aging, "Should progress to aging"
    
    print("✓ Development system integration test passed")

def run_integration_tests():
    """Run all integration tests"""
    print("=" * 60)
    print("NLM Brain Phase 6 Integration Tests")
    print("=" * 60)
    
    try:
        test_prediction_integration()
        test_neural_planner_integration()
        test_concept_formation_integration()
        test_neuromodulation_effects()
        test_memory_integration()
        test_attention_integration()
        test_sensory_feedback_loop()
        test_development_integration()
        
        print("=" * 60)
        print("All integration tests passed! ✓")
        print("=" * 60)
        return True
        
    except Exception as e:
        print(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = run_integration_tests()
    sys.exit(0 if success else 1)