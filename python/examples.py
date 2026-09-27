"""
NLM Python Examples
===================

This file contains executable examples demonstrating the NLM Python API.
Run these examples to understand how to use the neural simulation framework.

To run these examples:
1. Save this file as examples.py
2. Run: python examples.py
3. Or run individual examples: python example_basic.py
"""

import sys
import os

# Add the current directory to Python path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def example_basic():
    """Basic NLM usage - just create and run a brain."""
    print("=== Example 1: Basic NLM Usage ===")
    
    # Create a brain with default configuration
    brain = pynlm.createBrain()
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Initialize the brain
    brain.initialize()
    print("Brain initialized")
    
    # Run a simple simulation
    for step in range(10):
        brain.step(step)
        if step % 5 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
    print()

def example_with_world():
    """Example with a simple world and agent."""
    print("=== Example 2: Agent in a World ===")
    
    # Create all components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure the world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.setAgentStart(10.0, 10.0)
    
    # Initialize everything
    brain.initialize()
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(50):
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Tell the brain
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 10 == 0:
            print(f"Step {step}:")
            print(f"  Position: {world.getAgentPosition()[0]:.1f}, {world.getAgentPosition()[1]:.1f}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    
    print("Simulation complete!")
    print()

def example_complete_simulation():
    """Complete simulation with all features enabled."""
    print("=== Example 3: Complete Simulation with All Features ===")
    
    def run_agent_simulation(num_steps=1000):
        """Complete agent simulation as shown in documentation."""
        # 1. Create configuration
        config = pynlm.createDefaultConfig()
        
        # 2. Create brain and initialize
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # 3. Create agent brain interface
        agent = pynlm.createAgentBrain(brain)
        
        # 4. Create and configure world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        # 5. Initialize agent with world
        agent.initialize(world)
        
        # 6. Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # 7. Run simulation loop
        for step in range(num_steps):
            # Update world
            world.update(timestep=0.1)
            
            # Get sensory input from world
            percept = world.getSensoryPercept()
            
            # Process sensory input in brain
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode motor command from brain activity
            motor_cmd = agent.decodeMotorCommand()
            
            # Apply motor command to world
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Print progress
            if step % 100 == 0:
                print(f"Step {step}:")
                print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
                print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
                print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
                print(f"  Prediction Error: {agent.getPredictionError():.3f}")
                print(f"  Development Stage: {brain.getDevelopmentalStage()}")
        
        return brain, world, agent
    
    # Run the simulation
    brain, world, agent = run_agent_simulation(100)
    
    print(f"Simulation complete with {brain.getTotalNeuronCount()} neurons")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print()

def example_batch_operations():
    """Example of batch operations with multiple simulations."""
    print("=== Example 4: Batch Operations ===")
    
    # Create multiple simulations
    num_simulations = 3
    simulations = []
    
    for i in range(num_simulations):
        config = pynlm.createDefaultConfig()
        # Vary configuration slightly for each simulation
        config.setFloat("neuron_count", 500 + i * 200)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        world.setAgentStart(i * 10 + 5, i * 10 + 5)
        
        brain.initialize()
        agent.initialize(world)
        
        agent.enableRewardModulation(True)
        
        simulations.append((brain, agent, world))
    
    print(f"Created {num_simulations} simulations")
    
    # Run them all
    for i, (brain, agent, world) in enumerate(simulations):
        print(f"\nRunning simulation {i + 1}:")
        for step in range(20):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"  Neurons: {brain.getTotalNeuronCount()}, Spikes: {brain.getTotalSpikeCount()}")
    
    # Batch reset all simulations
    pynlm.batch_reset(
        [sim[0] for sim in simulations],
        [sim[1] for sim in simulations],
        [sim[2] for sim in simulations]
    )
    
    print("All simulations reset successfully")
    print()

def example_error_handling():
    """Example of error handling and validation."""
    print("=== Example 5: Error Handling ===")
    
    try:
        # Create a brain
        brain = pynlm.createBrain()
        brain.initialize()
        
        # Try to access an invalid neuron ID
        invalid_id = pynlm.INVALID_NEURON_ID
        print(f"Invalid neuron ID: {invalid_id}")
        
        # Try to get a config value that doesn't exist
        config = brain.getConfig()
        print(f"Trying to get non-existent config key: {config.get('non_existent_key', 'NOT_FOUND')}")
        
        # The above should work without errors
        
        print("Error handling example completed successfully")
        
    except Exception as e:
        print(f"Error occurred: {e}")
    
    print()

def example_advanced_configuration():
    """Example of advanced configuration."""
    print("=== Example 6: Advanced Configuration ===")
    
    config = pynlm.createDefaultConfig()
    
    # Set various configuration parameters
    config.setInt("neuron_count", 2000)
    config.setFloat("connection_probability", 0.15)
    config.setFloat("stdp_ltp_weight", 0.02)
    config.setFloat("stdp_ltd_weight", 0.025)
    config.setFloat("stdp_tau", 25.0)
    config.setFloat("synaptogenesis_rate", 0.00005)
    config.setFloat("pruning_rate", 0.000005)
    config.setBool("enable_curiosity", True)
    config.setBool("enable_structural_plasticity", True)
    config.setBool("enable_development", True)
    
    print("Configuration created with:")
    print(f"  Neuron count: {config.getInt('neuron_count')}")
    print(f"  Connection probability: {config.getFloat('connection_probability')}")
    print(f"  STDP LTP weight: {config.getFloat('stdp_ltp_weight')}")
    print(f"  STDP LTD weight: {config.getFloat('stdp_ltd_weight')}")
    print(f"  STDP tau: {config.getFloat('stdp_tau')}")
    print(f"  Synaptogenesis rate: {config.getFloat('synaptogenesis_rate')}")
    print(f"  Pruning rate: {config.getFloat('pruning_rate')}")
    
    # Show all configuration keys
    print(f"All config keys: {config.getKeys()}")
    
    # Save configuration
    try:
        config.saveToFile("example_config.txt")
        print("Configuration saved to example_config.txt")
    except Exception as e:
        print(f"Failed to save configuration: {e}")
    
    print()

def example_neuron_analysis():
    """Example of neuron analysis and statistics."""
    print("=== Example 7: Neuron Analysis ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get basic statistics
    stats = brain.getStats()
    print(f"Basic stats: {stats}")
    
    # Print human-readable stats
    print(f"Total neurons: {stats[0]}")
    print(f"Total synapses: {stats[1]}")
    print(f"Firing neurons: {stats[2]}")
    print(f"Average firing rate: {stats[3]:.2f} Hz")
    print(f"Total spikes: {stats[4]}")
    
    # Check developmental stage
    stage = brain.getDevelopmentalStage()
    print(f"Developmental stage: {stage}")
    
    # Run a few steps to see changes
    for step in range(5):
        brain.step(step)
    
    stats_after = brain.getStats()
    print(f"Stats after 5 steps: {stats_after}")
    
    print(f"Spike increase: {stats_after[4] - stats[4]}")
    print()

def main():
    """Run all examples."""
    print("NLM Python Examples")
    print("===================")
    print()
    
    example_basic()
    example_with_world()
    example_complete_simulation()
    example_batch_operations()
    example_error_handling()
    example_advanced_configuration()
    example_neuron_analysis()
    
    print("All examples completed successfully!")
    print()
    print("For more detailed documentation, see:")
    print("- HOW_TO_USE.md: Comprehensive usage guide")
    print("- easy_usage.md: Quick start guide")
    print("- docs/: Scientific documentation")

if __name__ == "__main__":
    main()
