"""
NLM Python Bindings - Main Usage Examples
=========================================

This file contains the main usage examples for the NLM Python bindings.
It serves as both documentation and executable demonstration.

To run these examples:
    python usage_examples.py
    python -m pynlm.usage_examples
"""

import pynlm

def main():
    """Run all usage examples."""
    print("NLM Python Bindings Usage Examples")
    print("=" * 50)
    print()
    
    # Example 1: Basic brain creation
    print("Example 1: Basic Brain Creation")
    print("-" * 30)
    brain = pynlm.createBrain()
    brain.initialize()
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain reset: {brain.getTotalSpikeCount() == 0}")
    brain.reset()
    print(f"After reset: {brain.getTotalSpikeCount() == 0}")
    print()
    
    # Example 2: Complete simulation with world
    print("Example 2: Complete Simulation with World")
    print("-" * 40)
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.setAgentStart(10, 10)
    
    # Initialize
    brain.initialize()
    agent.initialize(world)
    
    # Enable learning
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(5):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"Simulation completed in {world.getSimulationTime():.1f}s")
    print(f"Agent position: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print()
    
    # Example 3: Configuration management
    print("Example 3: Configuration Management")
    print("-" * 35)
    config = pynlm.createDefaultConfig()
    
    # Set configuration values
    config.setInt("neuron_count", 2000)
    config.setFloat("connection_probability", 0.15)
    config.setFloat("stdp_learning_rate", 0.005)
    config.setBool("enable_curiosity", True)
    
    # Get and display configuration
    print(f"Neuron count: {config.getInt('neuron_count')}")
    print(f"Connection probability: {config.getFloat('connection_probability')}")
    print(f"STDP learning rate: {config.getFloat('stdp_learning_rate')}")
    print(f"Curiosity enabled: {config.getBool('enable_curiosity')}")
    print()
    
    # Example 4: Advanced features
    print("Example 4: Advanced Features")
    print("-" * 25)
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure for advanced simulation
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world.setAgentStart(25, 25)
    
    brain.initialize()
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run advanced simulation
    for step in range(10):
        world.update(0.05)  # Smaller timesteps for more detail
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward for learning
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.05)
        
        if step % 5 == 0:
            print(f"Step {step}: Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}, "
                  f"DevStage={brain.getDevelopmentalStage()}")
    
    print()
    
    # Example 5: Performance benchmarking
    print("Example 5: Performance Benchmarking")
    print("-" * 35)
    import time
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    start_time = time.time()
    for step in range(100):
        brain.step(step)
    end_time = time.time()
    
    steps_per_second = 100 / (end_time - start_time)
    time_per_step_ms = (end_time - start_time) * 1000 / 100
    
    print(f"100 steps completed in {end_time - start_time:.2f}s")
    print(f"Throughput: {steps_per_second:.0f} steps/second")
    print(f"Time per step: {time_per_step_ms:.2f}ms")
    print()
    
    print("=" * 50)
    print("All examples completed successfully!")
    print()
    print("For more detailed documentation, see:")
    print("- python/api_reference.md - Complete API reference")
    print("- python/examples.py - Basic usage examples")
    print("- python/examples_advanced.py - Advanced usage examples")
    print("- python/benchmark_pynlm.py - Performance benchmarks")
    print("- python/test_pynlm.py - Comprehensive tests")
    print("- docs/ - Detailed documentation")

if __name__ == "__main__":
    main()
