#!/usr/bin/env python3
"""
Final verification script for NLM Python bindings enhancement.

This script verifies that the enhanced Python bindings are working correctly
and that all the new Pythonic features are properly integrated.
"""

import pynlm

def verify_backward_compatibility():
    """Verify that original API still works."""
    print("=== Testing Backward Compatibility ===")
    
    # Test original factory functions
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    print("✓ Original factory functions work")
    
    # Test original constants
    assert hasattr(pynlm, 'INVALID_NEURON_ID')
    assert hasattr(pynlm, 'INVALID_SYNAPSE_ID')
    assert hasattr(pynlm, 'INVALID_REGION_ID')
    assert hasattr(pynlm, 'INVALID_POPULATION_ID')
    print("✓ Original constants exist")
    
    # Test original types
    neuron_id = pynlm.NeuronId(42)
    assert neuron_id.value == 42
    print("✓ Original types work")
    
    return True

def verify_pythonic_features():
    """Verify new Pythonic features."""
    print("\n=== Testing Pythonic Features ===")
    
    # Test Pythonic properties
    neuron_id = pynlm.NeuronId(42)
    assert neuron_id.is_valid
    print("✓ Pythonic properties work")
    
    # Test enumeration helper methods
    neuron_type = pynlm.NeuronType.Excitatory
    assert neuron_type.is_excitatory_or_inhibitory
    assert neuron_type.is_sensory_or_motor
    print("✓ Enumeration helper methods work")
    
    # Test data utilities
    vec = [1.0, 2.0, 3.0, 4.0, 5.0]
    normalized = pynlm.normalize_vector(vec)
    assert all(0.0 <= x <= 1.0 for x in normalized)
    print("✓ Data utilities work")
    
    # Test new factory functions
    config = pynlm.create_config_with_defaults()
    assert config is not None
    print("✓ New factory functions work")
    
    return True

def verify_context_managers():
    """Verify context managers work."""
    print("\n=== Testing Context Managers ===")
    
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    world = pynlm.create_world()
    
    # Test brain context manager
    with pynlm.brain_context(brain) as ctx:
        assert ctx.get_brain() is not None
    print("✓ Brain context manager works")
    
    # Test world context manager
    with pynlm.world_context(world) as ctx:
        assert ctx.get_world() is not None
    print("✓ World context manager works")
    
    # Test config context manager
    with pynlm.config_context(config) as ctx:
        assert ctx.get_config() is not None
    print("✓ Config context manager works")
    
    return True

def verify_exception_handling():
    """Verify exception handling."""
    print("\n=== Testing Exception Handling ===")
    
    # Test new exceptions
    assert hasattr(pynlm, 'NLMException')
    assert hasattr(pynlm, 'ConfigError')
    assert hasattr(pynlm, 'BrainError')
    assert hasattr(pynlm, 'WorldError')
    assert hasattr(pynlm, 'SimulationError')
    print("✓ New exception classes exist")
    
    # Test exception inheritance
    try:
        raise pynlm.ConfigError("Test error")
    except pynlm.NLMException:
        print("✓ Exception inheritance works")
    
    return True

def verify_integration():
    """Verify integration with actual simulation."""
    print("\n=== Testing Integration ===")
    
    # Create simulation components
    config = pynlm.create_config_with_defaults()
    brain = pynlm.create_brain(config)
    world = pynlm.create_world()
    world.configure(20.0, 20.0, 8, 8)
    world.setAgentStart(10.0, 10.0)
    agent = pynlm.create_agent_brain(brain)
    agent.initialize(world)
    
    # Run simulation with context manager
    with pynlm.brain_context(brain) as ctx:
        for step in range(10):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, float(step))
            world.update(0.1)
    
    print("✓ Integration test passed")
    print(f"  - Final position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    print(f"  - Simulation time: {world.getSimulationTime():.1f}")
    
    return True

def main():
    """Run all verification tests."""
    print("=" * 60)
    print("NLM Python Bindings - Final Verification")
    print("=" * 60)
    
    try:
        verify_backward_compatibility()
        verify_pythonic_features()
        verify_context_managers()
        verify_exception_handling()
        verify_integration()
        
        print("\n" + "=" * 60)
        print("ALL VERIFICATION TESTS PASSED! ✓")
        print("=" * 60)
        print("\nThe enhanced Python bindings are working correctly:")
        print("  ✓ Backward compatibility maintained")
        print("  ✓ Pythonic features implemented")
        print("  ✓ Context managers working")
        print("  ✓ Exception handling improved")
        print("  ✓ Integration with simulation successful")
        
        return True
        
    except Exception as e:
        print(f"\n❌ VERIFICATION FAILED: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = main()
    exit(0 if success else 1)