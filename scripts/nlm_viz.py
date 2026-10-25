#!/usr/bin/env python3
"""
NLM Visualization and Debugging Tools

This module provides advanced debugging and visualization capabilities for NLM experiments.
It includes:
- Brain visualization tools
- Activity monitoring
- Debugging utilities
- Performance profiling
- Result visualization
"""

import os
import sys
import json
import time
import threading
import numpy as np
from pathlib import Path
from typing import Dict, List, Optional, Any, Callable
from dataclasses import dataclass
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from collections import deque

# Try to import NLM bindings
try:
    import nlm as nlm_module
    NLM_AVAILABLE = True
except ImportError:
    NLM_AVAILABLE = False

@dataclass
class VisualizationConfig:
    """Configuration for visualization tools"""
    output_dir: str = "./visualizations"
    width: int = 800
    height: int = 600
    fps: int = 30
    duration: float = 10.0
    theme: str = "dark"  # "dark" or "light"
    show_neurons: bool = True
    show_synapses: bool = True
    show_activity: bool = True
    colormap: str = "viridis"
    save_frames: bool = False
    generate_gif: bool = False

class BrainVisualizer:
    """Visualize brain activity and structure"""
    
    def __init__(self, config: Optional[VisualizationConfig] = None):
        self.config = config or VisualizationConfig()
        self.output_dir = Path(self.config.output_dir)
        self.output_dir.mkdir(exist_ok=True)
        
        # Set up matplotlib style
        if self.config.theme == "dark":
            plt.style.use('dark_background')
        else:
            plt.style.use('default')
    
    def create_brain_structure_plot(self, brain, save_path: Optional[str] = None):
        """Create a plot showing brain structure"""
        fig, axes = plt.subplots(2, 2, figsize=(self.config.width/100, self.config.height/100))
        fig.suptitle("NLM Brain Structure", fontsize=16)
        
        # Plot 1: Regions
        ax = axes[0, 0]
        if brain.getRegionCount() > 0:
            region_names = []
            neuron_counts = []
            
            for i, region_id in enumerate(brain.getRegionIds()):
                region = brain.getRegion(region_id)
                region_names.append(f"Region {i}")
                neuron_counts.append(region.getTotalNeuronCount())
            
            bars = ax.bar(region_names, neuron_counts, 
                         color=plt.cm.viridis(np.linspace(0, 1, len(region_names))))
            ax.set_title("Neurons per Region")
            ax.set_ylabel("Neuron Count")
            ax.tick_params(axis='x', rotation=45)
        
        # Plot 2: Neuron types
        ax = axes[0, 1]
        neuron_types = {
            "Excitatory": 0,
            "Inhibitory": 0,
            "Sensory": 0,
            "Motor": 0,
            "Internal": 0
        }
        
        # Count neuron types (simplified)
        for region_id in brain.getRegionIds():
            region = brain.getRegion(region_id)
            neurons = region.getAllNeurons()
            for neuron in neurons:
                # This is simplified - would need to check actual neuron types
                neuron_types["Internal"] += 1
        
        ax.pie(neuron_types.values(), labels=neuron_types.keys(), autopct='%1.1f%%')
        ax.set_title("Neuron Type Distribution")
        
        # Plot 3: Synapses
        ax = axes[1, 0]
        if brain.getRegionCount() > 0:
            total_synapses = brain.getTotalSynapseCount()
            ax.bar(["Synapses"], [total_synapses], color='coral')
            ax.set_title(f"Total Synapses: {total_synapses}")
            ax.set_ylabel("Count")
        
        # Plot 4: Activity metrics
        ax = axes[1, 1]
        activity_metrics = []
        metric_labels = []
        
        activity_metrics.append(brain.getTotalSpikeCount())
        metric_labels.append("Total Spikes")
        
        activity_metrics.append(brain.getFiringNeuronCount())
        metric_labels.append("Firing Neurons")
        
        activity_metrics.append(brain.getAverageFiringRate())
        metric_labels.append("Avg Firing Rate")
        
        if brain.getWorkingMemory():
            activity_metrics.append(brain.getWorkingMemory().getActiveTraces())
            metric_labels.append("Working Memory Traces")
        
        if brain.getEpisodicMemory():
            activity_metrics.append(brain.getEpisodicMemory().getEpisodeCount())
            metric_labels.append("Episodic Episodes")
        
        bars = ax.bar(metric_labels, activity_metrics, 
                     color=plt.cm.plasma(np.linspace(0, 1, len(metric_labels))))
        ax.set_title("Current Activity Metrics")
        ax.set_ylabel("Value")
        ax.tick_params(axis='x', rotation=45)
        
        # Format y-axis labels for readability
        for bar in bars:
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{height:.1f}', ha='center', va='bottom')
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
            plt.close()
            return save_path
        else:
            plt.show()
            plt.close()
            return None
    
    def create_activity_animation(self, brain, num_frames: int = 100, 
                                  save_path: Optional[str] = None):
        """Create an animation of brain activity over time"""
        fig, axes = plt.subplots(2, 2, figsize=(self.config.width/100, self.config.height/100))
        fig.suptitle("NLM Brain Activity Animation", fontsize=16)
        
        # Initialize plots
        plots = []
        for i in range(4):
            ax = axes[i//2, i%2]
            if i == 0:
                # Spike activity over time
                line, = ax.plot([], [], 'r-', linewidth=2, label='Spikes')
                ax.set_xlim(0, num_frames)
                ax.set_ylim(0, 100)
                ax.set_title("Spike Count Over Time")
                ax.set_xlabel("Time Step")
                ax.set_ylabel("Spike Count")
                ax.legend()
            elif i == 1:
                # Firing rate
                line, = ax.plot([], [], 'g-', linewidth=2, label='Firing Rate')
                ax.set_xlim(0, num_frames)
                ax.set_ylim(0, 20)
                ax.set_title("Average Firing Rate")
                ax.set_xlabel("Time Step")
                ax.set_ylabel("Firing Rate (Hz)")
                ax.legend()
            elif i == 2:
                # Active neurons
                line, = ax.plot([], [], 'b-', linewidth=2, label='Active Neurons')
                ax.set_xlim(0, num_frames)
                ax.set_ylim(0, brain.getTotalNeuronCount() if brain.getTotalNeuronCount() > 0 else 100)
                ax.set_title("Active Neurons Over Time")
                ax.set_xlabel("Time Step")
                ax.set_ylabel("Active Neurons")
                ax.legend()
            elif i == 3:
                # Synaptic weight distribution
                line, = ax.plot([], [], 'orange', linewidth=2, label='Weight Mean')
                ax.set_xlim(0, num_frames)
                ax.set_ylim(0, 2)
                ax.set_title("Average Synaptic Weight")
                ax.set_xlabel("Time Step")
                ax.set_ylabel("Weight")
                ax.legend()
            
            plots.append(line)
        
        # Animation data
        spike_data = deque(maxlen=num_frames)
        firing_rate_data = deque(maxlen=num_frames)
        active_neuron_data = deque(maxlen=num_frames)
        weight_data = deque(maxlen=num_frames)
        
        # Initial data
        for _ in range(num_frames):
            spike_data.append(0)
            firing_rate_data.append(0)
            active_neuron_data.append(0)
            weight_data.append(0)
        
        def update(frame):
            # Get current brain state
            spike_count = brain.getTotalSpikeCount()
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getFiringNeuronCount()
            
            # Update data (rolling average)
            spike_data[frame % num_frames] = spike_count
            firing_rate_data[frame % num_frames] = firing_rate
            active_neuron_data[frame % num_frames] = active_neurons
            
            # Update plots
            plots[0].set_data(range(len(spike_data)), list(spike_data))
            plots[1].set_data(range(len(firing_rate_data)), list(firing_rate_data))
            plots[2].set_data(range(len(active_neuron_data)), list(active_neuron_data))
            
            # Calculate average synaptic weight
            if brain.getRegionCount() > 0:
                region = brain.getRegion(brain.getRegionIds()[0])
                total_weight = 0.0
                weight_count = 0
                for syn in region.getSynapses():
                    if syn:
                        total_weight += syn.getWeight()
                        weight_count += 1
                avg_weight = total_weight / weight_count if weight_count > 0 else 0
                weight_data[frame % num_frames] = avg_weight
            
            plots[3].set_data(range(len(weight_data)), list(weight_data))
            
            # Update title with current metrics
            fig.suptitle(f"NLM Brain Activity Animation - Step {frame:04d} | "
                        f"Spikes: {spike_count}, Rate: {firing_rate:.2f}Hz, "
                        f"Active: {active_neurons}")
            
            return plots
        
        # Create animation
        anim = animation.FuncAnimation(fig, update, frames=num_frames,
                                       interval=1000/self.config.fps, blit=False)
        
        if save_path:
            # Save as GIF
            if self.config.generate_gif:
                anim.save(save_path, writer='pillow', fps=self.config.fps)
            # Save frames
            elif self.config.save_frames:
                for i in range(num_frames):
                    update(i)
                    frame_path = Path(save_path).parent / f"frame_{i:04d}.png"
                    plt.savefig(frame_path, dpi=150, bbox_inches='tight')
            
            plt.close()
            return save_path
        else:
            plt.show()
            plt.close()
            return None
    
    def create_comparison_plot(self, results: List[Dict], plot_type: str = "all"):
        """Create comparison plots for multiple experiment results"""
        if not results:
            return None
        
        fig, axes = plt.subplots(2, 2, figsize=(self.config.width/100, self.config.height/100))
        fig.suptitle("NLM Experiment Comparison", fontsize=16)
        
        # Extract data from results
        experiment_names = [r.get("experiment_type", f"Exp {i}") for i, r in enumerate(results)]
        
        if plot_type in ["all", "reward"]:
            ax = axes[0, 0]
            total_rewards = [r.get("total_reward", 0) for r in results]
            bars = ax.bar(experiment_names, total_rewards, 
                         color=plt.cm.viridis(np.linspace(0, 1, len(results))))
            ax.set_title("Total Reward Comparison")
            ax.set_ylabel("Total Reward")
            ax.tick_params(axis='x', rotation=45)
            
            # Add value labels
            for bar, value in zip(bars, total_rewards):
                ax.text(bar.get_x() + bar.get_width()/2., bar.get_height(),
                       f'{value:.2f}', ha='center', va='bottom')
        
        if plot_type in ["all", "firing_rate"]:
            ax = axes[0, 1]
            avg_rates = [r.get("avg_firing_rate", 0) for r in results]
            bars = ax.bar(experiment_names, avg_rates,
                         color=plt.cm.plasma(np.linspace(0, 1, len(results))))
            ax.set_title("Average Firing Rate Comparison")
            ax.set_ylabel("Firing Rate (Hz)")
            ax.tick_params(axis='x', rotation=45)
            
            # Add value labels
            for bar, value in zip(bars, avg_rates):
                ax.text(bar.get_x() + bar.get_width()/2., bar.get_height(),
                       f'{value:.2f}', ha='center', va='bottom')
        
        if plot_type in ["all", "episodes"]:
            ax = axes[1, 0]
            episodes = [r.get("memory_episodes_stored", 0) for r in results]
            bars = ax.bar(experiment_names, episodes,
                         color=plt.cm.coolwarm(np.linspace(0, 1, len(results))))
            ax.set_title("Memory Episodes Comparison")
            ax.set_ylabel("Episodes Stored")
            ax.tick_params(axis='x', rotation=45)
            
            # Add value labels
            for bar, value in zip(bars, episodes):
                ax.text(bar.get_x() + bar.get_width()/2., bar.get_height(),
                       f'{value:.0f}', ha='center', va='bottom')
        
        if plot_type in ["all", "dopamine"]:
            ax = axes[1, 1]
            dopamine_levels = [r.get("dopamine_level", 0) for r in results]
            bars = ax.bar(experiment_names, dopamine_levels,
                         color=plt.cm.magma(np.linspace(0, 1, len(results))))
            ax.set_title("Dopamine Level Comparison")
            ax.set_ylabel("Dopamine Level")
            ax.tick_params(axis='x', rotation=45)
            
            # Add value labels
            for bar, value in zip(bars, dopamine_levels):
                ax.text(bar.get_x() + bar.get_width()/2., bar.get_height(),
                       f'{value:.2f}', ha='center', va='bottom')
        
        plt.tight_layout()
        
        save_path = self.output_dir / "comparison_plot.png"
        plt.savefig(save_path, dpi=150, bbox_inches='tight')
        plt.close()
        
        return str(save_path)
    
    def create_time_series_plot(self, result: Dict, metrics: List[str], 
                               save_path: Optional[str] = None):
        """Create time series plot for experiment metrics"""
        fig, axes = plt.subplots(len(metrics), 1, figsize=(self.config.width/100, 
                                                           len(metrics)*self.config.height/200))
        fig.suptitle("NLM Experiment Time Series", fontsize=16)
        
        if len(metrics) == 1:
            axes = [axes]
        
        time_steps = list(range(len(next(iter(result.get("time_series", []), [])))))
        
        for i, metric in enumerate(metrics):
            ax = axes[i]
            if metric in result:
                data = result[metric]
                ax.plot(time_steps, data, linewidth=2)
                ax.set_title(metric.replace('_', ' ').title())
                ax.set_xlabel("Time Step")
                ax.set_ylabel("Value")
                ax.grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
            plt.close()
            return save_path
        else:
            plt.show()
            plt.close()
            return None

class BrainDebugger:
    """Debugging tools for NLM experiments"""
    
    def __init__(self):
        self.log_file = Path("nlm_debug.log")
        self.log_file.touch(exist_ok=True)
    
    def log_event(self, event_type: str, message: str, brain=None):
        """Log an event with brain state information"""
        timestamp = time.time()
        log_entry = {
            "timestamp": timestamp,
            "type": event_type,
            "message": message,
            "brain_state": self._capture_brain_state(brain) if brain else None
        }
        
        # Log to file
        with open(self.log_file, 'a') as f:
            json.dump(log_entry, f)
            f.write('\n')
        
        # Print to console
        print(f"[{timestamp:.2f}] {event_type}: {message}")
        
        return log_entry
    
    def _capture_brain_state(self, brain) -> Dict:
        """Capture current brain state for debugging"""
        state = {
            "region_count": brain.getRegionCount(),
            "neuron_count": brain.getTotalNeuronCount(),
            "synapse_count": brain.getTotalSynapseCount(),
            "spike_count": brain.getTotalSpikeCount(),
            "firing_rate": brain.getAverageFiringRate(),
            "active_neurons": brain.getFiringNeuronCount()
        }
        
        # Add memory system info
        if brain.getWorkingMemory():
            state["working_memory_traces"] = brain.getWorkingMemory().getActiveTraces()
        
        if brain.getEpisodicMemory():
            state["episodic_episodes"] = brain.getEpisodicMemory().getEpisodeCount()
        
        # Add neuromodulation info
        if brain.getDopamine():
            state["dopamine_level"] = brain.getDopamine().getLevel()
        
        return state
    
    def compare_brain_states(self, brain1, brain2, name1: str = "Brain 1", name2: str = "Brain 2"):
        """Compare two brain states"""
        state1 = self._capture_brain_state(brain1)
        state2 = self._capture_brain_state(brain2)
        
        differences = {}
        all_keys = set(state1.keys()) | set(state2.keys())
        
        for key in all_keys:
            val1 = state1.get(key, None)
            val2 = state2.get(key, None)
            
            if val1 != val2:
                differences[key] = {
                    name1: val1,
                    name2: val2,
                    "difference": val2 - val1 if isinstance(val1, (int, float)) and isinstance(val2, (int, float)) else None
                }
        
        return {
            "state1": state1,
            "state2": state2,
            "differences": differences,
            "similarity_score": len(set(state1.keys()) & set(state2.keys())) / len(all_keys) if all_keys else 0
        }
    
    def check_for_anomalies(self, brain, thresholds: Optional[Dict] = None):
        """Check brain state for potential anomalies"""
        if thresholds is None:
            thresholds = {
                "firing_rate": (0, 1000),  # Hz
                "spike_count": (0, 1000000),
                "active_neurons_ratio": (0, 1.0)
            }
        
        anomalies = {}
        total_neurons = brain.getTotalNeuronCount()
        
        if total_neurons > 0:
            firing_rate = brain.getAverageFiringRate()
            spike_count = brain.getTotalSpikeCount()
            active_neurons = brain.getFiringNeuronCount()
            active_ratio = active_neurons / total_neurons
            
            # Check firing rate
            if firing_rate < thresholds["firing_rate"][0] or firing_rate > thresholds["firing_rate"][1]:
                anomalies["firing_rate"] = {
                    "value": firing_rate,
                    "threshold_min": thresholds["firing_rate"][0],
                    "threshold_max": thresholds["firing_rate"][1],
                    "issue": "out of range"
                }
            
            # Check spike count
            if spike_count < thresholds["spike_count"][0] or spike_count > thresholds["spike_count"][1]:
                anomalies["spike_count"] = {
                    "value": spike_count,
                    "threshold_min": thresholds["spike_count"][0],
                    "threshold_max": thresholds["spike_count"][1],
                    "issue": "out of range"
                }
            
            # Check active neurons ratio
            if active_ratio < thresholds["active_neurons_ratio"][0] or active_ratio > thresholds["active_neurons_ratio"][1]:
                anomalies["active_neurons_ratio"] = {
                    "value": active_ratio,
                    "threshold_min": thresholds["active_neurons_ratio"][0],
                    "threshold_max": thresholds["active_neurons_ratio"][1],
                    "issue": "out of range"
                }
        
        return anomalies
    
    def save_debug_log(self, save_path: Optional[str] = None):
        """Save debug log to file"""
        if save_path:
            import shutil
            shutil.copy(self.log_file, save_path)
            return save_path
        else:
            return str(self.log_file)
    
    def load_debug_log(self, log_path: str) -> List[Dict]:
        """Load debug log from file"""
        loaded_log = []
        with open(log_path, 'r') as f:
            for line in f:
                loaded_log.append(json.loads(line.strip()))
        return loaded_log

class PerformanceProfiler:
    """Performance profiling tools for NLM experiments"""
    
    def __init__(self):
        self.profiling_data = []
        self.start_time = None
    
    def start_profiling(self, experiment_name: str = ""):
        """Start performance profiling"""
        self.start_time = time.time()
        self.profiling_data.append({
            "name": experiment_name,
            "start_time": self.start_time,
            "steps": []
        })
    
    def profile_step(self, step: int, brain=None, memory_usage: Optional[float] = None):
        """Profile a single step"""
        if not self.profiling_data:
            return
        
        current_profile = self.profiling_data[-1]
        step_data = {
            "step": step,
            "timestamp": time.time(),
            "brain_state": self._capture_brain_metrics(brain) if brain else None,
            "memory_usage": memory_usage
        }
        
        current_profile["steps"].append(step_data)
    
    def _capture_brain_metrics(self, brain) -> Dict:
        """Capture brain performance metrics"""
        metrics = {}
        
        if brain:
            metrics.update({
                "total_neurons": brain.getTotalNeuronCount(),
                "total_synapses": brain.getTotalSynapseCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_rate": brain.getAverageFiringRate(),
                "active_neurons": brain.getFiringNeuronCount(),
            })
            
            if brain.getWorkingMemory():
                metrics["working_memory_traces"] = brain.getWorkingMemory().getActiveTraces()
            
            if brain.getEpisodicMemory():
                metrics["episodic_episodes"] = brain.getEpisodicMemory().getEpisodeCount()
        
        return metrics
    
    def stop_profiling(self) -> Dict:
        """Stop performance profiling and return summary"""
        if not self.profiling_data:
            return {}
        
        current_profile = self.profiling_data[-1]
        end_time = time.time()
        
        summary = {
            "name": current_profile["name"],
            "start_time": current_profile["start_time"],
            "end_time": end_time,
            "duration": end_time - current_profile["start_time"],
            "total_steps": len(current_profile["steps"]),
            "steps_per_second": len(current_profile["steps"]) / (end_time - current_profile["start_time"]) if (end_time - current_profile["start_time"]) > 0 else 0
        }
        
        # Calculate metrics from steps
        if current_profile["steps"]:
            spike_counts = [step["brain_state"].get("total_spikes", 0) if step["brain_state"] else 0 
                          for step in current_profile["steps"]]
            firing_rates = [step["brain_state"].get("firing_rate", 0) if step["brain_state"] else 0 
                          for step in current_profile["steps"]]
            
            if spike_counts:
                summary["spike_count_range"] = (min(spike_counts), max(spike_counts))
                summary["avg_spike_count"] = sum(spike_counts) / len(spike_counts)
            
            if firing_rates:
                summary["firing_rate_range"] = (min(firing_rates), max(firing_rates))
                summary["avg_firing_rate"] = sum(firing_rates) / len(firing_rates)
        
        return summary
    
    def generate_performance_report(self, save_path: Optional[str] = None) -> str:
        """Generate a performance report"""
        if not self.profiling_data:
            return "No profiling data available"
        
        report_lines = []
        report_lines.append("NLM Performance Report")
        report_lines.append("=" * 50)
        
        for profile in self.profiling_data:
            report_lines.append(f"Experiment: {profile['name']}")
            report_lines.append(f"  Duration: {profile['end_time'] - profile['start_time']:.2f} seconds")
            report_lines.append(f"  Total Steps: {len(profile['steps'])}")
            report_lines.append(f"  Steps per Second: {len(profile['steps']) / (profile['end_time'] - profile['start_time']) if (profile['end_time'] - profile['start_time']) > 0 else 0:.2f}")
            
            if profile["steps"]:
                spike_counts = [step["brain_state"].get("total_spikes", 0) if step["brain_state"] else 0 
                              for step in profile["steps"]]
                if spike_counts:
                    report_lines.append(f"  Spike Count Range: {min(spike_counts)} - {max(spike_counts)}")
                    report_lines.append(f"  Average Spike Count: {sum(spike_counts) / len(spike_counts):.2f}")
            
            report_lines.append("")
        
        report = "\n".join(report_lines)
        
        if save_path:
            with open(save_path, 'w') as f:
                f.write(report)
        
        return report

def main():
    """Main entry point for NLM debugging and visualization tools"""
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Visualization and Debugging Tools")
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Visualize brain structure
    visual_parser = subparsers.add_parser("visualize", help="Visualize brain structure")
    visual_parser.add_argument("--brain-config", help="Brain configuration file")
    visual_parser.add_argument("--output-dir", default="./visualizations", help="Output directory")
    visual_parser.add_argument("--theme", choices=["dark", "light"], default="dark", help="Plot theme")
    visual_parser.add_argument("--save", action="store_true", help="Save visualizations")
    
    # Debug brain
    debug_parser = subparsers.add_parser("debug", help="Debug brain state")
    debug_parser.add_argument("--brain-config", help="Brain configuration file")
    debug_parser.add_argument("--log-file", help="Debug log file")
    
    # Profile performance
    profile_parser = subparsers.add_parser("profile", help="Profile performance")
    profile_parser.add_argument("--experiment-name", default="default", help="Experiment name")
    profile_parser.add_argument("--steps", type=int, default=100, help="Number of steps to profile")
    
    args = parser.parse_args()
    
    if args.command == "visualize":
        # Initialize visualizer
        config = VisualizationConfig(
            output_dir=args.output_dir,
            theme=args.theme,
            save_frames=args.save
        )
        
        visualizer = BrainVisualizer(config)
        
        # Load brain (simplified - would need proper brain loading)
        if NLM_AVAILABLE:
            cfg = nlm_module.createDefaultConfig()
            brain = nlm_module.createBrain(cfg)
            
            if brain.initialize():
                # Create visualizations
                visualizer.create_brain_structure_plot(brain)
                visualizer.create_activity_animation(brain, num_frames=args.steps)
        
    elif args.command == "debug":
        # Initialize debugger
        debugger = BrainDebugger()
        
        # Load brain (simplified - would need proper brain loading)
        if NLM_AVAILABLE:
            cfg = nlm_module.createDefaultConfig()
            brain1 = nlm_module.createBrain(cfg)
            
            if brain1.initialize():
                debugger.log_event("debug_start", f"Brain initialized with {brain1.getTotalNeuronCount()} neurons")
                
                # Check for anomalies
                anomalies = debugger.check_for_anomalies(brain1)
                if anomalies:
                    debugger.log_event("anomalies", f"Found anomalies: {anomalies}")
                
                # Save debug log
                debugger.save_debug_log()
    
    elif args.command == "profile":
        # Initialize profiler
        profiler = PerformanceProfiler()
        
        # Load brain (simplified - would need proper brain loading)
        if NLM_AVAILABLE:
            cfg = nlm_module.createDefaultConfig()
            brain = nlm_module.createBrain(cfg)
            
            if brain.initialize():
                # Start profiling
                profiler.start_profiling(args.experiment_name)
                
                # Profile steps
                for step in range(args.steps):
                    brain.step(step, step * 0.001)
                    profiler.profile_step(step, brain)
                
                # Stop profiling
                summary = profiler.stop_profiling()
                
                # Generate report
                report = profiler.generate_performance_report()
                print(report)

if __name__ == "__main__":
    main()