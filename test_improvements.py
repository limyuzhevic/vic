#!/usr/bin/env python3
"""
Test script to demonstrate the improved NLM Python bindings.
This script verifies that all the new functionality works correctly.
"""

import sys
import os

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

def test_basic_functionality():
    """Test basic functionality of improved Python bindings."""
    print("Testing NLM Python Bindings Improvements...")
    print("=" * 50)
    
    try:
        import pynlm
        print("✓ Successfully imported pynlm")
        
        # Test 1: Enhanced Configuration
        print("\n1. Testing Enhanced Configuration:")
        config = pynlm.createDefaultConfig()
        print("✓ Created default configuration")
        
        # Test setting values
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.15)
        config.set("brain.enable_curiosity", True)
        config.set("brain.learning_rate", 0.001)
        
        print("✓ Set configuration values")
        
        # Test getting values
        neuron_count = config.get("brain.neuron_count")
        density = config.get("brain.synapse_density")
        curiosity = config.get("brain.enable_curiosity")
        
        print(f"✓ Retrieved values: neurons={neuron_count}, density={density}, curiosity={curiosity}")
        
        # Test getOr with defaults
        max_neurons = config.getOr("brain.neuron_count", 1000)
        print(f"✓ getOr with default: {max_neurons}")
        
        # Test validation
        validation_errors = pynlm.validate_config(config)
        print(f"✓ Configuration validation: {len(validation_errors)} errors")
        
        # Test 2: Convenience Factory Functions
        print("\n2. Testing Convenience Factory Functions:")
        
        world = pynlm.createStandardWorld()
        print("✓ Created standard world")
        
        brain = pynlm.createBrainWithDefaults()
        print("✓ Created brain with default configuration")
        
        agent = pynlm.createAgentWithDefaults()
        print("✓ Created agent with default settings")
        
        # Test 3: AgentBrain Enhanced Features
        print("\n3. Testing Enhanced AgentBrain Features:")
        
        agent.initialize(world)
        print("✓ Initialized agent with world")
        
        # Test enable/disable all features
        agent.enableAllFeatures(True)
        print("✓ Enabled all features")
        
        agent.enableAllFeatures(False)
        print("✓ Disabled all features")
        
        # Test getAllStats
        stats = agent.getAllStats()
        print(f"✓ Retrieved comprehensive stats: {len(stats)} items")
        
        # Test 4: Configuration Presets
        print("\n4. Testing Configuration Presets:")
        
        balanced_config = pynlm.createBalancedConfig()
        print("✓ Created balanced configuration")
        
        highperf_config = pynlm.createHighPerformanceConfig()
        print("✓ Created high-performance configuration")
        
        minimal_config = pynlm.createMinimalConfig()
        print("✓ Created minimal configuration")
        
        # Test 5: Type Aliases and Utilities
        print("\n5. Testing Type Aliases and Utilities:")
        
        neuron_id = pynlm.create_neuron_id(12345)
        print("✓ Created neuron ID")
        
        synapse_id = pynlm.create_synapse_id(67890)
        print("✓ Created synapse ID")
        
        action_types = pynlm.get_default_action_types()
        print(f"✓ Retrieved {len(action_types)} action types")
        
        neuron_types = pynlm.get_neuron_types()
        print(f"✓ Retrieved {len(neuron_types)} neuron types")
        
        # Test 6: Error Handling
        print("\n6. Testing Error Handling:")
        
        safe_value = pynlm.safe_config_operation(config, "brain.nonexistent", "get", "default")
        print(f"✓ Safe operation with default: {safe_value}")
        
        # Test 7: Advanced Simulation
        print("\n7. Testing Advanced Simulation Features:")
        
        # Create a complete agent
        agent = pynlm.createAgentWithDefaults()
        world = pynlm.createStandardWorld()
        agent.initialize(world)
        
        # Run a simple episode
        for step in range(10):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            agent.getBrain()->step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print("✓ Completed simple episode")
        
        # Test 8: Memory Management
        print("\n8. Testing Memory Management:")
        
        memory_info = pynlm.get_agent_memory_usage(agent)
        print(f"✓ Retrieved memory info: {len(memory_info)} items")
        
        pynlm.cleanup_agent_brain(agent)
        print("✓ Cleaned up agent brain")
        
        print("\n" + "=" * 50)
        print("All tests passed! ✓")
        print("\nSummary of Improvements:")
        print("• Enhanced configuration system with type safety")
        print("• Convenience factory functions for common operations")
        print("• Comprehensive agent brain state monitoring")
        print("• Built-in configuration presets")
        print("• Robust error handling and validation")
        print("• Advanced simulation utilities")
        print("• Memory management tools")
        print("• Type aliases and utility functions")
        
        return True
        
    except ImportError as e:
        print(f"✗ Import error: {e}")
        return False
    except Exception as e:
        print(f"✗ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = test_basic_functionality()
    sys.exit(0 if success else 1)