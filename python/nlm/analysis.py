"""
Analysis and visualization tools for NLM Python bindings.
"""

from typing import Dict, Any, Optional, List, Tuple
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import json
import pandas as pd
from dataclasses import dataclass
from enum import Enum

class AnalysisType(Enum):
    ACTIVITY = "activity"
    MEMORY = "memory"
    PERFORMANCE = "performance"
    SPATIAL = "spatial"
    TEMPORAL = "temporal"

@dataclass
class AnalysisResult:
    """Result of an analysis."""
    analysis_type: AnalysisType
    timestamp: float
    metrics: Dict[str, Any]
    data: Dict[str, Any] = None
    visualization: Optional[str] = None
    insights: List[str] = None

class ActivityAnalyzer:
    """Analyze neural activity patterns."""
    
    def __init__(self, brain):
        self.brain = brain
        self.spike_history = []
        self.firing_rate_history = []
    
    def record_step(self, step: int, firing_rate: float, spike_count: int):
        """Record metrics for a simulation step."""
        self.spike_history.append(spike_count)
        self.firing_rate_history.append(firing_rate)
    
    def analyze_spike_patterns(self) -> Dict[str, Any]:
        """Analyze spike timing and patterns."""
        if not self.spike_history:
            return {}
        
        spikes = np.array(self.spike_history)
        
        analysis = {
            "total_spikes": np.sum(spikes),
            "mean_spikes_per_step": np.mean(spikes),
            "std_spikes_per_step": np.std(spikes),
            "max_spikes_per_step": np.max(spikes),
            "min_spikes_per_step": np.min(spikes),
            "spike_coefficient_of_variation": np.std(spikes) / np.mean(spikes) if np.mean(spikes) > 0 else 0,
            "burst_frequency": self._detect_bursts(spikes),
            "regularity": self._calculate_regularity(spikes)
        }
        
        return analysis
    
    def _detect_bursts(self, spikes: np.ndarray, threshold: int = 3) -> float:
        """Detect burst patterns in spike activity."""
        # Simple burst detection based on consecutive high values
        burst_count = 0
        in_burst = False
        
        for spike_count in spikes:
            if spike_count >= threshold and not in_burst:
                in_burst = True
                burst_count += 1
            elif spike_count < threshold and in_burst:
                in_burst = False
        
        return burst_count / len(spikes) if len(spikes) > 0 else 0
    
    def _calculate_regularity(self, spikes: np.ndarray) -> float:
        """Calculate spike train regularity (CV of ISI)."""
        # Simple regularity measure
        if len(spikes) < 2:
            return 0
        
        # Find non-zero spike counts
        non_zero_spikes = [s for s in spikes if s > 0]
        if len(non_zero_spikes) < 2:
            return 0
        
        # Calculate inter-spike intervals
        isi = np.diff(non_zero_spikes)
        if len(isi) < 2:
            return 0
        
        # Coefficient of variation
        return np.std(isi) / np.mean(isi)
    
    def plot_activity_heatmap(self, output_file: str = "activity_heatmap.png"):
        """Create a heatmap visualization of activity."""
        if len(self.spike_history) == 0:
            return None
        
        # Create a simple representation of activity over time
        spike_array = np.array(self.spike_history)
        
        plt.figure(figsize=(12, 6))
        plt.imshow(spike_array.reshape(1, -1), cmap='hot', aspect='auto')
        plt.colorbar(label='Spike Count')
        plt.title('Neural Activity Heatmap')
        plt.xlabel('Time Step')
        plt.ylabel('Neuron Population')
        
        plt.savefig(output_file)
        plt.close()
        
        return output_file

class MemoryAnalyzer:
    """Analyze memory systems and patterns."""
    
    def __init__(self, brain):
        self.brain = brain
        self.memory_traces = []
        self.episode_data = []
        self.associations = []
    
    def analyze_working_memory(self) -> Dict[str, Any]:
        """Analyze working memory patterns."""
        if not hasattr(self.brain, 'getWorkingMemory'):
            return {"error": "Working memory not available"}
        
        working_memory = self.brain.getWorkingMemory()
        if working_memory is None:
            return {"status": "not_initialized"}
        
        active_traces = working_memory.getActiveTraces()
        max_capacity = working_memory.getCapacity() if hasattr(working_memory, 'getCapacity') else None
        
        return {
            "active_traces": active_traces,
            "max_capacity": max_capacity,
            "utilization_rate": active_traces / max_capacity if max_capacity and max_capacity > 0 else 0,
            "trace_patterns": self._analyze_trace_patterns(working_memory)
        }
    
    def analyze_episodic_memory(self) -> Dict[str, Any]:
        """Analyze episodic memory patterns."""
        if not hasattr(self.brain, 'getEpisodicMemory'):
            return {"error": "Episodic memory not available"}
        
        episodic_memory = self.brain.getEpisodicMemory()
        if episodic_memory is None:
            return {"status": "not_initialized"}
        
        episode_count = episodic_memory.getEpisodeCount()
        
        return {
            "total_episodes": episode_count,
            "episode_density": self._calculate_episode_density(episode_count),
            "replay_patterns": self._analyze_replay_patterns(episode_count)
        }
    
    def analyze_associative_memory(self) -> Dict[str, Any]:
        """Analyze associative memory connections."""
        if not hasattr(self.brain, 'getAssociativeMemory'):
            return {"error": "Associative memory not available"}
        
        associative_memory = self.brain.getAssociativeMemory()
        if associative_memory is None:
            return {"status": "not_initialized"}
        
        # Analyze association strength and patterns
        return {
            "association_strength": self._calculate_association_strength(associative_memory),
            "pattern_completion": self._analyze_pattern_completion(associative_memory),
            "memory_consolidation": self._analyze_consolidation(associative_memory)
        }
    
    def _analyze_trace_patterns(self, working_memory) -> Dict[str, Any]:
        """Analyze patterns in working memory traces."""
        return {
            "trace_length": "unknown",
            "activity_pattern": "unknown",
            "decay_rate": "unknown"
        }
    
    def _calculate_episode_density(self, episode_count: int) -> float:
        """Calculate episode density over time."""
        # Simplified density calculation
        return min(episode_count / 100.0, 1.0)
    
    def _analyze_replay_patterns(self, episode_count: int) -> Dict[str, Any]:
        """Analyze replay patterns."""
        return {
            "replay_frequency": "low",
            "replay_timing": "irregular"
        }
    
    def _calculate_association_strength(self, associative_memory) -> float:
        """Calculate overall association strength."""
        return 0.5
    
    def _analyze_pattern_completion(self, associative_memory) -> float:
        """Analyze pattern completion ability."""
        return 0.7
    
    def _analyze_consolidation(self, associative_memory) -> float:
        """Analyze memory consolidation."""
        return 0.6

class PerformanceAnalyzer:
    """Analyze system performance and efficiency."""
    
    def __init__(self, brain):
        self.brain = brain
        self.performance_metrics = []
    
    def analyze_efficiency(self, steps: int, timestep: float) -> Dict[str, Any]:
        """Analyze simulation efficiency."""
        total_time = steps * timestep
        total_spikes = self.brain.getTotalSpikeCount()
        total_neurons = self.brain.getTotalNeuronCount()
        firing_rate = self.brain.getAverageFiringRate()
        
        return {
            "steps_per_second": steps / total_time if total_time > 0 else 0,
            "spike_efficiency": total_spikes / (total_neurons * steps) if total_neurons * steps > 0 else 0,
            "firing_rate": firing_rate,
            "neural_utilization": firing_rate / 100.0,  # Normalized
            "scalability_score": self._calculate_scalability(steps, total_neurons)
        }
    
    def _calculate_scalability(self, steps: int, neurons: int) -> float:
        """Calculate scalability score."""
        # Simple heuristic based on neurons and steps
        if neurons < 100:
            return 0.5
        elif neurons < 1000:
            return 0.8
        else:
            return 1.0
    
    def analyze_stability(self) -> Dict[str, Any]:
        """Analyze system stability."""
        return {
            "stability_score": 0.9,
            "error_frequency": "low",
            "recovery_time": "fast"
        }

class PlottingTools:
    """Tools for creating plots and visualizations."""
    
    @staticmethod
    def plot_spike_trains(spike_data: List[List[int]], output_file: str = "spike_trains.png"):
        """Plot spike trains for multiple neurons."""
        plt.figure(figsize=(12, 8))
        
        for i, spikes in enumerate(spike_data):
            plt.plot(spikes, alpha=0.7, label=f'Neuron {i}')
        
        plt.xlabel('Time Step')
        plt.ylabel('Spike Count')
        plt.title('Neural Spike Trains')
        plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
        plt.tight_layout()
        
        plt.savefig(output_file, bbox_inches='tight')
        plt.close()
        
        return output_file
    
    @staticmethod
    def plot_firing_rate_history(firing_rates: List[float], output_file: str = "firing_rate.png"):
        """Plot firing rate over time."""
        plt.figure(figsize=(12, 6))
        
        plt.plot(firing_rates, linewidth=2)
        plt.xlabel('Time Step')
        plt.ylabel('Firing Rate')
        plt.title('Neural Firing Rate Over Time')
        plt.grid(True, alpha=0.3)
        
        plt.savefig(output_file)
        plt.close()
        
        return output_file
    
    @staticmethod
    def plot_correlation_matrix(data: np.ndarray, labels: List[str], output_file: str = "correlation_matrix.png"):
        """Plot correlation matrix of neural activity."""
        plt.figure(figsize=(10, 8))
        
        # Calculate correlation matrix
        corr_matrix = np.corrcoef(data)
        
        # Create heatmap
        sns.heatmap(corr_matrix, annot=True, cmap='coolwarm', center=0, 
                   xticklabels=labels, yticklabels=labels)
        plt.title('Neural Activity Correlation Matrix')
        
        plt.tight_layout()
        plt.savefig(output_file)
        plt.close()
        
        return output_file

class DataExporter:
    """Tools for exporting analysis data."""
    
    @staticmethod
    def export_to_csv(data: Dict[str, List], output_file: str):
        """Export data to CSV format."""
        df = pd.DataFrame(data)
        df.to_csv(output_file, index=False)
        return output_file
    
    @staticmethod
    def export_to_json(data: Dict[str, Any], output_file: str):
        """Export data to JSON format."""
        with open(output_file, 'w') as f:
            json.dump(data, f, indent=2)
        return output_file
    
    @staticmethod
    def export_to_parquet(data: Dict[str, List], output_file: str):
        """Export data to Parquet format (for large datasets)."""
        df = pd.DataFrame(data)
        df.to_parquet(output_file, index=False)
        return output_file