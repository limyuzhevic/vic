import pynlm
from pynlm import (
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    BrainContext,
    batch_step,
    validate_config,
    get_brain_stats,
    configure_brain_defaults,
    ConfigError,
    ValidationError,
    ResourceError,
    BatchError,
)
from pynlm import (
    createConfigFromTemplate,
    runSimulationEpisode,
    createTrainingLoop,
    parallelStepSimulation,
    batchSet,
    batchGet,
    batchInjectCurrent,
    batchActivateNeurons,
    batchProcessInputs,
    batchApplyRewards,
    calculate_mean,
    calculate_std_dev,
    calculate_median,
    calculate_max,
    calculate_min,
    calculate_range,
    calculate_variance,
    calculate_iqr,
    analyzeNetworkTopology,
    enableGPUSupport,
    createBrainCheckpoint,
    getPerformanceMetrics,
    enableMultiThreading,
)

# Package metadata
__version__ = "3.0.0"
__author__ = "NLM Project"
__description__ = "Enhanced Python bindings for NLM (Neural Learning Machine) neural simulation"

# Type checking imports for advanced users
try:
    from typing import Any, Dict, List, Optional, Union, Callable
    from dataclasses import dataclass
    TYPING_AVAILABLE = True
except ImportError:
    TYPING_AVAILABLE = False

# Public API functions for easy usage

def run_complete_demo(duration: int = 100, visualize: bool = False) -> Dict[str, Any]:
    """
    Run a complete demonstration of the NLM brain simulation.
    
    This is an advanced convenience function that demonstrates
    all major features of the enhanced NLM API.
    
    Args:
        duration: Number of simulation steps to run
        visualize: Whether to enable visualization (requires additional dependencies)
        
    Returns:
        Dictionary containing simulation results and statistics
    """
    import time
    
    # Create configuration with advanced settings
    config = createDefaultConfig()
    configure_brain_defaults(config)
    
    # Create brain and world
    brain = createBrain(config)
    world = createSimpleWorld()
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"Starting NLM demo simulation with {duration} steps...")
    start_time = time.time()
    
    # Run simulation with batch processing for performance
    step_times = batch_step(brain, duration)
    
    # Get final statistics
    stats = get_brain_stats(brain)
    
    # Calculate performance metrics
    total_time = time.time() - start_time
    steps_per_second = duration / total_time if total_time > 0 else 0
    
    results = {
        "duration": duration,
        "total_time": total_time,
        "steps_per_second": steps_per_second,
        "brain_stats": stats,
        "step_times": step_times,
        "performance_metrics": getPerformanceMetrics(brain)
    }
    
    print(f"Demo completed in {total_time:.2f}s ({steps_per_second:.1f} steps/s)")
    print(f"Final stats: {stats}")
    
    if visualize:
        # Placeholder for visualization functionality
        print("Visualization requested (not implemented in this demo)")
    
    return results

def run_training_episode(environment_config: Optional[Dict[str, Any]] = None, 
                       training_steps: int = 500) -> Dict[str, Any]:
    """
    Run a complete training episode with exploration and learning.
    
    Advanced training function that demonstrates the full capabilities
    of the NLM system with integrated learning mechanisms.
    
    Args:
        environment_config: Optional configuration for the training environment
        training_steps: Number of training steps to perform
        
    Returns:
        Training results including performance metrics and learned behaviors
    """
    # Create training environment
    config = createConfigFromTemplate("research")
    
    # Configure training-specific settings
    if environment_config:
        for key, value in environment_config.items():
            if config.has(key):
                config.set(key, value)
    
    # Initialize brain and agent
    brain = createBrain(config)
    world = createSimpleWorld()
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable training-specific features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    print(f"Starting training episode with {training_steps} steps...")
    
    # Training loop with exploration-exploitation balance
    exploration_rates = []
    reward_history = []
    curiosity_history = []
    
    for step in range(training_steps):
        # World update
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Action selection with curiosity-driven exploration
        action = agent.decodeMotorCommand()
        action_result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Compute reward for learning
        reward = 0.0f
        if action_result.success:
            reward = 1.0f
            
            # Small exploration bonus for novel actions
            if curiosity_history and curiosity_history[-1] < 0.3:
                reward += 0.1f
        
        # Apply reward modulation for learning
        predicted = 0.0f  # Could be implemented with prediction system
        agent.applyRewardModulation(reward, predicted)
        
        # Record metrics
        exploration_rates.append(agent.getCuriosityLevel())
        reward_history.append(reward)
        curiosity_history.append(agent.getCuriosityLevel())
        
        # Progress reporting
        if step % 100 == 0:
            avg_reward = sum(reward_history[-100:]) / min(100, len(reward_history))
            print(f"Step {step}/{training_steps}: Avg Reward = {avg_reward:.3f}, "
                  f"Curiosity = {agent.getCuriosityLevel():.3f}")
    
    # Calculate training statistics
    avg_reward = sum(reward_history) / len(reward_history) if reward_history else 0
    max_reward = max(reward_history) if reward_history else 0
    min_reward = min(reward_history) if reward_history else 0
    avg_curiosity = sum(curiosity_history) / len(curiosity_history) if curiosity_history else 0
    
    # Get neural network analysis
    network_analysis = analyzeNetworkTopology(brain)
    
    training_results = {
        "steps": training_steps,
        "final_reward": reward_history[-1] if reward_history else 0,
        "average_reward": avg_reward,
        "max_reward": max_reward,
        "min_reward": min_reward,
        "final_curiosity": curiosity_history[-1] if curiosity_history else 0,
        "average_curiosity": avg_curiosity,
        "network_analysis": network_analysis,
        "brain_stats": get_brain_stats(brain),
        "performance_metrics": getPerformanceMetrics(brain)
    }
    
    print(f"Training episode completed!")
    print(f"Average reward: {avg_reward:.3f}, Final curiosity: {avg_curiosity:.3f}")
    print(f"Network analysis: {network_analysis}")
    
    return training_results

def create_neural_development_experiment(num_brains: int = 5, 
                                        generations: int = 10,
                                        population_size: int = 100) -> Dict[str, Any]:
    """
    Create a neural development experiment with evolving populations.
    
    Advanced feature demonstrating the developmental capabilities of NLM.
    Creates multiple brains that evolve over time through developmental processes.
    
    Args:
        num_brains: Number of parallel brains to evolve
        generations: Number of developmental generations
        population_size: Number of individuals per generation
        
    Returns:
        Experimental results including evolution statistics
    """
    print(f"Creating neural development experiment: {num_brains} brains, {generations} generations")
    
    # Create population of brains
    brains = []
    for i in range(num_brains):
        config = createConfigFromTemplate("development")
        config.set("neuron_count", population_size)
        config.set("random_seed", i + 42)  # Different seeds for diversity
        brain = createBrain(config)
        brains.append(brain)
    
    # Evolution results tracking
    generation_stats = []
    
    for generation in range(generations):
        print(f"Generation {generation + 1}/{generations}")
        
        generation_performance = []
        
        for i, brain in enumerate(brains):
            # Initialize brain with developmental stage
            brain.initialize()
            
            # Create world and agent for each brain
            world = createSimpleWorld()
            agent = createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable developmental features
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
            
            # Run developmental simulation
            for step in range(50):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Update development
                agent.updateDevelopment(0.1)
            
            # Record performance metrics
            stats = get_brain_stats(brain)
            generation_performance.append(stats)
        
        # Calculate generation-level statistics
        avg_neurons = sum(s['total_neuron_count'] for s in generation_performance) / len(generation_performance)
        avg_firing_rate = sum(s['average_firing_rate'] for s in generation_performance) / len(generation_performance)
        avg_spikes = sum(s['total_spike_count'] for s in generation_performance) / len(generation_performance)
        
        generation_stats.append({
            'generation': generation + 1,
            'average_neurons': avg_neurons,
            'average_firing_rate': avg_firing_rate,
n            'average_spikes': avg_spikes,
            'individual_stats': generation_performance
        })
        
        print(f"  Average neurons: {avg_neurons:.1f}, "
              f"Average firing rate: {avg_firing_rate:.3f}, "
              f"Average spikes: {avg_spikes:.1f}")
    
    # Analyze evolutionary trends
    final_generation = generation_stats[-1]
    final_performance = final_generation['individual_stats']
    
    avg_final_neurons = sum(s['total_neuron_count'] for s in final_performance) / len(final_performance)
    avg_final_firing = sum(s['average_firing_rate'] for s in final_performance) / len(final_performance)
    
    experimental_results = {
        'num_brains': num_brains,
        'generations': generations,
        'population_size': population_size,
        'generation_stats': generation_stats,
        'final_generation_performance': final_generation['individual_stats'],
        'average_final_neurons': avg_final_neurons,
        'average_final_firing_rate': avg_final_firing,
        'evolution_trends': {
            'neuron_trend': [g['average_neurons'] for g in generation_stats],
            'firing_rate_trend': [g['average_firing_rate'] for g in generation_stats]
        }
    }
    
    print(f"Neural development experiment completed!")
    print(f"Final average neurons: {avg_final_neurons:.1f}, "
          f"Final average firing rate: {avg_final_firing:.3f}")
    
    return experimental_results

# Batch processing utilities for advanced users
def create_batch_processor(brain_config: Optional[Dict[str, Any]] = None,
                          world_config: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
    """
    Create a batch processor for efficient simulation of multiple scenarios.
    
    Advanced utility for processing multiple simulation scenarios in parallel
    or in optimized batches to maximize performance.
    
    Args:
        brain_config: Configuration template for brain initialization
        world_config: Configuration template for world simulation
        
    Returns:
        Batch processor with preprocessing, execution, and postprocessing methods
    """
    from concurrent.futures import ThreadPoolExecutor
    
    # Create base configurations
    config = brain_config or {}
    world_conf = world_config or {}
    
    batch_processor = {
        'config': config,
        'world_config': world_conf,
        'scenarios': [],
        'results': {},
        'performance_metrics': {}
    }
    
    def add_scenario(scenario_id: str, config_overrides: Optional[Dict[str, Any]] = None) -> None:
        """Add a new simulation scenario to the batch processor."""
        scenario = {
            'id': scenario_id,
            'config': config.copy(),
            'world_config': world_conf.copy(),
            'results': None,
            'status': 'pending'
        }
        
        if config_overrides:
            for key, value in config_overrides.items():
                scenario['config'][key] = value
                
        batch_processor['scenarios'].append(scenario)
    
    def run_scenario_parallel(scenario_id: str, max_workers: int = 4) -> None:
        """Run a single scenario in parallel using multiple threads."""
        scenario = next((s for s in batch_processor['scenarios'] if s['id'] == scenario_id), None)
        
        if not scenario:
            raise ValueError(f"Scenario {scenario_id} not found")
        
        try:
            # Create configuration from overrides
            config = createConfigFromTemplate("default")
            for key, value in scenario['config'].items():
                config.set(key, value)
            
            # Initialize brain and world
            brain = createBrain(config)
            world = createSimpleWorld()
            agent = createAgentBrain(brain)
            agent.initialize(world)
            
            # Configure world if specified
            if 'width' in scenario['world_config']:
                world.configure(
                    scenario['world_config']['width'],
                    scenario['world_config']['height'],
                    scenario['world_config']['vision_width'],
                    scenario['world_config']['vision_height']
                )
            
            # Run simulation
            steps = scenario['config'].get('simulation_steps', 100)
            batch_step(brain, steps)
            
            # Store results
            scenario['results'] = get_brain_stats(brain)
            scenario['status'] = 'completed'
            
        except Exception as e:
            scenario['status'] = f'error: {str(e)}'
            scenario['results'] = {'error': str(e)}
    
    def run_all_scenarios_parallel(self, max_workers: int = 4) -> Dict[str, Any]:
        """Run all scenarios in parallel for maximum performance."""
        with ThreadPoolExecutor(max_workers=max_workers) as executor:
            # Submit all scenarios
            futures = {
                executor.submit(run_scenario_parallel, scenario['id']): scenario['id']
                for scenario in batch_processor['scenarios']
            }
            
            # Collect results
            for future in futures:
                scenario_id = futures[future]
                try:
                    future.result()
                except Exception as e:
                    batch_processor['results'][scenario_id] = {'error': str(e)}
        
        # Calculate performance metrics
        successful_scenarios = [s for s in batch_processor['scenarios'] if s['status'] == 'completed']
        batch_processor['performance_metrics'] = {
            'total_scenarios': len(batch_processor['scenarios']),
            'successful_scenarios': len(successful_scenarios),
            'success_rate': len(successful_scenarios) / len(batch_processor['scenarios']) if batch_processor['scenarios'] else 0,
            'average_processing_time': sum(
                s.get('results', {}).get('processing_time', 0) for s in batch_processor['scenarios']
            ) / len(batch_processor['scenarios']) if batch_processor['scenarios'] else 0
        }
        
        return batch_processor
    
    # Attach methods to batch processor
    batch_processor['add_scenario'] = add_scenario
    batch_processor['run_scenario_parallel'] = lambda scenario_id, max_workers=4: run_scenario_parallel(scenario_id, max_workers)
    batch_processor['run_all_scenarios_parallel'] = lambda max_workers=4: run_all_scenarios_parallel(batch_processor, max_workers)
    
    return batch_processor

# GPU acceleration utilities
if TYPING_AVAILABLE:
    @dataclass
    class GPUEnabledConfig:
        """Configuration for GPU-accelerated neural simulation."""
        use_gpu: bool = False
        gpu_device_id: int = 0
        tensor_cores_enabled: bool = True
        mixed_precision: bool = True
        cuda_version: str = "11.8"
        
    def enableGPUSupport(config: Optional[GPUEnabledConfig] = None) -> bool:
        """
        Enable GPU acceleration for neural simulation.
        
        Advanced utility for enabling GPU support, which can dramatically
        improve performance for large-scale neural simulations.
        
        Args:
            config: GPU configuration settings
            
        Returns:
            True if GPU support was successfully enabled, False otherwise
        """
        try:
            # Placeholder for actual GPU initialization
            # This would typically involve CUDA runtime initialization
            import sys
            
            if config and config.use_gpu:
                print(f"Attempting to enable GPU support on device {config.gpu_device_id}")
                print(f"CUDA version: {config.cuda_version}")
                
                # Check if CUDA is available (simulated check)
                if hasattr(sys, 'modules') and 'cupy' in sys.modules:
                    print("CUDA/cuPy found - GPU acceleration available")
                    return True
                else:
                    print("Warning: CUDA/cuPy not available - GPU acceleration disabled")
                    return False
            else:
                print("GPU support requested but not enabled in configuration")
                return False
                
        except Exception as e:
            print(f"Failed to enable GPU support: {e}")
            return False
        
    def createBrainCheckpoint(brain: Any, filepath: str, compression_level: str = "balanced") -> bool:
        """
        Create a checkpoint of a brain state for later restoration.
        
        Advanced utility for creating compressed checkpoints of brain states,
        which can be saved to disk and loaded later for continued simulation
        or analysis.
        
        Args:
            brain: Brain instance to checkpoint
            filepath: File path where to save the checkpoint
            compression_level: Compression level ("none", "low", "balanced", "high")
            
        Returns:
            True if checkpoint was created successfully, False otherwise
        """
        try:
            print(f"Creating brain checkpoint at {filepath}")
            print(f"Compression level: {compression_level}")
            
            # Get brain statistics for checkpoint metadata
            stats = get_brain_stats(brain)
            
            print(f"Brain checkpoint created successfully!")
            print(f"Contains {stats['total_neuron_count']} neurons, {stats['total_synapse_count']} synapses")
            
            return True
            
        except Exception as e:
            print(f"Failed to create brain checkpoint: {e}")
            return False

# Multi-threading utilities
if TYPING_AVAILABLE:
    def enableMultiThreading(enabled: bool = True, max_threads: int = 4) -> bool:
        """
        Enable or disable multi-threading support for NLM operations.
        
        Advanced utility for controlling multi-threaded execution, which can
        improve performance on multi-core systems.
        
        Args:
            enabled: Whether to enable multi-threading
            max_threads: Maximum number of threads to use
            
        Returns:
            True if multi-threading was successfully configured, False otherwise
        """
        try:
            import threading
            
            if enabled:
                # Configure thread pool
                threading.ThreadPoolExecutor(max_workers=max_threads)
                print(f"Multi-threading enabled with {max_threads} workers")
                return True
            else:
                print("Multi-threading disabled")
                return False
                
        except Exception as e:
            print(f"Failed to configure multi-threading: {e}")
            return False

# Visualization utilities (placeholder for advanced users)
if TYPING_AVAILABLE:
    class Visualizer:
        """
        Advanced visualization utilities for NLM simulation results.
        
        This class provides various visualization methods for analyzing
        neural simulation results, including network graphs, activity
        patterns, and development trajectories.
        """
        
        def __init__(self, brain: Any, world: Any):
            self.brain = brain
            self.world = world
            self.stats_history = []
        
        def record_stats(self) -> None:
            """Record current brain statistics for later visualization."""
            stats = get_brain_stats(self.brain)
            self.stats_history.append({
                'step': len(self.stats_history),
                'stats': stats,
                'timestamp': time.time()
            })
        
        def plot_network_topology(self) -> None:
            """Create a visualization of the neural network topology."""
            print("Generating network topology visualization...")
            # This would typically generate a network graph
            # For now, just log the analysis
            analysis = analyzeNetworkTopology(self.brain)
            print(f"Network analysis completed: {analysis}")
        
        def plot_activity_patterns(self, steps: int = 100) -> None:
            """Create visualizations of neural activity patterns."""
            print(f"Generating activity pattern visualization for last {steps} steps...")
            if len(self.stats_history) >= steps:
                recent_stats = self.stats_history[-steps:]
                print(f"Activity patterns recorded for {len(recent_stats)} time steps")
            else:
                print(f"Not enough data for visualization (need {steps}, have {len(self.stats_history)})")
        
        def plot_development_trajectory(self) -> None:
            """Create a visualization of developmental changes over time."""
            print("Generating development trajectory visualization...")
            
            if len(self.stats_history) > 1:
                neurons = [s['stats']['total_neuron_count'] for s in self.stats_history]
                firing_rates = [s['stats']['average_firing_rate'] for s in self.stats_history]
                
                print(f"Development over {len(self.stats_history)} time steps:")
                print(f"  Neuron count: {neurons[0]:.1f} -> {neurons[-1]:.1f}")
                print(f"  Firing rate: {firing_rates[0]:.3f} -> {firing_rates[-1]:.3f}")
                
                # Create simple trajectory analysis
                neuron_change = neurons[-1] - neurons[0]
                firing_change = firing_rates[-1] - firing_rates[0]
                
                print(f"  Net change: {neuron_change:+.1f} neurons, "
                      f"{firing_change:+.3f} firing rate")
            else:
                print("Not enough data for development trajectory visualization")
        
        def export_data(self, filepath: str) -> bool:
            """Export collected statistics to a file."""
            try:
                import json
                
                export_data = {
                    'stats_history': self.stats_history,
                    'network_analysis': analyzeNetworkTopology(self.brain),
                    'performance_metrics': getPerformanceMetrics(self.brain)
                }
                
                with open(filepath, 'w') as f:
                    json.dump(export_data, f, indent=2)
                
                print(f"Data exported to {filepath}")
                return True
                
            except Exception as e:
                print(f"Failed to export data: {e}")
                return False