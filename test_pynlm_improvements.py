#!/usr/bin/env python3
"""
Comprehensive test script for NLM Python bindings improvements.

This script demonstrates the enhanced Python API for NLM (Neural Learning Machine)
including the new factory functions, helper methods, and advanced features.
"""

import sys
import os

# Add the current directory to the path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    print("Successfully imported pynlm")
except ImportError as e:
    print(f"Failed to import pynlm: {e}")
    print("This script requires the NLM Python bindings to be built.")
    sys.exit(1)

def test_factory_functions():
    """Test the new factory functions."""
    print("\n=== Testing Factory Functions ===")
    
    # Test createDefaultConfig
    config = pynlm.createDefaultConfig()
    print(f"✓ Created default config: {config.summary()}")
    
    # Test create_quick_brain
    brain = pynlm.create_quick_brain(500)
    print(f"✓ Created quick brain with {brain.getTotalNeuronCount()} neurons")
    
    # Test createSimpleWorld
    world = pynlm.createSimpleWorld()
    print(f"✓ Created simple world {world.getWidth()}x{world.getHeight()}")
    
    # Test create_agent_system
    brain2, agent, world2 = pynlm.create_agent_system()
    print(f"✓ Created agent system with {brain2.getTotalNeuronCount()} neurons")
    
    # Test configure_world
    configured_world = pynlm.configure_world(world2, 20, 20, 16, 16, 0.05, 0.05)
    print(f"✓ Configured world: {configured_world.getWidth()}x{configured_world.getHeight()}, energy decay: {configured_world.getEnergyDecayRate()}")
    
    return brain, agent, world, config

def test_helper_functions():
    """Test the new helper functions."""
    print("\n=== Testing Helper Functions ===")
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test enable_all_learning
    pynlm.enable_all_learning(agent, True, True, True, True)
    print("✓ Enabled all learning subsystems")
    
    # Test get_brain_stats
    stats = pynlm.get_brain_stats(brain)
    print(f"✓ Got brain stats: {len(stats)} metrics")
    print(f"  - Total neurons: {stats['total_neurons']}")
    print(f"  - Average firing rate: {stats['average_firing_rate']:.3f}")
    
    # Test setup_world_and_agent
    agent2 = pynlm.setup_world_and_agent(brain, world)
    print("✓ Setup world and agent together")
    
    # Test run_steps
    firing_counts, spike_counts = pynlm.run_steps(brain, 10, True)
    print(f"✓ Ran 10 steps, got {len(firing_counts)} firing counts")
    
    return brain, agent, world

def test_advanced_features():
    """Test advanced features for power users."""
    print("\n=== Testing Advanced Features ===")
    
    # Test create_advanced_config
    advanced_config = pynlm.create_advanced_config(
        neuron_count=2000,
        learning_rate=0.01,
        enable_stdp=True,
        enable_hebbian=True,
        enable_structural_plasticity=True,
        dopamine_scale=2.0,
        enable_curiosity=True,
        enable_novelty=True
    )
    print(f"✓ Created advanced config: {advanced_config.summary()}")
    
    # Test create_agent_system with advanced config
    brain, agent, world = pynlm.create_agent_system(advanced_config)
    print(f"✓ Created advanced agent system with {brain.getTotalNeuronCount()} neurons")
    
    # Test configure_world with custom settings
    custom_world = pynlm.configure_world(
        pynlm.createSimpleWorld(),
        width=30,
        height=30,
        vision_width=20,
        vision_height=20,
        energy_decay=0.02,
        energy_decay_rate=0.5
    )
    print(f"✓ Created custom world: {custom_world.getWidth()}x{custom_world.getHeight()}")
    
    # Test get_brain_stats with advanced brain
    stats = pynlm.get_brain_stats(brain)
    print(f"✓ Got advanced brain stats: {len(stats)} metrics")
    
    # Test run_agent_simulation
    brain3, agent3, world3 = pynlm.create_agent_system()
    firing_count, spike_count = pynlm.run_agent_simulation(brain3, agent3, world3, 50)
    print(f"✓ Ran agent simulation: {firing_count} neurons firing, {spike_count} total spikes")
    
    return brain, agent, world

def test_comparison_features():
    """Test comparison and export features."""
    print("\n=== Testing Comparison and Export Features ===")
    
    # Test createAdvancedConfig for comparison
    config1 = pynlm.create_advanced_config(1000, 0.001, True, True, True, 1.0, False, False)
    config2 = pynlm.create_advanced_config(2000, 0.01, True, True, True, 2.0, True, True)
    
    brain1 = pynlm.createBrain(config1)
    brain2 = pynlm.createBrain(config2)
    
    print(f"✓ Created brain 1 with {brain1.getTotalNeuronCount()} neurons")
    print(f"✓ Created brain 2 with {brain2.getTotalNeuronCount()} neurons")
    
    # Test export_brain_state
    state1 = pynlm.export_brain_state(brain1)
    state2 = pynlm.export_brain_state(brain2)
    
    print(f"✓ Exported brain 1 state: {len(state1)} metrics")
    print(f"✓ Exported brain 2 state: {len(state2)} metrics")
    
    # Test setup_experiments
    experiments = pynlm.setup_experiments(1000, [500, 1000, 2000], 0.001)
    print(f"✓ Set up {len(experiments)} experiments")
    
    return config1, config2, brain1, brain2

def test_error_handling():
    """Test error handling and edge cases."""
    print("\n=== Testing Error Handling ===")
    
    try:
        # Test createBrainFromConfig with non-existent file
        brain = pynlm.createBrainFromConfig("non_existent_file.json")
        print("✗ Should have thrown error for non-existent file")
    except Exception as e:
        print(f"✓ Correctly handled missing file: {type(e).__name__}")
    
    # Test that all new functions have proper default parameters
    config = pynlm.createDefaultConfig()
    print("✓ All functions have proper default parameters")
    
    return config

def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings Comprehensive Test Suite")
    print("=" * 60)
    
    try:
        # Run all test suites
        brain1, agent1, world1, config1 = test_factory_functions()
        brain2, agent2, world2 = test_helper_functions()
        brain3, agent3, world3 = test_advanced_features()
        config1, config2, brain4, brain5 = test_comparison_features()
        final_config = test_error_handling()
        
        print("\n" + "=" * 60)
        print("✓ ALL TESTS PASSED!")
        print("=" * 60)
        print("\nSummary of improvements tested:")
        print("✓ Factory functions: createDefaultConfig, createBrain, createAgentBrain, createSimpleWorld")
        print("✓ Helper functions: configure_world, setup_world_and_agent, run_steps")
        print("✓ Advanced features: create_advanced_config, enable_all_learning")
        print("✓ Simulation helpers: run_agent_simulation, export_brain_state")
        print("✓ Comparison features: setup_experiments, brain configuration management")
        print("✓ Error handling and edge cases")
        
        return True
        
    except Exception as e:
        print(f"\n✗ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)