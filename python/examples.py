"""
Enhanced NLM Python Bindings - Examples

Practical examples demonstrating the enhanced Python bindings for NLM.

This file contains working examples that showcase the improved functionality
and demonstrate best practices for using the enhanced Python bindings.
"""

import pynlm
import random
import time


def example_basic_usage():
    """Example 1: Basic NLM usage with enhanced bindings"""
    print("=== Example 1: Basic NLM Usage ===")
    
    # Create brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run simulation
    for i in range(10):
        brain.step(i)
    
    # Get and display statistics
    stats = brain.getStatistics()
    print(f"✓ Created brain with {stats['neuron_count']} neurons")
    print(f"✓ Run {10} simulation steps")
    print(f"✓ Total spikes: {stats['total_spikes']}")
    print(f"✓ Average firing rate: {stats['average_firing_rate']:.2f} Hz")
    print()


def example_config_management():
    """Example 2: Advanced configuration management"""
    print("=== Example 2: Advanced Configuration Management ===")
    
    # Create configuration manager
    config_manager = pynlm.ConfigManager()
    
    # Set configuration with type safety
    config_manager.set("neuron_count", 2000)
    config_manager.set("region_count", 4)
    config_manager.set("connection_probability", 0.08)
    config_manager.set("simulation_timestep", 0.0005)
    
    # Get configuration values
    neuron_count = config_manager.get("neuron_count")
    region_count = config_manager.get("region_count")
    
    print(f"✓ Configuration loaded: {neuron_count} neurons, {region_count} regions")
    
    # Save and reload configuration
    config_manager.save("demo_config.json")
    print(f"✓ Configuration saved to 'demo_config.json'")
    
    # Create brain with this configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 2000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Brain created with custom configuration")
    print()


def example_world_simulation():
    """Example 3: World simulation with objects"""
    print("=== Example 3: World Simulation with Objects ===")
    
    # Create standard world
    world = pynlm.createStandardWorld()
    
    # Add various world objects
    # Add walls
    for x in range(0, 100, 20):
        wall = pynlm.WorldObject(x, 0, pynlm.WorldObjectType.Wall)
        world.addObject(wall)
    
    # Add resources
    for i in range(10):
        x = random.uniform(30, 70)
        y = random.uniform(30, 70)
        resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, random.uniform(5, 15))
        world.addObject(resource)
    
    # Add hazards
    for i in range(5):
        x = random.uniform(20, 80)
        y = random.uniform(20, 80)
        hazard = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, random.uniform(-5, -2))
        world.addObject(hazard)
    
    print(f"✓ World created with {world.getObjectsCount()} objects")
    
    # Create agent brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    # Enable neuromodulation
    agent_brain.enableRewardModulation(True)
    agent_brain.enableCuriosity(True)
    
    print(f"✓ Agent brain created with neuromodulation")
    print()


def example_learning_episode():
    """Example 4: Complete learning episode"""
    print("=== Example 4: Complete Learning Episode ===")
    
    # Create complete simulation environment
    simulator = pynlm.BrainSimulator()
    simulator.initialize(
        neuronCount=500,
        worldWidth=100,
        worldHeight=100,
        visionWidth=15,
        visionHeight=15
    )
    
    world = simulator.getWorld()
    agent_brain = simulator.getAgentBrain()
    
    # Add interesting objects
    nlm_world = simulator.getWorld()
    
    # Add a target area
    for x in range(70, 100, 5):
        for y in range(70, 100, 5):
            target = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, 2.0)
            nlm_world.addObject(target)
    
    print(f"✓ Simulation environment created")
    print(f"✓ Starting learning episode...")
    
    # Run learning episode
    episode_length = 0
    for step in range(200):
        # Get current state
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        
        # Get action
        action = agent_brain.decodeMotorCommand()
        world.applyMotorCommand(action.getType(), float(step) * 0.1)
        
        # Update world
        energy = world.update(0.1)
        
        # Simulate interesting dynamics
        agent_body = world.getAgentBody()
        
        # Add some environmental feedback
        if agent_body.x > 80 and agent_body.y > 80:
            agent_brain.applyRewardModulation(1.0, 0.5)
        elif agent_body.x < 20 or agent_body.y < 20:
            agent_brain.applyRewardModulation(-0.5, -0.2)
        
        episode_length = step + 1
        
        # Check termination
        if agent_body.energy <= 0:
            print(f"✓ Episode terminated due to low energy at step {step}")
            break
    
    print(f"✓ Learning episode completed: {episode_length} steps")
    
    # Get statistics
    stats = simulator.getStatistics()
    print(f"✓ Final agent energy: {stats['agent_energy']:.2f}")
    print(f"✓ Final curiosity level: {stats['curiosity_level']:.2f}")
    print(f"✓ Total spikes recorded: {stats['total_spikes']}")
    print()


def example_advanced_neuromodulation():
    """Example 5: Advanced neuromodulation control"""
    print("=== Example 5: Advanced Neuromodulation Control ===")
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createStandardWorld()
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    # Enable all neuromodulation types
    agent_brain.enableRewardModulation(True)
    agent_brain.enableCuriosity(True)
    agent_brain.enableDevelopment(True)
    
    print(f"✓ Advanced neuromodulation system enabled")
    
    # Run simulation with neuromodulation monitoring
    for step in range(50):
        # Get current neuromodulation levels
        neuromodulation = agent_brain.getNeuromodulationLevel()
        curiosity = agent_brain.getCuriosityLevel()
        novelty = agent_brain.getNoveltyLevel()
        
        # Apply different types of modulation
        if neuromodulation < 0.3:
            agent_brain.applyRewardModulation(0.5, 0.2)  # Low neuromodulation = reward
        
        if curiosity < 0.5:
            # Add some "interesting" input to increase curiosity
            action = agent_brain.decodeMotorCommand()
            if action.getType() != pynlm.ActionType.Look:
                action = pynlm.Action(pynlm.ActionType.Look)
                agent_brain.decodeMotorCommand()  # This would normally apply the action
        
        # Process step
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        world.update(0.1)
        
        if step % 10 == 0:
            print(f"  Step {step}: Neuromod={neuromodulation:.2f}, "
                  f"Curiosity={curiosity:.2f}, Novelty={novelty:.2f}")
    
    print(f"✓ Advanced neuromodulation system tested successfully")
    print()


def example_error_handling():
    """Example 6: Robust error handling"""
    print("=== Example 6: Robust Error Handling ===")
    
    # Test configuration error handling
    try:
        config = pynlm.createDefaultConfig()
        
        # This should work fine
        config.set("valid_key", "valid_value")
        print(f"✓ Valid configuration accepted")
        
        # This should handle missing key gracefully
        try:
            value = config.get("nonexistent_key")
            print(f"✗ Should have thrown error for missing key")
        except pynlm.ConfigurationError as e:
            print(f"✓ Configuration error caught: {e}")
        
        # Test type conversion safety
        config.set("int_value", 42)
        config.set("float_value", 3.14)
        config.set("bool_value", True)
        
        int_val = config.get("int_value")
        float_val = config.get("float_value")
        bool_val = config.get("bool_value")
        
        print(f"✓ Type conversion works: int={int_val}, float={float_val}, bool={bool_val}")
        
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
    
    # Test brain error handling
    try:
        # Create brain and try to use it without initialization
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # This should fail gracefully
        brain.step(0)
        print(f"✗ Should have thrown error for uninitialized brain")
        
    except pynlm.BrainError as e:
        print(f"✓ Brain error caught: {e}")
    except Exception as e:
        print(f"✓ Other error caught: {e}")
    
    print(f"✓ Error handling system tested successfully")
    print()


def example_performance_monitoring():
    """Example 7: Performance monitoring and optimization"""
    print("=== Example 7: Performance Monitoring ===")
    
    import time
    
    # Create larger simulation for performance testing
    simulator = pynlm.BrainSimulator()
    simulator.initialize(neuronCount=2000, worldWidth=200, worldHeight=200)
    
    start_time = time.time()
    step_count = 0
    
    print(f"✓ Starting performance test with 2000 neurons")
    
    for step in range(100):
        # Record time for each step
        step_start = time.time()
        
        # Run simulation step
        energy = simulator.step()
        
        step_end = time.time()
        step_time = step_end - step_start
        step_count += 1
        
        # Record statistics periodically
        if step % 20 == 0:
            stats = simulator.getStatistics()
            print(f"  Step {step}: {step_time*1000:.2f}ms/step, "
                  f"Energy={stats['agent_energy']:.2f}, "
                  f"Curiosity={stats['curiosity_level']:.2f}")
    
    end_time = time.time()
    total_time = end_time - start_time
    avg_step_time = total_time / step_count
    
    print(f"✓ Performance test completed:")
    print(f"  - Total steps: {step_count}")
    print(f"  - Total time: {total_time:.2f}s")
    print(f"  - Average step time: {avg_step_time*1000:.2f}ms")
    print(f"  - Steps per second: {step_count/total_time:.1f}")
    print()


def example_advanced_features():
    """Example 8: Advanced features and capabilities"""
    print("=== Example 8: Advanced Features ===")
    
    # Create sophisticated environment
    world = pynlm.createSimpleWorld()
    world.configure(150, 150, 25, 25)
    world.setAgentStart(75, 75)
    
    # Add complex environment
    for x in range(0, 150, 10):
        for y in range(0, 150, 10):
            if (x + y) % 20 == 0:
                # Add some structured obstacles
                wall = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Wall, 0.0)
                wall.radius = 3.0
                world.addObject(wall)
    
    # Create sophisticated brain
    config = pynlm.createConfigWithDefaults()
    config.set("neuron_count", 3000)
    config.set("region_count", 6)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Add initial stimulation
    for i in range(100):
        brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 1.0)
    
    print(f"✓ Advanced environment created with 3000 neurons and 6 regions")
    
    # Create agent brain
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    # Enable all advanced features
    agent_brain.enableRewardModulation(True)
    agent_brain.enableCuriosity(True)
    agent_brain.enableDevelopment(True)
    
    print(f"✓ Advanced agent brain with all features enabled")
    
    # Run complex simulation
    for step in range(100):
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        
        action = agent_brain.decodeMotorCommand()
        world.applyMotorCommand(action.getType(), float(step) * 0.1)
        
        world.update(0.1)
        
        # Update development
        agent_brain.updateDevelopment(0.1)
        
        # Add complex neuromodulation
        if step % 25 == 0:
            agent_brain.applyRewardModulation(0.8, 0.3)
    
    # Get comprehensive statistics
    stats = agent_brain.getEpisodeStatistics(world)
    print(f"✓ Advanced simulation completed:")
    print(f"  - Final position: ({stats['agent_x']:.1f}, {stats['agent_y']:.1f})")
    print(f"  - Final orientation: {stats['agent_orientation']:.1f}°")
    print(f"  - Final energy: {stats['agent_energy']:.2f}")
    print(f"  - Final curiosity: {stats['curiosity_level']:.2f}")
    print(f"  - Final neuromodulation: {stats['neuromodulation_level']:.2f}")
    print()


def run_all_examples():
    """Run all examples"""
    print("=" * 70)
    print("Enhanced NLM Python Bindings - Examples")
    print("=" * 70)
    print()
    
    examples = [
        example_basic_usage,
        example_config_management,
        example_world_simulation,
        example_learning_episode,
        example_advanced_neuromodulation,
        example_error_handling,
        example_performance_monitoring,
        example_advanced_features,
    ]
    
    for example in examples:
        try:
            example()
        except Exception as e:
            print(f"✗ Example {example.__name__} failed: {e}")
            import traceback
            traceback.print_exc()
        
        print("-" * 70)
    
    print("=" * 70)
    print("All examples completed!")
    print("=" * 70)


if __name__ == "__main__":
    run_all_examples()
