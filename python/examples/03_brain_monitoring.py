#!/usr/bin/env python3
"""
Example: Advanced Brain Monitoring and Analysis

This example demonstrates advanced techniques for monitoring, analyzing,
and controlling NLM brain behavior using Python bindings.

Features demonstrated:
- Real-time performance monitoring
- Memory system analysis
- Developmental stage tracking
- Synaptic weight analysis
- Visualization-ready data collection
"""

import pynlm
import numpy as np
from collections import defaultdict
import json
import time

def collect_performance_data(brain, step_interval=10):
    """
    Collect comprehensive performance data from the brain.
    
    Args:
        brain: NLM Brain instance
        step_interval: Number of steps between data collections
    
    Returns:
        Dictionary containing all collected metrics
    """
    data = {
        'steps': [],
        'spike_counts': [],
        'firing_rates': [],
        'active_neuron_counts': [],
        'excitement_inhibition_ratios': [],
        'development_stages': [],
        'working_memory_usage': [],
        'episodic_memory_counts': [],
        'neuromodulation_levels': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'prediction_errors': []
    }
    
    print("Collecting performance data...")
    print(f"{'Step':>6} {'Spikes':>10} {'FiringRate':>10} {'Active':>8} {'E/I':>8} {'Dev':>8} {'Memory':>10} {'Novelty':>10}")
    print("-" * 80)
    
    for step in range(0, 1000, step_interval):
        # Collect all metrics
        spike_count = brain.getTotalSpikeCount()
        firing_rate = brain.getAverageFiringRate()
        active_count = brain.getFiringNeuronCount()
        ei_ratio = brain.getExcitationInhibitionRatio()
        dev_stage = brain.getDevelopmentalStage()
        
        # Memory systems
        working_mem = brain.getWorkingMemory()
        episodic_mem = brain.getEpisodicMemory()
        wm_usage = working_mem.getActiveTraces() if working_mem else 0
        em_count = episodic_mem.getEpisodeCount() if episodic_mem else 0
        
        # Neuromodulation (would require agent access)
        # neuromod_level = agent.getNeuromodulationLevel() if agent else 0.0
        neuromod_level = 0.0  # Placeholder
        curiosity_level = 0.0  # Would need agent access
        novelty_level = 0.0    # Would need agent access
        prediction_error = 0.0  # Would need agent access
        
        # Store data
        data['steps'].append(step)
        data['spike_counts'].append(spike_count)
        data['firing_rates'].append(firing_rate)
        data['active_neuron_counts'].append(active_count)
        data['excitement_inhibition_ratios'].append(ei_ratio)
        data['development_stages'].append(dev_stage)
        data['working_memory_usage'].append(wm_usage)
        data['episodic_memory_counts'].append(em_count)
        data['neuromodulation_levels'].append(neuromod_level)
        data['curiosity_levels'].append(curiosity_level)
        data['novelty_levels'].append(novelty_level)
        data['prediction_errors'].append(prediction_error)
        
        # Print progress
        if step % 100 == 0:
            print(f"{step:6d} {spike_count:10d} {firing_rate:10.2f} {active_count:8d} "
                  f"{ei_ratio:8.3f} {dev_stage:8d} {wm_usage:10d} {novelty_level:10.3f}")
    
    return data

def analyze_brain_data(data):
    """
    Perform comprehensive analysis on collected brain data.
    
    Args:
        data: Dictionary containing performance data
    
    Returns:
        Dictionary with analysis results
    """
    analysis = {}
    
    # Basic statistics
    analysis['total_steps'] = len(data['steps'])
    analysis['total_spikes'] = sum(data['spike_counts'])
    analysis['avg_firing_rate'] = np.mean(data['firing_rates']) if data['firing_rates'] else 0
    analysis['max_firing_rate'] = np.max(data['firing_rates']) if data['firing_rates'] else 0
    analysis['min_firing_rate'] = np.min(data['firing_rates']) if data['firing_rates'] else 0
    
    # Excitement/Inhibition ratio analysis
    analysis['avg_ei_ratio'] = np.mean(data['excitement_inhibition_ratios']) if data['excitement_inhibition_ratios'] else 0
    analysis['ei_ratio_std'] = np.std(data['excitement_inhibition_ratios']) if data['excitement_inhibition_ratios'] else 0
    analysis['ei_ratio_range'] = (analysis['min_firing_rate'], analysis['max_firing_rate'])
    
    # Development stage analysis
    stage_counts = defaultdict(int)
    for stage in data['development_stages']:
        stage_counts[stage] += 1
    analysis['development_stage_distribution'] = dict(stage_counts)
    
    # Memory usage analysis
    analysis['avg_working_memory'] = np.mean(data['working_memory_usage']) if data['working_memory_usage'] else 0
    analysis['max_working_memory'] = np.max(data['working_memory_usage']) if data['working_memory_usage'] else 0
    analysis['avg_episodic_memory'] = np.mean(data['episodic_memory_counts']) if data['episodic_memory_counts'] else 0
    
    # Neural activity patterns
    # Calculate firing rate variance (measure of neural stability)
    firing_variance = np.var(data['firing_rates']) if len(data['firing_rates']) > 1 else 0
    analysis['firing_rate_variance'] = firing_variance
    analysis['neural_stability'] = 1.0 / (1.0 + firing_variance)  # Higher = more stable
    
    # Determine if learning occurred (based on E/I ratio changes)
    if len(data['excitement_inhibition_ratios']) > 1:
        initial_ei = data['excitement_inhibition_ratios'][0]
        final_ei = data['excitement_inhibition_ratios'][-1]
        analysis['ei_ratio_change'] = final_ei - initial_ei
        analysis['learning_occurred'] = abs(analysis['ei_ratio_change']) > 0.1
    else:
        analysis['learning_occurred'] = False
    
    # Memory system health
    memory_health = []
    if analysis['avg_working_memory'] > 0:
        memory_health.append('Working memory: ACTIVE')
    if analysis['avg_episodic_memory'] > 0:
        memory_health.append('Episodic memory: ACTIVE')
    analysis['memory_system_health'] = memory_health if memory_health else ['No memory data available']
    
    return analysis

def create_visualization_data(data, analysis):
    """
    Prepare data for visualization.
    
    Args:
        data: Original performance data
        analysis: Analysis results
    
    Returns:
        Dictionary formatted for plotting libraries
    """
    vis_data = {
        'time_series': {
            'steps': data['steps'],
            'spike_counts': data['spike_counts'],
            'firing_rates': data['firing_rates'],
            'active_neurons': data['active_neuron_counts'],
            'excitement_inhibition_ratio': data['excitement_inhibition_ratios'],
            'working_memory_usage': data['working_memory_usage'],
            'episodic_memory_count': data['episodic_memory_counts']
        },
        'summary': {
            'total_steps': analysis['total_steps'],
            'total_spikes': analysis['total_spikes'],
            'avg_firing_rate': analysis['avg_firing_rate'],
            'avg_ei_ratio': analysis['avg_ei_ratio'],
            'development_stage_distribution': analysis['development_stage_distribution'],
            'memory_system_health': analysis['memory_system_health'],
            'neural_stability': analysis['neural_stability'],
            'learning_occurred': analysis['learning_occurred']
        }
    }
    
    return vis_data

def save_analysis_results(data, analysis, vis_data, output_file='brain_analysis.json'):
    """
    Save analysis results to JSON file.
    
    Args:
        data: Original performance data
        analysis: Analysis results
        vis_data: Visualization data
        output_file: Output filename
    """
    # Convert enum values to strings for JSON serialization
    stage_dist = {}
    for stage, count in analysis['development_stage_distribution'].items():
        stage_dist[f"Stage_{stage.value}"] = count
    analysis['development_stage_distribution'] = stage_dist
    
    # Create complete results
    results = {
        'metadata': {
            'collection_time': time.time(),
            'sampling_interval': 'step_interval',
            'brain_type': 'NLM Phase 6',
            'total_steps_collected': len(data['steps'])
        },
        'performance_data': data,
        'analysis': analysis,
        'visualization_data': vis_data
    }
    
    with open(output_file, 'w') as f:
        json.dump(results, f, indent=2, default=str)
    
    print(f"\nAnalysis results saved to: {output_file}")

def main():
    print("=== NLM Advanced Brain Monitoring Example ===")
    print("Setting up advanced brain monitoring and analysis...")
    
    # Step 1: Create brain
    print("\n1. Creating brain for monitoring...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"   ✓ Brain created with {brain.getTotalNeuronCount()} neurons")
    
    # Step 2: Collect performance data
    print("\n2. Collecting performance data...")
    data = collect_performance_data(brain, step_interval=50)
    
    # Step 3: Analyze data
    print("\n3. Analyzing performance data...")
    analysis = analyze_brain_data(data)
    
    # Print analysis summary
    print("\n4. Analysis Summary:")
    print(f"   Total steps monitored: {analysis['total_steps']}")
    print(f"   Total spikes generated: {analysis['total_spikes']}")
    print(f"   Average firing rate: {analysis['avg_firing_rate']:.2f} Hz")
    print(f"   Average E/I ratio: {analysis['avg_ei_ratio']:.3f}")
    print(f"   Neural stability: {analysis['neural_stability']:.3f}")
    print(f"   Learning occurred: {'YES' if analysis['learning_occurred'] else 'NO'}")
    print(f"   Memory system health: {analysis['memory_system_health']}")
    
    # Step 4: Create visualization data
    print("\n5. Preparing visualization data...")
    vis_data = create_visualization_data(data, analysis)
    
    # Step 5: Save results
    print("\n6. Saving analysis results...")
    save_analysis_results(data, analysis, vis_data)
    
    print("\n" + "="*50)
    print("✓ ADVANCED BRAIN MONITORING COMPLETED!")
    print("="*50)
    print(f"Results saved to: brain_analysis.json")
    print("\nKey findings:")
    print(f"  • Neural stability: {analysis['neural_stability']:.3f} ({'Stable' if analysis['neural_stability'] > 0.5 else 'Unstable'})")
    print(f"  • Learning: {'Detected' if analysis['learning_occurred'] else 'Not detected'}")
    print(f"  • Memory systems: {'Active' if analysis['memory_system_health'] else 'Inactive'}")
    
    return 0

if __name__ == "__main__":
    exit(main())