#!/usr/bin/env python3

"""
NLM Visualization Utilities

This module provides visualization tools for NLM brain simulations,
compatible with the enhanced Python bindings.
"""

import sys
import json
import time
from pathlib import Path
from typing import Optional, Dict, Any, List, Tuple
import math

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent))

import nlm
class SimplePlotter:
    """Simple plotting utilities for NLM visualization."""
    
    @staticmethod
    def create_ascii_plot(data: List[float], width: int = 60, height: int = 20,
                         min_val: Optional[float] = None, max_val: Optional[float] = None,
                         title: str = "Plot") -> str:
        """Create an ASCII plot from data."""
        if not data:
            return f"{title}\n{'='*width}\nNo data available\n"
        
        # Calculate min/max values
        if min_val is None:
            min_val = min(data)
        if max_val is None:
            max_val = max(data)
        
        if max_val == min_val:
            max_val = min_val + 1.0
        
        # Create plot
        plot_lines = []
        plot_lines.append(f"{title}")
        plot_lines.append("=" * width)
        
        # Normalize data
        normalized = []
        for value in data:
            norm = (value - min_val) / (max_val - min_val)
            normalized.append(norm)
        
        # Create ASCII plot
        for row in range(height, 0, -1):
            line = "|"
            for col in range(width):
                # Map column position to data index
                data_index = int((col / (width - 1)) * (len(data) - 1))
                if data_index < len(normalized):
                    norm_value = normalized[data_index]
                    # Map row position to value (higher row = lower value)
                    value_row = int((1 - norm_value) * (height - 1))
                    
                    if row == value_row + 1:
                        line += "*"
                    else:
                        line += " "
            plot_lines.append(line)
        
        # Add axis
        axis_line = "+"
        for col in range(width):
            axis_line += "-"
        plot_lines.append(axis_line)
        
        # Add labels
        plot_lines.append(f"Min: {min_val:.3f}")
        plot_lines.append(f"Max: {max_val:.3f}")
        
        return "\n".join(plot_lines)
    
    @staticmethod
    def create_spike_raster(neuron_ids: List[int], spike_steps: List[int],
                           width: int = 80, height: int = 20) -> str:
        """Create an ASCII spike raster plot."""
        if not spike_steps:
            return "Spike Raster Plot\n==================\nNo spike data available\n"
        
        # Find max values
        max_neuron = max(neuron_ids) if neuron_ids else 10
        max_step = max(spike_steps) if spike_steps else 100
        
        # Create raster
        raster_lines = []
        raster_lines.append("Spike Raster Plot")
        raster_lines.append("=" * width)
        
        # Draw neurons as horizontal lines
        neurons_per_row = min(max_neuron + 1, height)
        neurons_displayed = min(len(neuron_ids), neurons_per_row)
        
        for neuron_idx in range(neurons_displayed - 1, -1, -1):
            line = f"N{neuron_idx:02d}:|"
            
            # Find spikes for this neuron
            neuron_spikes = [step for n, step in zip(neuron_ids, spike_steps) if n == neuron_idx]
            
            # Draw spikes
            for step in neuron_spikes:
                col = int((step / max_step) * (width - 10))
                if col >= len(line):
                    break
                line = line[:col + 1] + "*" + line[col + 2:]
            
            raster_lines.append(line)
        
        # Add axis
        axis_line = "  " + "-" * (width - 2)
        raster_lines.append(axis_line)
        raster_lines.append(f"  Time steps: 0 -> {max_step}")
        
        return "\n".join(raster_lines)
class BrainVisualizer:
    """Visualization utilities for NLM brain data."""
    
    def __init__(self, sim=None, brain=None):
        self.sim = sim
        self.brain = brain
    
    def create_statistics_report(self) -> str:
        """Create a comprehensive statistics report."""
        if self.sim:
            return self._create_sim_report()
        elif self.brain:
            return self._create_brain_report()
        else:
            return "No brain or simulation data available."
    
    def _create_sim_report(self) -> str:
        """Create report from simulation."""
        stats = self.sim.getStats()
        memory_stats = self.sim.getMemoryStats()
        prediction_stats = self.sim.getPredictionStats()
        cognitive_stats = self.sim.getCognitiveStats()
        neuromod_stats = self.sim.getNeuromodulationStats()
        dev_stats = self.sim.getDevelopmentStats()
        
        report_lines = []
        report_lines.append("=== NLM Brain Statistics Report ===")
        report_lines.append(f"Simulation Step: {stats.get('step', 'N/A')}")
        report_lines.append(f"Total Neurons: {stats.get('total_neurons', 'N/A')}")
        report_lines.append(f"Total Synapses: {stats.get('total_synapses', 'N/A')}")
        report_lines.append(f"Total Spikes: {stats.get('total_spikes', 'N/A')}")
        report_lines.append(f"Active Neurons: {stats.get('active_neurons', 'N/A')}")
        report_lines.append(f"Firing Neurons: {stats.get('firing_neurons', 'N/A')}")
        report_lines.append(f"Excitatory/Inhibition Ratio: {stats.get('exc_inhibition_ratio', 'N/A'):.3f}")
        report_lines.append(f"Average Firing Rate: {stats.get('avg_firing_rate', 'N/A'):.3f}")
        report_lines.append("")
        
        if dev_stats:
            report_lines.append(f"Development Stage: {dev_stats.get('stage', 'N/A')}")
            report_lines.append("")
        
        if memory_stats:
            report_lines.append("Memory Systems:")
            report_lines.append(f"  Working Memory: {memory_stats.get('working_memory_size', 'N/A')}/"
                              f"{memory_stats.get('working_memory_capacity', 'N/A')}")
            report_lines.append(f"  Episodic Memory: {memory_stats.get('episodic_memory_count', 'N/A')} episodes")
            if memory_stats.get('has_associative_memory', False):
                report_lines.append(f"  Associative Memory: Available")
            report_lines.append("")
        
        if prediction_stats:
            report_lines.append("Prediction System: Available")
            report_lines.append("")
        
        if cognitive_stats:
            report_lines.append("Cognitive Systems:")
            if cognitive_stats.get('has_planner', False):
                report_lines.append(f"  ✓ Neural Planner: Available")
            if cognitive_stats.get('has_concept_formation', False):
                report_lines.append(f"  ✓ Concept Formation: Available")
            if cognitive_stats.get('has_attention', False):
                report_lines.append(f"  ✓ Attention System: Available")
            report_lines.append("")
        
        if neuromod_stats:
            report_lines.append("Neuromodulation:")
            if neuromod_stats.get('has_dopamine', False):
                report_lines.append(f"  ✓ Dopamine System: Available")
            if neuromod_stats.get('has_curiosity', False):
                report_lines.append(f"  ✓ Curiosity Level: {neuromod_stats.get('curiosity_level', 'N/A')}")
            if neuromod_stats.get('has_prediction_error', False):
                report_lines.append(f"  ✓ Prediction Error: Available")
            report_lines.append("")
        
        return "\n".join(report_lines)
    
    def _create_brain_report(self) -> str:
        """Create report from brain object."""
        report_lines = []
        report_lines.append("=== NLM Brain Statistics Report ===")
        report_lines.append(f"Total Neurons: {self.brain.getTotalNeuronCount()}")
        report_lines.append(f"Total Synapses: {self.brain.getTotalSynapseCount()}")
        report_lines.append(f"Total Spikes: {self.brain.getTotalSpikeCount()}")
        report_lines.append(f"Active Neurons: {self.brain.getActiveNeuronCount()}")
        report_lines.append(f"Firing Neurons: {self.brain.getFiringNeuronCount()}")
        report_lines.append(f"Excitatory/Inhibition Ratio: {self.brain.getExcitationInhibitionRatio():.3f}")
        report_lines.append(f"Average Firing Rate: {self.brain.getAverageFiringRate():.3f}")
        report_lines.append(f"Development Stage: {self.brain.getDevelopmentalStage()}")
        report_lines.append("")
        
        # Memory systems (if available)
        working_mem = self.brain.getWorkingMemory()
        if working_mem:
            report_lines.append(f"Working Memory: {working_mem.getCurrentSize()}/"
                              f"{working_mem.getCapacity()}")
        
        episodic_mem = self.brain.getEpisodicMemory()
        if episodic_mem:
            report_lines.append(f"Episodic Memory: {episodic_mem.getEpisodeCount()} episodes")
        
        return "\n".join(report_lines)
    
    def create_development_timeline(self, steps: int = 100) -> str:
        """Create a development timeline visualization."""
        if not self.sim:
            return "No simulation data available for development timeline."
        
        lines = []
        lines.append("=== Development Timeline ===")
        lines.append("")
        
        # Collect developmental stage data
        stage_changes = []
        current_stage = self.sim.getDevelopmentStats().get('stage', 0)
        stage_changes.append((0, current_stage))
        
        # Run simulation and track stage changes
        for step in range(1, steps + 1):
            self.sim.run()  # Run one step
            dev_stats = self.sim.getDevelopmentStats()
            new_stage = dev_stats.get('stage', 0)
            
            if new_stage != stage_changes[-1][1]:
                stage_changes.append((step, new_stage))
        
        # Create timeline visualization
        timeline_width = 60
        for step, stage in stage_changes:
            # Map step to position
            pos = int((step / steps) * timeline_width)
            if pos > timeline_width:
                pos = timeline_width
            
            # Create stage marker
            stage_name = self._get_stage_name(stage)
            marker = f"{stage_name}"
            
            # Add to timeline
            timeline_line = " " * pos + marker
            lines.append(timeline_line)
        
        lines.append("")
        lines.append(f"Total steps: {steps}")
        lines.append(f"Stage changes: {len(stage_changes) - 1}")
        
        return "\n".join(lines)
    
    def _get_stage_name(self, stage_code: int) -> str:
        """Convert stage code to readable name."""
        stages = {
            0: "INIT",
            1: "CRIT",
            2: "MATU", 
            3: "ADULT",
            4: "AGING"
        }
        return stages.get(stage_code, f"STAGE{stage_code}")
    
    def create_memory_usage_chart(self) -> str:
        """Create a memory usage visualization."""
        if not self.sim:
            return "No simulation data available for memory chart."
        
        memory_stats = self.sim.getMemoryStats()
        if not memory_stats:
            return "No memory statistics available."
        
        lines = []
        lines.append("=== Memory Usage ===")
        lines.append("")
        
        # Working memory
        wm_size = memory_stats.get('working_memory_size', 0)
        wm_capacity = memory_stats.get('working_memory_capacity', 1)
        wm_percentage = (wm_size / wm_capacity) * 100 if wm_capacity > 0 else 0
        
        lines.append("Working Memory:")
        bar_width = 40
        filled = int((wm_percentage / 100) * bar_width)
        bar = "[" + "=" * filled + ">" + " " * (bar_width - filled - 1) + "]"
        lines.append(f"  {bar} {wm_percentage:.1f}%")
        lines.append(f"  Size: {wm_size}/{wm_capacity}")
        lines.append("")
        
        # Episodic memory
        episodic_count = memory_stats.get('episodic_memory_count', 0)
        lines.append(f"Episodic Memory:")
        lines.append(f"  Episodes stored: {episodic_count}")
        lines.append("")
        
        return "\n".join(lines)
    
    def create_neuromodulation_dashboard(self) -> str:
        """Create a neuromodulation status dashboard."""
        if not self.sim:
            return "No simulation data available for neuromodulation dashboard."
        
        neuromod_stats = self.sim.getNeuromodulationStats()
        if not neuromod_stats:
            return "No neuromodulation statistics available."
        
        lines = []
        lines.append("=== Neuromodulation Dashboard ===")
        lines.append("")
        
        # Dopamine
        if neuromod_stats.get('has_dopamine', False):
            lines.append("Dopamine: ✓ Available")
            lines.append("  Role: Reward processing and reinforcement learning")
            lines.append("")
        
        # Curiosity
        if neuromod_stats.get('has_curiosity', False):
            curiosity_level = neuromod_stats.get('curiosity_level', 0)
            lines.append(f"Curiosity: ✓ Active ({curiosity_level:.3f})")
            lines.append("  Role: Exploration motivation")
            lines.append("")
        
        # Novelty
        if neuromod_stats.get('has_novelty', False):
            lines.append("Novelty Detection: ✓ Available")
            lines.append("  Role: Detects new patterns and experiences")
            lines.append("")
        
        # Prediction Error
        if neuromod_stats.get('has_prediction_error', False):
            lines.append("Prediction Error: ✓ Available")
            lines.append("  Role: Error signal for learning and adaptation")
            lines.append("")
        
        return "\n".join(lines)
    
    def create_cognitive_status(self) -> str:
        """Create a cognitive systems status report."""
        if not self.sim:
            return "No simulation data available for cognitive status."
        
        cognitive_stats = self.sim.getCognitiveStats()
        if not cognitive_stats:
            return "No cognitive statistics available."
        
        lines = []
        lines.append("=== Cognitive Systems Status ===")
        lines.append("")
        
        # Neural Planner
        if cognitive_stats.get('has_planner', False):
            lines.append("Neural Planner: ✓ Available")
            lines.append("  Function: Multi-step action prediction and planning")
            lines.append("")
        
        # Concept Formation
        if cognitive_stats.get('has_concept_formation', False):
            lines.append("Concept Formation: ✓ Available")
            lines.append("  Function: Pattern discovery and category learning")
            lines.append("")
        
        # Attention System
        if cognitive_stats.get('has_attention', False):
            lines.append("Attention System: ✓ Available")
            lines.append("  Function: Competitive selection of neural activity")
            lines.append("")
        
        return "\n".join(lines)
    
    def create_full_dashboard(self) -> str:
        """Create a complete visualization dashboard."""
        if not self.sim:
            return "No simulation data available for dashboard."
        
        dashboard_lines = []
        
        # Main statistics
        dashboard_lines.append(self.create_statistics_report())
        dashboard_lines.append("")
        
        # Development timeline
        dashboard_lines.append(self.create_development_timeline(50))
        dashboard_lines.append("")
        
        # Memory usage
        dashboard_lines.append(self.create_memory_usage_chart())
        dashboard_lines.append("")
        
        # Neuromodulation
        dashboard_lines.append(self.create_neuromodulation_dashboard())
        dashboard_lines.append("")
        
        # Cognitive status
        dashboard_lines.append(self.create_cognitive_status())
        
        return "\n".join(dashboard_lines)
def create_ascii_brain_animation(steps: int = 20) -> str:
    """Create an ASCII animation of brain activity."""
    lines = []
    lines.append("=== Brain Activity Animation ===")
    lines.append("")
    
    # Simulate brain activity
    import random
    
    for step in range(steps):
        lines.append(f"Step {step + 1:2d}:")
        
        # Create activity heatmap
        activity = [[random.random() * 100 for _ in range(10)] for _ in range(5)]
        
        # Normalize for display
        max_activity = max(max(row) for row in activity)
        
        for row in activity:
            line = "|"
            for val in row:
                intensity = int((val / max_activity) * 10)
                if intensity >= 8:
                    line += "@@@"
                elif intensity >= 6:
                    line += "***"
                elif intensity >= 4:
                    line += "  *"
                else:
                    line += "   "
            line += "|"
            lines.append(line)
        
        lines.append("")
    
    return "\n".join(lines)
class DataExporter:
    """Utilities for exporting NLM data to various formats."""
    
    @staticmethod
    def export_to_csv(sim, filepath: str):
        """Export simulation data to CSV format."""
        import csv
        
        with open(filepath, 'w', newline='') as csvfile:
            writer = csv.writer(csvfile)
            
            # Header
            writer.writerow(['Step', 'Total Neurons', 'Total Synapses', 'Total Spikes',
                           'Active Neurons', 'Firing Neurons', 'Exc/Inh Ratio',
                           'Avg Firing Rate'])
            
            # Data for each step
            for step in range(sim.get_total_steps()):
                sim.run()
                stats = sim.getStats()
                
                writer.writerow([
                    stats.get('step', step),
                    stats.get('total_neurons', 'N/A'),
                    stats.get('total_synapses', 'N/A'),
                    stats.get('total_spikes', 'N/A'),
                    stats.get('active_neurons', 'N/A'),
                    stats.get('firing_neurons', 'N/A'),
                    stats.get('exc_inhibition_ratio', 'N/A'),
                    stats.get('avg_firing_rate', 'N/A')
                ])
        
        print(f"Data exported to {filepath}")
    
    @staticmethod
    def export_to_json(sim, filepath: str):
        """Export simulation data to JSON format."""
        import json
        
        data = {
            'simulation': {
                'total_steps': sim.get_total_steps(),
                'current_step': sim.getStep()
            },
            'statistics': sim.getStats(),
            'memory': sim.getMemoryStats(),
            'prediction': sim.getPredictionStats(),
            'cognitive': sim.getCognitiveStats(),
            'neuromodulation': sim.getNeuromodulationStats(),
            'development': sim.getDevelopmentStats(),
            'learning': sim.runLearningExperiment()
        }
        
        with open(filepath, 'w') as jsonfile:
            json.dump(data, jsonfile, indent=2)
        
        print(f"Data exported to {filepath}")
    
    @staticmethod
    def export_to_text_report(sim, filepath: str):
        """Export simulation data to text report format."""
        with open(filepath, 'w') as textfile:
            textfile.write("NLM Simulation Report\n")
            textfile.write("=" * 50 + "\n\n")
            
            # Run simulation and collect data
            for step in range(sim.get_total_steps()):
                sim.run()
                textfile.write(f"Step {step + 1}:\n")
                
                stats = sim.getStats()
                textfile.write(f"  Statistics:\n")
                textfile.write(f"    Total Neurons: {stats.get('total_neurons', 'N/A')}\n")
                textfile.write(f"    Total Synapses: {stats.get('total_synapses', 'N/A')}\n")
                textfile.write(f"    Total Spikes: {stats.get('total_spikes', 'N/A')}\n")
                textfile.write(f"    Average Firing Rate: {stats.get('avg_firing_rate', 'N/A'):.3f}\n")
                
                if step < sim.get_total_steps() - 1:
                    textfile.write("\n")
        
        print(f"Text report exported to {filepath}")
class VisualizationFactory:
    """Factory for creating different types of visualizations."""
    
    @staticmethod
    def create_ascii_plot(data: List[float], title: str = "Data Plot") -> str:
        """Create an ASCII plot."""
        return SimplePlotter.create_ascii_plot(data, title)
    
    @staticmethod
    def create_spike_raster(neuron_ids: List[int], spike_steps: List[int]) -> str:
        """Create a spike raster plot."""
        return SimplePlotter.create_spike_raster(neuron_ids, spike_steps)
    
    @staticmethod
    def create_brain_animation(steps: int = 20) -> str:
        """Create a brain activity animation."""
        return create_ascii_brain_animation(steps)
    
    @staticmethod
    def create_dashboard(sim) -> str:
        """Create a complete dashboard."""
        visualizer = BrainVisualizer(sim)
        return visualizer.create_full_dashboard()


# Convenience functions for common visualization tasks
def create_simple_stats_report(sim) -> str:
    """Create a simple statistics report."""
    visualizer = BrainVisualizer(sim)
    return visualizer.create_statistics_report()
def create_memory_usage_chart(sim) -> str:
    """Create a memory usage chart."""
    visualizer = BrainVisualizer(sim)
    return visualizer.create_memory_usage_chart()
def create_neuromodulation_dashboard(sim) -> str:
    """Create a neuromodulation dashboard."""
    visualizer = BrainVisualizer(sim)
    return visualizer.create_neuromodulation_dashboard()
# Export all public classes and functions
__all__ = [
    'SimplePlotter',
    'BrainVisualizer',
    'DataExporter',
    'VisualizationFactory',
    'create_ascii_plot',
    'create_spike_raster',
    'create_brain_animation',
    'create_dashboard',
    'create_simple_stats_report',
    'create_memory_usage_chart',
    'create_neuromodulation_dashboard',
    'Plotter',
    'Exporter',
    'Factory',
]
