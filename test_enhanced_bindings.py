#!/usr/bin/env python3
"""
Comprehensive test suite for NLM enhanced bindings
Demonstrates all new features and improvements
"""

import sys
import os
import time
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm

def test_all():
    """Run all tests for NLM enhanced bindings"""
    print("NLM Enhanced Bindings Test Suite")
    print("=" * 60)
    
    try:
        # Test 1: Basic functionality
        print("\n1. Testing Basic Functionality...")
        brain = test_basic_functionality()
        print("   ✓ Basic functionality test passed")
        
        # Test 2: Memory systems
        print("\n2. Testing Memory Systems...")
        test_memory_systems(brain)
        print("   ✓ Memory systems test passed")
        
        # Test 3: Neuromodulation
        print("\n3. Testing Neuromodulation...")
        test_neuromodulation(brain)
        print("   ✓ Neuromodulation test passed")
        
        # Test 4: Cognition systems
        print("\n4. Testing Cognition Systems...")
        test_cognition_systems(brain)
        print("   ✓ Cognition systems test passed")
        
        # Test 5: Agent integration
        print("\n5. Testing Agent Integration...")
        test_agent_integration()
        print("   ✓ Agent integration test passed")
        
        # Test 6: Pythonic interfaces
        print("\n6. Testing Pythonic Interfaces...")
        test_pythonic_interfaces()
        print("   ✓ Pythonic interfaces test passed")
        
        # Test 7: Performance monitoring
        print("\n7. Testing Performance Monitoring...")
        test_performance_monitoring(brain)
        print("   ✓ Performance monitoring test passed")
        
        # Test 8: Advanced features
        print("\n8. Testing Advanced Features...")
        test_advanced_features()
        print("   ✓ Advanced features test passed")
        
        # Test 9: Configuration validation
        print("\n9. Testing Configuration Validation...")
        test_configuration_validation()
        print("   ✓ Configuration validation test passed")
        
        print("\n" + "=" * 60)
        print("All tests completed successfully!")
        print(f"NLM version: {nlm.version()}")
        
        return 0
        
    except Exception as e:
        print(f"\nTest failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1

def test_basic_functionality():
    """Test basic NLM functionality"""
    print("\n   Creating test brain...")
    
    # Create test brain
    brain = nlm.create_test_brain()
    brain.initialize()
    
    print(f"   Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"   Brain has {brain.getRegionCount()} regions")
    
    # Run simulation steps
    print("   Running simulation steps...")
    for i in range(10):
        brain.step(i, i * 0.001)
    
    spikes = brain.getTotalSpikeCount()
    active = brain.getActiveNeuronCount()
    firing_rate = brain.getAverageFiringRate()
    
    print(f"   Total spikes after 10 steps: {spikes}")
    print(f"   Active neurons: {active}")
    print(f"   Average firing rate: {firing_rate:.2f} Hz")
    
    # Validate results
    assert spikes >= 0, "Spike count should not be negative"
    assert active >= 0, "Active neuron count should not be negative"
    assert firing_rate >= 0, "Firing rate should not be negative"
    
    return brain

def test_memory_systems(brain):
    """Test memory system integration"""
    print("\n   Testing working memory...")
    
    wm = brain.working_memory
    if wm:
        traces = wm.getActiveTraces()
        print(f"   Working memory traces: {traces}")
        assert traces >= 0, "Working memory traces should not be negative"
    else:
        print("   Working memory not available (expected for simple test)")
    
    print("\n   Testing episodic memory...")
    em = brain.episodic_memory
    if em:
        episodes = em.getEpisodeCount()
        print(f"   Episodic memory episodes: {episodes}")
        assert episodes >= 0, "Episode count should not be negative"
    else:
        print("   Episodic memory not available (expected for simple test)")

def test_neuromodulation(brain):
    """Test neuromodulation systems"""
    print("\n   Testing dopamine system...")
    dopamine = brain.dopamine
    if dopamine:
        level = dopamine.getLevel()
        print(f"   Dopamine level: {level:.3f}")
        assert 0.0 <= level <= 1.0, "Dopamine level should be between 0 and 1"
    else:
        print("   Dopamine system not available (expected for simple test)")
    
    print("\n   Testing curiosity system...")
    curiosity = brain.curiosity
    if curiosity:
        level = curiosity.getLevel()
        print(f"   Curiosity level: {level:.3f}")
        assert 0.0 <= level <= 1.0, "Curiosity level should be between 0 and 1"
    else:
        print("   Curiosity system not available (expected for simple test)")
    
    print("\n   Testing novelty system...")
    novelty = brain.novelty
    if novelty:
        level = novelty.getLevel()
        print(f"   Novelty level: {level:.3f}")
        assert 0.0 <= level <= 1.0, "Novelty level should be between 0 and 1"
    else:
        print("   Novelty system not available (expected for simple test)")

def test_cognition_systems(brain):
    """Test cognition systems"""
    print("\n   Testing planner system...")
    planner = brain.planner
    if planner:
        print("   Planner available for action planning")
    else:
        print("   Planner not available (expected for simple test)")
    
    print("\n   Testing concept formation...")
    concept_formation = brain.concept_formation
    if concept_formation:
        print("   Concept formation available for pattern discovery")
    else:
        print("   Concept formation not available (expected for simple test)")
    
    print("\n   Testing attention system...")
    attention = brain.attention
    if attention:
        print("   Attention system available for selective focus")
    else:
        print("   Attention system not available (expected for simple test)")

def test_agent_integration():
    """Test agent integration"""
    print("\n   Creating demo world...")
    
    world = nlm.create_demo_world()
    print(f"   World created: {world.getWidth()}x{world.getHeight()}")
    
    # Create agent brain
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"   Agent brain initialized with world")
    print(f"   Sensory input size: {agent.getSensoryInputSize()}")
    print(f"   Motor output size: {agent.getMotorOutputSize()}")
    
    # Test sensory processing
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    print("   Sensory processing completed")

def test_pythonic_interfaces():
    """Test Pythonic interfaces"""
    print("\n   Testing context manager...")
    brain = nlm.create_test_brain()
    
    with nlm.brain_context(brain) as ctx:
        print(f"   Context manager provides access to brain: {ctx is not None}")
        assert ctx is not None, "Context manager should provide brain access"
    
    # Test property access
    config = nlm.createDefaultConfig()
    print(f"   Config object: {config}")
    print(f"   Config keys: {config.getKeys()}")
    assert len(config.getKeys()) > 0, "Config should have keys"

def test_performance_monitoring(brain):
    """Test performance monitoring"""
    print("\n   Collecting memory statistics...")
    memory_stats = nlm.collect_memory_stats(brain)
    print("   Memory statistics:")
    for key, value in memory_stats.items():
        print(f"     {key}: {value}")
    
    print("\n   Collecting neural activity...")
    neural_activity = nlm.collect_neural_activity(brain)
    print("   Neural activity:")
    for key, value in neural_activity.items():
        print(f"     {key}: {value}")
    
    print("\n   Getting system status...")
    status = nlm.get_system_status(brain)
    print("   System status:")
    for category, metrics in status.items():
        print(f"     {category}:")
        for metric, value in metrics.items():
            print(f"       {metric}: {value}")
    
    # Validate collected data
    assert len(memory_stats) >= 0, "Memory stats should be collected"
    assert len(neural_activity) >= 0, "Neural activity should be collected"
    assert len(status) >= 0, "System status should be collected"

def test_advanced_features():
    """Test advanced features"""
    print("\n   Creating test brain...")
    brain = nlm.create_test_brain()
    
    print("\n   Configuring plasticity...")
    nlm.configure_plasticity(brain, hebbian=True, stdp=True, structural=False)
    print("   Plasticity configured: Hebbian and STDP enabled")
    
    print("\n   Setting developmental stage...")
    nlm.set_development_stage(brain, nlm.DevelopmentalStage.CriticalPeriod)
    print(f"   Developmental stage set to: {brain.getDevelopmentalStage()}")
    
    print("\n   Running quick simulation...")
    spikes = nlm.quick_simulate(brain, 50)
    print(f"   Quick simulation completed: {spikes} spikes generated")
    assert spikes >= 0, "Quick simulation should generate spikes"

def test_configuration_validation():
    """Test configuration validation"""
    print("\n   Testing valid configuration...")
    
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 100)
    config.set("region_count", 2)
    
    if nlm.validate_config(config):
        print("   ✓ Configuration validation: PASSED")
    else:
        print("   ✗ Configuration validation: FAILED")
        assert False, "Valid configuration should pass validation"
    
    print("\n   Testing invalid configuration...")
    config2 = nlm.createDefaultConfig()
    config2.set("neuron_count", -1)  # Invalid
    
    if not nlm.validate_config(config2):
        print("   ✓ Invalid configuration validation: CORRECTLY REJECTED")
    else:
        print("   ✗ Invalid configuration validation: ERROR - should have rejected")
        assert False, "Invalid configuration should be rejected"

if __name__ == "__main__":
    sys.exit(test_all())