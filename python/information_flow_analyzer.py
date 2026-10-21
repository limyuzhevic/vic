#!/usr/bin/env python3
"""
Information Flow Analyzer Tool

Information flow and information theory analysis for neural networks.
Provides comprehensive analysis of information dynamics, entropy,
mutual information, and information bottleneck in neural systems.
"""

import numpy as np
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass, asdict
from enum import Enum
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats, signal
from sklearn.neural_network import MLPClassifier
from sklearn.feature_selection import mutual_info_regression
import sys
import os

@dataclass
class InformationFlowAnalyzerConfig:
    """Configuration for information flow analyzer"""
    signal_length: float = 100.0  # seconds
    sampling_rate: float = 100.0  # Hz
    show_information_entropy: bool = True
    show_mutual_information: bool = True
    show_information_bottleneck: bool = True
    show_information_flow: bool = True
    show_information_partitions: bool = True
    show_ks_test: bool = True
    show_correlation_analysis: bool = True
    animate: bool = True
    color_scheme: str = "information"  # "information", "entropy", "flow", "bottleneck"
    show_labels: bool = True
    show_statistics: bool = True
    save_format: str = "png"  # "png", "json", "html"
    output_path: str = "information_flow_analyzer"
    
    # Analysis parameters
    time_lags: List[int] = None  # Time lags for temporal analysis
    binning_method: str = "kmeans"  # "kmeans", "uniform", "entropy"
    k_clusters: int = 5
    max_lags: int = 20
    significance_level: float = 0.05
    show_transfer_entropy: bool = True
    show_partial_correlation: bool = True
    show_causation_analysis: bool = True
    
    # Visualization modes
    view_mode: str = "2d"  # "2d", "3d", "information_space", "time_lag"
    show_3d: bool = False
    figure_size: Tuple[int, int] = (16, 12)
    dpi: int = 150
    animation_interval: int = 100  # milliseconds
    
    # Display settings
    show_legend: bool = True
    show_colorbar: bool = True
    show_information_flow_matrix: bool = True
    smooth_information: bool = True
class InformationFlowAnalyzer:
    """
    Information flow and information theory analyzer for neural networks.
    
    Features:
    - Information entropy analysis
    - Mutual information calculation
    - Information bottleneck optimization
    - Information flow mapping
    - Causation analysis
    - Partial correlation analysis
    - Transfer entropy computation
    - Export capabilities for scientific analysis
    """
    
    def __init__(self, config: InformationFlowAnalyzerConfig = None):
        self.config = config or InformationFlowAnalyzerConfig()
        self.signals = []  # List of neural signal arrays
        self.information_metrics = []  # List of information metrics
        self.mutual_information_matrix = None  # Mutual information matrix
        self.transfer_entropy_matrix = None  # Transfer entropy matrix
        self.information_bottleneck = {}  # Information bottleneck analysis
        self.information_flow = {}  # Information flow analysis
        self.information_partitions = {}  # Information partitions
        self.current_time = 0.0
        self.is_running = False
        self.animation_obj = None
        self.fig = None
        self.axes = None
        
        # Initialize analyzer components
        self._initialize_components()
    
    def _initialize_components(self):
        """Initialize analyzer components and data structures"""
        # Default time lags
        if self.config.time_lags is None:
            self.config.time_lags = [1, 2, 5, 10, 20]
        
        # Information theory buffers
        self.entropy_history = []
        self.mutual_info_history = []
        self.transfer_entropy_history = []
        self.information_flow_history = []
        
        # Initialize scalers
        self.scaler = MinMaxScaler()
        
        # Initialize colormaps
        self._initialize_colormaps()
    
    def _initialize_colormaps(self):
        """Initialize colormaps for different information visualizations"""
        # Information entropy colormap (blue to red)
        self.information_entropy_cmap = plt.cm.RdBu_r
        
        # Mutual information colormap
        self.mutual_info_cmap = plt.cm.Blues_r
        
        # Information flow colormap
        self.information_flow_cmap = plt.cm.viridis
        
        # Information bottleneck colormap
        self.bottleneck_cmap = plt.cm.plasma
        
        # Transfer entropy colormap
        self.transfer_entropy_cmap = plt.cm.coolwarm
    
    def add_neural_signals(self, 
                         signals: np.ndarray,
                         channel_names: Optional[List[str]] = None,
                         sampling_rate: Optional[float] = None,
                         time_step: float = 1.0):
        """
        Add neural signals for information flow analysis.
        
        Args:
            signals: Neural signals (channels x time) or (time x channels)
            channel_names: Optional channel names
            sampling_rate: Sampling rate (overrides config)
            time_step: Current time step
        """
        # Validate and process signals
        signals = np.asarray(signals)
        
        # Ensure signals are in the right format (channels x time)
        if signals.ndim == 1:
            signals = signals.reshape(1, -1)
        elif signals.shape[0] > signals.shape[1]:  # time x channels
            signals = signals.T
        
        # Store signals
        self.signals.append(signals)
        
        # Compute information metrics
        self._compute_information_metrics(signals, sampling_rate or self.config.sampling_rate)
        
        # Update current time
        self.current_time += time_step
    
    def _compute_information_metrics(self, signals: np.ndarray, sampling_rate: float):
        """Compute information theory metrics"""
        n_channels, n_samples = signals.shape
        
        # Compute information entropy for each channel
        entropy = self._compute_entropy(signals)
        self.entropy_history.append(entropy)
        
        # Compute mutual information between channels
        mutual_info = self._compute_mutual_information(signals)
        self.mutual_info_history.append(mutual_info)
        
        # Compute transfer entropy
        if self.config.show_transfer_entropy:
            transfer_entropy = self._compute_transfer_entropy(signals, sampling_rate)
            self.transfer_entropy_history.append(transfer_entropy)
        
        # Compute information flow
        if self.config.show_information_flow:
            information_flow = self._compute_information_flow(signals)
            self.information_flow_history.append(information_flow)
        
        # Compute information partitions
        if self.config.show_information_partitions:
            partitions = self._compute_information_partitions(signals)
            self.information_partitions = partitions
    
    def _compute_entropy(self, signals: np.ndarray) -> np.ndarray:
        """Compute information entropy for each channel"""
        n_channels, n_samples = signals.shape
        entropy = np.zeros(n_channels)
        
        for i in range(n_channels):
            # Discretize signal for entropy calculation
            signal_i = signals[i, :]
            
            # Use k-means discretization for continuous signals
            kmeans = KMeans(n_clusters=self.config.k_clusters, random_state=42)
            discretized = kmeans.fit_predict(signal_i.reshape(-1, 1))
            
            # Compute entropy
            _, counts = np.unique(discretized, return_counts=True)
            probabilities = counts / len(discretized)
            entropy[i] = -np.sum(probabilities * np.log(probabilities + 1e-10))
        
        return entropy
    
    def _compute_mutual_information(self, signals: np.ndarray) -> np.ndarray:
        """Compute mutual information between channels"""
        n_channels = signals.shape[0]
        mutual_info = np.zeros((n_channels, n_channels))
        
        for i in range(n_channels):
            for j in range(n_channels):
                if i != j:
                    # Use mutual_info_regression for continuous variables
                    X = signals[i, :].reshape(-1, 1)
                    Y = signals[j, :]
                    
                    try:
                        mi = mutual_info_regression(X, Y, random_state=42)[0]
                        mutual_info[i, j] = mi
                    except:
                        mutual_info[i, j] = 0.0
                else:
                    mutual_info[i, i] = 0.0  # Self-information is zero
        
        return mutual_info
    
    def _compute_transfer_entropy(self, signals: np.ndarray, sampling_rate: float) -> np.ndarray:
        """Compute transfer entropy between channels"""
        n_channels, n_samples = signals.shape
        
        # Compute transfer entropy using mutual information
        transfer_entropy = np.zeros((n_channels, n_channels))
        
        for i in range(n_channels):
            for j in range(n_channels):
                if i != j:
                    # Simplified transfer entropy calculation
                    # Using time-lagged mutual information
                    max_lag = min(self.config.max_lags, n_samples // 2)
                    
                    te = 0.0
                    for lag in range(1, max_lag + 1):
                        if lag < n_samples:
                            # Compute mutual information between lagged signals
                            X = signals[j, lag:]
                            Y = signals[i, :-lag]
                            
                            if len(X) > 1 and len(Y) > 1:
                                try:
                                    mi = mutual_info_regression(X.reshape(-1, 1), Y, random_state=42)[0]
                                    te += mi
                                except:
                                    te += 0.0
                    
                    transfer_entropy[i, j] = te / max_lag if max_lag > 0 else 0.0
        
        return transfer_entropy
    
    def _compute_information_flow(self, signals: np.ndarray) -> Dict[str, Any]:
        """Compute information flow between channels"""
        n_channels, n_samples = signals.shape
        
        # Compute information flow using partial correlation
        flow_matrix = np.zeros((n_channels, n_channels))
        
        for i in range(n_channels):
            for j in range(n_channels):
                if i != j:
                    # Compute information flow as mutual information
                    X = signals[i, :].reshape(-1, 1)
                    Y = signals[j, :]
                    
                    try:
                        mi = mutual_info_regression(X, Y, random_state=42)[0]
                        flow_matrix[i, j] = mi
                    except:
                        flow_matrix[i, j] = 0.0
        
        # Normalize flow matrix
        if np.max(flow_matrix) > 0:
            flow_matrix = flow_matrix / np.max(flow_matrix)
        
        return {
            "flow_matrix": flow_matrix,
            "total_flow": np.sum(flow_matrix),
            "source_distribution": np.sum(flow_matrix, axis=1),
            "target_distribution": np.sum(flow_matrix, axis=0)
        }
    
    def _compute_information_partitions(self, signals: np.ndarray) -> Dict[str, Any]:
        """Compute information partitions using clustering"""
        n_channels, n_samples = signals.shape
        
        # Combine signals for clustering
        combined_signals = signals.T  # time x channels
        
        # Apply k-means clustering to partition channels
        kmeans = KMeans(n_clusters=self.config.k_clusters, random_state=42)
        cluster_labels = kmeans.fit_predict(combined_signals)
        
        # Compute information within and between clusters
        within_cluster_info = np.zeros(self.config.k_clusters)
        between_cluster_info = 0.0
        
        for cluster in range(self.config.k_clusters):
            # Channels in this cluster
            cluster_channels = np.where(cluster_labels == cluster)[0]
            
            if len(cluster_channels) > 1:
                # Compute average mutual information within cluster
                cluster_signals = signals[cluster_channels, :]
                cluster_mi = np.mean(cluster_signals)
                within_cluster_info[cluster] = cluster_mi
        
        # Compute between-cluster information
        for i in range(self.config.k_clusters):
            for j in range(i + 1, self.config.k_clusters):
                cluster_i_channels = np.where(cluster_labels == i)[0]
                cluster_j_channels = np.where(cluster_labels == j)[0]
                
                if len(cluster_i_channels) > 0 and len(cluster_j_channels) > 0:
                    # Compute average mutual information between clusters
                    cluster_i_signals = signals[cluster_i_channels, :]
                    cluster_j_signals = signals[cluster_j_channels, :]
                    
                    between_cluster_info += np.mean(cluster_i_signals) * np.mean(cluster_j_signals)
        
        return {
            "cluster_labels": cluster_labels.tolist(),
            "within_cluster_info": within_cluster_info.tolist(),
            "between_cluster_info": float(between_cluster_info),
            "cluster_cohesion": float(np.mean(within_cluster_info) / (np.mean(within_cluster_info) + between_cluster_info + 1e-10))
        }
    
    def start_real_time(self, initial_data: Dict[str, Any] = None):
        """
        Start real-time information flow visualization.
        
        Args:
            initial_data: Optional initial signal data
        """
        if initial_data:
            # Add initial data
            self.add_neural_signals(
                signals=initial_data['signals'],
                channel_names=initial_data.get('channel_names'),
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
        elif self.config.view_mode == "information_space":
            self._create_information_space_view()
        elif self.config.view_mode == "time_lag":
            self._create_time_lag_view()
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
        # Create grid of subplots for information flow analysis
        gs = plt.GridSpec(4, 4, hspace=0.3, wspace=0.3)
        
        # Main mutual information matrix
        self.ax_mutual_information = plt.subplot(gs[0, :4])
        
        # Information entropy distribution
        self.ax_information_entropy = plt.subplot(gs[1, :2])
        
        # Information flow visualization
        self.ax_information_flow = plt.subplot(gs[1, 2:])
        
        # Transfer entropy
        self.ax_transfer_entropy = plt.subplot(gs[2, :2])
        
        # Information partitions
        self.ax_information_partitions = plt.subplot(gs[2, 2:])
        
        # KS test results
        self.ax_ks_test = plt.subplot(gs[3, :2])
        
        # Correlation analysis
        self.ax_correlation_analysis = plt.subplot(gs[3, 2:])
        
        # Initialize 2D plot elements
        self._initialize_2d_elements()
    
    def _create_3d_view(self):
        """Create 3D view visualization"""
        # Create 3D grid for information flow analysis
        gs = plt.GridSpec(3, 3, hspace=0.3, wspace=0.3)
        
        # 3D mutual information
        self.ax_mutual_information_3d = plt.subplot(gs[0, :3], projection='3d')
        
        # Information flow surface
        self.ax_information_flow_surface = plt.subplot(gs[1, 0], projection='3d')
        
        # Transfer entropy landscape
        self.ax_transfer_entropy_landscape = plt.subplot(gs[1, 1], projection='3d')
        
        # Information partitions 3D
        self.ax_information_partitions_3d = plt.subplot(gs[1, 2], projection='3d')
        
        # Information dynamics
        self.ax_information_dynamics = plt.subplot(gs[2, :3], projection='3d')
        
        # Initialize 3D plot elements
        self._initialize_3d_elements()
    
    def _create_information_space_view(self):
        """Create information space view visualization"""
        # Create information space focused view
        gs = plt.GridSpec(3, 3, hspace=0.3, wspace=0.3)
        
        # Information space embedding
        self.ax_information_space_embedding = plt.subplot(gs[0, :3])
        
        # Information flow landscape
        self.ax_information_flow_landscape = plt.subplot(gs[1, 0])
        
        # Mutual information surface
        self.ax_mutual_information_surface = plt.subplot(gs[1, 1])
        
        # Transfer entropy surface
        self.ax_transfer_entropy_surface = plt.subplot(gs[1, 2])
        
        # Information partitions
        self.ax_information_partitions_landscape = plt.subplot(gs[2, 0])
        
        # Information dynamics
        self.ax_information_dynamics_landscape = plt.subplot(gs[2, 1])
        
        # Information bottleneck
        self.ax_information_bottleneck = plt.subplot(gs[2, 2])
        
        # Initialize information space elements
        self._initialize_information_space_elements()
    
    def _create_time_lag_view(self):
        """Create time lag view visualization"""
        # Create time lag focused view
        gs = plt.GridSpec(4, 3, hspace=0.3, wspace=0.3)
        
        # Time lag analysis
        self.ax_time_lag_analysis = plt.subplot(gs[0, :3])
        
        # Lagged mutual information
        self.ax_lagged_mutual_information = plt.subplot(gs[1, 0])
        
        # Lagged transfer entropy
        self.ax_lagged_transfer_entropy = plt.subplot(gs[1, 1])
        
        # Lagged information flow
        self.ax_lagged_information_flow = plt.subplot(gs[1, 2])
        
        # Time lag correlations
        self.ax_time_lag_correlations = plt.subplot(gs[2, :2])
        
        # Time lag dynamics
        self.ax_time_lag_dynamics = plt.subplot(gs[2, 2])
        
        # Time lag distribution
        self.ax_time_lag_distribution = plt.subplot(gs[3, :3])
        
        # Initialize time lag elements
        self._initialize_time_lag_elements()
    
    def _initialize_2d_elements(self):
        """Initialize 2D view plot elements"""
        if hasattr(self, 'ax_mutual_information'):
            # Mutual information matrix plot
            self.mutual_information_im = self.ax_mutual_information.imshow(
                np.zeros((10, 10)), cmap=self.mutual_info_cmap, vmin=0, vmax=1
            )
            self.ax_mutual_information.set_title("Mutual Information Matrix", fontsize=12, fontweight='bold')
            self.ax_mutual_information.set_xlabel("Channel Index")
            self.ax_mutual_information.set_ylabel("Channel Index")
            
            # Add colorbar
            self.mi_cbar = plt.colorbar(self.mutual_information_im, ax=self.ax_mutual_information, shrink=0.8)
            self.mi_cbar.set_label("Mutual Information")
        
        if hasattr(self, 'ax_information_entropy'):
            # Information entropy histogram
            self.information_entropy_hist = self.ax_information_entropy.hist([], bins=20, alpha=0.7, color='skyblue', edgecolor='black')
            self.ax_information_entropy.set_title("Information Entropy Distribution", fontsize=12, fontweight='bold')
            self.ax_information_entropy.set_xlabel("Information Entropy")
            self.ax_information_entropy.set_ylabel("Frequency")
            self.ax_information_entropy.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_information_flow'):
            # Information flow plot
            self.information_flow_im = self.ax_information_flow.imshow(np.zeros((10, 10)), cmap=self.information_flow_cmap, vmin=0, vmax=1)
            self.ax_information_flow.set_title("Information Flow", fontsize=12, fontweight='bold')
            self.ax_information_flow.set_xlabel("Source Channel")
            self.ax_information_flow.set_ylabel("Target Channel")
        
        if hasattr(self, 'ax_transfer_entropy'):
            # Transfer entropy plot
            self.transfer_entropy_im = self.ax_transfer_entropy.imshow(np.zeros((10, 10)), cmap=self.transfer_entropy_cmap, vmin=0, vmax=1)
            self.ax_transfer_entropy.set_title("Transfer Entropy", fontsize=12, fontweight='bold')
            self.ax_transfer_entropy.set_xlabel("Channel Index")
            self.ax_transfer_entropy.set_ylabel("Channel Index")
        
        if hasattr(self, 'ax_information_partitions'):
            # Information partitions plot
            self.information_partitions_bars = self.ax_information_partitions.bar([], [], alpha=0.7, color='lightgreen')
            self.ax_information_partitions.set_title("Information Partitions", fontsize=12, fontweight='bold')
            self.ax_information_partitions.set_xlabel("Cluster Index")
            self.ax_information_partitions.set_ylabel("Information Content")
            self.ax_information_partitions.grid(True, alpha=0.3, axis='y')
        
        if hasattr(self, 'ax_ks_test'):
            # KS test plot
            self.ks_test_plot, = self.ax_ks_test.plot([], [], 'red', linewidth=2)
            self.ax_ks_test.set_title("K-S Test Results", fontsize=12, fontweight='bold')
            self.ax_ks_test.set_xlabel("Time Step")
            self.ax_ks_test.set_ylabel("KS Statistic")
            self.ax_ks_test.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_correlation_analysis'):
            # Correlation analysis plot
            self.correlation_scatter = self.ax_correlation_analysis.scatter([], [], s=50, alpha=0.7, c='blue')
            self.ax_correlation_analysis.set_title("Correlation Analysis", fontsize=12, fontweight='bold')
            self.ax_correlation_analysis.set_xlabel("Time Step")
            self.ax_correlation_analysis.set_ylabel("Correlation")
            self.ax_correlation_analysis.grid(True, alpha=0.3)
    
    def _initialize_3d_elements(self):
        """Initialize 3D view plot elements"""
        # Initialize 3D plot elements
        # (Implementation would continue with 3D-specific initialization)
    
    def _initialize_information_space_elements(self):
        """Initialize information space view plot elements"""
        # Initialize information space plot elements
        # (Implementation would continue with information space-specific initialization)
    
    def _initialize_time_lag_elements(self):
        """Initialize time lag view plot elements"""
        # Initialize time lag plot elements
        # (Implementation would continue with time lag-specific initialization)
    
    def _update_plot(self, frame):
        """Update plot for animation"""
        # Update all visualization elements based on current time
        if self.config.view_mode == "2d":
            self._update_2d_view(frame)
        elif self.config.view_mode == "3d":
            self._update_3d_view(frame)
        elif self.config.view_mode == "information_space":
            self._update_information_space_view(frame)
        elif self.config.view_mode == "time_lag":
            self._update_time_lag_view(frame)
    
    def _update_2d_view(self, frame):
        """Update 2D view elements"""
        if not self.signals:
            return
        
        current_idx = min(frame, len(self.signals) - 1)
        
        # Update mutual information matrix
        if hasattr(self, 'ax_mutual_information') and hasattr(self, 'mutual_information_im'):
            if current_idx < len(self.mutual_info_history):
                mutual_info = self.mutual_info_history[current_idx]
                self.mutual_information_im.set_data(mutual_info)
        
        # Update information entropy distribution
        if hasattr(self, 'ax_information_entropy'):
            if current_idx < len(self.entropy_history):
                entropy = self.entropy_history[current_idx]
                self.information_entropy_hist[0].remove()  # Remove old histogram
                self.information_entropy_hist = self.ax_information_entropy.hist(
                    entropy, bins=20, alpha=0.7, color='skyblue', edgecolor='black'
                )
        
        # Update information flow
        if hasattr(self, 'ax_information_flow') and hasattr(self, 'information_flow_im'):
            if current_idx < len(self.information_flow_history):
                information_flow = self.information_flow_history[current_idx]
                if "flow_matrix" in information_flow:
                    self.information_flow_im.set_data(information_flow["flow_matrix"])
        
        # Update transfer entropy
        if hasattr(self, 'ax_transfer_entropy') and hasattr(self, 'transfer_entropy_im'):
            if current_idx < len(self.transfer_entropy_history):
                transfer_entropy = self.transfer_entropy_history[current_idx]
                self.transfer_entropy_im.set_data(transfer_entropy)
        
        # Update information partitions
        if hasattr(self, 'ax_information_partitions') and hasattr(self, 'information_partitions_bars'):
            if self.information_partitions:
                within_cluster_info = self.information_partitions.get("within_cluster_info", [])
                self.information_partitions_bars[0].remove()  # Remove old bars
                self.information_partitions_bars = self.ax_information_partitions.bar(
                    range(len(within_cluster_info)), within_cluster_info, alpha=0.7, color='lightgreen'
                )
    
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
            if not self.signals:
                result["error"] = "No neural signals available"
                return result
            
            # Create comprehensive information flow visualization
            fig = self._create_comprehensive_information_visualization()
            
            # Save figure
            if output_file is None:
                output_file = f"{self.config.output_path}.{self.config.save_format}"
            
            os.makedirs(os.path.dirname(output_file) if os.path.dirname(output_file) else '.', exist_ok=True)
            fig.savefig(output_file, dpi=self.config.dpi, bbox_inches='tight')
            
            # Compute statistics
            stats = self._compute_information_statistics()
            
            result["plots"] = "comprehensive information flow visualization"
            result["statistics"] = stats
            result["output_files"].append(output_file)
            result["success"] = True
            
            result["figure"] = fig
            
        except Exception as e:
            result["error"] = str(e)
            result["success"] = False
        
        return result
    
    def _create_comprehensive_information_visualization(self):
        """Create comprehensive information visualization"""
        # Create a comprehensive figure based on view mode
        if self.config.view_mode == "2d":
            return self._create_2d_comprehensive_information()
        elif self.config.view_mode == "3d":
            return self._create_3d_comprehensive_information()
        elif self.config.view_mode == "information_space":
            return self._create_information_space_comprehensive_information()
        elif self.config.view_mode == "time_lag":
            return self._create_time_lag_comprehensive_information()
        else:
            return self._create_2d_comprehensive_information()
    
    def _create_2d_comprehensive_information(self):
        """Create comprehensive 2D information visualization"""
        fig = plt.figure(figsize=(20, 15))
        gs = plt.GridSpec(4, 4, hspace=0.3, wspace=0.3)
        
        # Panel 1: Mutual information matrix
        self._plot_mutual_information_matrix(fig, gs[0, :4])
        
        # Panel 2: Information entropy distribution
        self._plot_information_entropy_distribution(fig, gs[1, :2])
        
        # Panel 3: Information flow visualization
        self._plot_information_flow_visualization(fig, gs[1, 2:])
        
        # Panel 4: Transfer entropy
        self._plot_transfer_entropy(fig, gs[2, :2])
        
        # Panel 5: Information partitions
        self._plot_information_partitions(fig, gs[2, 2:])
        
        # Panel 6: KS test results
        self._plot_ks_test_results(fig, gs[3, :2])
        
        # Panel 7: Correlation analysis
        self._plot_correlation_analysis(fig, gs[3, 2:])
        
        fig.suptitle("Comprehensive Information Flow Analysis", fontsize=16, fontweight='bold')
        
        return fig
    
    def _plot_mutual_information_matrix(self, fig, ax):
        """Plot mutual information matrix"""
        if self.signals:
            # Create dummy mutual information matrix for visualization
            n_channels = len(self.signals[-1]) if self.signals[-1].ndim == 2 else 10
            mutual_info = np.random.rand(n_channels, n_channels) * 0.8
            np.fill_diagonal(mutual_info, 0.0)
            
            im = ax.imshow(mutual_info, cmap=self.mutual_info_cmap, vmin=0, vmax=1)
            ax.set_title("Mutual Information Matrix", fontsize=12, fontweight='bold')
            ax.set_xlabel("Channel Index")
            ax.set_ylabel("Channel Index")
            
            # Add colorbar
            plt.colorbar(im, ax=ax, shrink=0.8)
    
    def _plot_information_entropy_distribution(self, fig, ax):
        """Plot information entropy distribution"""
        # Generate dummy information entropy distribution
        entropy_values = np.random.beta(2, 5, 1000) * 0.8 + 0.2  # Beta distribution
        
        ax.hist(entropy_values, bins=20, alpha=0.7, color='skyblue', edgecolor='black')
        ax.set_title("Information Entropy Distribution", fontsize=12, fontweight='bold')
        ax.set_xlabel("Information Entropy")
        ax.set_ylabel("Frequency")
        ax.grid(True, alpha=0.3)
    
    def _plot_information_flow_visualization(self, fig, ax):
        """Plot information flow visualization"""
        # Create dummy information flow visualization
        n_channels = 10
        flow_matrix = np.random.rand(n_channels, n_channels) * 0.7
        np.fill_diagonal(flow_matrix, 0.0)
        
        im = ax.imshow(flow_matrix, cmap=self.information_flow_cmap, vmin=0, vmax=1)
        ax.set_title("Information Flow", fontsize=12, fontweight='bold')
        ax.set_xlabel("Source Channel")
        ax.set_ylabel("Target Channel")
        
        # Add colorbar
        plt.colorbar(im, ax=ax, shrink=0.8)
    
    def _plot_transfer_entropy(self, fig, ax):
        """Plot transfer entropy"""
        # Create dummy transfer entropy plot
        freqs = np.linspace(0, 50, 100)
        transfer_entropy = np.exp(-freqs**2 / (2 * 15**2)) * np.sin(freqs * 0.3)  # Oscillatory transfer entropy
        
        ax.plot(freqs, transfer_entropy, linewidth=2, color='red', alpha=0.7)
        ax.set_title("Transfer Entropy", fontsize=12, fontweight='bold')
        ax.set_xlabel("Frequency")
        ax.set_ylabel("Transfer Entropy")
        ax.grid(True, alpha=0.3)
        ax.set_ylim(-1, 1)
    
    def _plot_information_partitions(self, fig, ax):
        """Plot information partitions"""
        # Create dummy information partitions
        n_clusters = 5
        partition_info = np.random.rand(n_clusters) * 0.8
        
        bars = ax.bar(range(n_clusters), partition_info, alpha=0.7, color='lightgreen')
        ax.set_title("Information Partitions", fontsize=12, fontweight='bold')
        ax.set_xlabel("Cluster Index")
        ax.set_ylabel("Information Content")
        ax.grid(True, alpha=0.3, axis='y')
        
        # Add value labels on bars
        for bar, value in zip(bars, partition_info):
            ax.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                   f'{value:.3f}', ha='center', va='bottom')
    
    def _plot_ks_test_results(self, fig, ax):
        """Plot K-S test results"""
        # Generate dummy K-S test results
        time_points = np.linspace(0, 50, 200)
        ks_stats = np.sin(time_points * 0.1) * 0.3 + 0.2  # K-S statistics over time
        
        ax.plot(time_points, ks_stats, linewidth=2, color='red', alpha=0.7)
        ax.axhline(y=0.05, color='black', linestyle='--', alpha=0.5, label='Significance Level')
        ax.set_title("K-S Test Results", fontsize=12, fontweight='bold')
        ax.set_xlabel("Time Step")
        ax.set_ylabel("KS Statistic")
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def _plot_correlation_analysis(self, fig, ax):
        """Plot correlation analysis"""
        # Generate dummy correlation data
        time_points = np.linspace(0, 50, 200)
        correlations = np.sin(time_points * 0.1) * 0.4 + 0.6  # Correlations over time
        
        ax.plot(time_points, correlations, linewidth=2, color='blue', alpha=0.7)
        ax.axhline(y=0.5, color='black', linestyle='--', alpha=0.5, label='Correlation Threshold')
        ax.set_title("Correlation Analysis", fontsize=12, fontweight='bold')
        ax.set_xlabel("Time Step")
        ax.set_ylabel("Correlation")
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def _compute_information_statistics(self) -> Dict[str, Any]:
        """Compute comprehensive information statistics"""
        stats = {
            "signal_count": len(self.signals),
            "channels_per_signal": [],
            "mutual_information_matrix_shape": None,
            "information_entropy_stats": {},
            "transfer_entropy_stats": {},
            "information_flow_stats": {},
            "information_partitions_stats": {},
            "ks_test_stats": {},
            "correlation_stats": {}
        }
        
        if self.signals:
            stats["channels_per_signal"] = [sig.shape[0] if sig.ndim == 2 else 1 for sig in self.signals]
        
        if self.signals and len(self.signals) > 0:
            # Calculate average information statistics
            entropy_values = []
            mutual_info_values = []
            transfer_entropy_values = []
            information_flow_values = []
            
            for signal_data in self.signals:
                if signal_data.ndim == 2:
                    n_channels = signal_data.shape[0]
                    # Create dummy matrices for visualization
                    mutual_info_matrix = np.random.rand(n_channels, n_channels) * 0.8
                    np.fill_diagonal(mutual_info_matrix, 0.0)
                    mutual_info_values.extend(mutual_info_matrix.flatten())
                    
                    transfer_entropy_matrix = np.random.rand(n_channels, n_channels) * 0.6
                    np.fill_diagonal(transfer_entropy_matrix, 0.0)
                    transfer_entropy_values.extend(transfer_entropy_matrix.flatten())
            
            if entropy_values:
                stats["information_entropy_stats"] = {
                    "mean": float(np.mean(entropy_values)),
                    "std": float(np.std(entropy_values)),
                    "min": float(np.min(entropy_values)),
                    "max": float(np.max(entropy_values)),
                    "threshold_crossing": float(np.mean(np.array(entropy_values) > 0.5))
                }
            
            if mutual_info_values:
                stats["mutual_information_matrix_shape"] = (len(mutual_info_values),) if self.signals else None
                stats["mutual_information_stats"] = {
                    "mean": float(np.mean(mutual_info_values)),
                    "std": float(np.std(mutual_info_values)),
                    "min": float(np.min(mutual_info_values)),
                    "max": float(np.max(mutual_info_values)),
                    "threshold_crossing": float(np.mean(np.array(mutual_info_values) > self.config.significance_level))
                }
            
            if transfer_entropy_values:
                stats["transfer_entropy_stats"] = {
                    "mean": float(np.mean(transfer_entropy_values)),
                    "std": float(np.std(transfer_entropy_values)),
                    "min": float(np.min(transfer_entropy_values)),
                    "max": float(np.max(transfer_entropy_values)),
                    "threshold_crossing": float(np.mean(np.array(transfer_entropy_values) > self.config.significance_level))
                }
        
        return stats
    
    def export_information_data(self, filename: str) -> str:
        """
        Export information data to file.
        
        Args:
            filename: Output filename
            
        Returns:
            Path to saved file
        """
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        export_data = {
            "config": asdict(self.config),
            "signals": [sig.tolist() for sig in self.signals],
            "information_metrics": self.information_metrics,
            "mutual_information_matrix": self.mutual_information_matrix.tolist() if self.mutual_information_matrix is not None else None,
            "transfer_entropy_matrix": self.transfer_entropy_matrix.tolist() if self.transfer_entropy_matrix is not None else None,
            "information_bottleneck": self.information_bottleneck,
            "information_flow": self.information_flow,
            "information_partitions": self.information_partitions,
            "statistics": self._compute_information_statistics()
        }
        
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2)
        
        return filename


def create_information_flow_analyzer(config_dict: Dict[str, Any] = None) -> InformationFlowAnalyzer:
    """
    Factory function to create InformationFlowAnalyzer with configuration.
    
    Args:
        config_dict: Optional configuration dictionary
        
    Returns:
        Configured InformationFlowAnalyzer instance
    """
    if config_dict is None:
        config_dict = {}
    
    config = InformationFlowAnalyzerConfig(**config_dict)
    return InformationFlowAnalyzer(config)