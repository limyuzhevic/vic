"""
Example usage of the enhanced NLM Python API

This file demonstrates the new features and improvements in the NLM Python API,
showing how to use the enhanced functionality for both basic and advanced use cases.
"""

# Basic example showing simple brain usage
def basic_brain_example():
    """Basic example: Creating and using a simple brain"""
    print("=== Basic Brain Example ===")
    
    # Create a default configuration
    from pynlm import create_default_config, create_brain
    
    config = create_default_config()
    config.set("neuron_count", 1000)
    config.set("region_count", 2)
    
    # Create brain
    brain = create_brain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.get_total_neuron_count()} neurons")
    print(f"Brain has {brain.get_region_count()} regions")
    print(f"Total synapses: {brain.get_total_synapse_count()}")
    
    # Run a few simulation steps
    for i in range(5):
        brain.step(i)
        print(f"Step {i}: {brain.get_firing_neuron_count()} firing neurons")
    
    return brain

# Enhanced API example showing new features
def enhanced_api_example():
    """Enhanced API example: Using all the new features"""
    print("\n=== Enhanced API Example ===")
    
    from pynlm import (
        create_brain, create_default_config, create_simple_world,
        create_agent_brain, createAction, createWorldObject,
        run_simulation, capture_brain_state, analyze_spiking_patterns
    )
    
    # Create brain with configuration
    config = create_default_config()
    config.set("neuron_count", 2000)
    config.set("region_count", 3)
    config.set("connection_probability", 0.1)
    
    brain = create_brain(config)
    brain.initialize()
    
    # Create world and agent
    world = create_simple_world()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = create_agent_brain(brain)
    agent.initialize(world)
    
    print(f"Created enhanced brain with {brain.get_total_neuron_count()} neurons")
    print(f"Agent configured with {agent.get_sensory_input_size()} sensory inputs")
    print(f"Agent has {agent.get_motor_output_size()} motor outputs")
    
    # Run a simulation
    stats = run_simulation(brain, world, agent, num_steps=10, timestep=0.1)
    
    print(f"Simulation completed in {len(stats['firing_neurons'])} steps")
    print(f"Final firing rate: {stats['firing_neurons'][-1] if stats['firing_neurons'] else 0}")
    print(f"Final curiosity level: {agent.get_curiosity_level():.3f}")
    print(f"Final novelty level: {agent.get_novelty_level():.3f}")
    
    # Capture brain state
    state = capture_brain_state(brain)
    print(f"Brain state captured: {len(state['neurons'])} neuron statistics")
    
    return brain, agent, world, stats

# Neuromodulation example
def neuromodulation_example():
    """Example showing neuromodulation capabilities"""
    print("\n=== Neuromodulation Example ===")
    
    from pynlm import create_brain, create_default_config, create_simple_world, create_agent_brain
    
    # Create brain and agent
    config = create_default_config()
    brain = create_brain(config)
    brain.initialize()
    
    world = create_simple_world()
    agent = create_agent_brain(brain)
    agent.initialize(world)
    
    print("Testing neuromodulation systems...")
    
    # Check neuromodulation levels
    dopamine = brain.get_dopamine()
    curiosity = brain.get_curiosity()
    novelty = brain.get_novelty()
    prediction_error = brain.get_prediction_error()
    
    print(f"Dopamine level: {dopamine.get_level():.3f}")
    print(f"Curiosity level: {curiosity.get_level():.3f}")
    print(f"Novelty level: {novelty.get_level():.3f}")
    print(f"Prediction error: {prediction_error.get_level():.3f}")
    
    # Apply reward modulation
    reward = 1.0
    predicted_reward = 0.8
    agent.apply_reward_modulation(reward, predicted_reward)
    
    print(f"After reward modulation:")
    print(f"  Dopamine: {dopamine.get_level():.3f}")
    print(f"  Prediction error: {prediction_error.get_level():.3f}")
    
    # Apply development updates
    agent.update_development(0.1)
    print(f"Developmental stage: {agent.get_developmental_stage()}")
    print(f"Plasticity modifier: {agent._plasticity_modifier:.3f}")
    
    return brain, agent, world

# Memory system example
def memory_systems_example():
    """Example showing memory system capabilities"""
    print("\n=== Memory Systems Example ===")
    
    from pynlm import create_brain, create_default_config
    
    config = create_default_config()
    brain = create_brain(config)
    brain.initialize()
    
    # Get memory systems
    working_memory = brain.get_working_memory()
    episodic_memory = brain.get_episodic_memory()
    associative_memory = brain.get_associative_memory()
    
    print("Testing memory systems...")
    
    # Test working memory
    working_memory.store(1, 0.5)
    working_memory.store(2, 0.8)
    working_memory.store(3, 0.3)
    
    print(f"Working memory traces: {working_memory.get_active_traces()}")
    print(f"Total active traces: {working_memory.get_total_active_traces()}")
    print(f"Mean activation: {working_memory.get_mean_activation():.3f}")
    
    # Test episodic memory
    episode = {
        'timestamp': 100,
        'reward': 0.5,
        'active_neurons': [1, 2, 3, 4, 5],
        'neuron_activations': [0.1, 0.2, 0.3, 0.4, 0.5]
    }
    episodic_memory.store_episode(episode)
    
    print(f"Episodic memory episodes: {episodic_memory.get_episode_count()}")
    print(f"Recent episodes: {len(episodic_memory.get_recent_episodes(1))}")
    
    # Test associative memory
    pattern1 = [0.1, 0.2, 0.3, 0.4, 0.5]
    pattern2 = [0.5, 0.4, 0.3, 0.2, 0.1]
    
    associative_memory.learn(pattern1, 1.0)
    associative_memory.learn(pattern2, 2.0)
    
    recall1 = associative_memory.recall(pattern1)
    recall2 = associative_memory.recall(pattern2)
    
    print(f"Associative memory associations: {associative_memory.get_association_count()}")
    print(f"Pattern 1 recall: {recall1}")
    print(f"Pattern 2 recall: {recall2}")
    
    return brain, working_memory, episodic_memory, associative_memory

# Prediction system example
def prediction_system_example():
    """Example showing prediction system capabilities"""
    print("\n=== Prediction System Example ===")
    
    from pynlm import create_brain, create_default_config
    
    config = create_default_config()
    brain = create_brain(config)
    brain.initialize()
    
    # Get prediction systems
    prediction_system = brain.get_prediction_system()
    planner = brain.get_planner()
    concept_formation = brain.get_concept_formation()
    attention = brain.get_attention()
    
    print("Testing prediction systems...")
    
    # Test neural planner
    current_state = {'position': (10.0, 10.0), 'orientation': 0.0, 'energy': 50.0}
    available_actions = ['move_forward', 'turn_left', 'turn_right', 'wait']
    
    plan = planner.plan_action(current_state, available_actions)
    print(f"Planned action: {plan['action']}")
    print(f"Plan sequence: {plan['sequence']}")
    print(f"Plan score: {plan['score']:.3f}")
    
    # Test concept formation
    pattern1 = [0.1, 0.2, 0.3, 0.4, 0.5]
    pattern2 = [0.6, 0.7, 0.8, 0.9, 1.0]
    
    concept_formation.learn_pattern(pattern1, 1.0)
    concept_formation.learn_pattern(pattern2, 2.0)
    
    print(f"Discovered {concept_formation.get_concept_count()} concepts")
    print(f"Total patterns learned: {concept_formation.get_pattern_count()}")
    
    # Test attentional selection
    competitors = [1, 2, 3, 4, 5]
    attention_weights = attention.process_competition(competitors)
    
    print(f"Competitors: {competitors}")
    print(f"Attention weights: {[f'{w:.3f}' for w in attention_weights]}")
    print(f"Current focus: {attention.get_current_focus()}")
    
    return brain, prediction_system, planner, concept_formation, attention

# Utility functions example
def utility_functions_example():
    """Example showing utility functions"""
    print("\n=== Utility Functions Example ===")
    
    from pynlm import create_brain, create_default_config, run_simulation, create_simple_world, create_agent_brain, analyze_spiking_patterns
    
    # Create brain and agent
    config = create_default_config()
    brain = create_brain(config)
    brain.initialize()
    
    world = create_simple_world()
    agent = create_agent_brain(brain)
    agent.initialize(world)
    
    print("Testing utility functions...")
    
    # Run simulation
    stats = run_simulation(brain, world, agent, num_steps=20, timestep=0.1)
    
    # Analyze spiking patterns
    spike_counts = stats['spike_counts']
    analysis = analyze_spiking_patterns(spike_counts)
    
    print(f"Simulation analysis:")
    print(f"  Mean firing rate: {analysis['mean_rate']:.3f}")
    print(f"  Pattern: {analysis['pattern']}")
    print(f"  Trend: {analysis['trend']}")
    print(f"  Coefficient of variation: {analysis['coefficient_of_variation']:.3f}")
    
    # Create experiment setup
    from pynlm import create_experiment_setup
    experiment_setup = create_experiment_setup(config)
    
    print(f"Experiment setup: {experiment_setup['brain_config']['neuron_count']} neurons")
    
    return brain, agent, world, stats, analysis

# Main function to run all examples
def main():
    """Run all examples to demonstrate the enhanced NLM Python API"""
    print("NLM Enhanced Python API Examples")
    print("=" * 50)
    
    try:
        # Basic example
        basic_brain_example()
        
        # Enhanced API example
        enhanced_api_example()
        
        # Neuromodulation example
        neuromodulation_example()
        
        # Memory systems example
        memory_systems_example()
        
        # Prediction system example
        prediction_system_example()
        
        # Utility functions example
        utility_functions_example()
        
        print("\n" + "=" * 50)
        print("All examples completed successfully!")
        print("\nThe enhanced NLM Python API provides:")
        print("• Complete access to all NLM functionality")
        print("• Pythonic object-oriented interfaces")
        print("• Enhanced error handling and validation")
        print("• Convenience methods for common operations")
        print("• Memory, prediction, and cognition systems")
        print("• Neuromodulation and development support")
        print("• Performance monitoring and analysis tools")
        print("• Comprehensive factory functions for quick setup")
        
    except Exception as e:
        print(f"\nError running examples: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()
