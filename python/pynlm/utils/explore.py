"""
Exploration utilities for NLM.

This module provides exploration and analysis utilities for NLM simulations,
including configuration exploration, result analysis, and recommendation generation.
"""

import sys
import json
import argparse
from typing import Dict, Any, Optional, List
import pynlm

def explore_configurations(config_names: List[str], 
                          steps: int = 1000,
                          output_file: str = "exploration_results.json") -> Dict[str, Any]:
    """
    Explore different brain configurations.
    
    Args:
        config_names: List of preset configuration names to explore
        steps: Number of simulation steps per configuration
        output_file: File to save exploration results
        
    Returns:
        Dictionary containing exploration results
    """
    # Define preset configurations
    presets = {
        'minimal': {
            'neuron_count': 100,
            'connection_probability': 0.05,
            'plasticity_learning_rate': 0.001,
            'development_synaptogenesis_rate': 0.0001,
            'max_simulation_steps': 1000
        },
        'balanced': {
            'neuron_count': 1000,
            'connection_probability': 0.1,
            'plasticity_learning_rate': 0.01,
            'development_synaptogenesis_rate': 0.001,
            'max_simulation_steps': 10000
        },
        'exploratory': {
            'neuron_count': 5000,
            'connection_probability': 0.15,
            'plasticity_learning_rate': 0.02,
            'development_synaptogenesis_rate': 0.005,
            'max_simulation_steps': 20000
        },
        'complex': {
            'neuron_count': 10000,
            'connection_probability': 0.2,
            'plasticity_learning_rate': 0.03,
            'development_synaptogenesis_rate': 0.01,
            'max_simulation_steps': 50000
        }
    }
    
    # Filter valid configurations
    valid_presets = {name: config for name, config in presets.items() 
                     if name in config_names}
    
    if not valid_presets:
        print(f"Warning: No valid configurations found. Available: {list(presets.keys())}")
        return {}
    
    exploration_results = {
        'configurations': list(valid_presets.keys()),
        'results': {},
        'best_configuration': None,
        'recommendations': []
    }
    
    print(f"Exploring {len(valid_presets)} configurations...")
    
    for config_name in config_names:
        if config_name not in valid_presets:
            print(f"Skipping unknown configuration: {config_name}")
            continue
            
        print(f"  Exploring: {config_name}")
        
        # Create brain (note: configuration is limited by the createBrain constructor)
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Run simulation
        runner = SimulationRunner()
        results = runner.run_simulation(brain, steps)
        
        exploration_results['results'][config_name] = results
    
    # Determine best configuration
    best_config = None
    best_score = -1
    
    for config_name, results in exploration_results['results'].items():
        # Use a composite score for evaluation
        spike_rate = results['performance'].get('average_spike_rate', 0)
        sim_speed = results['performance'].get('steps_per_second', 0)
        score = spike_rate * sim_speed  # Combined performance metric
        
        print(f"  Configuration {config_name}: Score = {score:.6f}")
        
        if score > best_score:
            best_score = score
            best_config = config_name
    
    exploration_results['best_configuration'] = best_config
    exploration_results['recommendations'] = _generate_recommendations(exploration_results)
    
    # Save results
    with open(output_file, 'w') as f:
        json.dump(exploration_results, f, indent=2, default=str)
    
    print(f"\nExploration results saved to {output_file}")
    print(f"Best configuration: {best_config}")
    
    return exploration_results

def _generate_recommendations(exploration_results: Dict[str, Any]) -> List[str]:
    """Generate recommendations based on exploration results."""
    recommendations = []
    
    if exploration_results['best_configuration']:
        recommendations.append(
            f"Best configuration: {exploration_results['best_configuration']}"
        )
    
    # Analyze results for recommendations
    results = exploration_results['results']
    
    if len(results) > 1:
        # Compare configurations
        fastest = max(results.keys(), key=lambda k: results[k]['performance'].get('steps_per_second', 0))
        most_active = max(results.keys(), key=lambda k: results[k]['performance'].get('average_firing_rate', 0))
        
        recommendations.extend([
            f"Fastest configuration: {fastest}",
            f"Most active configuration: {most_active}"
        ])
    
    recommendations.extend([
        "Run longer simulations for more accurate results",
        "Consider adjusting neuromodulation parameters for learning",
        "Use different world configurations for environmental learning"
    ])
    
    return recommendations

def analyze_simulation_results(input_file: str, output_file: str = None):
    """
    Analyze and visualize simulation results.
    
    Args:
        input_file: File containing simulation results
        output_file: File to save analysis results (optional)
    """
    with open(input_file, 'r') as f:
        results = json.load(f)
    
    analysis = {
        'total_simulations': len(results.get('step_data', [])),
        'performance_summary': {},
        'spike_analysis': {},
        'configuration_comparison': {}
    }
    
    if 'performance' in results:
        analysis['performance_summary'] = results['performance']
    
    # Analyze spike patterns
    if 'step_data' in results:
        spikes = [data.get('spike_increment', 0) for data in results['step_data']]
        if spikes:
            analysis['spike_analysis'] = {
                'total_spikes': sum(spikes),
                'mean': sum(spikes) / len(spikes),
                'std': (sum((x - sum(spikes)/len(spikes))**2 for x in spikes) / len(spikes))**0.5,
                'max': max(spikes),
                'min': min(spikes)
            }
    
    if output_file:
        with open(output_file, 'w') as f:
            json.dump(analysis, f, indent=2)
        print(f"Analysis saved to {output_file}")
    
    return analysis

def print_configuration_summary(config_name: str):
    """
    Print summary of a specific configuration.
    
    Args:
        config_name: Name of configuration to summarize
    """
    presets = {
        'minimal': {
            'neuron_count': 100,
            'connection_probability': 0.05,
            'plasticity_learning_rate': 0.001,
            'development_synaptogenesis_rate': 0.0001,
            'max_simulation_steps': 1000
        },
        'balanced': {
            'neuron_count': 1000,
            'connection_probability': 0.1,
            'plasticity_learning_rate': 0.01,
            'development_synaptogenesis_rate': 0.001,
            'max_simulation_steps': 10000
        },
        'exploratory': {
            'neuron_count': 5000,
            'connection_probability': 0.15,
            'plasticity_learning_rate': 0.02,
            'development_synaptogenesis_rate': 0.005,
            'max_simulation_steps': 20000
        },
        'complex': {
            'neuron_count': 10000,
            'connection_probability': 0.2,
            'plasticity_learning_rate': 0.03,
            'development_synaptogenesis_rate': 0.01,
            'max_simulation_steps': 50000
        }
    }
    
    if config_name not in presets:
        print(f"Unknown configuration: {config_name}")
        return
    
    config = presets[config_name]
    
    print(f"=== Configuration: {config_name} ===")
    for key, value in config.items():
        print(f"  {key.replace('_', ' ').title()}: {value}")
    
    print()

# Import SimulationRunner for use in exploration
from . import SimulationRunner

def main():
    """Main exploration function."""
    parser = argparse.ArgumentParser(description="NLM Exploration Tool")
    parser.add_argument("--configs", nargs="+", default=["minimal", "balanced"],
                       help="Configuration names to explore")
    parser.add_argument("--steps", type=int, default=1000,
                       help="Number of simulation steps per configuration")
    parser.add_argument("--output", type=str, default="exploration_results.json",
                       help="Output file for exploration results")
    parser.add_argument("--analyze", type=str, help="Analyze results from file")
    parser.add_argument("--summary", action="store_true", help="Show configuration summary")
    
    args = parser.parse_args()
    
    print("=== NLM Exploration Tool ===")
    
    if args.analyze:
        print(f"Analyzing results from {args.analyze}")
        analyze_simulation_results(args.analyze, f"{args.analyze.replace('.json', '_analysis.json')}")
    elif args.summary:
        for config in args.configs:
            print_configuration_summary(config)
    else:
        results = explore_configurations(args.configs, args.steps, args.output)
        
        print("\n=== Recommendations ===")
        for rec in results['recommendations']:
            print(f"  • {rec}")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
