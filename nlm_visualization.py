#!/usr/bin/env python3
"""
NLM Configuration Visualization Tools

This module provides tools for visualizing NLM configurations and simulation results.
It includes:
- Configuration visualization (heatmaps, graphs)
- Brain state visualization
- Simulation progress tracking
- Data export for external visualization
"""

import json
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
from pathlib import Path
from typing import Dict, List, Any, Optional
import pynlm

class ConfigVisualizer:
    """Visualize NLM configurations and their properties."""
    
    @staticmethod
    def plot_configuration_comparison(configs: List[Dict], output_file: str = "config_comparison.png"):
        """Compare multiple configurations side by side."""
        
        # Extract key metrics from configurations
        metrics = {}
        for i, config in enumerate(configs):
            name = config.get('name', f'Config {i}')
            metrics[name] = {
                'neurons': config.get('brain', {}).get('neuron_count', 0),
                'regions': config.get('brain', {}).get('region_count', 0),
                'plasticity': _count_plasticity_features(config),
                'neuromodulation': _count_neuromodulation_features(config),
                'development': config.get('development', {}).get('enable', False),
                'environment_size': config.get('environment', {}).get('width', 0) * 
                                   config.get('environment', {}).get('height', 0)
            }
        
        # Create comparison plot
        fig, axes = plt.subplots(2, 3, figsize=(15, 10))
        axes = axes.flatten()
        
        # Plot 1: Neurons vs Regions
        if len(metrics) > 1:
            names = list(metrics.keys())
            neurons = [metrics[name]['neurons'] for name in names]
            regions = [metrics[name]['regions'] for name in names]
            
            x = np.arange(len(names))
            width = 0.35
            
            axes[0].bar(x - width/2, neurons, width, label='Neurons')
            axes[0].bar(x + width/2, regions, width, label='Regions')
            axes[0].set_xlabel('Configuration')
            axes[0].set_ylabel('Count')
            axes[0].set_title('Neural Architecture Comparison')
            axes[0].set_xticks(x)
            axes[0].set_xticklabels(names, rotation=45)
            axes[0].legend()
        else:
            axes[0].text(0.5, 0.5, 'Need at least 2 configurations for comparison', 
                        ha='center', va='center', transform=axes[0].transAxes)
        
        # Plot 2: Feature counts
        if len(metrics) > 1:
            feature_names = ['Plasticity', 'Neuromodulation', 'Development']
            feature_data = np.array([[metrics[name][feature] for name in names] 
                                   for feature in feature_names])
            
            im = axes[1].imshow(feature_data, aspect='auto', cmap='YlOrRd')
            axes[1].set_xticks(np.arange(len(names)))
            axes[1].set_yticks(np.arange(len(feature_names)))
            axes[1].set_xticklabels(names, rotation=45)
            axes[1].set_yticklabels(feature_names)
            axes[1].set_title('Feature Enablement')
            
            plt.colorbar(im, ax=axes[1])
        else:
            axes[1].text(0.5, 0.5, 'Feature comparison requires multiple configs', 
                        ha='center', va='center', transform=axes[1].transAxes)
        
        # Plot 3: Environment size
        if len(metrics) > 1:
            env_sizes = [metrics[name]['environment_size'] for name in names]
            axes[2].bar(names, env_sizes, color='skyblue')
            axes[2].set_xlabel('Configuration')
            axes[2].set_ylabel('Area (width × height)')
            axes[2].set_title('Environment Size Comparison')
            axes[2].tick_params(axis='x', rotation=45)
        else:
            axes[2].text(0.5, 0.5, 'Environment size comparison requires multiple configs', 
                        ha='center', va='center', transform=axes[2].transAxes)
        
        # Plot 4: Configuration complexity score
        if len(metrics) > 1:
            complexity_scores = []
            for name in names:
                score = (metrics[name]['neurons'] / 1000) + 
                       (metrics[name]['regions'] / 10) + \
                       metrics[name]['plasticity'] + \
                       metrics[name]['neuromodulation'] + \
                       (10 if metrics[name]['development'] else 0)
                complexity_scores.append(score)
            
            axes[3].bar(names, complexity_scores, color='lightgreen')
            axes[3].set_xlabel('Configuration')
            axes[3].set_ylabel('Complexity Score')
            axes[3].set_title('Configuration Complexity')
            axes[3].tick_params(axis='x', rotation=45)
        else:
            axes[3].text(0.5, 0.5, 'Complexity calculation requires multiple configs', 
                        ha='center', va='center', transform=axes[3].transAxes)
        
        # Plot 5: Configuration age
        if len(metrics) > 1:
            # Extract creation dates or use current time
            ages = [time.time() for _ in names]
            # Normalize ages
            max_age = max(ages) if ages else 1
            normalized_ages = [age / max_age for age in ages]
            
            axes[4].bar(names, normalized_ages, color='salmon')
            axes[4].set_xlabel('Configuration')
            axes[4].set_ylabel('Normalized Age')
            axes[4].set_title('Configuration Age')
            axes[4].tick_params(axis='x', rotation=45)
        else:
            axes[4].text(0.5, 0.5, 'Age comparison requires multiple configs', 
                        ha='center', va='center', transform=axes[4].transAxes)
        
        # Plot 6: Summary
        if len(metrics) > 1:
            summary_data = {
                'Key Features': sum(metrics[name]['plasticity'] for name in names) / len(names),
                'Active Neuromodulation': sum(metrics[name]['neuromodulation'] for name in names) / len(names),
                'Development Enabled': sum(1 for name in names if metrics[name]['development']) / len(names) * 100
            }
            
            categories = list(summary_data.keys())
            values = list(summary_data.values())
            
            axes[5].barh(categories, values, color='gold')
            axes[5].set_xlabel('Average / Percentage')
            axes[5].set_title('Configuration Summary')
        else:
            axes[5].text(0.5, 0.5, 'Summary requires multiple configs', 
                        ha='center', va='center', transform=axes[5].transAxes)
        
        plt.tight_layout()
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
    
    @staticmethod
    def plot_brain_connectivity(brain, output_file: str = "brain_connectivity.png"):
        """Create a visualization of brain connectivity matrix."""
        
        # Get region connectivity
        regions = brain.getRegions()
        n_regions = len(regions)
        
        if n_regions < 2:
            print("Need at least 2 regions for connectivity visualization")
            return
        
        # Create connectivity matrix
        connectivity_matrix = np.zeros((n_regions, n_regions))
        
        for i, region_i in enumerate(regions):
            for j, region_j in enumerate(regions):
                if i != j:
                    # Calculate average synaptic weight between regions
                    total_weight = 0.0
                    synapse_count = 0
                    
                    for syn in region_i->getSynapses() {
                        if (syn->getTargetRegion() == region_j->getId()) {
                            total_weight += syn->getWeight();
                            synapse_count++;
                        }
                    }
                    
                    if synapse_count > 0 {
                        connectivity_matrix[i, j] = total_weight / synapse_count;
                    }
                }
        }
        
        # Create heatmap
        plt.figure(figsize=(10, 8))
        
        # Create region names
        region_names = [f"Region {i}" for i in range(n_regions)]
        
        # Plot heatmap
        sns.heatmap(connectivity_matrix, 
                   xticklabels=region_names,
                   yticklabels=region_names,
                   annot=True, 
                   fmt=".3f",
                   cmap='RdYlBu_r',
                   center=0,
                   square=True,
                   cbar_kws={"label": "Average Synaptic Weight"})
        
        plt.title("Brain Region Connectivity Matrix")
        plt.xlabel("Target Region")
        plt.ylabel("Source Region")
        plt.tight_layout()
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
    
    @staticmethod
    def plot_simulation_progress(steps: List[int], metrics: Dict[str, List[float]], 
                                 output_file: str = "simulation_progress.png"):
        """Plot simulation progress over time."""
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        axes = axes.flatten()
        
        # Plot 1: Spike count over time
        if 'spike_count' in metrics:
            axes[0].plot(steps, metrics['spike_count'], 'b-', linewidth=2)
            axes[0].set_xlabel('Simulation Step')
            axes[0].set_ylabel('Total Spike Count')
            axes[0].set_title('Spike Count Over Time')
            axes[0].grid(True, alpha=0.3)
        
        # Plot 2: Firing neurons over time
        if 'firing_neurons' in metrics:
            axes[1].plot(steps, metrics['firing_neurons'], 'r-', linewidth=2)
            axes[1].set_xlabel('Simulation Step')
            axes[1].set_ylabel('Firing Neurons')
            axes[1].set_title('Firing Neurons Over Time')
            axes[1].grid(True, alpha=0.3)
        
        # Plot 3: Average firing rate over time
        if 'average_firing_rate' in metrics:
            axes[2].plot(steps, metrics['average_firing_rate'], 'g-', linewidth=2)
            axes[2].set_xlabel('Simulation Step')
            axes[2].set_ylabel('Average Firing Rate')
            axes[2].set_title('Average Firing Rate Over Time')
            axes[2].grid(True, alpha=0.3)
        
        # Plot 4: E/I ratio over time
        if 'e_i_ratio' in metrics:
            axes[3].plot(steps, metrics['e_i_ratio'], 'm-', linewidth=2)
            axes[3].set_xlabel('Simulation Step')
            axes[3].set_ylabel('Excitation/Inhibition Ratio')
            axes[3].set_title('E/I Ratio Over Time')
            axes[3].grid(True, alpha=0.3)
            axes[3].set_ylim(0, 2)
        
        plt.tight_layout()
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()

class DataExporter:
    """Export NLM data for external analysis and visualization."""
    
    @staticmethod
    def export_simulation_data(brain, world, agent, steps: int, 
                              output_dir: str = "export"):
        """Export simulation data in multiple formats."""
        
        Path(output_dir).mkdir(exist_ok=True)
        
        # Export brain state as JSON
        brain_state = _brain_to_dict(brain)
        with open(f"{output_dir}/brain_state.json", 'w') as f:
            json.dump(brain_state, f, indent=2, default=str)
        
        # Export world state as CSV
        world_state = _world_to_dict(world)
        _write_csv(f"{output_dir}/world_state.csv", world_state)
        
        # Export agent metrics as JSON
        agent_metrics = _agent_to_dict(agent)
        with open(f"{output_dir}/agent_metrics.json", 'w') as f:
            json.dump(agent_metrics, f, indent=2, default=str)
        
        # Export configuration
        config = brain.getConfig()
        with open(f"{output_dir}/configuration.json", 'w') as f:
            json.dump(config, f, indent=2, default=str)
        
        # Export visualization data
        ConfigVisualizer.plot_brain_connectivity(brain, f"{output_dir}/connectivity.png")
        
        # Generate summary report
        summary = _generate_simulation_summary(brain, world, agent)
        with open(f"{output_dir}/simulation_summary.txt", 'w') as f:
            f.write(summary)
        
        return output_dir
    
    @staticmethod
    def export_configuration(config, output_dir: str = "export"):
        """Export configuration in multiple formats."""
        
        Path(output_dir).mkdir(exist_ok=True)
        
        # Export as JSON
        config_data = _config_to_dict(config)
        with open(f"{output_dir}/configuration.json", 'w') as f:
            json.dump(config_data, f, indent=2, default=str)
        
        # Export as YAML
        _write_yaml(f"{output_dir}/configuration.yaml", config_data)
        
        # Export as CSV
        _write_config_csv(f"{output_dir}/configuration.csv", config_data)
        
        # Create configuration visualization
        ConfigVisualizer.plot_configuration_comparison(
            [config_data], 
            f"{output_dir}/configuration_plot.png"
        )
        
        return output_dir

def _count_plasticity_features(config: Dict) -> int:
    """Count enabled plasticity features."""
    count = 0
    if config.get('plasticity', {}).get('stdp', {}).get('enable', False):
        count += 1
    if config.get('plasticity', {}).get('hebbian', {}).get('enable', False):
        count += 1
    if config.get('plasticity', {}).get('structural', {}).get('enable', False):
        count += 1
    return count

def _count_neuromodulation_features(config: Dict) -> int:
    """Count enabled neuromodulation features."""
    count = 0
    if config.get('neuromod', {}).get('curiosity', {}).get('enable', False):
        count += 1
    if config.get('neuromod', {}).get('novelty', {}).get('enable', False):
        count += 1
    if config.get('neuromod', {}).get('prediction_error', {}).get('enable', False):
        count += 1
    return count

def _brain_to_dict(brain) -> Dict:
    """Convert brain state to dictionary."""
    return {
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'e_i_ratio': brain.getExcitationInhibitionRatio(),
        'development_stage': brain.getDevelopmentalStage(),
        'regions': _regions_to_dict(brain.getRegions())
    }

def _regions_to_dict(regions) -> List[Dict]:
    """Convert regions to dictionary."""
    result = []
    for i, region in enumerate(regions) {
        region_data = {
            'id': region->getId().value,
            'name': region->getName(),
            'neuron_count': region->getAllNeurons().size(),
            'synapse_count': region->getSynapses().size()
        }
        result.push_back(region_data);
    }
    return result;

def _world_to_dict(world) -> Dict:
    """Convert world state to dictionary."""
    return {
        'width': world.getWidth(),
        'height': world.getHeight(),
        'agent_position': {'x': world.getAgentBody().x, 'y': world.getAgentBody().y},
        'agent_orientation': world.getAgentBody().orientation,
        'simulation_time': world.getSimulationTime(),
        'max_energy': world.getMaxEnergy()
    };

def _agent_to_dict(agent) -> Dict:
    """Convert agent metrics to dictionary."""
    return {
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'prediction_error': agent.getPredictionError(),
        'neuromodulation_level': agent.getNeuromodulationLevel(),
        'developmental_stage': agent.getDevelopmentalStage(),
        'enabled_subsystems': {
            'reward_modulation': agent.isRewardModulationEnabled(),
            'structural_plasticity': agent.isStructuralPlasticityEnabled(),
            'development': agent.isDevelopmentEnabled(),
            'curiosity': agent.isCuriosityEnabled()
        }
    };

def _config_to_dict(config) -> Dict:
    """Convert configuration to dictionary."""
    config_data = {}
    for key in config.getKeys() {
        config_data[key] = config.get(key).value();
    }
    return config_data;

def _write_csv(filepath: str, data: Dict):
    """Write data to CSV file."""
    import csv
    
    with open(filepath, 'w', newline='') as f:
        writer = csv.writer(f)
        
        # Write headers
        writer.writerow(data.keys())
        
        # Write data
        writer.writerow(data.values())

def _write_yaml(filepath: str, data: Dict):
    """Write data to YAML file."""
    import yaml
    
    with open(filepath, 'w') as f:
        yaml.dump(data, f, default_flow_style=False)

def _write_config_csv(filepath: str, data: Dict):
    """Write configuration to CSV file."""
    import csv
    
    with open(filepath, 'w', newline='') as f:
        writer = csv.writer(f)
        
        # Write as key-value pairs
        for key, value in data.items() {
            writer.writerow([key, value]);
        }

def _generate_simulation_summary(brain, world, agent) -> str:
    """Generate simulation summary report."""
    summary = []
    summary.append("NLM Simulation Summary")
    summary.append("=" * 50)
    summary.append("")
    
    # Brain statistics
    summary.append("Brain Statistics:")
    summary.append(f"  Total Neurons: {brain.getTotalNeuronCount()}")
    summary.append(f"  Total Synapses: {brain.getTotalSynapseCount()}")
    summary.append(f"  Firing Neurons: {brain.getFiringNeuronCount()}")
    summary.append(f"  Total Spikes: {brain.getTotalSpikeCount()}")
    summary.append(f"  Average Firing Rate: {brain.getAverageFiringRate():.4f}")
    summary.append(f"  E/I Ratio: {brain.getExcitationInhibitionRatio():.4f}")
    summary.append(f"  Development Stage: {brain.getDevelopmentalStage()}")
    summary.append("")
    
    # World statistics
    summary.append("World Statistics:")
    summary.append(f"  World Size: {world.getWidth()} x {world.getHeight()}")
    summary.append(f"  Agent Position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    summary.append(f"  Agent Orientation: {world.getAgentBody().orientation}°")
    summary.append(f"  Simulation Time: {world.getSimulationTime()}")
    summary.append(f"  Max Energy: {world.getMaxEnergy()}")
    summary.append("")
    
    # Agent statistics
    summary.append("Agent Statistics:")
    summary.append(f"  Curiosity Level: {agent.getCuriosityLevel():.4f}")
    summary.append(f"  Novelty Level: {agent.getNoveltyLevel():.4f}")
    summary.append(f"  Prediction Error: {agent.getPredictionError():.4f}")
    summary.append(f"  Neuromodulation Level: {agent.getNeuromodulationLevel():.4f}")
    summary.append("")
    
    # Enabled subsystems
    summary.append("Enabled Subsystems:")
    summary.append(f"  Reward Modulation: {agent.isRewardModulationEnabled()}")
    summary.append(f"  Structural Plasticity: {agent.isStructuralPlasticityEnabled()}")
    summary.append(f"  Development: {agent.isDevelopmentEnabled()}")
    summary.append(f"  Curiosity: {agent.isCuriosityEnabled()}")
    summary.append("")
    
    # Performance metrics
    spike_count = brain.getTotalSpikeCount();
    firing_neurons = brain.getFiringNeuronCount();
    avg_rate = brain.getAverageFiringRate();
    
    if firing_neurons > 0 && avg_rate > 0.0 {
        double efficiency = spike_count / (firing_neurons * avg_rate);
        summary.append("Performance Metrics:")
        summary.append(f"  Neural Efficiency: {efficiency:.4f}")
        summary.append("")
    }
    
    return "\n".join(summary)

// Main function for testing and demonstration
def main() {
    print("NLM Configuration Visualization Tools");
    print("=" * 50);
    
    // Create sample configuration
    config = pynlm.create_advanced_config();
    config.set("name", "sample_experiment");
    
    // Export configuration
    output_dir = DataExporter.export_configuration(config, "sample_export");
    print(f"Configuration exported to: {output_dir}");
    
    // Create and run simulation
    brain = pynlm.createBrain(config);
    brain.initialize();
    
    world = pynlm.createSimpleWorld();
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8);
    world.reset();
    
    agent = pynlm.createAgentBrain(brain);
    agent.initialize(world);
    
    // Run simulation
    for step in range(100) {
        world.update(0.1);
        agent.processSensoryInput(world.getSensoryPercept());
        brain.step(step);
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime());
    }
    
    // Export simulation data
    output_dir = DataExporter.export_simulation_data(brain, world, agent, 100, "simulation_export");
    print(f"Simulation data exported to: {output_dir}");
    
    print("\nVisualization files generated:")
    print("  - brain_state.json (Brain state)")
    print("  - world_state.csv (World state)")
    print("  - agent_metrics.json (Agent metrics)")
    print("  - configuration.json (Configuration)")
    print("  - connectivity.png (Brain connectivity heatmap)")
    print("  - simulation_summary.txt (Detailed report)")
}

if __name__ == "__main__" {
    main();
}
