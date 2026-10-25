#!/usr/bin/env python3
"""
Quick test script to verify the enhanced NLM Python bindings functionality.
This script tests the key improvements made to the NLM Python bindings.
"""

import sys
from pathlib import Path

# Add the python module to the path
sys.path.insert(0, str(Path(__file__).parent / 'python'))

def test_pythonic_interfaces():
    """Test Pythonic interfaces."""
    print("Testing Pythonic Interfaces...")
    
    try:
        import pynlm
        print("✓ Successfully imported pynlm")
        
        # Test context manager
        with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
            brain.initialize()
            print("✓ Context manager works")
        
        print("✓ Pythonic interfaces test passed")
        return True
    except Exception as e:
        print(f"✗ Pythonic interfaces test failed: {e}")
        return False

def test_enhanced_configuration():
    """Test enhanced configuration features."""
    print("\nTesting Enhanced Configuration...")
    
    try:
        import pynlm
        from pynlm.utils import create_config, validate_config
        
        # Create configuration
        config = create_config(neuron_count=100)
        print("✓ Created configuration")
        
        # Validate configuration
        result = validate_config(config)
        if result['valid']:
            print("✓ Configuration validation passed")
        else:
            print(f"✗ Configuration validation failed: {result['errors']}")
            return False
        
        print("✓ Enhanced configuration test passed")
        return True
    except Exception as e:
        print(f"✗ Enhanced configuration test failed: {e}")
        return False

def test_property_based_access():
    """Test property-based access."""
    print("\nTesting Property-Based Access...")
    
    try:
        import pynlm
        
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Test property access
        neurons = brain.total_neurons
        synapses = brain.total_synapses
        active = brain.active_neurons
        firing = brain.firing_neurons
        
        print(f"✓ Total neurons: {neurons}")
        print(f"✓ Total synapses: {synapses}")
        print(f"✓ Active neurons: {active}")
        print(f"✓ Firing neurons: {firing}")
        
        print("✓ Property-based access test passed")
        return True
    except Exception as e:
        print(f"✗ Property-based access test failed: {e}")
        return False

def test_enums():
    """Test enumeration types."""
    print("\nTesting Enumeration Types...")
    
    try:
        import pynlm
        
        # Test neuron types
        neuron_types = [t.value for t in pynlm.NeuronType]
        print(f"✓ Neuron types: {neuron_types}")
        
        # Test action types
        action_types = [t.value for t in pynlm.ActionType]
        print(f"✓ Action types: {action_types}")
        
        # Test developmental stages
        developmental_stages = [s.value for s in pynlm.DevelopmentalStage]
        print(f"✓ Developmental stages: {developmental_stages}")
        
        print("✓ Enumeration types test passed")
        return True
    except Exception as e:
        print(f"✗ Enumeration types test failed: {e}")
        return False

def test_advanced_features():
    """Test advanced simulation features."""
    print("\nTesting Advanced Simulation Features...")
    
    try:
        import pynlm
        
        # Create brain and world
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable advanced features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        print("✓ Advanced features enabled:")
        print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
        print(f"  Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
        print(f"  Development: {agent.isDevelopmentEnabled()}")
        print(f"  Curiosity: {agent.isCuriosityEnabled()}")
        
        print("✓ Advanced simulation features test passed")
        return True
    except Exception as e:
        print(f"✗ Advanced simulation features test failed: {e}")
        return False

def test_monitoring_tools():
    """Test monitoring tools."""
    print("\nTesting Monitoring Tools...")
    
    try:
        import pynlm
        from pynlm.monitoring import StatisticsCollector
        
        # Create monitoring tools
        collector = StatisticsCollector(pynlm.MonitoringLevel.BASIC)
        
        # Create brain and generate metrics
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        metrics = brain.get_statistics()
        
        # Record step
        collector.record_step(0, metrics)
        
        print("✓ Created and used monitoring tools")
        
        print("✓ Monitoring tools test passed")
        return True
    except Exception as e:
        print(f"✗ Monitoring tools test failed: {e}")
        return False

def test_utilities():
    """Test utility functions."""
    print("\nTesting Utility Functions...")
    
    try:
        import pynlm
        from pynlm.utils import create_config, analyze_results
        
        # Create configuration
        config = create_config(neuron_count=50)
        print("✓ Created configuration using utility")
        
        # Analyze results
        sample_data = [
            {'step': 0, 'total_spikes': 5, 'firing_rate': 0.5},
            {'step': 1, 'total_spikes': 8, 'firing_rate': 0.8},
        ]
        
        analysis = analyze_results(sample_data, 'summary')
        print("✓ Analyzed results using utility")
        
        print("✓ Utility functions test passed")
        return True
    except Exception as e:
        print(f"✗ Utility functions test failed: {e}")
        return False

def main():
    """Run all tests."""
    print("NLM Python Bindings - Enhanced Features Quick Test")
    print("=" * 60)
    
    tests = [
        test_pythonic_interfaces,
        test_enhanced_configuration,
        test_property_based_access,
        test_enums,
        test_advanced_features,
        test_monitoring_tools,
        test_utilities,
    ]
    
    passed = 0
    total = len(tests)
    
    for test in tests:
        if test():
            passed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 All tests passed! Enhanced NLM Python bindings are working correctly.")
        print("\nKey improvements verified:")
        print("  ✓ Pythonic interfaces with context managers")
        print("  ✓ Enhanced configuration validation")
        print("  ✓ Property-based access")
        print("  ✓ Comprehensive enumeration types")
        print("  ✓ Advanced simulation features")
        print("  ✓ Monitoring and analysis tools")
        print("  ✓ Utility functions")
        return 0
    else:
        print("❌ Some tests failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    exit(main())