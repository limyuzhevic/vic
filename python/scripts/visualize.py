#!/usr/bin/env python3
"""
NLM Visualization Tools

A collection of visualization and analysis tools for NLM simulation data.
Provides plotting capabilities, statistical analysis, and result visualization.

Usage:
    python visualize.py --input data.json --output plot.png
"""

import argparse
import json
import csv
import sys
import os
from pathlib import Path
from typing import Dict, List, Any, Optional
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns
from dataclasses import dataclass

# Configure matplotlib for better visualization
plt.style.use('seaborn-v0_8')

import argparse
import json
import csv
import sys
import os
from pathlib import Path
from typing import Dict, List, Any, Optional
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
from dataclasses import dataclass

# Configure matplotlib for better visualization
plt.style.use('seaborn-v0_8')

@dataclass
class SimulationResult:
    """Data class for storing simulation results."""
    timestamp: float
    step: int
    firing_rate: float
    neurons_firing: int
    total_spikes: int
    curiosity_level: float
    novelty_level: float
    prediction_error: float
    e_i_ratio: float
    simulation_time: float
    
    @classmethod
    def from_dict_list(cls, data: List[Dict]) -> List['SimulationResult']:
        """Create SimulationResult objects from a list of dictionaries."""
        results = []
        for item in data:
            result = cls(
                timestamp=float(item.get('timestamp', 0)),
                step=int(item.get('step', 0)),
                firing_rate=float(item.get('firing_rate', 0)),
                neurons_firing=int(item.get('neurons_firing', 0)),
                total_spikes=int(item.get('total_spikes', 0)),
                curiosity_level=float(item.get('curiosity_level', 0)),
                novelty_level=float(item.get('novelty_level', 0)),
                prediction_error=float(item.get('prediction_error', 0)),
                e_i_ratio=float(item.get('e_i_ratio', 0)),
                simulation_time=float(item.get('simulation_time', 0))
            )
            results.append(result)
        return results
    
    @classmethod
    def from_dataframe(cls, df: pd.DataFrame) -> List['SimulationResult']:
        """Create SimulationResult objects from a pandas DataFrame."""
        results = []
        for _, row in df.iterrows():
            result = cls(
                timestamp=float(row.get('timestamp', 0)),
                step=int(row.get('step', 0)),
                firing_rate=float(row.get('firing_rate', 0)),
                neurons_firing=int(row.get('neurons_firing', 0)),
                total_spikes=int(row.get('total_spikes', 0)),
                curiosity_level=float(row.get('curiosity_level', 0)),
                novelty_level=float(row.get('novelty_level', 0)),
                prediction_error=float(row.get('prediction_error', 0)),
                e_i_ratio=float(row.get('e_i_ratio', 0)),
                simulation_time=float(row.get('simulation_time', 0))
            )
            results.append(result)
        return results

def load_simulation_data(file_path: str, file_format: str = "json") -> List[SimulationResult]:
    """Load simulation data from file."""
    if file_format == "json":
        with open(file_path, 'r') as f:
            data = json.load(f)
            return SimulationResult.from_dict_list(data)
    elif file_format == "csv":
        df = pd.read_csv(file_path)
        return SimulationResult.from_dataframe(df)
    else:
        raise ValueError(f"Unsupported format: {file_format}")

def analyze_simulation_trends(data: List[SimulationResult]) -> Dict[str, Any]:
    """Analyze trends in simulation data."""
    if not data:
        return {}
    
    # Convert to pandas DataFrame for easier analysis
    df = pd.DataFrame([{
        'timestamp': r.timestamp,
        'step': r.step,
        'firing_rate': r.firing_rate,
        'neurons_firing': r.neurons_firing,
        'total_spikes': r.total_spikes,
        'curiosity_level': r.curiosity_level,
        'novelty_level': r.novelty_level,
        'prediction_error': r.prediction_error,
        'e_i_ratio': r.e_i_ratio,
        'simulation_time': r.simulation_time
    } for r in data])
    
    analysis = {
        'total_steps': len(data),
        'time_range': (df['timestamp'].min(), df['timestamp'].max()),
        'firing_rate_stats': {
            'mean': df['firing_rate'].mean(),
            'std': df['firing_rate'].std(),
            'min': df['firing_rate'].min(),
            'max': df['firing_rate'].max(),
            'median': df['firing_rate'].median()
        },
        'curiosity_stats': {
            'mean': df['curiosity_level'].mean(),
            'std': df['curiosity_level'].std(),
            'trend': _calculate_trend(df['curiosity_level'])
        },
        'novelty_stats': {
            'mean': df['novelty_level'].mean(),
            'std': df['novelty_level'].std(),
            'trend': _calculate_trend(df['novelty_level'])
        },
        'prediction_error_stats': {
            'mean': df['prediction_error'].mean(),
            'std': df['prediction_error'].std(),
            'trend': _calculate_trend(df['prediction_error'])
        },
        'e_i_ratio_stats': {
            'mean': df['e_i_ratio'].mean(),
            'std': df['e_i_ratio'].std(),
            'trend': _calculate_trend(df['e_i_ratio'])
        },
        'staleness_stats': {
            'spike_frequency': _calculate_spike_frequency(df),
            'activity_patterns': _identify_activity_patterns(df)
        }
    }
    
    return analysis

def _calculate_trend(series: pd.Series) -> str:
    """Calculate trend direction."""
    if len(series) < 2:
        return "insufficient_data"
    
    correlation = series.corr(pd.Series(range(len(series))))
    if abs(correlation) < 0.1:
        return "stable"
    elif correlation > 0:
        return "increasing"
    else:
        return "decreasing"

def _calculate_spike_frequency(df: pd.DataFrame) -> Dict[str, float]:
    """Calculate spike frequency metrics."""
    if df['total_spikes'].sum() == 0:
        return {'average': 0.0, 'peak': 0.0, 'stability': 0.0}
    
    spike_frequency = df['total_spikes'] / df['simulation_time']
    return {
        'average': spike_frequency.mean(),
        'peak': spike_frequency.max(),
        'stability': 1.0 - (spike_frequency.std() / spike_frequency.mean() if spike_frequency.mean() > 0 else 0)
    }

def _identify_activity_patterns(df: pd.DataFrame) -> List[str]:
    """Identify activity patterns in the simulation."""
    patterns = []
    
    # Check for burst activity
    firing_rate_std = df['firing_rate'].std()
    firing_rate_mean = df['firing_rate'].mean()
    if firing_rate_std > firing_rate_mean * 0.5:
        patterns.append("burst_activity")
    
    # Check for exploration phase (high curiosity)
    if df['curiosity_level'].mean() > 0.7:
        patterns.append("exploration_phase")
    
    # Check for exploitation phase (low curiosity)
    if df['curiosity_level'].mean() < 0.3:
        patterns.append("exploitation_phase")
    
    # Check for learning phase (decreasing prediction error)
    pred_error_trend = _calculate_trend(df['prediction_error'])
    if pred_error_trend == "decreasing":
        patterns.append("learning_phase")
    
    return patterns

def create_visualization_plots(data: List[SimulationResult], analysis: Dict[str, Any], 
                              output_path: Optional[str] = None) -> None:
    """Create comprehensive visualization plots."""
    if not data:
        print("No data available for visualization")
        return
    
    df = pd.DataFrame([{
        'timestamp': r.timestamp,
        'step': r.step,
        'firing_rate': r.firing_rate,
        'neurons_firing': r.neurons_firing,
        'total_spikes': r.total_spikes,
        'curiosity_level': r.curiosity_level,
        'novelty_level': r.novelty_level,
        'prediction_error': r.prediction_error,
        'e_i_ratio': r.e_i_ratio
    } for r in data])
    
    # Create a comprehensive figure with multiple subplots
    fig, axes = plt.subplots(3, 3, figsize=(18, 12))
    fig.suptitle('NLM Simulation Analysis', fontsize=16, fontweight='bold')
    
    # 1. Firing rate over time
    ax = axes[0, 0]
    ax.plot(df['timestamp'], df['firing_rate'], linewidth=2, color='blue')
    ax.set_title('Firing Rate Over Time')
    ax.set_xlabel('Time (s)')
    ax.set_ylabel('Firing Rate (Hz)')
    ax.grid(True, alpha=0.3)
    
    # 2. Neurons firing over time
    ax = axes[0, 1]
    ax.plot(df['timestamp'], df['neurons_firing'], linewidth=2, color='green')
    ax.set_title('Neurons Firing Over Time')
    ax.set_xlabel('Time (s)')
    ax.set_ylabel('Firing Neurons')
    ax.grid(True, alpha=0.3)
    
    # 3. Spike count over time
    ax = axes[0, 2]
    ax.plot(df['timestamp'], df['total_spikes'], linewidth=2, color='red')
    ax.set_title('Total Spikes Over Time')
    ax.set_xlabel('Time (s)')
    ax.set_ylabel('Total Spikes')
    ax.grid(True, alpha=0.3)
    
    # 4. Curiosity level over time
    ax = axes[1, 0]
    ax.plot(df['timestamp'], df['curiosity_level'], linewidth=2, color='orange')
    ax.set_title('Curiosity Level Over Time')
    ax.set_xlabel('Time (s)')
    ax.set_ylabel('Curiosity Level')
    ax.grid(True, alpha=0.3)
    
    # 5. Novelty level over time
    ax = axes[1, 1]
    ax.plot(df['timestamp'], df['novelty_level'], linewidth=2, color='purple')
    ax.set_title('Novelty Level Over Time')
    ax.set_xlabel('Time (s)')
    ax.set_ylabel('Novelty Level')
    ax.grid(True, alpha=0.3)
    
    # 6. Prediction error over time
    ax = axes[1, 2]
    ax.plot(df['timestamp'], df['prediction_error'], linewidth=2, color='brown')
    ax.set_title('Prediction Error Over Time')
    ax.set_xlabel('Time (s)')
    ax.set_ylabel('Prediction Error')
    ax.grid(True, alpha=0.3)
    
    # 7. E/I ratio over time
    ax = axes[2, 0]
    ax.plot(df['timestamp'], df['e_i_ratio'], linewidth=2, color='cyan')
    ax.set_title('Excitation/Inhibition Ratio Over Time')
    ax.set_xlabel('Time (s)')
    ax.set_ylabel('E/I Ratio')
    ax.grid(True, alpha=0.3)
    
    # 8. Activity heatmap (subsampled data)
    ax = axes[2, 1]
    if len(df) > 100:
        subsample = df.iloc[::max(1, len(df)//100)]
        im = ax.imshow(subsample[['firing_rate', 'curiosity_level', 'novelty_level']].T,
                      aspect='auto', cmap='viridis', interpolation='nearest')
        ax.set_title('Activity Heatmap')
        ax.set_xlabel('Time Steps')
        ax.set_ylabel('Metrics')
        plt.colorbar(im, ax=ax)
    else:
        ax.text(0.5, 0.5, 'Insufficient data for heatmap', 
               ha='center', va='center', transform=ax.transAxes)
        ax.set_title('Activity Heatmap')
    
    # 9. Summary statistics
    ax = axes[2, 2]
    stats = analysis.get('firing_rate_stats', {})
    metrics = ['Mean', 'Std', 'Min', 'Max', 'Median']
    values = [stats.get(m.lower(), 0) for m in metrics]
    
    bars = ax.bar(metrics, values, color=['skyblue', 'lightcoral', 'lightgreen', 'gold', 'plum'])
    ax.set_title('Firing Rate Statistics')
    ax.set_ylabel('Value (Hz)')
    ax.set_ylim(0, max(values) * 1.1 if values else 1)
    
    # Add value labels on bars
    for bar, value in zip(bars, values):
        height = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2., height + 0.01,
               f'{value:.2f}', ha='center', va='bottom')
    
    plt.tight_layout()
    
    if output_path:
        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        print(f"Visualization saved to {output_path}")
    
    plt.show()

def create_statistical_report(analysis: Dict[str, Any]) -> str:
    """Create a statistical report from analysis data."""
    report = "=== NLM Simulation Statistical Report ===\n\n"
    
    if not analysis:
        return report + "No analysis data available."
    
    # Basic information
    report += f"Total simulation steps: {analysis.get('total_steps', 0)}\n"
    time_range = analysis.get('time_range', (0, 0))
    report += f"Time range: {time_range[0]:.2f} - {time_range[1]:.2f} seconds\n\n"
    
    # Firing rate statistics
    fr_stats = analysis.get('firing_rate_stats', {})
    report += "=== Firing Rate Statistics ===\n"
    report += f"Mean: {fr_stats.get('mean', 0):.3f} Hz\n"
    report += f"Standard deviation: {fr_stats.get('std', 0):.3f} Hz\n"
    report += f"Minimum: {fr_stats.get('min', 0):.3f} Hz\n"
    report += f"Maximum: {fr_stats.get('max', 0):.3f} Hz\n"
    report += f"Median: {fr_stats.get('median', 0):.3f} Hz\n\n"
    
    # Curiosity statistics
    cur_stats = analysis.get('curiosity_stats', {})
    report += "=== Curiosity Statistics ===\n"
    report += f"Mean level: {cur_stats.get('mean', 0):.3f}\n"
    report += f"Standard deviation: {cur_stats.get('std', 0):.3f}\n"
    report += f"Trend: {cur_stats.get('trend', 'unknown')}\n\n"
    
    # Novelty statistics
    nov_stats = analysis.get('novelty_stats', {})
    report += "=== Novelty Statistics ===\n"
    report += f"Mean level: {nov_stats.get('mean', 0):.3f}\n"
    report += f"Standard deviation: {nov_stats.get('std', 0):.3f}\n"
    report += f"Trend: {nov_stats.get('trend', 'unknown')}\n\n"
    
    # Prediction error statistics
    pred_stats = analysis.get('prediction_error_stats', {})
    report += "=== Prediction Error Statistics ===\n"
    report += f"Mean error: {pred_stats.get('mean', 0):.3f}\n"
    report += f"Standard deviation: {pred_stats.get('std', 0):.3f}\n"
    report += f"Trend: {pred_stats.get('trend', 'unknown')}\n\n"
    
    # E/I ratio statistics
    ei_stats = analysis.get('e_i_ratio_stats', {})
    report += "=== Excitation/Inhibition Ratio Statistics ===\n"
    report += f"Mean ratio: {ei_stats.get('mean', 0):.3f}\n"
    report += f"Standard deviation: {ei_stats.get('std', 0):.3f}\n"
    report += f"Trend: {ei_stats.get('trend', 'unknown')}\n\n"
    
    # Activity patterns
    patterns = analysis.get('staleness_stats', {}).get('activity_patterns', [])
    if patterns:
        report += "=== Activity Patterns ===\n"
        for pattern in patterns:
            report += f"• {pattern}\n"
        report += "\n"
    
    return report

def main():
    """Main function."""
    parser = argparse.ArgumentParser(
        description="NLM Visualization and Analysis Tools",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument("--input", required=True, help="Input data file path")
    parser.add_argument("--input-format", choices=["json", "csv"], 
                      default="json", help="Input file format")
    parser.add_argument("--output", help="Output plot file path")
    parser.add_argument("--report", help="Output statistical report file path")
    parser.add_argument("--no-display", action="store_true", help="Do not display plots (save only)")
    
    args = parser.parse_args()
    
    # Load data
    print(f"Loading simulation data from {args.input}...")
    data = load_simulation_data(args.input, args.input_format)
    print(f"Loaded {len(data)} data points")
    
    if not data:
        print("No data available for analysis")
        return
    
    # Analyze data
    print("Analyzing simulation data...")
    analysis = analyze_simulation_trends(data)
    
    # Create statistical report
    report = create_statistical_report(analysis)
    print("\n" + report)
    
    # Save report if requested
    if args.report:
        with open(args.report, 'w') as f:
            f.write(report)
        print(f"Statistical report saved to {args.report}")
    
    # Create visualizations
    if args.output or not args.no_display:
        print("Creating visualization plots...")
        create_visualization_plots(data, analysis, args.output if args.output else None)
    
    print("\nAnalysis complete!")

if __name__ == "__main__":
    main()
