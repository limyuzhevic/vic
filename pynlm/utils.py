"""
Utility functions for NLM Python bindings.

Provides helper functions for common operations, validation, and convenience
utilities to enhance the user experience with NLM simulations.
"""

import time
import datetime
import statistics
import json
import sys
from typing import Any, Callable, List, Dict, Optional
from pathlib import Path

from pynlm.exceptions import ValidationError

__all__ = [
    'validate_simulation_setup',
    'run_with_progress',
    'save_checkpoint',
    'benchmark_simulation',
    'get_simulation_stats',
    'track_learning_progress',
    'monitor_neuromodulation',
    'profile_performance',
    'export_simulation_data',
]


class SimulationProfiler:
    """Comprehensive simulation profiler for detailed performance analysis."""
    
    def __init__(self):
        self.start_time = None
        self.end_time = None
        self.step_times = []
        self.spike_counts = []
        self.firing_rates = []
        self.neuromodulation_levels = []
        self.curiosity_levels = []
        self.novelty_levels = []
        self.prediction_errors = []
        self.development_stages = []
        self.memory_usage = []
        self.cpu_times = []
        self.event_counts = {'spikes': 0, 'actions': 0, 'rewards': 0}
        
    def start(self):
        """Start profiling the simulation."""
        self.start_time = time.time()
        self.step_times = []
        
    def record_step(self, step_num, brain, agent=None, world=None):
        """Record metrics for a single simulation step."""
        step_start = time.process_time()
        
        # Collect brain metrics
        spike_count = brain.getTotalSpikeCount()
        firing_rate = brain.getAverageFiringRate()
        development_stage = brain.getDevelopmentalStage()
        
        # Collect agent metrics if available
        neuromodulation = curiosity = novelty = prediction_error = None
        if agent:
            neuromodulation = agent.getNeuromodulationLevel()
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            prediction_error = agent.getPredictionError()
        
        # Record metrics
        self.spike_counts.append(spike_count)
        self.firing_rates.append(firing_rate)
        self.development_stages.append(development_stage)
        self.neuromodulation_levels.append(neuromodulation)
        self.curiosity_levels.append(curiosity)
        self.novelty_levels.append(novelty)
        self.prediction_errors.append(prediction_error)
        
        # Event counting (would need to be extended based on actual events)
        self.event_counts['spikes'] += spike_count
        
        # Record timing
        step_time = time.process_time() - step_start
        self.step_times.append(step_time)
        
    def end(self):
        """End profiling and return summary statistics."""
        self.end_time = time.time()
        
        if not self.step_times:
            return self._get_empty_summary()
        
        return self._get_summary()
    
    def _get_summary(self):
        """Get comprehensive summary statistics."""
        total_time = self.end_time - self.start_time if self.start_time else 0
        
        return {
            'total_time': total_time,
            'total_steps': len(self.step_times),
            'steps_per_second': len(self.step_times) / total_time if total_time > 0 else 0,
            'avg_step_time': statistics.mean(self.step_times) if self.step_times else 0,
            'std_step_time': statistics.stdev(self.step_times) if len(self.step_times) > 1 else 0,
            'min_step_time': min(self.step_times) if self.step_times else 0,
            'max_step_time': max(self.step_times) if self.step_times else 0,
            
            'spike_stats': {
                'total': sum(self.spike_counts) if self.spike_counts else 0,
                'avg_per_step': statistics.mean(self.spike_counts) if self.spike_counts else 0,
                'avg_total': statistics.mean(self.spike_counts) if self.spike_counts else 0,
                'max_per_step': max(self.spike_counts) if self.spike_counts else 0,
            },
            
            'firing_rate_stats': {
                'avg': statistics.mean(self.firing_rates) if self.firing_rates else 0,
                'min': min(self.firing_rates) if self.firing_rates else 0,
                'max': max(self.firing_rates) if self.firing_rates else 0,
            },
            
            'development_stages': list(set(self.development_stages)),
            
            'neuromodulation_stats': self._get_list_stats(self.neuromodulation_levels),
            'curiosity_stats': self._get_list_stats(self.curiosity_levels),
            'novelty_stats': self._get_list_stats(self.novelty_levels),
            'prediction_error_stats': self._get_list_stats(self.prediction_errors),
            
            'events': self.event_counts.copy(),
            
            'profiling_info': {
                'start_time': datetime.datetime.fromtimestamp(self.start_time).isoformat() if self.start_time else None,
                'end_time': datetime.datetime.fromtimestamp(self.end_time).isoformat() if self.end_time else None,
            }
        }
    
    def _get_list_stats(self, values):
        """Get statistics for a list of values, filtering out None values."""
        filtered_values = [v for v in values if v is not None]
        if not filtered_values:
            return {'avg': 0, 'min': 0, 'max': 0, 'count': 0}
        
        return {
            'avg': statistics.mean(filtered_values),
            'min': min(filtered_values),
            'max': max(filtered_values),
            'count': len(filtered_values),
        }
    
    def _get_empty_summary(self):
        """Return empty summary when no data was collected."""
        return {
            'total_time': 0,
            'total_steps': 0,
            'steps_per_second': 0,
            'avg_step_time': 0,
            'std_step_time': 0,
            'min_step_time': 0,
            'max_step_time': 0,
            
            'spike_stats': {'total': 0, 'avg_per_step': 0, 'avg_total': 0, 'max_per_step': 0},
            'firing_rate_stats': {'avg': 0, 'min': 0, 'max': 0},
            'development_stages': [],
            'neuromodulation_stats': {'avg': 0, 'min': 0, 'max': 0, 'count': 0},
            'curiosity_stats': {'avg': 0, 'min': 0, 'max': 0, 'count': 0},
            'novelty_stats': {'avg': 0, 'min': 0, 'max': 0, 'count': 0},
            'prediction_error_stats': {'avg': 0, 'min': 0, 'max': 0, 'count': 0},
            'events': {'spikes': 0, 'actions': 0, 'rewards': 0},
            'profiling_info': {'start_time': None, 'end_time': None},
        }


def validate_simulation_setup(brain, world=None, agent=None):
    """Validate that brain, world, and agent are properly configured for simulation.
    
    Args:
        brain: The brain object to validate
        world: Optional world object to validate
        agent: Optional agent object to validate
        
    Returns:
        List of validation errors, empty if validation passes
        
    Raises:
        ValidationError: If any validation fails
    """
    errors = []
    
    # Validate brain
    if not brain:
        errors.append("Brain is not initialized")
    elif not hasattr(brain, 'initialize'):
        errors.append("Brain object does not have 'initialize' method")
    elif not hasattr(brain, 'step'):
        errors.append("Brain object does not have 'step' method")
    
    # Validate world if provided
    if world is not None:
        if not hasattr(world, 'update'):
            errors.append("World object does not have 'update' method")
        if not hasattr(world, 'getSensoryPercept'):
            errors.append("World object does not have 'getSensoryPercept' method")
        if not hasattr(world, 'applyMotorCommand'):
            errors.append("World object does not have 'applyMotorCommand' method")
    
    # Validate agent if provided
    if agent is not None:
        if not hasattr(agent, 'processSensoryInput'):
            errors.append("Agent object does not have 'processSensoryInput' method")
        if not hasattr(agent, 'decodeMotorCommand'):
            errors.append("Agent object does not have 'decodeMotorCommand' method")
    
    if errors:
        raise ValidationError("Simulation setup validation failed:\n" + "\n".join(f"  - {error}" for error in errors))
    
    return errors


def run_with_progress(brain, world=None, agent=None, num_steps=1000, 
                     progress_interval=100, progress_callback=None):
    """Run simulation with progress reporting and statistics collection.
    
    Args:
        brain: The brain object to simulate
        world: Optional world object for agent-based simulation
        agent: Optional agent object for processing sensory input
        num_steps: Number of simulation steps to run
        progress_interval: How often to print progress (every N steps)
        progress_callback: Optional callback function for custom progress reporting
        
    Returns:
        Dictionary with simulation statistics
    """
    print(f"Starting simulation for {num_steps} steps...")
    start_time = time.time()
    
    stats = {
        'steps_completed': 0,
        'start_time': datetime.datetime.now(),
        'end_time': None,
        'total_spike_count': 0,
        'average_firing_rate': 0.0,
        'max_firing_rate': 0.0,
        'min_firing_rate': 0.0,
        'development_stage': None,
    }
    
    for step in range(num_steps):
        try:
            # Run simulation step
            if world is not None and agent is not None:
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            else:
                brain.step(step)
            
            # Update statistics
            spike_count = brain.getTotalSpikeCount()
            firing_rate = brain.getAverageFiringRate()
            
            stats['total_spike_count'] = spike_count
            stats['average_firing_rate'] = firing_rate
            stats['max_firing_rate'] = max(stats['max_firing_rate'], firing_rate)
            stats['min_firing_rate'] = min(stats['min_firing_rate'], firing_rate)
            stats['development_stage'] = brain.getDevelopmentalStage()
            stats['steps_completed'] = step + 1
            
            # Report progress
            if step % progress_interval == 0 and step > 0:
                progress = (step / num_steps) * 100
                elapsed_time = time.time() - start_time
                print(f"Progress: {progress:.1f}% (Step {step}/{num_steps})")
                print(f"  Time elapsed: {elapsed_time:.2f}s")
                print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
                print(f"  Total spikes: {spike_count}")
                print(f"  Avg firing rate: {firing_rate:.3f}")
                if world:
                    print(f"  Simulation time: {world.getSimulationTime():.2f}")
                print()
                
                # Call custom progress callback if provided
                if progress_callback:
                    progress_callback(step, num_steps, stats)
                    
        except Exception as e:
            print(f"Error at step {step}: {e}")
            raise
    
    # Calculate final statistics
    end_time = time.time()
    stats['end_time'] = datetime.datetime.now()
    stats['elapsed_time'] = end_time - start_time
    
    if num_steps > 0:
        stats['average_firing_rate'] = stats['total_spike_count'] / num_steps
    
    print("\n" + "="*50)
    print("Simulation Complete!")
    print("="*50)
    print(f"Total steps completed: {stats['steps_completed']}")
    print(f"Total time elapsed: {stats['elapsed_time']:.2f} seconds")
    print(f"Total spike count: {stats['total_spike_count']}")
    print(f"Average firing rate: {stats['average_firing_rate']:.3f} Hz")
    print(f"Max firing rate: {stats['max_firing_rate']:.3f} Hz")
    print(f"Min firing rate: {stats['min_firing_rate']:.3f} Hz")
    print(f"Development stage: {stats['development_stage']}")
    
    return stats


def save_checkpoint(brain, filepath, description="Simulation checkpoint"):
    """Save brain state to a checkpoint file.
    
    Args:
        brain: The brain object to save
        filepath: Path to save the checkpoint file
        description: Optional description for the checkpoint
        
    Returns:
        Dictionary with checkpoint metadata
    """
    filepath = Path(filepath)
    filepath.parent.mkdir(parents=True, exist_ok=True)
    
    # Save the brain state
    brain.save(str(filepath))
    
    # Create metadata file
    metadata = {
        'timestamp': datetime.datetime.now().isoformat(),
        'description': description,
        'brain_info': {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'development_stage': brain.getDevelopmentalStage(),
        }
    }
    
    metadata_path = filepath.with_suffix('.meta.json')
    with open(metadata_path, 'w') as f:
        json.dump(metadata, f, indent=2)
    
    print(f"Checkpoint saved: {filepath}")
    print(f"Metadata saved: {metadata_path}")
    print(f"Description: {description}")
    
    return metadata


def benchmark_simulation(brain, world=None, agent=None, num_steps=1000, 
                        num_runs=5, warmup_steps=10):
    """Benchmark simulation performance.
    
    Args:
        brain: The brain object to benchmark
        world: Optional world object for agent-based simulation
        agent: Optional agent object for processing sensory input
        num_steps: Number of steps per benchmark run
        num_runs: Number of benchmark runs to average over
        warmup_steps: Number of warmup steps to discard
        
    Returns:
        Dictionary with benchmark statistics
    """
    print(f"Running benchmark: {num_runs} runs of {num_steps} steps each")
    print(f"Warmup: {warmup_steps} steps")
    
    times = []
    
    for run in range(num_runs):
        # Warmup
        for step in range(warmup_steps):
            if world is not None and agent is not None:
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            else:
                brain.step(step)
        
        # Benchmark run
        start_time = time.time()
        
        for step in range(num_steps):
            if world is not None and agent is not None:
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            else:
                brain.step(step)
        
        end_time = time.time()
        run_time = end_time - start_time
        times.append(run_time)
        
        print(f"  Run {run + 1}/{num_runs}: {run_time:.3f}s")
    
    # Calculate statistics
    avg_time = statistics.mean(times)
    std_time = statistics.stdev(times) if len(times) > 1 else 0.0
    steps_per_second = num_steps / avg_time
    
    benchmark_stats = {
        'num_runs': num_runs,
        'num_steps': num_steps,
        'warmup_steps': warmup_steps,
        'times': times,
        'average_time': avg_time,
        'std_time': std_time,
        'steps_per_second': steps_per_second,
        'min_time': min(times),
        'max_time': max(times),
    }
    
    print("\n" + "="*50)
    print("Benchmark Complete!")
    print("="*50)
    print(f"Average time: {avg_time:.3f}s")
    print(f"Standard deviation: {std_time:.3f}s")
    print(f"Steps per second: {steps_per_second:.1f}")
    print(f"Min time: {benchmark_stats['min_time']:.3f}s")
    print(f"Max time: {benchmark_stats['max_time']:.3f}s")
    
    return benchmark_stats


def get_simulation_stats(brain, world=None, agent=None, duration=None):
    """Collect comprehensive statistics from a running simulation.
    
    Args:
        brain: The brain object to collect statistics from
        world: Optional world object
        agent: Optional agent object
        duration: Optional duration in seconds for timing
        
    Returns:
        Dictionary with comprehensive simulation statistics
    """
    stats = {
        'timestamp': datetime.datetime.now().isoformat(),
        'brain_stats': {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'development_stage': brain.getDevelopmentalStage(),
            'excitatory_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'total_spike_count': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
        },
        'agent_stats': {},
        'world_stats': {},
        'simulation_stats': {
            'duration': duration,
            'memory_usage': sys.getsizeof(brain) if brain else 0,
        }
    }
    
    if agent:
        stats['agent_stats'] = {
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'reward_modulation_enabled': agent.isRewardModulationEnabled(),
            'structural_plasticity_enabled': agent.isStructuralPlasticityEnabled(),
            'development_enabled': agent.isDevelopmentEnabled(),
            'curiosity_enabled': agent.isCuriosityEnabled(),
        }
    
    if world:
        stats['world_stats'] = {
            'width': world.getWidth(),
            'height': world.getHeight(),
            'simulation_time': world.getSimulationTime(),
            'max_energy': world.getMaxEnergy(),
            'energy_decay_rate': world.getEnergyDecayRate(),
        }
    
    return stats


def track_learning_progress(brain, agent, steps_completed, initial_stats):
    """Track and analyze learning progress over time.
    
    Args:
        brain: The brain object
        agent: The agent object
        steps_completed: Number of steps completed
        initial_stats: Initial statistics dictionary
        
    Returns:
        Dictionary with learning progress analysis
    """
    current_stats = get_simulation_stats(brain, agent=agent)
    
    progress = {
        'step': steps_completed,
        'timestamp': datetime.datetime.now().isoformat(),
        'stats': current_stats,
        'improvement_metrics': {},
        'convergence_analysis': {},
    }
    
    if initial_stats:
        # Calculate improvement metrics
        initial_firing_rate = initial_stats['brain_stats']['firing_rate']
        current_firing_rate = current_stats['brain_stats']['firing_rate']
        
        progress['improvement_metrics'] = {
            'firing_rate_change': current_firing_rate - initial_firing_rate,
            'firing_rate_change_percent': ((current_firing_rate - initial_firing_rate) / initial_firing_rate * 100) if initial_firing_rate > 0 else 0,
            'spike_count_change': current_stats['brain_stats']['total_spikes'] - initial_stats['brain_stats']['total_spikes'],
            'development_stage_change': current_stats['brain_stats']['development_stage'] - initial_stats['brain_stats']['development_stage'],
            'neuromodulation_change': current_stats['agent_stats']['neuromodulation_level'] - initial_stats['agent_stats']['neuromodulation_level'],
        }
    
    # Convergence analysis
    development_stage = current_stats['brain_stats']['development_stage']
    progress['convergence_analysis'] = {
        'development_stage': development_stage,
        'is_converged': development_stage.name in ['Adult', 'Aging'],
        'learning_activity': current_stats['agent_stats']['curiosity_level'] > 0.1 if 'curiosity_level' in current_stats['agent_stats'] else False,
    }
    
    return progress


def monitor_neuromodulation(agent, duration_seconds=60):
    """Monitor neuromodulation levels over time.
    
    Args:
        agent: The agent object to monitor
        duration_seconds: Duration of monitoring in seconds
        
    Returns:
        Dictionary with neuromodulation monitoring data
    """
    print(f"Monitoring neuromodulation for {duration_seconds} seconds...")
    
    monitoring_data = {
        'start_time': datetime.datetime.now().isoformat(),
        'duration_seconds': duration_seconds,
        'samples': [],
        'neuromodulation_stats': {},
        'curiosity_stats': {},
        'novelty_stats': {},
    }
    
    start_time = time.time()
    sample_interval = 1.0  # Sample every second
    
    while time.time() - start_time < duration_seconds:
        sample = {
            'timestamp': time.time() - start_time,
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
        }
        monitoring_data['samples'].append(sample)
        
        # Collect statistics
        neuromod_levels = [s['neuromodulation_level'] for s in monitoring_data['samples']]
        curiosity_levels = [s['curiosity_level'] for s in monitoring_data['samples']]
        novelty_levels = [s['novelty_level'] for s in monitoring_data['samples']]
        
        monitoring_data['neuromodulation_stats'] = {
            'avg': statistics.mean(neuromod_levels) if neuromod_levels else 0,
            'min': min(neuromod_levels) if neuromod_levels else 0,
            'max': max(neuromod_levels) if neuromod_levels else 0,
            'std': statistics.stdev(neuromod_levels) if len(neuromod_levels) > 1 else 0,
        }
        
        monitoring_data['curiosity_stats'] = {
            'avg': statistics.mean(curiosity_levels) if curiosity_levels else 0,
            'min': min(curiosity_levels) if curiosity_levels else 0,
            'max': max(curiosity_levels) if curiosity_levels else 0,
            'std': statistics.stdev(curiosity_levels) if len(curiosity_levels) > 1 else 0,
        }
        
        monitoring_data['novelty_stats'] = {
            'avg': statistics.mean(novelty_levels) if novelty_levels else 0,
            'min': min(novelty_levels) if novelty_levels else 0,
            'max': max(novelty_levels) if novelty_levels else 0,
            'std': statistics.stdev(novelty_levels) if len(novelty_levels) > 1 else 0,
        }
        
        time.sleep(sample_interval)
    
    monitoring_data['end_time'] = datetime.datetime.now().isoformat()
    
    print(f"Neuromodulation monitoring complete!")
    print(f"Samples collected: {len(monitoring_data['samples'])}")
    print(f"Average neuromodulation level: {monitoring_data['neuromodulation_stats']['avg']:.3f}")
    print(f"Average curiosity level: {monitoring_data['curiosity_stats']['avg']:.3f}")
    print(f"Average novelty level: {monitoring_data['novelty_stats']['avg']:.3f}")
    
    return monitoring_data


def profile_simulation(brain, world=None, agent=None, num_steps=1000, 
                      profiling_interval=10):
    """Profile simulation performance with detailed metrics.
    
    Args:
        brain: The brain object to profile
        world: Optional world object
        agent: Optional agent object
        num_steps: Number of simulation steps to run
        profiling_interval: How often to record detailed metrics
        
    Returns:
        Dictionary with comprehensive profiling data
    """
    print(f"Starting simulation profiling for {num_steps} steps...")
    profiler = SimulationProfiler()
    profiler.start()
    
    for step in range(num_steps):
        profiler.record_step(step, brain, agent, world)
        
        # Record detailed metrics at intervals
        if step % profiling_interval == 0 and step > 0:
            # Collect snapshot of current metrics
            pass
    
    # Get final profiling results
    profile_results = profiler.end()
    
    print(f"\n" + "="*50)
    print("Simulation Profiling Complete!")
    print("="*50)
    print(f"Total time: {profile_results['total_time']:.3f}s")
    print(f"Total steps: {profile_results['total_steps']}")
    print(f"Steps per second: {profile_results['steps_per_second']:.1f}")
    print(f"Average spike rate: {profile_results['spike_stats']['avg_per_step']:.2f} spikes/step")
    print(f"Max spike rate: {profile_results['spike_stats']['max_per_step']:.2f} spikes/step")
    print(f"Development stages: {profile_results['development_stages']}")
    
    return profile_results


def export_simulation_data(data, filepath, format='json'):
    """Export simulation data to file.
    
    Args:
        data: Simulation data dictionary to export
        filepath: Path to export file
        format: Export format ('json', 'csv', 'pickle')
        
    Returns:
        Boolean indicating success
    """
    filepath = Path(filepath)
    filepath.parent.mkdir(parents=True, exist_ok=True)
    
    try:
        if format.lower() == 'json':
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2, default=str)
                
        elif format.lower() == 'csv':
            # Convert to CSV format (simplified)
            import csv
            with open(filepath, 'w', newline='') as f:
                if data and isinstance(data, dict):
                    # Get headers from first level keys
                    headers = list(data.keys())
                    writer = csv.writer(f)
                    writer.writerow(headers)
                    
                    # Write data rows
                    for key, value in data.items():
                        if isinstance(value, dict):
                            row = [value.get(k, '') for k in headers]
                        else:
                            row = [value] + [''] * (len(headers) - 1)
                        writer.writerow(row)
                        
        elif format.lower() == 'pickle':
            import pickle
            with open(filepath, 'wb') as f:
                pickle.dump(data, f)
                
        else:
            raise ValueError(f"Unsupported export format: {format}")
        
        print(f"Data exported successfully to {filepath} ({format.upper()} format)")
        return True
        
    except Exception as e:
        print(f"Error exporting data: {e}")
        return False
