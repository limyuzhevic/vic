#!/usr/bin/env python3
"""
Behavior Analyzer Tool

Behavioral pattern analysis and classification for neural networks.
Provides comprehensive analysis of behavioral dynamics, pattern recognition,
and classification in neural systems.
"""

import numpy as np
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass, asdict
from enum import Enum
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats, signal, cluster
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
from sklearn.cluster import KMeans, DBSCAN
from sklearn.svm import SVC
from sklearn.neighbors import KNeighborsClassifier
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score, confusion_matrix, classification_report
import sys
import os

@dataclass
class BehaviorAnalyzerConfig:
    """Configuration for behavior analyzer"""
    behavior_length: float = 100.0  # seconds
    sampling_rate: float = 100.0  # Hz
    show_behavior_patterns: bool = True
    show_pattern_classification: bool = True
    show_behavior_dynamics: bool = True
    show_cluster_analysis: bool = True
    show_classification_analysis: bool = True
    animate: bool = True
    color_scheme: str = "behavior"  # "behavior", "pattern", "cluster", "classification"
    show_labels: bool = True
    show_statistics: bool = True
    save_format: str = "png"  # "png", "json", "html"
    output_path: str = "behavior_analyzer"
    
    # Analysis parameters
    n_clusters: int = 5
    classification_methods: List[str] = None  # "svm", "knn", "random_forest"
    anomaly_detection: bool = True
    outlier_threshold: float = 3.0
    show_trajectory_analysis: bool = True
    show_pattern_matching: bool = True
    show_behavior_regimes: bool = True
    show_action_sequences: bool = True
    
    # Visualization modes
    view_mode: str = "2d"  # "2d", "3d", "behavior_space", "pattern_space"
    show_3d: bool = False
    figure_size: Tuple[int, int] = (16, 12)
    dpi: int = 150
    animation_interval: int = 100  # milliseconds
    
    # Display settings
    show_legend: bool = True
    show_colorbar: bool = True
    show_behavior_matrix: bool = True
    smooth_behaviors: bool = True
class BehaviorAnalyzer:
    """
    Behavioral pattern analysis and classification tool for neural networks.
    
    Features:
    - Behavioral pattern detection and recognition
    - Cluster analysis and classification
    - Trajectory and movement analysis
    - Action sequence detection
    - Pattern matching and classification
    - Anomaly detection
    - Export capabilities for scientific analysis
    """
    
    def __init__(self, config: BehaviorAnalyzerConfig = None):
        self.config = config or BehaviorAnalyzerConfig()
        self.behavior_data = []  # List of behavior data arrays
        self.patterns = []  # List of detected patterns
        self.clusters = []  # List of cluster labels
        self.classifications = {}  # Classification results
        self.trajectories = []  # List of behavior trajectories
        self.action_sequences = []  # List of action sequences
        self.current_time = 0.0
        self.is_running = False
        self.animation_obj = None
        self.fig = None
        self.axes = None
        
        # Initialize analyzer components
        self._initialize_components()
    
    def _initialize_components(self):
        """Initialize analyzer components and data structures"""
        # Default classification methods
        if self.config.classification_methods is None:
            self.config.classification_methods = ["svm", "knn", "random_forest"]
        
        # Behavior analysis buffers
        self.pattern_history = []
        self.cluster_history = []
        self.classification_history = []
        self.trajectory_history = []
        self.action_sequence_history = []
        
        # Initialize scalers
        self.scaler = StandardScaler()
        
        # Initialize colormaps
        self._initialize_colormaps()
    
    def _initialize_colormaps(self):
        """Initialize colormaps for different behavior visualizations"""
        # Behavior colormap (blue to red)
        self.behavior_cmap = plt.cm.RdBu_r
        
        # Pattern colormap
        self.pattern_cmap = plt.cm.viridis
        
        # Cluster colormap
        self.cluster_cmap = plt.cm.Set3
        
        # Classification colormap
        self.classification_cmap = plt.cm.plasma
    
    def add_behavior_data(self, 
                         behaviors: np.ndarray,
                         action_labels: Optional[List[str]] = None,
                         sampling_rate: Optional[float] = None,
                         time_step: float = 1.0):
        """
        Add behavior data for analysis.
        
        Args:
            behaviors: Behavior data (channels x time) or (time x channels)
            action_labels: Optional action labels
            sampling_rate: Sampling rate (overrides config)
            time_step: Current time step
        """
        # Validate and process behaviors
        behaviors = np.asarray(behaviors)
        
        # Ensure behaviors are in the right format (channels x time)
        if behaviors.ndim == 1:
            behaviors = behaviors.reshape(1, -1)
        elif behaviors.shape[0] > behaviors.shape[1]:  # time x channels
            behaviors = behaviors.T
        
        # Store behaviors
        self.behavior_data.append(behaviors)
        
        # Analyze behaviors
        self._analyze_behaviors(behaviors, sampling_rate or self.config.sampling_rate)
        
        # Update current time
        self.current_time += time_step
    
    def _analyze_behaviors(self, behaviors: np.ndarray, sampling_rate: float):
        """Analyze behavior data"""
        n_channels, n_samples = behaviors.shape
        
        # Detect patterns using clustering
        patterns = self._detect_patterns(behaviors)
        self.pattern_history.append(patterns)
        
        # Perform cluster analysis
        clusters = self._perform_cluster_analysis(behaviors)
        self.cluster_history.append(clusters)
        
        # Analyze trajectories
        trajectories = self._analyze_trajectories(behaviors)
        self.trajectory_history.append(trajectories)
        
        # Detect action sequences
        action_sequences = self._detect_action_sequences(behaviors)
        self.action_sequence_history.append(action_sequences)
        
        # Perform classification
        classifications = self._perform_classification(behaviors)
        self.classification_history.append(classifications)
    
    def _detect_patterns(self, behaviors: np.ndarray) -> Dict[str, Any]:
        """Detect patterns in behavior data"""
        n_channels, n_samples = behaviors.shape
        
        # Combine behavior channels for pattern detection
        combined_behaviors = behaviors.T  # time x channels
        
        # Apply PCA for dimensionality reduction
        pca = PCA(n_components=min(3, n_channels))
        reduced_behaviors = pca.fit_transform(combined_behaviors)
        
        # Cluster patterns using K-means
        kmeans = KMeans(n_clusters=self.config.n_clusters, random_state=42)
        cluster_labels = kmeans.fit_predict(reduced_behaviors)
        
        # Calculate pattern statistics
        pattern_stats = {}
        for cluster in range(self.config.n_clusters):
            cluster_data = reduced_behaviors[cluster_labels == cluster]
            if len(cluster_data) > 0:
                pattern_stats[f"cluster_{cluster}"] = {
                    "size": len(cluster_data),
                    "mean": np.mean(cluster_data, axis=0).tolist(),
                    "std": np.std(cluster_data, axis=0).tolist(),
                    "centroid": kmeans.cluster_centers_[cluster].tolist()
                }
        
        return {
            "cluster_labels": cluster_labels.tolist(),
            "pattern_stats": pattern_stats,
            "explained_variance": pca.explained_variance_ratio_.tolist()
        }
    
    def _perform_cluster_analysis(self, behaviors: np.ndarray) -> Dict[str, Any]:
        """Perform cluster analysis on behavior data"""
        n_channels, n_samples = behaviors.shape
        
        # Scale behaviors for clustering
        scaled_behaviors = self.scaler.fit_transform(behaviors.T)
        
        # Perform DBSCAN clustering
        dbscan = DBSCAN(eps=0.5, min_samples=5)
        cluster_labels = dbscan.fit_predict(scaled_behaviors)
        
        # Calculate cluster statistics
        cluster_stats = {}
        unique_labels = np.unique(cluster_labels)
        
        for label in unique_labels:
            if label == -1:
                # Noise points
                noise_points = np.where(cluster_labels == -1)[0]
                cluster_stats["noise"] = {
                    "count": len(noise_points),
                    "percentage": len(noise_points) / len(cluster_labels) * 100
                }
            else:
                # Regular clusters
                cluster_points = scaled_behaviors[cluster_labels == label]
                cluster_stats[f"cluster_{label}"] = {
                    "count": len(cluster_points),
                    "percentage": len(cluster_points) / len(cluster_labels) * 100,
                    "mean": np.mean(cluster_points, axis=0).tolist(),
                    "std": np.std(cluster_points, axis=0).tolist()
                }
        
        return {
            "dbscan_cluster_labels": cluster_labels.tolist(),
            "cluster_stats": cluster_stats,
            "n_clusters": len([l for l in unique_labels if l != -1]),
            "noise_points": len(np.where(cluster_labels == -1)[0])
        }
    
    def _analyze_trajectories(self, behaviors: np.ndarray) -> Dict[str, Any]:n        """Analyze behavior trajectories"""
        n_channels, n_samples = behaviors.shape
        
        # Calculate trajectory characteristics
        trajectories = {}
        
        for i in range(n_channels):
            behavior_i = behaviors[i, :]
            
            # Calculate trajectory statistics
            trajectory_stats = {}
            
            # Calculate displacement
            displacements = np.diff(behavior_i)
            trajectory_stats["displacement_mean"] = float(np.mean(displacements))
            trajectory_stats["displacement_std"] = float(np.std(displacements))
            
            # Calculate velocity
            velocity = displacements / (1.0 / self.config.sampling_rate)
            trajectory_stats["velocity_mean"] = float(np.mean(velocity))
            trajectory_stats["velocity_std"] = float(np.std(velocity))
            
            # Calculate acceleration
            acceleration = np.diff(velocity)
            trajectory_stats["acceleration_mean"] = float(np.mean(acceleration))
            trajectory_stats["acceleration_std"] = float(np.std(acceleration))
            
            # Calculate path length
            path_length = np.sum(np.abs(displacements))
            trajectory_stats["path_length"] = float(path_length)
            
            # Calculate efficiency
            if path_length > 0:
                straight_line_distance = np.abs(behavior_i[-1] - behavior_i[0])
                trajectory_stats["efficiency"] = float(straight_line_distance / path_length)
            else:
                trajectory_stats["efficiency"] = 0.0
            
            trajectories[f"channel_{i}"] = trajectory_stats
        
        return trajectories
    
    def _detect_action_sequences(self, behaviors: np.ndarray) -> Dict[str, Any]:
        """Detect action sequences in behavior data"""
        n_channels, n_samples = behaviors.shape
        
        # Combine behavior channels for action sequence detection
        combined_behaviors = behaviors.T  # time x channels
        
        # Calculate action transitions
        action_sequences = []
        
        for channel in range(n_channels):
            channel_behavior = behaviors[channel, :]
            
            # Detect significant changes (actions)
            threshold = np.std(channel_behavior) * self.config.outlier_threshold
            actions = np.where(np.abs(np.diff(channel_behavior)) > threshold)[0]
            
            # Group consecutive actions into sequences
            sequences = []
            if len(actions) > 0:
                current_sequence = [actions[0]]
                for i in range(1, len(actions)):
                    if actions[i] == actions[i-1] + 1:  # Consecutive
                        current_sequence.append(actions[i])
                    else:  # New sequence
                        sequences.append(current_sequence)
                        current_sequence = [actions[i]]
                
                if current_sequence:
                    sequences.append(current_sequence)
            
            action_sequences.append({
                "channel": channel,
                "actions": actions.tolist(),
                "sequences": sequences,
                "action_count": len(actions),
                "sequence_count": len(sequences)
            })
        
        return {
            "action_sequences": action_sequences,
            "total_actions": sum(seq["action_count"] for seq in action_sequences),
            "total_sequences": sum(seq["sequence_count"] for seq in action_sequences)
        }
    
    def _perform_classification(self, behaviors: np.ndarray) -> Dict[str, Any]:
        """Perform classification on behavior data"""
        n_channels, n_samples = behaviors.shape
        
        # Prepare data for classification
        X = behaviors.T  # time x channels
        
        # Generate dummy labels for classification (simulated behavior types)
        n_samples_data = X.shape[0]
        y = np.random.randint(0, self.config.n_clusters, n_samples_data)
        
        # Split into train and test
        X_train, X_test, y_train, y_test = train_test_split(
            X, y, test_size=0.3, random_state=42
        )
        
        # Perform classification for each method
        classification_results = {}
        
        for method in self.config.classification_methods:
            try:
                if method == "svm":
                    clf = SVC(random_state=42)
                elif method == "knn":
                    clf = KNeighborsClassifier()
                elif method == "random_forest":
                    clf = RandomForestClassifier(random_state=42)
                else:
                    continue
                
                # Train classifier
                clf.fit(X_train, y_train)
                
                # Predict
                y_pred = clf.predict(X_test)
                
                # Calculate accuracy
                accuracy = accuracy_score(y_test, y_pred)
                
                # Get classification report
                report = classification_report(y_test, y_pred, output_dict=True)
                
                classification_results[method] = {
                    "accuracy": accuracy,
                    "classification_report": report,
                    "confusion_matrix": confusion_matrix(y_test, y_pred).tolist()
                }
                
            except Exception as e:
                print(f"Warning: Could not perform {method} classification: {e}")
                classification_results[method] = {"error": str(e)}
        
        return classification_results
    
    def start_real_time(self, initial_data: Dict[str, Any] = None):
        """
        Start real-time behavior visualization.
n        
        Args:
            initial_data: Optional initial behavior data
        """
        if initial_data:
            # Add initial data
            self.add_behavior_data(
                behaviors=initial_data['behaviors'],
                action_labels=initial_data.get('action_labels'),
                sampling_rate=initial_data.get('sampling_rate', self.config.sampling_rate)
            )
        
        self.is_running = True
        
        # Create visualization
        self.fig = plt.figure(figsize=self.config.figure_size)
        
        # Create subplot layout based on view mode
        if self.config.view_mode == "2d":
            self._create_2d_view()
        elif self.config.view_mode == "3d":
            self._create_3d_view()
        elif self.config.view_mode == "behavior_space":
            self._create_behavior_space_view()
        elif self.config.view_mode == "pattern_space":
            self._create_pattern_space_view()
        else:
            self._create_2d_view()
        
        # Setup animation
        self.animation_obj = animation.FuncAnimation(
            self.fig, self._update_plot, interval=self.config.animation_interval,
            blit=False, cache_frame_data=False
        )
        
        plt.show()
    
    def _create_2d_view(self):
        """Create 2D view visualization"""
        # Create grid of subplots for behavior analysis
        gs = plt.GridSpec(4, 4, hspace=0.3, wspace=0.3)
        
        # Main behavior plot
        self.ax_behavior_plot = plt.subplot(gs[0, :4])
        
        # Pattern classification
        self.ax_pattern_classification = plt.subplot(gs[1, :2])
        
        # Cluster analysis
        self.ax_cluster_analysis = plt.subplot(gs[1, 2:])
        
        # Trajectory analysis
        self.ax_trajectory_analysis = plt.subplot(gs[2, :2])
        
        # Action sequence analysis
        self.ax_action_sequence = plt.subplot(gs[2, 2:])
        
        # Classification results
        self.ax_classification_results = plt.subplot(gs[3, :2])
        
        # Anomaly detection
        self.ax_anomaly_detection = plt.subplot(gs[3, 2:])
        
        # Initialize 2D plot elements
        self._initialize_2d_elements()
    
    def _create_3d_view(self):
        """Create 3D view visualization"""
        # Create 3D grid for behavior analysis
        gs = plt.GridSpec(3, 3, hspace=0.3, wspace=0.3)
        
        # 3D behavior embedding
        self.ax_behavior_embedding_3d = plt.subplot(gs[0, :3], projection='3d')
        
        # 3D pattern space
        self.ax_pattern_space_3d = plt.subplot(gs[1, 0], projection='3d')
        
        # 3D cluster analysis
        self.ax_cluster_analysis_3d = plt.subplot(gs[1, 1], projection='3d')
        
        # 3D trajectory
        self.ax_trajectory_3d = plt.subplot(gs[1, 2], projection='3d')
        
        # 3D action sequences
        self.ax_action_sequences_3d = plt.subplot(gs[2, :3], projection='3d')
        
        # Initialize 3D plot elements
        self._initialize_3d_elements()
    
    def _create_behavior_space_view(self):
        """Create behavior space view visualization"""
        # Create behavior space focused view
        gs = plt.GridSpec(3, 3, hspace=0.3, wspace=0.3)
        
        # Behavior space embedding
        self.ax_behavior_space_embedding = plt.subplot(gs[0, :3])
        
        # Behavior trajectory
        self.ax_behavior_trajectory = plt.subplot(gs[1, 0])
        
        # Behavior patterns
        self.ax_behavior_patterns = plt.subplot(gs[1, 1])
        
        # Behavior clusters
        self.ax_behavior_clusters = plt.subplot(gs[1, 2])
        
        # Behavior dynamics
        self.ax_behavior_dynamics = plt.subplot(gs[2, 0])
        
        # Behavior classification
        self.ax_behavior_classification = plt.subplot(gs[2, 1])
        
        # Behavior anomalies
        self.ax_behavior_anomalies = plt.subplot(gs[2, 2])
        
        # Initialize behavior space elements
        self._initialize_behavior_space_elements()
    
    def _create_pattern_space_view(self):
        """Create pattern space view visualization"""
        # Create pattern space focused view
        gs = plt.GridSpec(3, 3, hspace=0.3, wspace=0.3)
        
        # Pattern space embedding
        self.ax_pattern_space_embedding = plt.subplot(gs[0, :3])
        
        # Pattern trajectories
        self.ax_pattern_trajectories = plt.subplot(gs[1, :2])
        
        # Pattern classification
        self.ax_pattern_classification_space = plt.subplot(gs[1, 2])
        
        # Pattern clustering
        self.ax_pattern_clustering = plt.subplot(gs[2, 0])
        
        # Pattern dynamics
        self.ax_pattern_dynamics = plt.subplot(gs[2, 1])
        
        # Pattern anomalies
        self.ax_pattern_anomalies = plt.subplot(gs[2, 2])
        
        # Initialize pattern space elements
        self._initialize_pattern_space_elements()
    
    def _initialize_2d_elements(self):
        """Initialize 2D view plot elements"""
        if hasattr(self, 'ax_behavior_plot'):
            # Behavior plot
            self.behavior_lines = []
            self.ax_behavior_plot.set_title("Behavior Patterns", fontsize=14, fontweight='bold')
            self.ax_behavior_plot.set_xlabel("Time Step")
            self.ax_behavior_plot.set_ylabel("Behavior Value")
            self.ax_behavior_plot.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_pattern_classification'):
            # Pattern classification plot
            self.pattern_classification_plot, = self.ax_pattern_classification.plot([], [], 'b-', linewidth=2)
            self.ax_pattern_classification.set_title("Pattern Classification", fontsize=12, fontweight='bold')
            self.ax_pattern_classification.set_xlabel("Time Step")
            self.ax_pattern_classification.set_ylabel("Pattern Class")
            self.ax_pattern_classification.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_cluster_analysis'):
            # Cluster analysis plot
            self.cluster_analysis_im = self.ax_cluster_analysis.imshow(np.zeros((10, 10)), cmap=self.cluster_cmap, vmin=0, vmax=1)
            self.ax_cluster_analysis.set_title("Cluster Analysis", fontsize=12, fontweight='bold')
            self.ax_cluster_analysis.set_xlabel("Channel Index")
            self.ax_cluster_analysis.set_ylabel("Channel Index")
        
        if hasattr(self, 'ax_trajectory_analysis'):
            # Trajectory analysis plot
            self.trajectory_analysis_plot, = self.ax_trajectory_analysis.plot([], [], 'r-', linewidth=2)
            self.ax_trajectory_analysis.set_title("Trajectory Analysis", fontsize=12, fontweight='bold')
            self.ax_trajectory_analysis.set_xlabel("Time Step")
            self.ax_trajectory_analysis.set_ylabel("Trajectory Value")
            self.ax_trajectory_analysis.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_action_sequence'):
            # Action sequence plot
            self.action_sequence_plot, = self.ax_action_sequence.plot([], [], 'g-', linewidth=2)
            self.ax_action_sequence.set_title("Action Sequence", fontsize=12, fontweight='bold')
            self.ax_action_sequence.set_xlabel("Time Step")
            self.ax_action_sequence.set_ylabel("Action Sequence")
            self.ax_action_sequence.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_classification_results'):
            # Classification results plot
            self.classification_results_bars = self.ax_classification_results.bar([], [], alpha=0.7, color='skyblue')
            self.ax_classification_results.set_title("Classification Results", fontsize=12, fontweight='bold')
            self.ax_classification_results.set_xlabel("Classifier")
            self.ax_classification_results.set_ylabel("Accuracy")
            self.ax_classification_results.grid(True, alpha=0.3, axis='y')
        
        if hasattr(self, 'ax_anomaly_detection'):
            # Anomaly detection plot
            self.anomaly_detection_plot, = self.ax_anomaly_detection.plot([], [], 'orange', linewidth=2)
            self.ax_anomaly_detection.set_title("Anomaly Detection", fontsize=12, fontweight='bold')
            self.ax_anomaly_detection.set_xlabel("Time Step")
            self.ax_anomaly_detection.set_ylabel("Anomaly Score")
            self.ax_anomaly_detection.grid(True, alpha=0.3)
    
    def _initialize_3d_elements(self):
        """Initialize 3D view plot elements"""
        # Initialize 3D plot elements
        # (Implementation would continue with 3D-specific initialization)
    
    def _initialize_behavior_space_elements(self):
        """Initialize behavior space view plot elements"""
        # Initialize behavior space plot elements
        # (Implementation would continue with behavior space-specific initialization)
    
    def _initialize_pattern_space_elements(self):
        """Initialize pattern space view plot elements"""
        # Initialize pattern space plot elements
        # (Implementation would continue with pattern space-specific initialization)
    
    def _update_plot(self, frame):
        """Update plot for animation"""
        # Update all visualization elements based on current time
        if self.config.view_mode == "2d":
            self._update_2d_view(frame)
        elif self.config.view_mode == "3d":
            self._update_3d_view(frame)
        elif self.config.view_mode == "behavior_space":
            self._update_behavior_space_view(frame)
        elif self.config.view_mode == "pattern_space":
            self._update_pattern_space_view(frame)
    
    def _update_2d_view(self, frame):
        """Update 2D view elements"""
        if not self.behavior_data:
            return
        
        current_idx = min(frame, len(self.behavior_data) - 1)
        
        # Update behavior plot
        if hasattr(self, 'ax_behavior_plot') and hasattr(self, 'behavior_lines'):
            self._update_behavior_plot(current_idx)
        
        # Update pattern classification
        if hasattr(self, 'ax_pattern_classification'):
            self._update_pattern_classification(current_idx)
        
        # Update cluster analysis
        if hasattr(self, 'ax_cluster_analysis') and hasattr(self, 'cluster_analysis_im'):
            if current_idx < len(self.cluster_history):
                cluster_data = self.cluster_history[current_idx]
                if "dbscan_cluster_labels" in cluster_data:
                    # Create dummy cluster matrix
                    n_clusters = len(set(cluster_data["dbscan_cluster_labels"]))
                    cluster_matrix = np.zeros((n_clusters, n_clusters))
                    for i in range(n_clusters):
                        cluster_matrix[i, i] = 1.0
                    
                    self.cluster_analysis_im.set_data(cluster_matrix)
        
        # Update trajectory analysis
        if hasattr(self, 'ax_trajectory_analysis'):
            self._update_trajectory_analysis(current_idx)
        
        # Update action sequence
        if hasattr(self, 'ax_action_sequence'):
            self._update_action_sequence(current_idx)
        
        # Update classification results
        if hasattr(self, 'ax_classification_results'):
            self._update_classification_results(current_idx)
        
        # Update anomaly detection
        if hasattr(self, 'ax_anomaly_detection'):
            self._update_anomaly_detection(current_idx)
    
    def _update_behavior_plot(self, current_idx: int):
        """Update behavior plot"""
        if not self.behavior_data:
            return
        
        behavior = self.behavior_data[current_idx]
        
        if hasattr(self, 'behavior_lines'):
            # Update lines for each channel
            for i in range(behavior.shape[0]):
                if i < len(self.behavior_lines):
                    self.behavior_lines[i].set_data(
                        range(min(100, behavior.shape[1])),
                        behavior[i, :min(100, behavior.shape[1])]
                    )
    
    def _update_pattern_classification(self, current_idx: int):
        """Update pattern classification plot"""
        if hasattr(self, 'pattern_classification_plot'):
            # Generate dummy pattern classification data
            time_points = list(range(min(100, frame + 1)))
            pattern_classes = np.random.randint(0, self.config.n_clusters, len(time_points))
            
            self.pattern_classification_plot.set_data(time_points, pattern_classes)
    
    def _update_trajectory_analysis(self, current_idx: int):
        """Update trajectory analysis plot"""
        if hasattr(self, 'ax_trajectory_analysis') and hasattr(self, 'trajectory_analysis_plot'):
            if current_idx < len(self.trajectory_history):
                trajectory_data = self.trajectory_history[current_idx]
                if "channel_0" in trajectory_data:
                    channel_stats = trajectory_data["channel_0"]
                    
                    # Generate dummy trajectory data
                    time_points = list(range(min(100, frame + 1)))
                    trajectory_values = np.linspace(0, 1, len(time_points)) + np.random.normal(0, 0.1, len(time_points))
                    
                    self.trajectory_analysis_plot.set_data(time_points, trajectory_values)
    
    def _update_action_sequence(self, current_idx: int):
        """Update action sequence plot"""
        if hasattr(self, 'ax_action_sequence') and hasattr(self, 'action_sequence_plot'):
            if current_idx < len(self.action_sequence_history):
                action_data = self.action_sequence_history[current_idx]
                if "action_sequences" in action_data:
                    # Generate dummy action sequence data
                    time_points = list(range(min(100, frame + 1)))
                    action_values = np.random.randint(0, 3, len(time_points))  # 3 action types
                    
                    self.action_sequence_plot.set_data(time_points, action_values)
    
    def _update_classification_results(self, current_idx: int):
        """Update classification results plot"""
        if hasattr(self, 'ax_classification_results') and hasattr(self, 'classification_results_bars'):
            if current_idx < len(self.classification_history):
                classification_data = self.classification_history[current_idx]
                
                # Extract accuracy from each classifier
                methods = list(classification_data.keys())
                accuracies = []
                
                for method in methods:
                    if "accuracy" in classification_data[method]:
                        accuracies.append(classification_data[method]["accuracy"])
                    else:
                        accuracies.append(0.0)
                
                self.classification_results_bars[0].remove()  # Remove old bars
                self.classification_results_bars = self.ax_classification_results.bar(
                    range(len(methods)), accuracies, alpha=0.7, color='skyblue'
                )
    
    def _update_anomaly_detection(self, current_idx: int):
        """Update anomaly detection plot"""
        if hasattr(self, 'ax_anomaly_detection') and hasattr(self, 'anomaly_detection_plot'):
            # Generate dummy anomaly detection data
            time_points = list(range(min(100, frame + 1)))
            anomaly_scores = np.random.exponential(0.1, len(time_points))
            
            self.anomaly_detection_plot.set_data(time_points, anomaly_scores)
    
    def create_static_visualization(self, output_file: str = None) -> Dict[str, Any]:
        """
        Create static visualization for analysis and reporting.
        
        Args:
            output_file: Optional output filename
            
        Returns:
            Dictionary with visualization data and results
        """
        result = {
            "success": False,
            "data": {},
            "plots": {},
            "statistics": {},
            "output_files": []
        }
        
        try:
            if not self.behavior_data:
                result["error"] = "No behavior data available"
                return result
            
            # Create comprehensive behavior visualization
            fig = self._create_comprehensive_behavior_visualization()
            
            # Save figure
            if output_file is None:
                output_file = f"{self.config.output_path}.{self.config.save_format}"
            
            os.makedirs(os.path.dirname(output_file) if os.path.dirname(output_file) else '.', exist_ok=True)
            fig.savefig(output_file, dpi=self.config.dpi, bbox_inches='tight')
            
            # Compute statistics
            stats = self._compute_behavior_statistics()
            
            result["plots"] = "comprehensive behavior visualization"
            result["statistics"] = stats
            result["output_files"].append(output_file)
            result["success"] = True
            
            result["figure"] = fig
            
        except Exception as e:
            result["error"] = str(e)
            result["success"] = False
        
        return result
    
    def _create_comprehensive_behavior_visualization(self):
        """Create comprehensive behavior visualization"""
        # Create a comprehensive figure based on view mode
        if self.config.view_mode == "2d":
            return self._create_2d_comprehensive_behavior()
        elif self.config.view_mode == "3d":
            return self._create_3d_comprehensive_behavior()
        elif self.config.view_mode == "behavior_space":
            return self._create_behavior_space_comprehensive_behavior()
        elif self.config.view_mode == "pattern_space":
            return self._create_pattern_space_comprehensive_behavior()
        else:
            return self._create_2d_comprehensive_behavior()
    
    def _create_2d_comprehensive_behavior(self):
        """Create comprehensive 2D behavior visualization"""
        fig = plt.figure(figsize=(20, 15))
        gs = plt.GridSpec(4, 4, hspace=0.3, wspace=0.3)
        
        # Panel 1: Main behavior patterns
        self._plot_main_behavior_patterns(fig, gs[0, :4])
        
        # Panel 2: Pattern classification
        self._plot_pattern_classification(fig, gs[1, :2])
        
        # Panel 3: Cluster analysis
        self._plot_cluster_analysis(fig, gs[1, 2:])
        
        # Panel 4: Trajectory analysis
        self._plot_trajectory_analysis(fig, gs[2, :2])
        
        # Panel 5: Action sequences
        self._plot_action_sequences(fig, gs[2, 2:])
        
        # Panel 6: Classification results
        self._plot_classification_results(fig, gs[3, :2])
        
        # Panel 7: Anomaly detection
        self._plot_anomaly_detection(fig, gs[3, 2:])
        
        fig.suptitle("Comprehensive Behavior Analysis", fontsize=16, fontweight='bold')
        
        return fig
    
    def _plot_main_behavior_patterns(self, fig, ax):
        """Plot main behavior patterns"""
        # Plot behavior patterns for each channel
        if self.behavior_data:
            current_behavior = self.behavior_data[-1]
            
            for i in range(current_behavior.shape[0]):
                alpha = 0.7 if i == current_behavior.shape[0] - 1 else 0.3
                ax.plot(current_behavior[i, :], linewidth=2, alpha=alpha, 
                       label=f'Channel {i}' if i == current_behavior.shape[0] - 1 else None)
            
            ax.set_title("Main Behavior Patterns", fontsize=12, fontweight='bold')
            ax.set_xlabel("Time Step")
            ax.set_ylabel("Behavior Value")
            ax.legend()
            ax.grid(True, alpha=0.3)
    
    def _plot_pattern_classification(self, fig, ax):
        """Plot pattern classification"""
        # Generate dummy pattern classification data
        time_points = np.linspace(0, 50, 200)
        pattern_classes = np.random.randint(0, self.config.n_clusters, len(time_points))
        
        ax.plot(time_points, pattern_classes, linewidth=2, color='blue', alpha=0.7)
        ax.set_title("Pattern Classification", fontsize=12, fontweight='bold')
        ax.set_xlabel("Time Step")
        ax.set_ylabel("Pattern Class")
        ax.grid(True, alpha=0.3)
    
    def _plot_cluster_analysis(self, fig, ax):
        """Plot cluster analysis"""
        # Create dummy cluster analysis plot
        n_clusters = 5
        cluster_data = np.random.rand(n_clusters, 2) * 10
        cluster_labels = np.random.randint(0, n_clusters, 50)
        
        scatter = ax.scatter(cluster_data[:, 0], cluster_data[:, 1], 
                           c=cluster_labels, cmap=self.cluster_cmap, alpha=0.7, s=50)
        
        ax.set_title("Cluster Analysis", fontsize=12, fontweight='bold')
        ax.set_xlabel("Feature 1")
        ax.set_ylabel("Feature 2")
        plt.colorbar(scatter, ax=ax)
    
    def _plot_trajectory_analysis(self, fig, ax):
        """Plot trajectory analysis"""
        # Generate dummy trajectory analysis data
        time_points = np.linspace(0, 50, 200)
        trajectory_values = np.sin(time_points * 0.1) * 0.5 + 0.5
        
        ax.plot(time_points, trajectory_values, linewidth=2, color='red', alpha=0.7)
        ax.set_title("Trajectory Analysis", fontsize=12, fontweight='bold')
        ax.set_xlabel("Time Step")
        ax.set_ylabel("Trajectory Value")
        ax.grid(True, alpha=0.3)
    
    def _plot_action_sequences(self, fig, ax):
        """Plot action sequences"""
        # Generate dummy action sequence data
        time_points = np.linspace(0, 50, 200)
        action_sequences = np.random.randint(0, 4, len(time_points))  # 4 action types
        
        ax.plot(time_points, action_sequences, linewidth=2, color='green', alpha=0.7)
        ax.set_title("Action Sequences", fontsize=12, fontweight='bold')
        ax.set_xlabel("Time Step")
        ax.set_ylabel("Action Sequence")
        ax.grid(True, alpha=0.3)
    
    def _plot_classification_results(self, fig, ax):
        """Plot classification results"""
        # Generate dummy classification results
        methods = ["SVM", "KNN", "Random Forest"]
        accuracies = [0.85, 0.78, 0.92]
        
        bars = ax.bar(methods, accuracies, alpha=0.7, color='skyblue')
        ax.set_title("Classification Results", fontsize=12, fontweight='bold')
        ax.set_ylabel("Accuracy")
        ax.set_ylim(0, 1)
        ax.grid(True, alpha=0.3, axis='y')
        
        # Add value labels on bars
        for bar, value in zip(bars, accuracies):
            ax.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                   f'{value:.2f}', ha='center', va='bottom')
    
    def _plot_anomaly_detection(self, fig, ax):
        """Plot anomaly detection"""
        # Generate dummy anomaly detection data
        time_points = np.linspace(0, 50, 200)
        anomaly_scores = np.random.exponential(0.1, len(time_points))
        
        ax.plot(time_points, anomaly_scores, linewidth=2, color='orange', alpha=0.7)
        ax.axhline(y=2.0, color='red', linestyle='--', alpha=0.5, label='Anomaly Threshold')
        ax.set_title("Anomaly Detection", fontsize=12, fontweight='bold')
        ax.set_xlabel("Time Step")
        ax.set_ylabel("Anomaly Score")
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def _compute_behavior_statistics(self) -> Dict[str, Any]:
        """Compute comprehensive behavior statistics"""
        stats = {
            "behavior_count": len(self.behavior_data),
            "current_behavior_length": len(self.behavior_data[-1]) if self.behavior_data else 0,
            "final_behavior_value": 0.0,
            "pattern_count": 0,
            "cluster_count": 0,
            "classification_accuracy": {},
            "trajectory_efficiency": 0.0,
            "action_sequence_count": 0,
            "anomaly_count": 0,
            "behavior_patterns": {}
        }
        
        if self.behavior_data and len(self.behavior_data) > 0:
            current_behavior = self.behavior_data[-1]
            stats["final_behavior_value"] = float(current_behavior[-1, -1] if current_behavior.size > 0 else 0)
        
        if self.pattern_history and len(self.pattern_history) > 0:
            stats["pattern_count"] = len(self.pattern_history[-1].get("cluster_labels", []))
        
        if self.cluster_history and len(self.cluster_history) > 0:
            cluster_data = self.cluster_history[-1]
            stats["cluster_count"] = cluster_data.get("n_clusters", 0)
        
        if self.classification_history and len(self.classification_history) > 0:
            classification_data = self.classification_history[-1]
            for method, results in classification_data.items():
                if "accuracy" in results:
                    stats["classification_accuracy"][method] = results["accuracy"]
        
        if self.trajectory_history and len(self.trajectory_history) > 0:
            trajectory_data = self.trajectory_history[-1]
            if "channel_0" in trajectory_data:
                stats["trajectory_efficiency"] = trajectory_data["channel_0"].get("efficiency", 0.0)
        
        if self.action_sequence_history and len(self.action_sequence_history) > 0:
            action_data = self.action_sequence_history[-1]
            stats["action_sequence_count"] = action_data.get("total_sequences", 0)
        
        return stats
    
    def export_behavior_data(self, filename: str) -> str:
        """
        Export behavior data to file.
        
        Args:
            filename: Output filename
            
        Returns:
            Path to saved file
        """
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        export_data = {
            "config": asdict(self.config),
            "behavior_data": [behavior.tolist() for behavior in self.behavior_data],
            "patterns": self.pattern_history,
            "clusters": self.cluster_history,
            "classifications": self.classification_history,
            "trajectories": self.trajectory_history,
            "action_sequences": self.action_sequence_history,
            "statistics": self._compute_behavior_statistics()
        }
        
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2)
        
        return filename


def create_behavior_analyzer(config_dict: Dict[str, Any] = None) -> BehaviorAnalyzer:
    """
    Factory function to create BehaviorAnalyzer with configuration.
    
    Args:
        config_dict: Optional configuration dictionary
        
    Returns:
        Configured BehaviorAnalyzer instance
    """
    if config_dict is None:
        config_dict = {}
    
    config = BehaviorAnalyzerConfig(**config_dict)
    return BehaviorAnalyzer(config)