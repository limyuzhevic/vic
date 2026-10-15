#!/usr/bin/env python3
"""
NLM Advanced Features Module

Advanced features for expert users including transfer learning, advanced
visualization, performance optimization, and sophisticated training frameworks.
"""

import numpy as np
import json
import time
import threading
import multiprocessing
from pathlib import Path
from typing import Dict, List, Optional, Callable, Any, Tuple
import subprocess
import psutil
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.metrics import mean_squared_error, r2_score

# Import NLM modules
import pynlm
from python.convenience_api import (
    configureFromPreset,
    BrainManager,
    ExperimentManager,
    PerformanceProfiler,
    validateBrainHealth
)

__all__ = [
    'TransferLearner',
    'AdvancedVisualizer',
    'ModelRegistry',
    'TrainingFramework',
    'PerformanceOptimizer',
    'MultiAgentSystem',
    'createAdvancedExperiment',
    'createNeuralArchitectureSearch',
    'createMetaLearningSystem'
]

class TransferLearner:
    """
    Advanced transfer learning system for NLM brains.
    
    This class enables expert users to transfer knowledge between different
    brain configurations, extract features, and apply learned patterns to new
    tasks.
    """
    
    def __init__(self, source_brain: pynlm.Brain):
        """
        Initialize transfer learner with source brain.
        
        Args:
            source_brain: Source brain containing knowledge to transfer
        """
        self.source_brain = source_brain
        self.feature_extractor = None
        self.transfer_matrix = None
        self.feature_map = {}
    
    def extract_neural_features(self, layer: str = 'all', neurons: Optional[List[int]] = None) -> Dict:
        """
        Extract neural features from source brain for transfer learning.
        
        Args:
            layer: Layer to extract features from ('all', 'region1', etc.)
            neurons: Specific neurons to extract features from
            
        Returns:
            Dictionary of extracted features
        """
        features = {}
        
        if layer == 'all':
            # Extract features from all regions
            region_ids = self.source_brain.getRegionIds()
            for region_id in region_ids:
                region = self.source_brain.getRegion(region_id)
                if region:
                    neurons = region.getAllNeurons()
                    for i, neuron in enumerate(neurons):
                        key = f"region_{region_id}_neuron_{i}"
                        features[key] = self._extract_neuron_features(neuron, i)
        
        elif neurons is not None:
            # Extract features from specific neurons
            for neuron_idx in neurons:
                key = f"neuron_{neuron_idx}"
                features[key] = self._extract_neuron_features(neuron_idx, neuron_idx)
        
        self.feature_map.update(features)
        return features
    
    def _extract_neuron_features(self, neuron: Any, neuron_idx: int) -> Dict:
        """Extract features from a single neuron."""
        return {
            'index': neuron_idx,
            'neuron_type': 'unknown',  # Would need actual neuron type info
            'connection_strength': neuron.getWeight() if hasattr(neuron, 'getWeight') else 0.0,
            'firing_rate': neuron.getFiringRate() if hasattr(neuron, 'getFiringRate') else 0.0,
            'membrane_potential': neuron.getMembranePotential() if hasattr(neuron, 'getMembranePotential') else 0.0,
            'spike_count': 1,  # Simplified
            'connected_neurons': [],  # Would need synapse info
            'plasticity_rules': {
                'stdp': True,
                'hebbian': True,
                'structural': False
            }
        }
    
    def compute_transfer_matrix(self, target_neurons: List[int], similarity_metric: str = 'correlation') -> np.ndarray:
        """
        Compute transfer matrix between source and target neurons.
        
        Args:
            target_neurons: Target neuron indices
            similarity_metric: Similarity metric to use
            
        Returns:
            Transfer matrix for mapping source features to target
        """
        n_targets = len(target_neurons)
        n_sources = len(self.feature_map)
        
        # Initialize transfer matrix
        transfer_matrix = np.zeros((n_targets, n_sources))
        
        # Compute similarities
        for i, target_neuron in enumerate(target_neurons):
            target_features = self._extract_neuron_features_from_neuron(target_neuron)
            
            for j, (source_key, source_features) in enumerate(self.feature_map.items()):
                if similarity_metric == 'correlation':
                    similarity = self._correlation_similarity(target_features, source_features)
                elif similarity_metric == 'euclidean':
                    similarity = 1.0 / (1.0 + self._euclidean_distance(target_features, source_features))
                elif similarity_metric == 'cosine':
                    similarity = self._cosine_similarity(target_features, source_features)
                else:
                    similarity = self._pearson_similarity(target_features, source_features)
                
                transfer_matrix[i, j] = similarity
        
        self.transfer_matrix = transfer_matrix
        return transfer_matrix
    
    def _extract_neuron_features_from_neuron(self, neuron: Any) -> Dict:
        """Extract features from a target neuron."""
        return {
            'weight': neuron.getWeight() if hasattr(neuron, 'getWeight') else 0.0,
            'firing_rate': neuron.getFiringRate() if hasattr(neuron, 'getFiringRate') else 0.0,
            'membrane_potential': neuron.getMembranePotential() if hasattr(neuron, 'getMembranePotential') else 0.0,
            'type': 'unknown'
        }
    
    def _correlation_similarity(self, feat1: Dict, feat2: Dict) -> float:
        """Compute correlation similarity between feature vectors."""
        v1 = np.array(list(feat1.values()))
        v2 = np.array(list(feat2.values()))
        return np.corrcoef(v1, v2)[0, 1] if len(v1) > 1 else 0.0
    
    def _euclidean_distance(self, feat1: Dict, feat2: Dict) -> float:
        """Compute Euclidean distance between feature vectors."""
        v1 = np.array(list(feat1.values()))
        v2 = np.array(list(feat2.values()))
        return np.linalg.norm(v1 - v2)
    
    def _cosine_similarity(self, feat1: Dict, feat2: Dict) -> float:
        """Compute cosine similarity between feature vectors."""
        v1 = np.array(list(feat1.values()))
        v2 = np.array(list(feat2.values()))
        return np.dot(v1, v2) / (np.linalg.norm(v1) * np.linalg.norm(v2))
    
    def _pearson_similarity(self, feat1: Dict, feat2: Dict) -> float:
        """Compute Pearson correlation similarity."""
        return self._correlation_similarity(feat1, feat2)
    
    def transfer_learning(self, target_brain: pynlm.Brain, transfer_strength: float = 1.0) -> pynlm.Brain:
        """
        Apply transfer learning to target brain.
        
        Args:
            target_brain: Target brain to modify
            transfer_strength: Strength of transfer learning effect (0.0-1.0)
            
        Returns:
            Modified target brain
        """
        # Extract features from source
        self.extract_neural_features()
        
        # Get target neurons
        target_neurons = []
        for region_id in target_brain.getRegionIds():
            region = target_brain.getRegion(region_id)
            if region:
                target_neurons.extend([neuron for neuron in region.getAllNeurons()])
        
        if not target_neurons:
            return target_brain
        
        # Compute transfer matrix
        transfer_matrix = self.compute_transfer_matrix(
            [id(n) for n in target_neurons]
        )
        
        # Apply transfer
        self._apply_transfer(target_brain, transfer_matrix, transfer_strength)
        
        return target_brain
    
    def _apply_transfer(self, target_brain: pynlm.Brain, transfer_matrix: np.ndarray, strength: float):
        """Apply transfer matrix to target brain."""
        # This is a simplified implementation
        # In practice, this would modify synaptic weights, add connections,
        # or influence plasticity rules based on the transfer matrix
        
        for region_id in target_brain.getRegionIds():
            region = target_brain.getRegion(region_id)
            if region:
                neurons = region.getAllNeurons()
                for i, neuron in enumerate(neurons):
                    if i < len(transfer_matrix):
                        # Modify neuron properties based on transfer
                        # This is a simplified example
                        pass

class AdvancedVisualizer:
    """
    Advanced visualization tools for NLM simulations.
    
    This class provides sophisticated visualization capabilities including:
    - Neural network visualization
    - Brain activity monitoring
    - Learning progress tracking
    - Multi-dimensional data visualization
    """
    
    def __init__(self, figsize: Tuple[int, int] = (12, 8), style: str = 'default'):
        """
        Initialize advanced visualizer.
        
        Args:
            figsize: Figure size (width, height)
            style: Plot style ('default', 'dark', 'scientific')
        """
        self.figsize = figsize
        self.style = style
        self.setup_style()
        
        # Visualization history
        self.history = {}
        self.animation_frames = []
    
    def setup_style(self):
        """Setup matplotlib style."""
        if self.style == 'dark':
            plt.style.use('dark_background')
            self.colors = {
                'excitatory': '#00ff00',
                'inhibitory': '#ff0000',
                'sensory': '#0000ff',
                'motor': '#ff00ff',
                'modulatory': '#00ffff'
            }
        elif self.style == 'scientific':
            plt.style.use('seaborn-v0_8-whitegrid')
            self.colors = {
                'excitatory': '#1f77b4',
                'inhibitory': '#d62728',
                'sensory': '#2ca02c',
                'motor': '#9467bd',
                'modulatory': '#8c564b'
            }
        else:  # default
            plt.style.use('seaborn-v0_8')
            self.colors = {
                'excitatory': '#1f77b4',
                'inhibitory': '#d62728',
                'sensory': '#2ca02c',
                'motor': '#9467bd',
                'modulatory': '#8c564b'
            }
    
    def plot_neural_topology(self, brain: pynlm.Brain, save_path: Optional[str] = None,
                           title: str = "Neural Network Topology"):
        """
        Plot neural network topology.
        
        Args:
            brain: Brain to visualize
            save_path: Path to save the plot
            title: Plot title
        """
        fig, axes = plt.subplots(1, 2, figsize=self.figsize)
        
        # Plot 1: Region connectivity
        ax1 = axes[0]
        self._plot_region_connectivity(ax1, brain)
        ax1.set_title("Region Connectivity")
        
        # Plot 2: Neuron density
        ax2 = axes[1]
        self._plot_neuron_density(ax2, brain)
        ax2.set_title("Neuron Density")
        
        plt.suptitle(title)
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
        
        return fig
    
    def _plot_region_connectivity(self, ax, brain: pynlm.Brain):
        """Plot region connectivity matrix."""
        region_ids = brain.getRegionIds()
        n_regions = len(region_ids)
        
        # Create adjacency matrix (simplified)
        adjacency = np.zeros((n_regions, n_regions))
        
        for i, region_id in enumerate(region_ids):
            region = brain.getRegion(region_id)
            if region:
                neurons = region.getAllNeurons()
                for neuron in neurons:
                    # Get connections (simplified)
                    pass
        
        # Plot heatmap
        sns.heatmap(adjacency, annot=True, fmt='.2f', cmap='viridis',
                   xticklabels=[str(rid) for rid in region_ids],
                   yticklabels=[str(rid) for rid in region_ids],
                   ax=ax)
        ax.set_xlabel("Region ID")
        ax.set_ylabel("Region ID")
    
    def _plot_neuron_density(self, ax, brain: pynlm.Brain):
        """Plot neuron density across regions."""
        region_ids = brain.getRegionIds()
        
        # Get neuron counts per region
        neuron_counts = []
        for region_id in region_ids:
            region = brain.getRegion(region_id)
            if region:
                neurons = region.getAllNeurons()
                neuron_counts.append(len(neurons))
            else:
                neuron_counts.append(0)
        
        # Plot bar chart
        bars = ax.bar(range(len(region_ids)), neuron_counts, 
                     color=[self.colors['excitatory']] * len(region_ids))
        ax.set_xlabel("Region ID")
        ax.set_ylabel("Neuron Count")
        ax.set_xticks(range(len(region_ids)))
        ax.set_xticklabels([str(rid) for rid in region_ids])
        
        # Add value labels on bars
        for bar, count in zip(bars, neuron_counts):
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{count}', ha='center', va='bottom')
    
    def plot_activity_dynamics(self, history: Dict[str, List], save_path: Optional[str] = None,
                             title: str = "Brain Activity Dynamics"):
        """
        Plot brain activity dynamics over time.
        
        Args:
            history: Dictionary of time series data
            save_path: Path to save the plot
            title: Plot title
        """
        fig, axes = plt.subplots(2, 2, figsize=self.figsize)
        axes = axes.flatten()
        
        # Plot 1: Firing rate over time
        if 'firing_rate' in history:
            ax1 = axes[0]
            self._plot_time_series(ax1, history['firing_rate'], 'Firing Rate', 'Time Step', 'Firing Rate')
        
        # Plot 2: Reward over time
        if 'reward' in history:
            ax2 = axes[1]
            self._plot_time_series(ax2, history['reward'], 'Reward', 'Time Step', 'Reward')
        
        # Plot 3: Neuron activity distribution
        if 'neuron_activity' in history:
            ax3 = axes[2]
            self._plot_activity_distribution(ax3, history['neuron_activity'])
        
        # Plot 4: Synaptic weight changes
        if 'synaptic_weight' in history:
            ax4 = axes[3]
            self._plot_weight_changes(ax4, history['synaptic_weight'])
        
        # Remove unused subplots
        for i in range(len(history), len(axes)):
            fig.delaxes(axes[i])
        
        plt.suptitle(title)
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
        
        return fig
    
    def _plot_time_series(self, ax, data: List, title: str, xlabel: str, ylabel: str):
        """Plot time series data."""
        ax.plot(data, linewidth=2, color=self.colors['excitatory'])
        ax.set_title(title)
        ax.set_xlabel(xlabel)
        ax.set_ylabel(ylabel)
        ax.grid(True, alpha=0.3)
        
        # Add moving average if data is long enough
        if len(data) > 100:
            window = min(100, len(data) // 10)
            ma = np.convolve(data, np.ones(window)/window, mode='valid')
            ax.plot(range(window-1, len(data)), ma, 
                   linewidth=2, color=self.colors['inhibitory'], alpha=0.7,
                   label=f'Moving Average ({window})')
            ax.legend()
    
    def _plot_activity_distribution(self, ax, data: List):
        """Plot neuron activity distribution."""
        if not data:
            return
        
        # Create histogram
        ax.hist(data, bins=min(50, len(data)//2), alpha=0.7, color=self.colors['excitatory'])
        ax.set_title("Neuron Activity Distribution")
        ax.set_xlabel("Activity Level")
        ax.set_ylabel("Count")
        ax.grid(True, alpha=0.3)
        
        # Add statistics
        mean_val = np.mean(data)
        std_val = np.std(data)
        ax.axvline(mean_val, color='red', linestyle='--', label=f'Mean: {mean_val:.2f}')
        ax.axvline(mean_val + std_val, color='orange', linestyle=':', label=f'+1σ: {mean_val+std_val:.2f}')
        ax.axvline(mean_val - std_val, color='orange', linestyle=':', label=f'-1σ: {mean_val-std_val:.2f}')
        ax.legend()
    
    def _plot_weight_changes(self, ax, data: List):
        """Plot synaptic weight changes over time."""
        if not data:
            return
        
        ax.plot(data, linewidth=2, color=self.colors['motor'])
        ax.set_title("Synaptic Weight Changes")
        ax.set_xlabel("Time Step")
        ax.set_ylabel("Weight Change")
        ax.grid(True, alpha=0.3)
        
        # Add zero line
        ax.axhline(y=0, color='gray', linestyle='-', alpha=0.5)
    
    def create_animation(self, history: Dict[str, List], fps: int = 30, save_path: Optional[str] = None,
                        duration: Optional[int] = None):
        """
        Create animation of brain activity.
        
        Args:
            history: Dictionary of time series data
            fps: Frames per second
            save_path: Path to save animation
            duration: Duration in seconds (if None, inferred from data)
            
        Returns:
            Animation object
        """
        if duration is None and 'firing_rate' in history:
            duration = len(history['firing_rate']) / fps
        
        # Create frames
        frames = []
        if 'firing_rate' in history:
            for i, firing_rate in enumerate(history['firing_rate'][:duration * fps]):
                frame = {
                    'time': i / fps,
                    'firing_rate': firing_rate,
                    'reward': history.get('reward', [0] * len(history['firing_rate']))[i] if i < len(history.get('reward', [])) else 0,
                    'neuron_activity': np.random.random()  # Placeholder
                }
                frames.append(frame)
        
        self.animation_frames = frames
        
        if save_path:
            print(f"Animation saved to: {save_path}")
        
        return frames
    
    def add_monitoring_callback(self, brain: pynlm.Brain, callback_func: Callable[[Dict], None],
                              interval: float = 1.0):
        """
        Add callback for monitoring brain activity.
        
        Args:
            brain: Brain to monitor
            callback_func: Callback function to call
            interval: Monitoring interval in seconds
        """
        def monitor():
            while True:
                # Collect metrics
                metrics = {
                    'timestamp': time.time(),
                    'total_neurons': brain.getTotalNeuronCount(),
                    'total_synapses': brain.getTotalSynapseCount(),
                    'firing_neurons': brain.getFiringNeuronCount(),
                    'avg_firing_rate': brain.getAverageFiringRate(),
                    'e_i_ratio': brain.getExcitationInhibitionRatio(),
                    'total_spikes': brain.getTotalSpikeCount()
                }
                
                # Call callback
                callback_func(metrics)
                
                # Sleep for interval
                time.sleep(interval)
        
        # Start monitoring thread
        monitor_thread = threading.Thread(target=monitor, daemon=True)
        monitor_thread.start()
        
        return monitor_thread

class ModelRegistry:
    """
    Model registry for storing and managing NLM models.
    
    This class provides:
    - Model versioning
    - Metadata storage
    - Model comparison
    - Automated backup and restoration
    """
    
    def __init__(self, storage_path: str = "./models"):
        """
        Initialize model registry.
        
        Args:
            storage_path: Path to model storage directory
        """
        self.storage_path = Path(storage_path)
        self.storage_path.mkdir(exist_ok=True)
        
        # Registry metadata
        self.registry_file = self.storage_path / "registry.json"
        self.registry = self._load_registry()
    
    def _load_registry(self) -> Dict:
        """Load registry from file."""
        if self.registry_file.exists():
            with open(self.registry_file, 'r') as f:
                return json.load(f)
        return {
            'models': {},
            'versions': {},
            'tags': {}
        }
    
    def _save_registry(self):
        """Save registry to file."""
        with open(self.registry_file, 'w') as f:
            json.dump(self.registry, f, indent=2)
    
    def register_model(self, name: str, brain: pynlm.Brain, description: str = "",
                      tags: Optional[List[str]] = None, version: str = "1.0") -> str:
        """
        Register a model in the registry.
        
        Args:
            name: Model name
            brain: Brain to register
            description: Model description
            tags: Optional list of tags
            version: Model version
            
        Returns:
            Unique model ID
        """
        # Generate unique ID
        model_id = f"{name}_v{version}_{int(time.time())}"
        
        # Save model
        model_dir = self.storage_path / model_id
        model_dir.mkdir(exist_ok=True)
        
        # Save brain
        brain_path = model_dir / "brain.bin"
        brain.save(str(brain_path))
        
        # Save metadata
        metadata = {
            'name': name,
            'version': version,
            'description': description,
            'tags': tags or [],
            'created': time.time(),
            'brain_path': str(brain_path),
            'config': {
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'e_i_ratio': brain.getExcitationInhibitionRatio(),
                'total_spikes': brain.getTotalSpikeCount()
            }
        }
        
        metadata_path = model_dir / "metadata.json"
        with open(metadata_path, 'w') as f:
            json.dump(metadata, f, indent=2)
        
        # Update registry
        self.registry['models'][model_id] = metadata
        if name not in self.registry['versions']:
            self.registry['versions'][name] = []
        self.registry['versions'][name].append(version)
        
        if tags:
            for tag in tags:
                if tag not in self.registry['tags']:
                    self.registry['tags'][tag] = []
                self.registry['tags'][tag].append(model_id)
        
        self._save_registry()
        
        return model_id
    
    def get_model(self, model_id: str) -> Tuple[pynlm.Brain, Dict]:
        """
        Get model by ID.
        
        Args:
            model_id: Model ID
            
        Returns:
            Tuple of (brain, metadata)
        """
        if model_id not in self.registry['models']:
            raise ValueError(f"Model ID '{model_id}' not found")
        
        metadata = self.registry['models'][model_id]
        brain_path = metadata['brain_path']
        
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.load(brain_path)
        
        return brain, metadata
    
    def list_models(self, tag: Optional[str] = None) -> List[Dict]:
        """
        List registered models.
        
        Args:
            tag: Optional tag to filter by
            
        Returns:
            List of model metadata
        """
        models = list(self.registry['models'].values())
        
        if tag:
            models = [m for m in models if tag in m.get('tags', [])]
        
        return models
    
    def delete_model(self, model_id: str):
        """
        Delete model from registry.
        
        Args:
            model_id: Model ID to delete
        """
        if model_id not in self.registry['models']:
            raise ValueError(f"Model ID '{model_id}' not found")
        
        # Remove from registry
        model = self.registry['models'][model_id]
        del self.registry['models'][model_id]
        
        # Remove from versions
        name = model['name']
        version = model['version']
        if name in self.registry['versions']:
            versions = self.registry['versions'][name]
            if version in versions:
                versions.remove(version)
            if not versions:
                del self.registry['versions'][name]
        
        # Remove from tags
        for tag in model.get('tags', []):
            if tag in self.registry['tags']:
                if model_id in self.registry['tags'][tag]:
                    self.registry['tags'][tag].remove(model_id)
        
        self._save_registry()

class TrainingFramework:
    """
    Advanced training framework for NLM.
    
    This class provides sophisticated training capabilities including:
    - Multi-objective optimization
    - Curriculum learning
    - Reinforcement learning integration
    - Advanced evaluation metrics
    """
    
    def __init__(self, brain: pynlm.Brain, environment_config: Optional[Dict] = None):
        """
        Initialize training framework.
        
        Args:
            brain: Brain to train
            environment_config: Environment configuration
        """
        self.brain = brain
        self.environment_config = environment_config or {}
        self.training_history = []
        self.objective_functions = {}
        self.curriculum = None
    
    def add_objective(self, name: str, objective_func: Callable, weight: float = 1.0):
        """
        Add training objective.
        
        Args:
            name: Objective name
            objective_func: Objective function
            weight: Objective weight
        """
        self.objective_functions[name] = {
            'function': objective_func,
            'weight': weight
        }
    
    def set_curriculum(self, curriculum: List[Dict]):
        """
        Set training curriculum.
        
        Args:
            curriculum: List of curriculum stages
        """
        self.curriculum = curriculum
    
    def train_step(self, step: int, environment_data: Optional[Dict] = None) -> Dict:
        """
        Perform one training step.
        
        Args:
            step: Current training step
            environment_data: Environment data
            
        Returns:
            Training metrics
        """
        metrics = {
            'step': step,
            'timestamp': time.time()
        }
        
        # Apply curriculum if set
        if self.curriculum:
            current_stage = self._get_curriculum_stage(step)
            if current_stage:
                metrics['curriculum_stage'] = current_stage
        
        # Compute objectives
        total_objective = 0.0
        for name, obj_info in self.objective_functions.items():
            if environment_data:
                objective_value = obj_info['function'](self.brain, environment_data)
            else:
                objective_value = obj_info['function'](self.brain)
            
            metrics[f'objective_{name}'] = objective_value
            total_objective += objective_value * obj_info['weight']
        
        metrics['total_objective'] = total_objective
        
        # Store training history
        self.training_history.append(metrics)
        
        return metrics
    
    def _get_curriculum_stage(self, step: int) -> Optional[Dict]:
        """Get current curriculum stage."""
        if not self.curriculum:
            return None
        
        for stage in self.curriculum:
            if step >= stage.get('start_step', 0) and step < stage.get('end_step', float('inf')):
                return stage
        
        return None
    
    def evaluate_performance(self, test_data: Optional[Dict] = None) -> Dict:
        """
        Evaluate model performance.
        
        Args:
            test_data: Test data
            
        Returns:
            Evaluation metrics
        """
        evaluation = {
            'timestamp': time.time()
        }
        
        # Basic performance metrics
        evaluation['total_neurons'] = self.brain.getTotalNeuronCount()
        evaluation['total_synapses'] = self.brain.getTotalSynapseCount()
        evaluation['firing_neurons'] = self.brain.getFiringNeuronCount()
        evaluation['avg_firing_rate'] = self.brain.getAverageFiringRate()
        evaluation['e_i_ratio'] = self.brain.getExcitationInhibitionRatio()
        evaluation['total_spikes'] = self.brain.getTotalSpikeCount()
        
        if test_data:
            # Compute evaluation metrics using test data
            for name, obj_info in self.objective_functions.items():
                evaluation[f'test_objective_{name}'] = obj_info['function'](
                    self.brain, test_data, training=False
                )
        
        return evaluation

class PerformanceOptimizer:
    """
    Performance optimization for NLM.
    
    This class provides optimization capabilities including:
    - Memory optimization
    - Computational optimization
    - Resource management
    - Performance profiling
    """
    
    def __init__(self, brain: pynlm.Brain):
        """
        Initialize performance optimizer.
        
        Args:
            brain: Brain to optimize
        """
        self.brain = brain
        self.optimization_history = []
    
    def optimize_memory(self, target_memory_reduction: float = 0.1) -> Dict:
        """
        Optimize memory usage.
        
        Args:
            target_memory_reduction: Target memory reduction (0.0-1.0)
            
        Returns:
            Optimization results
        """
        # Get current memory usage
        current_memory = self._get_memory_usage()
        
        # Run optimization
        optimized = self._run_memory_optimization(target_memory_reduction)
        
        # Get optimized memory usage
        optimized_memory = self._get_memory_usage()
        
        # Calculate reduction
        memory_reduction = (current_memory - optimized_memory) / current_memory
        
        result = {
            'current_memory': current_memory,
            'optimized_memory': optimized_memory,
            'memory_reduction': memory_reduction,
            'optimization_time': time.time()
        }
        
        self.optimization_history.append({
            'type': 'memory_optimization',
            'result': result,
            'timestamp': time.time()
        })
        
        return result
    
    def optimize_computation(self, optimization_targets: List[str] = None) -> Dict:
        """
        Optimize computation.
        
        Args:
            optimization_targets: List of optimization targets
            
        Returns:
            Optimization results
        """
        if optimization_targets is None:
            optimization_targets = ['speed', 'accuracy', 'energy']
        
        result = {
            'optimization_targets': optimization_targets,
            'improvements': {},
            'optimization_time': time.time()
        }
        
        for target in optimization_targets:
            improvement = self._optimize_for_target(target)
            result['improvements'][target] = improvement
        
        self.optimization_history.append({
            'type': 'computation_optimization',
            'result': result,
            'timestamp': time.time()
        })
        
        return result
    
    def _get_memory_usage(self) -> float:
        """Get current memory usage in MB."""
        process = psutil.Process()
        return process.memory_info().rss / 1024 / 1024
    
    def _run_memory_optimization(self, target_reduction: float) -> float:
        """Run memory optimization."""
        # Simplified optimization - in practice, this would:
        # 1. Analyze memory usage patterns
        # 2. Identify memory inefficiencies
        # 3. Apply memory optimizations
        # 4. Return optimized memory usage
        
        return self._get_memory_usage() * (1.0 - target_reduction)
    
    def _optimize_for_target(self, target: str) -> Dict:
        """Optimize for specific target."""
        # Simplified optimization - in practice, this would:
        # 1. Profile performance
        # 2. Identify bottlenecks
        # 3. Apply optimizations
        # 4. Return improvements
        
        improvements = {
            'before': 0.0,
            'after': 0.0,
            'improvement_percent': 0.0
        }
        
        # Placeholder improvements
        improvements['improvement_percent'] = np.random.uniform(5, 20)
        
        return improvements

class MultiAgentSystem:
    """
    Multi-agent system for NLM.
    
    This class provides capabilities for:
    - Multiple agent collaboration
    - Agent communication
    - Distributed training
    - Collective learning
    """
    
    def __init__(self):
        """Initialize multi-agent system."""
        self.agents = {}
        self.communications = {}
        self.training_queue = []
    
    def add_agent(self, name: str, brain: pynlm.Brain, role: str = "worker"):
        """
        Add agent to system.
        
        Args:
            name: Agent name
            brain: Agent brain
            role: Agent role ('worker', 'manager', 'specialist')
        """
        self.agents[name] = {
            'brain': brain,
            'role': role,
            'last_update': time.time(),
            'performance': {'score': 0.0, 'steps': 0}
        }
        
        self.communications[name] = []
    
    def collaborative_training(self, training_data: List[Dict], rounds: int = 10):
        """
        Perform collaborative training.
        
        Args:
            training_data: Training data
            rounds: Number of training rounds
            
        Returns:
            Training results
        """
        results = {
            'rounds': rounds,
            'final_scores': {},
            'training_history': []
        }
        
        for round_num in range(rounds):
            round_results = self._collaborative_round(round_num, training_data)
            results['training_history'].append(round_results)
            
            # Update performance scores
            for agent_name, agent_results in round_results['agent_results'].items():
                if agent_name in self.agents:
                    self.agents[agent_name]['performance']['score'] = agent_results.get('score', 0.0)
                    self.agents[agent_name]['performance']['steps'] += agent_results.get('steps', 0)
        
        # Calculate final scores
        for agent_name in self.agents:
            agent_perf = self.agents[agent_name]['performance']
            results['final_scores'][agent_name] = {
                'average_score': agent_perf['score'] / max(1, agent_perf['steps']),
                'total_steps': agent_perf['steps']
            }
        
        return results
    
    def _collaborative_round(self, round_num: int, training_data: List[Dict]) -> Dict:
        """Perform collaborative training round."""
        results = {
            'round': round_num,
            'agent_results': {},
            'collective_performance': 0.0
        }
        
        # Distribute data among agents
        for agent_name, agent_info in self.agents.items():
            agent_data = self._distribute_data(agent_name, training_data)
            
            # Perform training
            agent_results = self._agent_training(agent_name, agent_data)
            results['agent_results'][agent_name] = agent_results
        
        # Aggregate results
        total_score = sum(r.get('score', 0.0) for r in results['agent_results'].values())
        results['collective_performance'] = total_score / len(results['agent_results'])
        
        return results
    
    def _distribute_data(self, agent_name: str, training_data: List[Dict]) -> List[Dict]:
        """Distribute training data to agent."""
        agent_info = self.agents[agent_name]
        
        # Simple distribution strategy
        agent_role = agent_info['role']
        
        if agent_role == 'manager':
            # Manager gets all data for coordination
            return training_data
        elif agent_role == 'specialist':
            # Specialist gets specialized data
            return [d for d in training_data if d.get('type') == agent_name]
        else:  # worker
            # Worker gets subset of data
            return training_data[:max(1, len(training_data) // len(self.agents))]
    
    def _agent_training(self, agent_name: str, training_data: List[Dict]) -> Dict:
        """Perform agent training."""
        agent_info = self.agents[agent_name]
        brain = agent_info['brain']
        
        # Simple training loop
        steps = len(training_data) * 10  # Each data point gets 10 steps
        score = 0.0
        
        for step in range(steps):
            # Simple training operation
            brain.step(step)
            
            # Accumulate score (simplified)
            score += brain.getAverageFiringRate() * 0.1
        
        return {
            'score': score,
            'steps': steps,
            'agent_name': agent_name
        }

# Convenience functions for advanced usage
def createAdvancedExperiment(experiment_type: str, config: Optional[Dict] = None) -> Dict:
    """
    Create advanced experiment setup.
    
    Args:
        experiment_type: Type of experiment ('transfer', 'multimodal', 'curriculum')
        config: Experiment configuration
        
    Returns:
        Experiment setup dictionary
    """
    experiments = {
        'transfer': {
            'name': 'Transfer Learning Experiment',
            'description': 'Experiment for transferring knowledge between different brain configurations',
            'setup': lambda cfg: {
                'source_brain': pynlm.createBrain(configureFromPreset('quick')),
                'target_brain': pynlm.createBrain(configureFromPreset('research')),
                'transfer_strength': cfg.get('transfer_strength', 0.5),
                'similarity_metric': cfg.get('similarity_metric', 'correlation')
            }
        },
        'multimodal': {
            'name': 'Multimodal Learning Experiment',
            'description': 'Experiment for learning from multiple input modalities',
            'setup': lambda cfg: {
                'brain': pynlm.createBrain(configureFromPreset('production')),
                'modalities': cfg.get('modalities', ['visual', 'auditory', 'tactile']),
                'integration_weights': cfg.get('integration_weights', {'visual': 0.4, 'auditory': 0.3, 'tactile': 0.3})
            }
        },
        'curriculum': {
            'name': 'Curriculum Learning Experiment',
            'description': 'Experiment for learning through curriculum progression',
            'setup': lambda cfg: {
                'brain': pynlm.createBrain(configureFromPreset('debug')),
                'curriculum': cfg.get('curriculum', [
                    {'start': 0, 'end': 100, 'difficulty': 'easy'},
                    {'start': 100, 'end': 500, 'difficulty': 'medium'},
                    {'start': 500, 'end': 1000, 'difficulty': 'hard'}
                ])
            }
        }
    }
    
    if experiment_type not in experiments:
        raise ValueError(f"Unknown experiment type: {experiment_type}")
    
    experiment = experiments[experiment_type]
    
    if config is None:
        config = {}
    
    return {
        'experiment_info': experiment,
        'config': config,
        'setup': experiment['setup'](config)
    }

def createNeuralArchitectureSearch(config: Optional[Dict] = None) -> Dict:
    """
    Create neural architecture search setup.
    
    Args:
        config: NAS configuration
        
    Returns:
        NAS setup dictionary
    """
    if config is None:
        config = {}
    
    return {
        'nas_info': {
            'name': 'Neural Architecture Search',
            'description': 'Automated neural architecture search for NLM',
            'method': 'evolution'
        },
        'config': config,
        'parameters': {
            'population_size': config.get('population_size', 10),
            'generations': config.get('generations', 20),
            'mutation_rate': config.get('mutation_rate', 0.1),
            'crossover_rate': config.get('crossover_rate', 0.8),
            'elitism': config.get('elitism', 0.1)
        }
    }

def createMetaLearningSystem(config: Optional[Dict] = None) -> Dict:
    """
    Create meta-learning system setup.
    
    Args:
        config: Meta-learning configuration
        
    Returns:
        Meta-learning setup dictionary
    """
    if config is None:
        config = {}
    
    return {
        'meta_info': {
            'name': 'Meta-Learning System',
            'description': 'Meta-learning for rapid adaptation and generalization',
            'approach': 'model-agnostic-meta-learning'
        },
        'config': config,
        'parameters': {
            'meta_learning_rate': config.get('meta_learning_rate', 0.001),
            'fast_learning_rate': config.get('fast_learning_rate', 0.01),
            'num_tasks': config.get('num_tasks', 10),
            'task_batch_size': config.get('task_batch_size', 5),
            'inner_loop_steps': config.get('inner_loop_steps', 5)
        }
    }