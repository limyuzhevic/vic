#!/usr/bin/env python3
"""
Example script demonstrating the enhanced NLM Python API.
This script shows the new convenience methods, validation, and advanced features.
"""

import pynlm
import time

def example_basic_brain():
    """Example 1: Basic brain simulation with enhanced API"""
    print("=== Example 1: Basic Brain Simulation ===")
    
    # Create brain using enhanced factory method
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Run simulation with status reporting
    for i in range(10):
        brain.step(i)
        if i % 5 == 0:
            firing = brain.getFiringNeuronCount()
            total_spikes = brain.getTotalSpikeCount()
            print(f"Step {i}: {firing} firing neurons, {total_spikes} total spikes")
    
    print("Basic brain simulation complete!\n")

def example_enhanced_configuration():
    """Example 2: Advanced configuration with validation"""
    print("=== Example 2: Enhanced Configuration ===")
    
    # Create configuration with validation
    config = pynlm.Config()
    
    # Set configuration with validation
    config.set("neuron_count", 2000)
    config["region_count"] = 5
    config["learning_rate"] = 0.01
    
    # Validate configuration
    try:
        config.validate(["neuron_count", "region_count", "learning_rate"])
        print("Configuration validation passed!")
    except ValueError as e:
        print(f"Configuration validation failed: {e}")
        return
    
    # Get statistics
    stats = config.getNumericStats()
    print(f"Config statistics: mean={stats['mean']:.3f}, median={stats['median']:.3f}")
    
    # Create brain with enhanced config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons and {brain.getRegionCount()} regions\n")

def example_action_enhancements():
    """Example 3: Enhanced action management"""
    print("=== Example 3: Enhanced Action Management ===")
    
    # Create actions using factory methods
    actions = [
        pynlm.Action.create_move_forward(2.0),
        pynlm.Action.create_turn_left(0.5),
        pynlm.Action.create_turn_right(0.5),
        pynlm.Action.create_interact(),
        pynlm.Action.create_wait()
    ]
    
    # Modify actions
    actions[0].setParameters([2.0, 0.1])  # Move with rotation
    actions[1].setType(pynlm.ActionType.Look)  # Look action
    
    # Process action results
    results = []
    for action in actions:
        result = pynlm.ActionResult(
            reward=1.0 if action.getType() != pynlm.ActionType.Wait else 0.5,
            success=True,
            message=f"Executed {action.getName()}"
        )
        results.append(result)
    
    # Get action statistics
    successful = sum(1 for r in results if r.success)
    total_reward = sum(r.reward for r in results)
    
    print(f"Processed {len(actions)} actions")
    print(f"Successful actions: {successful}/{len(actions)}")
    print(f"Total reward: {total_reward:.2f}")
    print()

def example_world_operations():
    """Example 4: Advanced world operations"""
    print("=== Example 4: Advanced World Operations ===")
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.setAgentStart(10.0, 10.0)
    
    # Add world objects
    world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0))
    world.addObject(pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Hazard, value=-5.0))
    
    # Get world statistics
    stats = world.getWorldStatistics()
    obj_stats = world.getObjectStatistics()
    
    print(f"World size: {stats['width']}x{stats['height']}")
    print(f"Total objects: {stats['total_objects']}")
    print(f"Resource nodes: {obj_stats.get('Resource', 0)}")
    print(f"Hazard nodes: {obj_stats.get('Hazard', 0)}")
    
    # Create agent and run episode
    brain = pynlm.createBrain()
    agent = pynlm.createAgentBrain(brain)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run episode
    episode = world.runEpisode(agent, num_steps=50, timestep=0.1)
    
    print(f"Episode completed in {episode['actions']} actions")
    print(f"Total reward: {episode['total_reward']:.2f}")
    print(f"Average curiosity: {episode['avg_curiosity']:.3f}")
    print()

def example_batch_processing():
    """Example 5: Batch processing with multiple brains"""
    print("=== Example 5: Batch Processing ===")
    
    # Create multiple brains with different configurations
    brains = []
    for i in range(3):
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500 + i * 300)
        config.set("learning_rate", 0.01 / (i + 1))
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
    
    # Run steps for all brains
    all_results = []
    for i, brain in enumerate(brains):
        results = brain.runSteps(50, timestep=0.1)
        all_results.append(results)
        
        firing = brain.getFiringNeuronCount()
        spikes = brain.getTotalSpikeCount()
        print(f"Brain {i}: {firing} firing, {spikes} spikes")
    
    # Compare results
    comparison = {
        'neurons': [b.getTotalNeuronCount() for b in brains],
        'firing_rates': [b.getAverageFiringRate() for b in brains],
        'total_spikes': [b.getTotalSpikeCount() for b in brains]
    }
    
    print(f"Comparison: neurons {comparison['neurons']}")
    print(f"Comparison: firing rates {comparison['firing_rates']}")
    print()

def example_validation_and_error_handling():
    """Example 6: Validation and error handling"""
    print("=== Example 6: Validation and Error Handling ===")
    
    # Example 1: Configuration validation
    print("Configuration validation:")
    config = pynlm.Config()
    
    try:
        config.set("learning_rate", 1.5)  # This should fail validation
        print("Learning rate set successfully")
    except ValueError as e:
        print(f"Validation error: {e}")
    
    # Example 2: Action validation
    print("\nAction validation:")
    try:
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        print(f"Action created: {action.getName()}")
    except ValueError as e:
        print(f"Action error: {e}")
    
    # Example 3: Neuron ID validation
    print("\nNeuron ID validation:")
    from pynlm import NeuronId
    valid_id = NeuronId(42)
    invalid_id = pynlm.INVALID_NEURON_ID
    
    try:
        pynlm.validate_neuron_id(valid_id)
        print("Valid neuron ID accepted")
    except ValueError as e:
        print(f"Neuron ID error: {e}")
    
    try:
        pynlm.validate_neuron_id(invalid_id)
        print("Invalid neuron ID accepted")
    except ValueError as e:
        print(f"Neuron ID validation caught error: {e}")
    
    print()

def example_helper_utilities():
    """Example 7: Helper utilities and mathematical operations"""
    print("=== Example 7: Helper Utilities ===")
    
    # Mathematical utilities
    print("Mathematical utilities:")
    value = pynlm.clamp(0.8, 0.0, 1.0)
    print(f"Clamp 0.8 to [0,1]: {value}")
    
    interpolated = pynlm.lerp(0.0, 1.0, 0.75)
    print(f"Lerp from 0.0 to 1.0 at 0.75: {interpolated}")
    
    distance = pynlm.distance([0.0, 0.0], [3.0, 4.0])
    print(f"Distance between (0,0) and (3,4): {distance}")
    
    formatted_time = pynlm.format_time(125.5)
    print(f"Formatted time 125.5s: {formatted_time}")
    
    # Validation utilities
    print("\nValidation utilities:")
    try:
        pynlm.validate_float_range(0.5, 0.0, 1.0, "learning_rate")
        print("Float range validation passed")
    except ValueError as e:
        print(f"Float range error: {e}")
    
    try:
        pynlm.validate_action_type(pynlm.ActionType.MoveForward)
        print("Action type validation passed")
    except ValueError as e:
        print(f"Action type error: {e}")
    
    print()

def example_comprehensive_agent_training():
    """Example 8: Comprehensive agent training with all features"""
    print("=== Example 8: Comprehensive Agent Training ===")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    
    # Add obstacles and resources
    world.addObject(pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Resource))
    world.addObject(pynlm.WorldObject(20.0, 20.0, pynlm.WorldObjectType.Hazard))
    world.addObject(pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Wall))
    
    # Create agent with enhanced features
    brain = pynlm.createBrain()
    agent = pynlm.createAgentBrain(brain)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Train agent
    print("Training agent with curiosity and reward modulation...")
    start_time = time.time()
    
    training_results = agent.trainAgent(
        world,
        episodes=50,
        steps_per_episode=100,
        timestep=0.1
    )
    
    end_time = time.time()
    duration = end_time - start_time
    
    # Print training summary
    print(f"Training completed in {duration:.1f} seconds")
    print(f"Average reward per episode: {training_results['avg_reward']:.3f}")
    print(f"Best episode reward: {training_results['best_reward']:.3f}")
    print(f"Average curiosity level: {training_results['avg_curiosity']:.3f}")
    print(f"Total actions taken: {training_results['total_actions']}")
    
    # Evaluate trained agent
    print("\nEvaluating trained agent...")
    evaluation_results = agent.evaluateAgent(
        world,
        episodes=20,
        steps_per_episode=100,
        timestep=0.1
    )
    
    print(f"Evaluation reward: {evaluation_results['avg_reward']:.3f}")
    print(f"Evaluation success rate: {evaluation_results['success_rate']:.1%}")
    print()

def main():
    """Run all examples"""
    print("NLM Enhanced Python API Examples")
    print("=" * 50)
    print()
    
    examples = [
        example_basic_brain,
        example_enhanced_configuration,
        example_action_enhancements,
        example_world_operations,
        example_batch_processing,
        example_validation_and_error_handling,
        example_helper_utilities,
        example_comprehensive_agent_training,
    ]
    
    for example in examples:
        try:
            example()
        except Exception as e:
            print(f"Example failed with error: {e}")
            print(f"Error type: {type(e).__name__}")
            print()
    
    print("All examples completed!")
    print("\nKey enhancements demonstrated:")
    print("✓ Enhanced validation and error handling")
    print("✓ Convenience factory methods for actions")
    print("✓ Comprehensive configuration management")
    print("✓ Batch processing capabilities")
    print("✓ Statistics aggregation")
    print("✓ Helper utilities and mathematical functions")
    print("✓ Complete agent training and evaluation")

if __name__ == "__main__":
    main()
