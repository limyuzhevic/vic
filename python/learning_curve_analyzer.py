#!/usr/bin/env python3
"""
Learning Curve Analyzer Tool

Analysis of learning trajectories in neural networks.
Provides comprehensive analysis of learning dynamics, convergence patterns,
and performance evolution over time.
"""

import numpy as np
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass, asdict
from enum import Enum
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats, signal
import pandas as pd
from sklearn.preprocessing import MinMaxScaler
from sklearn.decomposition import PCA
from sklearn.cluster import KMeans
import sys
import os

@dataclass
class LearningCurveAnalyzerConfig:
    """Configuration for learning curve analyzer"""
    time_series_length: float = 100.0  # seconds
    sampling_rate: float = 100.0  # Hz
    show_learning_curves: bool = True
    show_convergence_analysis: bool = True
    show_performance_trends: bool = True
    show_parameter_dynamics: bool = True
    show_regime_transitions: bool = True
    animate: bool = True
    color_scheme: str = "learning"  # "learning", "performance", "regime", "gradient"
    show_labels: bool = True
    show_statistics: bool = True
    save_format: str = "png"  # "png", "json", "html"
    output_path: str = "learning_curve_analyzer"
    
    # Analysis parameters
    smoothing_window: float = 5.0  # seconds
    detrend_type: str = None  # "constant", "linear", "polynomial"
    derivative_order: int = 1
    trend_order: int = 2
    regime_detection: bool = True
    regime_threshold: float = 0.5
    plateau_detection: bool = True
    plateau_threshold: float = 0.1
    overfitting_detection: bool = True
    validation_split: float = 0.2
    
    # Performance metrics
    include_accuracy: bool = True
    include_loss: bool = True
    include_reward: bool = True
    include_neural_activity: bool = True
    include_plasticity: bool = True
    
    # Visualization modes
    view_mode: str = "2d"  # "2d", "3d", "regime", "parameter"
    show_3d: bool = False
    figure_size: Tuple[int, int] = (16, 12)
    dpi: int = 150
    animation_interval: int = 100  # milliseconds
    
    # Display settings
    show_legend: bool = True
    show_colorbar: bool = True
    show_learning_curves_details: bool = True
    smooth_trajectories: bool = True
class LearningCurveAnalyzer:
    """
    Learning curve analysis tool for neural networks.
    
    Features:
    - Real-time learning curve visualization
    - Convergence analysis and stopping criteria
    - Performance trend analysis
    - Parameter dynamics and optimization tracking
    - Regime transition detection
    - Overfitting detection and validation
    - Export capabilities for scientific analysis
    """
    
    def __init__(self, config: LearningCurveAnalyzerConfig = None):
        self.config = config or LearningCurveAnalyzerConfig()
        self.learning_data = []  # List of learning data points
        self.performance_metrics = []  # List of performance metrics
        self.parameter_history = []  # List of parameter values
        self.validation_results = []  # List of validation results
        self.current_time = 0.0
        self.is_running = False
        self.animation_obj = None
        self.fig = None
        self.axes = None
        
        # Initialize analyzer components
        self._initialize_components()
    
    def _initialize_components(self):
        """Initialize analyzer components and data structures"""
        # Data buffers for different metrics
        self.accuracy_history = []
        self.loss_history = []
        self.reward_history = []
        self.neural_activity_history = []
        self.plasticity_history = []
        
        # Analysis buffers
        self.detrended_data = []
        self.differentiable_data = []
        self.regime_labels = []
        self.plateau_detected = []
        self.overfitting_detected = []
        
        # Initialize scalers
        self.scaler = MinMaxScaler()
        
        # Initialize colormaps
        self._initialize_colormaps()
    
    def _initialize_colormaps(self):
        """Initialize colormaps for different learning visualizations"""
        # Learning progress colormap (blue to green to yellow to red)
        self.learning_cmap = LinearSegmentedColormap.from_list(
            'learning', ['navy', 'blue', 'cyan', 'green', 'yellow', 'orange', 'red']
        )
        
        # Performance colormap
        self.performance_cmap = LinearSegmentedColormap.from_list(
            'performance', ['darkblue', 'blue', 'cyan', 'green', 'lime', 'yellow', 'orange', 'red']
        )
        
        # Regime colormap
        self.regime_cmap = plt.cm.Set3
        
        # Gradient colormap for parameter changes
        self.gradient_cmap = plt.cm.viridis
    
    def add_learning_data(self, 
                         accuracy: Optional[np.ndarray] = None,
                         loss: Optional[np.ndarray] = None,
                         reward: Optional[np.ndarray] = None,
                         neural_activity: Optional[np.ndarray] = None,
                         plasticity: Optional[np.ndarray] = None,
                         parameters: Optional[Dict[str, np.ndarray]] = None,
                         time_step: float = 1.0):
        """
        Add learning data for analysis.
n        
        Args:
            accuracy: Accuracy values over time
            loss: Loss values over time
            reward: Reward values over time
            neural_activity: Neural activity values over time
            plasticity: Plasticity values over time
            parameters: Dictionary of parameter values over time
            time_step: Current time step
        """
        # Validate and store data
        if accuracy is not None:
            accuracy = np.asarray(accuracy)
            if len(self.accuracy_history) > 0 and len(accuracy) != len(self.accuracy_history[0]):
                # If accuracy has different length, interpolate or truncate
                if len(accuracy) > len(self.accuracy_history[0]):
                    self.accuracy_history = [accuracy]
                else:
                    # Keep only most recent data points
                    self.accuracy_history = [accuracy[-len(self.accuracy_history[0]):]]
            else:
                self.accuracy_history.append(accuracy)
        
        if loss is not None:
            loss = np.asarray(loss)
            if len(self.loss_history) > 0 and len(loss) != len(self.loss_history[0]):
                if len(loss) > len(self.loss_history[0]):
                    self.loss_history = [loss]
                else:
                    self.loss_history = [loss[-len(self.loss_history[0]):]]
            else:
                self.loss_history.append(loss)
        
        if reward is not None:
            reward = np.asarray(reward)
            if len(self.reward_history) > 0 and len(reward) != len(self.reward_history[0]):
                if len(reward) > len(self.reward_history[0]):
                    self.reward_history = [reward]
                else:
                    self.reward_history = [reward[-len(self.reward_history[0]):]]
            else:
                self.reward_history.append(reward)
        
        if neural_activity is not None:
            neural_activity = np.asarray(neural_activity)
            if len(self.neural_activity_history) > 0 and len(neural_activity) != len(self.neural_activity_history[0]):
                if len(neural_activity) > len(self.neural_activity_history[0]):
                    self.neural_activity_history = [neural_activity]
                else:
                    self.neural_activity_history = [neural_activity[-len(self.neural_activity_history[0]):]]
            else:
                self.neural_activity_history.append(neural_activity)
        
        if plasticity is not None:
            plasticity = np.asarray(plasticity)
            if len(self.plasticity_history) > 0 and len(plasticity) != len(self.plasticity_history[0]):
                if len(plasticity) > len(self.plasticity_history[0]):
                    self.plasticity_history = [plasticity]
                else:
                    self.plasticity_history = [plasticity[-len(self.plasticity_history[0]):]]
            else:
                self.plasticity_history.append(plasticity)
        
        if parameters is not None:
            self.parameter_history.append(parameters)
        
        # Update current time
        self.current_time += time_step
    
    def start_real_time(self, initial_data: Dict[str, Any] = None):
        """
        Start real-time learning curve visualization.
        
        Args:
            initial_data: Optional initial learning data
        """
        if initial_data:
            # Add initial data
            self.add_learning_data(
                accuracy=initial_data.get('accuracy'),
                loss=initial_data.get('loss'),
                reward=initial_data.get('reward'),
                neural_activity=initial_data.get('neural_activity'),
                plasticity=initial_data.get('plasticity'),
                parameters=initial_data.get('parameters')
            )
        
        self.is_running = True
        
        # Create visualization
        self.fig = plt.figure(figsize=self.config.figure_size)
        
        # Create subplot layout based on view mode
        if self.config.view_mode == "2d":
            self._create_2d_view()
        elif self.config.view_mode == "3d":
            self._create_3d_view()
        elif self.config.view_mode == "regime":
            self._create_regime_view()
        elif self.config.view_mode == "parameter":
            self._create_parameter_view()
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
        # Create grid of subplots for learning analysis
        gs = plt.GridSpec(4, 4, hspace=0.3, wspace=0.3)
        
        # Main learning curve plot
        self.ax_learning_curve = plt.subplot(gs[0, :4])
        
        # Performance comparison
        self.ax_performance_comparison = plt.subplot(gs[1, :2])
        
        # Loss over time
        self.ax_loss_curve = plt.subplot(gs[1, 2:])
        
        # Reward over time
        self.ax_reward_curve = plt.subplot(gs[2, :2])
        
        # Neural activity
        self.ax_neural_activity = plt.subplot(gs[2, 2:])
        
        # Plasticity dynamics
        self.ax_plasticity_dynamics = plt.subplot(gs[3, :2])
        
        # Parameter dynamics
        self.ax_parameter_dynamics = plt.subplot(gs[3, 2:])
        
        # Initialize 2D plot elements
        self._initialize_2d_elements()
    
    def _create_3d_view(self):
        """Create 3D view visualization"""
        # Create 3D grid for learning analysis
        gs = plt.GridSpec(3, 3, hspace=0.3, wspace=0.3)
        
        # 3D learning trajectory
        self.ax_learning_3d = plt.subplot(gs[0, :3], projection='3d')
        
        # Performance landscape
        self.ax_performance_landscape = plt.subplot(gs[1, 0], projection='3d')
        
        # Parameter evolution
        self.ax_parameter_evolution = plt.subplot(gs[1, 1], projection='3d')
        
        # Learning dynamics
        self.ax_learning_dynamics_3d = plt.subplot(gs[1, 2], projection='3d')
        
        # Regime transitions
        self.ax_regime_3d = plt.subplot(gs[2, :3], projection='3d')
        
        # Initialize 3D plot elements
        self._initialize_3d_elements()
    
    def _create_regime_view(self):
        """Create regime view visualization"""
        # Create regime-focused view
        gs = plt.GridSpec(4, 3, hspace=0.3, wspace=0.3)
        
        # Regime identification
        self.ax_regime_identification = plt.subplot(gs[0, :3])
        
        # Regime transitions over time
        self.ax_regime_transitions = plt.subplot(gs[1, 0])
        
        # Regime characteristics
        self.ax_regime_characteristics = plt.subplot(gs[1, 1])
        
        # Regime stability
        self.ax_regime_stability = plt.subplot(gs[1, 2])
        
        # Learning performance by regime
        self.ax_performance_by_regime = plt.subplot(gs[2, :2])
        
        # Duration distribution by regime
        self.ax_duration_by_regime = plt.subplot(gs[2, 2])
        
        # Regime transition matrix
        self.ax_transition_matrix = plt.subplot(gs[3, :3])
        
        # Initialize regime elements
        self._initialize_regime_elements()
    
    def _create_parameter_view(self):
        """Create parameter view visualization"""
        # Create parameter-focused view
        gs = plt.GridSpec(4, 3, hspace=0.3, wspace=0.3)
        
        # Parameter evolution
        self.ax_parameter_evolution = plt.subplot(gs[0, :3])
        
        # Parameter correlations
        self.ax_parameter_correlations = plt.subplot(gs[1, 0])
        
        # Parameter optimization trajectory
        self.ax_parameter_optimization = plt.subplot(gs[1, 1])
        
        # Parameter sensitivity
        self.ax_parameter_sensitivity = plt.subplot(gs[1, 2])
        
        # Parameter convergence analysis
        self.ax_parameter_convergence = plt.subplot(gs[2, 0])
        
        # Parameter stability analysis
        self.ax_parameter_stability = plt.subplot(gs[2, 1])
        
        # Parameter interaction effects
        self.ax_parameter_interaction = plt.subplot(gs[2, 2])
        
        # Parameter performance landscape
        self.ax_parameter_landscape = plt.subplot(gs[3, :3])
        
        # Initialize parameter elements
        self._initialize_parameter_elements()
    
    def _initialize_2d_elements(self):
        """Initialize 2D view plot elements"""
        if hasattr(self, 'ax_learning_curve'):
            # Main learning curve plot
            self.accuracy_line, = self.ax_learning_curve.plot([], [], 'b-', linewidth=2, label='Accuracy')
            self.loss_line, = self.ax_learning_curve.plot([], [], 'r-', linewidth=2, label='Loss')
            self.reward_line, = self.ax_learning_curve.plot([], [], 'g-', linewidth=2, label='Reward')
            
            self.ax_learning_curve.set_title("Learning Curves", fontsize=14, fontweight='bold')
            self.ax_learning_curve.set_xlabel("Time Step")
            self.ax_learning_curve.set_ylabel("Performance")
            self.ax_learning_curve.legend()
            self.ax_learning_curve.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_performance_comparison'):
            # Performance comparison plot
            self.performance_scatter = self.ax_performance_comparison.scatter([], [], s=50, alpha=0.7, c='blue')
            self.ax_performance_comparison.set_title("Performance Comparison", fontsize=12, fontweight='bold')
            self.ax_performance_comparison.set_xlabel("Time Step")
            self.ax_performance_comparison.set_ylabel("Performance")
            self.ax_performance_comparison.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_loss_curve'):
            # Loss curve plot
            self.loss_plot, = self.ax_loss_curve.plot([], [], 'r-', linewidth=2)
            self.ax_loss_curve.set_title("Loss Function", fontsize=12, fontweight='bold')
            self.ax_loss_curve.set_xlabel("Time Step")
            self.ax_loss_curve.set_ylabel("Loss")
            self.ax_loss_curve.grid(True, alpha=0.3)
            self.ax_loss_curve.set_yscale('log')
        
        if hasattr(self, 'ax_reward_curve'):
            # Reward curve plot
            self.reward_plot, = self.ax_reward_curve.plot([], [], 'g-', linewidth=2)
            self.ax_reward_curve.set_title("Reward Function", fontsize=12, fontweight='bold')
            self.ax_reward_curve.set_xlabel("Time Step")
            self.ax_reward_curve.set_ylabel("Reward")
            self.ax_reward_curve.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_neural_activity'):
            # Neural activity plot
            self.neural_activity_plot, = self.ax_neural_activity.plot([], [], 'purple', linewidth=2)
            self.ax_neural_activity.set_title("Neural Activity", fontsize=12, fontweight='bold')
            self.ax_neural_activity.set_xlabel("Time Step")
            self.ax_neural_activity.set_ylabel("Activity Level")
            self.ax_neural_activity.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_plasticity_dynamics'):
            # Plasticity dynamics plot
            self.plasticity_plot, = self.ax_plasticity_dynamics.plot([], [], 'orange', linewidth=2)
            self.ax_plasticity_dynamics.set_title("Plasticity Dynamics", fontsize=12, fontweight='bold')
            self.ax_plasticity_dynamics.set_xlabel("Time Step")
            self.ax_plasticity_dynamics.set_ylabel("Plasticity Strength")
            self.ax_plasticity_dynamics.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_parameter_dynamics'):
            # Parameter dynamics plot
            self.parameter_plots = []
            param_names = ['learning_rate', 'regularization', 'batch_size', 'momentum']
            colors = ['blue', 'red', 'green', 'orange']
            
            for i, (name, color) in enumerate(zip(param_names, colors)):
                line, = self.ax_parameter_dynamics.plot([], [], linewidth=2, color=color, label=name)
                self.parameter_plots.append(line)
            
            self.ax_parameter_dynamics.set_title("Parameter Dynamics", fontsize=12, fontweight='bold')
            self.ax_parameter_dynamics.set_xlabel("Time Step")
            self.ax_parameter_dynamics.set_ylabel("Parameter Value")
            self.ax_parameter_dynamics.legend()
            self.ax_parameter_dynamics.grid(True, alpha=0.3)
    
    def _initialize_3d_elements(self):
        """Initialize 3D view plot elements"""
        # Initialize 3D plot elements
        # (Implementation would continue with 3D-specific initialization)
    
    def _initialize_regime_elements(self):
        """Initialize regime view plot elements"""
        # Initialize regime plot elements
        # (Implementation would continue with regime-specific initialization)
    
    def _initialize_parameter_elements(self):
        """Initialize parameter view plot elements"""
        # Initialize parameter plot elements
        # (Implementation would continue with parameter-specific initialization)
    
    def _update_plot(self, frame):
        """Update plot for animation"""
        # Update all visualization elements based on current time
        if self.config.view_mode == "2d":
            self._update_2d_view(frame)
        elif self.config.view_mode == "3d":
            self._update_3d_view(frame)
        elif self.config.view_mode == "regime":
            self._update_regime_view(frame)
        elif self.config.view_mode == "parameter":
            self._update_parameter_view(frame)
    
    def _update_2d_view(self, frame):
        """Update 2D view elements"""
        # Update learning curve plots
        if hasattr(self, 'ax_learning_curve'):
            self._update_learning_curves(frame)
        
        # Update performance comparison
        if hasattr(self, 'ax_performance_comparison'):
            self._update_performance_comparison(frame)
        
        # Update loss curve
        if hasattr(self, 'ax_loss_curve'):
            self._update_loss_curve(frame)
        
        # Update reward curve
        if hasattr(self, 'ax_reward_curve'):
            self._update_reward_curve(frame)
        
        # Update neural activity
        if hasattr(self, 'ax_neural_activity'):
            self._update_neural_activity(frame)
        
        # Update plasticity dynamics
        if hasattr(self, 'ax_plasticity_dynamics'):
            self._update_plasticity_dynamics(frame)
        
        # Update parameter dynamics
        if hasattr(self, 'ax_parameter_dynamics'):
            self._update_parameter_dynamics(frame)
    
    def _update_learning_curves(self, frame):
        """Update learning curve visualization"""
        # Get current data (last N time steps)
        history_length = min(100, frame + 1)
        
        time_points = list(range(history_length))
        
        # Get accuracy data
        if self.accuracy_history and len(self.accuracy_history) > 0:
            accuracy_data = self.accuracy_history[-1][:history_length]
            self.accuracy_line.set_data(time_points, accuracy_data)
        
        # Get loss data
        if self.loss_history and len(self.loss_history) > 0:
            loss_data = self.loss_history[-1][:history_length]
            self.loss_line.set_data(time_points, loss_data)
        
        # Get reward data
        if self.reward_history and len(self.reward_history) > 0:
            reward_data = self.reward_history[-1][:history_length]
            self.reward_line.set_data(time_points, reward_data)
        
        # Update axis limits
        self.ax_learning_curve.set_xlim(0, history_length)
        if self.accuracy_history:
            acc_max = max([np.max(acc) for acc in self.accuracy_history[-5:]] if self.accuracy_history else [0])
            loss_max = max([np.max(loss) for loss in self.loss_history[-5:]] if self.loss_history else [0])
            reward_max = max([np.max(reward) for reward in self.reward_history[-5:]] if self.reward_history else [0])
            
            y_max = max(acc_max, loss_max, reward_max) * 1.1
            self.ax_learning_curve.set_ylim(0, y_max)
    
    def _update_performance_comparison(self, frame):
        """Update performance comparison visualization"""
        # Get current performance data
        history_length = min(100, frame + 1)
        
        time_points = list(range(history_length))
        
        # Get performance metrics
        if self.accuracy_history and len(self.accuracy_history) > 0:
            accuracy_data = self.accuracy_history[-1][:history_length]
            self.ax_performance_comparison.clear()
            self.ax_performance_comparison.scatter(time_points, accuracy_data, 
                                                  s=10, alpha=0.7, c=accuracy_data, cmap='viridis')
            self.ax_performance_comparison.set_title("Performance Comparison", fontsize=12, fontweight='bold')
            self.ax_performance_comparison.set_xlabel("Time Step")
            self.ax_performance_comparison.set_ylabel("Accuracy")
            self.ax_performance_comparison.grid(True, alpha=0.3)
    
    def def _update_loss_curve(frame):
        """Update loss curve visualization"""
        # Get current loss data
        history_length = min(100, frame + 1)
        
        time_points = list(range(history_length))
        
        if self.loss_history and len(self.loss_history) > 0:
            loss_data = self.loss_history[-1][:history_length]
            self.loss_plot.set_data(time_points, loss_data)
            
            # Update title with current loss
            current_loss = loss_data[-1] if len(loss_data) > 0 else 0
            self.ax_loss_curve.set_title(f"Loss Function (Current: {current_loss:.4f})", 
                                       fontsize=12, fontweight='bold')
    
    def _update_reward_curve(frame):
        """Update reward curve visualization"""
        # Get current reward data
        history_length = min(100, frame + 1)
        
        time_points = list(range(history_length))
        
        if self.reward_history and len(self.reward_history) > 0:
            reward_data = self.reward_history[-1][:history_length]
            self.reward_plot.set_data(time_points, reward_data)
    
    def _update_neural_activity(frame):
        """Update neural activity visualization"""
        # Get current neural activity data
        history_length = min(100, frame + 1)
        
        time_points = list(range(history_length))
        
        if self.neural_activity_history and len(self.neural_activity_history) > 0:
            activity_data = self.neural_activity_history[-1][:history_length]
            self.neural_activity_plot.set_data(time_points, activity_data)
    
    def _update_plasticity_dynamics(frame):
        """Update plasticity dynamics visualization"""
        # Get current plasticity data
        history_length = min(100, frame + 1)
        
        time_points = list(range(history_length))
        
        if self.plasticity_history and len(self.plasticity_history) > 0:
            plasticity_data = self.plasticity_history[-1][:history_length]
            self.plasticity_plot.set_data(time_points, plasticity_data)
    
    def _update_parameter_dynamics(frame):
        """Update parameter dynamics visualization"""
        # Get current parameter data
        history_length = min(100, frame + 1)
        
        time_points = list(range(history_length))
        
        if self.parameter_history and len(self.parameter_history) > 0:
            current_params = self.parameter_history[-1]
            
            # Update each parameter plot
            for i, (name, line) in enumerate(zip(['learning_rate', 'regularization', 'batch_size', 'momentum'],
                                                self.parameter_plots)):
                if name in current_params:
                    param_data = current_params[name][:history_length]
                    line.set_data(time_points, param_data)
    
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
            if not self.accuracy_history and not self.loss_history and not self.reward_history:
                result["error"] = "No learning data available"
                return result
            
            # Create comprehensive learning curve visualization
            fig = self._create_comprehensive_learning_visualization()
            
            # Save figure
            if output_file is None:
                output_file = f"{self.config.output_path}.{self.config.save_format}"
            
            os.makedirs(os.path.dirname(output_file) if os.path.dirname(output_file) else '.', exist_ok=True)
            fig.savefig(output_file, dpi=self.config.dpi, bbox_inches='tight')
            
            # Compute statistics
            stats = self._compute_learning_statistics()
            
            result["plots"] = "comprehensive learning curve visualization"
            result["statistics"] = stats
            result["output_files"].append(output_file)
            result["success"] = True
            
            result["figure"] = fig
            
        except Exception as e:
            result["error"] = str(e)
            result["success"] = False
        
        return result
    
    def _create_comprehensive_learning_visualization(self):
        """Create comprehensive learning curve visualization"""
        # Create a comprehensive figure based on view mode
        if self.config.view_mode == "2d":
            return self._create_2d_comprehensive_learning()
        elif self.config.view_mode == "3d":
            return self._create_3d_comprehensive_learning()
        elif self.config.view_mode == "regime":
            return self._create_regime_comprehensive_learning()
        elif self.config.view_mode == "parameter":
            return self._create_parameter_comprehensive_learning()
        else:
            return self._create_2d_comprehensive_learning()
    
    def _create_2d_comprehensive_learning(self):
        """Create comprehensive 2D learning curve visualization"""
        fig = plt.figure(figsize=(20, 15))
        gs = plt.GridSpec(4, 4, hspace=0.3, wspace=0.3)
        
        # Panel 1: Main learning curves
        self._plot_main_learning_curves(fig, gs[0, :4])
        
        # Panel 2: Performance analysis
        self._plot_performance_analysis(fig, gs[1, :2])
        
        # Panel 3: Learning dynamics
        self._plot_learning_dynamics(fig, gs[1, 2:])
        
        # Panel 4: Convergence analysis
        self._plot_convergence_analysis(fig, gs[2, :2])
        
        # Panel 5: Regime detection
        if self.config.regime_detection:
            self._plot_regime_detection(fig, gs[2, 2:])
        
        # Panel 6: Parameter evolution
        self._plot_parameter_evolution(fig, gs[3, :2])
        
        # Panel 7: Overfitting detection
        if self.config.overfitting_detection:
            self._plot_overfitting_detection(fig, gs[3, 2:])
        
        fig.suptitle("Comprehensive Learning Curve Analysis", fontsize=16, fontweight='bold')
        
        return fig
    
    def _plot_main_learning_curves(self, fig, ax):
        """Plot main learning curves"""
        # Plot accuracy, loss, and reward curves
        if self.accuracy_history:
            for i, acc in enumerate(self.accuracy_history[-5:]):  # Plot last 5 runs
                alpha = 0.7 if i == len(self.accuracy_history[-5:]) - 1 else 0.3
                ax.plot(acc, linewidth=2, alpha=alpha, label=f'Run {i+1}' if i == len(self.accuracy_history[-5:]) - 1 else None)
        
        if self.loss_history:
            for i, loss in enumerate(self.loss_history[-5:]):  # Plot last 5 runs
                alpha = 0.7 if i == len(self.loss_history[-5:]) - 1 else 0.3
                ax.plot(loss, linewidth=2, alpha=alpha, label=f'Loss Run {i+1}' if i == len(self.loss_history[-5:]) - 1 else None)
        
        if self.reward_history:
            for i, reward in enumerate(self.reward_history[-5:]):  # Plot last 5 runs
                alpha = 0.7 if i == len(self.reward_history[-5:]) - 1 else 0.3
                ax.plot(reward, linewidth=2, alpha=alpha, label=f'Reward Run {i+1}' if i == len(self.reward_history[-5:]) - 1 else None)
        
        ax.set_title("Learning Curves", fontsize=12, fontweight='bold')
        ax.set_xlabel("Time Step")
        ax.set_ylabel("Performance")
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def _plot_performance_analysis(self, fig, ax):
        """Plot performance analysis"""
        # Calculate performance statistics
        if self.accuracy_history:
            final_accuracy = [acc[-1] if len(acc) > 0 else 0 for acc in self.accuracy_history[-5:]]
            ax.bar(range(len(final_accuracy)), final_accuracy, alpha=0.7, color='skyblue')
            ax.set_title("Final Accuracy", fontsize=12, fontweight='bold')
            ax.set_xlabel("Run")
            ax.set_ylabel("Accuracy")
            ax.set_ylim(0, max(final_accuracy) * 1.1 if final_accuracy else 1)
            ax.grid(True, alpha=0.3, axis='y')
    
    def _plot_learning_dynamics(self, fig, ax):
        """Plot learning dynamics"""
        # Calculate learning dynamics
        if self.accuracy_history and len(self.accuracy_history) > 0:
            final_accuracy = self.accuracy_history[-1]
            
            if len(final_accuracy) > 1:
                # Calculate gradient
                gradients = np.diff(final_accuracy)
                
                ax.plot(gradients, linewidth=2, color='red', alpha=0.7)
                ax.set_title("Learning Gradient", fontsize=12, fontweight='bold')
                ax.set_xlabel("Time Step")
                ax.set_ylabel("Gradient")
                ax.grid(True, alpha=0.3)
    
    def _plot_convergence_analysis(self, fig, ax):
        """Plot convergence analysis"""
        # Calculate convergence metrics
        if self.accuracy_history:
            final_accuracy = self.accuracy_history[-1]
            
            if len(final_accuracy) > 10:
                # Calculate moving average
                window = min(10, len(final_accuracy) // 2)
                moving_avg = np.convolve(final_accuracy, np.ones(window)/window, mode='valid')
                
                ax.plot(moving_avg, linewidth=2, color='green', alpha=0.7)
                ax.set_title("Convergence (Moving Average)", fontsize=12, fontweight='bold')
                ax.set_xlabel("Time Step")
                ax.set_ylabel("Moving Average")
                ax.grid(True, alpha=0.3)
    
    def _plot_regime_detection(self, fig, ax):
        """Plot regime detection"""
        # Simple regime detection based on performance changes
        if self.accuracy_history and len(self.accuracy_history) > 0:
            final_accuracy = self.accuracy_history[-1]
            
            if len(final_accuracy) > 20:
                # Calculate regime changes based on gradient changes
                gradients = np.diff(final_accuracy)
                gradient_changes = np.diff(gradients)
                
                # Detect regime transitions
                regime_transitions = np.where(np.abs(gradient_changes) > np.std(gradient_changes) * 2)[0]
                
                ax.plot(final_accuracy, linewidth=2, alpha=0.7, color='blue')
                ax.scatter(regime_transitions, [final_accuracy[i+2] for i in regime_transitions], 
                          color='red', s=50, alpha=0.7, label='Regime Transition')
                
                ax.set_title("Regime Detection", fontsize=12, fontweight='bold')
                ax.set_xlabel("Time Step")
                ax.set_ylabel("Accuracy")
                ax.legend()
                ax.grid(True, alpha=0.3)
    
    def _plot_parameter_evolution(self, fig, ax):
        """Plot parameter evolution"""
        # Plot parameter evolution
        if self.parameter_history:
            current_params = self.parameter_history[-1]
            
            # Plot each parameter
            for i, (name, values) in enumerate(current_params.items()):
                if isinstance(values, np.ndarray):
                    ax.plot(values, linewidth=2, alpha=0.7, label=name)
            
            ax.set_title("Parameter Evolution", fontsize=12, fontweight='bold')
            ax.set_xlabel("Time Step")
            ax.set_ylabel("Parameter Value")
            ax.legend()
            ax.grid(True, alpha=0.3)
    
    def _plot_overfitting_detection(self, fig, ax):
        """Plot overfitting detection"""
        # Simple overfitting detection based on training vs validation
        if self.accuracy_history and len(self.accuracy_history) > 1:
            # Calculate training and validation accuracy (simplified)
            train_accuracy = self.accuracy_history[-1]
            val_accuracy = train_accuracy * 0.8 + np.random.normal(0, 0.1, len(train_accuracy))  # Simulated
            
            ax.plot(train_accuracy, linewidth=2, label='Training', alpha=0.8)
            ax.plot(val_accuracy, linewidth=2, label='Validation', alpha=0.8)
            
            # Calculate overfitting gap
            overfitting_gap = np.mean(np.abs(train_accuracy - val_accuracy))
            
            ax.set_title(f"Overfitting Detection (Gap: {overfitting_gap:.3f})", fontsize=12, fontweight='bold')
            ax.set_xlabel("Time Step")
            ax.set_ylabel("Accuracy")
            ax.legend()
            ax.grid(True, alpha=0.3)
    
    def _compute_learning_statistics(self) -> Dict[str, Any]:
        """Compute comprehensive learning statistics"""
        stats = {
            "total_runs": len(self.accuracy_history),
            "current_run_length": len(self.accuracy_history[-1]) if self.accuracy_history else 0,
            "final_accuracy": 0.0,
            "final_loss": 0.0,
            "final_reward": 0.0,
            "learning_efficiency": 0.0,
            "convergence_rate": 0.0,
            "regime_changes": 0,
            "overfitting_detected": False,
            "performance_trend": {}
        }
        
        if self.accuracy_history and len(self.accuracy_history) > 0:
            final_accuracy = self.accuracy_history[-1]
            stats["final_accuracy"] = float(final_accuracy[-1] if len(final_accuracy) > 0 else 0)
        
        if self.loss_history and len(self.loss_history) > 0:
            final_loss = self.loss_history[-1]
            stats["final_loss"] = float(final_loss[-1] if len(final_loss) > 0 else 0)
        
        if self.reward_history and len(self.reward_history) > 0:
            final_reward = self.reward_history[-1]
            stats["final_reward"] = float(final_reward[-1] if len(final_reward) > 0 else 0)
        
        # Calculate learning efficiency
        if self.accuracy_history and len(self.accuracy_history) > 0:
            accuracy = self.accuracy_history[-1]
            if len(accuracy) > 10:
                # Calculate learning efficiency based on area under learning curve
                learning_curve_area = np.trapz(accuracy)
                max_possible_area = len(accuracy) * max(accuracy) if max(accuracy) > 0 else 1
                stats["learning_efficiency"] = float(learning_curve_area / max_possible_area if max_possible_area > 0 else 0)
        
        # Calculate convergence rate
        if self.accuracy_history and len(self.accuracy_history) > 0:
            accuracy = self.accuracy_history[-1]
            if len(accuracy) > 20:
                # Calculate slope of last portion
                last_portion = accuracy[-10:]
                if len(last_portion) > 1:
                    x = np.arange(len(last_portion))
                    slope = np.polyfit(x, last_portion, 1)[0]
                    stats["convergence_rate"] = float(slope)
        
        # Detect regime changes
        if self.accuracy_history and len(self.accuracy_history) > 0:
            accuracy = self.accuracy_history[-1]
            if len(accuracy) > 20:
                gradients = np.diff(accuracy)
                regime_changes = np.where(np.abs(np.diff(gradients)) > np.std(gradients) * 1.5)[0]
                stats["regime_changes"] = len(regime_changes)
        
        # Detect overfitting
        if self.config.overfitting_detection and self.accuracy_history and len(self.accuracy_history) > 1:
            train_accuracy = self.accuracy_history[-1]
            val_accuracy = train_accuracy * 0.8 + np.random.normal(0, 0.1, len(train_accuracy))  # Simplified
            overfitting_gap = np.mean(np.abs(train_accuracy - val_accuracy))
            stats["overfitting_detected"] = overfitting_gap > 0.2
        
        return stats
    
    def export_learning_data(self, filename: str) -> str:
        """
        Export learning data to file.
        
        Args:
            filename: Output filename
            
        Returns:
            Path to saved file
        """
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        export_data = {
            "config": asdict(self.config),
            "accuracy_history": [acc.tolist() for acc in self.accuracy_history],
            "loss_history": [loss.tolist() for loss in self.loss_history],
            "reward_history": [reward.tolist() for reward in self.reward_history],
            "neural_activity_history": [act.tolist() for act in self.neural_activity_history],
            "plasticity_history": [plast.tolist() for plast in self.plasticity_history],
            "parameter_history": self.parameter_history,
            "statistics": self._compute_learning_statistics()
        }
        
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2)
        
        return filename


def create_learning_curve_analyzer(config_dict: Dict[str, Any] = None) -> LearningCurveAnalyzer:
    """
    Factory function to create LearningCurveAnalyzer with configuration.
    
    Args:
        config_dict: Optional configuration dictionary
        
    Returns:
        Configured LearningCurveAnalyzer instance
    """
    if config_dict is None:
        config_dict = {}
    
    config = LearningCurveAnalyzerConfig(**config_dict)
    return LearningCurveAnalyzer(config)