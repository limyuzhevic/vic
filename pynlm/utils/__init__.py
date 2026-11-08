#!/usr/bin/env python3
"""
Utility functions for the NLM Python package.

This module provides additional utility functions and high-level APIs to make working with NLM more convenient and powerful.
"""

import pynlm
from pynlm import createBrain, createDefaultConfig
from typing import List, Dict, Any, Optional, Callable
import time
import gc
import matplotlib.pyplot as plt
import numpy as np
def run_simple_simulation(brain: Any, steps: int) -> Dict[str, Any]:
    """
    Run a simple brain simulation and return statistics.
    
    Args:
        brain: NLM brain object
        steps: Number of simulation steps
        
    Returns:
        Dictionary with simulation statistics
    """
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
    
    end_time = time.time()
    
    stats = {
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'development_stage': brain.getDevelopmentalStage(),
        'total_neuron_count': brain.getTotalNeuronCount(),
        'total_synapse_count': brain.getTotalSynapseCount(),
        'simulation_time': end_time - start_time,
        'steps_per_second': steps / (end_time - start_time) if end_time > start_time else 0,
        'steps': steps
    }
    
    return stats
def run_agent_simulation(brain: Any, world: Any, agent: Any, steps: int) -> Dict[str, Any]:
    """
    Run a complete agent simulation with world interaction.
    
    Args:
        brain: NLM brain object
        world: NLM world object
        agent: NLM agent object
        steps: Number of simulation steps
        
    Returns:
        Dictionary with simulation statistics
    """
    start_time = time.time()
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation if available
        if hasattr(agent, 'applyRewardModulation'):
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development if available
        if hasattr(agent, 'updateDevelopment'):
            agent.updateDevelopment(0.1)
    
    end_time = time.time()
    
    stats = {
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'development_stage': brain.getDevelopmentalStage(),
        'curiosity_level': agent.getCuriosityLevel() if hasattr(agent, 'getCuriosityLevel') else 0.0,
        'novelty_level': agent.getNoveltyLevel() if hasattr(agent, 'getNoveltyLevel') else 0.0,
        'neuromodulation_level': agent.getNeuromodulationLevel() if hasattr(agent, 'getNeuromodulationLevel') else 0.0,
        'prediction_error': agent.getPredictionError() if hasattr(agent, 'getPredictionError') else 0.0,
        'simulation_time': end_time - start_time,
        'steps_per_second': steps / (end_time - start_time) if end_time > start_time else 0,
        'steps': steps,
        'world_width': world.getWidth() if hasattr(world, 'getWidth') else 0,
        'world_height': world.getHeight() if hasattr(world, 'getHeight') else 0
    }
    
    return stats
def run_batch_simulation(configs: List[Any], steps: int) -> List[Dict[str, Any]]:
    """
    Run multiple brains in batch processing.
    
    Args:
        configs: List of NLM configurations
        steps: Number of simulation steps for each brain
        
    Returns:
        List of simulation statistics for each brain
    """
    results = []
    
    for i, config in enumerate(configs):
        brain = createBrain(config)
        brain.initialize()
        
        for step in range(steps):
            brain.step(step)
        
        stats = {
            'brain_id': i,
            'config': config,
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'development_stage': brain.getDevelopmentalStage(),
            'steps': steps
        }
        
        results.append(stats)
        
        # Clean up memory
        del brain
        gc.collect()
    
    return results
def collect_spike_data(brain: Any, steps: int) -> Dict[str, Any]:
    """
    Collect spike data for neural analysis.
    
    Args:
        brain: NLM brain object
        steps: Number of simulation steps
        
    Returns:
        Dictionary with spike data
    """
    spike_data = {
        'spike_timestamps': [],
        'neuron_activities': [],
        'regional_activity': {},
        'total_spikes': 0,
        'active_neurons': [],
        'firing_rates': [],
        'excitatory_activity': [],
        'inhibitory_activity': [],
        'steps': steps
    }
    
    for step in range(steps):
        brain.step(step)
        
        # Collect spike data
        current_spikes = brain.getTotalSpikeCount()
        spike_data['spike_timestamps'].append(step)
        spike_data['total_spikes'] = current_spikes
        
        # Collect neuron activities
        spike_data['neuron_activities'].append({
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'active_neuron_count': brain.getActiveNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'development_stage': brain.getDevelopmentalStage()
        })
        
        # Collect regional activity
        for region in brain.getRegions():
            region_name = f"Region_{region.getId().value}"
            spike_data['regional_activity'][region_name] = {
                'neuron_count': region.getNeuronCount(),
                'firing_neuron_count': region.getFiringNeuronCount(),
                'synapse_count': region.getSynapseCount()
            }
    
    return spike_data
def analyze_neural_patterns(spike_data: Dict[str, Any]) -> Dict[str, Any]:
    """
    Analyze neural patterns from spike data.
    
    Args:
        spike_data: Dictionary with spike data
        
    Returns:
        Dictionary with analysis results
    """
    if not spike_data['spike_timestamps']:
        return {'error': 'No spike data available'}
    
    analysis = {
        'total_steps': spike_data['steps'],
        'total_spikes': spike_data['total_spikes'],
        'spike_rate': spike_data['total_spikes'] / spike_data['steps'] if spike_data['steps'] > 0 else 0,
        'neurons_activated': len([d for d in spike_data['neuron_activities'] if d['firing_neuron_count'] > 0]),
        'average_firing_rate': np.mean([d['avg_firing_rate'] for d in spike_data['neuron_activities']]),
        'e_i_ratio_mean': np.mean([d['excitation_inhibition_ratio'] for d in spike_data['neuron_activities']]),
        'development_stage': spike_data['neuron_activities'][-1]['development_stage'],
        'regional_patterns': {}
    }
    
    # Analyze regional patterns
    for region_name, region_data in spike_data['regional_patterns'].items():
        analysis['regional_patterns'][region_name] = {
            'neuron_count': region_data['neuron_count'],
            'firing_neuron_count': region_data['firing_neuron_count'],
            'firing_rate': region_data['firing_neuron_count'] / region_data['neuron_count'] if region_data['neuron_count'] > 0 else 0,
            'synapse_count': region_data['synapse_count']
        }
    
    return analysis
def compute_connectivity(brain: Any) -> Dict[str, Any]:
    """
    Compute connectivity matrix for the brain.
    
    Args:
        brain: NLM brain object
        
    Returns:
        Dictionary with connectivity information
    """
    connectivity = {
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'synapses_per_neuron': brain.getTotalSynapseCount() / brain.getTotalNeuronCount() if brain.getTotalNeuronCount() > 0 else 0,
        'regions': [],
        'regional_connectivity': {},
        'connectivity_density': 0.0
    }
    
    # Compute regional connectivity
    for region in brain.getRegions():
        region_id = region.getId().value
        region_synapses = region.getSynapseCount()
        region_neurons = region.getNeuronCount()
        
        connectivity['regions'].append({
            'id': region_id,
            'neuron_count': region_neurons,
            'synapse_count': region_synapses,
            'synapses_per_neuron': region_synapses / region_neurons if region_neurons > 0 else 0
        })
        
        connectivity['regional_connectivity'][f'Region_{region_id}'] = {
            'neuron_count': region_neurons,
            'synapse_count': region_synapses,
            'connectivity_density': region_synapses / region_neurons if region_neurons > 0 else 0
        }
    
    # Compute overall connectivity density
    total_neurons = brain.getTotalNeuronCount()
    total_synapses = brain.getTotalSynapseCount()
    connectivity['connectivity_density'] = total_synapses / total_neurons if total_neurons >  cadena else 0
    
    return connectivity
def debug_neural_activity(brain: Any, step: int) -> Dict[str, Any]:
    """
    Debug neural activity at a specific step.
    
    Args:
        brain: NLM brain object
        step: Simulation step to debug
        
    Returns:
        Dictionary with debug information
    """
    debug_info = {
        'step': step,
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neuron_count': brain.getFiringNeuronCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'e_i_ratio': brain.getExcitationInhibitionRatio(),
        'development_stage': brain.getDevelopmentalStage(),
        'regions': [],
        'neuron_details': [],
        'synapse_details': []
    }
    
    # Collect regional details
    for region in brain.getRegions():
        region_id = region.getId().value
        debug_info['regions'].append({
            'id': region_id,
            'neuron_count': region.getNeuronCount(),
            'firing_neuron_count': region.getFiringNeuronCount(),
            'synapse_count': region.getSynapseCount(),
            'firing_rate': region.getFiringNeuronCount() / region.getNeuronCount() if region.getNeuronCount() > 0 else 0
        })
    
    return debug_info
def validate_brain_state(brain: Any) -> bool:
    """
    Validate the state of a brain.
    
    Args:
        brain: NLM brain object
        
    Returns:
        True if brain state is valid, False otherwise
    """
    try:
        # Check basic properties
        if brain.getTotalNeuronCount() <= 0:
            return False
        
        if brain.getTotalSynapseCount() <= 0:
            return False
        
        # Check consistency
        if brain.getTotalSynapseCount() / brain.getTotalNeuronCount() > 10.0:
            return False
        
        # Check development stage
        development_stage = brain.getDevelopmentalStage()
        if development_stage.value == 0:
            return True  # Initial stage is valid
        
        return True
    except Exception as e:
        print(f"Validation error: {e}")
        return False
def find_anomalies(brain: Any, window_size: int = 100) -> List[Dict[str, Any]]:
    """
    Find anomalies in brain activity.
    
    Args:
        brain: NLM brain object
        window_size: Window size for anomaly detection
        
    Returns:
        List of anomalies found
    """
    anomalies = []
    
    for step in range(window_size, brain.getTotalSpikeCount()):
        # Get activity data for current window
        window_activity = []
        for i in range(window_size):
            current_step = step - i
            if current_step <= 0:
                continue
            
            # Get activity for this step (simplified)
            window_activity.append({
                'firing_neuron_count': brain.getFiringNeuronCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio()
            })
        
        # Check for anomalies
        if window_activity:
            firing_counts = [act['firing_neuron_count'] for act in window_activity]
            avg_firing = np.mean(firing_counts)
            std_firing = np.std(firing_counts)
            
            # Find anomaly: firing rate more than 3 standard deviations from mean
            current_activity = window_activity[-1]
            if abs(current_activity['avg_firing_rate'] - avg_firing) > 3 * std_firing:
                anomalies.append({
                    'step': step,
                    'firing_rate': current_activity['avg_firing_rate'],
                    'mean_firing_rate': avg_firing,
                    'standard_deviation': std_firing,
                    'type': 'firing_rate_anomaly'
                })
    
    return anomalies
def save_results(filename: str, data: Dict[str, Any]) -> bool:
    """
    Save results to file.
    
    Args:
        filename: Output filename
        data: Data to save
        
    Returns:
        True if save successful, False otherwise
    """
    try:
        with open(filename, 'w') as f:
            f.write(str(data))
        return True
    except Exception as e:
        print(f"Error saving results: {e}")
        return False
def load_results(filename: str) -> Optional[Dict[str, Any]]:
    """
    Load results from file.
    
    Args:
        filename: Input filename
        
    Returns:
        Loaded data or None if error
    """
    try:
        with open(filename, 'r') as f:
            return eval(f.read())
    except Exception as e:
        print(f"Error loading results: {e}")
        return None
def plot_neural_activity(brain: Any, output_file: Optional[str] = None) -> bool:
    """
    Plot neural activity.
    
    Args:
        brain: NLM brain object
        output_file: Optional output file
        
    Returns:
        True if plot successful, False otherwise
    """
    try:
        # Collect activity data
        steps = min(1000, 10000)  # Limit to reasonable size
        activity_data = []
        
        for step in range(steps):
            brain.step(step)
            activity_data.append({
                'step': step,
                'firing_neuron_count': brain.getFiringNeuronCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                'development_stage': brain.getDevelopmentalStage()
            })
        
        if not activity_data:
            return False
        
        # Create plots
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('Neural Activity Analysis', fontsize=16)
        
        # Plot 1: Firing neuron count
        axes[0, 0].plot([d['step'] for d in activity_data], 
                       [d['firing_neuron_count'] for d in activity_data],
                       linewidth=1)
        axes[0, 0].set_title('Firing Neuron Count')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Firing Neuron Count')
        
        # Plot 2: Average firing rate
        axes[0, 1].plot([d['step'] for d in activity_data], 
                       [d['avg_firing_rate'] for d in activity_data],
                       color='orange', linewidth=1)
        axes[0, 1].set_title('Average Firing Rate')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Firing Rate (Hz)')
        
        # Plot 3: Excitation/Inhibition ratio
        axes[1, 0].plot([d['step'] for d in activity_data], 
                       [d['excitation_inhibition_ratio'] for d in activity_data],
                       color='green', linewidth=1)
        axes[1, 0].set_title('Excitation/Inhibition Ratio')
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('E/I Ratio')
        
        # Plot 4: Development stage
        axes[1, 1].step([d['step'] for d in activity_data], 
                      [d['development_stage'].value for d in activity_data],
                      where='post')
        axes[1, 1].set_title('Development Stage')
        axes[1, 1].set_xlabel('Step')
        axes[1, 1].set_ylabel('Development Stage')
        axes[1, 1].legend([0, 1, 2, 3, 4], ['Initial', 'Critical', 'Mature', 'Adult', 'Aging'])
        
        # Save plot
        if output_file:
            plt.savefig(output_file, bbox_inches='tight')
        else:
            plt.savefig('neural_activity.png', bbox_inches='tight')
        
        plt.close()
        return True
        
    except Exception as e:
        print(f"Error plotting neural activity: {e}")
        return False
def plot_spike_patterns(brain: Any, output_file: Optional[str] = None) -> bool:
    """
    Plot spike patterns.
    
    Args:
        brain: NLM brain object
        output_file: Optional output file
        
    Returns:
        True if plot successful, False otherwise
    """
    try:
        # Collect spike data
        steps = min(500, 2000)  # Limit to reasonable size
        
        # Run simulation
        for step in range(steps):
            brain.step(step)
        
        # Create plot
        fig, ax = plt.subplots(figsize=(10, 6))
        
        # Plot spike timeline
        ax.step(range(steps), [brain.getTotalSpikeCount() for _ in range(steps)],
               where='post', linewidth=0.5)
        ax.set_title('Spike Timeline')
        ax.set_xlabel('Step')
        ax.set_ylabel('Total Spike Count')
        
        # Add statistics
        fig_info = f"Total spikes: {brain.getTotalSpikeCount()}\n"
        fig_info += f"Firing neurons: {brain.getFiringNeuronCount()}\n"
        fig_info += f"Average firing rate: {brain.getAverageFiringRate():.3f} Hz\n"
        fig_info += f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}\n"
        
        ax.text(0.02, 0.98, fig_info, transform=ax.transAxes,
               verticalalignment='top', bbox=dict(boxstyle='round', facecolor='blue', alpha=0.8))
        
        # Save plot
        if output_file:
            plt.savefig(output_file, bbox_inches='tight')
        else:
            plt.savefig('spike_patterns.png', bbox_inches='tight')
        
        plt.close()
        return True
        
    except Exception as e:
        print(f"Error plotting spike patterns: {e}")
        return False
def plot_development_progress(brain: Any, output_file: Optional[str] = None) -> bool:
    """
    Plot development progress.
    
    Args:
        brain: NLM brain object
        output_file: Optional output file
        
    Returns:
        True if plot successful, False otherwise
    """
    try:
        # Collect development data
        development_stages = []
        stage_counts = {}
        
        # Run simulation
        for step in range(0, 10000, 100):  # Every 100 steps
            brain.step(step)
            stage = brain.getDevelopmentalStage()
            development_stages.append((step, stage.value))
            
            if stage.value not in stage_counts:
                stage_counts[stage.value] = 0
            stage_counts[stage.value] += 1
        
        if not development_stages:
            return False
        
        # Create plot
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('Development Progress Analysis', fontsize=16)
        
        # Plot 1: Development stage over time
        steps, stages = zip(*development_stages)
        axes[0, 0].step(steps, stages, where='post')
        axes[0, 0].set_title('Development Stage Over Time')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Development Stage')
        axes[0, 0].legend([0, 1, 2, 3, 4], ['Initial', 'Critical', 'Mature', 'Adult', 'Aging'])
        
        # Plot 2: Time spent in each stage
        axes[0, 1].bar(stage_counts.keys(), stage_counts.values())
        axes[0, 1].set_title('Time Spent in Each Development Stage')
        axes[0, 1].set_xlabel('Development Stage')
        axes[0, 1].set_ylabel('Number of Steps')
        
        # Plot 3: Current brain statistics
        stats_text = f"Total Neurons: {brain.getTotalNeuronCount()}\n"
        stats_text += f"Total Synapses: {brain.getTotalSynapseCount()}\n"
        stats_text += f"Firing Neurons: {brain.getFiringNeuronCount()}\n"
        stats_text += f"Average Firing Rate: {brain.getAverageFiringRate():.3f} Hz\n"
        stats_text += f"E/I Ratio: {brain.getExcitationInhibitionRatio():.3f}\n"
        
        axes[1, 0].text(0.1, 0.5, stats_text, fontsize=12, verticalalignment='center',
                       bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.8))
        axes[1, 0].set_title('Current Brain Statistics')
        axes[1, 0].set_xticks([])
        axes[1, 0].set_yticks([])
        
        # Plot 4: Development timeline
        axes[1, 1].plot(steps, stages, 'b-', linewidth=2, label='Development Stage')
        axes[1, 1].fill_between(steps, min(stages), max(stages), alpha=0.1, color='blue')
        axes[1, 1].set_title('Development Timeline')
        axes[1, 1].set_xlabel('Step')
        axes[1, 1].set_ylabel('Development Stage')
        
        # Save plot
        if output_file:
            plt.savefig(output_file, bbox_inches='tight')
        else:
            plt.savefig('development_progress.png', bbox_inches='tight')
        
        plt.close()
        return True
        
    except Exception as e:
        print(f"Error plotting development progress: {e}")
        return False
if __name__ == "__main__":
    # Test utility functions
    print("Testing NLM utility functions...")
    
    # Test simple simulation
    config = pynlm.createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    # Run simple simulation
    stats = run_simple_simulation(brain, 100)
    print(f"Simple simulation: {stats}")
    
    # Test spike data collection
    spike_data = collect_spike_data(brain, 100)
    print(f"Spike data collected: {len(spike_data['spike_timestamps'])} timestamps")
    
    # Test neural pattern analysis
    analysis = analyze_neural_patterns(spike_data)
    print(f"Neural pattern analysis: {analysis}")
    
    print("Utility functions test completed successfully!")