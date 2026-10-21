#!/usr/bin/env python3
"""
NLM Python Bindings - Enhanced Version Examples

This file demonstrates the enhanced Python bindings for the NLM (Neural Learning Machine)
framework. The new version includes ~50% more functionality with:

- Enhanced configuration builders (default, expert, quick)
- Simulation management with SimulationManager
- Action builders (move toward, explore, interact)
- Memory recording and replay
- Statistical analysis utilities
- Convenience simulation patterns
- Advanced validation and error handling
- World object builders (resources, walls, hazards)
- Agent setup helpers
- Development and learning statistics
- Debug and development tools
- Enhanced documentation and type safety

Author: NLM Team
Version: Enhanced Edition
"""

import pynlm
import sys

print("=== NLM Python Bindings - Enhanced Version ===")
print(f"Version: {pynlm.__version__}")
print()

def example_1_basic_enhanced():
    """Example 1: Basic enhanced brain creation with convenience builders"""
    print("Example 1: Basic Enhanced Brain Creation")
    print("-" * 50)
    
    # Create brain with enhanced convenience builder
    brain = pynlm.createBrainSimple()
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Initialize and run basic simulation
    brain.initialize()
    print("Brain initialized")
    
    for i in range(50):
        brain.step(i)
        
    print(f"Simulation complete. Firing neurons: {brain.getFiringNeuronCount()}")
    print()

def example_2_experiment_setup():
    """Example 2: Complete experimental setup with research tools"""
    print("Example 2: Complete Experimental Setup")
    print("-" * 50)
    
    # Create experimental setup using enhanced helper
    config, brain, world, agent = pynlm.create_experimental_setup()
    
    # Enable all subsystems for comprehensive research
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Created experimental setup:")
    print(f"  - {brain.getTotalNeuronCount()} neurons")
    print(f"  - World size: {world.getWidth()}x{world.getHeight()}")
    print(f"  - Agent enabled: reward_mod={agent.isRewardModulationEnabled()}, curiosity={agent.isCuriosityEnabled()}")
    
    # Add world objects
    world.add_resource(5.0, 5.0, value=10.0)
    world.add_wall(10.0, 10.0, width=2.0)
    world.add_hazard(15.0, 15.0, damage=0.5)
    
    # Run experiment
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    # Show results
    stats = brain.get_statistics()
    print(f"\nExperiment Results:")
    print(stats)
    print()

def example_3_simulation_manager():
    """Example 3: Advanced simulation management"""
    print("Example 3: Advanced Simulation Management")
    print("-" * 50)
    
    # Create components
    config = pynlm.create_expert_config()
    brain = pynlm.createBrain(config)
    world = pynlm.create_default_world()
    agent = pynlm.createAgentBrain(brain)
    
    # Create simulation manager
    sim_manager = pynlm.createSimulationManager(brain, world, agent)
    
    # Initialize and validate
    sim_manager.initialize()
    errors = pynlm.validate_simulation_setup(brain, world, agent)
    
    if errors:
        print(f"Validation errors: {errors}")
    else:
        print("Setup validated successfully")
    
    # Run episode using simulation manager
    sim_manager.run_episode(300, 0.1)
    
    # Get comprehensive statistics
    print("\n=== Simulation Manager Results ===")
    print(sim_manager.get_statistics())
    print()

def example_4_action_builders():
    """Example 4: Advanced action building and utilities"""
    print("Example 4: Advanced Action Building")
    print("-" * 50)
    
    # Demonstrate action builders
    move_action = pynlm.createActionMoveToward(10.0, 10.0, 5.0, 5.0)
    print(f"Movement action type: {move_action.getType()}")
    print(f"Parameters: {move_action.getParameters()}")
    
    explore_action = pynlm.createActionExplore()
    print(f"Exploration action type: {explore_action.getType()}")
    
    interact_action = pynlm.createActionInteract()
    print(f"Interaction action type: {interact_action.getType()}")
    
    # Create world and test actions
    world = pynlm.create_default_world()
    world.setAgentStart(2.0, 2.0)
    
    # Use action to move toward resource
    action = pynlm.createActionMoveToward(8.0, 8.0)
    world.applyMotorCommand(action, 0.0)
    
    # Create and add world objects
    for i in range(5):
        world.add_resource(i * 3, i * 2, value=5.0)
    
    print(f"World setup complete. Resources added: 5")
    print()

def example_5_memory_recording():
    """Example 5: Memory recording and analysis"""
    print("Example 5: Memory Recording and Analysis")
    print("-" * 50)
    
    # Create memory recorder
    recorder = pynlm.createMemoryRecorder()
    
    # Setup experimental environment
    config, brain, world, agent = pynlm.create_debug_setup()
    agent.initialize(world)
    agent.enableCuriosity(True)
    
    # Run and record experience
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record experience
        recorder.record(step, world.getSensoryPercept())
    
    print(f"Recorded {recorder.size()} experiences")
    
    # Analyze recorded data
    print("\n=== Memory Analysis ===")
    if recorder.size() > 0:
        sample_percept = recorder.recording[0][1]
        print(f"Sample perception - Vision: {len(sample_percept.getVision())} features")
        print(f"Sample perception - Internal: {len(sample_percept.getInternal())} signals")
        print(f"Recording time range: {recorder.recording[0][0]} to {recorder.recording[-1][0]}")
    
    print()

def example_6_statistical_analysis():
    """Example 6: Statistical analysis and reporting"""
    print("Example 6: Statistical Analysis and Reporting")
    print("-" * 50)
    
    # Run a comprehensive simulation
    config, brain, world, agent = pynlm.create_experimental_setup()
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Add some variety to the world
    for i in range(10):
        world.add_resource(i * 2, i * 3, value=1.0 + (i % 5))
    
    # Run simulation
    for step in range(150):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
    
    # Get neural statistics
    neural_stats = pynlm.compute_neural_statistics(brain)
    print("Neural Statistics:")
    for key, value in neural_stats.items():
        print(f"  {key}: {value}")
    
    # Get development statistics
    development_stats = pynlm.compute_development_statistics(brain, agent)
    print("\nDevelopment Statistics:")
    for key, value in development_stats.items():
        print(f"  {key}: {value}")
    
    # Validate everything
    brain_errors = pynlm.validate_brain_state(brain)
    world_errors = pynlm.validate_world_state(world)
    
    if brain_errors:
        print(f"\nBrain validation errors: {brain_errors}")
    if world_errors:
        print(f"World validation errors: {world_errors}")
    
    print()

def example_7_configuration_variants():
    """Example 7: Configuration variants for different use cases"""
    print("Example 7: Configuration Variants")
    print("-" * 50)
    
    # Create different configuration types
    default_config = pynlm.create_config()
    expert_config = pynlm.create_expert_config()
    quick_config = pynlm.create_quick_config()
    
    print("Configuration Comparisons:")
    print(f"Default config - Neurons: {default_config.getOr('brain.neuron_count', 0)}, "
          f"Connections: {default_config.getOr('brain.connection_probability', 0)}")
    print(f"Expert config - Neurons: {expert_config.getOr('brain.neuron_count', 0)}, "
          f"Connections: {expert_config.getOr('brain.connection_probability', 0)}")
    print(f"Quick config - Neurons: {quick_config.getOr('brain.neuron_count', 0)}, "
          f"Connections: {quick_config.getOr('brain.connection_probability', 0)}")
    
    # Show validation
    default_keys = default_config.getKeys()
    expert_keys = expert_config.getKeys()
    quick_keys = quick_config.getKeys()
    
    print(f"\nConfig keys: default={len(default_keys)}, expert={len(expert_keys)}, quick={len(quick_keys)}")
    print()

def example_8_debugging_tools():
    """Example 8: Debugging and development tools"""
    print("Example 8: Debugging and Development Tools")
    print("-" * 50)
    
    # Get current time for debugging
    current_time = pynlm.get_current_time()
    print(f"Current simulation time: {current_time} ms")
    
    # Toggle debug mode
    debug_result = pynlm.toggle_debug_mode()
    print(f"Debug mode: {debug_result}")
    
    # Get formatted statistics
    config, brain, world, agent = pynlm.create_debug_setup()
    agent.initialize(world)
    
    # Run a few steps
    for step in range(10):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
    
    # Get and format statistics
    stats = brain.get_statistics()
    formatted_stats = pynlm.format_statistics(stats)
    print("\nFormatted Statistics (first 500 chars):")
    print(formatted_stats[:500] + "...")
    
    print()

def example_9_complete_workflow():
    """Example 9: Complete workflow with all features"""
    print("Example 9: Complete Workflow with All Features")
    print("-" * 50)
    
    # Welcome message
    welcome = pynlm.get_welcome_message()
    print(welcome)
    
    # Create experimental setup
    config, brain, world, agent = pynlm.create_experimental_setup()
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Create simulation manager
    sim_manager = pynlm.createSimulationManager(brain, world, agent)
    
    # Validate setup
    errors = pynlm.validate_simulation_setup(brain, world, agent)
    if not errors:
        print("Setup validated successfully")
    else:
        print(f"Setup validation errors: {errors}")
    
    # Run complete simulation
    sim_manager.run_episode(200, 0.1)
    
    # Get comprehensive results
    results = sim_manager.get_statistics()
    print("\n=== Complete Simulation Results ===")
    print(results)
    
    # Show configuration info
    version_info = pynlm.get_version_info()
    print(f"\nVersion Info:\n{version_info}")
    
    print()

def example_10_batch_processing():
    """Example 10: Batch processing and automation"""
    print("Example 10: Batch Processing and Automation")
    print("-" * 50)
    
    # Define different simulation scenarios
    scenarios = [
        {"name": "Quick Test", "config": "quick", "steps": 50, "duration": 0.1},
        {"name": "Standard Run", "config": "default", "steps": 100, "duration": 0.1},
        {"name": "Expert Simulation", "config": "expert", "steps": 200, "duration": 0.05},
    ]
    
    all_results = []
    
    for scenario in scenarios:
        print(f"Running {scenario['name']}...")
        
        if scenario['config'] == 'quick':
            config, brain, world, agent = pynlm.create_debug_setup()
        elif scenario['config'] == 'expert':
            config, brain, world, agent = pynlm.create_experimental_setup()
        else:
            config, brain, world, agent = pynlm.create_experimental_setup()
        
        agent.initialize(world)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(scenario['steps']):
            world.update(scenario['duration'])
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect results
        stats = pynlm.compute_neural_statistics(brain)
        stats['scenario'] = scenario['name']
        stats['steps'] = scenario['steps']
        all_results.append(stats)
        
        print(f"  Complete. Neurons: {brain.getTotalNeuronCount()}, "
              f"Spikes: {brain.getTotalSpikeCount()}")
    
    # Summary of results
    print("\n=== Batch Processing Summary ===")
    for result in all_results:
        print(f"{result['scenario']} ({result['steps']} steps): "
              f"{result['total_neurons']} neurons, {result['firing_rate']:.2f} Hz firing rate")
    
    print()

def main():
    """Run all examples"""
    try:
        example_1_basic_enhanced()
        example_2_experiment_setup()
        example_3_simulation_manager()
        example_4_action_builders()
        example_5_memory_recording()
        example_6_statistical_analysis()
        example_7_configuration_variants()
        example_8_debugging_tools()
        example_9_complete_workflow()
        example_10_batch_processing()
        
        # Get features summary
        features = pynlm.get_features_summary()
        print("\n" + "=" * 60)
        print("NLM Python Bindings - Enhanced Version Complete!")
        print("=" * 60)
        print(features)
        
        print("\nAll examples completed successfully!")
        print("The enhanced Python bindings provide significantly more functionality")
        print("compared to the original implementation (~50% more features).")
        
    except Exception as e:
        print(f"Error during execution: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
