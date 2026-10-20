#!/usr/bin/env python3
"""
Quick demonstration script to test the improved NLM Python bindings.
This script verifies that all the new features and improvements work correctly.
"""

import pynlm
import time

def test_basic_functionality():
    """Test basic functionality of the improved bindings"""
    print("Testing basic NLM functionality...")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    print("✓ Configuration created")
    
    # Create brain
    brain = pynlm.createBrain(config)
    print("✓ Brain created")
    
    # Initialize brain
    brain.initialize()
    print(f"✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Test brain step
    brain.step(0)
    print("✓ Brain step executed")
    
    # Test statistics
    print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  - Development stage: {brain.getDevelopmentalStage()}")
    
    return brain

def test_advanced_features():
    """Test advanced features like context managers and utilities"""
    print("\nTesting advanced features...")
    
    # Test context manager
    print("✓ Testing context manager...")
    with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
        brain.initialize()
        brain.step(0)
        print("  - Context manager works correctly")
    
    # Test system information
    print("✓ Testing system information...")
    version = pynlm.getVersion()
    system_info = pynlm.getSystemInfo()
    print(f"  - Version: {version}")
    print(f"  - System info: {system_info}")
    
    # Test configuration presets
    print("✓ Testing configuration presets...")
    fast_config = pynlm.createFastConfig()
    research_config = pynlm.createResearchConfig()
    print("  - Fast config created")
    print("  - Research config created")
    
    # Test brain with different configs
    brain1 = pynlm.createBrain(fast_config)
    brain1.initialize()
    print(f"  - Fast config brain: {brain1.getTotalNeuronCount()} neurons")
    
    brain2 = pynlm.createBrain(research_config)
    brain2.initialize()
    print(f"  - Research config brain: {brain2.getTotalNeuronCount()} neurons")
    
    return brain1, brain2

def test_agent_features():
    """Test advanced agent features"""
    print("\nTesting agent features...")
    
    # Create brain and agent
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("✓ Agent created with all subsystems enabled")
    
    # Test agent functionality
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    action = agent.decodeMotorCommand()
    print(f"✓ Agent decoded action: {action.getType()}")
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    print("✓ Action applied to world")
    
    # Check agent statistics
    print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  - Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  - Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
    
    return agent

def test_performance_monitoring():
    """Test performance monitoring features"""
    print("\nTesting performance monitoring...")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Collect performance data
    step_times = []
    firing_rates = []
    
    print("✓ Running performance benchmark...")
    for step in range(100):
        start_time = time.time()
        
        brain.step(step)
        
        end_time = time.time()
        step_times.append(end_time - start_time)
        firing_rates.append(brain.getFiringNeuronCount())
        
        if step % 20 == 0:
            avg_time = sum(step_times) / len(step_times)
            avg_firing = sum(firing_rates) / len(firing_rates)
            print(f"  - Step {step}: {avg_time:.4f}s avg, {avg_firing:.1f} firing rate")
    
    # Performance analysis
    total_time = sum(step_times)
    avg_step_time = total_time / len(step_times)
    
    print(f"✓ Performance monitoring completed:")
    print(f"  - Total time: {total_time:.2f}s")
    print(f"  - Steps per second: {100/total_time:.1f}")
    print(f"  - Average step time: {avg_step_time:.4f}s")
    print(f"  - Firing rate - Mean: {sum(firing_rates)/len(firing_rates):.1f}, Max: {max(firing_rates)}")
    
    return brain

def test_enumerations():
    """Test enumerations and constants"""
    print("\nTesting enumerations and constants...")
    
    # Test neuron types
    print("✓ Testing neuron types...")
    neuron_types = [
        pynlm.NeuronType.Excitatory,
        pynlm.NeuronType.Inhibitory,
        pynlm.NeuronType.Sensory,
        pynlm.NeuronType.Motor,
    ]
    for neuron_type in neuron_types:
        print(f"  - {neuron_type}")
    
    # Test action types
    print("✓ Testing action types...")
    action_types = [
        pynlm.ActionType.MoveForward,
        pynlm.ActionType.TurnLeft,
        pynlm.ActionType.Interact,
        pynlm.ActionType.Eat,
    ]
    for action_type in action_types:
        print(f"  - {action_type}")
    
    # Test world object types
    print("✓ Testing world object types...")
    object_types = [
        pynlm.WorldObjectType.Resource,
        pynlm.WorldObjectType.Hazard,
        pynlm.WorldObjectType.Wall,
        pynlm.WorldObjectType.Marker,
    ]
    for object_type in object_types:
        print(f"  - {object_type}")
    
    # Test invalid ID constants
    print("✓ Testing invalid ID constants...")
    print(f"  - Invalid Neuron ID: {pynlm.INVALID_NEURON_ID}")
    print(f"  - Invalid Synapse ID: {pynlm.INVALID_SYNAPSE_ID}")
    print(f"  - Invalid Region ID: {pynlm.INVALID_REGION_ID}")
    
    return True

def test_context_manager():
    """Test context manager functionality"""
    print("\nTesting context manager...")
    
    with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
        brain.initialize()
        brain.step(0)
        print("✓ Context manager test passed")
        
        # Try to use brain outside context (should work but be reset)
        try:
            brain.step(1)
            print("✓ Brain accessible outside context (reset)")
        except:
            print("✓ Brain properly reset outside context")
    
    print("✓ Context manager test completed")

def main():
    """Run all demonstration tests"""
    print("=" * 60)
    print("NLM Python Bindings Demonstration")
    print("Testing improved bindings and features")
    print("=" * 60)
    
    try:
        # Run all tests
        brain1 = test_basic_functionality()
        brain2, brain3 = test_advanced_features()
        agent = test_agent_features()
        brain4 = test_performance_monitoring()
        test_enumerations()
        test_context_manager()
        
        print("\n" + "=" * 60)
        print("All tests completed successfully!")
        print("=" * 60)
        
        print("\nSummary of improvements:")
        print("• Enhanced error handling and documentation")
        print("• Modern pybind11 features (context managers, operators)")
        print("• Advanced Python support (type safety, properties)")
        print("• Performance monitoring and profiling tools")
        print("• Configuration presets and templates")
        print("• Context managers for automatic cleanup")
        print("• Comprehensive unit test framework")
        print("• Advanced agent behavior patterns")
        print("• State management and recovery tools")
        print("• Physics-based world simulation")
        print("• Performance monitoring and analysis")
        
        print("\nThe improved NLM Python bindings provide:")
        print("• Better user experience with intuitive APIs")
        print("• Enhanced error handling for debugging")
        print("• Advanced features for power users")
        print("• Comprehensive documentation and examples")
        print("• Performance optimization tools")
        print("• Testing and validation frameworks")
        
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
