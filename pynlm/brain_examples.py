"""
NLM Brain Examples
===================

This module contains examples demonstrating the enhanced NLM Python API.
Each example shows different capabilities of the brain simulation framework.
"""

from .pynlm import create_brain, create_simple_world, create_agent, load_config_template, Config
def simple_brain_example():
    """Example 1: Create and run a simple brain.
    
    This demonstrates the basic usage of creating a brain, initializing it,
    and running it for several steps to see neural activity.
    """
    print("Example 1: Simple Brain")
    print("-" * 30)
    
    # Create a simple brain with default configuration
    brain = create_brain()
    print(f"Created brain with {brain.get_stats()['total_neurons']} neurons")
    
    # Initialize the brain
    brain.initialize()
    print("Brain initialized")
    
    # Run for 100 steps
    for i in range(100):
        brain.step(i)
    
    # Get statistics
    stats = brain.get_stats()
    print(f"Results after {i+1} steps:")
    print(f"  - Total spikes: {stats['total_spikes']}")
    print(f"  - Active neurons: {stats['active_neurons']}")
    print(f"  - Firing neurons: {stats['firing_neurons']}")
    print(f"  - Average firing rate: {stats['average_firing_rate']:.2f} Hz")
    print(f"  - Developmental stage: {stats['developmental_stage']}")
    print()


def basic_agent_world_example():
    """Example 2: Create an agent in a world.
    
    This shows how to create a complete agent-world simulation where
    the brain interacts with its environment.
    """
    print("Example 2: Agent in World")
    print("-" * 30)
    
    # Create brain, world, and agent
    brain = create_brain({"neuron_count": 200, "connection_probability": 0.2})
    world = create_simple_world(width=20, height=20, vision_width=8, vision_height=8)
    agent = create_agent(brain, world)
    
    print(f"Created world ({world.get_simulation_time():.1f}x{world.get_simulation_time():.1f})")
    print(f"Agent sensory input size: {agent.get_sensory_input_size()}")
    print(f"Agent motor output size: {agent.get_motor_output_size()}")
    
    # Enable learning features
    agent.enable_feature('reward_modulation', True)
    agent.enable_feature('curiosity', True)
    print("Enabled reward modulation and curiosity")
    
    # Run simulation
    steps = 50
    total_reward = 0.0
    
    for step in range(steps):
        world.update(0.1)
        percept = world.get_sensory_percept()
        agent.process_sensory(percept)
        brain.step(step)
        action = agent.decode_action()
        world.apply_action(action, world.get_simulation_time())
        
        # Simulate simple reward based on action type
        reward = 1.0 if action.getType() != agent._agent.getBrain().produceAction().getType() else 0.0
        agent.apply_reward(reward)
        total_reward += reward
        
        if step % 10 == 0:
            print(f"Step {step}: Action={action.getType()}, Reward={reward}, "
                  f"Development Stage={agent.get_development_stage()}, "
                  f"Curiosity={agent.get_curiosity_level():.2f}")
    
    stats = brain.get_stats()
    print(f"\nSimulation results after {steps} steps:")
    print(f"  - Total reward: {total_reward}")
    print(f"  - Average reward: {total_reward/steps:.2f}")
    print(f"  - Final curiosity level: {agent.get_curiosity_level():.2f}")
    print(f"  - Final novelty level: {agent.get_novelty_level():.2f}")
    print(f"  - Final prediction error: {agent.get_prediction_error():.4f}")
    print()


def learning_example():
    """Example 3: Learning and development.
    
    This demonstrates how the brain learns through experience,
    including reward-based learning and developmental changes.
    """
    print("Example 3: Learning and Development")
    print("-" * 30)
    
    # Create brain with learning-friendly configuration
    config = load_config_template('rich')
    brain = create_brain(config)
    world = create_simple_world(width=30, height=30, vision_width=12, vision_height=12)
    agent = create_agent(brain, world)
    
    # Enable all learning features
    agent.enable_feature('reward_modulation', True)
    agent.enable_feature('structural_plasticity', True)
    agent.enable_feature('development', True)
    agent.enable_feature('curiosity', True)
    
    print(f"Initialized rich learning environment with {config['neuron_count']} neurons")
    
    # Run learning simulation
    steps = 200
    learning_data = []
    
    for step in range(steps):
        world.update(0.1)
        percept = world.get_sensory_percept()
        agent.process_sensory(percept)
        brain.step(step)
        action = agent.decode_action()
        world.apply_action(action, world.get_simulation_time())
        
        # Create reward based on random chance and curiosity
        import random
        base_reward = 0.1
        curiosity_bonus = agent.get_curiosity_level() * 0.5
        exploration_bonus = random.random() * 0.3 if curiosity_bonus > 0.5 else 0.0
        
        reward = base_reward + curiosity_bonus + exploration_bonus
        
        # Store current state
        learning_data.append({
            'step': step,
            'reward': reward,
            'curiosity': agent.get_curiosity_level(),
            'novelty': agent.get_novelty_level(),
            'prediction_error': agent.get_prediction_error(),
            'development_stage': str(agent.get_development_stage())
        }n
        )
        
        agent.apply_reward(reward, 0.0)  # No predicted reward
        
        # Update development
        agent.update_development(0.1)
    
    # Analyze learning results
    print(f"Learning completed over {steps} steps:")
    print(f"  - Average curiosity: {sum(d['curiosity'] for d in learning_data)/steps:.2f}")
    print(f"  - Average novelty: {sum(d['novelty'] for d in learning_data)/steps:.2f}")
    print(f"  - Average prediction error: {sum(d['prediction_error'] for d in learning_data)/steps:.4f}")
    
    # Check developmental progression
    initial_stage = learning_data[0]['development_stage']
    final_stage = learning_data[-1]['development_stage']
    print(f"  - Development: {initial_stage} → {final_stage}")
    
    # Show learning curve
    print("\nLearning curve (every 50 steps):")
    for i in range(0, steps, 50):
        if i < len(learning_data):
            d = learning_data[i]
            print(f"  Step {d['step']}: Reward={d['reward']:.2f}, "
                  f"Curiosity={d['curiosity']:.2f}, Novelty={d['novelty']:.2f}")
    print()


def benchmarking_example():
    """Example 4: Performance benchmarking.
    
    This shows how to benchmark different configurations
    to find the optimal balance between performance and learning capacity.
    """
    print("Example 4: Performance Benchmarking")
    print("-" * 30)
    
    from .pynlm import benchmark_performance
    
    configurations = {
        "Small (50 neurons)": {"neuron_count": 50, "connection_probability": 0.1},
        "Medium (200 neurons)": {"neuron_count": 200, "connection_probability": 0.2},
        "Large (500 neurons)": {"neuron_count": 500, "connection_probability": 0.3},
        "Very Large (1000 neurons)": {"neuron_count": 1000, "connection_probability": 0.4},
    }
    
    results = {}
    
    for name, config in configurations.items():
        print(f"Benchmarking {name}...")
        try:
            result = benchmark_performance(config, steps=500)
            results[name] = result
            print(f"  - Steps per second: {result['benchmark']['steps_per_second']:.0f}")
            print(f"  - Time per step: {result['benchmark']['time_per_step_ms']:.2f} ms")
        except Exception as e:
            print(f"  - Failed: {e}")
            results[name] = None
    
    print("\nBenchmark summary:")
    successful = {k: v for k, v in results.items() if v is not None}
    if successful:
        best = max(successful.items(), key=lambda x: x[1]['benchmark']['steps_per_second'])
        print(f"Best performance: {best[0]}")
        print(f"  - {best[1]['benchmark']['steps_per_second']:.0f} steps/second")
        print(f"  - {best[1]['benchmark']['time_per_step_ms']:.2f} ms/step")
    
    print()


def configuration_examples():
    """Example 5: Working with configurations.
    
    This demonstrates different ways to create and manage configurations.
    """
    print("Example 5: Configuration Management")
    print("-" * 30)
    
    from .pynlm import create_default_config, create_brain, load_config_template
    
    # Example 1: Default configuration
    print("1. Default configuration:")
    default_config = create_default_config()
    print(f"   Neurons: {default_config.get('neuron_count')}")
    print(f"   Connections: {default_config.get('connection_probability')}")
    
    # Example 2: Template configuration
    print("\n2. Template configurations:")
    templates = ['simple', 'rich', 'fast']
    for template in templates:
        config_dict = load_config_template(template)
        print(f"   {template:10} | Neurons: {config_dict['neuron_count']:4d} | "
              f"Connections: {config_dict['connection_probability']}")
    
    # Example 3: Custom configuration
    print("\n3. Custom configuration:")
    custom_config = Config({
        "neuron_count": 300,
        "connection_probability": 0.25,
        "random_seed": 123,
        "enable_development": True
    }, name="Custom Learning")
    
    brain = create_brain(custom_config.to_dict(), "Custom Brain")
    brain.initialize()
    for i in range(50):
        brain.step(i)
    
    stats = brain.get_stats()
    print(f"   Custom brain stats: {stats['total_spikes']} spikes, "
          f"{stats['average_firing_rate']:.2f} Hz average")
    
    # Example 4: Save and load configuration
    print("\n4. Configuration persistence:")
    save_path = "/tmp/test_config.json"
    
    # Save configuration
    save_config_to_file(custom_config.to_dict(), save_path)
    print(f"   Saved configuration to {save_path}")
    
    # Load configuration
    loaded_config = load_config_from_file(save_path)
    print(f"   Loaded configuration with {len(loaded_config)} parameters")
    
    print()


if __name__ == "__main__":
    print("NLM Brain Examples")
    print("==================")
    print()
    
    try:
        simple_brain_example()
        basic_agent_world_example()
        learning_example()
        benchmarking_example()
        configuration_examples()
        
        print("All examples completed successfully!")
        
    except Exception as e:
        print(f"Error running examples: {e}")
        import traceback
        traceback.print_exc()
