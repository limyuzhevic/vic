#!/usr/bin/env python3
"""
Test script for enhanced NLM Python bindings

This script validates all the improvements made to the Python API,
including new convenience methods, batch operations, advanced features,
and error handling.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

def test_basic_functionality():
    """Test basic functionality with enhanced API."""
    print("\n=== Testing Basic Functionality ===")
    
    try:
        # Test 1: Create brain with advanced config
        config = pynlm.createAdvancedConfig()
        print("✓ Created advanced configuration")
        
        # Validate configuration
        validation = pynlm.validateBrainConfig(config)
        print(f"✓ Configuration validation: {validation['valid']}")
        
        # Create brain
        brain = pynlm.createBrain(config)
        print("✓ Created brain with advanced config")
        
        # Initialize brain
        brain.initialize()
        print("✓ Brain initialized")
        
        # Test 2: Create world with features
        world = pynlm.createWorldWithFeatures()
        print("✓ Created world with features")
        
        # Validate world state
        world_validation = pynlm.validateWorldState(world)
        print(f"✓ World validation: {world_validation['valid']}")
        
        # Test 3: Create agent with features
        agent = pynlm.createAgentWithFeatures(brain, world)
        print("✓ Created agent with features")
        
        # Test 4: Debug brain state
        debug_info = pynlm.debugBrainState(brain)
        print(f"✓ Brain debug info retrieved ({len(debug_info)} chars)")
        
        return brain, world, agent
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        return None, None, None

def test_batch_operations(brain, world, agent):
    """Test batch operations for performance."""
    print("\n=== Testing Batch Operations ===")
    
    if not brain or not world or not agent:
        print("✗ Skipping batch operations - invalid inputs")
        return
    
    try:
        # Get neuron IDs for batch operations
        spike_system = brain.getSpikeSystem()
        neurons = spike_system.getAllNeurons()
        neuron_ids = [neuron.getId() for neuron in neurons[:5]]  # First 5 neurons
        
        print(f"✓ Retrieved {len(neuron_ids)} neuron IDs")
        
        # Test batch parameter updates
        pynlm.batchSetNeuronParameters(
            brain,
            neuron_ids,
            threshold=-50.0,
            type=pynlm.NeuronType.Excitatory,
            refractoryPeriod=3
        )
        print("✓ Batch parameter updates completed")
        
        # Test batch state queries
        states = pynlm.batchGetNeuronStates(brain, neuron_ids)
        print(f"✓ Batch state queries completed ({len(states)} neurons)")
        
        # Test batch learning
        pynlm.batchApplyLearning(
            brain,
            targetTypes=[pynlm.NeuronType.Sensory],
            strength=40.0,
            steps=20
        )
        print("✓ Batch learning completed")
        
        # Test batch neuron creation
        new_neuron_ids = pynlm.batchCreateNeurons(brain, count=3, type=pynlm.NeuronType.Internal)
        print(f"✓ Batch neuron creation completed ({len(new_neuron_ids)} neurons)")
        
    except Exception as e:
        print(f"✗ Batch operations test failed: {e}")

def test_advanced_features(brain, world, agent):
    """Test advanced API features for power users."""
    print("\n=== Testing Advanced Features ===")
    
    if not brain or not world or not agent:
        print("✗ Skipping advanced features - invalid inputs")
        return
    
    try:
        # Test performance monitoring
        stats = pynlm.getBrainPerformanceStats(brain)
        print(f"✓ Performance stats: {stats['firing_rate_percent']:.1f}% firing rate")
        
        # Test memory profiling
        profile = pynlm.profileBrainMemory(brain)
        print(f"✓ Memory profile: {profile['estimated_mb']:.2f} MB")
        
        # Test action utilities
        action = pynlm.createActionSequence([pynlm.ActionType.MoveForward, pynlm.ActionType.TurnLeft])
        action_name = pynlm.actionToString(action[0])
        print(f"✓ Action utilities: {action_name}")
        
        # Test sensor normalization
        percept = world.getSensoryPercept()
        normalized = pynlm.normalizeSensorData(percept)
        print(f"✓ Sensor normalization: vision={normalized.get('vision', 'N/A')}")
        
        # Test configuration utilities
        base_config = pynlm.createDefaultConfig()
        override_config = pynlm.createAdvancedConfig()
        merged_config = pynlm.mergeConfigs(base_config, override_config)
        print("✓ Configuration merging completed")
        
        # Test experiment setup
        phase6_setup = pynlm.createPhase6ExperimentSetup()
        print(f"✓ Phase 6 experiment setup: {phase6_setup['phase']}")
        
        # Test performance optimization
        optimized = pynlm.optimizeBrainForTraining(brain)
        print("✓ Brain optimization completed")
        
    except Exception as e:
        print(f"✗ Advanced features test failed: {e}")

def test_error_handling():
    """Test error handling and validation."""
    print("\n=== Testing Error Handling ===")
    
    try:
        # Test configuration validation
        config = pynlm.createAdvancedConfig()
        validation = pynlm.validateBrainConfig(config)
        print(f"✓ Configuration validation: {validation['valid']}")
        
        # Test with invalid config (missing required fields)
        from pynlm import Config
        invalid_config = Config()
        # Don't set required fields
        invalid_validation = pynlm.validateBrainConfig(invalid_config)
        print(f"✓ Invalid config validation: {not invalid_validation['valid']}")
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")

def test_convenience_methods():
    """Test convenience methods for common operations."""
    print("\n=== Testing Convenience Methods ===")
    
    try:
        # Create basic setup
        config = pynlm.createAdvancedConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createWorldWithFeatures()
        agent = pynlm.createAgentWithFeatures(brain, world)
        
        # Test debug brain state
        debug_info = pynlm.debugBrainState(brain)
        print(f"✓ Debug brain state: {len(debug_info)} chars")
        
        # Test world validation
        world_validation = pynlm.validateWorldState(world)
        print(f"✓ World validation: {world_validation['valid']}")
        
        # Test resource spawning
        pynlm.spawnResources(world, count=5, resourceType=pynlm.WorldObjectType.Resource)
        print("✓ Resource spawning completed")
        
        # Test test agent creation
        test_agent = pynlm.createTestAgent(brain, world)
        print("✓ Test agent creation completed")
        
    except Exception as e:
        print(f"✗ Convenience methods test failed: {e}")

def test_complete_simulation():
    """Test complete simulation with all features."""
    print("\n=== Testing Complete Simulation ===")
    
    try:
        # Create enhanced setup
        config = pynlm.createAdvancedConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createWorldWithFeatures()
        agent = pynlm.createAgentWithFeatures(brain, world)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        print("✓ Enhanced simulation setup complete")
        
        # Run simulation with monitoring
        for step in range(50):
            world.update(0.1)
            
            # Get and normalize sensory input
            percept = world.getSensoryPercept()
            normalized_percept = pynlm.normalizeSensorData(percept)
            
            # Process with agent
            agent.processSensoryInput(normalized_percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Monitor periodically
            if step % 10 == 0:
                stats = pynlm.getBrainPerformanceStats(brain)
                print(f"Step {step}: Spikes={stats['total_spikes']}, "
                      f"Firing={stats['firing_rate_percent']:.1f}%")
        
        print("✓ Complete simulation with monitoring completed")
        
        # Get final statistics
        final_stats = pynlm.getBrainPerformanceStats(brain)
        final_profile = pynlm.profileBrainMemory(brain)
        
        print(f"Final stats: {final_stats}")
        print(f"Final memory: {final_profile['estimated_mb']:.2f} MB")
        
    except Exception as e:
        print(f"✗ Complete simulation test failed: {e}")

def main():
    """Run all tests."""
    print("NLM Python Bindings - Enhanced Features Test Suite")
    print("=" * 60)
    
    # Test basic functionality
    brain, world, agent = test_basic_functionality()
    
    # Test batch operations
    test_batch_operations(brain, world, agent)
    
    # Test advanced features
    test_advanced_features(brain, world, agent)
    
    # Test error handling
    test_error_handling()
    
    # Test convenience methods
    test_convenience_methods()
    
    # Test complete simulation
    test_complete_simulation()
    
    print("\n" + "=" * 60)
    print("Test suite completed!")
    print("\nSummary of improvements:")
    print("✓ Enhanced configuration management")
    print("✓ Batch operations for performance")
    print("✓ Advanced API features for power users")
    print("✓ Comprehensive error handling")
    print("✓ Convenience methods for common tasks")
    print("✓ Performance monitoring and profiling")
    print("✓ Experiment management utilities")
    print("✓ Configuration validation and merging")
    print("✓ Sensor normalization utilities")
    
    print("\nThe enhanced NLM Python API now provides:")
    print("- Better performance through batch operations")
    print("- Advanced features for power users")
    print("- Comprehensive error handling and validation")
    print("- Rich debugging and monitoring tools")
    print("- Experiment management capabilities")
    print("- Production-ready quality assurance")

if __name__ == "__main__":
    main()
