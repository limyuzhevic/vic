#!/usr/bin/env python3
"""
Advanced NLM Examples - Demonstrating enhanced Python features and usage patterns.

These examples showcase the advanced capabilities of the NLM Python bindings,
including context managers, batch processing, advanced configuration,
and sophisticated simulation scenarios.
"""

import sys
import os
import json
import tempfile
import time
from pathlib import Path

# Add the current directory to Python path so we can import pynlm
sys.path.insert(0, str(Path(__file__).parent))

import pynlm
def example_1_context_manager_pattern():
    """Example 1: Using context managers for clean resource management."""
    print("Example 1: Context Manager Pattern")
    print("=" * 40)
    
    # Using the brain_session context manager for complete isolation
    with pynlm.brain_session() as session:
        brain = session.brain
        world = session.world
        agent = session.agent
        
        # Configure the world
        world.configure(width=30, height=30, vision_width=12, vision_height=12)
        
        # Enable advanced features
        agent.enable_features({
            'reward_modulation': True,
            'structural_plasticity': True,
            'development': True,
            'curiosity': True,
        })
        
        # Run a complete episode with automatic cleanup
        print("Running complete agent episode...")
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 20 == 0:
                stats = brain.get_stats()
                print(f"  Step {step}: {stats['firing_neurons']} neurons firing, "
                      f"curiosity: {agent.get_features()['curiosity_level']:.3f}")
        
        # Get final statistics
        stats = brain.get_stats()
        agent_features = agent.get_features()
        
        print(f"\nEpisode completed!")
        print(f"  Total steps: {stats['total_steps']}")
        print(f"  Final curiosity level: {agent_features['curiosity_level']:.3f}")
        print(f"  Developmental stage: {stats['developmental_stage']}")
        
        # Context manager automatically resets everything
    
    print("Context manager has cleaned up resources.\n")
def example_2_batch_processing():
    """Example 2: Batch processing multiple simulations."""
    print("Example 2: Batch Processing")
    print("=" * 40)
    
    def run_single_simulation(config_file=None):
        """Run a single simulation with given configuration."""
        with pynlm.brain_session() as session:
            brain = session.brain
            world = session.world
            agent = session.agent
            
            # Load custom configuration if provided
            if config_file and os.path.exists(config_file):
                config = pynlm.ConfigManager()
                config.load_from_file(config_file)
                # Recreate brain with new config
                brain = pynlm.EnhancedBrain(config)
                session.brain = brain
                session.agent = pynlm.EnhancedAgentBrain(brain)
            
            # Enable features based on configuration
            agent.enable_features({
                'reward_modulation': True,
                'curiosity': True,
                'development': config.get('development.enabled', False) if 'config' in locals() else False,
            })
            
            # Run simulation
            for step in range(50):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decode_motor_command()
                world.apply_motor_command(action, world.get_simulation_time())
                
            return brain.get_stats()
    
    # Create different configurations
    configs = {
        'conservative': {'brain.neuron_count': 500, 'brain.synapse_density': 0.05},
        'balanced': {'brain.neuron_count': 1000, 'brain.synapse_density': 0.1},
        'exploratory': {'brain.neuron_count': 2000, 'brain.synapse_density': 0.15},
    }
    
    # Run batch simulations
    results = {}
    for config_name, config_params in configs.items():
        print(f"Running {config_name} simulation...")
        start_time = time.time()
        
        # Create config
        config = pynlm.ConfigManager()
        config.set('brain.neuron_count', config_params['brain.neuron_count'])
        config.set('brain.synapse_density', config_params['brain.synapse_density'])
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config.save_to_file(f.name, pretty=True)
            config_file = f.name
        
        try:
            # Run simulation
            stats = run_single_simulation(config_file)
            
            results[config_name] = {
                'stats': stats,
                'config': config_params,
                'execution_time': time.time() - start_time,
            }
            
            print(f"  Completed in {results[config_name]['execution_time']:.2f}s")
            print(f"  Neurons firing: {stats['firing_neurons']}")
            
        finally:
            os.unlink(config_file)
    
    # Compare results
    print("\nBatch Processing Results:")
    print("-" * 40)
    for config_name, result in results.items():
        print(f"{config_name:12} | "
              f"Neurons: {result['stats']['total_neurons']:4d} | "
              f"Firing: {result['stats']['firing_neurons']:3d} | "
              f"Time: {result['execution_time']:5.2f}s")
    
    print("\n")
def example_3_advanced_configuration():
    """Example 3: Advanced configuration and validation."""
    print("Example 3: Advanced Configuration")
    print("=" * 40)
    
    # Create a sophisticated configuration
    config = pynlm.ConfigManager()
    
    # Basic brain configuration
    config.set('brain.neuron_count', 2000)
    config.set('brain.synapse_density', 0.12)
    config.set('brain.v_thresh', -52.0)
    config.set('brain.v_rest', -68.0)
    config.set('brain.tau_mem', 25.0)
    
    # Advanced plasticity configuration
    config.set('plasticity.stdp.enable', True)
    config.set('plasticity.stdp.learning_rate', 0.002)
    config.set('plasticity.stdp.tau_plus', 25.0)
    config.set('plasticity.stdp.tau_minus', 25.0)
    
    config.set('plasticity.hebbian.enable', True)
    config.set('plasticity.hebbian.learning_rate', 0.001)
    
    config.set('plasticity.structural.enable', True)
    config.set('plasticity.structural.growth_rate', 0.05)
    
    # Neuromodulation configuration
    config.set('neuromod.dopamine.scale', 1.5)
    config.set('neuromod.curiosity.enable', True)
    config.set('neuromod.curiosity.exploration_rate', 0.8)
    config.set('neuromod.novelty.enable', True)
    config.set('neuromod.novelty.threshold', 0.3)
    
    # Development configuration
    config.set('development.stage_duration.initial', 100)
    config.set('development.stage_duration.critical_period', 200)
    config.set('development.stage_duration.maturation', 400)
    config.set('development.plasticity_scaling.initial', 1.0)
    config.set('development.plasticity_scaling.adult', 0.7)
    
    # Validate configuration
    print("Validating configuration...")
    config.validate()
    print("✓ Configuration is valid!")
    
    # Display configuration summary
    print("\nConfiguration Summary:")
    print("-" * 40)
    summary = config.summary()
    print(summary)
    
    # Save configuration to file
    config_file = 'advanced_config.json'
    config.save_to_file(config_file, pretty=True)
    print(f"\nConfiguration saved to: {config_file}")
    
    # Create brain with this configuration
    brain = pynlm.EnhancedBrain(config)
    print(f"Created brain with {brain.get_total_neuron_count()} neurons")
    
    # Test configuration loading
    new_config = pynlm.ConfigManager()
    new_config.load_from_file(config_file)
    
    # Verify key values
    print("\nVerifying configuration loading...")
    self_check = True
    for key in ['brain.neuron_count', 'plasticity.stdp.enable', 'neuromod.curiosity.enable']:
        original_value = config.get(key)
        loaded_value = new_config.get(key)
        if original_value == loaded_value:
            print(f"✓ {key}: {original_value}")
        else:
            print(f"✗ {key}: {original_value} != {loaded_value}")
            self_check = False
    
    if self_check:
        print("\n✓ All configuration checks passed!")
    else:
        print("\n✗ Configuration verification failed!")
    
    print("\n")
def example_4_monitoring_and_statistics():
    """Example 4: Real-time monitoring and statistics."""
    print("Example 4: Real-time Monitoring and Statistics")
    print("=" * 40)
    
    with pynlm.brain_session() as session:
        brain = session.brain
        world = session.world
        agent = session.agent
        
        # Configure world
        world.configure(width=50, height=50, vision_width=16, vision_height=16)
        
        # Enable all features for monitoring
        agent.enable_features({
            'reward_modulation': True,
            'structural_plasticity': True,
            'development': True,
            'curiosity': True,
        })
        
        print("Starting simulation with real-time monitoring...")
        print("Time   | Neurons | Firing | Curiosity | Novelty | DevStage | Reward")
        print("-" * 70)
        
        for step in range(150):
            # Update world
            world.update(0.1)
            
            # Process input and step brain
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            # Get monitoring data
            stats = brain.get_stats()
            features = agent.get_features()
            
            # Log statistics
            print(f"{step:5.1f} | "
                  f"{stats['total_neurons']:6d} | "
                  f"{stats['firing_neurons']:5d} | "
                  f"{features['curiosity_level']:7.3f} | "
                  f"{features['novelty_level']:6.3f} | "
                  f"{stats['developmental_stage']:6d} | "
                  f"{features['neuromodulation_level']:5.3f}")
            
            # Apply motor command
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Log important events
            if step % 50 == 0:
                print(f"  -> Step {step} completed, "
                      f"region count: {brain.get_region_count()}")
                
        # Final statistics
        final_stats = brain.get_stats()
        final_features = agent.get_features()
        
        print("\nFinal Statistics:")
        print("-" * 40)
        print(f"Total simulation time: {final_stats['elapsed_time']:.2f}s")
        print(f"Total steps completed: {final_stats['total_steps']}")
        print(f"Final firing rate: {final_stats['average_firing_rate']:.2f} Hz")
        print(f"Final curiosity level: {final_features['curiosity_level']:.3f}")
        print(f"Final novelty level: {final_features['novelty_level']:.3f}")
        print(f"Developmental stage: {final_stats['developmental_stage']}")
        print(f"Total synapses: {final_stats['total_synapses']}")
        print(f"Excitatory/inhibition ratio: {final_stats['excitatory_inhibition_ratio']:.3f}")
        
    print("\n")
def example_5_checkpoint_and_recovery():
    """Example 5: Checkpoint saving and recovery."""
    print("Example 5: Checkpoint and Recovery")
    print("=" * 40)
    
    # Create and run a simulation
    with pynlm.brain_session() as session:
        brain = session.brain
        world = session.world
        agent = session.agent
        
        world.configure(width=20, height=20, vision_width=8, vision_height=8)
        
        # Run some steps
        print("Running initial simulation...")
        for step in range(50):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulation_time())
            
        print(f"Initial simulation completed: {brain.get_stats()['total_steps']} steps")
        
        # Save checkpoint
        checkpoint_file = 'simulation_checkpoint.bin'
        save_result = pynlm.save_simulation_checkpoint(brain, checkpoint_file)
        print(f"Checkpoint saved to: {checkpoint_file}")
        
    # Simulate interruption and recovery
    print("\nSimulating interruption and recovery...")
    
    # Create new brain (simulating fresh start)
    with pynlm.brain_session() as session:
        brain = session.brain
        world = session.world
        agent = session.agent
        
        world.configure(width=20, height=20, vision_width=8, vision_height=8)
        
        # Load checkpoint (if it exists)
        if os.path.exists(checkpoint_file):
            print("Loading checkpoint...")
            load_result = pynlm.load_simulation_checkpoint(brain, checkpoint_file)
            print(f"Checkpoint loaded: {load_result}")
            
            # Continue from saved state
            print("Continuing simulation...")
            for step in range(50, 100):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.get_simulation_time())
                
            print(f"Recovery simulation completed: {brain.get_stats()['total_steps']} total steps")
        else:
            print("Checkpoint not found, starting fresh")
            
    # Clean up checkpoint
    if os.path.exists(checkpoint_file):
        os.remove(checkpoint_file)
        print("Checkpoint cleaned up")
    
    print("\n")
def example_6_performance_benchmarking():
    """Example 6: Performance benchmarking."""
    print("Example 6: Performance Benchmarking")
    print("=" * 40)
    
    def create_simple_brain():
        """Factory function for creating test brains."""
        config = pynlm.create_default_config()
        return pynlm.create_brain(config)
    
    # Run performance benchmarks
    print("Running performance benchmarks...")
    
    benchmarks = [
        {'steps': 100, 'runs': 5, 'description': 'Light load'},
        {'steps': 500, 'runs': 3, 'description': 'Medium load'},
        {'steps': 1000, 'runs': 2, 'description': 'Heavy load'},
    ]
    
    results = []
    for benchmark in benchmarks:
        print(f"\nBenchmark: {benchmark['description']} "
              f"({benchmark['steps']} steps, {benchmark['runs']} runs)")
        
        run_results = pynlm.benchmark_simulation(
            create_simple_brain,
            num_steps=benchmark['steps'],
            num_runs=benchmark['runs']
        )
        
        # Calculate statistics
        elapsed_times = [r['elapsed_time'] for r in run_results]
        steps_per_second = [r['steps_per_second'] for r in run_results]
        
        result = {
            'description': benchmark['description'],
            'avg_elapsed': sum(elapsed_times) / len(elapsed_times),
            'min_elapsed': min(elapsed_times),
            'max_elapsed': max(elapsed_times),
            'avg_steps_per_sec': sum(steps_per_second) / len(steps_per_second),
            'min_steps_per_sec': min(steps_per_second),
            'max_steps_per_sec': max(steps_per_second),
        }
        
        results.append(result)
        
        print(f"  Average steps/sec: {result['avg_steps_per_sec']:,.0f}")
        print(f"  Time range: {result['min_elapsed']:.2f}s - {result['max_elapsed']:.2f}s")
    
    # Compare results
    print("\nBenchmark Comparison:")
    print("-" * 80)
    print(f"{'Benchmark':<15} {'Avg Steps/sec':<15} {'Time Range (s)':<15} {'Min Steps/sec':<15} {'Max Steps/sec':<15}")
    print("-" * 80)
    
    for result in results:
        print(f"{result['description']:<15} "
              f"{result['avg_steps_per_sec']:>12,.0f} "
              f"{result['min_elapsed']:>5.2f}-{result['max_elapsed']:>5.2f} "
              f"{result['min_steps_per_sec']:>12,.0f} "
              f"{result['max_steps_per_sec']:>12,.0f}")
    
    print("\n")
def example_7_error_handling_and_recovery():
    """Example 7: Error handling and recovery."""
    print("Example 7: Error Handling and Recovery")
    print("=" * 40)
    
    print("Testing error handling...")
    
    # Test 1: Configuration validation error
    print("\n1. Configuration validation test:")
    try:
        config = pynlm.ConfigManager()
        # Intentionally create an invalid config
        config.set('brain.neuron_count', -100)  # Invalid: negative neurons
        config.validate()
        print("  ✗ Configuration should have failed validation")
    except pynlm.ValidationError as e:
        print(f"  ✓ Validation error caught: {e}")
    except Exception as e:
        print(f"  ? Unexpected error: {e}")
    
    # Test 2: File not found error
    print("\n2. File not found error test:")
    try:
        config = pynlm.ConfigManager()
        config.load_from_file('nonexistent_file.json')
        print("  ✗ Should have raised FileNotFoundError")
    except FileNotFoundError as e:
        print(f"  ✓ FileNotFoundError caught: {e}")
    except Exception as e:
        print(f"  ? Unexpected error: {e}")
    
    # Test 3: Brain initialization error
    print("\n3. Brain initialization error test:")
    try:
        # This depends on implementation - could test uninitialized brain
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        brain.step(0)  # Should work after initialize
        print("  ✓ Brain step completed successfully")
    except Exception as e:
        print(f"  ? Unexpected error: {e}")
    
    print("\nError handling tests completed.\n")
def example_8_advanced_agent_training():
    """Example 8: Advanced agent training scenarios."""
    print("Example 8: Advanced Agent Training")
    print("=" * 40)
    
    def run_agent_training_episode(agent, world, brain, episode_num, enable_curiosity=True):
        """Run a single training episode with advanced features."""
        print(f"  Episode {episode_num}: ", end="")
        
        # Configure agent for this episode
        if enable_curiosity:
            agent.enable_curiosity(True)
            
        # Store initial state
        initial_stats = brain.get_stats()
        initial_features = agent.get_features()
        
        # Run episode
        rewards = []
        curiosity_levels = []
        
        for step in range(30):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record metrics
            curiosity_levels.append(agent.get_features()['curiosity_level'])
            
        # Calculate episode statistics
        final_stats = brain.get_stats()
        final_features = agent.get_features()
        
        curiosity_mean = sum(curiosity_levels) / len(curiosity_levels)
        
        print(f"Curiosity: {curiosity_mean:.3f}, "
              f"Firing: {final_stats['firing_neurons']}, "
              f"Novelty: {final_features['novelty_level']:.3f}")
        
        return {
            'episode': episode_num,
            'curiosity_mean': curiosity_mean,
            'final_firing': final_stats['firing_neurons'],
            'final_novelty': final_features['novelty_level'],
            'developmental_stage': final_stats['developmental_stage'],
        }
    
    # Create training environment
    print("Creating training environment...")
    with pynlm.brain_session() as session:
        brain = session.brain
        world = session.world
        agent = session.agent
        
        world.configure(width=25, height=25, vision_width=10, vision_height=10)
        
        # Enable learning capabilities
        agent.enable_features({
            'reward_modulation': True,
            'curiosity': True,
            'development': True,
        })
        
        # Run training episodes
        episodes = []
        for episode in range(5):
            print(f"\nTraining episode {episode + 1}/5")
            
            # Vary curiosity level based on episode
            enable_curiosity = (episode % 2 == 0)
            
            episode_result = run_agent_training_episode(
                agent, world, brain, episode + 1, enable_curiosity
            )
            
            episodes.append(episode_result)
            
        # Analyze training progress
        print("\nTraining Analysis:")
        print("-" * 50)
        
        curiosity_scores = [e['curiosity_mean'] for e in episodes]
        firing_rates = [e['final_firing'] for e in episodes]
        novelty_scores = [e['final_novelty'] for e in episodes]
        
        print(f"Average curiosity level: {sum(curiosity_scores)/len(curiosity_scores):.3f}")
        print(f"Curiosity trend: {[(e['curiosity_mean'] - (sum(curiosity_scores)/len(curiosity_scores))) for e in episodes]}")
        print(f"Firing rate progression: {firing_rates}")
        print(f"Novelty scores: {[f'{x:.3f}' for x in novelty_scores]}")
        print(f"Developmental stages: {[e['developmental_stage'] for e in episodes]}")
        
        # Check for learning patterns
        curiosity_increasing = all(
            episodes[i]['curiosity_mean'] < episodes[i+1]['curiosity_mean']
            for i in range(len(episodes)-1)
        )
        
        if curiosity_increasing:
            print("✓ Curiosity levels increased - evidence of learning!")
        else:
            print("? Curiosity levels did not consistently increase")
        
        # Save training results
        training_data = {
            'episodes': episodes,
            'summary': {
                'total_episodes': len(episodes),
                'avg_curiosity': sum(curiosity_scores)/len(curiosity_scores),
                'avg_firing': sum(firing_rates)/len(firing_rates),
                'avg_novelty': sum(novelty_scores)/len(novelty_scores),
            }
        }
        
        # Save to file
        with open('training_results.json', 'w') as f:
            json.dump(training_data, f, indent=2)
            
        print(f"\nTraining results saved to: training_results.json")
    
    print("\n")
def main():
    """Run all advanced examples."""
    print("NLM Advanced Examples Suite")
    print("=" * 60)
    print("Demonstrating advanced Python features and usage patterns")
    print()
    
    examples = [
        ("Context Manager Pattern", example_1_context_manager_pattern),
        ("Batch Processing", example_2_batch_processing),
        ("Advanced Configuration", example_3_advanced_configuration),
        ("Real-time Monitoring", example_4_monitoring_and_statistics),
        ("Checkpoint and Recovery", example_5_checkpoint_and_recovery),
        ("Performance Benchmarking", example_6_performance_benchmarking),
        ("Error Handling and Recovery", example_7_error_handling_and_recovery),
        ("Advanced Agent Training", example_8_advanced_agent_training),
    ]
    
    for name, example_func in examples:
        try:
            print(f"\n{'='*60}")
            print(f"Running: {name}")
            print(f"{'='*60}")
            example_func()
            
        except Exception as e:
            print(f"\n✗ Error in example '{name}': {e}")
            import traceback
            traceback.print_exc()
            
        print(f"\n{'='*60}")
        input("Press Enter to continue to next example...")
    
    print("\nAll examples completed!")
    print("\nEnhanced NLM Python bindings demonstration complete.")
if __name__ == "__main__":
    main()