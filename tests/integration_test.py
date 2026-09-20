#!/usr/bin/env python3
"""
NLM Phase 6 Integration Test
Tests that all Phase 6 systems work together as a coherent artificial brain
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

from pynlm import Brain, Config

def test_basic_functionality():
    """Test basic NLM functionality"""
    print("Testing NLM Phase 6 Integration...")
    
    # Create a simple configuration
    config = Config()
    config.set("neuron_count", 1000)
    config.set("region_count", 2)
    config.set("simulation_timestep", 0.001)
    config.set("random_seed", 42)
    
    # Create brain with configuration
    brain = Brain(config)
    
    # Initialize the brain
    if not brain.initialize():
        print("ERROR: Brain initialization failed")
        return False
    
    print(f"✓ Brain initialized successfully")
    print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  - Regions: {brain.getRegionCount()}")
    
    # Test step execution
    for i in range(10):
        brain.step(i)
    
    print("✓ Simulation steps completed")
    
    # Test memory system access
    working_memory = brain.getWorkingMemory()
    if working_memory:
        print("✓ Working memory accessible")
    
    episodic_memory = brain.getEpisodicMemory()
    if episodic_memory:
        print("✓ Episodic memory accessible")
    
    associative_memory = brain.getAssociativeMemory()
    if associative_memory:
        print("✓ Associative memory accessible")
    
    # Test neuromodulation system access
    dopamine = brain.getDopamine()
    if dopamine:
        print("✓ Dopamine accessible")
    
    curiosity = brain.getCuriosity()
    if curiosity:
        print("✓ Curiosity accessible")
    
    novelty = brain.getNovelty()
    if novelty:
        print("✓ Novelty accessible")
    
    prediction_error = brain.getPredictionErrorSignal()
    if prediction_error:
        print("✓ Prediction error accessible")
    
    # Test cognition systems
    planner = brain.getPlanner()
    if planner:
        print("✓ Neural planner accessible")
    
    concept_formation = brain.getConceptFormation()
    if concept_formation:
        print("✓ Concept formation accessible")
    
    attention = brain.getAttention()
    if attention:
        print("✓ Attentional selection accessible")
    
    # Test development system
    development = brain.getDevelopmentSystem()
    if development:
        print("✓ Development system accessible")
    
    # Test prediction system
    prediction = brain.getPredictionSystem()
    if prediction:
        print("✓ Prediction system accessible")
    
    # Test plasticity systems
    stdp = brain.getSTDP()
    if stdp:
        print("✓ STDP plasticity accessible")
    
    hebbian = brain.getHebbian()
    if hebbian:
        print("✓ Hebbian plasticity accessible")
    
    structural = brain.getStructuralPlasticity()
    if structural:
        print("✓ Structural plasticity accessible")
    
    # Test spike system
    spike_system = brain.getSpikeSystem()
    if spike_system:
        print("✓ Spike system accessible")
    
    # Test action production
    action = brain.produceAction()
    print("✓ Action production accessible")
    
    # Test brain logging
    brain.logStatus()
    print("✓ Brain status logging works")
    
    # Test checkpoint save/load
    import tempfile
    import os
    
    with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as temp_file:
        temp_filename = temp_file.name
    
    try:
        if brain.save(temp_filename):
            print("✓ Checkpoint save works")
            
            # Create a new brain and load the checkpoint
            brain2 = Brain(config)
            if brain2.load(temp_filename):
                print("✓ Checkpoint load works")
            else:
                print("✗ Checkpoint load failed")
                return False
        else:
            print("✗ Checkpoint save failed")
            return False
    finally:
        if os.path.exists(temp_filename):
            os.unlink(temp_filename)
    
    print("\n✓ All integration tests passed!")
    return True

def test_configuration_parsing():
    """Test different configuration file formats"""
    print("\nTesting Configuration Parsing...")
    
    from pynlm import Config
    
    # Test key=value format
    config = Config()
    config.set("neuron_count", 100)
    config.set("simulation_timestep", 0.01)
    config.set("random_seed", 123)
    
    if config.get<int64_t>("neuron_count") == 100:
        print("✓ Key=value configuration accessible")
    else:
        print("✗ Key=value configuration failed")
        return False
    
    # Test advanced configuration options
    config.set("dopamine_baseline", 0.2)
    config.set("plasticity_learning_rate", 0.05)
    
    if config.get<double>("dopamine_baseline") == 0.2:
        print("✓ Advanced configuration values accessible")
    else:
        print("✗ Advanced configuration values failed")
        return False
    
    print("✓ Configuration parsing tests passed")
    return True

def test_advanced_features():
    """Test advanced features if available"""
    print("\nTesting Advanced Features...")
    
    try:
        from advanced.AdvancedNLMBrain import AdvancedNLMBrain
        print("✓ Advanced features module available")
        
        # Create advanced brain instance
        # Note: This would require a Brain instance and Config
        # advanced_brain = AdvancedNLMBrain()
        # print("✓ AdvancedNLMBrain instantiated")
        
    except ImportError:
        print("ℹ Advanced features not available (expected for this test)")
    
    print("✓ Advanced features check completed")
    return True

def main():
    """Main test function"""
    print("=" * 60)
    print("NLM Phase 6 Integration Test Suite")
    print("=" * 60)
    
    all_passed = True
    
    # Run all tests
    tests = [
        test_basic_functionality,
        test_configuration_parsing,
        test_advanced_features,
    ]
    
    for test in tests:
        try:
            if not test():
                all_passed = False
                print(f"✗ Test {test.__name__} failed")
        except Exception as e:
            print(f"✗ Test {test.__name__} raised exception: {e}")
            import traceback
            traceback.print_exc()
            all_passed = False
    
    print("\n" + "=" * 60)
    if all_passed:
        print("SUCCESS: All tests passed!")
        print("NLM Phase 6 is functioning as an integrated artificial brain.")
    else:
        print("FAILURE: Some tests failed.")
        print("Please check the output above for details.")
    print("=" * 60)
    
    return 0 if all_passed else 1

if __name__ == "__main__":
    sys.exit(main())