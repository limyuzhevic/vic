"""
NLM Advanced Examples
=====================

This module contains comprehensive examples for using the enhanced NLM Python API.
It covers basic usage, advanced features, and practical applications.

Examples included:
1. Minimal working example
2. Complete agent simulation
3. Batch processing
4. Neural activity analysis
5. Advanced configuration
6. Performance optimization
7. Learning and development
8. World exploration
"""

import pynlm as nlm
import numpy as np
from typing import List, Dict, Any
import time

def example_1_minimal_working():
    """Example 1: Minimal working example - just neurons thinking."""
    print("=== Example 1: Minimal Working Example ===")
    
    # One line to create a virtual brain
    brain = nlm.createBrain(nlm.createDefaultConfig())
    
    # Initialize it
    brain.initialize()
    
    # Make it active
    brain.step(0)
    
    print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
    print(f"Total spikes so far: {brain.getTotalSpikeCount()}")
    print("✓ Minimal example works!")
    print()

def example_2_basic_neural_activity():
    """Example 2: Basic neural activity simulation."""
    print("=== Example 2: Basic Neural Activity Simulation ===")
    
    # Create and initialize brain
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Run simulation for 100 steps
    for step in range(100):
        brain.step(step)
        
        # Log statistics periodically
        if step % 25 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    print("✓ Basic neural activity simulation complete!")
    print()

def example_3_complete_agent_simulation():
    """Example 3: Complete agent simulation with brain and world."""
    print("=== Example 3: Complete Agent Simulation ===")
    
    def run_agent_simulation(num_steps=500):
        """Run a complete agent simulation with brain and world."""
        
        # 1. Create configuration
        config = nlm.createDefaultConfig()
        
        # 2. Create brain and initialize
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # 3. Create agent brain interface
        agent = nlm.createAgentBrain(brain)
        
        # 4. Create and configure world
        world = nlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(7.0, 7.0)
        
        # 5. Initialize agent with world
        agent.initialize(world)
        
        # 6. Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
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
            reward = 0.0
            if percept and percept.getInternal():
                reward = percept.getInternal()[0]
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Print progress
            if step % 100 == 0:
                print(f"Step {step}:")
                print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
                print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
                print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
                print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
        
        return brain, agent, world
    
    # Run the simulation
    brain, agent, world = run_agent_simulation(300)
    
    print("✓ Complete agent simulation finished!")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Final development stage: {brain.getDevelopmentalStage()}")
    print()

def example_4_batch_processing():
    """Example 4: Batch processing multiple simulations."""
    print("=== Example 4: Batch Processing ===")
    
    import time
    
    # Create multiple brain configurations
    configs = []
    for i in range(3):
        config = nlm.createDefaultConfig()
        # Vary neuron counts for different experiments
        config.set("neuron_count", 500 + i * 500)
        configs.append(config)
    
    # Create multiple brains
    brains = [nlm.createBrain(config) for config in configs]
    for brain in brains:
        brain.initialize()
    
    # Create corresponding worlds
    worlds = []
    for brain in brains:
        world = nlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        world.reset()
        worlds.append(world)
    
    # Create agent interfaces
    agents = [nlm.createAgentBrain(brain) for brain in brains]
    for agent, world in zip(agents, worlds):
        agent.initialize(world)
        agent.enableRewardModulation(True)
    
    print(f"Running batch simulation with {len(brains)} brains for 100 steps each...")
    start_time = time.time()
    
    # Run all simulations
    for step in range(100):
        for i, (brain, world, agent) in enumerate(zip(brains, worlds, agents)):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            reward = percept.getInternal()[0] if percept and percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
    
    end_time = time.time()
    
    # Collect results
    print("\nBatch Results:")
    print("-" * 50)
    for i, (brain, agent) in enumerate(zip(brains, agents)):
        print(f"Brain {i+1}:")
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Firing: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print()
    
    print(f"Batch simulation completed in {end_time - start_time:.2f} seconds")
    print("✓ Batch processing complete!")
    print()

def example_5_neural_activity_analysis():
    """Example 5: Neural activity pattern analysis."""
    print("=== Example 5: Neural Activity Analysis ===")
    
    # Create brain with specific configuration
    config = nlm.create_advanced_config(
        neuron_count=1000,
        connection_probability=0.2,
        enable_advanced_features=True
    )
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Run simulation to develop neural patterns
    for step in range(200):
        brain.step(step)
    
    # Use enhanced analysis utilities (requires pynlm_enhanced)
    try:
        import pynlm_enhanced as enhanced
        
        # Compute comprehensive statistics
        stats = enhanced.BrainAnalyzer.compute_comprehensive_stats(brain)
        print("Comprehensive Brain Statistics:")
        print("-" * 40)
        for key, value in stats.items():
            if isinstance(value, float):
                print(f"  {key}: {value:.2f}")
            else:
                print(f"  {key}: {value}")
        
        # Get neuron activity pattern
        pattern = enhanced.BrainAnalyzer.get_neuron_activity_pattern(
            brain, nlm.NeuronType.Excitatory, max_neurons=50
        )
        
        if pattern:
            print(f"\nNeuron Activity Pattern (Excitatory, first 10):")
            print(f"  {pattern[:10]}")
            print(f"  Mean activity: {np.mean(pattern):.3f}")
            print(f"  Activity std: {np.std(pattern):.3f}")
        
    except ImportError:
        print("Enhanced analysis not available (pynlm_enhanced not installed)")
        print("Using basic statistics:")
        
        print(f"\nBrain Statistics:")
        print("-" * 30)
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  Total synapses: {brain.getTotalSynapseCount()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    print("✓ Neural activity analysis complete!")
    print()

def example_6_advanced_configuration():
    """Example 6: Advanced configuration and optimization."""
    print("=== Example 6: Advanced Configuration ===")
    
    # Create advanced configuration
    config = nlm.create_advanced_config(
        neuron_count=2000,
        region_count=3,
        connection_probability=0.25,
        plasticity_rate=0.015,
        development_rate=0.002,
        environment="ComplexWorld",
        enable_advanced_features=True
    )
    
    # Configure additional advanced settings
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("prediction.enabled", True)
    config.set("cognition.planning_enabled", True)
    config.set("visualization.enabled", True)
    config.set("performance.parallel_enabled", True)
    
    # Get configuration summary
    summary = nlm.get_config_summary(config)
    print("Configuration Summary:")
    print("-" * 40)
    for key, value in list(summary.items())[:10]:  # Show first 10 items
        print(f"  {key}: {value}")
    
    if len(summary) > 10:
        print(f"  ... and {len(summary) - 10} more settings")
    
    # Create brain with advanced config
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print(f"\nAdvanced brain created with:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    print(f"  Regions: {brain.getRegionCount()}")
    print("✓ Advanced configuration complete!")
    print()

def example_7_agent_performance_monitoring():
    """Example 7: Agent performance monitoring and analysis."""
    print("=== Example 7: Agent Performance Monitoring ===")
    
    # Setup simulation
    config = nlm.create_default_config()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    agent = nlm.createAgentBrain(brain)
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Monitor performance over time
    performance_history = []
    
    for step in range(200):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        reward = percept.getInternal()[0] if percept and percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        # Record performance metrics
        perf = {
            'step': step,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'neuromodulation': agent.getNeuromodulationLevel(),
            'firing_rate': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount()
        }
        performance_history.append(perf)
        
        # Print performance summary every 50 steps
        if step % 50 == 0:
            print(f"Step {step} Performance:")
            print(f"  Curiosity: {perf['curiosity']:.3f}")
            print(f"  Novelty: {perf['novelty']:.3f}")
            print(f"  Prediction Error: {perf['prediction_error']:.3f}")
            print(f"  Neuromodulation: {perf['neuromodulation']:.3f}")
            print(f"  Firing Rate: {perf['firing_rate']}")
            print()
    
    # Analyze performance trends
    try:
        import pynlm_enhanced as enhanced
        
        # Get final performance analysis
        final_perf = enhanced.AgentAnalyzer.analyze_performance(agent)
        
        print("Final Performance Analysis:")
        print("-" * 40)
        for key, value in final_perf.items():
            if isinstance(value, float):
                print(f"  {key}: {value:.3f}")
            else:
                print(f"  {key}: {value}")
        
        # Get recommended actions
        recommendations = enhanced.AgentAnalyzer.get_recommended_actions(
            agent, world, max_suggestions=5
        )
        
        if recommendations:
            print(f"\nRecommended Actions:")
            for i, action in enumerate(recommendations, 1):
                print(f"  {i}. {action}")
    
    except ImportError:
        print("Performance analysis not available (pynlm_enhanced not installed)")
        print(f"\nBasic performance summary:")
        final_step = performance_history[-1]
        print(f"  Final curiosity: {final_step['curiosity']:.3f}")
        print(f"  Final novelty: {final_step['novelty']:.3f}")
        print(f"  Final prediction error: {final_step['prediction_error']:.3f}")
        print(f"  Total spikes: {final_step['total_spikes']}")
    
    print("✓ Agent performance monitoring complete!")
    print()

def example_8_world_exploration():
    """Example 8: World exploration and navigation."""
    print("=== Example 8: World Exploration ===")
    
    # Create world
    world = nlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=7, visionHeight=7)
    world.setMaxEnergy(200.0)
    world.setEnergyDecayRate(0.005)
    world.reset()
    
    # Set agent at center
    world.setAgentStart(15.0, 15.0)
    
    # Create brain and agent
    config = nlm.create_default_config()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable exploration features
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    print("Starting world exploration...")
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    print(f"Agent start: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    print()
    
    # Explore world
    exploration_data = {
        'visited_positions': [(world.getAgentBody().x, world.getAgentBody().y)],
        'energy_levels': [world.getAgentBody().energy],
        'curiosity_levels': [agent.getCuriosityLevel()],
        'steps_explored': 0
    }
    
    for step in range(100):
        # Get current state
        body = world.getAgentBody()
        exploration_data['visited_positions'].append((body.x, body.y))
        exploration_data['energy_levels'].append(body.energy)
        exploration_data['curiosity_levels'].append(agent.getCuriosityLevel())
        
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Get action
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward
        reward = percept.getInternal()[0] if percept and percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        exploration_data['steps_explored'] += 1
        
        # Print progress
        if step % 20 == 0:
            print(f"Step {step}: Position ({body.x:.1f}, {body.y:.1f}), "
                  f"Energy: {body.energy:.1f}, Curiosity: {agent.getCuriosityLevel():.3f}")
    
    # Analyze exploration
    positions = exploration_data['visited_positions']
    unique_positions = set(positions)
    coverage = len(unique_positions) / (world.getWidth() * world.getHeight())
    
    print(f"\nExploration Results:")
    print("-" * 30)
    print(f"  Steps explored: {exploration_data['steps_explored']}")
    print(f"  Unique positions: {len(unique_positions)}")
    print(f"  World coverage: {coverage:.1%}")
    print(f"  Final energy: {world.getAgentBody().energy:.1f}")
    print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
    
    # Get environment statistics
    try:
        import pynlm_enhanced as enhanced
        stats = enhanced.WorldAnalyzer.get_environment_statistics(world)
        print(f"\nEnvironment Statistics:")
        for key, value in stats.items():
            print(f"  {key}: {value:.2f}")
    except ImportError:
        print("\nEnvironment statistics not available")
    
    print("✓ World exploration complete!")
    print()

def example_9_learning_and_development():
    """Example 9: Learning and developmental processes."""
    print("=== Example 9: Learning and Development ===")
    
    # Create development-focused configuration
    config = nlm.create_advanced_config(
        neuron_count=1500,
        development_rate=0.005,
        plasticity_rate=0.02,
        enable_advanced_features=True
    )
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create agent for interaction
    agent = nlm.createAgentBrain(brain)
    
    # Create simple world for learning
    world = nlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    agent.initialize(world)
    
    # Enable developmental learning
    agent.enableDevelopment(True)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    
    print("Starting developmental learning simulation...")
    print(f"Initial development stage: {brain.getDevelopmentalStage()}")
    print()
    
    # Track development
    development_history = []
    
    for step in range(300):
        # Update world
        world.update(0.1)
        
        # Get input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Get output
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply learning
        reward = percept.getInternal()[0] if percept and percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record development
        development_history.append({
            'step': step,
            'stage': brain.getDevelopmentalStage(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'firing_rate': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount()
        })
        
        # Print development milestones
        if step % 50 == 0:
            print(f"Step {step}: Development Stage {brain.getDevelopmentalStage()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Novelty: {agent.getNoveltyLevel():.3f}")
    
    # Analyze development
    final_stage = brain.getDevelopmentalStage()
    stage_progress = {
        'Initial': 0, 'CriticalPeriod': 1, 'Maturation': 2, 
        'Adult': 3, 'Aging': 4
    }
    
    current_stage_value = stage_progress.get(str(final_stage), 0)
    max_stage_value = stage_progress['Adult']
    development_progress = current_stage_value / max_stage_value
    
    print(f"\nDevelopment Analysis:")
    print("-" * 30)
    print(f"  Final development stage: {final_stage}")
    print(f"  Development progress: {development_progress:.1%}")
    print(f"  Total learning steps: {len(development_history)}")
    print(f"  Final firing rate: {development_history[-1]['firing_rate']}")
    print(f"  Total spikes accumulated: {development_history[-1]['total_spikes']}")
    print(f"  Average curiosity: {np.mean([d['curiosity'] for d in development_history]):.3f}")
    print(f"  Average novelty: {np.mean([d['novelty'] for d in development_history]):.3f}")
    
    print("✓ Learning and development complete!")
    print()

def example_10_performance_optimization():
    """Example 10: Performance optimization and benchmarking."""
    print("=== Example 10: Performance Optimization ===")
    
    import time
    
    # Test different configurations for performance
    configurations = [
        {'neurons': 500, 'region_count': 2, 'plasticity': 0.01},
        {'neurons': 1000, 'region_count': 2, 'plasticity': 0.01},
        {'neurons': 2000, 'region_count': 3, 'plasticity': 0.02},
        {'neurons': 3000, 'region_count': 4, 'plasticity': 0.015}
    ]
    
    print("Performance Benchmarking:")
    print("-" * 50)
    
    for i, params in enumerate(configurations):
        print(f"\nConfiguration {i+1}: {params}")
        
        # Create configuration
        config = nlm.create_advanced_config(
            neuron_count=params['neurons'],
            region_count=params['region_count'],
            plasticity_rate=params['plasticity']
        )
        
        # Create brain
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Time initialization
        start_time = time.time()
        brain.initialize()
        init_time = time.time() - start_time
        
        # Time simulation
        start_time = time.time()
        for step in range(50):
            brain.step(step)
        sim_time = time.time() - start_time
        
        # Get performance metrics
        firing_rate = brain.getAverageFiringRate()
        spikes_per_second = brain.getTotalSpikeCount() / 50.0
        
        print(f"  Initialization time: {init_time:.3f}s")
        print(f"  Simulation time (50 steps): {sim_time:.3f}s")
        print(f"  Average firing rate: {firing_rate:.2f}")
        print(f"  Spikes per second: {spikes_per_second:.1f}")
    
    print("\n✓ Performance optimization analysis complete!")
    print()

def main():
    """Run all examples."""
    print("NLM Advanced Examples")
    print("=" * 60)
    print()
    
    examples = [
        example_1_minimal_working,
        example_2_basic_neural_activity,
        example_3_complete_agent_simulation,
        example_4_batch_processing,
        example_5_neural_activity_analysis,
        example_6_advanced_configuration,
        example_7_agent_performance_monitoring,
        example_8_world_exploration,
        example_9_learning_and_development,
        example_10_performance_optimization
    ]
    
    for example in examples:
        try:
            example()
        except Exception as e:
            print(f"Error in {example.__name__}: {e}")
            print("Continuing with next example...")
        print("=" * 60)
        print()
    
    print("All examples completed!")
    print("\nFor more detailed information, see:")
    print("- easy_usage.md: Basic usage guide")
    print("- HOW_TO_USE.md: Complete documentation")
    print("- python/pynlm_enhanced.py: Enhanced API documentation")

if __name__ == "__main__":
    main()