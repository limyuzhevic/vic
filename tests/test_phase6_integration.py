import pynlm

# Phase 6 Integration Tests for Python API
# Tests Python bindings integration with all brain systems

# Configuration for integration tests
def test_phase6_integration():
    """Test Phase 6 integration using Python bindings"""
    print("=== Phase 6 Integration Tests (Python) ===")
    print("Testing Python bindings integration with all brain systems...")
    print()
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("region_count", 2)
    config.set("random_seed", 42)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test memory system integration
    print("  Testing Memory System Integration...")
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    assert working_memory is not None, "Working memory not accessible"
    assert episodic_memory is not None, "Episodic memory not accessible"
    print("    ✓ Memory systems accessible")
    
    # Test neuromodulation integration
    print("  Testing Neuromodulation Integration...")
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    prediction_error = brain.getPredictionErrorSignal()
    
    assert dopamine is not None, "Dopamine neuromodulator not accessible"
    assert curiosity is not None, "Curiosity neuromodulator not accessible"
    assert novelty is not None, "Novelty neuromodulator not accessible"
    assert prediction_error is not None, "Prediction error not accessible"
    print("    ✓ Neuromodulation systems accessible")
    
    # Test cognition system integration
    print("  Testing Cognition System Integration...")
    planner = brain.getPlanner()
    concept_formation = brain.getConceptFormation()
    attention = brain.getAttention()
    
    assert planner is not None, "Planner not accessible"
    assert concept_formation is not None, "Concept formation not accessible"
    assert attention is not None, "Attention system not accessible"
    print("    ✓ Cognition systems accessible")
    
    # Test development system integration
    print("  Testing Development System Integration...")
    development_system = brain.getDevelopmentSystem()
    assert development_system is not None, "Development system not accessible"
    print("    ✓ Development system accessible")
    
    # Run integration simulation
    print("  Running integration simulation...")
    for step in range(100):
        brain.step(step)
        
        # Inject sensory input
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 1.0)
        
        # Apply neuromodulation
        brain.applyNeuromodulation(dopamine)
        
        # Update development
        brain.develop()
        
        if step % 25 == 0:
            print(f"    Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print("    ✓ Integration simulation completed")
    
    # Test checkpoint functionality
    print("  Testing checkpoint functionality...")
    import os
    
    # Save checkpoint
    checkpoint_file = "test_checkpoint.bin"
    success = brain.save(checkpoint_file)
    assert success, "Failed to save checkpoint"
    print("    ✓ Checkpoint saved successfully")
    
    # Create new brain and load checkpoint
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    
    # Note: Python API doesn't implement load yet
    # When implemented, this would test:
    # success = brain2.load(checkpoint_file)
    # assert success, "Failed to load checkpoint"
    # print("    ✓ Checkpoint loaded successfully")
    
    # Clean up
    if os.path.exists(checkpoint_file):
        os.remove(checkpoint_file)
        print("    ✓ Checkpoint file cleaned up")
    
    # Test statistics
    print("  Testing statistics...")
    total_spikes = brain.getTotalSpikeCount()
    avg_firing_rate = brain.getAverageFiringRate()
    e_i_ratio = brain.getExcitationInhibitionRatio()
    
    print(f"    Total spikes: {total_spikes}")
    print(f"    Average firing rate: {avg_firing_rate:.2f} Hz")
    print(f"    E/I ratio: {e_i_ratio:.2f}")
    
    assert total_spikes > 0, "Should have generated spikes"
    assert avg_firing_rate >= 0.0, "Firing rate should be non-negative"
    assert not math.isnan(e_i_ratio), "E/I ratio should be valid"
    
    print("    ✓ Statistics valid")
    
    print()
    print("=== Phase 6 Integration Tests Passed! ===")

# Additional helper functions
def run_all_integration_tests():
    """Run all Phase 6 integration tests"""
    try:
        test_phase6_integration()
        print("\nAll tests completed successfully!")
    except Exception as e:
        print(f"\nTest failed with error: {e}")
        raise

# Test function for command-line execution
if __name__ == "__main__":
    import math
    run_all_integration_tests()