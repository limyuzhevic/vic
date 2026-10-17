#!/usr/bin/env python3
"""
Enhanced NLM Python Example
Demonstrates the new enhanced_pynlm API with comprehensive features.
"""

import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), 'python'))

# Import the enhanced bindings
try:
    import enhanced_pynlm as nlm
    print("Successfully imported enhanced_pynlm")
except ImportError as e:
    print(f"Failed to import enhanced_pynlm: {e}")
    print("Please build the enhanced bindings first.")
    sys.exit(1)

def example_basic_usage():
    """Example 1: Basic usage of the enhanced API"""
    print("\n=== Example 1: Basic Usage ===")
    
    # Create enhanced configuration with defaults
    config = nlm.Config.create_default()
    config.set("random_seed", 12345)
    
    # Create brain
    brain = nlm.Brain(config)
    print(f"Created brain with {brain.total_neurons()} neurons")
    
    # Initialize brain
    if brain.initialize():
        print("Brain initialized successfully")
    else:
        print("Failed to initialize brain")
        return
    
    # Create world
    world = nlm.World()
    world.configure(width=20.0, height=20.0, vision_width=16, vision_height=16)
    world.reset()
    print(f"Created world with dimensions {world.width()}x{world.height()}")
    
    # Set agent start
    world.set_agent_start(10.0, 10.0)
    
    # Main simulation loop
    print("\nRunning simulation...")
    for step in range(10):
        brain.step(step)
        world.update(0.01)
        
        if step % 2 == 0:
            body = world.get_agent_body()
            print(f"Step {step}: "
                  f"Pos=({body.x:.1f},{body.y:.1f}), "
                  f"Energy={body.energy:.1f}, "
                  f"Firing rate={brain.firing_rate():.3f}")

def example_learning_with_curiosity():
    """Example 2: Learning simulation with curiosity and exploration"""
    print("\n=== Example 2: Learning with Curiosity ===")
    
    # Create configuration for learning
    config = nlm.Config.create_default()
    config.set("random_seed", 42)
    config.set("curiosity_enabled", True)
    config.set("reward_modulation_enabled", True)
    
    # Create brain, world, and agent brain
    brain = nlm.Brain(config)
    world = nlm.World()
    world.configure(width=30.0, height=30.0, vision_width=32, vision_height=32)
    world.reset()
    
    agent_brain = nlm.AgentBrain(brain)
    agent_brain.initialize(world)
    
    # Enable all neuromodulation systems
    agent_brain.enable_reward_modulation(True)
    agent_brain.enable_structural_plasticity(True)
    agent_brain.enable_development(True)
    agent_brain.enable_curiosity(True)
    
    # Add some interesting objects to world
    world.add_object(5.0, 5.0, nlm.WorldObjectType.Resource, 50.0)
    world.add_object(25.0, 25.0, nlm.WorldObjectType.Hazard, -30.0)
    
    # Run learning episode
    print("Running learning episode...")
    for step in range(20):
        # Get observations
        percept = world.get_sensory_percept()
        body = world.get_agent_body()
        
        # In a real scenario, process_sensory_input would be called here
        # agent_brain.process_sensory_input(percept)
        
        # Get action from brain
        action = agent_brain.decode_motor_command()
        
        # Apply action
        result = world.apply_action(action)
        
        # Apply reward
        agent_brain.apply_reward(result["reward"])
        
        # Update development
        agent_brain.update_development(0.01)
        
        # Update world
        world.update(0.01)
        
        # Print statistics
        if step % 5 == 0:
            curiosity = agent_brain.curiosity_level()
            novelty = agent_brain.novelty_level()
            neuromod = agent_brain.neuromodulation_level()
            print(f"Step {step}: "
                  f"Pos=({body.x:.1f},{body.y:.1f}), "
                  f"Energy={body.energy:.1f}, "
                  f"Curiosity={curiosity:.3f}, "
                  f"Novelty={novelty:.3f}, "
                  f"Neuromod={neuromod:.3f}")

def example_experiment_management():
    """Example 3: Experiment management with data collection"""
    print("\n=== Example 3: Experiment Management ===")
    
    # Create configuration for experiment
    config = nlm.Config.create_default()
    config.set("random_seed", 123)
    config.set("max_energy", 150.0)
    
    # Create simulation objects
    brain = nlm.Brain(config)
    world = nlm.World()
    world.configure(width=25.0, height=25.0, vision_width=20, vision_height=20)
    world.reset()
    
    agent_brain = nlm.AgentBrain(brain)
    agent_brain.initialize(world)
    
    # Enable advanced features
    agent_brain.enable_reward_modulation(True)
    agent_brain.enable_curiosity(True)
    
    # Add experiment objects
    world.add_object(5.0, 5.0, nlm.WorldObjectType.Resource, 40.0)
    world.add_object(20.0, 20.0, nlm.WorldObjectType.Hazard, -20.0)
    
    # Run multiple episodes
    print("Running experiment with 3 episodes...")
    total_rewards = []
    
    for episode in range(3):
        print(f"\n--- Episode {episode + 1} ---")
        
        # Reset for new episode
        agent_brain.reset()
        world.reset()
        
        episode_reward = 0.0
        
        # Run episode
        for step in range(15):
            # Get state
            body = world.get_agent_body()
            
            # Choose action
            action = agent_brain.decode_motor_command()
            
            # Apply action
            result = world.apply_action(action)
            
            # Apply reward
            agent_brain.apply_reward(result["reward"])
            
            # Update development
            agent_brain.update_development(0.01)
            
            # Update world
            world.update(0.01)
            
            # Accumulate reward
            episode_reward += result["reward"]
            
            # Print step info
            if step % 5 == 0:
                print(f"  Step {step}: "
                      f"Reward={result['reward']:.1f}, "
                      f"Total={episode_reward:.1f}, "
                      f"Energy={body.energy:.1f}")
        
        total_rewards.append(episode_reward)
        print(f"Episode {episode + 1} total reward: {episode_reward:.1f}")
    
    # Print experiment summary
    print(f"\n=== Experiment Summary ===")
    print(f"Episodes run: {len(total_rewards)}")
    print(f"Average reward per episode: {sum(total_rewards) / len(total_rewards):.2f}")
    print(f"Best episode reward: {max(total_rewards):.2f}")
    print(f"Learning rate: {(total_rewards[-1] - total_rewards[0]) / total_rewards[0] * 100:.1f}%")

def example_advanced_features():
    """Example 4: Advanced features and configuration"""
    print("\n=== Example 4: Advanced Features ===")
    
    # Create highly customized configuration
    config = nlm.Config.create_default()
    
    # Customize configuration for specific experiment
    config.set("random_seed", 999)
    config.set("max_energy", 200.0)
    config.set("development_enabled", True)
    config.set("prediction_enabled", True)
    config.set("planning_enabled", True)
    config.set("attention_enabled", True)
    
    print("Configuration created:")
    print(f"  Vision resolution: {config.get('vision_width', 16)}x{config.get('vision_height', 16)}")
    print(f"  Max energy: {config.get('max_energy')}")
    print(f"  Development enabled: {config.get('development_enabled')}")
    
    # Create brain with advanced configuration
    brain = nlm.Brain(config)
    brain.initialize()
    
    # Print advanced brain statistics
    print(f"\nAdvanced Brain Statistics:")
    print(f"  Total neurons: {brain.total_neurons()}")
    print(f"  Total synapses: {brain.total_synapses()}")
    print(f"  Active neurons: {brain.active_neurons()}")
    print(f"  Firing rate: {brain.firing_rate():.3f}")
    print(f"  Developmental stage: {brain.developmental_stage()}")
    
    # Create optimized world
    world = nlm.World()
    world.configure(width=40.0, height=40.0, vision_width=64, vision_height=64)
    world.set_random_seed(config.get("random_seed"))
    world.reset()
    
    print(f"\nWorld configured with {world.width()}x{world.height()} dimensions")
    print(f"Vision resolution: {world.width()}x{world.height()}")

def example_complex_environment():
    """Example 5: Complex environment with multiple object types"""
    print("\n=== Example 5: Complex Environment ===")
    
    # Create world
    world = nlm.World()
    world.configure(width=60.0, height=60.0, vision_width=80, vision_height=80)
    world.set_random_seed(42)
    
    print("Creating complex environment...")
    
    # Create complex environment with various object types
    
    # Resource areas (positive reward)
    for x, y in [(10, 10), (30, 20), (50, 30)]:
        world.add_object(x, y, nlm.WorldObjectType.Resource, 50.0 + (x + y), 0.8)
    
    # Hazard areas (negative reward)
    for x, y in [(70, 10), (40, 50)]:
        world.add_object(x, y, nlm.WorldObjectType.Hazard, -30.0 - (x + y), 0.6)
    
    # Navigation markers
    markers = [(20, 20), (40, 40), (60, 20), (20, 60)]
    for x, y in markers:
        world.add_object(x, y, nlm.WorldObjectType.Marker, 0.0, 0.4)
    
    # Boundary walls
    wall_thickness = 2.0
    for x in [0, 60]:
        for y in range(0, 61, 5):
            world.add_object(x, y, nlm.WorldObjectType.Wall, 0.0, wall_thickness)
    
    for y in [0, 60]:
        for x in range(0, 61, 5):
            world.add_object(x, y, nlm.WorldObjectType.Wall, 0.0, wall_thickness)
    
    # Create brain and agent
    config = nlm.Config.create_default()
    config.set("random_seed", 12345)
    
    brain = nlm.Brain(config)
    brain.initialize()
    
    agent_brain = nlm.AgentBrain(brain)
    agent_brain.initialize(world)
    
    print(f"Environment created with:")
    print(f"  World size: {world.width()}x{world.height()}")
    print(f"  Vision resolution: 64x64 pixels")
    
    # Run exploration
    print("\nStarting exploration in complex environment...")
    world.reset()
    
    for step in range(30):
        # Get observations
        body = world.get_agent_body()
        
        # Choose action
        action = agent_brain.decode_motor_command()
        
        # Apply action
        result = world.apply_action(action)
        agent_brain.apply_reward(result["reward"])
        agent_brain.update_development(0.01)
        world.update(0.01)
        
        # Print exploration statistics
        if step % 6 == 0:
            curiosity = agent_brain.curiosity_level()
            novelty = agent_brain.novelty_level()
            print(f"Step {step}: "
                  f"Pos=({body.x:.1f},{body.y:.1f}), "
                  f"Energy={body.energy:.1f}, "
                  f"Curiosity={curiosity:.3f}, "
                  f"Novelty={novelty:.3f}")

def main():
    """Main function to run all examples"""
    print("Enhanced NLM Python Examples")
    print("=" * 40)
    
    try:
        example_basic_usage()
        example_learning_with_curiosity()
        example_experiment_management()
        example_advanced_features()
        example_complex_environment()
        
        print("\n" + "=" * 40)
        print("All examples completed successfully!")
        print("=" * 40)
        
    except Exception as e:
        print(f"\nError during examples: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
