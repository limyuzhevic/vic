#!/usr/bin/env python3
"""
Simple test script to verify the NLM Python bindings work correctly.

This script tests basic functionality of the pynlm module to ensure
the Python bindings are working as expected.
"""

import sys
import os

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_python_module_import():
    """Test that we can import the pynlm module."""
    print("Testing pynlm module import...")
    
    try:
        import pynlm
        print("✓ Successfully imported pynlm")
        print(f"  Version: {pynlm.__version__}")
        return pynlm
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return None
    except Exception as e:
        print(f"✗ Unexpected error importing pynlm: {e}")
        return None


def test_config_creation(pynlm):
    """Test configuration creation."""
    print("\nTesting configuration creation...")
    
    try:
        # Test default config
        config = pynlm.createDefaultConfig()
        print("✓ Created default configuration")
        
        # Test that config has expected keys
        expected_keys = [
            "brain.neuron_count",
            "brain.region_count", 
            "simulation.timestep",
            "log.level"
        ]
        
        for key in expected_keys:
            if config.has(key):
                print(f"  ✓ Config has key: {key}")
            else:
                print(f"  ✗ Config missing key: {key}")
        
        return config
    except Exception as e:
        print(f"✗ Error creating configuration: {e}")
        return None


def test_brain_creation(pynlm):
    """Test brain creation."""
    print("\nTesting brain creation...")
    
    try:
        # Create brain with default config
        brain = pynlm.createBrain()
        print("✓ Created brain")
        
        # Check brain properties
        neurons = brain.getTotalNeuronCount()
        synapses = brain.getTotalSynapseCount()
        print(f"  - Neurons: {neurons}")
        print(f"  - Synapses: {synapses}")
        
        # Initialize brain
        brain.initialize()
        print("✓ Brain initialized")
        
        return brain
    except Exception as e:
        print(f"✗ Error creating brain: {e}")
        return None


def test_brain_simulation(pynlm, brain):
    """Test basic brain simulation."""
    print("\nTesting brain simulation...")
    
    try:
        if brain is None:
            print("✗ No brain to simulate")
            return
        
        # Run simulation
        for step in range(10):
            brain.step(step)
        
        print("✓ Simulation completed")
        
        # Get statistics
        stats = pynlm.get_brain_stats(brain)
        print(f"  - Total spikes: {stats.get('total_spikes', 0)}")
        print(f"  - Firing neurons: {stats.get('firing_neurons', 0)}")
        
        return True
    except Exception as e:
        print(f"✗ Error during simulation: {e}")
        return False


def test_world_creation(pynlm):
    """Test world creation."""
    print("\nTesting world creation...")
    
    try:
        world = pynlm.createSimpleWorld()
        print("✓ Created simple world")
        
        # Check world properties
        width = world.getWidth()
        height = world.getHeight()
        vision_width = world.getVisionWidth()
        vision_height = world.getVisionHeight()
        
        print(f"  - Dimensions: {width}x{height}")
        print(f"  - Vision: {vision_width}x{vision_height}")
        
        return world
    except Exception as e:
        print(f"✗ Error creating world: {e}")
        return None


def test_agent_creation(pynlm, brain):
    """Test agent creation."""
    print("\nTesting agent creation...")
    
    try:
        if brain is None:
            print("✗ No brain to create agent for")
            return None
        
        agent = pynlm.createAgentBrain(brain)
        print("✓ Created agent brain")
        
        # Check agent properties
        sensory_size = agent.getSensoryInputSize()
        motor_size = agent.getMotorOutputSize()
        
        print(f"  - Sensory input size: {sensory_size}")
        print(f"  - Motor output size: {motor_size}")
        
        return agent
    except Exception as e:
        print(f"✗ Error creating agent: {e}")
        return None


def test_config_presets(pynlm):
    """Test configuration presets."""
    print("\nTesting configuration presets...")
    
    try:
        presets = [
            ("Phase 2", pynlm.createPhase2Config),
            ("Phase 3", pynlm.createPhase3Config),
            ("Phase 6", pynlm.createPhase6Config),
        ]
        
        for name, preset_func in presets:
            config = preset_func()
            print(f"✓ Created {name} configuration")
            
            # Test that it's a valid config
            if hasattr(config, 'get'):
                neuron_count = config.get("brain.neuron_count")
                print(f"  - Neuron count: {neuron_count}")
        
        return True
    except Exception as e:
        print(f"✗ Error testing presets: {e}")
        return False


def test_examples(pynlm):
    """Test example functions."""
    print("\nTesting example functions...")
    
    try:
        # Test simple brain example
        print("Testing simple_brain_example...")
        pynlm.simple_brain_example()
        print("✓ simple_brain_example completed")
        
        return True
    except Exception as e:
        print(f"✗ Error in example functions: {e}")
        return False


def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    # Test module import
    pynlm = test_python_module_import()
    if pynlm is None:
        print("\n✗ Failed to import pynlm module. Tests cannot continue.")
        return 1
    
    # Run all tests
    config = test_config_creation(pynlm)
    brain = test_brain_creation(pynlm)
    world = test_world_creation(pynlm)
    agent = test_agent_creation(pynlm, brain)
    simulation_ok = test_brain_simulation(pynlm, brain)
    presets_ok = test_config_presets(pynlm)
    examples_ok = test_examples(pynlm)
    
    # Summary
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    
    tests = [
        ("Module Import", pynlm is not None),
        ("Config Creation", config is not None),
        ("Brain Creation", brain is not None),
        ("World Creation", world is not None),
        ("Agent Creation", agent is not None),
        ("Brain Simulation", simulation_ok),
        ("Config Presets", presets_ok),
        ("Example Functions", examples_ok),
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_result in tests:
        status = "PASS" if test_result else "FAIL"
        print(f"{test_name}: {status}")
        if test_result:
            passed += 1
    
    print(f"\nResults: {passed}/{total} tests passed")
    
    if passed == total:
        print("✓ All tests passed!")
        return 0
    else:
        print(f"✗ {total - passed} tests failed")
        return 1


if __name__ == "__main__":
    sys.exit(main())
