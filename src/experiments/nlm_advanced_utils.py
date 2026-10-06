#!/usr/bin/env python3
"""
Advanced NLM Python Utilities
=================================

Utilities to enhance the NLM Python experience for advanced users.
Provides additional functionality beyond the basic bindings.
"""

import numpy as np
import json
import sys
import os
from typing import List, Dict, Any, Optional

try:
    import pynlm
except ImportError:
    print("Error: pynlm not found. Please install it first.")
    print("pip install pynlm")
    sys.exit(1)

class NLMPerformanceProfiler:
    """Performance profiling utilities for NLM brain simulations."""
    
    def __init__(self, brain):
        self.brain = brain
        self.metrics = []
        self.step_count = 0
        
    def record_metrics(self):
        """Record current brain state metrics."""
        metrics = {
            'step': self.step_count,
            'neurons': self.brain.getTotalNeuronCount(),
            'synapses': self.brain.getTotalSynapseCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'avg_firing_rate': self.brain.getAverageFiringRate(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'exc_inh_ratio': self.brain.getExcitationInhibitionRatio(),
            'timestamp': pynlm.get_current_time() if hasattr(pynlm, 'get_current_time') else self.step_count * 0.001
        }
        self.metrics.append(metrics)
        self.step_count += 1
        return metrics
        
    def get_performance_stats(self):
        """Get performance statistics."""
        if not self.metrics:
            return None
            
        neurons = [m['neurons'] for m in self.metrics]
        spikes = [m['total_spikes'] for m in self.metrics]
        firing_rates = [m['avg_firing_rate'] for m in self.metrics]
        
        return {
            'neurons_per_step': np.mean(np.diff(neurons) / np.diff(np.arange(len(neurons)))),
            'spikes_per_step': np.mean(np.diff(spikes) / np.diff(np.arange(len(spikes)))),
            'avg_firing_rate': np.mean(firing_rates),
            'max_firing_rate': np.max(firing_rates),
            'final_spikes_per_neuron': spikes[-1] / neurons[-1],
            'growth_rate': (spikes[-1] - spikes[0]) / (neurons[-1] - neurons[0]) if neurons[-1] > neurons[0] else 0
        }
        
    def export_metrics(self, filename: str):
        """Export metrics to CSV file."""
        with open(filename, 'w') as f:
            f.write('step,neurons,synapses,active_neurons,firing_neurons,avg_firing_rate,total_spikes,exc_inh_ratio,timestamp\n')
            for metrics in self.metrics:
                f.write(f"{metrics['step']},{metrics['neurons']},{metrics['synapses']},"
                       f"{metrics['active_neurons']},{metrics['firing_neurons']},"
                       f"{metrics['avg_firing_rate']},{metrics['total_spikes']},"
                       f"{metrics['exc_inh_ratio']},{metrics['timestamp']}\n")
        print(f"Metrics exported to {filename}")
class NLMBrainOptimizer:
    """Brain optimization utilities for advanced users."""
    
    def __init__(self, brain):
        self.brain = brain
        
    def optimize_neural_connectivity(self, target_sparsity: float = 0.1):
        """
        Optimize neural connectivity for target sparsity.
        
        This is a high-level optimization interface for advanced users.
        """
        print(f"Optimizing neural connectivity for target sparsity: {target_sparsity}")
        
        # Get current connectivity stats
        total_synapses = self.brain.getTotalSynapseCount()
        current_sparsity = total_synapses / (self.brain.getTotalNeuronCount() ** 2)
        
        print(f"Current sparsity: {current_sparsity:.6f}")
        print(f"Target sparsity: {target_sparsity:.6f}")
        
        # Adjust structural plasticity settings for optimization
        config = self.brain.getConfig()
        
        # Store original values
        original_synaptogenesis = config.getOr("synaptogenesis_rate", 0.0001)
        original_pruning = config.getOr("pruning_rate", 0.00001)
        
        # Set optimization parameters
        if current_sparsity < target_sparsity:
            # Need more connections
            config.set("synaptogenesis_rate", original_synaptogenesis * 10, pynlm.ConfigSource.Default)
            print(f"Increasing synaptogenesis rate to {config.getOr('synaptogenesis_rate', 0.0001):.6f}")
        else:
            # Need fewer connections
            config.set("pruning_rate", original_pruning * 10, pynlm.ConfigSource.Default)
            print(f"Increasing pruning rate to {config.getOr('pruning_rate', 0.00001):.6f}")
        
        print("Optimization complete - adjust configuration as needed")
        return {
            'current_sparsity': current_sparsity,
            'target_sparsity': target_sparsity,
            'synaptogenesis_rate': config.getOr("synaptogenesis_rate", 0.0001),
            'pruning_rate': config.getOr("pruning_rate", 0.00001)
        }
        
    def simulate_development(self, target_stage = None, developmental_cycles: int = 10):
        """Simulate brain development to target stage."""
        print(f"Simulating development for {developmental_cycles} cycles...")
        
        config = self.brain.getConfig()
        
        for cycle in range(developmental_cycles):
            print(f"Development cycle {cycle + 1}/{developmental_cycles}")
            
            # Adjust development parameters
            if cycle < developmental_cycles / 3:
                # Early development: faster synaptogenesis
                config.set("synaptogenesis_rate", 0.001, pynlm.ConfigSource.Default)
                config.set("pruning_rate", 0.0, pynlm.ConfigSource.Default)
            elif cycle < 2 * developmental_cycles / 3:
                # Critical period: balanced plasticity
                config.set("synaptogenesis_rate", 0.0001, pynlm.ConfigSource.Default)
                config.set("pruning_rate", 0.00001, pynlm.ConfigSource.Default)
            else:
                # Adult: reduced plasticity
                config.set("synaptogenesis_rate", 0.00001, pynlm.ConfigSource.Default)
                config.set("pruning_rate", 0.000001, pynlm.ConfigSource.Default)
            
            # Progress to next developmental stage
            stage = self.brain.getDevelopmentalStage()
            if stage.value < 4:  # Not yet at Adult stage
                self.brain.setDevelopmentalStage(pynlm.DevelopmentalStage(stage.value + 1))
        
        print("Development simulation complete")
        return {
            'cycles_completed': developmental_cycles,
            'final_stage': self.brain.getDevelopmentalStage()
        }
class NLMExperimentManager:
    """Experiment management utilities for research workflows."""
    
    def __init__(self):
        self.experiments = {}
        self.results = []
        
    def create_experiment_config(self, **kwargs) -> pynlm.Config:
        """Create a standardized experiment configuration."""
        config = pynlm.createDefaultConfig()
        
        # Default experiment parameters
        defaults = {
            'neuron_count': 500,
            'max_steps': 1000,
            'random_seed': 42,
            'simulation_timestep': 0.001,
            'stdp_ltp_weight': 0.01,
            'stdp_ltd_weight': 0.012,
            'stdp_tau': 20.0,
            'connection_probability': 0.1,
            'region_count': 2,
            'enable_checkpointing': True,
            'enable_replay': True,
            'enable_development': True,
            'enable_curiosity': True,
            'enable_reward_modulation': True,
            'enable_structural_plasticity': True
        }
        
        # Override with user-provided parameters
        for key, value in defaults.items():
            if key in kwargs:
                value = kwargs[key]
            config.set(key, value, pynlm.ConfigSource.Default)
        
        return config
        
    def run_standard_experiment(self, brain, config: pynlm.Config, 
                               num_episodes: int = 10, max_steps_per_episode: int = 100) -> Dict[str, Any]:
        """Run a standard NLM experiment with metrics collection."""
        print(f"Running standard experiment: {num_episodes} episodes, {max_steps_per_episode} steps each")
        
        # This is a template - actual implementation would depend on experiment framework
        experiment_result = {
            'success': True,
            'episodes_completed': num_episodes,
            'total_steps': num_episodes * max_steps_per_episode,
            'brain_state': {
                'final_neuron_count': brain.getTotalNeuronCount(),
                'final_synapse_count': brain.getTotalSynapseCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'final_stage': brain.getDevelopmentalStage().value
            },
            'performance_metrics': {
                'spikes_per_neuron': brain.getTotalSpikeCount() / max(1, brain.getTotalNeuronCount()),
                'efficiency': brain.getAverageFiringRate() / max(1, brain.getTotalNeuronCount())
            }
        }
        
        self.results.append(experiment_result)
        return experiment_result
        
    def compare_experiments(self, experiment_ids: List[str]) -> Dict[str, Any]:
        """Compare results from multiple experiments."""
        print("Comparing experiments...")
        
        if len(self.results) < 2:
            return {'error': 'Not enough experiment results to compare'}
        
        comparison = {
            'experiment_count': len(self.results),
            'results_summary': []
        }
        
        for i, result in enumerate(self.results):
            comparison['results_summary'].append({
                'id': f'exp_{i}',
                'final_neuron_count': result['brain_state']['final_neuron_count'],
                'final_synapse_count': result['brain_state']['final_synapse_count'],
                'total_spikes': result['brain_state']['total_spikes'],
                'efficiency': result['performance_metrics']['efficiency']
            })
        
        return comparison
class NLMVisualizationHelper:
    """Visualization and output utilities."""
    
    @staticmethod
    def create_brain_visualization(brain, filename: str = "brain_state.txt"):
        """Create a text-based visualization of brain state."""
        with open(filename, 'w') as f:
            f.write("NLM Brain State Visualization\n")
            f.write("=" * 40 + "\n\n")
            
            f.write(f"Total Neurons: {brain.getTotalNeuronCount()}\n")
            f.write(f"Total Synapses: {brain.getTotalSynapseCount()}\n")
            f.write(f"Active Neurons: {brain.getActiveNeuronCount()}\n")
            f.write(f"Firing Neurons: {brain.getFiringNeuronCount()}\n")
            f.write(f"Average Firing Rate: {brain.getAverageFiringRate():.4f} Hz\n")
            f.write(f"Total Spikes: {brain.getTotalSpikeCount()}\n")
            f.write(f"Excitation/Inhibition Ratio: {brain.getExcitationInhibitionRatio():.4f}\n")
            f.write(f"Developmental Stage: {brain.getDevelopmentalStage().value}\n\n")
            
            f.write("Region Summary:\n")
            for region_id in brain.getRegionIds():
                region = brain.getRegion(region_id)
                if region:
                    f.write(f"  Region {region_id.value}: ")
                    f.write(f"{region.getAllNeurons().size()} neurons, ")
                    f.write(f"{region.getSynapses().size()} synapses\n")
        
        print(f"Brain visualization saved to {filename}")
        return filename
        
    @staticmethod
    def generate_json_report(brain, filename: str = "brain_report.json"):
        """Generate JSON report of brain state."""
        report = {
            'timestamp': pynlm.get_current_time() if hasattr(pynlm, 'get_current_time') else None,
            'brain_state': {
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'exc_inh_ratio': brain.getExcitationInhibitionRatio(),
                'developmental_stage': brain.getDevelopmentalStage().value
            },
            'regions': []
        }
        
        for region_id in brain.getRegionIds():
            region = brain.getRegion(region_id)
            if region:
                report['regions'].append({
                    'id': region_id.value,
                    'neuron_count': region.getAllNeurons().size(),
                    'synapse_count': region.getSynapses().size()
                })
        
        with open(filename, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"JSON report saved to {filename}")
        return filename
        
    @staticmethod
    def create_numpy_dataset(brain, filename: str = "brain_dataset.npz"):
        """Create numpy dataset for machine learning analysis."""
        # Create feature matrix from brain state
        features = {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'active_fraction': brain.getActiveNeuronCount() / max(1, brain.getTotalNeuronCount()),
            'firing_fraction': brain.getFiringNeuronCount() / max(1, brain.getTotalNeuronCount()),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'exc_inh_ratio': brain.getExcitationInhibitionRatio(),
            'developmental_stage': brain.getDevelopmentalStage().value
        }
        
        # Add region-specific features
        region_neuron_counts = []
        region_synapse_counts = []
        
        for region_id in brain.getRegionIds():
            region = brain.getRegion(region_id)
            if region:
                region_neuron_counts.append(region.getAllNeurons().size())
                region_synapse_counts.append(region.getSynapses().size())
        
        features['region_neuron_counts'] = np.array(region_neuron_counts)
        features['region_synapse_counts'] = np.array(region_synapse_counts)
        features['num_regions'] = len(region_neuron_counts)
        
        # Save as compressed numpy archive
        np.savez_compressed(filename, **features)
        print(f"Numpy dataset saved to {filename}")
        return filename
def main():
    """Main entry point for advanced utilities."""
    if len(sys.argv) < 2:
        print("NLM Advanced Utilities")
        print("======================")
        print()
        print("Available utilities:")
        print("  profiler        - Performance profiling tools")
        print("  optimizer       - Brain optimization tools")
        print("  experiment      - Experiment management")
        print("  visualize       - Visualization helpers")
        print("  dataset        - Create ML datasets")
        print()
        print("Usage: python nlm_utils.py <utility> [options]")
        return
    
    utility = sys.argv[1].lower()
    
    if utility == "profiler":
        if len(sys.argv) < 3:
            print("Profiler options:")
            print("  record         - Record current brain metrics")
            print("  stats          - Get performance statistics")
            print("  export <file>  - Export metrics to CSV")
        
    elif utility == "optimizer":
        if len(sys.argv) < 3:
            print("Optimizer options:")
            print("  optimize <sparsity>  - Optimize neural connectivity")
            print("  develop [cycles]     - Simulate development")
            
    elif utility == "experiment":
        if len(sys.argv) < 3:
            print("Experiment options:")
            print("  create [params]      - Create experiment config")
            print("  run [episodes] [steps] - Run experiment")
            print("  compare [ids]        - Compare experiments")
            
    elif utility == "visualize":
        if len(sys.argv) < 3:
            print("Visualization options:")
            print("  brain <file>      - Create brain visualization")
            print("  json <file>       - Generate JSON report")
            print("  numpy <file>      - Create numpy dataset")
    
    elif utility == "dataset":
        if len(sys.argv) < 3:
            print("Dataset options:")
            print("  create <file> - Create ML dataset from brain state")
    
    else:
        print(f"Unknown utility: {utility}")
        return

if __name__ == "__main__":
    main()
