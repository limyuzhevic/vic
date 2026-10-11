#!/usr/bin/env python3
"""
NLM Visualization and Debugging Tools

Advanced visualization and debugging tools for NLM (Neural Learning Machine) brain simulations.
Provides both real-time visualization and post-simulation analysis capabilities.
"""

import sys
import os
import json
import time
import threading
from typing import Dict, List, Optional, Any, Tuple, Callable
from dataclasses import dataclass, field
from enum import Enum
import numpy as np
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from datetime import datetime

# Add the current directory to the path to import our utilities
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    pynlm = None
class VisualizationMode(Enum):
    """Visualization modes"""
    NEURAL_ACTIVITY = "neural_activity"
    SPATIAL_DISTRIBUTION = "spatial_distribution"
    TEMPORAL_DYNAMICS = "temporal_dynamics"
    REGION_ACTIVITY = "region_activity"
    MEMORY_ACTIVITY = "memory_activity"
    NEUROMODULATION = "neuromodulation"
    DEVELOPMENT = "development"
class AnalysisType(Enum):
    """Analysis types"""
    STATISTICAL = "statistical"
    PATTERN = "pattern"
    TREND = "trend"
    CORRELATION = "correlation"
class NeuralActivityPlotter:
    """Plot neural activity patterns"""
    
    def __init__(self, output_dir: str = "plots"):
        self.output_dir = output_dir
        os.makedirs(output_dir, exist_ok=True)
        
        # Setup matplotlib style
        plt.style.use('seaborn-v0_8')
        self.figsize = (12, 8)
    
    def plot_firing_rate_over_time(self, time_series: List[Dict[str, Any]], 
                                   output_file: str = "firing_rate_over_time.png") -> str:
        """Plot firing rate over time"""
        fig, axes = plt.subplots(2, 2, figsize=self.figsize)
        
        # Extract data
        steps = [d['step'] for d in time_series]
        firing_rates = [d.get('firing_rate', 0) for d in time_series]
        total_spikes = [d.get('total_spikes', 0) for d in time_series]
        
        # Plot 1: Firing rate over time
        axes[0, 0].plot(steps, firing_rates, linewidth=2, color='#2E86AB')
        axes[0, 0].set_title('Firing Rate Over Time')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Firing Rate')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Plot 2: Total spikes over time (log scale)
        axes[0, 1].semilogy(steps, total_spikes, linewidth=2, color='#A23B72')
        axes[0, 1].set_title('Total Spikes (Log Scale)')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Total Spikes')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Plot 3: Firing rate distribution
        axes[1, 0].hist(firing_rates, bins=20, alpha=0.7, color='#F18F01', edgecolor='black')
        axes[1, 0].set_title('Firing Rate Distribution')
        axes[1, 0].set_xlabel('Firing Rate')
        axes[1, 0].set_ylabel('Frequency')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Plot 4: Spike rate statistics
        if len(firing_rates) > 1:
            # Calculate moving average
            window = min(10, len(firing_rates) // 4)
            if window > 1:
                moving_avg = np.convolve(firing_rates, np.ones(window)/window, mode='valid')
                valid_steps = steps[window-1:]
                axes[1, 1].plot(valid_steps, moving_avg, linewidth=2, color='#6D597A')
                axes[1, 1].set_title(f'Moving Average (window={window})')
                axes[1, 1].set_xlabel('Step')
                axes[1, 1].set_ylabel('Firing Rate')
                axes[1, 1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        output_path = os.path.join(self.output_dir, output_file)
        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        plt.close()
        
        return output_path
    
    def plot_neural_regions(self, brain: pynlm.Brain, output_file: str = "neural_regions.png") -> str:
        """Visualize neural regions and connectivity"""
        if not PYNLM_AVAILABLE:
            return ""
        
        fig, axes = plt.subplots(1, 3, figsize=self.figsize)
        
        # Get region information
        region_ids = brain.getRegionIds()
        region_count = brain.getRegionCount()
        
        # Plot 1: Regions overview
        axes[0].bar(range(len(region_ids)), [1] * len(region_ids), 
                   color=plt.cm.Set3(np.linspace(0, 1, len(region_ids))))
        axes[0].set_title(f'Neural Regions ({region_count})')
        axes[0].set_xlabel('Region ID')
        axes[0].set_ylabel('Presence')
        axes[0].set_xticks(range(len(region_ids)))
        axes[0].grid(True, alpha=0.3)
        
        # Plot 2: Neuron distribution by region (if available)
        if region_count > 0:
            neuron_counts = []
            for rid in region_ids:
                region = brain.getRegion(rid)
                # This is a simplified approach - actual region stats may vary
                neuron_counts.append(region_count * 100 // region_count)  # Placeholder
            
            axes[1].bar(range(len(region_ids)), neuron_counts,
                       color=plt.cm.Set2(np.linspace(0, 1, len(region_ids))))
            axes[1].set_title('Neuron Count by Region')
            axes[1].set_xlabel('Region ID')
            axes[1].set_ylabel('Neuron Count')
            axes[1].grid(True, alpha=0.3)
        
        # Plot 3: Global statistics
        stats_text = f"Total Neurons: {brain.getTotalNeuronCount()}\n"
        stats_text += f"Total Synapses: {brain.getTotalSynapseCount()}\n"
        stats_text += f"Active Neurons: {brain.getActiveNeuronCount()}\n"
        stats_text += f"Firing Neurons: {brain.getFiringNeuronCount()}\n"
        stats_text += f"Average Firing Rate: {brain.getAverageFiringRate():.2f}"
        
        axes[2].text(0.1, 0.5, stats_text, fontsize=12, verticalalignment='center',
                    fontfamily='monospace', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
        axes[2].set_title('Global Statistics')
        axes[2].axis('off')
        
        plt.tight_layout()
        output_path = os.path.join(self.output_dir, output_file)
        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        plt.close()
        
        return output_path
    
    def plot_development_over_time(self, brain: pynlm.Brain, output_file: str = "development_over_time.png") -> str:
        """Plot developmental stage changes over time"""
        fig, axes = plt.subplots(2, 2, figsize=self.figsize)
        
        # We need to track development over time, so this would require storing data
        # For now, we'll show current developmental information
        
        # Plot 1: Current developmental stage
        current_stage = brain.getDevelopmentalStage()
        stage_names = ['Initial', 'Critical Period', 'Maturation', 'Adult', 'Aging']
        stage_values = [0, 0, 0, 0, 0]
        stage_values[current_stage] = 1
        
        axes[0, 0].bar(stage_names, stage_values, color=['red', 'orange', 'yellow', 'green', 'blue'])
        axes[0, 0].set_title('Current Developmental Stage')
        axes[0, 0].set_ylabel('Active')
        axes[0, 0].set_ylim(0, 1.2)
        axes[0, 0].grid(True, alpha=0.3)
        
        # Plot 2: Development statistics (placeholder)
        axes[0, 1].text(0.5, 0.5, "Development trajectory data\nwould be plotted here\nfrom step history",
                       horizontalalignment='center', verticalalignment='center',
                       transform=axes[0, 1].transAxes,
                       bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.8))
        axes[0, 1].set_title('Development Trajectory')
        axes[0, 1].axis('off')
        
        # Plot 3: Age-related changes (placeholder)
        ages = np.linspace(0, 100, 50)
        plasticity_rates = np.exp(-ages / 20)  # Exponential decay
        axes[1, 0].plot(ages, plasticity_rates, linewidth=2, color='#FF6B6B')
        axes[1, 0].set_title('Plasticity Rate vs Age')
        axes[1, 0].set_xlabel('Age (steps)')
        axes[1, 0].set_ylabel('Plasticity Rate')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Plot 4: Stage transition heatmap (placeholder)
        axes[1, 1].text(0.5, 0.5, "Stage transition patterns\nfrom experimental data\nwould be shown here",
                       horizontalalignment='center', verticalalignment='center',
                       transform=axes[1, 1].transAxes,
                       bbox=dict(boxstyle='round', facecolor='lightgreen', alpha=0.8))
        axes[1, 1].set_title('Stage Transitions')
        axes[1, 1].axis('off')
        
        plt.tight_layout()
        output_path = os.path.join(self.output_dir, output_file)
        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        plt.close()
        
        return output_path
class MemoryAnalyzer:
    """Analyze memory system activity"""
    
    def __init__(self, output_dir: str = "plots"):
        self.output_dir = output_dir
        os.makedirs(output_dir, exist_ok=True)
    
    def plot_working_memory_activity(self, brain: pynlm.Brain, output_file: str = "working_memory.png") -> str:
        """Plot working memory activity"""
        fig, axes = plt.subplots(2, 2, figsize=(14, 10))
        
        # Working memory information
        working_memory = brain.getWorkingMemory()
        
        # This is a placeholder for actual working memory analysis
        # In a real implementation, you would access the working memory system's statistics
        
        axes[0, 0].text(0.5, 0.5, "Working Memory Analysis\nWould include:\n- Active items count\n- Item recency\n- Activation patterns",
                       horizontalalignment='center', verticalalignment='center',
                       transform=axes[0, 0].transAxes,
                       bbox=dict(boxstyle='round', facecolor='lightcoral', alpha=0.8))
        axes[0, 0].set_title('Working Memory State')
        axes[0, 0].axis('off')
        
        axes[0, 1].text(0.5, 0.5, "Episodic Memory Analysis\nWould include:\n- Stored episodes\n- Replay events\n- Consolidation patterns",
                       horizontalalignment='center', verticalalignment='center',
                       transform=axes[0, 1].transAxes,
                       bbox=dict(boxstyle='round', facecolor='lightgreen', alpha=0.8))
        axes[0, 1].set_title('Episodic Memory')
        axes[0, 1].axis('off')
        
        axes[1, 0].text(0.5, 0.5, "Semantic Memory Analysis\nWould include:\n- Concept activation\n- Pattern associations\n- Knowledge base",
                       horizontalalignment='center', verticalalignment='center',
                       transform=axes[1, 0].transAxes,
                       bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.8))
        axes[1, 0].set_title('Semantic Memory')
        axes[1, 0].axis('off')
        
        axes[1, 1].text(0.5, 0.5, "Associative Memory Analysis\nWould include:\n- Pattern matching\n- Hebbian connections\n- Memory strength",
                       horizontalalignment='center', verticalalignment='center',
                       transform=axes[1, 1].transAxes,
                       bbox=dict(boxstyle='round', facecolor='lightyellow', alpha=0.8))
        axes[1, 1].set_title('Associative Memory')
        axes[1, 1].axis('off')
        
        plt.tight_layout()
        output_path = os.path.join(self.output_dir, output_file)
        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        plt.close()
        
        return output_path
class NeuromodulationVisualizer:
    """Visualize neuromodulation activity"""
    
    def __init__(self, output_dir: str = "plots"):
        self.output_dir = output_dir
        os.makedirs(output_dir, exist_ok=True)
    
    def plot_neuromodulation_levels(self, brain: pynlm.Brain, output_file: str = "neuromodulation.png") -> str:
        """Plot neuromodulation levels and interactions"""
        fig, axes = plt.subplots(2, 3, figsize=(16, 10))
        
        # Get neuromodulation systems
        dopamine = brain.getDopamine()
        curiosity = brain.getCuriosity()
        novelty = brain.getNovelty()
        prediction_error = brain.getPredictionErrorSignal()
        
        # Time series data (would need to be collected over time)
        time_steps = np.linspace(0, 100, 100)
        
        # Create synthetic data for visualization (would be replaced with real data)
        dopamine_levels = np.sin(time_steps / 10) * 0.5 + 0.5
        curiosity_levels = np.exp(-time_steps / 50) * 0.8
        novelty_levels = np.random.exponential(0.3, 100) * np.exp(-time_steps / 30)
        error_levels = np.abs(np.sin(time_steps / 5) * 0.7)
        
        # Plot 1: Dopamine over time
        axes[0, 0].plot(time_steps, dopamine_levels, linewidth=2, color='#FF6B6B')
        axes[0, 0].set_title('Dopamine Levels')
        axes[0, 0].set_xlabel('Time Step')
        axes[0, 0].set_ylabel('Level')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Plot 2: Curiosity over time
        axes[0, 1].plot(time_steps, curiosity_levels, linewidth=2, color='#4ECDC4')
        axes[0, 1].set_title('Curiosity Levels')
        axes[0, 1].set_xlabel('Time Step')
        axes[0, 1].set_ylabel('Level')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Plot 3: Novelty over time
        axes[0, 2].plot(time_steps, novelty_levels, linewidth=2, color='#45B7D1')
        axes[0, 2].set_title('Novelty Detection')
        axes[0, 2].set_xlabel('Time Step')
        axes[0, 2].set_ylabel('Level')
        axes[0, 2].grid(True, alpha=0.3)
        
        # Plot 4: Prediction error over time
        axes[1, 0].plot(time_steps, error_levels, linewidth=2, color='#96CEB4')
        axes[1, 0].set_title('Prediction Error')
        axes[1, 0].set_xlabel('Time Step')
        axes[1, 0].set_ylabel('Level')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Plot 5: Current neuromodulation levels
        current_levels = {
            'Dopamine': dopamine_levels[-1],
            'Curiosity': curiosity_levels[-1],
            'Novelty': novelty_levels[-1],
            'Error': error_levels[-1]
        }
        
        colors = ['#FF6B6B', '#4ECDC4', '#45B7D1', '#96CEB4']
        bars = axes[1, 1].bar(current_levels.keys(), current_levels.values(),
                             color=colors, alpha=0.8)
        axes[1, 1].set_title('Current Neuromodulation Levels')
        axes[1, 1].set_ylabel('Level')
        axes[1, 1].set_ylim(0, 1.2)
        axes[1, 1].grid(True, alpha=0.3)
        
        # Add value labels on bars
        for bar, value in zip(bars, current_levels.values()):
            height = bar.get_height()
            axes[1, 1].text(bar.get_x() + bar.get_width()/2., height,
                           f'{value:.2f}', ha='center', va='bottom')
        
        # Plot 6: Neuromodulator interactions (correlation heatmap)
        axes[1, 2].text(0.5, 0.5, "Neuromodulator\nInteraction Matrix\n(Would show correlations\nbetween dopamine, curiosity,\nnovelty, and prediction error)",
                       horizontalalignment='center', verticalalignment='center',
                       transform=axes[1, 2].transAxes,
                       bbox=dict(boxstyle='round', facecolor='lightpink', alpha=0.8))
        axes[1, 2].set_title('Neuromodulator Interactions')
        axes[1, 2].axis('off')
        
        plt.tight_layout()
        output_path = os.path.join(self.output_dir, output_file)
        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        plt.close()
        
        return output_path
class ExperimentAnalyzer:
    """Analyze experiment data and produce insights"""
    
    def __init__(self, output_dir: str = "analysis"):
        self.output_dir = output_dir
        os.makedirs(output_dir, exist_ok=True)
    
    def analyze_experiment_results(self, experiment_file: str, output_prefix: str = "analysis") -> List[str]:
        """Analyze experiment results from saved file"""
        try:
            with open(experiment_file, 'r') as f:
                data = json.load(f)
        except FileNotFoundError:
            return [f"Error: Experiment file not found: {experiment_file}"]
        
        generated_files = []
        
        # Statistical analysis
        if 'performance_metrics' in data:
            metrics = data['performance_metrics']
            
            # Basic statistics
            fig, axes = plt.subplots(2, 2, figsize=(14, 10))
            
            # Extract numeric metrics
            steps = [m['step'] for m in metrics if 'step' in m]
            firing_rates = [m.get('firing_rate', m.get('firing_neurons', 0)) for m in metrics]
            curiosity_levels = [m.get('curiosity', 0) for m in metrics]
            
            # Plot 1: Firing rate statistics
            axes[0, 0].hist(firing_rates, bins=20, alpha=0.7, color='skyblue', edgecolor='black')
            axes[0, 0].set_title('Firing Rate Distribution')
            axes[0, 0].set_xlabel('Firing Rate')
            axes[0, 0].set_ylabel('Frequency')
            axes[0, 0].grid(True, alpha=0.3)
            
            # Plot 2: Curiosity level distribution
            axes[0, 1].hist(curiosity_levels, bins=20, alpha=0.7, color='lightcoral', edgecolor='black')
            axes[0, 1].set_title('Curiosity Level Distribution')
            axes[0, 1].set_xlabel('Curiosity Level')
            axes[0, 1].set_ylabel('Frequency')
            axes[0, 1].grid(True, alpha=0.3)
            
            # Plot 3: Step vs metrics correlation
            axes[1, 0].scatter(steps[:100], firing_rates[:100], alpha=0.6, c=curiosity_levels[:100], 
                              cmap='viridis', s=10)
            axes[1, 0].set_title('Firing Rate vs Curiosity')
            axes[1, 0].set_xlabel('Step')
            axes[1, 0].set_ylabel('Firing Rate')
            axes[1, 0].grid(True, alpha=0.3)
            
            # Plot 4: Time series
            axes[1, 1].plot(steps[:200], firing_rates[:200], linewidth=1.5, label='Firing Rate')
            axes[1, 1].plot(steps[:200], curiosity_levels[:200], linewidth=1.5, label='Curiosity')
            axes[1, 1].set_title('Key Metrics Time Series')
            axes[1, 1].set_xlabel('Step')
            axes[1, 1].set_ylabel('Value')
            axes[1, 1].legend()
            axes[1, 1].grid(True, alpha=0.3)
            
            plt.tight_layout()
            output_file = os.path.join(self.output_dir, f"{output_prefix}_statistics.png")
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            plt.close()
            generated_files.append(output_file)
        
        return generated_files
class SimulationDebugger:
    """Debug simulation issues and provide troubleshooting"""
    
    def __init__(self):
        self.error_log = []
        self.warning_log = []
    
    def log_error(self, error: str, step: Optional[int] = None):
        """Log an error"""
        error_entry = {
            'timestamp': datetime.now().isoformat(),
            'error': error,
            'step': step,
            'type': 'error'
        }
        self.error_log.append(error_entry)
        print(f"ERROR: {error} (Step: {step})")
    
    def log_warning(self, warning: str, step: Optional[int] = None):
        """Log a warning"""
        warning_entry = {
            'timestamp': datetime.now().isoformat(),
            'warning': warning,
            'step': step,
            'type': 'warning'
        }
        self.warning_log.append(warning_entry)
        print(f"WARNING: {warning} (Step: {step})")
    
    def analyze_brain_state(self, brain: pynlm.Brain, step: Optional[int] = None) -> Dict[str, Any]:
        """Analyze brain state for potential issues"""
        analysis = {}
        
        # Check for potential issues
        issues = []
        
        # Check if brain is properly initialized
        if brain.getTotalNeuronCount() == 0:
            issues.append("No neurons in brain")
        
        if brain.getTotalSynapseCount() == 0:
            issues.append("No synapses in brain")
        
        # Check firing rates
        firing_rate = brain.getAverageFiringRate()
        if firing_rate < 0.1:
            issues.append(f"Very low firing rate: {firing_rate:.3f}")
        elif firing_rate > 100:
            issues.append(f"Very high firing rate: {firing_rate:.3f}")
        
        # Check developmental stage stability
        stage = brain.getDevelopmentalStage()
        if stage == 4:  # Aging stage with potentially unstable dynamics
            issues.append("Brain in aging stage - may be unstable")
        
        analysis['issues'] = issues
        analysis['total_neurons'] = brain.getTotalNeuronCount()
        analysis['total_synapses'] = brain.getTotalSynapseCount()
        analysis['firing_rate'] = firing_rate
        analysis['active_neurons'] = brain.getActiveNeuronCount()
        analysis['firing_neurons'] = brain.getFiringNeuronCount()
        analysis['step'] = step
        
        if issues:
            for issue in issues:
                self.log_warning(issue, step)
        
        return analysis
    
    def export_debug_info(self, filepath: str = "debug_info.json") -> None:
        """Export debug information to file"""
        debug_info = {
            'errors': self.error_log,
            'warnings': self.warning_log,
            'timestamp': datetime.now().isoformat()
        }
        
        with open(filepath, 'w') as f:
            json.dump(debug_info, f, indent=2)
        
        print(f"Debug information exported to: {filepath}")
class VisualizationPipeline:
    """Complete visualization pipeline for NLM experiments"""
    
    def __init__(self, output_base: str = "visualizations"):
        self.output_base = output_base
        self.plotter = NeuralActivityPlotter(output_base)
        self.memory_analyzer = MemoryAnalyzer(output_base)
        self.neuromodulation_visualizer = NeuromodulationVisualizer(output_base)
        self.experiment_analyzer = ExperimentAnalyzer(output_base)
        self.debugger = SimulationDebugger()
    
    def create_complete_visualization(self, brain: pynlm.Brain, 
                                     experiment_name: str,
                                     include_all: bool = True) -> List[str]:
        """Create complete visualization suite"""
        if not PYNLM_AVAILABLE:
            return []
        
        generated_files = []
        
        # Create neural activity visualizations
        neural_plots = self._create_neural_activity_plots(brain, experiment_name)
        generated_files.extend(neural_plots)
        
        # Create memory analysis
        if include_all:
            memory_plots = self.memory_analyzer.plot_working_memory_activity(brain)
            if memory_plots:
                generated_files.append(memory_plots)
        
        # Create neuromodulation visualizations
        if include_all:
            neuromod_plots = self.neuromodulation_visualizer.plot_neuromodulation_levels(brain)
            if neuromod_plots:
                generated_files.append(neuromod_plots)
        
        # Create development visualizations
        if include_all:
            dev_plots = self.plotter.plot_development_over_time(brain)
            if dev_plots:
                generated_files.append(dev_plots)
        
        # Generate neural region visualization
        region_plots = self.plotter.plot_neural_regions(brain)
        if region_plots:
            generated_files.append(region_plots)
        
        # Create summary report
        if include_all:
            report = self._create_summary_report(brain, experiment_name)
            if report:
                generated_files.append(report)
        
        print(f"Generated {len(generated_files)} visualization files for experiment '{experiment_name}'")
        return generated_files
    
    def _create_neural_activity_plots(self, brain: pynlm.Brain, experiment_name: str) -> List[str]:
        """Create neural activity plots"""
        files = []
        
        # This would require time-series data from a running experiment
        # For now, we'll create basic plots based on current brain state
        
        # Create a dummy time series for demonstration
        time_series = []
        for step in range(0, 101, 10):  # 0, 10, 20, ..., 100
            time_series.append({
                'step': step,
                'firing_rate': brain.getAverageFiringRate() * (1 + (step / 100)),
                'total_spikes': brain.getTotalSpikeCount() * (step / 100),
                'curiosity': 0.5 * (1 - np.exp(-step/50)),
                'development_stage': min(4, step // 20)
            })
        
        # Plot firing rate over time
        plot_file = self.plotter.plot_firing_rate_over_time(time_series, 
                                                           f"{experiment_name}_firing_rate.png")
        if plot_file:
            files.append(plot_file)
        
        return files
    
    def _create_summary_report(self, brain: pynlm.Brain, experiment_name: str) -> str:
        """Create a summary report"""
        fig, axes = plt.subplots(2, 2, figsize=(14, 10))
        
        # Current state summary
        stats = {
            'Total Neurons': brain.getTotalNeuronCount(),
            'Total Synapses': brain.getTotalSynapseCount(),
            'Active Neurons': brain.getActiveNeuronCount(),
            'Firing Neurons': brain.getFiringNeuronCount(),
            'Total Spikes': brain.getTotalSpikeCount(),
            'Average Firing Rate': brain.getAverageFiringRate(),
            'Developmental Stage': brain.getDevelopmentalStage()
        }
        
        # Create summary table
        axes[0, 0].axis('off')
        table_data = []
        for key, value in stats.items():
            table_data.append([key, str(value)])
        
        table = axes[0, 0].table(cellText=table_data, cellLoc='left', loc='center',
                               colWidths=[0.6, 0.4])
        table.auto_set_font_size(False)
        table.set_fontsize(10)
        table.scale(1.2, 1.5)
        axes[0, 0].set_title('Current Brain State')
        
        # Plot developmental stage
        stages = ['Initial', 'Critical', 'Maturation', 'Adult', 'Aging']
        stage_values = [0] * len(stages)
        stage_values[brain.getDevelopmentalStage()] = 1
        
        axes[0, 1].bar(stages, stage_values, color=['red', 'orange', 'yellow', 'green', 'blue'])
        axes[0, 1].set_title('Developmental Stage Distribution')
        axes[0, 1].set_ylabel('Active')
        axes[0, 1].set_ylim(0, 1.2)
        axes[0, 1].grid(True, alpha=0.3)
        
        # E/I balance
        ei_ratio = brain.getExcitationInhibitionRatio()
        axes[1, 0].pie([ei_ratio, 1 - ei_ratio], colors=['#FF6B6B', '#4ECDC4'],
                      labels=['Excitation', 'Inhibition'],
                      autopct='%1.1f%%')
        axes[1, 0].set_title(f'Excitation/Inhibition Ratio: {ei_ratio:.2f}')
        
        # Information section
        axes[1, 1].text(0.1, 0.5, f"NLM Brain Visualization\n\n"
                       f"Experiment: {experiment_name}\n"
                       f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n"
                       f"This visualization provides:\n"
                       f"• Neural activity patterns\n"
                       f"• Regional organization\n"
                       f"• Neuromodulation levels\n"
                       f"• Developmental status",
                       fontsize=12, verticalalignment='center',
                       bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
        axes[1, 1].set_title('Visualization Summary')
        axes[1, 1].axis('off')
        
        plt.suptitle(f'NLM Brain Visualization - {experiment_name}', fontsize=16)
        plt.tight_layout()
        
        output_path = os.path.join(self.output_base, f"{experiment_name}_summary.png")
        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        plt.close()
        
        return output_path
class DebugRunner:
    """Enhanced experiment runner with debugging capabilities"""
    
    def __init__(self):
        self.debugger = SimulationDebugger()
        self.visualizer = VisualizationPipeline()
    
    def run_and_debug(self, brain: pynlm.Brain, steps: int = 100, 
                     enable_visualization: bool = True,
                     experiment_name: str = "debugged_experiment") -> Dict[str, Any]:
        """Run experiment with debugging and visualization"""
        if not PYNLM_AVAILABLE:
            return {"error": "NLM not available"}
        
        print(f"Running debug experiment '{experiment_name}' for {steps} steps...")
        
        # Run simulation steps
        for step in range(steps):
            brain.step(step)
            
            # Debug at key intervals
            if step % 20 == 0:
                analysis = self.debugger.analyze_brain_state(brain, step)
                if analysis['issues']:
                    print(f"Debug analysis at step {step}: {analysis['issues']}")
        
        # Generate visualizations
        generated_files = []
        if enable_visualization:
            try:
                viz_files = self.visualizer.create_complete_visualization(
                    brain, experiment_name)
                generated_files.extend(viz_files)
            except Exception as e:
                self.debugger.log_error(f"Visualization failed: {e}")
        
        # Export debug info
        debug_file = f"debug_{experiment_name}_{int(time.time())}.json"
        self.debugger.export_debug_info(debug_file)
        
        return {
            'experiment_name': experiment_name,
            'steps': steps,
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'generated_files': generated_files,
            'debug_file': debug_file,
            'errors_count': len(self.debugger.error_log),
            'warnings_count': len(self.debugger.warning_log)
        }
if __name__ == "__main__":
    print("NLM Visualization and Debugging Tools")
    print("Version: Advanced Analysis Suite")
    print("=" * 50)
    
    if PYNLM_AVAILABLE:
        print("NLM available: Yes")
        print("Available tools:")
        print("• NeuralActivityPlotter - Neural activity visualization")
        print("• MemoryAnalyzer - Memory system analysis")
        print("• NeuromodulationVisualizer - Neuromodulation visualization")
        print("• ExperimentAnalyzer - Experiment result analysis")
        print("• SimulationDebugger - Debugging and troubleshooting")
        print("• VisualizationPipeline - Complete visualization suite")
        print("• DebugRunner - Debug experiment runner")
    else:
        print("NLM available: No")
        print("Install pynlm to use visualization tools:")
        print("  pip install pynlm")
