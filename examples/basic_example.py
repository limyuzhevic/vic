#!/usr/bin/env python3
"""
Example demonstrating the improved NLM Python API
"""

from nlm import Brain, World, AgentBrain, NLMConfig, MotorCommand

def main():
    print("=== NLM Python API Example ===\n")
    
    # Create configuration
    config = NLMConfig(
        neuron_count=500,
        region_count=2,
        environment_width=20.0,
        environment_height=20.0,
        random_seed=42
    )
    
    print(f"Configuration: {config.neuron_count} neurons, {config.region_count} regions")
    
    # Create brain
    print("Creating brain...")
    brain = Brain(config)
    
    # Initialize brain
    print("Initializing brain...")
    brain.initialize()
    brain.log_status()
    
    # Create world
    print("Creating world...")
    world = World(width=20.0, height=20.0, random_seed=42)
    
    # Create agent brain
    print("Creating agent brain...")
    agent_brain = AgentBrain(brain)
    agent_brain.initialize(world)
    
    # Run simulation
    print("Running simulation...")
    for step in range(10):
        # Get sensory input
        percept = world.get_sensory_percept()
        
        # Process sensory input
        agent_brain.process_sensory_input(percept)
        
        # Step brain
        brain.step(step, step * 0.001)
        
        # Decode and apply motor command
        command = agent_brain.decode_motor_command()
        print(f"  Step {step}: Command = {command.to_string()}")
        
        action_result = world.apply_motor_command(command)
        
        # Apply reward modulation
        agent_brain.apply_reward_modulation(action_result.reward)
        
        # Update world and development
        world.update(0.01)
        agent_brain.update_development(0.01)
        
        # Log progress
        if step % 5 == 0:
            print(f"    Reward: {action_result.reward:.2f}, "
                  f"Neuromod: {agent_brain.getNeuromodulationLevel():.2f}, "
                  f"Novelty: {agent_brain.getNoveltyLevel():.2f}")
    
    print("\n=== Simulation Complete ===")
    print(f"Total spikes: {brain.get_total_spike_count()}")
    print(f"Firing neurons: {brain.get_firing_neuron_count()}")
    print(f"Average firing rate: {brain.get_average_firing_rate():.2f}")
    
    # Test configuration management
    print("\n=== Configuration Management Test ===")
    
    # Create a new configuration from file
    config.save("test_config.json")
    print(f"Saved configuration to test_config.json")
    
    config2 = NLMConfig.load("test_config.json")
    print(f"Loaded configuration: {config2.neuron_count} neurons")
    
    # Test advanced features
    print("\n=== Advanced Features Test ===")
    
    # Create brain with advanced configuration
    advanced_config = NLMConfig(
        neuron_count=1000,
        region_count=3,
        connection_probability=0.15,
        simulation_timestep=0.0005,
        enable_checkpointing=True,
        enable_development=True
    )
    
    brain2 = Brain(advanced_config)
    brain2.initialize()
    print(f"Created advanced brain with {brain2.get_total_neuron_count()} neurons")
    
    print("\n=== Example Complete ===")

if __name__ == "__main__":
    main()
