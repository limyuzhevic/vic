"""
Advanced NLM Examples

This file demonstrates advanced usage patterns and integration of all enhanced
features in NLM Python bindings version 0.1.0.
"""

import pynlm
import tempfile
import os

def example_1_full_integration():
    """Example 1: Full integration of all NLM systems."""
    print("Example 1: Full Integration of All NLM Systems")
    print("-" * 50)
    
    print("Setting up a complete NLM brain with all integrated systems...")
    
    # 1. Create brain with optimized configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("region_count", 3)
    config.set("connection_probability", 0.12)
    config.set("plasticity_learning_rate", 0.02)
    
    # 2. Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons and "
          f"{brain.getRegionCount()} regions")
    
    # 3. Check all integrated systems
    print("\nChecking integrated systems:")
    
    # Memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    print(f"✓ Memory systems: Working={working_memory is not None}, "
          f"Episodic={episodic_memory is not None}, "
          f"Associative={associative_memory is not None}")
    
    # Prediction system
    prediction_system = brain.getPredictionSystem()
    print(f"✓ Prediction system: {prediction_system is not None}")
    
    # Cognition systems
    planner = brain.getPlanner()
    concept_formation = brain.getConceptFormation()
    attention = brain.getAttention()
    print(f"✓ Cognition systems: Planner={planner is not None}, "
          f"Concept={concept_formation is not None}, "
          f"Attention={attention is not None}")
    
    # Neuromodulation systems
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    prediction_error = brain.getPredictionErrorSignal()
    print(f"✓ Neuromodulation systems: Dopamine={dopamine is not None}, "
          f"Curiosity={curiosity is not None}, Novelty={novelty is not None}, "
          f"PredictionError={prediction_error is not None}")
    
    # Development system
    development = brain.getDevelopmentSystem()
    print(f"✓ Development system: {development is not None}")
    
    # 4. Run simulation with system updates
    print("\nRunning simulation with system integration...")
    
    for step in range(50):
        # Update world
        world = pynlm.createSimpleWorld()
        world.configure(30, 30, 12, 12)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Single step simulation
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(1.0, 0.8)
        agent.updateDevelopment(0.1)
        
        if step % 10 == 0:
            print(f"  Step {step}: Spike count={brain.getTotalSpikeCount()}, "
                  f"Firing neurons={brain.getFiringNeuronCount()}, "
                  f"Curiosity={agent.getCuriosityLevel():.2f}, "
                  f"Development={agent.getDevelopmentalStage()}")
    
    print("\n✓ Full integration simulation completed successfully!")
    print()

def example_2_advanced_configuration():
    """Example 2: Advanced configuration management."""
    print("Example 2: Advanced Configuration Management")
    print("-" * 45)
    
    # Create a complex configuration
    def create_advanced_config():
        """Create an advanced NLM configuration."""
        config = pynlm.createDefaultConfig()
        
        # Basic settings
        config.set("neuron_count", 1000)
        config.set("region_count", 5)
        config.set("simulation_timestep", 0.0005)
        
        # Connectivity
        config.set("connection_probability", 0.15)
        config.set("delayed_spike_base", 2)
        
        # Plasticity
        config.set("plasticity_learning_rate", 0.025)
        config.set("stdp_ltp_weight", 0.025)
        config.set("stdp_ltd_weight", 0.03)
        config.set("stdp_tau", 25.0)
        
        # Memory systems
        config.set("working_memory.capacity", 200)
        config.set("episodic_memory.max_episodes", 5000)
        config.set("consolidation_interval", 2000)
        config.set("replay_interval", 50)
        
        # Neuromodulation
        config.set("dopamine_baseline", 0.1)
        config.set("curiosity_strength", 0.8)
        config.set("novelty_threshold", 0.3)
        
        # Development
        config.set("synaptogenesis_rate", 0.002)
        config.set("pruning_rate", 0.0002)
        config.set("critical_period_start", 100)
        
        # Cognition
        config.set("planning_depth", 8)
        config.set("attention_strength", 1.2)
        
        return config
    
    # Create advanced configuration
    advanced_config = create_advanced_config()
    
    # Validate configuration
    print("Validating advanced configuration...")
    try:
        if pynlm.validate_config(advanced_config):
            print("✓ Configuration validation passed")
    except pynlm.ConfigurationError as e:
        print(f"✗ Configuration validation failed: {e}")
        return
    
    # Save configuration
    print("Saving configuration...")
    try:
        pynlm.save_config_to_file(advanced_config, "advanced_config.json")
        print("✓ Configuration saved successfully")
    except pynlm.ConfigurationError as e:
        print(f"✗ Failed to save configuration: {e}")
        return
    
    # Load configuration
    print("Loading configuration...")
    try:
        loaded_config = pynlm.load_config_from_file("advanced_config.json")
        print("✓ Configuration loaded successfully")
        
        # Compare configurations
        differences = advanced_config.compare(loaded_config)
        if differences:
            print(f"⚠ Found {len(differences)} differences")
        else:
            print("✓ Configurations are identical")
            
    except pynlm.ConfigurationError as e:
        print(f"✗ Failed to load configuration: {e}")
    
    # Create brain from configuration
    print("Creating brain from configuration...")
    try:
        brain = pynlm.createBrain(advanced_config)
        brain.initialize()
        print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
        
        # Quick simulation
        for step in range(10):
            brain.step(step)
        
        print(f"✓ Simulation completed: {brain.getTotalSpikeCount()} spikes")
        
    except Exception as e:
        print(f"✗ Failed to create brain: {e}")
    
    # Clean up
    import os
    if os.path.exists("advanced_config.json"):
        os.unlink("advanced_config.json")
        print("✓ Cleaned up temporary file")
    
    print()

def example_3_performance_benchmarking():
    """Example 3: Performance benchmarking and profiling."""
    print("Example 3: Performance Benchmarking")
    print("-" * 35)
    
    import time
    
    def benchmark_brain_creation(neuron_count, steps):
        """Benchmark brain creation and simulation."""
        print(f"\nBenchmarking: {neuron_count} neurons, {steps} steps")
        
        try:
            # Create configuration
            start_time = time.time()
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", neuron_count)
            config.set("region_count", max(1, neuron_count // 250))
            config_time = time.time() - start_time
            
            # Create brain
            start_time = time.time()
            brain = pynlm.createBrain(config)
            brain_time = time.time() - start_time
            
            # Initialize
            start_time = time.time()
            brain.initialize()
            init_time = time.time() - start_time
            
            # Run simulation
            start_time = time.time()
            for step in range(steps):
                brain.step(step)
            sim_time = time.time() - start_time
            
            # Calculate metrics
            total_time = config_time + brain_time + init_time + sim_time
            spikes_per_second = brain.getTotalSpikeCount() / sim_time if sim_time > 0 else 0
            neurons_per_second = neuron_count / brain_time if brain_time > 0 else 0
            
            print(f"  Configuration creation: {config_time:.4f}s")
            print(f"  Brain creation: {brain_time:.4f}s")
            print(f"  Initialization: {init_time:.4f}s")
            print(f"  Simulation ({steps} steps): {sim_time:.4f}s")
            print(f"  Total time: {total_time:.4f}s")
            print(f"  Performance: {spikes_per_second:.0f} spikes/sec, "
                  f"{neurons_per_second:.0f} neurons/sec setup")
            print(f"  Efficiency: {brain.getTotalSpikeCount() / max(1, steps):.1f} spikes/step")
            
            return {
                'neuron_count': neuron_count,
                'steps': steps,
                'total_time': total_time,
                'config_time': config_time,
                'brain_time': brain_time,
                'init_time': init_time,
                'sim_time': sim_time,
                'spikes_per_second': spikes_per_second,
                'neurons_per_second': neurons_per_second
            }
            
        except Exception as e:
            print(f"✗ Benchmark failed: {e}")
            return None
    
    # Run benchmarks for different configurations
    benchmarks = [
        (100, 100),
        (500, 200),
        (1000, 100),
        (2000, 50)
    ]
    
    results = []
    for neurons, steps in benchmarks:
        result = benchmark_brain_creation(neurons, steps)
        if result:
            results.append(result)
    
    # Summary
    print("\n" + "=" * 50)
    print("Benchmark Summary")
    print("=" * 50)
    
    for result in results:
        print(f"{result['neuron_count']} neurons, {result['steps']} steps: "
              f"{result['total_time']:.3f}s total, "
              f"{result['spikes_per_second']:.0f} spikes/sec")
    
    # Performance analysis
    if results:
        fastest_setup = min(results, key=lambda x: x['brain_time'] + x['init_time'])
        fastest_sim = max(results, key=lambda x: x['spikes_per_second'])
        
        print(f"\nFastest setup: {fastest_setup['neuron_count']} neurons")
        print(f"Highest spike rate: {fastest_sim['spikes_per_second']:.0f} spikes/sec")
    
    print()

def example_4_experiment_framework():
    """Example 4: Experiment framework with automatic results collection."""
    print("Example 4: Experiment Framework")
    print("-" * 30)
    
    def run_experiment(experiment_name, config_template, variations):
        """Run an experiment with multiple parameter variations."""
        print(f"Running experiment: {experiment_name}")
        
        results = []
        
        for i, variation in enumerate(variations):
            print(f"  Variation {i+1}: {variation}")
            
            # Create configuration from template
            config = pynlm.createDefaultConfig()
            
            # Apply variations
            for key, value in variation.items():
                config.set(key, value)
            
            # Override with template values
            for key, value in config_template.items():
                config.set(key, value)
            
            # Run experiment
            try:
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                # Run simulation
                for step in range(variation.get('steps', 50)):
                    brain.step(step)
                    
                    if step % 10 == 0:
                        print(f"    Step {step}: {brain.getTotalSpikeCount()} spikes")
                
                # Collect results
                result = {
                    'variation_id': i,
                    'parameters': variation,
                    'final_spike_count': brain.getTotalSpikeCount(),
                    'final_firing_rate': brain.getAverageFiringRate(),
                    'total_neurons': brain.getTotalNeuronCount(),
                    'total_synapses': brain.getTotalSynapseCount(),
                    'simulation_time': variation.get('steps', 50)
                }
                
                results.append(result)
                print(f"    ✓ Completed: {result['final_spike_count']} spikes")
                
            except Exception as e:
                print(f"    ✗ Failed: {e}")
                results.append({
                    'variation_id': i,
                    'parameters': variation,
                    'error': str(e)
                })
        
        return results
    
    # Define experiment
    experiment_config = {
        'neuron_count': 500,
        'region_count': 2,
        'connection_probability': 0.1
    }
    
    experiment_variations = [
        {'steps': 100, 'plasticity_learning_rate': 0.01},
        {'steps': 200, 'plasticity_learning_rate': 0.02},
        {'steps': 150, 'plasticity_learning_rate': 0.03},
        {'steps': 300, 'plasticity_learning_rate': 0.015}
    ]
    
    # Run experiment
    experiment_results = run_experiment("Plasticity Experiment", experiment_config, experiment_variations)
    
    # Analyze results
    print("\n" + "=" * 50)
    print("Experiment Analysis")
    print("=" * 50)
    
    successful_results = [r for r in experiment_results if 'error' not in r]
    
    if successful_results:
        spike_counts = [r['final_spike_count'] for r in successful_results]
        firing_rates = [r['final_firing_rate'] for r in successful_results]
        
        print(f"Successful experiments: {len(successful_results)}/{len(experiment_results)}")
        print(f"Average spike count: {sum(spike_counts) / len(spike_counts):.0f}")
        print(f"Spike count range: {min(spike_counts)} - {max(spike_counts)}")
        print(f"Average firing rate: {sum(firing_rates) / len(firing_rates):.2f}")
        
        # Find best parameter combination
        best_result = max(successful_results, key=lambda x: x['final_spike_count'])
        print(f"\nBest performing variation:")
        print(f"  Parameters: {best_result['parameters']}")
        print(f"  Spike count: {best_result['final_spike_count']}")
        print(f"  Firing rate: {best_result['final_firing_rate']:.2f}")
    
    print()

def example_5_visualization_and_reporting():
    """Example 5: Visualization and reporting (simulated)."""
    print("Example 5: Visualization and Reporting")
    print("-" * 40)
    
    print("This example demonstrates how to prepare data for visualization")
    print("and create comprehensive reports (visualization functions would")
    print("typically use matplotlib, plotly, or other visualization libraries)")
    print()
    
    # Simulate data collection for visualization
    def collect_brain_data(brain, world, agent, steps):
        """Collect comprehensive brain data for visualization."""
        data = {
            'simulation_parameters': {
                'neuron_count': brain.getTotalNeuronCount(),
                'region_count': brain.getRegionCount(),
                'steps': steps
            },
            'time_series': [],
            'final_statistics': {}
        }
        
        for step in range(steps):
            # Run simulation step
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record time series data
            time_point = {
                'step': step,
                'spike_count': brain.getTotalSpikeCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'action_type': action.getType(),
                'curiosity_level': agent.getCuriosityLevel(),
                'reward_modulation': agent.isRewardModulationEnabled()
            }
            
            data['time_series'].append(time_point)
        
        # Record final statistics
        data['final_statistics'] = {
            'total_spikes': brain.getTotalSpikeCount(),
            'total_firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'development_stage': agent.getDevelopmentalStage(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel()
        }
        
        return data
    
    # Setup and run simulation
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 300)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Collect data
    simulation_data = collect_brain_data(brain, world, agent, 50)
    
    # Report data collection
    print("Data collection completed:")
    print(f"✓ Collected {len(simulation_data['time_series'])} time points")
    print(f"✓ Neuron count: {simulation_data['simulation_parameters']['neuron_count']}")
    print(f"✓ Simulation steps: {simulation_data['simulation_parameters']['steps']}")
    print(f"✓ Final spike count: {simulation_data['final_statistics']['total_spikes']}")
    print(f"✓ Average firing rate: {simulation_data['final_statistics']['average_firing_rate']:.2f}")
    
    # Prepare data for visualization
    print("\nVisualization-ready data:")
    print("- Time series data for spike count plotting")
    print("- Statistics data for bar charts")
    print("- Configuration parameters for experiment comparison")
    
    # Example time series analysis
    print("\nTime series analysis example:")
    spike_counts = [point['spike_count'] for point in simulation_data['time_series']]
    if spike_counts:
        print(f"  Spike count range: {min(spike_counts)} - {max(spike_counts)}")
        print(f"  Average spike count: {sum(spike_counts) / len(spike_counts):.1f}")
        print(f"  Spike count trend: {'increasing' if spike_counts[-1] > spike_counts[0] else 'decreasing'}")
    
    print()

if __name__ == "__main__":
    print("Advanced NLM Examples")
    print("=" * 60)
    print("Demonstrating advanced usage patterns and integration")
    print()
    
    example_1_full_integration()
    example_2_advanced_configuration()
    example_3_performance_benchmarking()
    example_4_experiment_framework()
    example_5_visualization_and_reporting()
    
    print("All advanced examples completed successfully!")
    print()
    print("Advanced features demonstrated:")
    print("• Complete integration of all NLM systems")
    print("• Advanced configuration management with validation")
    print("• Performance benchmarking and optimization")
    print("• Experiment framework with parameter variations")
    print("• Data collection for visualization and reporting")
    print("• Comprehensive system analysis and reporting")
