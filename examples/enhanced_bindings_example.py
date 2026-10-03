#!/usr/bin/env python3
"""
Enhanced Python Bindings Example for NLM (Neural Learning Machine)

This script demonstrates the enhanced Python bindings features for NLM.
It shows advanced usage patterns, multi-agent simulations, and performance monitoring.

Run this script to see the enhanced Python bindings in action:

    python enhanced_bindings_example.py
"""

import nlm
import numpy as np
import time
import matplotlib.pyplot as plt
from pathlib import Path

def basic_usage_example():
    """Demonstrate basic usage of enhanced Python bindings"""
    print("=== Basic Usage Example ===")
    
    # Create configuration with enhanced settings
    config = nlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("development.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create and initialize brain
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    print(f"Starting simulation with {brain.getTotalNeuronCount()} neurons...")
    for i in range(500):
        brain.step(i)
        
        # Print progress
        if i % 100 == 0:
            firing_rate = brain.getAverageFiringRate()
            spike_count = brain.getTotalSpikeCount()
            print(f"Step {i}: Firing rate {firing_rate:.3f} Hz, Total spikes {spike_count}")
    
    print(f"Simulation complete! Final metrics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}\n")
    return brain

def advanced_agent_example():
    """Demonstrate advanced agent simulation"""
    print("=== Advanced Agent Example ===")
    
    # Create simulation environment
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    world = nlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=10, visionHeight=10)
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # Run advanced simulation
    print("Starting advanced simulation with enhanced features...")
    
    simulation_data = {
        'steps': [],
        'rewards': [],
        'curiosity': [],
        'novelty': [],
        'firing_rates': [],
    }
    
    for step in range(1000):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain simulation
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        reward = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        agent.applyRewardModulation(reward.reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Collect data
        simulation_data['steps'].append(step)
        simulation_data['rewards'].append(reward.reward)
        simulation_data['curiosity'].append(agent.getCuriosityLevel())
        simulation_data['novelty'].append(agent.getNoveltyLevel())
        simulation_data['firing_rates'].append(brain.getAverageFiringRate())
        
        # Print progress
        if step % 200 == 0:
            print(f"  Step {step}: Reward {reward.reward:.3f}, "
                  f"Curiosity {agent.getCuriosityLevel():.3f}, "
                  f"Novelty {agent.getNoveltyLevel():.3f}")
    
    print("Advanced simulation completed!")
    return brain, simulation_data

def multi_agent_example():
    """Demonstrate multi-agent simulation capabilities"""
    print("=== Multi-Agent Simulation Example ===")
    
    # Create multi-agent simulator
    simulator = {
        'agents': [],
        'world': None,
    }
    
    # Create world
    simulator['world'] = nlm.createSimpleWorld()
    simulator['world'].configure(100, 100, 20, 20)
    
    # Create multiple agents with different characteristics
    agent_types = ['conservative', 'exploratory', 'social']
    
    for i, agent_type in enumerate(agent_types):
        brain = nlm.createBrain(nlm.createDefaultConfig())
        brain.initialize()
        
        # Configure based on agent type
        if agent_type == 'conservative':
            brain.set("brain.v_thresh", -60.0)
            brain.set("neuromod.curiosity.enable", False)
        elif agent_type == 'exploratory':
            brain.set("brain.v_thresh", -45.0)
            brain.set("neuromod.curiosity.scale", 3.0)
        else:  # social
            brain.set("brain.v_thresh", -55.0)
            brain.set("neuromod.social.enable", True)
        
        agent = nlm.createAgentBrain(brain)
        agent.initialize(simulator['world'])
        
        simulator['agents'].append({
            'id': i,
            'type': agent_type,
            'brain': brain,
            'agent': agent,
        })
    
    print(f"Created {len(simulator['agents'])} agents with different characteristics")
    
    # Run multi-agent simulation
    print("Running multi-agent simulation...")
    for step in range(300):
        simulator['world'].update(0.1)
        
        for agent_data in simulator['agents']:
            agent = agent_data['agent']
            percept = simulator['world'].getSensoryPercept()
            agent.processSensoryInput(percept)
            agent_data['brain'].step(step)
            agent.decodeMotorCommand()
            agent.applyRewardModulation(0.0, 0.0)
    
    print("Multi-agent simulation completed!")
    return simulator

def performance_monitoring_example():
    """Demonstrate performance monitoring capabilities"""
    print("=== Performance Monitoring Example ===")
    
    # Create performance monitor
    monitor = nlm.performance_monitor.PerformanceMonitor()
    monitor.start_monitoring()
    
    # Create simulation
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    # Run performance test
    start_time = time.time()
    steps_per_second = []
    
    for step in range(100):
        step_start = time.time()
        brain.step(step)
        step_time = time.time() - step_start
        steps_per_second.append(1.0 / step_time if step_time > 0 else 0)
        
        # Record metrics
        monitor.record_metrics(brain, None, None)
    
    end_time = time.time()
    duration = end_time - start_time
    
    # Generate performance report
    report = monitor.get_performance_report()
    print("Performance Report:")
    print(report)
    
    print(f"Simulation completed in {duration:.2f} seconds")
    print(f"Average steps per second: {np.mean(steps_per_second):.2f}")
    
    return brain, monitor

def configuration_management_example():
    """Demonstrate advanced configuration management"""
    print("=== Configuration Management Example ===")
    
    # Create config manager
    config_manager = nlm.config_manager.ConfigManager()
    
    # Create preset configurations
    basic_config = config_manager.create_preset_config('basic')
    advanced_config = config_manager.create_preset_config('advanced')
    exploratory_config = config_manager.create_preset_config('exploratory')
    conservative_config = config_manager.create_preset_config('conservative')
    
    # Save configurations
    config_manager.save_config(basic_config, 'basic_simulation', "Basic simulation setup")
    config_manager.save_config(advanced_config, 'advanced_simulation', "Advanced simulation with full features")
    
    # Load configurations
    loaded_config = config_manager.load_config('basic_simulation')
    print(f"Loaded basic config with {loaded_config.get('brain.neuron_count', 'unknown')} neurons")
    
    # List all saved configurations
    print("\nSaved configurations:")
    for config in config_manager.list_configs():
        print(f"  - {config['name']}: {config['description']} (saved: {config['timestamp']})")
    
    return basic_config

def advanced_analysis_example():
    """Demonstrate advanced analysis capabilities"""
    print("=== Advanced Analysis Example ===")
    
    # Create analyzer
    analyzer = nlm.analysis.AdvancedNeuralAnalyzer(sampling_rate=100)
    visualizer = nlm.visualization.AdvancedVisualization(style='scientific')
    
    # Generate example spike data
    duration = 60  # seconds
    sampling_rate = 100  # Hz
    num_samples = duration * sampling_rate
    
    # Create synthetic spike train
    np.random.seed(42)
    spike_times = np.sort(np.random.uniform(0, duration, 500))
    
    # Create spike train for visualization
    spike_train = np.zeros(num_samples)
    for spike_time in spike_times:
        idx = int(spike_time * sampling_rate)
        if idx < num_samples:
            spike_train[idx] = 1
    
    # Analyze spike train
    analysis = analyzer.analyze_spike_train(spike_times, duration)
    
    print("Spike Train Analysis Results:")
    print(f"  Firing rate: {analysis['firing_rate']:.2f} Hz")
    print(f"  Mean ISI: {analysis['isi_mean']:.3f} s")
    print(f"  ISI coefficient of variation: {analysis['isi_cv']:.3f}")
    print(f"  Number of bursts: {analysis['bursts']['num_bursts']}")
    print(f"  Dominant frequency: {analysis['periodicity']['dominant_frequency']:.2f} Hz")
    
    # Create visualizations (commented out for environment)
    # print("\nCreating visualizations...")
    # visualizer.plot_spike_raster(spike_times, duration, neurons=50)
    # visualizer.plot_neural_dynamics(time_series_data, dynamics_analysis)
    
    return analysis

def main():
    """Main function demonstrating all enhanced features"""
    print("Enhanced Python Bindings for NLM - Demonstration")
    print("=" * 60)
    
    try:
        # Check if enhanced modules are available
        from nlm import enhanced_bindings
        print("✅ Enhanced Python bindings module available")
    except ImportError:
        print("⚠️  Enhanced bindings module not available")
        return
    
    # Run demonstrations
    print("\n1. Basic Usage Example")
    basic_brain = basic_usage_example()
    
    print("\n2. Advanced Agent Example")
    advanced_brain, advanced_data = advanced_agent_example()
    
    print("\n3. Multi-Agent Example")
    multi_agent_sim = multi_agent_example()
    
    print("\n4. Performance Monitoring Example")
    perf_brain, perf_monitor = performance_monitoring_example()
    
    print("\n5. Configuration Management Example")
    config = configuration_management_example()
    
    print("\n6. Advanced Analysis Example")
    analysis = advanced_analysis_example()
    
    print("\n" + "=" * 60)
    print("Enhanced Python Bindings Demonstration Complete!")
    print("All examples successfully demonstrated enhanced NLM capabilities.")
    print("=" * 60)

if __name__ == "__main__":
    main()