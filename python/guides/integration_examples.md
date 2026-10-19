#!/usr/bin/env python3
"""
Integration Examples - Connecting NLM with External Systems

This guide demonstrates integration of NLM Python bindings with external systems:
- Machine learning frameworks (TensorFlow, PyTorch)
- Neuroscience toolkits (NEURON, NetPyNE)
- Simulation environments (Gym, PyBullet)
- Data analysis tools (pandas, numpy)
- Visualization libraries (Matplotlib, Plotly)
- Robotics control systems
- Neuroscience recording systems
- Real-time data streams
"""
import nlm
import time
import numpy as np
import json
from typing import List, Dict, Any, Optional, Callable
from pathlib import Path
import threading
import queue
import asyncio
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.preprocessing import StandardScaler
from sklearn.cluster import KMeans
import plotly.graph_objects as go
from collections import deque
import zmq
import pika
from abc import ABC, abstractmethod

class ExternalSystemInterface(ABC):
    """Abstract base class for external system integration."""
    
    @abstractmethod
    def connect(self, config: Dict[str, Any]) -> bool:
        """Connect to external system."""
        pass
    
    @abstractmethod
    def disconnect(self) -> bool:
        """Disconnect from external system."""
        pass
    
    @abstractmethod
    def send_data(self, data: Dict[str, Any]) -> bool:
        """Send data to external system."""
        pass
    
    @abstractmethod
    def receive_data(self) -> Optional[Dict[str, Any]]:
        """Receive data from external system."""
        pass
class MachineLearningIntegration:
    """
    Integration with machine learning frameworks for:
    - Model training and validation
    - Feature extraction
    - Predictive analytics
    - Reinforcement learning
    """
    
    def __init__(self, framework: str = "sklearn"):
        """
        Initialize ML integration.
        
        Args:
            framework: ML framework to use
        """
        print(f"Initializing Machine Learning Integration ({framework})...")
        
        self.framework = framework
        self.models = {}
        self.scalers = {}
        self.feature_extractors = {}
        
        # Initialize framework-specific components
        if framework == "sklearn":
            self._setup_sklearn()
        elif framework == "tensorflow":
            self._setup_tensorflow()
        elif framework == "torch":
            self._setup_pytorch()
        
        print("✓ ML integration initialized")
    
    def _setup_sklearn(self):
        """Setup scikit-learn integration."""
        try:
            from sklearn.preprocessing import StandardScaler, MinMaxScaler
            from sklearn.cluster import KMeans
            from sklearn.decomposition import PCA
            from sklearn.ensemble import RandomForestClassifier
            
            self.StandardScaler = StandardScaler
            self.MinMaxScaler = MinMaxScaler
            self.KMeans = KMeans
            self.PCA = PCA
            self.RandomForestClassifier = RandomForestClassifier
            
            print("✓ scikit-learn components loaded")
        except ImportError:
            print("⚠ scikit-learn not available")
            self.StandardScaler = None
            self.MinMaxScaler = None
            self.KMeans = None
            self.PCA = None
            self.RandomForestClassifier = None
    
    def _setup_tensorflow(self):
        """Setup TensorFlow integration."""
        try:
            import tensorflow as tf
            from tensorflow.keras.models import Sequential
            from tensorflow.keras.layers import Dense, LSTM, Embedding
            from tensorflow.keras.optimizers import Adam
            
            self.tf = tf
            self.Sequential = Sequential
            self.Dense = Dense
            self.LSTM = LSTM
            self.Embedding = Embedding
            self.Adam = Adam
            
            print("✓ TensorFlow components loaded")
        except ImportError:
            print("⚠ TensorFlow not available")
            self.tf = None
    
    def _setup_pytorch(self):
        """Setup PyTorch integration."""
        try:
            import torch
            import torch.nn as nn
            import torch.optim as optim
            
            self.torch = torch
            self.nn = nn
            self.optim = optim
            
            print("✓ PyTorch components loaded")
        except ImportError:
            print("⚠ PyTorch not available")
            self.torch = None
            self.nn = None
            self.optim = None
    
    def extract_features_from_percept(self, percept: nlm.SensoryPercept) -> Dict[str, Any]:
        """
        Extract features from percept for ML model input.
        
        Args:
            percept: Sensory percept to extract features from
            
        Returns:
            Dictionary of extracted features
        """
        # Extract raw data
        vision_data = percept.getVision()
        touch_data = percept.getTouch()
        internal_data = percept.getInternal()
        proprioception_data = percept.getProprioception()
        
        features = {
            'vision': {
                'mean': np.mean(vision_data),
                'std': np.std(vision_data),
                'min': np.min(vision_data),
                'max': np.max(vision_data),
                'median': np.median(vision_data),
                'percentile_25': np.percentile(vision_data, 25),
                'percentile_75': np.percentile(vision_data, 75),
                'range': np.max(vision_data) - np.min(vision_data),
                'skewness': self._calculate_skewness(vision_data),
                'kurtosis': self._calculate_kurtosis(vision_data),
            },
            'touch': {
                'mean': np.mean(touch_data),
                'std': np.std(touch_data),
                'sum': np.sum(touch_data),
                'product': np.prod(touch_data) if len(touch_data) > 0 else 0,
            },
            'internal': {
                'mean': np.mean(internal_data),
                'std': np.std(internal_data),
                'max_positive': np.max(internal_data[internal_data > 0]) if any(internal_data > 0) else 0,
                'min_negative': np.min(internal_data[internal_data < 0]) if any(internal_data < 0) else 0,
                'positive_ratio': np.sum(internal_data > 0) / len(internal_data),
            },
            'proprioception': {
                'movement_speed': np.sqrt(np.sum(np.diff(proprioception_data[:2])**2)),
                'energy_efficiency': proprioception_data[-1] / (np.sum(proprioception_data[:-1]) + 1e-9),
                'stability': 1.0 / (1.0 + np.std(proprioception_data[-3:])),
            },
        }
        
        return features
    
    def _calculate_skewness(self, data: List[float]) -> float:
        """Calculate skewness of data."""
        if len(data) < 3:
            return 0.0
        
        mean = np.mean(data)
        std = np.std(data)
        
        if std < 1e-9:
            return 0.0
        
        skewness = np.mean(((np.array(data) - mean) / std) ** 3)
        return skewness
    
    def _calculate_kurtosis(self, data: List[float]) -> float:
        """Calculate kurtosis of data."""
        if len(data) < 4:
            return 0.0
        
        mean = np.mean(data)
        std = np.std(data)
        
        if std < 1e-9:
            return 0.0
        
        kurtosis = np.mean(((np.array(data) - mean) / std) ** 4) - 3.0
        return kurtosis
    
    def train_behavior_predictor(self, training_data: List[Dict[str, Any]]) -> Any:
        """
        Train ML model to predict behavior from percepts.
        
        Args:
            training_data: List of training examples with percepts and behaviors
            
        Returns:
            Trained ML model
        """
        print("Training behavior predictor model...")
        
        # Extract features and labels
        X = []
        y = []
        
        for example in training_data:
            # Extract features
            features = self.extract_features_from_percept(example['percept'])
            
            # Flatten features for model input
            flat_features = self._flatten_features(features)
            X.append(flat_features)
            
            # Get behavior label
            y.append(example['behavior'])
        
        # Convert to numpy arrays
        X = np.array(X)
        y = np.array(y)
        
        # Scale features
        if self.StandardScaler:
            scaler = self.StandardScaler()
            X_scaled = scaler.fit_transform(X)
            self.scalers['behavior_predictor'] = scaler
        else:
            X_scaled = X
        
        # Train model
        if self.framework == "sklearn" and self.RandomForestClassifier:
            model = self.RandomForestClassifier(n_estimators=100, random_state=42)
            model.fit(X_scaled, y)
            self.models['behavior_predictor'] = model
            
        elif self.framework == "tensorflow" and self.tf:
            # TensorFlow neural network
            model = self.Sequential([
                self.Dense(128, activation='relu', input_shape=(X_scaled.shape[1],)),
                self.Dense(64, activation='relu'),
                self.Dense(len(np.unique(y)), activation='softmax')
            ])
            
            model.compile(
                optimizer=self.Adam(learning_rate=0.001),
                loss='sparse_categorical_crossentropy',
                metrics=['accuracy']
            )
            
            model.fit(X_scaled, y, epochs=50, batch_size=32, validation_split=0.2)
            self.models['behavior_predictor'] = model
        
        print("✓ Behavior predictor trained")
        return self.models['behavior_predictor']
    
    def _flatten_features(self, features: Dict[str, Any]) -> List[float]:
        """Flatten nested features into single list."""
        flat = []
        
        for category, data in features.items():
            if isinstance(data, dict):
                for key, value in data.items():
                    if isinstance(value, (list, np.ndarray)):
                        flat.extend(value)
                    else:
                        flat.append(value)
            else:
                flat.append(data)
        
        return flat
    
    def predict_behavior(self, percept: nlm.SensoryPercept, model: Any = None) -> Dict[str, Any]:
        """
        Predict behavior from percept using trained model.
        
        Args:
            percept: Sensory percept to analyze
            model: Trained model (if None, uses default behavior predictor)
            
        Returns:
            Dictionary with predictions
        """
        if model is None and 'behavior_predictor' not in self.models:
            raise ValueError("No trained model available. Train a model first.")
        
        model = self.models.get('behavior_predictor', model)
        
        # Extract features
        features = self.extract_features_from_percept(percept)
        X = self._flatten_features(features)
        
        if self.StandardScaler:
            X_scaled = self.scalers['behavior_predictor'].transform([X])
        else:
            X_scaled = [X]
        
        # Make prediction
        if self.framework == "sklearn":
            prediction = model.predict(X_scaled)[0]
            probabilities = model.predict_proba(X_scaled)[0]
        elif self.framework == "tensorflow":
            prediction = np.argmax(model.predict(X_scaled)[0])
            probabilities = model.predict(X_scaled)[0]
        else:
            prediction = "unknown"
            probabilities = {}
        
        return {
            'predicted_behavior': prediction,
            'confidence': float(np.max(probabilities) if len(probabilities) > 0 else 0.0),
            'probabilities': dict(zip(range(len(probabilities)), probabilities) if hasattr(probabilities, '__len__') else probabilities),
            'features': features,
        }
    
    def train_development_predictor(self, episode_data: List[Dict[str, Any]]) -> Any:
        """
        Train model to predict developmental progression.
        
        Args:
            episode_data: List of episode data with developmental metrics
            
        Returns:
            Trained development predictor model
        """
        print("Training development progression predictor...")
        
        # Extract features and target
        X = []
        y = []
        
        for episode in episode_data:
            # Extract developmental features
            features = {
                'total_reward': episode.get('total_reward', 0),
                'max_reward': episode.get('max_reward', 0),
                'min_reward': episode.get('min_reward', 0),
                'final_curiosity': episode.get('final_curiosity', 0),
                'final_novelty': episode.get('final_novelty', 0),
                'final_prediction_error': episode.get('final_prediction_error', 0),
                'steps_taken': episode.get('steps_taken', 0),
                'development_score': episode.get('development_score', 0),
                'efficiency': episode.get('efficiency', 0),
            }
            
            X.append(features)
            y.append(episode.get('developmental_stage', 0))
        
        # Convert to numpy arrays
        feature_names = list(X[0].keys())
        X_array = np.array([list(x.values()) for x in X])
        y_array = np.array(y)
        
        # Scale features
        if self.StandardScaler:
            scaler = self.StandardScaler()
            X_scaled = scaler.fit_transform(X_array)
            self.scalers['development_predictor'] = scaler
        else:
            X_scaled = X_array
        
        # Train model
        if self.framework == "sklearn" and self.RandomForestClassifier:
            model = self.RandomForestClassifier(n_estimators=100, random_state=42)
            model.fit(X_scaled, y_array)
            self.models['development_predictor'] = model
        
        print("✓ Development predictor trained")
        return self.models['development_predictor']
class NeuroscienceToolkitIntegration:
    """
    Integration with neuroscience toolkits for:
    - Neural data analysis
    - Spike train analysis
    - Network modeling
    - Brain connectivity analysis
    """
    
    def __init__(self, toolkit: str = "netpyne"):
        """
        Initialize neuroscience toolkit integration.
        
        Args:
            toolkit: Neuroscience toolkit to use
        """
        print(f"Initializing Neuroscience Toolkit Integration ({toolkit})...")
        
        self.toolkit = toolkit
        self.neural_data = []
        self.spike_trains = []
        self.network_models = {}
        
        # Initialize toolkit-specific components
        if toolkit == "netpyne":
            self._setup_netpyne()
        elif toolkit == "neuron":
            self._setup_neuron()
        elif toolkit == "brian2":
            self._setup_brian2()
        
        print("✓ Neuroscience toolkit integration initialized")
    
    def _setup_netpyne(self):
        """Setup NetPyNE integration."""
        try:
            import netpyne
            from netpyne import sim, net
            
            self.netpyne = netpyne
            self.sim = sim
            self.net = net
            
            print("✓ NetPyNE components loaded")
        except ImportError:
            print("⚠ NetPyNE not available")
            self.netpyne = None
    
    def _setup_neuron(self):
        """Setup NEURON integration."""
        try:
            from neuron import h, gui, netpyne as np
            
            self.h = h
            self.gui = gui
            self.netpyne = np
            
            print("✓ NEURON components loaded")
        except ImportError:
            print("⚠ NEURON not available")
            self.h = None
    
    def _setup_brian2(self):
        """Setup Brian2 integration."""
        try:
            import brian2 as b2
            
            self.brian2 = b2
            print("✓ Brian2 components loaded")
        except ImportError:
            print("⚠ Brian2 not available")
            self.brian2 = None
    
    def analyze_spike_trains(self, spike_data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Analyze spike trains for neural activity patterns.
        
        Args:
            spike_data: List of spike train data
            
        Returns:
            Dictionary with analysis results
        """
        print("Analyzing spike trains...")
        
        analysis = {
            'total_spikes': sum(len(spike['times']) for spike in spike_data),
            'firing_rates': [],
            'burst_patterns': [],
            'inter_spike_intervals': [],
            'regularity_indices': [],
            'oscillations': [],
        }
        
        for i, spike in enumerate(spike_data):
            times = spike['times']
            neuron_id = spike.get('neuron_id', i)
            
            if len(times) > 1:
                # Calculate firing rate
                duration = max(times) - min(times) if len(times) > 1 else 1.0
                firing_rate = len(times) / duration if duration > 0 else 0
                analysis['firing_rates'].append({
                    'neuron_id': neuron_id,
                    'rate': firing_rate,
                    'duration': duration,
                })
                
                # Calculate inter-spike intervals
                isi = np.diff(times)
                analysis['inter_spike_intervals'].extend(isi.tolist())
                
                # Calculate regularity index
                if len(isi) > 0:
                    regularity = np.std(isi) / np.mean(isi) if np.mean(isi) > 0 else 0
                    analysis['regularity_indices'].append({
                        'neuron_id': neuron_id,
                        'regularity': regularity,
                    })
                
                # Detect burst patterns
                if len(isi) > 0:
                    # Simple burst detection (ISI < threshold)
                    isi_mean = np.mean(isi)
                    isi_std = np.std(isi)
                    threshold = isi_mean + 2 * isi_std
                    
                    bursts = np.sum(isi < threshold)
                    analysis['burst_patterns'].append({
                        'neuron_id': neuron_id,
                        'bursts': bursts,
                        'threshold': threshold,
                    })
            
            # Detect oscillations
            if len(times) >= 10:
                # Check for rhythmic patterns
                cycles = np.diff(times)
                if len(cycles) > 3:
                    # Simple periodicity detection
                    autocorr = np.correlate(cycles - np.mean(cycles), 
                                          cycles - np.mean(cycles), mode='full')
                    if len(autocorr) > 5:
                        peak_indices = np.where(np.abs(autocorr) > 0.5 * np.max(np.abs(autocorr)))[0]
                        if len(peak_indices) > 1:
                            dominant_period = cycles[peak_indices[0]]
                            analysis['oscillations'].append({
                                'neuron_id': neuron_id,
                                'dominant_period': dominant_period,
                                'confidence': len(peak_indices) / len(autocorr),
                            })
        
        print("✓ Spike train analysis complete")
        return analysis
    
    def analyze_brain_connectivity(self, connectivity_data: Dict[str, Any]) -> Dict[str, Any]:
        """
        Analyze brain connectivity patterns.
        
        Args:
            connectivity_data: Connectivity data between brain regions
            
        Returns:
            Dictionary with connectivity analysis
        """
        print("Analyzing brain connectivity...")
        
        analysis = {
            'adjacency_matrix': connectivity_data.get('adjacency', []),
            'degree_distribution': [],
            'modularity_scores': [],
            'community_structure': [],
            'path_lengths': [],
            'eigengroup': {},
        }
        
        adjacency = connectivity_data.get('adjacency', [])
        if not adjacency:
            print("⚠ No adjacency data provided")
            return analysis
        
        # Convert to numpy array
        adj_matrix = np.array(adjacency)
        
        # Calculate degree distribution
        if len(adj_matrix.shape) == 2 and adj_matrix.shape[0] == adj_matrix.shape[1]:
            degrees = np.sum(adj_matrix > 0, axis=1)
            analysis['degree_distribution'] = degrees.tolist()
            
            # Calculate average degree
            avg_degree = np.mean(degrees)
            analysis['average_degree'] = float(avg_degree)
            
            # Calculate clustering coefficient
            clustering = []
            for i in range(len(adj_matrix)):
                neighbors = np.where(adj_matrix[i] > 0)[0]
                if len(neighbors) >= 2:
                    # Count triangles
                    triangles = 0
                    for j in neighbors:
                        for k in neighbors:
                            if j < k and adj_matrix[j, k] > 0:
                                triangles += 1
                    
                    clustering_coeff = (2 * triangles) / (len(neighbors) * (len(neighbors) - 1))
                    clustering.append(clustering_coeff)
            
            analysis['clustering_coefficient'] = {
                'average': np.mean(clustering) if clustering else 0,
                'distribution': clustering,
            }
        
        print("✓ Brain connectivity analysis complete")
        return analysis
    
    def compare_with_neurodata(self, neural_data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Compare NLM neural data with real neural data.
        
        Args:
            neural_data: Real neural data for comparison
            
        Returns:
            Dictionary with comparison results
        """
        print("Comparing with real neural data...")
        
        # Extract statistics from NLM data
        nlm_stats = {
            'firing_rate_mean': np.mean([neuron.get('firing_rate', 0) for neuron in neural_data]),
            'firing_rate_std': np.std([neuron.get('firing_rate', 0) for neuron in neural_data]),
            'activity_patterns': [neuron.get('firing_pattern', []) for neuron in neural_data],
            'correlation_matrix': self._calculate_correlation_matrix(neural_data),
        }
        
        # Compare with real neural data
        comparison = {
            'nlm_statistics': nlm_stats,
            'real_neural_statistics': {},
            'similarity_scores': {},
            'differences': {},
        }
        
        # Calculate similarity scores
        for neuron_type in ['excitatory', 'inhibitory', 'modulatory']:
            if neuron_type in [neuron.get('type', '') for neuron in neural_data]:
                # Extract real neural data for this type
                real_data = [neuron for neuron in neural_data if neuron.get('type') == neuron_type]
                
                if real_data:
                    # Calculate statistics
                    firing_rates = [neuron.get('firing_rate', 0) for neuron in real_data]
                    comparison['real_neural_statistics'][neuron_type] = {
                        'mean_firing_rate': np.mean(firing_rates),
                        'std_firing_rate': np.std(firing_rates),
                        'firing_rate_range': [np.min(firing_rates), np.max(firing_rates)],
                    }
                    
                    # Calculate similarity
                    nlm_rate = nlm_stats['firing_rate_mean']
                    real_rate = comparison['real_neural_statistics'][neuron_type]['mean_firing_rate']
                    
                    similarity = 1.0 / (1.0 + abs(nlm_rate - real_rate))
                    comparison['similarity_scores'][neuron_type] = similarity
                    
                    # Record difference
                    comparison['differences'][neuron_type] = abs(nlm_rate - real_rate)
        
        print("✓ Neural data comparison complete")
        return comparison
    
    def _calculate_correlation_matrix(self, neural_data: List[Dict[str, Any]]) -> List[List[float]]:
        """Calculate correlation matrix from neural data."""
        if len(neural_data) < 2:
            return [[1.0]]
        
        # Extract features
        features = []
        for neuron in neural_data:
            neuron_features = [
                neuron.get('firing_rate', 0),
                neuron.get('firing_pattern_mean', 0),
                neuron.get('firing_pattern_std', 0),
                neuron.get('activity_dynamics', 0),
                neuron.get('correlation_with_network', 0),
            ]
            features.append(neuron_features)
        
        # Calculate correlation matrix
        feature_array = np.array(features)
        correlation_matrix = np.corrcoef(feature_array.T)
        
        return correlation_matrix.tolist()
class SimulationEnvironmentIntegration:
    """
    Integration with simulation environments for:
    - Realistic world simulation
    - Physics-based interactions
    - Multi-agent environments
    - Reward function optimization
    """
    
    def __init__(self, environment: str = "gym"):
        """
        Initialize simulation environment integration.
        
        Args:
            environment: Simulation environment to use
        """
        print(f"Initializing Simulation Environment Integration ({environment})...")
        
        self.environment = environment
        self.env = None
        self.observation_spaces = {}
        self.action_spaces = {}
        self.rewards = {}
        
        # Initialize environment-specific components
        if environment == "gym":
            self._setup_gym()
        elif environment == "pybullet":
            self._setup_pybullet()
        elif environment == "webots":
            self._setup_webots()
        
        print("✓ Simulation environment integration initialized")
    
    def _setup_gym(self):
        """Setup Gym environment integration."""
        try:
            import gym
            import gym.spaces as spaces
            
            self.gym = gym
            self.spaces = spaces
            
            print("✓ Gym environment components loaded")
        except ImportError:
            print("⚠ Gym not available")
            self.gym = None
    
    def _setup_pybullet(self):
        """Setup PyBullet integration."""
        try:
            import pybullet as p
            
            self.pybullet = p
            print("✓ PyBullet components loaded")
        except ImportError:
            print("⚠ PyBullet not available")
            self.pybullet = None
    
    def _setup_webots(self):
        """Setup Webots integration."""
        try:
            import webots
            
            self.webots = webots
            print("✓ Webots components loaded")
        except ImportError:
            print("⚠ Webots not available")
            self.webots = None
    
    def create_nlm_world_from_env(self, env_config: Dict[str, Any]) -> nlm.SimpleWorld:
        """
        Create NLM world from simulation environment configuration.
        
        Args:
            env_config: Environment configuration
            
        Returns:
            Configured NLM SimpleWorld
        """
        print("Creating NLM world from environment configuration...")
        
        # Create world
        world = nlm.createSimpleWorld()
        
        # Configure based on environment
        config = env_config.get('world_config', {})
        width = config.get('width', 100)
        height = config.get('height', 100)
        vision_width = config.get('vision_width', 32)
        vision_height = config.get('vision_height', 32)
        
        world.configure(width, height, vision_width, vision_height)
        
        # Set initial agent position
        start_pos = config.get('agent_start', [50, 50])
        if isinstance(start_pos, list) and len(start_pos) >= 2:
            world.setAgentStart(start_pos[0], start_pos[1])
        
        # Set world parameters
        if 'max_energy' in config:
            world.setMaxEnergy(config['max_energy'])
        
        if 'energy_decay_rate' in config:
            world.setEnergyDecayRate(config['energy_decay_rate'])
        
        if 'random_seed' in config:
            world.setRandomSeed(config['random_seed'])
        
        # Add environment objects
        objects = config.get('world_objects', [])
        for obj_config in objects:
            obj = self._create_environment_object(obj_config)
            if obj:
                world.addObject(obj)
        
        print("✓ NLM world created from environment configuration")
        return world
    
    def _create_environment_object(self, obj_config: Dict[str, Any]) -> nlm.WorldObject:
        """Create environment object from configuration."""
        try:
            # Create world object
            obj = nlm.WorldObject()
            
            # Set properties
            obj.x = obj_config.get('x', 0.0)
            obj.y = obj_config.get('y', 0.0)
            obj.type = self._map_object_type(obj_config.get('type', 'empty'))
            obj.value = obj_config.get('value', 0.0)
            obj.radius = obj_config.get('radius', 0.5)
            obj.active = obj_config.get('active', True)
            
            return obj
            
        except Exception as e:
            print(f"⚠ Could not create environment object: {e}")
            return None
    
    def _map_object_type(self, env_type: str) -> nlm.WorldObjectType:
        """Map environment object type to NLM WorldObjectType."""
        type_mapping = {
            'empty': nlm.WorldObjectType.Empty,
            'resource': nlm.WorldObjectType.Resource,
            'hazard': nlm.WorldObjectType.Hazard,
            'wall': nlm.WorldObjectType.Wall,
            'marker': nlm.WorldObjectType.Marker,
        }
        
        return type_mapping.get(env_type, nlm.WorldObjectType.Empty)
    
    def integrate_reward_function(self, env_config: Dict[str, Any]) -> Callable:
        """
        Integrate custom reward function from environment.
        
        Args:
            env_config: Environment configuration with reward function
            
        Returns:
            Reward function
        """
        reward_config = env_config.get('reward_function', {})
        reward_type = reward_config.get('type', 'simple')
        
        if reward_type == 'simple':
            return self._simple_reward_function(env_config)
        elif reward_type == 'curiosity_based':
            return self._curiosity_based_reward_function(env_config)
        elif reward_type == 'prediction_error_based':
            return self._prediction_error_based_reward_function(env_config)
        else:
            return self._simple_reward_function(env_config)
    
    def _simple_reward_function(self, env_config: Dict[str, Any]) -> Callable:
        """Simple reward function based on proximity to goals."""
        goal_positions = env_config.get('goal_positions', [])
        
        def reward_function(world, agent, action, reward_type='action'):
            """Calculate reward based on agent actions and world state."""
            body = agent.getBody()
            total_reward = 0.0
            
            # Distance-based reward
            if goal_positions:
                min_distance = min(
                    np.sqrt((body.x - goal[0])**2 + (body.y - goal[1])**2)
                    for goal in goal_positions
                )
                total_reward += max(0, 1.0 - min_distance / 100.0)
            
            # Energy-based reward
            total_reward += body.energy * 0.1
            
            # Action-specific reward
            if reward_type == 'action':
                total_reward += 0.1
            elif reward_type == 'success':
                if len(goal_positions) > 0 and min_distance < 5.0:
                    total_reward += 1.0
            
            return total_reward
        
        return reward_function
    
    def _curiosity_based_reward_function(self, env_config: Dict[str, Any]) -> Callable:
        """Curiosity-based reward function."""
        def reward_function(world, agent, action, reward_type='action'):
            """Calculate reward based on curiosity."""
            body = agent.getBody()
            total_reward = 0.0
            
            # Add curiosity bonus
            total_reward += agent.getCuriosityLevel() * 0.5
            
            # Novelty reward
            total_reward += agent.getNoveltyLevel() * 0.3
            
            # Exploration reward
            if agent.getExplorationRate() > 0.5:
                total_reward += 0.2
            
            return total_reward
        
        return reward_function
    
    def _prediction_error_based_reward_function(self, env_config: Dict[str, Any]) -> Callable:
        """Prediction error-based reward function."""
        def reward_function(world, agent, action, reward_type='action'):
            """Calculate reward based on prediction error."""
            body = agent.getBody()
            total_reward = 0.0
            
            # Add prediction error reward
            prediction_error = agent.getPredictionError()
            total_reward += prediction_error * 0.4
            
            # Add prediction uncertainty bonus
            if prediction_error > 0.5:
                total_reward += 0.5
            
            return total_reward
        
        return reward_function
    
    def optimize_environment_parameters(self, agent: nlm.AgentBrain, 
                                      world: nlm.SimpleWorld, 
                                      episodes: int = 100) -> Dict[str, Any]:
        """
        Optimize environment parameters for agent performance.
        
        Args:
            agent: Agent brain
            world: Simulation world
            episodes: Number of episodes for optimization
            
        Returns:
            Dictionary with optimized parameters
        """
        print("Optimizing environment parameters...")
        
        # Define parameter search space
        param_ranges = {
            'world_width': (50, 150),
            'world_height': (50, 150),
            'vision_width': (16, 64),
            'vision_height': (16, 64),
            'max_energy': (0.5, 2.0),
            'energy_decay': (0.01, 0.05),
        }
        
        # Run parameter search
        best_params = None
        best_score = float('-inf')
        
        print(f"  Running parameter search over {episodes} episodes...")
        
        for episode in range(episodes):
            # Generate random parameters
            params = {}
            for param, (min_val, max_val) in param_ranges.items():
                params[param] = np.random.uniform(min_val, max_val)
            
            # Create temporary world with parameters
            temp_world = nlm.createSimpleWorld()
            temp_world.configure(
                int(params['world_width']),
                int(params['world_height']),
                int(params['vision_width']),
                int(params['vision_height']),
            )
            
            temp_world.setMaxEnergy(params['max_energy'])
            temp_world.setEnergyDecayRate(params['energy_decay'])
            
            # Initialize agent with temporary world
            temp_agent = nlm.createAgentBrain(agent.getBrain())
            temp_agent.initialize(temp_world)
            
            # Run episode
            percept = nlm.SensoryPercept()
            vision_data = [0.5] * params['vision_width'] * params['vision_height']
            touch_data = [0.1] * 6
            internal_data = [0.1] * 6
            proprioception_data = [0.0] * 12
            
            percept.setVision(vision_data)
            percept.setTouch(touch_data)
            percept.setInternal(internal_data)
            percept.setProprioception(proprioception_data)
            
            temp_agent.processSensoryInput(percept, False)
            action = temp_agent.decodeMotorCommand()
            
            # Calculate performance score
            score = temp_agent.getEpisodeScore() + temp_agent.getCuriosityLevel()
            
            # Update best parameters
            if score > best_score:
                best_score = score
                best_params = params
        
        print(f"✓ Parameter optimization complete")
        print(f"  - Best score: {best_score:.3f}")
        print(f"  - Best parameters: {best_params}")
        
        return {
            'optimized_parameters': best_params,
            'best_score': best_score,
            'parameter_ranges': param_ranges,
            'episodes_tested': episodes,
        }
class DataAnalysisIntegration:
    """
    Integration with data analysis tools for:
    - Statistical analysis
    - Machine learning model training
    - Visualization
    - Results reporting
    """
    
    def __init__(self, analysis_type: str = "comprehensive"):
        """
        Initialize data analysis integration.
        
        Args:
            analysis_type: Type of analysis to perform
        """
        print(f"Initializing Data Analysis Integration ({analysis_type})...")
        
        self.analysis_type = analysis_type
        self.data = {}
        self.models = {}
        self.visualizations = {}
        
        # Initialize analysis tools
        self._setup_analysis_tools()
        
        print("✓ Data analysis integration initialized")
    
    def _setup_analysis_tools(self):
        """Setup data analysis tools."""
        try:
            import pandas as pd
            import numpy as np
            import matplotlib.pyplot as plt
            import plotly.graph_objects as go
            from sklearn.preprocessing import StandardScaler, MinMaxScaler
            from sklearn.cluster import KMeans
            from sklearn.decomposition import PCA
            from sklearn.model_selection import train_test_split
            from sklearn.metrics import mean_squared_error, r2_score
            
            self.pd = pd
            self.np = np
            self.plt = plt
            self.go = go
            self.StandardScaler = StandardScaler
            self.MinMaxScaler = MinMaxScaler
            self.KMeans = KMeans
            self.PCA = PCA
            self.train_test_split = train_test_split
            self.mean_squared_error = mean_squared_error
            self.r2_score = r2_score
            
            print("✓ Data analysis tools loaded")
        except ImportError as e:
            print(f"⚠ Some analysis tools not available: {e}")
    
    def analyze_episode_data(self, episode_data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Perform comprehensive analysis of episode data.
        
        Args:
            episode_data: List of episode data
            
        Returns:
            Dictionary with analysis results
        """
        print("Analyzing episode data...")
        
        analysis = {
            'summary': self._analyze_summary(episode_data),
            'temporal_patterns': self._analyze_temporal_patterns(episode_data),
            'behavioral_analysis': self._analyze_behavior(episode_data),
            'performance_metrics': self._analyze_performance(episode_data),
            'comparative_analysis': self._analyze_comparative(episode_data),
        }
        
        print("✓ Episode data analysis complete")
        return analysis
    
    def _analyze_summary(self, episode_data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze episode data summary."""
        if not episode_data:
            return {}
        
        # Extract key metrics
        total_rewards = [ep.get('total_reward', 0) for ep in episode_data]
        steps_taken = [ep.get('steps_taken', 0) for ep in episode_data]
        episode_scores = [ep.get('episode_score', 0) for ep in episode_data]
        
        return {
            'total_episodes': len(episode_data),
            'total_steps': sum(steps_taken),
            'total_rewards': sum(total_rewards),
            'average_reward_per_episode': np.mean(total_rewards) if total_rewards else 0,
            'std_reward_per_episode': np.std(total_rewards) if total_rewards else 0,
            'average_steps_per_episode': np.mean(steps_taken) if steps_taken else 0,
            'min_reward': np.min(total_rewards) if total_rewards else 0,
            'max_reward': np.max(total_rewards) if total_rewards else 0,
            'reward_range': np.max(total_rewards) - np.min(total_rewards) if total_rewards else 0,
        }
    
    def _analyze_temporal_patterns(self, episode_data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze temporal patterns in episode data."""
        if len(episode_data) < 2:
            return {}
        
        # Extract temporal features
        episodes = list(range(len(episode_data)))
        rewards = [ep.get('total_reward', 0) for ep in episode_data]
        scores = [ep.get('episode_score', 0) for ep in episode_data]
        curiosities = [ep.get('final_curiosity', 0) for ep in episode_data]
        novelties = [ep.get('final_novelty', 0) for ep in episode_data]
        
        # Calculate trends
        reward_trend = self._calculate_trend(episodes, rewards)
        score_trend = self._calculate_trend(episodes, scores)
        curiosity_trend = self._calculate_trend(episodes, curiosities)
        novelty_trend = self._calculate_trend(episodes, novelties)
        
        return {
            'reward_trend': reward_trend,
            'score_trend': score_trend,
            'curiosity_trend': curiosity_trend,
            'novelty_trend': novelty_trend,
            'learning_curve': self._analyze_learning_curve(episode_data),
            'convergence_analysis': self._analyze_convergence(episode_data),
        }
    
    def _calculate_trend(self, x: List[float], y: List[float]) -> Dict[str, Any]:
        """Calculate trend of data."""
        if len(x) < 2:
            return {'slope': 0, 'intercept': 0, 'r_squared': 0}
        
        # Simple linear regression
        x_array = np.array(x)
        y_array = np.array(y)
        
        # Calculate slope and intercept
        slope = (len(x_array) * np.sum(x_array * y_array) - 
                np.sum(x_array) * np.sum(y_array)) / (len(x_array) * np.sum(x_array**2) - np.sum(x_array)**2)
        
        intercept = np.mean(y_array) - slope * np.mean(x_array)
        
        # Calculate R-squared
        y_pred = slope * x_array + intercept
        ss_res = np.sum((y_array - y_pred) ** 2)
        ss_tot = np.sum((y_array - np.mean(y_array)) ** 2)
        r_squared = 1 - (ss_res / ss_tot) if ss_tot > 0 else 0
        
        return {
            'slope': float(slope),
            'intercept': float(intercept),
            'r_squared': float(r_squared),
            'trend_direction': 'increasing' if slope > 0.01 else 'decreasing' if slope < -0.01 else 'stable',
        }
    
    def _analyze_behavior(self, episode_data: List[Dict[str, Any]]) -> Dict[str, Any]:nlm.SensoryPercept:
        """Analyze behavioral patterns."""
        if not episode_data:
            return {}
        
        # Extract behavioral metrics
        behaviors = []
        action_types = []
        action_counts = {}
        
        for episode in episode_data:
            # Extract actions
            for step in episode.get('steps', []):
                action = step.get('action')
                if action:
                    behaviors.append(str(action))
                    action_counts[action] = action_counts.get(action, 0) + 1
            
            # Extract action types
            for step in episode.get('steps', []):
                action_type = step.get('action_result', {}).get('message', '')
                if action_type:
                    action_types.append(action_type)
        
        return {
            'action_distribution': action_counts,
            'behavior_patterns': self._extract_behavior_patterns(behaviors),
            'action_types': action_types,
            'behavioral_clustering': self._cluster_behaviors(behaviors) if behaviors else {},
            'action_sequence_analysis': self._analyze_action_sequences(episode_data),
        }
    
    def _extract_behavior_patterns(self, behaviors: List[str]) -> Dict[str, Any]:nlm.SensoryPercept:
        """Extract patterns from behaviors."""
        if not behaviors:
            return {}
        
        patterns = {}
        
        # Calculate frequency
        from collections import Counter
        freq = Counter(behaviors)
        patterns['frequency'] = dict(freq)
        
        # Calculate most common behavior
        if freq:
            most_common = freq.most_common(1)[0]
            patterns['most_common'] = {
                'behavior': most_common[0],
                'count': most_common[1],
                'ratio': most_common[1] / len(behaviors),
            }
        
        return patterns
    
    def _cluster_behaviors(self, behaviors: List[str]) -> Dict[str, Any]:nlm.SensoryPercept:
        """Cluster similar behaviors."""
        if len(behaviors) < 2:
            return {}
        
        try:
            # Convert behaviors to numerical features
            from sklearn.feature_extraction.text import TfidfVectorizer
            
            vectorizer = TfidfVectorizer(max_features=10)
            X = vectorizer.fit_transform(behaviors)
            
            # Cluster behaviors
            kmeans = KMeans(n_clusters=min(3, len(behaviors)), random_state=42)
            clusters = kmeans.fit_predict(X)
            
            return {
                'n_clusters': len(set(clusters)),
                'cluster_centers': vectorizer.inverse_transform(kmeans.cluster_centers_).tolist(),
                'cluster_assignment': clusters.tolist(),
            }
        except Exception as e:
            print(f"⚠ Could not cluster behaviors: {e}")
            return {}
    
    def _analyze_action_sequences(self, episode_data: List[Dict[str, Any]]) -> Dict[str, Any]:nlm.SensoryPercept:
        """Analyze action sequences."""
        if not episode_data:
            return {}
        
        sequences = []
        
        for episode in episode_data:
            # Extract action sequence
            sequence = []
            for step in episode.get('steps', []):
                action = step.get('action')
                if action:
                    sequence.append(str(action))
            
            sequences.append(sequence)
        
        # Analyze sequence patterns
        return {
            'sequences': sequences,
            'sequence_length_stats': {
                'mean': np.mean([len(seq) for seq in sequences]),
                'std': np.std([len(seq) for seq in sequences]),
                'min': np.min([len(seq) for seq in sequences]),
                'max': np.max([len(seq) for seq in sequences]),
            },
            'common_patterns': self._find_common_patterns(sequences),
        }
    
    def _find_common_patterns(self, sequences: List[List[str]]) -> List[str]:nlm.SensoryPercept:
        """Find common patterns in sequences."""
        if not sequences:
            return []
        
        # Find most common subsequences (simplified)
        patterns = []
        
        # Analyze for simple patterns
        all_actions = []
        for seq in sequences:
            all_actions.extend(seq)
        
        from collections import Counter
        action_freq = Counter(all_actions)
        
        # Get top patterns
        top_patterns = action_freq.most_common(5)
        patterns = [pattern[0] for pattern in top_patterns]
        
        return patterns
    
    def _analyze_performance(self, episode_data: List[Dict[str, Any]]) -> Dict[str, Any]:nlm.SensoryPercept:
        """Analyze performance metrics."""
        if not episode_data:
            return {}
        
        # Extract performance metrics
        performance_metrics = []
        
        for episode in episode_data:
            # Calculate episode performance
            steps = episode.get('steps', [])
            if steps:
                # Calculate metrics from episode data
                action_results = [step.get('action_result', {}) for step in steps]
                rewards = [ar.get('reward', 0) for ar in action_results]
                successes = [ar.get('success', False) for ar in action_results]
                
                performance_metrics.append({
                    'episode_id': episode.get('episode_id', 0),
                    'total_reward': sum(rewards),
                    'average_reward_per_step': np.mean(rewards) if rewards else 0,
                    'success_rate': np.mean(successes) if successes else 0,
                    'total_steps': len(steps),
                    'efficiency_score': episode.get('total_reward', 0) / max(1, len(steps)),
                })
        
        return {
            'performance_metrics': performance_metrics,
            'performance_trends': self._analyze_performance_trends(performance_metrics),
            'efficiency_analysis': self._analyze_efficiency(performance_metrics),
        }
    
    def _analyze_performance_trends(self, performance_metrics: List[Dict[str, Any]]) -> Dict[str, Any]:nlm.SensoryPercept:
        """Analyze performance trends."""
        if not performance_metrics:
            return {}
        
        # Extract metrics
        efficiencies = [pm.get('efficiency_score', 0) for pm in performance_metrics]
        success_rates = [pm.get('success_rate', 0) for pm in performance_metrics]
        rewards = [pm.get('total_reward', 0) for pm in performance_metrics]
        
        # Calculate trends
        return {
            'efficiency_trend': self._calculate_trend(
                list(range(len(performance_metrics))),
                efficiencies
            ),
            'success_trend': self._calculate_trend(
                list(range(len(performance_metrics))),
                success_rates
            ),
            'reward_trend': self._calculate_trend(
                list(range(len(performance_metrics))),
                rewards
            ),
        }
    
    def _analyze_efficiency(self, performance_metrics: List[Dict[str, Any]]) -> Dict[str, Any]:nlm.SensoryPercept:
        """Analyze efficiency metrics."""
        if not performance_metrics:
            return {}
        
        # Calculate efficiency statistics
        efficiencies = [pm.get('efficiency_score', 0) for pm in performance_metrics]
        
        # Efficiency quartiles
        q1, q2, q3 = np.percentile(efficiencies, [25, 50, 75])
        
        # Identify high and low efficiency episodes
        high_efficiency = [pm for pm in performance_metrics if pm.get('efficiency_score', 0) > q2]
        low_efficiency = [pm for pm in performance_metrics if pm.get('efficiency_score', 0) < q2]
        
        # Analyze patterns in high/low efficiency episodes
        high_efficiency_patterns = self._analyze_efficiency_patterns(high_efficiency)
        low_efficiency_patterns = self._analyze_efficiency_patterns(low_efficiency)
        
        return {
            'efficiency_statistics': {
                'mean': np.mean(efficiencies),
                'median': np.median(efficiencies),
                'std': np.std(efficiencies),
                'min': np.min(efficiencies),
                'max': np.max(efficiencies),
                'q1': q1,
                'q2': q2,
                'q3': q3,
            },
            'high_efficiency_episodes': len(high_efficiency),
            'low_efficiency_episodes': len(low_efficiency),
            'high_efficiency_patterns': high_efficiency_patterns,
            'low_efficiency_patterns': low_efficiency_patterns,
        }
    
    def _analyze_efficiency_patterns(self, metrics: List[Dict[str, Any]]) -> Dict[str, Any]:nlm.SensoryPercept:
        """Analyze patterns in efficiency metrics."""
        if not metrics:
            return {}
        
        patterns = {}
        
        # Extract features
        episode_ids = [m.get('episode_id', 0) for m in metrics]
        success_rates = [m.get('success_rate', 0) for m in metrics]
        total_rewards = [m.get('total_reward', 0) for m in metrics]
        steps = [m.get('total_steps', 0) for m in metrics]
        
        # Calculate correlations
        if len(episode_ids) >= 2:
            success_reward_corr = np.corrcoef(success_rates, total_rewards)[0, 1] if len(success_rates) > 1 else 0
            success_steps_corr = np.corrcoef(success_rates, steps)[0, 1] if len(success_rates) > 1 else 0
            efficiency_reward_corr = np.corrcoef(
                [m.get('efficiency_score', 0) for m in metrics],
                total_rewards
            )[0, 1] if len(metrics) > 1 else 0
            
            patterns['correlations'] = {
                'success_reward': success_reward_corr,
                'success_steps': success_steps_corr,
                'efficiency_reward': efficiency_reward_corr,
            }
        
        return patterns
    
    def _analyze_learning_curve(self, episode_data: List[Dict[str, Any]]) -> Dict[str, Any]:nlm.SensoryPercept:
        """Analyze learning curve."""
        if len(episode_data) < 2:
            return {}
        
        # Extract learning metrics
        episode_numbers = list(range(len(episode_data)))
        rewards = [ep.get('total_reward', 0) for ep in episode_data]
        scores = [ep.get('episode_score', 0) for ep in episode_data]
        
        # Calculate learning rate
        if len(rewards) >= 2:
            # Fit exponential learning curve
            from scipy.optimize import curve_fit
            
            def learning_curve(x, a, b, c):
                return a * (1 - np.exp(-b * x)) + c
            
            try:
                popt, pcov = curve_fit(learning_curve, episode_numbers, rewards, maxfev=1000)
                fitted_rewards = learning_curve(np.array(episode_numbers), *popt)
                
                learning_metrics = {
                    'learning_rate': popt[1],
                    'asymptote': popt[0] + popt[2],
                    'initial_performance': popt[2],
                    'goodness_of_fit': 1 - np.sum((rewards - fitted_rewards)**2) / np.sum((rewards - np.mean(rewards))**2),
                    'fitted_curve': fitted_rewards.tolist(),
                }
            except Exception as e:
                print(f"⚠ Could not fit learning curve: {e}")
                learning_metrics = {
                    'learning_rate': 0.0,
                    'asymptote': np.mean(rewards),
                    'initial_performance': rewards[0] if rewards else 0,
                    'goodness_of_fit': 0.0,
                }
        else:
            learning_metrics = {
                'learning_rate': 0.0,
                'asymptote': rewards[0] if rewards else 0,
                'initial_performance': rewards[0] if rewards else 0,
                'goodness_of_fit': 0.0,
            }
        
        return learning_metrics
    
    def _analyze_convergence(self, episode_data: List[Dict[str, Any]]) -> Dict[str, Any]:nlm.SensoryPercept:
        """Analyze convergence of learning."""
        if len(episode_data) < 2:
            return {}
        
        # Extract convergence metrics
        rewards = [ep.get('total_reward', 0) for ep in episode_data]
        
        # Calculate convergence statistics
        convergence_metrics = {}
        
        # Calculate standard deviation of rewards
        convergence_metrics['reward_std'] = np.std(rewards)
        
        # Calculate coefficient of variation
        if np.mean(rewards) > 0:
            convergence_metrics['coefficient_of_variation'] = np.std(rewards) / np.mean(rewards)
        else:
            convergence_metrics['coefficient_of_variation'] = 0.0
        
        # Check for convergence using rolling windows
        window_size = min(5, len(rewards) // 2)
        if window_size > 1:
            rolling_means = []
            rolling_stds = []
            
            for i in range(len(rewards) - window_size + 1):
                window_rewards = rewards[i:i + window_size]
                rolling_means.append(np.mean(window_rewards))
                rolling_stds.append(np.std(window_rewards))
            
            convergence_metrics['converged'] = np.std(rolling_means) < 0.1 * np.mean(rolling_means)
            convergence_metrics['convergence_window'] = window_size
            convergence_metrics['rolling_mean_stdev'] = np.std(rolling_means)
        
        return convergence_metrics
    
    def create_visualization(self, analysis_results: Dict[str, Any], plot_type: str = "comprehensive") -> Any:
        """
        Create visualization from analysis results.
        
        Args:
            analysis_results: Analysis results to visualize
            plot_type: Type of visualization to create
            
        Returns:
            Visualization object
        """
        print(f"Creating visualization: {plot_type}")
        
        try:
            if plot_type == "learning_curve" and 'learning_curve' in analysis_results:
                return self._create_learning_curve_plot(analysis_results['learning_curve'])
            
            elif plot_type == "performance_metrics" and 'performance_metrics' in analysis_results:
                return self._create_performance_metrics_plot(analysis_results['performance_metrics'])
            
            elif plot_type == "behavior_patterns" and 'behavioral_analysis' in analysis_results:
                return self._create_behavior_patterns_plot(analysis_results['behavioral_analysis'])
            
            elif plot_type == "temporal_analysis" and 'temporal_patterns' in analysis_results:
                return self._create_temporal_analysis_plot(analysis_results['temporal_patterns'])
            
            elif plot_type == "comprehensive":
                return self._create_comprehensive_visualization(analysis_results)
            
            else:
                print(f"⚠ Unknown plot type: {plot_type}")
                return None
                
        except Exception as e:
            print(f"⚠ Could not create visualization: {e}")
            return None
    
    def _create_learning_curve_plot(self, learning_curve_data: Dict[str, Any]) -> Any:
        """Create learning curve plot."""
        try:
            fig = go.Figure()
            
            # Add fitted curve
            x_values = list(range(len(learning_curve_data.get('fitted_curve', []))))
            y_values = learning_curve_data.get('fitted_curve', [])
            
            fig.add_trace(go.Scatter(
                x=x_values,
                y=y_values,
                mode='lines+markers',
                name='Fitted Learning Curve',
                line=dict(color='blue', width=2)
            ))
            
            # Add parameters as annotations
            params_text = (f"Learning Rate: {learning_curve_data.get('learning_rate', 0):.3f}<br>"
                         f"Asymptote: {learning_curve_data.get('asymptote', 0):.2f}<br>"
                         f"Goodness of Fit: {learning_curve_data.get('goodness_of_fit', 0):.3f}")
            
            fig.add_annotation(
                x=max(x_values) if x_values else 0,
                y=max(y_values) if y_values else 0,
                text=params_text,
                showarrow=False,
                bgcolor='white',
                bordercolor='black',
                borderwidth=1
            )
            
            fig.update_layout(
                title='Learning Curve Analysis',
                xaxis_title='Episode Number',
                yaxis_title='Total Reward',
                width=800,
                height=600
            )
            
            return fig
            
        except Exception as e:
            print(f"⚠ Could not create learning curve plot: {e}")
            return None
    
    def _create_performance_metrics_plot(self, performance_metrics: List[Dict[str, Any]]) -> Any:
        """Create performance metrics plot."""
        try:
            if not performance_metrics:
                return None
            
            fig = go.Figure()
            
            # Create subplots
            metrics = [
                ('total_reward', 'Total Reward', ' Reward'),
                ('efficiency_score', 'Efficiency Score', ''),
                ('success_rate', 'Success Rate', '%'),
                ('total_steps', 'Steps per Episode', ''),
            ]
            
            for i, (metric_name, metric_title, metric_suffix) in enumerate(metrics):
                values = [pm.get(metric_name, 0) for pm in performance_metrics]
                
                fig.add_trace(go.Scatter(
                    x=list(range(len(performance_metrics))),
                    y=values,
                    mode='lines+markers',
                    name=metric_title,
                    yaxis=f'y{i+1}'
                ))
            
            # Update layout
            fig.update_layout(
                title='Performance Metrics Over Time',
                xaxis_title='Episode Number',
                height=600,
                showlegend=True
            )
            
            return fig
            
        except Exception as e:
            print(f"⚠ Could not create performance metrics plot: {e}")
            return None
    
    def _create_behavior_patterns_plot(self, behavioral_analysis: Dict[str, Any]) -> Any:
        """Create behavior patterns plot."""
        try:
            fig = go.Figure()
            
            # Add action distribution
            if 'action_distribution' in behavioral_analysis:
                action_counts = behavioral_analysis['action_distribution']
                
                fig.add_trace(go.Bar(
                    x=list(action_counts.keys()),
                    y=list(action_counts.values()),
                    name='Action Frequency',
                    marker_color='lightblue'
                ))
            
            fig.update_layout(
                title='Behavior Pattern Analysis',
                xaxis_title='Actions',
                yaxis_title='Frequency',
                width=800,
                height=600
            )
            
            return fig
            
        except Exception as e:
            print(f"⚠ Could not create behavior patterns plot: {e}")
            return None
    
    def _create_temporal_analysis_plot(self, temporal_analysis: Dict[str, Any]) -> Any:
        """Create temporal analysis plot."nlm.SensoryPercept:
        Try to create temporal analysis plot.
        """
        try:
            fig = go.Figure()
            
            # Add trend plots
            if 'reward_trend' in temporal_analysis:
                trend_data = temporal_analysis['reward_trend']
                x_values = list(range(len(trend_data.get('slope', []))))
                y_values = []
                
                # Recreate trend values
                for i in range(len(x_values)):
                    y_values.append(trend_data.get('slope', 0) * i + trend_data.get('intercept', 0))
            
            fig.add_trace(go.Scatter(
                x=x_values,
                y=y_values,
                mode='lines+markers',
                name='Reward Trend',
                line=dict(color='green', width=2)
            ))
            
            fig.update_layout(
                title='Temporal Pattern Analysis',
                xaxis_title='Episode Number',
                yaxis_title='Value',
                width=800,
                height=600
            )
            
            return fig
            
        except Exception as e:
            print(f"⚠ Could not create temporal analysis plot: {e}")
            return None
    
    def _create_comprehensive_visualization(self, analysis_results: Dict[str, Any]) -> Any:
        """Create comprehensive visualization."""
        try:
            # Create a comprehensive dashboard
            fig = go.Figure()
            
            # Add multiple subplots
            # This is a simplified version - in practice would use grid layouts
            
            fig.update_layout(
                title='Comprehensive NLM Analysis Dashboard',
                width=1200,
                height=800
            )
            
            return fig
            
        except Exception as e:
            print(f"⚠ Could not create comprehensive visualization: {e}")
            return None
class RealTimeDataIntegration:
    """
    Real-time data integration for:
    - Live data streaming
    - Real-time processing
    - Online learning
    - Dynamic adaptation
    """
    
    def __init__(self, stream_source: str = "zmq"):
        """
        Initialize real-time data integration.
        
        Args:
            stream_source: Source of real-time data
        """
        print(f"Initializing Real-Time Data Integration ({stream_source})...")
        
        self.stream_source = stream_source
        self.data_queue = queue.Queue()
        self.is_running = False
        self.stream_threads = []
        self.processors = {}
        
        # Initialize stream-specific components
        if stream_source == "zmq":
            self._setup_zmq()
        elif stream_source == "kafka":
            self._setup_kafka()
        elif stream_source == "mqtt":
            self._setup_mqtt()
        elif stream_source == "pubsub":
            self._setup_pubsub()
        
        print("✓ Real-time data integration initialized")
    
    def _setup_zmq(self):
        """Setup ZeroMQ integration."""
        try:
            import zmq
            
            self.zmq = zmq
            print("✓ ZeroMQ components loaded")
        except ImportError:
            print("⚠ ZeroMQ not available")
            self.zmq = None
    
    def _setup_kafka(self):
        """Setup Kafka integration."""
        try:
            import kafka
            
            self.kafka = kafka
            print("✓ Kafka components loaded")
        except ImportError:
            print("⚠ Kafka not available")
            self.kafka = None
    
    def _setup_mqtt(self):
        """Setup MQTT integration."""
        try:
            import paho.mqtt.client as mqtt
            
            self.mqtt = mqtt
            print("✓ MQTT components loaded")
        except ImportError:
            print("⚠ MQTT not available")
            self.mqtt = None
    
    def _setup_pubsub(self):
        """Setup Pub/Sub integration."""
        try:
            import redis
            from redis import pubsub
            
            self.redis = redis
            self.pubsub = pubsub
            print("✓ Pub/Sub components loaded")
        except ImportError:
            print("⚠ Pub/Sub not available")
            self.redis = None
            self.pubsub = None
    
    def start_streaming(self, data_callback: Callable, stream_config: Dict[str, Any]):nlm.SensoryPercept:
        """
        Start real-time data streaming.
        
        Args:
            data_callback: Callback function for processed data
            stream_config: Stream configuration
            
        Returns:
            True if streaming started successfully
        """
        try:
            self.is_running = True
            
            # Start streaming thread
            stream_thread = threading.Thread(
                target=self._stream_data,
                args=(data_callback, stream_config),
                daemon=True
            )
            
            stream_thread.start()
            self.stream_threads.append(stream_thread)
            
            print("✓ Real-time streaming started")
            return True
            
        except Exception as e:
            print(f"✗ Could not start streaming: {e}")
            return False
    
    def _stream_data(self, data_callback: Callable, stream_config: Dict[str, Any]):nlm.SensoryPercept:
        """Internal streaming method."""
        print(f"  Stream source: {self.stream_source}")
        print(f"  Stream config: {stream_config}")
        
        # Simulate streaming (would use actual stream implementation)
        # For demonstration, generate sample data
        for i in range(100):  # Generate 100 data points
            # Create sample data
            sample_data = {
                'timestamp': time.time(),
                'sequence_id': i,
                'sensor_readings': {
                    'temperature': 20.0 + 5.0 * np.sin(time.time() / 10),
                    'pressure': 1.0 + 0.1 * np.random.randn(),
                    'humidity': 0.5 + 0.2 * np.random.random(),
                },
                'neural_signals': {
                    'firing_rate': 10.0 + 2.0 * np.random.randn(),
                    'burst_frequency': 0.5 + 0.1 * np.random.random(),
                },
                'contextual_info': {
                    'environment_id': stream_config.get('env_id', 'default'),
                    'agent_id': stream_config.get('agent_id', 'default'),
                    'session_id': stream_config.get('session_id', 'default'),
                }
            }
            
            # Send data to callback
            try:
                data_callback(sample_data)
            except Exception as e:
                print(f"  ⚠ Data callback error: {e}")
            
            # Wait for next data point
            time.sleep(stream_config.get('sample_interval', 0.1))
    
    def add_processor(self, processor_name: str, processor_func: Callable):
        """
        Add data processor for streaming data.
        
        Args:
            processor_name: Name of processor
            processor_func: Processor function
        """
        self.processors[processor_name] = processor_func
        print(f"✓ Added processor: {processor_name}")
    
    def process_stream_data(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """
        Process data through all registered processors.
        
        Args:
            data: Data to process
            
        Returns:
            Processed data
        """
        processed_data = data.copy()
        
        # Apply all processors
        for processor_name, processor_func in self.processors.items():
            try:
                processed_data = processor_func(processed_data)
                print(f"  Applied processor: {processor_name}")
            except Exception as e:
                print(f"  ⚠ Processor error ({processor_name}): {e}")
        
        return processed_data
    
    def stop_streaming(self):
        """Stop real-time streaming."""
        self.is_running = False
        
        # Wait for threads to finish
        for thread in self.stream_threads:
            thread.join(timeout=1.0)
        
        self.stream_threads.clear()
        
        print("✓ Real-time streaming stopped")
class RoboticsIntegration:
    """
    Integration with robotics systems for:
    - Control system integration
    - Sensor data processing
    - Motor command conversion
    - Real-world interaction
    """
    
    def __init__(self, robot_type: str = "differential_drive"):
        """
        Initialize robotics integration.
        
        Args:
            robot_type: Type of robot to integrate with
        """
        print(f"Initializing Robotics Integration ({robot_type})...")
        
        self.robot_type = robot_type
        self.robot_state = {}
        self.control_systems = {}
        self.sensor_data = {}
        self.motor_commands = {}
        
        # Initialize robot-specific components
        if robot_type == "differential_drive":
            self._setup_differential_drive()
        elif robot_type == "quadrotor":
            self._setup_quadrotor()
        elif robot_type == "arm":
            self._setup_robot_arm()
        elif robot_type == "maze_solver":
            self._setup_maze_solver()
        
        print("✓ Robotics integration initialized")
    
    def _setup_differential_drive(self):
        """Setup differential drive robot integration."""
        # Differential drive parameters
        self.differential_params = {
            'wheel_radius': 0.05,  # meters
            'wheel_base': 0.3,     # distance between wheels
            'max_speed': 1.0,      # m/s
            "steering_angle_limit": 0.5,  # radians
        }
        
        print("✓ Differential drive setup complete")
    
    def _setup_quadrotor(self):
        """Setup quadrotor integration."""
        # Quadrotor parameters
        self.quadrotor_params = {
            'mass': 0.5,           # kg
            'arm_length': 0.2,    # meters
            'max_thrust': 10.0,   # Newtons
            'drag_coefficient': 0.1,
            'max_angular_velocity': 10.0,  # rad/s
        }
        
        print("✓ Quadrotor setup complete")
    
    def _setup_robot_arm(self):
        """Setup robot arm integration."""
        # Robot arm parameters
        self.arm_params = {
            'num_joints': 6,
            'joint_limits': [(-180, 180)] * 6,  # degrees
            'max_torque': 10.0,  # Nm
            'speed_limits': [(-90, 90)] * 6,  # degrees per second
        }
        
        print("✓ Robot arm setup complete")
    
    def _setup_maze_solver(self):
        """Setup maze solving robot integration."""
        # Maze solver parameters
        self.maze_params = {
            'sensor_range': 2.0,    # meters
            'wall_detection_threshold': 0.7,
            'turn_angle': 90,       # degrees
            'step_size': 0.1,       # meters
        }
        
        print("✓ Maze solver setup complete")
    
    def convert_nlm_action_to_robot_command(self, nlm_action: Any, robot_config: Dict[str, Any]) -> Dict[str, Any]:nlm.SensoryPercept:
        """
        Convert NLM action to robot command.
        
        Args:
            nlm_action: NLM action
            robot_config: Robot configuration
            
        Returns:
            Robot command
        """
        if self.robot_type == "differential_drive":
            return self._convert_to_differential_drive_command(nlm_action, robot_config)
        elif self.robot_type == "quadrotor":
            return self._convert_to_quadrotor_command(nlm_action, robot_config)
        elif self.robot_type == "arm":
            return self._convert_to_arm_command(nlm_action, robot_config)
        elif self.robot_type == "maze_solver":
            return self._convert_to_maze_solver_command(nlm_action, robot_config)
        else:
            return self._convert_to_generic_command(nlm_action, robot_config)
    
    def _convert_to_differential_drive_command(self, nlm_action: Any, robot_config: Dict[str, Any]) -> Dict[str, Any]:nlm.SensoryPercept:
        """Convert NLM action to differential drive command."""
        # Map NLM actions to wheel commands
        action_mapping = {
            nlm.MotorCommand.MoveForward: {'left_speed': 0.8, 'right_speed': 0.8},
            nlm.MotorCommand.MoveBackward: {'left_speed': -0.8, 'right_speed': -0.8},
            nlm.MotorCommand.TurnLeft: {'left_speed': -0.5, 'right_speed': 0.5},
            nlm.MotorCommand.TurnRight: {'left_speed': 0.5, 'right_speed': -0.5},
            nlm.MotorCommand.LookLeft: {'left_speed': 0.2, 'right_speed': 0.1},
            nlm.MotorCommand.LookRight: {'left_speed': 0.1, 'right_speed': 0.2},
            nlm.MotorCommand.Interact: {'left_speed': 0.0, 'right_speed': 0.0, 'grip': True},
            nlm.MotorCommand.Wait: {'left_speed': 0.0, 'right_speed': 0.0},
        }
        
        # Get base command
        command = action_mapping.get(nlm_action, {'left_speed': 0.0, 'right_speed': 0.0})
        
        # Apply robot-specific scaling
        for key, value in command.items():
            if 'speed' in key:
                command[key] = self._apply_speed_limits(value, robot_config)
        
        return {
            'command_type': 'differential_drive',
            'parameters': command,
            'timestamp': time.time(),
            'robot_id': robot_config.get('robot_id', 'default'),
        }
    
    def _convert_to_quadrotor_command(self, nlm_action: Any, robot_config: Dict[str, Any]) -> Dict[str, Any]:nlm.SensoryPercept:
        """Convert NLM action to quadrotor command."""
        # Define quadrotor control parameters
        throttle_map = {
            nlm.MotorCommand.MoveForward: 0.6,
            nlm.MotorCommand.MoveBackward: 0.4,
            nlm.MotorCommand.TurnLeft: 0.5,
            nlm.MotorCommand.TurnRight: 0.5,
            nlm.MotorCommand.LookLeft: 0.55,
            nlm.MotorCommand.LookRight: 0.55,
            nlm.MotorCommand.Interact: 0.5,
            nlm.MotorCommand.Wait: 0.3,
        }
        
        # Get base control
        throttle = throttle_map.get(nlm_action, 0.5)
        
        # Calculate motor speeds
        base_speed = throttle * self.quadrotor_params['max_thrust'] / 4
        motor_speeds = [base_speed] * 4
        
        # Apply rotation for turning
        if nlm_action == nlm.MotorCommand.TurnLeft:
            motor_speeds[0] *= 1.2
            motor_speeds[1] *= 1.2
        elif nlm_action == nlm.MotorCommand.TurnRight:
            motor_speeds[2] *= 1.2
            motor_speeds[3] *= 1.2
        
        return {
            'command_type': 'quadrotor',
            'parameters': {
                'motor_speeds': motor_speeds,
                'throttle': throttle,
            },
            'timestamp': time.time(),
            'robot_id': robot_config.get('robot_id', 'default'),
        }
    
    def _convert_to_arm_command(self, nlm_action: Any, robot_config: Dict[str, Any]) -> Dict[str, Any]:nlm.SensoryPercept:
        """Convert NLM action to robot arm command."""
        # Define arm joint commands
        joint_commands = {
            nlm.MotorCommand.MoveForward: {'joint_1': 45, 'joint_2': 0, 'joint_3': 0},
            nlm.MotorCommand.MoveBackward: {'joint_1': -45, 'joint_2': 0, 'joint_3': 0},
            nlm.MotorCommand.TurnLeft: {'joint_1': 0, 'joint_2': 45, 'joint_3': 0},
            nlm.MotorCommand.TurnRight: {'joint_1': 0, 'joint_2': -45, 'joint_3': 0},
            nlm.MotorCommand.LookLeft: {'joint_1': 0, 'joint_2': 0, 'joint_3': 45},
            nlm.MotorCommand.LookRight: {'joint_1': 0, 'joint_2': 0, 'joint_3': -45},
            nlm.MotorCommand.Interact: {'joint_1': 0, 'joint_2': 0, 'joint_3': 0, 'gripper': True},
            nlm.MotorCommand.Wait: {'joint_1': 0, 'joint_2': 0, 'joint_3': 0},
        }
        
        return {
            'command_type': 'robot_arm',
            'parameters': joint_commands.get(nlm_action, {'joint_1': 0, 'joint_2': 0, 'joint_3': 0}),
            'timestamp': time.time(),
            'robot_id': robot_config.get('robot_id', 'default'),
        }
    
    def _convert_to_maze_solver_command(self, nlm_action: Any, robot_config: Dict[str, Any]) -> Dict[str, Any]:nlm.SensoryPercept:
        """Convert NLM action to maze solver command."""
        # Maze solver movement commands
        movement_commands = {
            nlm.MotorCommand.MoveForward: 'forward',
            nlm.MotorCommand.MoveBackward: 'backward',
            nlm.MotorCommand.TurnLeft: 'left',
            nlm.MotorCommand.TurnRight: 'right',
            nlm.MotorCommand.LookLeft: 'scan_left',
            nlm.MotorCommand.LookRight: 'scan_right',
            nlm.MotorCommand.Interact: 'interact',
            nlm.MotorCommand.Wait: 'idle',
        }
        
        # Get base command
        base_command = movement_commands.get(nlm_action, 'idle')
        
        # Get current sensor data
        sensor_data = self._get_sensor_data()
        
        # Determine if movement is safe
        safe_to_move = self._is_safe_to_move(base_command, sensor_data)
        
        return {
            'command_type': 'maze_solver',
            'parameters': {
                'action': base_command,
                'safe_to_move': safe_to_move,
                'sensor_data': sensor_data,
            },
            'timestamp': time.time(),
            'robot_id': robot_config.get('robot_id', 'default'),
        }
    
    def _convert_to_generic_command(self, nlm_action: Any, robot_config: Dict[str, Any]) -> Dict[str, Any]:nlm.SensoryPercept:
        """Convert NLM action to generic robot command."""
        return {
            'command_type': 'generic',
            'parameters': {
                'nlm_action': str(nlm_action),
                'timestamp': time.time(),
            },
            'robot_id': robot_config.get('robot_id', 'default'),
        }
    
    def _apply_speed_limits(self, speed: float, robot_config: Dict[str, Any]) -> float:
        """Apply speed limits to command."""
        max_speed = robot_config.get('max_speed', self.differential_params['max_speed'])
        
        # Clamp speed to valid range
        return max(-max_speed, min(max_speed, speed))
    
    def _get_sensor_data(self) -> Dict[str, Any]:
        """Get current sensor data."""
        # Simulate sensor readings
        return {
            'front_distance': np.random.uniform(0.1, 2.0),
            'left_distance': np.random.uniform(0.1, 2.0),
            'right_distance': np.random.uniform(0.1, 2.0),
            'back_distance': np.random.uniform(0.1, 2.0),
            'wall_detected': np.random.random() < 0.3,
            'obstacle_detected': np.random.random() < 0.2,
        }
    
    def _is_safe_to_move(self, action: str, sensor_data: Dict[str, Any]) -> bool:
        """Check if movement is safe based on sensor data."""
        if not sensor_data.get('wall_detected', False):
            return True
        
        # Check if action would cause collision
        if action == 'forward' and sensor_data['front_distance'] < self.maze_params['wall_detection_threshold']:
            return False
        
        if action == 'left' and sensor_data['left_distance'] < self.maze_params['wall_detection_threshold']:
            return False
        
        if action == 'right' and sensor_data['right_distance'] < self.maze_params['wall_detection_threshold']:
            return False
        
        return True
class NeuroscienceRecordingIntegration:
    """
    Integration with neuroscience recording systems for:
    - Real-time neural recording
    - Spike sorting
    - Signal processing
    - Data management
    """
    
    def __init__(self, recording_type: str = "extracellular"):
        """
        Initialize neuroscience recording integration.
        
        Args:
            recording_type: Type of recording to perform
        """
        print(f"Initializing Neuroscience Recording Integration ({recording_type})...")
        
        self.recording_type = recording_type
        self.recording_status = 'idle'
        self.recorded_data = []
        self.spike_detectors = {}
        self.data_processors = {}
        
        # Initialize recording-specific components
        if recording_type == "extracellular":
            self._setup_extracellular_recording()
        elif recording_type == "intracellular":
            self._setup_intracellular_recording()
        elif recording_type == "optical":
            self._setup_optical_recording()
        
        print("✓ Neuroscience recording integration initialized")
    
    def _setup_extracellular_recording(self):
        """Setup extracellular recording."""
        self.recording_params = {
            'sampling_rate': 20000,  # Hz
            'channel_count': 64,
            'electrode_spacing': 50,  # microns
            'recording_duration': 300,  # seconds
            'noise_threshold': 3.0,  # standard deviations
        }
        
        print("✓ Extracellular recording setup complete")
    
    def _setup_intracellular_recording(self):
        """Setup intracellular recording."""
        self.recording_params = {
            'sampling_rate': 20000,  # Hz
            'channel_count': 16,
            'access_resistance': 100,  # MOhm
            'recording_duration': 600,  # seconds
        }
        
        print("✓ Intracellular recording setup complete")
    
    def _setup_optical_recording(self):
        """Setup optical recording."""
        self.recording_params = {
            'frame_rate': 60,  # Hz
            'field_of_view': 100,  # micrometers
            'pixel_size': 0.1,  # micrometers
            'recording_duration': 180,  # seconds
        }
        
        print("✓ Optical recording setup complete")
    
    def start_recording(self, recording_config: Dict[str, Any]) -> bool:
        """
        Start neuroscience recording.
        
        Args:
            recording_config: Recording configuration
            
        Returns:
            True if recording started successfully
        """
        try:
            self.recording_status = 'recording'
            self.recording_start_time = time.time()
            
            # Start recording thread
            recording_thread = threading.Thread(
                target=self._record_data,
                args=(recording_config,),
                daemon=True
            )
            
            recording_thread.start()
            self.recording_thread = recording_thread
            
            print("✓ Neuroscience recording started")
            return True
            
        except Exception as e:
            print(f"✗ Could not start recording: {e}")
            return False
    
    def _record_data(self, recording_config: Dict[str, Any]):nlm.SensoryPercept:
        """Internal recording method."""
        print(f"  Recording type: {self.recording_type}")
        print(f"  Recording config: {recording_config}")
        
        # Simulate recording (would use actual recording hardware)
        # For demonstration, generate sample neural data
        duration = recording_config.get('duration', self.recording_params['recording_duration'])
        
        for i in range(int(duration * self.recording_params['sampling_rate'])):
            # Generate sample neural data
            sample_data = {
                'timestamp': time.time(),
                'sample_id': i,
                'channel_data': {
                    f'channel_{j}': np.random.normal(0, 1) for j in range(self.recording_params['channel_count'])
                },
                'spike_events': self._detect_spikes(sample_data['channel_data']),
                'filtering': self._apply_signal_filtering(sample_data['channel_data']),
                'recording_quality': self._assess_recording_quality(sample_data),
            }
            
            # Store recorded data
            self.recorded_data.append(sample_data)
            
            # Wait for next sample
            time.sleep(1.0 / self.recording_params['sampling_rate'])
    
    def _detect_spikes(self, channel_data: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Detect spikes from channel data."""
        spikes = []
        
        for channel_id, data in channel_data.items():
            # Simple spike detection (threshold crossing)
            threshold = self.recording_params.get('noise_threshold', 3.0)
            
            for i in range(1, len(data)):
                if abs(data[i]) > threshold and abs(data[i-1]) <= threshold:
                    spikes.append({
                        'channel_id': channel_id,
                        'timestamp': i / self.recording_params['sampling_rate'],
                        'amplitude': data[i],
                        'waveform': data[max(0, i-5):i+5],  # Sample waveform
                    })
        
        return spikes
    
    def _apply_signal_filtering(self, channel_data: Dict[str, Any]) -> Dict[str, Any]:
        """Apply signal filtering to reduce noise."""
        filtered_data = {}
        
        for channel_id, data in channel_data.items():
            # Apply bandpass filter (0.5 - 10 kHz)
            # For demonstration, use simple moving average
            window_size = 5
            filtered = np.convolve(data, np.ones(window_size)/window_size, mode='same')
            filtered_data[channel_id] = filtered.tolist()
        
        return filtered_data
    
    def _assess_recording_quality(self, sample_data: Dict[str, Any]) -> Dict[str, Any]:nlm.SensoryPercept:
        """Assess quality of recording."""
        channel_data = sample_data['channel_data']
        
        quality_metrics = {
            'signal_to_noise_ratio': 0.0,
            'stability_index': 0.0,
            'artifact_level': 0.0,
        }
        
        # Calculate signal-to-noise ratio
        all_data = np.concatenate([np.array(list(channel_data.values()))])
        signal_power = np.mean(all_data ** 2)
        noise_power = np.var(all_data)
        
        if noise_power > 0:
            quality_metrics['signal_to_noise_ratio'] = 10 * np.log10(signal_power / noise_power)
        
        return quality_metrics
    
    def stop_recording(self):
        """Stop neuroscience recording."""
        self.recording_status = 'stopped'
        
        # Wait for recording thread to finish
        if hasattr(self, 'recording_thread'):
            self.recording_thread.join(timeout=1.0)
        
        print("✓ Neuroscience recording stopped")
        print(f"  Total recorded samples: {len(self.recorded_data)}")
        
        return len(self.recorded_data)
    
    def add_spike_detector(self, detector_name: str, detector_func: Callable):
        """
        Add spike detector for processing recorded data.
        
        Args:
            detector_name: Name of spike detector
            detector_func: Spike detector function
        """
        self.spike_detectors[detector_name] = detector_func
        print(f"✓ Added spike detector: {detector_name}")
    
    def add_data_processor(self, processor_name: str, processor_func: Callable):
        """
        Add data processor for recorded data.
        
        Args:
            processor_name: Name of data processor
            processor_func: Data processor function
        """
        self.data_processors[processor_name] = processor_func
        print(f"✓ Added data processor: {processor_name}")
    
    def analyze_recorded_data(self) -> Dict[str, Any]:nlm.SensoryPercept:
        """
        Analyze recorded neural data.
        
        Returns:
            Dictionary with analysis results
        """
        print("Analyzing recorded neural data...")
        
        analysis = {
            'recording_summary': self._analyze_recording_summary(),
            'spike_analysis': self._analyze_spike_data(),
            'frequency_analysis': self._analyze_frequency_data(),
            'quality_report': self._generate_quality_report(),
        }
        
        # Apply processors
        for processor_name, processor_func in self.data_processors.items():
            try:
                analysis = processor_func(analysis)
                print(f"  Applied processor: {processor_name}")
            except Exception as e:
                print(f"  ⚠ Processor error ({processor_name}): {e}")
        
        print("✓ Recorded data analysis complete")
        return analysis
    
    def _analyze_recording_summary(self) -> Dict[str, Any]:nlm.SensoryPercept:
        """Analyze recording summary."""
        if not self.recorded_data:
            return {}
        
        total_samples = len(self.recorded_data)
        duration = time.time() - self.recording_start_time if self.recording_status == 'recording' else 300
        
        # Calculate average samples per second
        sampling_rate = total_samples / duration if duration > 0 else self.recording_params['sampling_rate']
        
        return {
            'total_samples': total_samples,
            'duration_seconds': duration,
            'actual_sampling_rate': sampling_rate,
            'target_sampling_rate': self.recording_params['sampling_rate'],
            'data_completeness': min(1.0, total_samples / (self.recording_params['recording_duration'] * sampling_rate)),
        }
    
    def _analyze_spike_data(self) -> Dict[str, Any]:nlm.SensoryPercept:
        """Analyze spike data."""
        all_spikes = []
        
        for sample in self.recorded_data:
            all_spikes.extend(sample.get('spike_events', []))
        
        if not all_spikes:
            return {}
        
        # Analyze spike statistics
        spikes_by_channel = {}
        for spike in all_spikes:
            channel_id = spike['channel_id']
            if channel_id not in spikes_by_channel:
                spikes_by_channel[channel_id] = []
            spikes_by_channel[channel_id].append(spike)
        
        analysis = {
            'total_spikes': len(all_spikes),
            'spikes_per_channel': {k: len(v) for k, v in spikes_by_channel.items()},
            'firing_rates': {k: len(v) / (self.recording_params['recording_duration'] / 60) for k, v in spikes_by_channel.items()},
        }
        
        return analysis
    
    def _analyze_frequency_data(self) -> Dict[str, Any]:nlm.SensoryPercept:
        """Analyze frequency data."""
        # Extract frequency information
        frequencies = []
        
        for sample in self.recorded_data:
            channel_data = sample.get('channel_data', {})
            for channel_id, data in channel_data.items():
                # Calculate power spectrum
                fft_data = np.fft.fft(data)
                freqs = np.fft.fftfreq(len(data), 1.0 / self.recording_params['sampling_rate'])
                
                # Get dominant frequency
                magnitude = np.abs(fft_data)
                dominant_idx = np.argmax(magnitude[1:len(magnitude)//2]) + 1
                dominant_freq = freqs[dominant_idx]
                
                frequencies.append({
                    'channel_id': channel_id,
                    'frequency': dominant_freq,
                    'magnitude': magnitude[dominant_idx],
                })
        
        if frequencies:
            dominant_frequencies = [f['frequency'] for f in frequencies]
            
            return {
                'dominant_frequencies': {
                    'mean': np.mean(dominant_frequencies),
                    'std': np.std(dominant_frequencies),
                    'min': np.min(dominant_frequencies),
                    'max': np.max(dominant_frequencies),
                },
                'frequency_distribution': self._bin_frequencies(dominant_frequencies),
            }
        
        return {}
    
    def _bin_frequencies(self, frequencies: List[float]) -> Dict[str, Any]:nlm.SensoryPercept:
        """Bin frequencies into ranges."""
        bins = {
            'slow': 0,    # < 4 Hz
            'beta': 0,    # 13-30 Hz
            'gamma': 0,   # > 30 Hz
        }
        
        for freq in frequencies:
            if abs(freq) < 4:
                bins['slow'] += 1
            elif 13 <= abs(freq) <= 30:
                bins['beta'] += 1
            else:
                bins['gamma'] += 1
        
        return bins
    
    def _generate_quality_report(self) -> Dict[str, Any]:nlm.SensoryPercept:
        """Generate quality report."""
        report = {
            'overall_quality': 'good',
            'quality_metrics': {},
            'recommendations': [],
        }
        
        # Calculate quality metrics
        if self.recorded_data:
            # Check signal-to-noise ratio
            snr_values = [sample.get('recording_quality', {}).get('signal_to_noise_ratio', 0) for sample in self.recorded_data]
            
            if snr_values:
                avg_snr = np.mean(snr_values)
                report['quality_metrics']['average_snr'] = avg_snr
                
                if avg_snr < 10:
                    report['overall_quality'] = 'poor'
                    report['recommendations'].append("Increase signal gain or improve electrode placement")
                elif avg_snr < 20:
                    report['overall_quality'] = 'fair'
                    report['recommendations'].append("Consider signal filtering")
                else:
                    report['overall_quality'] = 'good'
        
        return report
def main():
    """Main function demonstrating NLM integration examples."""
    print("Integration Examples - Connecting NLM with External Systems")
    print("=" * 80)
    
    # Demo 1: Machine Learning Integration
    print("\n" + "=" * 80)
    print("DEMO 1: MACHINE LEARNING INTEGRATION")
    print("=" * 80)
    
    ml_integration = MachineLearningIntegration(framework="sklearn")
    
    # Create sample training data
    training_data = []
    for i in range(50):
        # Create percept
        percept = nlm.SensoryPercept()
        vision_data = [np.random.random() for _ in range(256)]
        touch_data = [np.random.random() for _ in range(6)]
        internal_data = [np.random.random() for _ in range(6)]
        proprioception_data = [np.random.random() for _ in range(12)]
        
        percept.setVision(vision_data)
        percept.setTouch(touch_data)
        percept.setInternal(internal_data)
        percept.setProprioception(proprioception_data)
        
        # Create behavior label
        behavior = np.random.choice(['move_forward', 'turn_left', 'turn_right', 'wait'])
        
        training_data.append({'percept': percept, 'behavior': behavior})
    
    # Train behavior predictor
    behavior_model = ml_integration.train_behavior_predictor(training_data)
    
    # Test prediction
    test_percept = nlm.SensoryPercept()
    vision_data = [0.5] * 256
    touch_data = [0.3] * 6
    internal_data = [0.2] * 6
    proprioception_data = [0.1] * 12
    
    test_percept.setVision(vision_data)
    test_percept.setTouch(touch_data)
    test_percept.setInternal(internal_data)
    test_percept.setProprioception(proprioception_data)
    
    prediction = ml_integration.predict_behavior(test_percept)
    print(f"✓ ML Integration: Predicted behavior = {prediction['predicted_behavior']}")
    print(f"  Confidence = {prediction['confidence']:.2f}")
    
    # Demo 2: Neuroscience Toolkit Integration
    print("\n" + "=" * 80)
    print("DEMO 2: NEUROSCIENCE TOOLKIT INTEGRATION")
    print("=" * 80)
    
    neuro_integration = NeuroscienceToolkitIntegration(toolkit="netpyne")
    
    # Create sample spike data
    spike_data = []
    for i in range(10):
        spike_train = {
            'neuron_id': i,
            'times': np.sort(np.random.random(20) * 100).tolist(),  # 20 spikes over 100 time units
        }
        spike_data.append(spike_train)
    
    # Analyze spike trains
    spike_analysis = neuro_integration.analyze_spike_trains(spike_data)
    print(f"✓ Neuro Integration: Analyzed {len(spike_data)} spike trains")
    print(f"  Total spikes: {spike_analysis['total_spikes']}")
    
    # Demo 3: Simulation Environment Integration
    print("\n" + "=" * 80)
    print("DEMO 3: SIMULATION ENVIRONMENT INTEGRATION")
    print("=" * 80)
    
    env_integration = SimulationEnvironmentIntegration(environment="gym")
    
    # Create environment configuration
    env_config = {
        'world_config': {
            'width': 100,
            'height': 100,
            'vision_width': 32,
            'vision_height': 32,
            'agent_start': [50, 50],
            'max_energy': 1.0,
            'energy_decay_rate': 0.01,
            'world_objects': [
                {'x': 25, 'y': 25, 'type': 'resource', 'value': 1.0},
                {'x': 75, 'y': 75, 'type': 'hazard', 'value': -0.5},
            ],
        },
        'reward_function': {
            'type': 'curiosity_based',
        },
    }
    
    # Create NLM world from environment
    world = env_integration.create_nlm_world_from_env(env_config)
    print(f"✓ Env Integration: Created NLM world with size {world.getWidth()}x{world.getHeight()}")
    
    # Create agent brain
    agent = nlm.createAgentBrain(nlm.createBrain(nlm.createDefaultConfig()))
    agent.enableLearning(True)
    
    # Optimize environment parameters
    opt_results = env_integration.optimize_environment_parameters(agent, world, episodes=20)
    print(f"✓ Env Integration: Optimized environment parameters")
    print(f"  Best score: {opt_results['best_score']:.3f}")
    
    # Demo 4: Data Analysis Integration
    print("\n" + "=" * 80)
    print("DEMO 4: DATA ANALYSIS INTEGRATION")
    print("=" * 80)
    
    analysis_integration = DataAnalysisIntegration(analysis_type="comprehensive")
    
    # Create sample episode data
    episode_data = []
    for episode_id in range(5):
        episode = {
            'episode_id': episode_id,
            'total_reward': np.random.uniform(-10, 50),
            'max_reward': np.random.uniform(10, 50),
            'min_reward': np.random.uniform(-10, 0),
            'final_curiosity': np.random.uniform(0, 1),
            'final_novelty': np.random.uniform(0, 1),
            'final_prediction_error': np.random.uniform(0, 1),
            'steps_taken': np.random.randint(10, 100),
            'development_score': np.random.uniform(0, 100),
            'efficiency': np.random.uniform(0, 1),
            'steps': [],  # Would contain actual step data
        }
        
        # Add some steps
        for step in range(episode['steps_taken']):
            action_result = {
                'reward': np.random.uniform(-1, 10),
                'success': np.random.random() > 0.3,
                'message': 'action completed',
            }
            
            episode['steps'].append({
                'action': nlm.MotorCommand.MoveForward,
                'action_result': action_result,
                'neuromodulation_level': np.random.uniform(0, 1),
                'curiosity_level': np.random.uniform(0, 1),
                'novelty_level': np.random.uniform(0, 1),
                'prediction_error': np.random.uniform(0, 1),
                'episode_score': np.random.uniform(0, 100),
                'step_time': np.random.uniform(0.01, 0.1),
                'brain_neurons_active': np.random.randint(10, 100),
                'brain_neurons_firing': np.random.randint(5, 50),
            })
        
        episode_data.append(episode)
    
    # Analyze episode data
    analysis_results = analysis_integration.analyze_episode_data(episode_data)
    print(f"✓ Analysis Integration: Analyzed {len(episode_data)} episodes")
    print(f"  Average reward per episode: {analysis_results['summary']['average_reward_per_episode']:.2f}")
    print(f"  Reward trend: {analysis_results['temporal_patterns']['reward_trend']['trend_direction']}")
    
    # Create visualization
    visualization = analysis_integration.create_visualization(analysis_results, plot_type="learning_curve")
    print(f"✓ Analysis Integration: Created learning curve visualization")
    
    # Demo 5: Real-Time Data Integration
    print("\n" + "=" * 80)
    print("DEMO 5: REAL-TIME DATA INTEGRATION")
    print("=" * 80)
    
    realtime_integration = RealTimeDataIntegration(stream_source="zmq")
    
    # Define data callback
    def data_callback(data):
        print(f"  📥 Received data: timestamp={data['timestamp']:.2f}, sensor_temp={data['sensor_readings']['temperature']:.1f}°C")
        return data
    
    # Define processors
    def add_timestamps(data):
        data['processed_at'] = time.time()
        return data
    
    def filter_low_quality(data):
        if data['neural_signals']['firing_rate'] < 0:
            data['quality'] = 'low'
        else:
            data['quality'] = 'good'
        return data
    
    realtime_integration.add_processor("add_timestamps", add_timestamps)
    realtime_integration.add_processor("filter_low_quality", filter_low_quality)
    
    # Start streaming
    stream_config = {
        'env_id': 'demo_env',
        'agent_id': 'demo_agent',
        'session_id': 'demo_session',
        'sample_interval': 0.5,
    }
    
    realtime_integration.start_streaming(data_callback, stream_config)
    
    # Process sample data
    sample_data = {
        'timestamp': time.time(),
        'sensor_readings': {'temperature': 22.5, 'pressure': 1.0},
        'neural_signals': {'firing_rate': 15.0, 'burst_frequency': 0.8},
    }
    
    processed_data = realtime_integration.process_stream_data(sample_data)
    print(f"✓ Real-Time Integration: Processed sample data with quality: {processed_data.get('quality', 'unknown')}")
    
    # Stop streaming
    realtime_integration.stop_streaming()
    
    # Demo 6: Robotics Integration
    print("\n" + "=" * 80)
    print("DEMO 6: ROBOTICS INTEGRATION")
    print("=" * 80)
    
    robotics_integration = RoboticsIntegration(robot_type="differential_drive")
    
    # Create robot configuration
    robot_config = {
        'robot_id': 'demo_robot_1',
        'max_speed': 1.0,
    }
    
    # Convert NLM action to robot command
    nlm_action = nlm.MotorCommand.MoveForward
    robot_command = robotics_integration.convert_nlm_action_to_robot_command(nlm_action, robot_config)
    print(f"✓ Robotics Integration: Converted NLM action to robot command")
    print(f"  Command type: {robot_command['command_type']}")
    print(f"  Parameters: {robot_command['parameters']}")
    
    # Demo 7: Neuroscience Recording Integration
    print("\n" + "=" * 80)
    print("DEMO 7: NEUROSCIENCE RECORDING INTEGRATION")
    print("=" * 80)
    
    recording_integration = NeuroscienceRecordingIntegration(recording_type="extracellular")
    
    # Start recording
    recording_config = {
        'duration': 5,
        'env_id': 'demo_recording',
    }
    
    recording_integration.start_recording(recording_config)
    
    # Wait for recording
    time.sleep(6)
    
    # Stop recording
    total_samples = recording_integration.stop_recording()
    
    # Analyze recorded data
    analysis = recording_integration.analyze_recorded_data()
    print(f"✓ Recording Integration: Recorded {total_samples} samples")
    print(f"  Total spikes detected: {analysis['spike_analysis']['total_spikes']}")
    print(f"  Overall quality: {analysis['quality_report']['overall_quality']}")
    
    print("\n" + "=" * 80)
    print("INTEGRATION EXAMPLES COMPLETE")
    print("=" * 80)
    
    # Print summary
    print(f"\n🎯 INTEGRATION SUMMARY:")
    print(f"  ✓ Machine Learning Integration: Behavior prediction model trained")
    print(f"  ✓ Neuroscience Toolkit Integration: Spike train analysis completed")
    print(f"  ✓ Simulation Environment Integration: World optimization completed")
    print(f"  ✓ Data Analysis Integration: Comprehensive episode analysis completed")
    print(f"  ✓ Real-Time Data Integration: Streaming and processing completed")
    print(f"  ✓ Robotics Integration: Action conversion completed")
    print(f"  ✓ Neuroscience Recording Integration: Data recording and analysis completed")
    
    print(f"\n📁 All integration examples completed successfully!")
    print(f"   This demonstrates the comprehensive integration capabilities of NLM Python bindings.")
    
    # Save integration results
    integration_results = {
        'ml_integration': {
            'behavior_model_trained': True,
            'prediction_confidence': prediction['confidence'],
        },
        'neuro_integration': {
            'spike_analysis_completed': True,
            'total_spikes': spike_analysis['total_spikes'],
        },
        'env_integration': {
            'world_optimized': True,
            'best_score': opt_results['best_score'],
        },
        'realtime_integration': {
            'streaming_completed': True,
            'samples_processed': 1,
        },
        'robotics_integration': {
            'action_conversion_completed': True,
            'command_type': robot_command['command_type'],
        },
        'recording_integration': {
            'recording_completed': True,
            'total_samples': total_samples,
            'analysis_completed': True,
        },
    }
    
    with open("integration_results.json", "w") as f:
        json.dump(integration_results, f, indent=2, default=str)
    
    print(f"📁 Integration results saved to integration_results.json")

if __name__ == "__main__":
    main()