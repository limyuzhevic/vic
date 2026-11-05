#!/usr/bin/env python3
"""
NLM Visualization CLI - Generate basic visualizations of brain activity.

This tool provides command-line access to visualization and analysis
of NLM brain activity, including data export and analysis.
"""

import argparse
import json
import logging
import sys
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple
import matplotlib
matplotlib.use('Agg')  # Use non-interactive backend
import matplotlib.pyplot as plt
import numpy as np

# Add parent directory to path for pynlm import
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    import pynlm
except ImportError as e:
    print(f"ERROR: Could not import pynlm: {e}")
    sys.exit(1)


def create_brain_activity_plot(args: argparse.Namespace) -> None:
    """Create a visualization of brain activity metrics."""
    logger = logging.getLogger("visualization_cli")
    
    # Parse visualization parameters
    steps = int(args.steps) if args.steps else 100
    neurons = int(args.neurons) if args.neurons else 100
    plot_type = args.plot_type or 'activity'
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    logger.info(f"Creating {plot_type} visualization with {steps} steps and {neurons} neurons")
    
    # Collect data
    time_series = []
    spike_counts = []
    firing_rates = []
    neuron_activity = []
    
    for step in range(steps):
        brain.step(step)
        
        # Collect metrics
        time_series.append(step)
        spike_counts.append(brain.getTotalSpikeCount())
        firing_rates.append(brain.getAverageFiringRate())
        neuron_activity.append(brain.getFiringNeuronCount())
    
    # Create visualization
    if plot_type == 'activity':
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Spike count over time
        axes[0, 0].plot(time_series, spike_counts, 'b-', linewidth=2)
        axes[0, 0].set_title('Total Spike Count Over Time')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Spike Count')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Firing rate over time
        axes[0, 1].plot(time_series, firing_rates, 'r-', linewidth=2)
        axes[0, 1].set_title('Average Firing Rate Over Time')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Firing Rate (Hz)')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Active neurons over time
        axes[1, 0].plot(time_series, neuron_activity, 'g-', linewidth=2)
        axes[1, 0].set_title('Active Neurons Over Time')
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('Active Neurons')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Correlation between active neurons and firing rate
        if len(neuron_activity) > 1:
            axes[1, 1].scatter(neuron_activity, firing_rates, alpha=0.6, s=10)
            axes[1, 1].set_title('Active Neurons vs Firing Rate')
            axes[1, 1].set_xlabel('Active Neurons')
            axes[1, 1].set_ylabel('Firing Rate (Hz)')
            axes[1, 1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        # Save or display plot
        output_file = args.output_file or 'brain_activity_plot.png'
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
        
        plot_results = {
            "success": True,
            "message": f"Brain activity visualization saved to {output_file}",
            "file_path": output_file,
            "plot_type": plot_type,
            "parameters": {
                "steps": steps,
                "neurons": neurons
            },
            "final_metrics": {
                "total_spikes": spike_counts[-1] if spike_counts else 0,
                "final_firing_rate": firing_rates[-1] if firing_rates else 0,
                "final_active_neurons": neuron_activity[-1] if neuron_activity else 0
            }
        }
        
    elif plot_type == 'summary':
        # Create summary statistics plot
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))
        
        # Bar chart of final statistics
        final_metrics = {
            'Total Neurons': brain.getTotalNeuronCount(),
            'Total Synapses': brain.getTotalSynapseCount(),
            'Active Neurons': brain.getActiveNeuronCount(),
            'Firing Neurons': brain.getFiringNeuronCount(),
            'Total Spikes': brain.getTotalSpikeCount(),
            'Average Firing Rate': brain.getAverageFiringRate()
        }
        
        bars = ax1.bar(final_metrics.keys(), final_metrics.values(), color='skyblue', alpha=0.7)
        ax1.set_title('Final Brain Statistics')
        ax1.set_ylabel('Count / Rate')
        ax1.tick_params(axis='x', rotation=45)
        
        # Add value labels on bars
        for bar, value in zip(bars, final_metrics.values()):
            height = bar.get_height()
            ax1.text(bar.get_x() + bar.get_width()/2., height,
                    f'{value:.1f}', ha='center', va='bottom')
        
        # Distribution of neuron types (simulated based on final counts)
        neuron_types = ['Excitatory', 'Inhibitory', 'Sensory', 'Motor', 'Internal']
        type_counts = [
            int(brain.getTotalNeuronCount() * 0.7),  # 70% excitatory
            int(brain.getTotalNeuronCount() * 0.2),  # 20% inhibitory
            int(brain.getTotalNeuronCount() * 0.05),  # 5% sensory
            int(brain.getTotalNeuronCount() * 0.03),  # 3% motor
            int(brain.getTotalNeuronCount() * 0.02)   # 2% internal
        ]
        
        colors = ['red', 'blue', 'green', 'orange', 'purple']
        bars = ax2.bar(neuron_types, type_counts, color=colors, alpha=0.7)
        ax2.set_title('Neuron Type Distribution (Simulated)')
        ax2.set_ylabel('Count')
        
        # Add value labels on bars
        for bar, value in zip(bars, type_counts):
            height = bar.get_height()
            ax2.text(bar.get_x() + bar.get_width()/2., height,
                    f'{value}', ha='center', va='bottom')
        
        plt.tight_layout()
        
        # Save plot
        output_file = args.output_file or 'brain_summary_plot.png'
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
        
        plot_results = {
            "success": True,
            "message": f"Brain summary visualization saved to {output_file}",
            "file_path": output_file,
            "plot_type": plot_type,
            "parameters": {
                "steps": steps,
                "neurons": neurons
            },
            "final_metrics": {
                "total_neuron_count": brain.getTotalNeuronCount(),
                "total_synapse_count": brain.getTotalSynapseCount(),
                "active_neuron_count": brain.getActiveNeuronCount(),
                "firing_neuron_count": brain.getFiringNeuronCount(),
                "total_spike_count": brain.getTotalSpikeCount(),
                "average_firing_rate": brain.getAverageFiringRate(),
                "excitatory_inhibitory_ratio": brain.getExcitationInhibitionRatio(),
                "developmental_stage": str(brain.getDevelopmentalStage())
            }
        }
    
    else:
        logger.error(f"Unknown plot type: {plot_type}")
        sys.exit(1)
    
    # Output results
    if args.json:
        json.dump(plot_results, sys.stdout, indent=2)
        sys.stdout.write('\n')
    else:
        if plot_results["success"]:
            print(f"Visualization completed successfully!")
            print(f"  - Plot type: {plot_results['plot_type']}")
            print(f"  - Output file: {plot_results['file_path']}")
            print(f"  - Final metrics: {json.dumps(plot_results['final_metrics'], indent=4)}")
        else:
            print(f"Visualization failed: {plot_results.get('message', 'Unknown error')}")
            sys.exit(1)


def export_brain_data(args: argparse.Namespace) -> None:
    """Export brain data for external analysis."""
    logger = logging.getLogger("visualization_cli")
    
    # Parse export parameters
    steps = int(args.steps) if args.steps else 100
    output_format = args.format or 'json'
    output_file = args.output_file or 'brain_data.export'
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    logger.info(f"Exporting brain data: {steps} steps, format: {output_format}, output: {output_file}")
    
    # Collect time-series data
    time_series_data = []
    
    for step in range(steps):
        brain.step(step)
        
        step_data = {
            "step": step,
            "neuron_count": brain.getTotalNeuronCount(),
            "synapse_count": brain.getTotalSynapseCount(),
            "active_neuron_count": brain.getActiveNeuronCount(),
            "firing_neuron_count": brain.getFiringNeuronCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "total_spike_count": brain.getTotalSpikeCount(),
            "excitatory_inhibitory_ratio": brain.getExcitationInhibitionRatio(),
            "developmental_stage": str(brain.getDevelopmentalStage()),
            "learned_preference": brain.getLearnedPreference()
        }
        
        time_series_data.append(step_data)
    
    # Export based on format
    export_results = {
        "success": True,
        "message": f"Brain data exported successfully to {output_file}",
        "file_path": output_file,
        "export_format": output_format,
        "parameters": {
            "steps": steps,
            "brain_type": "NLM"
        },
        "final_summary": {
            "total_neuron_count": brain.getTotalNeuronCount(),
            "total_synapse_count": brain.getTotalSynapseCount(),
            "total_spike_count": brain.getTotalSpikeCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "excitatory_inhibitory_ratio": brain.getExcitationInhibitionRatio(),
            "developmental_stage": str(brain.getDevelopmentalStage()),
            "learned_preference": brain.getLearnedPreference()
        }
    }
    
    # Write to file based on format
    try:
        if output_format == 'json':
            export_data = {
                "metadata": export_results,
                "time_series": time_series_data
            }
            with open(output_file, 'w') as f:
                json.dump(export_data, f, indent=2)
        elif output_format == 'csv':
            import csv
            with open(output_file, 'w', newline='') as f:
                fieldnames = ['step', 'neuron_count', 'synapse_count', 'active_neuron_count',
                             'firing_neuron_count', 'average_firing_rate', 'total_spike_count',
                             'excitatory_inhibitory_ratio', 'developmental_stage', 'learned_preference']
                writer = csv.DictWriter(f, fieldnames=fieldnames)
                writer.writeheader()
                writer.writerows(time_series_data)
        elif output_format == 'tsv':
            import csv
            with open(output_file, 'w', newline='') as f:
                fieldnames = ['step', 'neuron_count', 'synapse_count', 'active_neuron_count',
                             'firing_neuron_count', 'average_firing_rate', 'total_spike_count',
                             'excitatory_inhibitory_ratio', 'developmental_stage', 'learned_preference']
                writer = csv.DictWriter(f, fieldnames=fieldnames, delimiter='\t')
                writer.writeheader()
                writer.writerows(time_series_data)
        else:
            # Default to JSON for unknown formats
            export_data = {
                "metadata": export_results,
                "time_series": time_series_data
            }
            with open(output_file, 'w') as f:
                json.dump(export_data, f, indent=2)
        
        logger.info(f"Data successfully written to {output_file}")
        
    except Exception as e:
        error_msg = f"Failed to write data to {output_file}: {str(e)}"
        logger.error(error_msg)
        
        if args.json:
            json.dump({"success": False, "error": error_msg}, sys.stdout, indent=2)
        else:
            print(f"ERROR: {error_msg}")
        sys.exit(1)
    
    # Output results
    if args.json:
        json.dump(export_results, sys.stdout, indent=2)
        sys.stdout.write('\n')
    else:
        print(f"Data export completed successfully!")
        print(f"  - Format: {output_format}")
        print(f"  - Output file: {output_file}")
        print(f"  - Data points: {len(time_series_data)}")
        print(f"  - Final metrics: {json.dumps(export_results['final_summary'], indent=4)}")


def analyze_activity_patterns(args: argparse.Namespace) -> None:
    """Analyze brain activity patterns and provide insights."""
    logger = logging.getLogger("visualization_cli")
    
    # Parse analysis parameters
    steps = int(args.steps) if args.steps else 200
    analysis_type = args.analysis_type or 'patterns'
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    logger.info(f"Analyzing brain activity patterns: {steps} steps, analysis type: {analysis_type}")
    
    # Collect data for analysis
    metrics = {
        'step': [],
        'neuron_count': [],
        'synapse_count': [],
        'active_neuron_count': [],
        'firing_neuron_count': [],
        'average_firing_rate': [],
        'total_spike_count': [],
        'excitatory_inhibitory_ratio': [],
        'developmental_stage': []
    }
    
    for step in range(steps):
        brain.step(step)
        
        metrics['step'].append(step)
        metrics['neuron_count'].append(brain.getTotalNeuronCount())
        metrics['synapse_count'].append(brain.getTotalSynapseCount())
        metrics['active_neuron_count'].append(brain.getActiveNeuronCount())
        metrics['firing_neuron_count'].append(brain.getFiringNeuronCount())
        metrics['average_firing_rate'].append(brain.getAverageFiringRate())
        metrics['total_spike_count'].append(brain.getTotalSpikeCount())
        metrics['excitatory_inhibitory_ratio'].append(brain.getExcitationInhibitionRatio())
        metrics['developmental_stage'].append(str(brain.getDevelopmentalStage()))
    
    # Perform analysis based on type
    analysis_results = {
        "analysis_type": analysis_type,
        "parameters": {
            "steps": steps
        },
        "brain_info": {
            "final_neuron_count": brain.getTotalNeuronCount(),
            "final_synapse_count": brain.getTotalSynapseCount(),
            "final_development_stage": str(brain.getDevelopmentalStage()),
            "final_learned_preference": brain.getLearnedPreference()
        }
    }
    
    if analysis_type == 'patterns':
        # Analyze activity patterns
        activity_variation = np.var(metrics['average_firing_rate'])
        spike_accumulation = metrics['total_spike_count'][-1] if metrics['total_spike_count'] else 0
        
        analysis_results["pattern_analysis"] = {
            "activity_stability": {
                "coefficient_of_variation": activity_variation**0.5 / (np.mean(metrics['average_firing_rate']) if np.mean(metrics['average_firing_rate']) != 0 else 1),
                "mean_firing_rate": np.mean(metrics['average_firing_rate']),
                "firing_rate_stddev": np.std(metrics['average_firing_rate'])
            },
            "spike_dynamics": {
                "total_spikes": spike_accumulation,
                "spike_growth_rate": (metrics['total_spike_count'][-1] - metrics['total_spike_count'][0]) / max(steps, 1),
                "plateau_reached": False  # Simplified detection
            },
            "neuron_dynamics": {
                "neuron_count_stable": abs(metrics['neuron_count'][-1] - metrics['neuron_count'][0]) < 0.1 * metrics['neuron_count'][0],
                "synapse_count_stable": abs(metrics['synapse_count'][-1] - metrics['synapse_count'][0]) < 0.1 * metrics['synapse_count'][0]
            }
        }
        
    elif analysis_type == 'temporal':
        # Analyze temporal patterns
        # Calculate autocorrelation of firing rate
        firing_rates = np.array(metrics['average_firing_rate'])
        if len(firing_rates) > 10:
            # Simple autocorrelation at lag 1
            autocorr = np.corrcoef(firing_rates[:-1], firing_rates[1:])[0, 1] if len(firing_rates) > 1 else 0
        else:
            autocorr = 0
        
        # Detect cycles (simplified: look for peaks)
        from scipy.signal import find_peaks
        try:
            peaks, _ = find_peaks(firing_rates, height=np.mean(firing_rates))
            peak_intervals = np.diff(peaks) if len(peaks) > 1 else []
            avg_cycle_length = np.mean(peak_intervals) if peak_intervals.size > 0 else 0
        except:
            peaks = []
            avg_cycle_length = 0
        
        analysis_results["temporal_analysis"] = {
            "autocorrelation": autocorr,
            "peak_detection": {
                "number_of_peaks": len(peaks),
                "average_cycle_length": avg_cycle_length,
                "has_periodic_activity": len(peaks) > 2 and avg_cycle_length > 0
            },
            "trend_analysis": {
                "firing_rate_trend": "increasing" if metrics['average_firing_rate'][-1] > metrics['average_firing_rate'][0] else "decreasing",
                "activity_volatility": np.std(firing_rates)
            }
        }
    
    elif analysis_type == 'developmental':
        # Analyze developmental stages
        stage_counts = {}
        for stage in metrics['developmental_stage']:
            stage_counts[stage] = stage_counts.get(stage, 0) + 1
        
        analysis_results["developmental_analysis"] = {
            "stage_distribution": stage_counts,
            "stage_transitions": list(set(metrics['developmental_stage'])),
            "development_progression": {
                "final_stage": metrics['developmental_stage'][-1],
                "total_stages_visited": len(set(metrics['developmental_stage'])),
                "stage_change_frequency": len(set(metrics['developmental_stage'])) / max(steps, 1)
            }
        }
    
    else:
        logger.error(f"Unknown analysis type: {analysis_type}")
        sys.exit(1)
    
    # Output analysis results
    if args.json:
        json.dump(analysis_results, sys.stdout, indent=2)
        sys.stdout.write('\n')
    else:
        print(f"\n=== Brain Activity Analysis Results ===")
        print(f"Analysis Type: {analysis_results['analysis_type']}")
        print(f"Final Brain Info:")
        for key, value in analysis_results['brain_info'].items():
            print(f"  {key}: {value}")
        
        if 'pattern_analysis' in analysis_results:
            print(f"\nPattern Analysis:")
            pattern = analysis_results['pattern_analysis']
            print(f"  Activity Stability:")
            print(f"    Coefficient of Variation: {pattern['activity_stability']['coefficient_of_variation']:.3f}")
            print(f"    Mean Firing Rate: {pattern['activity_stability']['mean_firing_rate']:.4f}")
            print(f"  Spike Dynamics:")
            print(f"    Total Spikes: {pattern['spike_dynamics']['total_spikes']}")
            print(f"    Growth Rate: {pattern['spike_dynamics']['spike_growth_rate']:.2f}")
        
        if 'temporal_analysis' in analysis_results:
            print(f"\nTemporal Analysis:")
            temporal = analysis_results['temporal_analysis']
            print(f"  Autocorrelation: {temporal['autocorrelation']:.3f}")
            print(f"  Number of Peaks: {temporal['peak_detection']['number_of_peaks']}")
            print(f"  Average Cycle Length: {temporal['peak_detection']['average_cycle_length']:.1f}")
        
        if 'developmental_analysis' in analysis_results:
            print(f"\nDevelopmental Analysis:")
            dev = analysis_results['developmental_analysis']
            print(f"  Stage Distribution: {dev['stage_distribution']}")
            print(f"  Total Stages Visited: {dev['development_progression']['total_stages_visited']}")
            print(f"  Final Stage: {dev['development_progression']['final_stage']}")


def main(args: argparse.Namespace) -> int:
    """Main entry point for Visualization CLI."""
    logger = logging.getLogger("visualization_cli")
    
    if args.verbose:
        setup_logging(logging.DEBUG)
    else:
        setup_logging(logging.INFO)
    
    # Execute command based on subcommand
    if args.command == 'plot':
        create_brain_activity_plot(args)
    elif args.command == 'export':
        export_brain_data(args)
    elif args.command == 'analyze':
        analyze_activity_patterns(args)
    else:
        logger.error(f"Unknown command: {args.command}")
        return 1
    
    return 0


def setup_logging(level: int = logging.INFO) -> None:
    """Setup logging configuration."""
    logging.basicConfig(
        level=level,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        datefmt='%Y-%m-%d %H:%M:%S'
    )


def create_parser() -> argparse.ArgumentParser:
    """Create argument parser for Visualization CLI."""
    parser = argparse.ArgumentParser(
        prog='nlm visualize',
        description='NLM Visualization CLI - Generate basic visualizations of brain activity',
        epilog='Examples:\n'
              '  nlm visualize plot --steps 100 --plot-type activity\n'
              '  nlm visualize export --steps 200 --format json --output-file brain_data.json\n'
              '  nlm visualize analyze --steps 500 --analysis-type patterns'
    )
    
    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help='Enable verbose logging'
    )
    
    parser.add_argument(
        '--json',
        action='store_true',
        help='Output results as JSON'
    )
    
    # Subcommands
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Plot command
    plot_parser = subparsers.add_parser('plot', help='Create brain activity visualization')
    plot_parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps')
    plot_parser.add_argument('--neurons', type=int, default=100, help='Number of neurons (for configuration)')
    plot_parser.add_argument('--plot-type', choices=['activity', 'summary'], default='activity', help='Type of plot to create')
    plot_parser.add_argument('--output-file', type=str, help='Output file path (default: auto-generated)')
    plot_parser.set_defaults(func=create_brain_activity_plot)
    
    # Export command
    export_parser = subparsers.add_parser('export', help='Export brain data for external analysis')
    export_parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps')
    export_parser.add_argument('--format', choices=['json', 'csv', 'tsv'], default='json', help='Export format')
    export_parser.add_argument('--output-file', type=str, default='brain_data.export', help='Output file path')
    export_parser.set_defaults(func=export_brain_data)
    
    # Analyze command
    analyze_parser = subparsers.add_parser('analyze', help='Analyze brain activity patterns')
    analyze_parser.add_argument('--steps', type=int, default=200, help='Number of simulation steps')
    analyze_parser.add_argument('--analysis-type', choices=['patterns', 'temporal', 'developmental'], default='patterns', help='Type of analysis')
    analyze_parser.set_defaults(func=analyze_activity_patterns)
    
    return parser


if __name__ == '__main__':
    parser = create_parser()
    args = parser.parse_args()
    
    # Setup logging
    setup_logging(logging.DEBUG if args.verbose else logging.INFO)
    
    # Execute command
    try:
        if hasattr(args, 'func'):
            sys.exit(args.func(args))
        else:
            parser.print_help()
            sys.exit(0)
    except Exception as e:
        logging.error(f"Command failed: {e}")
        if args.json:
            json.dump({"success": False, "error": str(e)}, sys.stdout, indent=2)
        else:
            print(f"ERROR: {e}")
        sys.exit(1)