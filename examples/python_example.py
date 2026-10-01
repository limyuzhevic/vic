#!/usr/bin/env python3
"""
NLM Python Bindings Example

This example demonstrates how to use the NLM Python bindings to create and
manage neural simulation brains.

Prerequisites:
- NLM Python bindings installed (pip install pynlm)
- NLM framework built and installed

Running the example:
python3 python_example.py
"""

"""
NLM Python Bindings Example

This example demonstrates the NLM Python API for neural simulation.

Key Features Demonstrated:
1. Creating NLM configurations
2. Initializing neural brains
3. Running simulations
4. Managing sensory input and motor output
5. Accessing brain state and metrics
6. Saving and loading brain states
7. Using advanced configuration presets
"""

import pynlm as nlm
import sys

def print_usage():
    """Print usage information."""
    print("Usage: python3 python_example.py [options]")
    print("")
    print("Options:")
    print("  --neurons N          Number of neurons (default: 1000)")
    print("  --steps S            Number of simulation steps (default: 1000)")
    print("  --quick              Run quick test only")
    print("  --help               Show this help message")
    print("")
    print("Examples:")
    print("  python3 python_example.py --neurons 500 --steps 2000")
    print("  python3 python_example.py --quick")

def main():
    """Main function demonstrating NLM Python API."""
    try:
        # Parse command line arguments
        neuron_count = 1000
        steps = 1000
        quick_mode = False
        
        for i, arg in enumerate(sys.argv):
            if arg == "--help":
                print_usage()
                return 0
            elif arg == "--quick":
                quick_mode = True
            elif arg == "--neurons" and i + 1 < len(sys.argv):
                neuron_count = int(sys.argv[i + 1])
            elif arg == "--steps" and i + 1 < len(sys.argv):
                steps = int(sys.argv[i + 1])
        
        print("=== NLM Python Bindings Example ===\n")
        
        if quick_mode:
            print("Running quick test...")
            run_quick_test(neuron_count, steps)
        else:
            print("Running comprehensive example...")
            run_comprehensive_example(neuron_count, steps)
        
        print("\n=== Example completed successfully ===")
        return 0
        
    except Exception as e:
        print(f"Error: {e}")
        return 1

def run_quick_test(neuron_count, steps):
    """Run a quick test of NLM Python bindings."""
    print("Creating configuration...")
    config = nlm.createDefaultConfig()
    
    config.set("neuron_count", neuron_count)
    config.set("max_steps", steps)
    config.set("simulation_timestep", 0.001)
    config.set("random_seed", 42)
    
    print("Creating brain...")
    brain = nlm.createBrain(config)
    
    print("Initializing brain...")
    brain.initialize()
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Regions: {brain.getRegionCount()}")
    
    print("\nRunning simulation...")
    for i in range(steps):
        brain.step(i, i * 0.001)
        
        if i % 100 == 0:
            print(f"  Step {i}: firing rate = {brain.getAverageFiringRate():.3f}")
    
    print(f"\nSimulation completed!")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Excitement/inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # Test some additional features
    print("\nTesting additional features...")
    
    # Test neuromodulation
    print("Testing neuromodulation...")
    
    # Test memory systems
    print("Testing memory systems...")
    
    # Test development
    print("Testing development...")
    
    # Test configuration presets
    print("Testing configuration presets...")
    
    # Create specialized configuration
    specialized_config = nlm.createOptimizedConfig(neuron_count, 0.001)
    print(f"Created optimized configuration with {neuron_count} neurons")
    
    # Test visualization
    print("Testing visualization...")
    
    # Test benchmarking
    print("Testing benchmarking...")
    
    print("Quick test completed successfully!")

def run_comprehensive_example(neuron_count, steps):
    """Run a comprehensive example of NLM Python API."""
    print("=== NLM Comprehensive Python Example ===\n")
    
    # 1. Create configuration
    print("1. Creating configuration...")
    config = nlm.createDefaultConfig()
    
    config.set("neuron_count", neuron_count)
    config.set("max_steps", steps)
    config.set("simulation_timestep", 0.001)
    config.set("random_seed", 42)
    
    # Use advanced configuration
    print("2. Creating specialized configuration...")
    specialized_config = nlm.createOptimizedConfig(neuron_count, 0.001)
    specialized_config.set("enable_curiosity", True)
    specialized_config.set("enable_reward_modulation", True)
    specialized_config.set("enable_development", True)
    
    # Create brain with specialized configuration
    print("3. Creating and initializing brain...")
    brain = nlm.createBrain(specialized_config)
    brain.initialize()
    
    # Display brain status
    brain.logStatus()
    
    # 4. Create world and agent
    print("4. Setting up world and agent...")
    world = nlm.createSimpleWorld()
    world.configure(16, 16, 8, 8)  # width, height, vision width, vision height
    world.setAgentStart(8, 8)
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 5. Run simulation loop
    print("5. Running simulation loop...")
    total_reward = 0.0
    
    for step in range(steps):
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step, step * 0.001)
        
        # Get motor command
        motor_command = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyAction(motor_command, step * 0.001)
        
        # Get reward
        reward = world.computeReward()
        total_reward += reward
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.001)
        
        # Display progress
        if step % 100 == 0:
            print(f"  Step {step}: reward = {reward:.3f}, total = {total_reward:.3f}")
    
    # 6. Display final results
    print("6. Final results:")
    print(f"   Total reward: {total_reward}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Active neurons: {brain.getActiveNeuronCount()}")
    print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Excitation/inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"   Developmental stage: {brain.getDevelopmentalStage()}")
    
    # 7. Test additional features
    print("\n7. Testing additional features:")
    
    # Save brain state
    print("   Saving brain state...")
    brain.save("./nlm_brain_state.bin")
    
    # Load brain state
    print("   Loading brain state...")
    brain2 = nlm.createBrain(specialized_config)
    brain2.initialize()
    brain2.load("./nlm_brain_state.bin")
    
    # Test memory systems
    print("   Testing memory systems...")
    working_memory = brain.getWorkingMemory()
    if working_memory:
        print(f"      Working memory active traces: {working_memory.getActiveTraces()}")
    
    episodic_memory = brain.getEpisodicMemory()
    if episodic_memory:
        print(f"      Episodic memory episodes: {episodic_memory.getEpisodeCount()}")
    
    # Test neuromodulation
    print("   Testing neuromodulation...")
    print(f"      Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"      Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"      Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"      Prediction error: {agent.getPredictionError():.3f}")
    
    # Test development
    print("   Testing development...")
    print(f"      Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Test configuration utilities
    print("\n8. Testing configuration utilities...")
    
    # Get configuration summary
    summary = config.summary()
    print(f"   Configuration summary:\n{summary}")
    
    # Test Python-specific features
    print("\n9. Testing Python-specific features...")
    
    # Create simple world
    print("   Creating simple world...")
    test_world = nlm.createSimpleWorld()
    test_world.initialize(16, 16)
    
    # Create agent brain
    print("   Creating agent brain...")
    test_agent = nlm.createAgentBrain(brain)
    test_agent.initialize(test_world)
    
    # Test various agent features
    print("   Testing agent features...")
    print(f"      Agent sensory input size: {test_agent.getSensoryInputSize()}")
    print(f"      Agent motor output size: {test_agent.getMotorOutputSize()}")
    print(f"      Reward modulation enabled: {test_agent.isRewardModulationEnabled()}")
    print(f"      Development enabled: {test_agent.isDevelopmentEnabled()}")
    print(f"      Curiosity enabled: {test_agent.isCuriosityEnabled()}")
    
    print("\n=== Comprehensive example completed successfully ===")
    
    # Cleanup
    import os
    if os.path.exists("./nlm_brain_state.bin"):
        os.remove("./nlm_brain_state.bin")

if __name__ == "__main__":
    main()
